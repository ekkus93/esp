#ifndef SDCARD_H
#define SDCARD_H

esp_err_t sdcardInit();
bool dirList(const char *dirname, bool (*cb)(const char *filename, bool isDir, size_t fileSize));
void fileWrite(const char *path, const uint8_t *buf, size_t buflen);
bool dirSafeCreate(String path);
bool fileDelete(String path);
bool fileRename(String sourcePath, String destPath);
bool dirPrint(const char *filename, bool isDir, size_t fileSize);

#endif