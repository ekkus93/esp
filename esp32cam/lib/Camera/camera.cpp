#include "camera.h"
#include "esp_timer.h"
#include "esp_camera.h"
#include "img_converters.h"
#include "camera_index.h"
#include "sdcard.h"
#include "SD_MMC.h"

#include "fb_gfx.h"
#include "fd_forward.h"
#include "fr_forward.h"
#include "HardwareSerial.h"

//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//

// Select camera model
// #define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM

#include "camera_pins.h"

void outputCameraStatus()
{
    sensor_t *s = esp_camera_sensor_get();
    Serial.printf("framesize: %u\n", s->status.framesize);
    Serial.printf("quality: %u\n", s->status.quality);
    Serial.printf("brightness: %d\n", s->status.brightness);
    Serial.printf("contrast: %d\n", s->status.contrast);
    Serial.printf("saturation: %d\n", s->status.saturation);
    Serial.printf("sharpness: %d\n", s->status.sharpness);
    Serial.printf("special_effect: %u\n", s->status.special_effect);
    Serial.printf("wb_mode: %u\n", s->status.wb_mode);
    Serial.printf("awb: %u\n", s->status.awb);
    Serial.printf("awb_gain: %u\n", s->status.awb_gain);
    Serial.printf("aec: %u\n", s->status.aec);
    Serial.printf("aec2: %u\n", s->status.aec2);
    Serial.printf("ae_level: %d\n", s->status.ae_level);
    Serial.printf("aec_value: %u\n", s->status.aec_value);
    Serial.printf("agc: %u\n", s->status.agc);
    Serial.printf("agc_gain: %u\n", s->status.agc_gain);
    Serial.printf("gainceiling: %u\n", s->status.gainceiling);
    Serial.printf("bpc: %u\n", s->status.bpc);
    Serial.printf("wpc: %u\n", s->status.wpc);
    Serial.printf("raw_gma: %u\n", s->status.raw_gma);
    Serial.printf("lenc: %u\n", s->status.lenc);
    Serial.printf("vflip: %u\n", s->status.vflip);
    Serial.printf("hmirror: %u\n", s->status.hmirror);
    Serial.printf("dcw: %u\n", s->status.dcw);
    Serial.printf("colorbar: %u\n", s->status.colorbar);
}

