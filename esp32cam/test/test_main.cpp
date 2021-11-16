#include <Arduino.h>
#include <unity.h>
#include "sdcard.h"
#include "SD_MMC.h"

const char *fileName = "/sdFile_test.txt";
char *actualStr = NULL; 

void setup(void)
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    if (!SD_MMC.begin("/sdcard", true))
    { // https://dr-mntn.net/2021/02/using-the-sd-card-in-1-bit-mode-on-the-esp32-cam-from-ai-thinker
        // if(!SD_MMC.begin()){
        Serial.println("Card Mount Failed");
        return;
    }

    deleteFile(SD_MMC, fileName);
}

void tearDown(void)
{
    if (actualStr != NULL) {
        free(actualStr);
        actualStr = NULL;
    }

    deleteFile(SD_MMC, fileName);
}

char * concatData(char *str1, const char *str2, int str2Len) {
    char *retStr = NULL;

    if (str1 == NULL) {
        retStr = (char *)calloc(str2Len+1, sizeof(char));
        strcpy(retStr, str2);
        return retStr;
    }

    int newLen = strlen(str1) + str2Len + 1;
 
    str1 = (char *)realloc(str1, newLen * sizeof(char));
    strncat(str1, str2, str2Len);
    str1[newLen-1] = (char)0;
    return str1;
}

void test_readWrite(void)
{
    SDWriteFile *sdWriteFile = new SDWriteFile(fileName);
    if (!sdWriteFile->open(SD_MMC))
    {
        Serial.printf("openWrite failed");
    }
    if (!sdWriteFile->file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }

    const char *txtStr1 = "foobar";
    size_t txtStrSize = sdWriteFile->write((const uint8_t *)txtStr1, strlen(txtStr1));

    const char *txtStr2 = "spam";
    txtStrSize = sdWriteFile->write((const uint8_t *)txtStr2, strlen(txtStr2));
    if (!sdWriteFile->close())
    {
        Serial.printf("###sdFile close failed");
    }

    readFile(SD_MMC, sdWriteFile->path);
    Serial.printf("\n");

    SDReadFile *sdReadFile = new SDReadFile(fileName);
    if (!sdReadFile->open(SD_MMC))
    {
        Serial.printf("openRead failed");
    }

    const int readBufLen = 4;
    uint8_t *readBuf = (uint8_t *)calloc(4, sizeof(uint8_t));
    
    bool hasChars = true;
    while (hasChars)
    {
        sdReadFile->clearBuffer(readBuf, readBufLen);
        int numOfChars = sdReadFile->read(readBuf, readBufLen - 1);
        if (numOfChars < 0)
        {
            Serial.printf("ERROR: read failed\n");
            sdReadFile->close();
            return;
        }

        if (numOfChars > 0)
        {
            char *readBufStr = (char *)readBuf;
            actualStr = concatData(actualStr, readBufStr, numOfChars);
            Serial.printf("%s", readBufStr);
        }
        else
        {
            Serial.printf("\n");
            hasChars = false;
        }
    }
    free(readBuf);
    sdReadFile->close();

    TEST_ASSERT_EQUAL(strcmp(actualStr, "foobarspam"), 0);
}

void loop() 
{
    UNITY_BEGIN();
    RUN_TEST(test_readWrite);
    UNITY_END(); // stop unit testing
}