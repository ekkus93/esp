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

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }
  
    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

void sdcard_setup(){
    // Serial.begin(115200);

    if (!SD_MMC.begin("/sdcard", true)){         // https://dr-mntn.net/2021/02/using-the-sd-card-in-1-bit-mode-on-the-esp32-cam-from-ai-thinker
    // if(!SD_MMC.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD_MMC.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD_MMC card attached");
        return;
    }

    Serial.print("SD_MMC Card Type: ");
    if(cardType == CARD_MMC){  
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);

    listDir(SD_MMC, "/", 0);
    createDir(SD_MMC, "/mydir");
    listDir(SD_MMC, "/", 0);
    removeDir(SD_MMC, "/mydir");
    listDir(SD_MMC, "/", 2);
    writeFile(SD_MMC, "/hello.txt", "Hello ");
    appendFile(SD_MMC, "/hello.txt", "World!\n");
    readFile(SD_MMC, "/hello.txt");
    deleteFile(SD_MMC, "/foo.txt");
    renameFile(SD_MMC, "/hello.txt", "/foo.txt");
    readFile(SD_MMC, "/foo.txt");
    testFileIO(SD_MMC, "/test.txt");
    Serial.printf("Total space: %lluMB\n", SD_MMC.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD_MMC.usedBytes() / (1024 * 1024));

    // test SDFile
    const char* fileName = "/sdFile_test.txt";
    deleteFile(SD_MMC, fileName);
    SDFile *sdFile = new SDFile(fileName);
    if (!sdFile->openWrite(SD_MMC)) {
      Serial.printf("openWrite failed");
    }
    if(!sdFile->file){
        Serial.println("Failed to open file for writing");
        return;
    }

    const char* txtStr1 = "foobar";
    size_t txtStrSize = sdFile->write((const uint8_t*)txtStr1, strlen(txtStr1));

    const char* txtStr2 = "spam";
    txtStrSize = sdFile->write((const uint8_t*)txtStr2, strlen(txtStr2));
    if (!sdFile->close()) {
      Serial.printf("###sdFile close failed");
    }

    readFile(SD_MMC, sdFile->path);
    Serial.printf("\n");

    if (!sdFile->openRead(SD_MMC)) {
      Serial.printf("openRead failed");
    }

    const int readBufLen = 4;
    uint8_t *readBuf = (uint8_t *)calloc(4, sizeof(uint8_t));
    bool hasChars = true;
    while(hasChars) {
      sdFile->clearBuffer(readBuf, readBufLen);
      int numOfChars = sdFile->read(readBuf, readBufLen-1);
      if (numOfChars < 0) {
        Serial.printf("ERROR: read failed\n");
        sdFile->close();
        return;        
      }

      if (numOfChars > 0) {
        Serial.printf("%s", (char *)readBuf);
      } else {
        Serial.printf("\n");
        hasChars = false;
      }
    }
    free(readBuf);
    sdFile->close();
}

SDFile::SDFile(const char* _path) 
{
  openFlag = false;
  path = _path;
}

bool SDFile::openRead(fs::FS &fs) 
{
  if (openFlag) {
    Serial.printf("ERROR: File, %s, is already open - 1\n", path);
    return false;
  }

  file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return false;
  }  

  openFlag = true;
  fileType = 0;

  return true;
}

bool SDFile::openWrite(fs::FS &fs) 
{
  if (openFlag) {
    Serial.printf("ERROR: File, %s, is already open - 2\n", path);
    return false;
  }

  file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return false;
  }

  openFlag = true;
  fileType = 1;

  return true;
}

size_t SDFile::read(uint8_t *buf, size_t size)
{
  if (!openFlag) {
    Serial.printf("ERROR: File, %s, is not open - 2\n", path);
    return -1;
  }

  if (fileType != 0) {
    Serial.printf("ERROR: File, %s, is not open for reading", path);
    return -1;    
  }

  return file.read(buf, size);
}

size_t SDFile::write(const uint8_t *buf, size_t size) 
{
  if (!openFlag) {
    Serial.printf("ERROR: File, %s, is not open - 3\n", path);
    return -1;
  }

  if (fileType != 1) {
    Serial.printf("ERROR: File, %s, is not open for writing", path);
    return -1;    
  }

  return file.write(buf, size);
}

bool SDFile::close() {
  if (!openFlag) {
    Serial.printf("ERROR: File, %s, is not open - 1\n", path);
    return false;
  }

  file.close();
  openFlag = false;

  return true;
}

uint8_t *SDFile::clearBuffer(uint8_t *buf, int size)
{
  for(int i=0; i<size; i++) 
  {
    buf[i] = 0;
  }

  return buf;
}
