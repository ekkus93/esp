#include <Arduino.h>
#include <unity.h>
#include "sdcard.h"
#include "SD_MMC.h"

const char *fileName = "/sdFile_test.txt";

class SafeStr 
{
    private:
        uint8_t *buf;
        size_t bufLen;
        void copyBuf(uint8_t *dest, const uint8_t *source, size_t sourceLen);
    public:
        SafeStr();
        void append(const uint8_t *data, size_t dataLen);
        char *toStr();
        ~SafeStr();
};

void SafeStr::copyBuf(uint8_t *dest, const uint8_t *source, size_t sourceLen) {
    for(int i=0; i<sourceLen; i++) 
    {
        dest[i] = source[i];
    }
}

SafeStr::SafeStr() 
{
    buf = NULL;
    bufLen = 0;
}

void SafeStr::append(const uint8_t *data, size_t dataLen)
{
    if (buf == NULL) {
        buf = (uint8_t *)malloc(dataLen*sizeof(uint8_t));
        this->copyBuf(buf, data, dataLen);
        bufLen = dataLen;
        return;
    }

    buf = (uint8_t *)realloc(buf, bufLen+dataLen);
    this->copyBuf(&(buf[bufLen]), data, dataLen);
    bufLen += dataLen;
}

char *SafeStr::toStr() 
{
    char *retStr = (char *)calloc(bufLen+1, sizeof(char));
    for(int i=0; i<bufLen; i++) 
    {
        retStr[i] = buf[i];
    }

    return retStr;
}

SafeStr::~SafeStr()
{
    Serial.print("###SafeStr destructor - 1\n");
    if (buf != NULL) {
        free(buf);
        buf = NULL;
    }
    Serial.print("###SafeStr destructor - 2\n");
}

SafeStr *actualData = NULL;

void setup(void)
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    if (!SD_MMC.begin("/sdcard", true))
    { // https://dr-mntn.net/2021/02/using-the-sd-card-in-1-bit-mode-on-the-esp32-cam-from-ai-thinker
        // if(!SD_MMC.begin()){
        Serial.println("Card Mount Failed\n");
        return;
    }
}

void tearDown(void)
{
    if (actualData != NULL)
    {
        delete actualData;
        actualData = NULL;
    }

    deleteFile(SD_MMC, fileName);
}

void writeTestFile()
{
    deleteFile(SD_MMC, fileName);

    Serial.printf("###test_writeRead - 1\n");
    SDWriteFile *sdWriteFile = new SDWriteFile(&SD_MMC, fileName);
    Serial.printf("###test_writeRead - 2\n");
    if (!sdWriteFile->open())
    {
        Serial.printf("openWrite failed\n");
    }
    Serial.printf("###test_writeRead - 3\n");
    if (!sdWriteFile->file)
    {
        Serial.println("Failed to open file for writing\n");
        return;
    }
    Serial.printf("###test_writeRead - 4\n");

    const char *txtStr1 = "foobar";
    size_t txtStrSize = sdWriteFile->write((const uint8_t *)txtStr1, strlen(txtStr1));
    Serial.printf("###test_writeRead - 5\n");

    const char *txtStr2 = "spam";
    txtStrSize = sdWriteFile->write((const uint8_t *)txtStr2, strlen(txtStr2));
    if (!sdWriteFile->close())
    {
        Serial.printf("###sdFile close failed\n");
    }
    Serial.printf("###test_writeRead - 6\n");
}

void test_writeRead(void)
{
    writeTestFile();

    actualData = new SafeStr();
    SDReadFile *sdReadFile = new SDReadFile(&SD_MMC, fileName);
    Serial.printf("###test_writeRead - 8\n");
    if (!sdReadFile->open())
    {
        Serial.printf("openRead failed\n");
    }
    Serial.printf("###test_writeRead - 9\n");

    const int readBufLen = 4;
    uint8_t *readBuf = (uint8_t *)calloc(4, sizeof(uint8_t));

    Serial.printf("###test_writeRead - 10\n");
    bool hasChars = true;
    while (hasChars)
    {
        Serial.printf("###test_writeRead - 11\n");
        sdReadFile->clearBuffer(readBuf, readBufLen);
        int numOfChars = sdReadFile->read(readBuf, readBufLen - 1);
        if (numOfChars < 0)
        {
            Serial.printf("###test_writeRead - 12\n");
            Serial.printf("ERROR: read failed\n\n");
            sdReadFile->close();
            return;
        }
        Serial.printf("###test_writeRead - 13\n");

        if (numOfChars > 0)
        {
            Serial.printf("###test_writeRead - 14\n");
            actualData->append(readBuf, numOfChars);
        }
        else
        {
            Serial.printf("###test_writeRead - 15\n");
            Serial.printf("\n\n");
            hasChars = false;
        }
        Serial.printf("###test_writeRead - 16\n");
    }
    free(readBuf);
    sdReadFile->close();

    char *actualStr = actualData->toStr();
    Serial.printf("###test_writeRead - 17 - actualStr: %s\n", actualStr);
    
    int result = strcmp(actualStr, "foobarspam");
    free(actualStr);
    Serial.printf("###test_writeRead - 18\n");
    delete actualData;
    actualData = NULL;
    Serial.printf("###test_writeRead - 19 - result: %d\n", result);
    TEST_ASSERT_EQUAL(0, result);
}

bool processBuffer(const uint8_t *cbBuf, size_t cbSize)
{
    actualData->append(cbBuf, cbSize);

    return true;
}

void test_writeReadAll(void)
{
    writeTestFile();

    actualData = new SafeStr();
    SDReadFile *sdReadFile = new SDReadFile(&SD_MMC, fileName);
    Serial.printf("###test_writeReadAll - 1\n");
    if (!sdReadFile->open())
    {
        Serial.printf("openRead failed\n");
    }
    Serial.printf("###test_writeReadAll - 2\n");

    const int readBufLen = 4;

    Serial.printf("###test_writeReadAll - 3\n");
    bool result = sdReadFile->readAll(readBufLen, processBuffer);
    TEST_ASSERT_EQUAL(true, result);

    Serial.printf("###test_writeReadAll - 4\n");
    sdReadFile->close();

    char *actualStr = actualData->toStr();
    Serial.printf("###test_writeReadAll - 5\n");
    Serial.printf("actualStr: %s\n", actualStr);

    Serial.printf("###test_writeReadAll - 6\n");
    
    TEST_ASSERT_EQUAL(0, strcmp(actualStr, "foobarspam"));
    free(actualStr);
    Serial.printf("###test_writeReadAll - 7\n");
    delete actualData;
    actualData = NULL;
    Serial.printf("###test_writeReadAll - 8\n");
}

void loop()
{
    UNITY_BEGIN();
    RUN_TEST(test_writeRead);
    RUN_TEST(test_writeReadAll);
    UNITY_END(); // stop unit testing
}