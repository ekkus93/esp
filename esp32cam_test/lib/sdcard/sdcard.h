#ifndef SDCARD_H
#define SDCARD_H

esp_err_t sdcardInit();
void write_file(const char *path, const uint8_t *buf, size_t buflen);

#endif