
#include "SD_MMC.h"
#include "sdcard.h"

esp_err_t sdcardInit()
{
    if (!SD_MMC.begin("/sdcard", true))
    { // https://dr-mntn.net/2021/02/using-the-sd-card-in-1-bit-mode-on-the-esp32-cam-from-ai-thinker
        // if(!SD_MMC.begin()){
        Serial.println("Card Mount Failed\n");
        return ESP_FAIL;
    }

    return ESP_OK;
}

void write_file(const char *path, const uint8_t *buf, size_t buflen)
{
  uint8_t *copyBuf = (uint8_t *)malloc(sizeof(uint8_t) * buflen);
  memcpy(copyBuf, buf, buflen * sizeof(uint8_t));
  File file = SD_MMC.open(path, FILE_WRITE);
  file.write(copyBuf, buflen);
  free(copyBuf);
  file.close();
}