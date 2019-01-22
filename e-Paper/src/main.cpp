#include "main.hpp"

static const byte HEADER_LENGTH = 0x10;
static const byte HEADER_EPD_LENGTH = 0x10;

static const byte HEADER_OFFSET_COMMAND = 0x00;
static const byte HEADER_OFFSET_SLOT = 0x01;
static const byte HEADER_OFFSET_AREA_X = 0x02;
static const byte HEADER_OFFSET_AREA_Y = 0x04;
static const byte HEADER_OFFSET_AREA_W = 0x06;
static const byte HEADER_OFFSET_AREA_H = 0x08;
static const byte HEADER_OFFSET_FILESIZE = 0x0A;
static const byte HEADER_OFFSET_CHUNKSIZE = 0x0E;

static const byte HEADER_VALUE_CMD_RESET = 0x00;
static const byte HEADER_VALUE_CMD_SETROI = 0x01;
static const byte HEADER_VALUE_CMD_UPLOAD = 0x02;
static const byte HEADER_VALUE_CMD_REFRESH = 0x03;
static const byte HEADER_VALUE_CMD_GETBUSY = 0x10;

static const byte IS_DISPLAY_CONNECTED = 1;

/**
 * @class Main
 * @brief Program start point with main loop
 */

//--------------------------------------------------------------

//2 = select
//4 = busy
//5 = en

// null pointer: 0xA000003A9A10131140E0090C00

/*
 * Header: 16 byte
 * 1 byte command:
 *    0x00 = Reset
 *    0x01 = set ROI
 *    0x02 = upload image
 *    0x03 = refresh display
 *    0x10 = get busy // deprecated
 * 1 byte slot: 0x00 = default
 * 2 byte area x (dividable by 8!)
 * 2 byte area y
 * 2 byte area w
 * 2 byte area h
 * 4 byte file size (in bytes)
 * 2 byte chunk size (in bytes)
 * 
 * 
 * 
 * 
 * 
 * 
 */

//--------------------------------------------------------------

uint32_t maxFileSize = 1;
uint32_t curFileSize = 0;
MAIN_STATE receiveState = MAIN_STATE ::STATE_NVS;
int sleepTime = 1000;
int dataCounter = 0;
int chunkLength = 250;
unsigned long lastTime = 0;
unsigned long lastTimeTCM = 0;
unsigned long lastTimeUpload = 0;
int tcmWaitCounter = 0;

/**
 * @brief Never called, only for Clion IDE
 */
int main()
{
    setup();
    while (true)
    {
        loop();
    };
}

/**
 * @brief Called once on start
 * 
 * Sets up the pins and initializes the SPI Handler
 */
void setup()
{
    // set up the pins
    pinMode(PIN_EN, OUTPUT);  //enable, active low
    pinMode(PIN_BUSY, INPUT); //busy, high when device is available
    pinMode(PIN_CS, OUTPUT);  //device select, active low

    pinMode(PIN_RED, OUTPUT);
    pinMode(PIN_BLUE, OUTPUT);
    pinMode(PIN_YELLOW, OUTPUT);

    WiFi.mode(WIFI_STA);

    SPIHandler::init();

    Serial.begin(9600);
    delay(1000);

    SPIHandler::start();

    Storage::init();

    SPIHandler::displayUpdate(0x86);

    delay(1000);
}

/**
 * @brief Resets programm
 * 
 * Ereases frame buffer and resets state machine
 */
void reset()
{
    SPIHandler::imageEraseFrameBuffer(0);
    receiveState = MAIN_STATE ::STATE_NVS;
}

/**
 * @brief Called in endless loop after setup
 * 
 * Contains the state machine which handles, reading from flash, hosting an AP, connecting to network, receiving images and uploading them with SPI
 */
