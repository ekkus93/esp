#ifndef SDCARD_H
#define SDCARD_H

#include "FS.h"

class SDDir
{
  public:
    fs::FS *filestream = NULL;

    SDDir(fs::FS *filestream);
    bool listDir(const char *dirname, bool (*cb)(const char *filename, bool isDir, size_t fileSize));
    bool createDir(const char * path);
    bool removeDir(const char * path);    
};

class SDFile 
{
  public:
    fs::FS *filestream = NULL;
    const char *path;
    File file;
    bool openFlag = false;
    
    SDFile(fs::FS *filestream, const char *path);
    virtual bool open();
    bool close();
    bool deleteFile();
    bool renameFile(const char *newPath);
    virtual ~SDFile();
};

class SDReadFile: public SDFile 
{    
  public:
    SDReadFile(fs::FS *filestream, const char *path);
    bool open();
    size_t read(uint8_t *buf, size_t size);
    bool readAll(size_t size, bool (*cb)(const uint8_t *cbBuf, size_t cbSize));
    uint8_t *clearBuffer(uint8_t *buf, size_t size);
};

class SDWriteFile: public SDFile
{   
  public: 
    SDWriteFile(fs::FS *filestream, const char *path);
    bool open();
    size_t write(const uint8_t *buf, size_t size);
    void flush();
};

#endif
