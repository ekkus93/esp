#include <Arduino.h>
#include <unity.h>
#include "esp_camera.h"
#include "SD_MMC.h"

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

void setup(void)
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    if (!SD_MMC.begin("/sdcard", true))
    { // https://dr-mntn.net/2021/02/using-the-sd-card-in-1-bit-mode-on-the-esp32-cam-from-ai-thinker
        // if(!SD_MMC.begin()){
        Serial.println("Card Mount Failed\n");
        return;
    }
}

void tearDown(void)
{
}

esp_err_t write_file(const char *path, const uint8_t *buf, size_t buflen)
{
    File file = SD_MMC.open(path, FILE_WRITE);
    file.write(buf, buflen);

    file.close();
    return ESP_OK;
};

esp_err_t takePic()
{
    Serial.printf("###takePic - 1\n");
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

    Serial.printf("###takePic - 2\n");
    // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
    //                      for larger pre-allocated frame buffer.
    if (psramFound())
    {
        Serial.printf("###takePic - 3\n");
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
    }
    else
    {
        Serial.printf("###takePic - 4\n");
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }
    Serial.printf("###takePic - 5\n");

#if defined(CAMERA_MODEL_ESP_EYE)
    pinMode(13, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
#endif

    Serial.printf("###takePic - 6\n");
    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
        return ESP_FAIL;
    }

    Serial.printf("###takePic - 7\n");
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

    Serial.printf("###takePic - 8\n");
#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
#endif

    Serial.printf("###takePic - 9\n");
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    int64_t fr_start = esp_timer_get_time();

    Serial.printf("###takePic - 10\n");
    fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        return ESP_FAIL;
    }

    Serial.printf("###takePic - 11\n");
    if(fb->width > 400){
        Serial.printf("###takePic - 12\n");
        size_t fb_len = 0;
        if(fb->format == PIXFORMAT_JPEG){
            Serial.printf("###takePic - 13\n");
            fb_len = fb->len;

            write_file("/test_cam.jpg", fb->buf, fb->len);
            res = ESP_OK;
        } else {
            Serial.printf("###takePic - 14\n");
            /*
            jpg_chunking_t jchunk = {req, 0};
            res = frame2jpg_cb(fb, 80, jpg_encode_stream, &jchunk)?ESP_OK:ESP_FAIL;
            httpd_resp_send_chunk(req, NULL, 0);
            fb_len = jchunk.len;
            */
           Serial.printf("Invalid format\n");
           res = ESP_FAIL;
        }
        Serial.printf("###takePic - 15\n");
        esp_camera_fb_return(fb);
        int64_t fr_end = esp_timer_get_time();
        Serial.printf("JPG: %uB %ums\n", (uint32_t)(fb_len), (uint32_t)((fr_end - fr_start)/1000));
        return res;
    }

    Serial.printf("Invalid width\n");
    return ESP_FAIL;
}

void test_takePic()
{
    TEST_ASSERT_EQUAL(ESP_OK, takePic());
}

void loop()
{
    UNITY_BEGIN();
    RUN_TEST(test_takePic);
    UNITY_END(); // stop unit testing
}