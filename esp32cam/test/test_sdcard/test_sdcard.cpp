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

void SafeStr::copyBuf(uint8_t *dest, const uint8_t *source, size_t sourceLen)
{
    for (int i = 0; i < sourceLen; i++)
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
    if (buf == NULL)
    {
        buf = (uint8_t *)malloc(dataLen * sizeof(uint8_t));
        this->copyBuf(buf, data, dataLen);
        bufLen = dataLen;
        return;
    }

    buf = (uint8_t *)realloc(buf, bufLen + dataLen);
    this->copyBuf(&(buf[bufLen]), data, dataLen);
    bufLen += dataLen;
}

char *SafeStr::toStr()
{
    char *retStr = (char *)calloc(bufLen + 1, sizeof(char));
    for (int i = 0; i < bufLen; i++)
    {
        retStr[i] = buf[i];
    }

    return retStr;
}

SafeStr::~SafeStr()
{
    if (buf != NULL)
    {
        free(buf);
        buf = NULL;
    }
}

SafeStr *actualData = NULL;

char filenames[3][20] = {
    "/hello.txt",
    "/foo.txt",
    "/test.txt"};

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

    for (int i = 0; i < 3; i++)
    {
        SDFile *sdFile = new SDFile(&SD_MMC, filenames[i]);
        sdFile->deleteFile();
    }

    SDDir *sdDir = new SDDir(&SD_MMC);
    sdDir->removeDir("/mydir");
}

void tearDown(void)
{
    if (actualData != NULL)
    {
        delete actualData;
        actualData = NULL;
    }

    SDFile *sdFile = new SDFile(&SD_MMC, fileName);
    sdFile->deleteFile();
}

void writeTestFile()
{
    SDFile *sdFile = new SDFile(&SD_MMC, fileName);
    sdFile->deleteFile();

    SDWriteFile *sdWriteFile = new SDWriteFile(&SD_MMC, fileName);
    if (!sdWriteFile->open())
    {
        Serial.printf("openWrite failed\n");
    }
    if (!sdWriteFile->file)
    {
        Serial.println("Failed to open file for writing\n");
        return;
    }

    const char *txtStr1 = "foobar";
    size_t txtStrSize = sdWriteFile->write((const uint8_t *)txtStr1, strlen(txtStr1));

    const char *txtStr2 = "spam";
    txtStrSize = sdWriteFile->write((const uint8_t *)txtStr2, strlen(txtStr2));
    if (!sdWriteFile->close())
    {
        Serial.printf("sdFile close failed\n");
    }
}

void test_writeRead(void)
{
    writeTestFile();

    actualData = new SafeStr();
    SDReadFile *sdReadFile = new SDReadFile(&SD_MMC, fileName);
    if (!sdReadFile->open())
    {
        Serial.printf("openRead failed\n");
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
            sdReadFile->close();
            return;
        }

        if (numOfChars > 0)
        {
            actualData->append(readBuf, numOfChars);
        }
        else
        {
            Serial.printf("\n\n");
            hasChars = false;
        }
    }
    free(readBuf);
    sdReadFile->close();

    char *actualStr = actualData->toStr();

    int result = strcmp(actualStr, "foobarspam");
    free(actualStr);
    delete actualData;
    actualData = NULL;
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
    if (!sdReadFile->open())
    {
        Serial.printf("openRead failed\n");
    }

    const int readBufLen = 4;

    bool result = sdReadFile->readAll(readBufLen, processBuffer);
    TEST_ASSERT_EQUAL(true, result);

    sdReadFile->close();

    char *actualStr = actualData->toStr();
    Serial.printf("actualStr: %s\n", actualStr);


    TEST_ASSERT_EQUAL(0, strcmp(actualStr, "foobarspam"));
    free(actualStr);
    delete actualData;
    actualData = NULL;
}

int fileDirCnt = 0;
int fileCnt = 0;
int dirCnt = 0;
int helloCnt = 0;
int fooCnt = 0;
bool processListDir(const char *filename, bool isDir, size_t fileSize)
{
    Serial.printf("filename: %s, filetype: %d, filesize: %d\n", filename, isDir, fileSize);
    fileDirCnt++;
    if (isDir)
    {
        dirCnt++;
    }
    else
    {
        fileCnt++;
    }

    if (strcmp(filename, "/hello.txt") == 0)
    {
        helloCnt = 1;
    }

    if (strcmp(filename, "/foo.txt") == 0)
    {
        fooCnt = 1;
    }
    return true;
}

bool printBuf(const uint8_t *cbBuf, size_t cbSize)
{
    char *bufStr = (char *)calloc(cbSize + 1, sizeof(char));
    for (int i = 0; i < cbSize; i++)
    {
        bufStr[i] = (char)cbBuf[i];
    }
    Serial.printf("%s|", bufStr);
    free(bufStr);

    return true;
}

