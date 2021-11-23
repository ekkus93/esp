#ifndef CAMERA_H
#define CAMERA_H

#include "esp_err.h"
#include "esp_camera.h"

void outputCameraStatus();
esp_err_t write_file(const char *path, const uint8_t *buf, size_t buflen);
esp_err_t _takePic(const char* path);
void printBufferHead(const uint8_t *buf, size_t buflen);
bool isJpgBuf(const uint8_t *buf, size_t buflen);

class Camera
{
    public:
        Camera();
        esp_err_t takePic(const char *path);
};

#endif