Camera::Camera()
{
    camera_config_t config;

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    // config.xclk_freq_hz = 16500000; 
    config.jpeg_quality = 10;
    config.fb_count = 1;

    // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
    //                      for larger pre-allocated frame buffer.
    if (psramFound())
    {
        Serial.printf("###psram was found\n");
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
    }
    else
    {
        Serial.printf("###psram was not found\n");
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

#if defined(CAMERA_MODEL_ESP_EYE)
    pinMode(13, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
#endif

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

    sensor_t *s = esp_camera_sensor_get();
    // initial sensors are flipped vertically and colors are a bit saturated
    if (s->id.PID == OV3660_PID)
    {
        s->set_vflip(s, 1);       // flip it back
        s->set_brightness(s, 1);  // up the brightness just a bit
        s->set_saturation(s, -2); // lower the saturation
    }
    // drop down frame size for higher initial frame rate
    s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
#endif
}

bool isJpgBuf(const uint8_t *buf, size_t buflen)
{
    if (buflen < 4) 
    {
        return false;
    }

    const uint8_t jpegHeader[4] = {0xff, 0xd8, 0xff, 0xe0};

    for(int i=0; i<4; i++)
    {
        if (buf[i] != jpegHeader[i])
        {
            return false;
        }
    }

    return true;
}

esp_err_t _takePic(const char *path)
{
    camera_fb_t *fb = NULL;
    int64_t fr_start = esp_timer_get_time();

    outputCameraStatus();

    fb = esp_camera_fb_get();
    if (!fb)
    {
        Serial.println("_takePic failed\n");
        return ESP_FAIL;
    }
    else
    {
        if (fb->width > 400)
        {
            size_t fb_len = 0;
            if (fb->format == PIXFORMAT_JPEG)
            {
                fb_len = fb->len;

                while(!isJpgBuf(fb->buf, fb->len))
                {
                    Serial.printf("not jpg\n");
                    esp_camera_fb_return(fb);
                    delay(100);
                    fb = esp_camera_fb_get();
                }
                size_t buflen = fb->len;
                uint8_t *buf = (uint8_t *)malloc(sizeof(uint8_t)*buflen);
                for(int i=0; i<buflen; i++)
                {
                    buf[i] = fb->buf[i];
                }
                printBufferHead(buf, buflen);
                write_file(path, buf, buflen);
                free(buf);
            }
            else
            {
                Serial.printf("not jpeg\n");
            }
            esp_camera_fb_return(fb);
            int64_t fr_end = esp_timer_get_time();
            Serial.printf("JPG: %uB %ums\n", (uint32_t)(fb_len), (uint32_t)((fr_end - fr_start) / 1000));
        }
    }

    return ESP_OK;
}

/*
static esp_err_t write_file(const char *path, camera_fb_t *fb)
{
    SDWriteFile *sdWriteFile = new SDWriteFile(&SD_MMC, path);
    if (!sdWriteFile->open())
    {
        Serial.printf("openWrite failed\n");
        return ESP_FAIL;
    }
    if (!sdWriteFile->file)
    {
        Serial.println("Failed to open file for writing\n");
        return ESP_FAIL;
    }

    if (sdWriteFile->write(fb->buf, fb->len))
    {
        Serial.printf("###takePic - 6\n");
        sdWriteFile->flush();
        return (sdWriteFile->close() ? ESP_OK : ESP_FAIL);
    }
    else
    {
        Serial.printf("Writing to file failed\n");
        return ESP_FAIL;
    }
}
*/

void printBufferHead(const uint8_t *buf, size_t buflen)
{
    for (int i = 0; i < 10 && i<buflen; i++)
    {
        if (i != 0)
        {
            Serial.printf(", ");
        }
        Serial.printf("%x", (int)buf[i]);
    }
    Serial.printf("\n");
}

esp_err_t write_file(const char *path, const uint8_t *buf, size_t buflen)
{
    File file = SD_MMC.open(path, FILE_WRITE);
    file.write(buf, buflen);

    printBufferHead(buf, buflen);

    file.close();

    SDReadFile *rf = new SDReadFile(&SD_MMC, path);
    if (!rf->open())
    {
        Serial.printf("read file failed\n");
        return ESP_FAIL;
    }

    uint8_t *readBuf = (uint8_t *)calloc(buflen, sizeof(uint8_t));
    int numOfChars = rf->read(readBuf, buflen);
    if (!rf->close())
    {
        Serial.printf("close read file failed\n");
        free(readBuf);
        return ESP_FAIL;
    }
    if (numOfChars != buflen)
    {
        Serial.printf("numOfChars don't match - actual: %d, expected: %d\n", numOfChars, buflen);
        free(readBuf);
        return ESP_FAIL;
    }
    if (numOfChars < 4)
    {
        Serial.printf("numOfChars less than 4 - %d\n", numOfChars);
        free(readBuf);
        return ESP_FAIL;
    }       

    for (int i = 0; i < buflen; i++)
    {
        if (buf[i] != readBuf[i])
        {
            Serial.printf("byte[%d] doesn't match\n", i);
            free(readBuf);
            return ESP_FAIL;
        }
    }

    free(readBuf);
    return ESP_OK;
}

esp_err_t Camera::takePic(const char *path)
{
    /*
    Serial.printf("###takePic - 1\n");
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
    int64_t fr_start = esp_timer_get_time();

    Serial.printf("###takePic - 2\n");
    // get camera frame buffer
    fb = esp_camera_fb_get();
    if (!fb)
    {
        Serial.println("Camera capture failed");
        return ESP_FAIL;
    }

    Serial.printf("###takePic - 3 - width: %d\n", fb->width);
    if (fb->width > 400)
    {
        Serial.printf("###takePic - 4\n");
        size_t fb_len = 0;

        if (fb->format == PIXFORMAT_JPEG)
        {
            Serial.printf("###takePic - 5\n");
            fb_len = fb->len;
            Serial.printf("###fb_len: %d\n", fb_len);
            res = write_file(path, fb);
        }
        else
        {
            Serial.printf("###takePic - 8\n");
            // Don't handle this case for now
            res = ESP_FAIL;
        }
        Serial.printf("###takePic - 9\n");

        esp_camera_fb_return(fb);
        int64_t fr_end = esp_timer_get_time();
        Serial.printf("JPG: %uB %ums\n", (uint32_t)(fb_len), (uint32_t)((fr_end - fr_start) / 1000));
        Serial.printf("###takePic - 10\n");
        return res;
    }

    return ESP_FAIL;
    */

    _takePic(path);
    return ESP_OK;
}