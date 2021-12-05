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

bool dirList(const char *dirname, bool (*cb)(const char *filename, bool isDir, size_t fileSize))
{
  Serial.printf("Listing directory: %s\n", dirname);

  File root = SD_MMC.open(dirname);
  if (!root)
  {
    Serial.println("Failed to open directory");
    return false;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return false;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      cb(file.name(), true, 0);
    }
    else
    {
      cb(file.name(), false, file.size());
    }
    file.close();
    file = root.openNextFile();
  }

  return true;
}

void fileWrite(const char *path, const uint8_t *buf, size_t buflen)
{
  uint8_t *copyBuf = (uint8_t *)malloc(sizeof(uint8_t) * buflen);
  memcpy(copyBuf, buf, buflen * sizeof(uint8_t));
  File file = SD_MMC.open(path, FILE_WRITE);
  file.write(copyBuf, buflen);
  free(copyBuf);
  file.close();
}

bool dirSafeCreate(String path)
{
  File dir = SD_MMC.open(path);
  if (!dir || !dir.isDirectory())
  {
    return SD_MMC.mkdir(path);
  }

  return true;
}

bool fileDelete(String path)
{
  if (SD_MMC.remove(path))
  {
    Serial.println("File deleted");
    return true;
  }
  else
  {
    Serial.println("Delete failed");
    return false;
  }
}

bool fileRename(String sourcePath, String destPath)
{
    if (SD_MMC.rename(sourcePath, destPath)) {
        Serial.println("File renamed");
        return true;
    } else {
        Serial.println("Rename failed");
        return false;
    }  
}

bool dirPrint(const char *filename, bool isDir, size_t fileSize)
{
  Serial.printf("filename: %s, isDir: %d, fileSize: %d\n", filename, isDir, fileSize);
  return true;
}