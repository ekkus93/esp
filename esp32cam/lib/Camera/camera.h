#ifndef CAMERA_H
#define CAMERA_H

#include "esp_err.h"
#include "esp_camera.h"

class Camera
{
    public:
        camera_config_t config;
        Camera();
        esp_err_t takePic(bool (*cb)(const uint8_t *cbBuf, size_t cbSize));
};

#endif