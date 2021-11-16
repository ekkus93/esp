#ifndef SDCARD_H
#define SDCARD_H

#include "FS.h"

void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void createDir(fs::FS &fs, const char * path);
void removeDir(fs::FS &fs, const char * path);
void readFile(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void renameFile(fs::FS &fs, const char * path1, const char * path2);
void deleteFile(fs::FS &fs, const char * path);
void testFileIO(fs::FS &fs, const char * path);
void sdcard_setup();

class SDFile 
{
  public:
    const char *path;
    File file;
    bool openFlag = false;
    
    SDFile(const char *_path);
    virtual bool open(fs::FS &fs);
    bool close();
};

class SDReadFile: public SDFile 
{    
  public:
    SDReadFile(const char *_path);
    bool open(fs::FS &fs);
    size_t read(uint8_t *buf, size_t size);
    uint8_t *clearBuffer(uint8_t *buf, int size);
};

class SDWriteFile: public SDFile
{   
  public: 
    SDWriteFile(const char *_path);
    bool open(fs::FS &fs);
    size_t write(const uint8_t *buf, size_t size);
};

#endif
