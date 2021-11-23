#include <Arduino.h>
#include <unity.h>
#include "camera.h"
#include "sdcard.h"
#include "SD_MMC.h"

Camera *camera = NULL;

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

    // Camera
    camera = new Camera();
    sensor_t *sensor = esp_camera_sensor_get();
    sensor->set_pixformat(sensor, PIXFORMAT_JPEG);
    sensor->set_framesize(sensor, FRAMESIZE_VGA);    

    outputCameraStatus();

    Serial.printf("\n");
}

void tearDown(void)
{
    // delete camera;
}

void test_takePic()
{   
    Serial.printf("##test_takePic - 1\n");
    char path[20];

    for(int i=0; i<20; i++)
    {
        /*
        Serial.printf("##test_takePic - 2\n");
        camera = new Camera();
        Serial.printf("##test_takePic - 3\n");
        sensor_t *sensor = esp_camera_sensor_get();
        Serial.printf("##test_takePic - 4\n");
        sensor->set_pixformat(sensor, PIXFORMAT_JPEG);
        Serial.printf("##test_takePic - 5\n");
        sensor->set_framesize(sensor, FRAMESIZE_VGA);    
        */

        Serial.printf("test_pic_%d\n", i);
        sprintf(path, "/test_pic_%d.jpg", i);
        outputCameraStatus();
        Serial.printf("\n\n");

        // TEST_ASSERT_NOT_EQUAL(ESP_FAIL, camera->takePic(path));
        TEST_ASSERT_NOT_EQUAL(ESP_FAIL, _takePic(path));
        delete camera;
        delay(5000);
    }
}

void loop()
{
    UNITY_BEGIN();
    RUN_TEST(test_takePic);
    UNITY_END(); // stop unit testing
}