void test_sddir()
{
    uint8_t cardType = SD_MMC.cardType();
    TEST_ASSERT_NOT_EQUAL(CARD_NONE, cardType);

    Serial.print("SD_MMC Card Type: ");
    if (cardType == CARD_MMC)
    {
        Serial.println("MMC");
    }
    else if (cardType == CARD_SD)
    {
        Serial.println("SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        Serial.println("SDHC");
    }
    else
    {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);

    // no files
    Serial.printf("no files\n");
    fileDirCnt = fileCnt = dirCnt = 0;
    SDDir *sdDir = new SDDir(&SD_MMC);
    TEST_ASSERT_EQUAL(true, sdDir->listDir("/", &processListDir));
    TEST_ASSERT_EQUAL(1, fileDirCnt);
    TEST_ASSERT_EQUAL(0, fileCnt);
    TEST_ASSERT_EQUAL(1, dirCnt);

    // one directory
    Serial.printf("one directory\n");
    fileDirCnt = fileCnt = dirCnt = 0;
    TEST_ASSERT_EQUAL(true, sdDir->createDir("/mydir"));
    TEST_ASSERT_EQUAL(true, sdDir->listDir("/", &processListDir));
    TEST_ASSERT_EQUAL(2, fileDirCnt);
    TEST_ASSERT_EQUAL(0, fileCnt);
    TEST_ASSERT_EQUAL(2, dirCnt);

    // no directory
    Serial.printf("no directory\n");
    fileDirCnt = fileCnt = dirCnt = 0;
    TEST_ASSERT_EQUAL(true, sdDir->removeDir("/mydir"));
    TEST_ASSERT_EQUAL(true, sdDir->listDir("/", &processListDir));
    TEST_ASSERT_EQUAL(1, fileDirCnt);
    TEST_ASSERT_EQUAL(0, fileCnt);
    TEST_ASSERT_EQUAL(1, dirCnt);

    // hello
    Serial.printf("hello\n");
    fileDirCnt = fileCnt = dirCnt = helloCnt = 0;
    SDWriteFile *sdWriteFile = new SDWriteFile(&SD_MMC, "/hello.txt");
    TEST_ASSERT_EQUAL(true, sdWriteFile->open());
    const char *helloStr = "Hello ";
    const char *worldStr = "World!\n";
    sdWriteFile->write((const uint8_t *)helloStr, strlen(helloStr));
    sdWriteFile->write((const uint8_t *)worldStr, strlen(worldStr));
    sdWriteFile->close();
    TEST_ASSERT_EQUAL(true, sdDir->listDir("/", &processListDir));
    TEST_ASSERT_EQUAL(2, fileDirCnt);
    TEST_ASSERT_EQUAL(1, fileCnt);
    TEST_ASSERT_EQUAL(1, dirCnt);
    TEST_ASSERT_EQUAL(1, helloCnt);
    delete sdWriteFile;

    SDReadFile *sdReadFile = new SDReadFile(&SD_MMC, "/hello.txt");
    TEST_ASSERT_EQUAL(true, sdReadFile->open());
    TEST_ASSERT_EQUAL(true, sdReadFile->readAll(4, &printBuf));
    TEST_ASSERT_EQUAL(true, sdReadFile->close());
    delete sdReadFile;

    // foo
    Serial.printf("foo\n");
    SDFile *sdFile = new SDFile(&SD_MMC, "/hello.txt");
    sdFile->renameFile("/foo.txt");
    fileDirCnt = fileCnt = dirCnt = helloCnt = fooCnt = 0;
    TEST_ASSERT_EQUAL(true, sdDir->listDir("/", &processListDir));
    TEST_ASSERT_EQUAL(2, fileDirCnt);
    TEST_ASSERT_EQUAL(1, fileCnt);
    TEST_ASSERT_EQUAL(1, dirCnt);
    TEST_ASSERT_EQUAL(0, helloCnt);
    TEST_ASSERT_EQUAL(1, fooCnt);
    delete sdFile;

    sdReadFile = new SDReadFile(&SD_MMC, "/foo.txt");
    TEST_ASSERT_EQUAL(true, sdReadFile->open());
    TEST_ASSERT_EQUAL(true, sdReadFile->readAll(4, &printBuf));
    TEST_ASSERT_EQUAL(true, sdReadFile->close());
    delete sdReadFile;

    Serial.printf("Total space: %lluMB\n", SD_MMC.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD_MMC.usedBytes() / (1024 * 1024));
}

void test_FileIO()
{
    const char *path = "/test.txt";
    File file = SD_MMC.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if (file)
    {
        len = file.size();
        size_t flen = len;
        start = millis();
        while (len)
        {
            size_t toRead = len;
            if (toRead > 512)
            {
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    }
    else
    {
        Serial.println("Failed to open file for reading");
    }

    file = SD_MMC.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for (i = 0; i < 2048; i++)
    {
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

void loop()
{
    UNITY_BEGIN();
    RUN_TEST(test_writeRead);
    RUN_TEST(test_writeReadAll);
    RUN_TEST(test_sddir);
    RUN_TEST(test_FileIO);
    UNITY_END(); // stop unit testing
}