void loop()
{
    byte *buffer;

    char *ssid;
    char *password;
    bool success;

    switch (receiveState)
    {
    case MAIN_STATE ::STATE_NVS: // read flash
        // try to read "ssid" and "password" from flash
        ssid = Storage::readSSID();
        password = Storage::readPassword();
        // try to connect them. if valid, true will be returned
        //success = true;//
        success = WiFiHandler::init(ssid, password);
        if (success)
        {
            receiveState = MAIN_STATE ::STATE_WAITING;
        }
        else
        {
            receiveState = MAIN_STATE ::STATE_BEACON;
        }
        break;

    case MAIN_STATE ::STATE_BEACON: // host access point
        LED::setOnlyYellow();

        bool answer;
        answer = Beacon::hostAP();
        if (answer)
        {
            ssid = Beacon::getSSID();
            password = Beacon::getPassword();
            if (strlen(ssid) != 0 && strlen(password) != 0)
            {
                Serial.println(ssid);
                Serial.println(password);
                success = WiFiHandler::init(ssid, password);
                if (success)
                {
                    receiveState = MAIN_STATE ::STATE_WAITING;
                    Storage::write(ssid, password);
                }
            }
        }
        break;

    case MAIN_STATE ::STATE_WAITING: // waiting for input over wifi
        LED::setOnlyBlue();

        buffer = (byte *)malloc(HEADER_LENGTH); // allocate memory for custom header

        waitForTCM(); // check if TCM is ready

        if (WiFiHandler::handle(buffer, HEADER_LENGTH) == WiFiStatus::WIFI_SUCCESS)
        {

            // read file size from header
            maxFileSize = (buffer[HEADER_OFFSET_FILESIZE + 0] << 24) |
                          (buffer[HEADER_OFFSET_FILESIZE + 1] << 16) |
                          (buffer[HEADER_OFFSET_FILESIZE + 2] << 8) |
                          (buffer[HEADER_OFFSET_FILESIZE + 3] << 0);

            //maxFileSize -= 16; //substract epd header, as it is not part of data

            // read chunk size from header
            chunkLength = (buffer[HEADER_OFFSET_CHUNKSIZE + 0] << 8) |
                          (buffer[HEADER_OFFSET_CHUNKSIZE + 1] << 0);
            sleepTime = 5; // 5 us

            // TEMP!!!
            //if(IS_DISPLAY_CONNECTED == 1) ImageEraseFrameBuffer(0);

            receiveState = MAIN_STATE ::STATE_RECEIVE_DATA; // EPD header is skipped for now
            lastTime = millis();
            tcmWaitCounter = 0;
            //receiveState = STATE_RECEIVE_EPD_HEADER;
        }

        free(buffer);

        break;

    case MAIN_STATE ::STATE_RECEIVE_EPD_HEADER: // currently not used (because not needed)
        buffer = (byte *)malloc(HEADER_EPD_LENGTH);

        waitForTCM();

        if (WiFiHandler::handle(buffer, HEADER_EPD_LENGTH) == WiFiStatus::WIFI_SUCCESS)
        {

            if (IS_DISPLAY_CONNECTED == 1)
                SPIHandler::imageEraseFrameBuffer(0);
            receiveState = MAIN_STATE ::STATE_RECEIVE_DATA;
        }

        free(buffer);
        break;

    case MAIN_STATE ::STATE_RECEIVE_DATA: // receive image data
        LED::setOnlyRed();

        // last chunk!
        if (curFileSize + chunkLength >= maxFileSize)
            chunkLength = maxFileSize - curFileSize;

        buffer = (byte *)malloc(chunkLength); // allocate memory for next chunk

        waitForTCM(); // check if TCM is ready

        if (WiFiHandler::handle(buffer, chunkLength) == WiFiStatus::WIFI_SUCCESS)
        {
            dataCounter++;

            //printBytes(buffer, 8);

            // upload image data chunk to TCM
            if (IS_DISPLAY_CONNECTED == 1)
                uploadChunk(buffer, chunkLength);

            // curFileSize is currently handled in uploadChunk()
            //curFileSize += chunkLength;

            waitForTCM(); // check if TCM is ready

            WiFiHandler::requestDataChunk(); // send tcp signal to order the next chunk
        }

        free(buffer);

        // last chunk!
        if (curFileSize >= maxFileSize) // || curFileSize >= 0x30d4)
        {
            // tell the TCM to show the new image
            if (IS_DISPLAY_CONNECTED == 1)
                SPIHandler::displayUpdate(SPIHandler::DISPLAY_UPDATE_MODE_DEFAULT);

            Serial.print("total time:       ");
            Serial.println(millis() - lastTime);
            Serial.print("tcm wait counter: ");
            Serial.println(tcmWaitCounter);
            SPIHandler::printSpiTime();

            //if (IS_DISPLAY_CONNECTED == 1) SPIHandler::imageEraseFrameBuffer(0); // TEMP

            sleepTime = 1000000;
            receiveState = MAIN_STATE ::STATE_WAITING;
            maxFileSize = 1;
            curFileSize = 0;
            dataCounter = 0;
            chunkLength = 250;

            reset();
        }
        break;
    }

    yield();
    delayMicroseconds(sleepTime);
}

/**
 * @brief Waits until TCM is ready
 */
void waitForTCM()
{
    if (IS_DISPLAY_CONNECTED == 1)
    {
        while (digitalRead(PIN_BUSY) == LOW)
        {
            yield();
            tcmWaitCounter++;
            delayMicroseconds(5);
        }
    }
}

/**
 * @brief Uploads chunk over SPI
 * 
 * @param data 
 * 
 * @param length
 */
void uploadChunk(byte *data, int length)
{
    int bufferLength = 250; // length of one package inside the chunk (251 bytes is maximum, we used 250 because it's more convenient)
    //uint32_t result = 0;
    for (int i = 0; i < chunkLength; i += bufferLength) // attention! i is incremented by 250 each loop!
    {
        // adjust length for last chunk if it's smaller
        if (i + bufferLength > chunkLength)
            bufferLength = chunkLength % bufferLength;
        // ... and also adjust length if it's the last package of the last chunk
        if (curFileSize + bufferLength > maxFileSize)
            bufferLength = maxFileSize - curFileSize;

        //lastTimeTCM = millis();
        waitForTCM();
        //Serial.print("TCM time: "); Serial.println(millis() - lastTimeTCM);

        //lastTime = millis();

        // upload image data to TCM memory (ca 45 ms per upload (that's a lot!))
        if (IS_DISPLAY_CONNECTED == 1)
        {
            SPIHandler::uploadImageData(0, bufferLength, data + i);
        }

        //Serial.print("time to process: "); Serial.println(millis() - lastTime);

        //Serial.println(i);
        //result = i + bufferLength;

        curFileSize += bufferLength;
        if (curFileSize >= maxFileSize)
            return;
    }
}
