/*
 * Connect the SD card to the following pins:
 *
 * SD Card | ESP32
 *    D2       12
 *    D3       13
 *    CMD      15
 *    VSS      GND
 *    VDD      3.3V
 *    CLK      14
 *    VSS      GND
 *    D0       2  (add 1K pull up after flashing)
 *    D1       4
 */

#include "FS.h"
#include "SD_MMC.h"
#include "sdcard.h"

// ***SDDir***
SDDir::SDDir(fs::FS *filestream) {
  this->filestream = filestream;
}

bool SDDir::listDir(const char *dirname, bool (*cb)(const char *filename, bool isDir, size_t fileSize))
{
  Serial.printf("Listing directory: %s\n", dirname);

  File root = filestream->open(dirname);
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

bool SDDir::createDir(const char *path)
{
  Serial.printf("Creating Dir: %s\n", path);
  if (filestream->mkdir(path))
  {
    Serial.println("Dir created");
    return true;
  }
  else
  {
    Serial.println("mkdir failed");
    return false;
  }
}

bool SDDir::removeDir(const char *path)
{
  Serial.printf("Removing Dir: %s\n", path);
  if (filestream->rmdir(path))
  {
    Serial.println("Dir removed");
    return true;
  }
  else
  {
    Serial.println("rmdir failed");
    return false;
  }
}

// ***SDFile***
SDFile::SDFile(fs::FS *filestream, const char *path)
{
  this->filestream = filestream;
  this->openFlag = false;
  this->path = path;
}

bool SDFile::open()
{
  Serial.printf("ERROR: not implemented");
  return false;
}

bool SDFile::close()
{
  if (!openFlag)
  {
    Serial.printf("ERROR: File, %s, is not open - 1\n", path);
    return false;
  }

  file.close();
  openFlag = false;

  return true;
}

bool SDFile::deleteFile()
{
  if (openFlag)
  {
    Serial.printf("Delete failed. Close file before renaming.");
    return false;
  }

  Serial.printf("Deleting file: %s\n", path);
  if (filestream->remove(path))
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

bool SDFile::renameFile(const char *newPath)
{
  if (openFlag)
  {
    Serial.printf("Rename failed. Close file before renaming.");
    return false;
  }

  Serial.printf("Renaming file %s to %s\n", path, newPath);
  if (filestream->rename(path, newPath))
  {
    Serial.println("File renamed");
    path = newPath;
    return true;
  }
  else
  {
    Serial.println("Rename failed");
    return false;
  }
}

SDFile::~SDFile()
{
  if (openFlag)
  {
    if (!close())
    {
      Serial.printf("Closing file, %s, failed\n", path);
    }
  }
}

// ***SDReadFile***
SDReadFile::SDReadFile(fs::FS *filestream, const char *path): SDFile(filestream, path)
{
}

bool SDReadFile::open()
{
  if (openFlag)
  {
    Serial.printf("ERROR: File, %s, is already open - 1\n", path);
    return false;
  }

  file = filestream->open(path);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return false;
  }

  openFlag = true;

  return true;
}

size_t SDReadFile::read(uint8_t *buf, size_t size)
{
  if (!openFlag)
  {
    Serial.printf("ERROR: File, %s, is not open - 2\n", path);
    return -1;
  }

  return file.read(buf, size);
}

bool SDReadFile::readAll(size_t size, bool (*cb)(const uint8_t *cbBuf, size_t cbSize)) 
{
  uint8_t *buf = (uint8_t *)calloc(size, sizeof(uint8_t));
  size_t sizeRead = read(buf, size);
  if (sizeRead == -1) {
    free(buf);
    return false;
  }

  while(sizeRead > 0) {
    if (!cb(buf, sizeRead)) {
      free(buf);
      return true;
    }

    sizeRead = read(buf, size);
  }

  free(buf);
  return (sizeRead != -1);
}

uint8_t *SDReadFile::clearBuffer(uint8_t *buf, size_t size)
{
  for (int i = 0; i < size; i++)
  {
    buf[i] = 0;
  }

  return buf;
}

// ***SDWriteFile***
SDWriteFile::SDWriteFile(fs::FS *filestream, const char *path) : SDFile(filestream, path)
{
}

bool SDWriteFile::open()
{
  if (openFlag)
  {
    Serial.printf("ERROR: File, %s, is already open - 2\n", path);
    return false;
  }

  file = filestream->open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return false;
  }

  openFlag = true;

  return true;
}

size_t SDWriteFile::write(const uint8_t *buf, size_t size)
{
  if (!openFlag)
  {
    Serial.printf("ERROR: File, %s, is not open - 3\n", path);
    return -1;
  }

  int wsize = file.write(buf, size);
  file.flush();

  return wsize;
}

void SDWriteFile::flush()
{
  if (!openFlag)
  {
    file.flush();
  }
}
