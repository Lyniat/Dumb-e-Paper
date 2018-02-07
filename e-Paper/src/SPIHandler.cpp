#include "SPIHandler.hpp"

long SPIHandler::spiTime = 0;
long SPIHandler::lastSpiTime = 0;

void SPIHandler::printSpiTime()
{
    Serial.print("SPI time: ");
    Serial.println(spiTime);
}

/**
 * @brief initilaizes SPI
 * 
 */
void SPIHandler::init()
{
    // initialize SPI:
    SPI.begin();
    //SPI.setClockDivider (SPI_CLOCK_DIV8);  //slow the clock down (recommended: 3mhz, max: 12mhz)
    SPI.setFrequency(4000000L); // 500000
    SPI.setDataMode(SPI_MODE3); // CPOL = 1, CPH = 1
    SPI.setBitOrder(MSBFIRST);

    //does not work with my IDE
    //SPI.beginTransaction (SPISettings (1000000, MSBFIRST, SPI_MODE0));
}

/**
 * @brief Sends bytes over SPI to display
 * 
 * @param data command
 * @param commandLength 
 * @param result pointer to save result to
 * @param resultLength expected length of result
 */
void SPIHandler::spiWrite(byte *data, char commandLength, byte *result, char resultLength)
{
    lastSpiTime = micros();

    // the delay(0) is used to prevent cpu lock result in WDT resets
    yield();

    //uint8_t* out = (uint8_t*) malloc(commandLength + resultLength);

    // wait until controller is ready
    while (digitalRead(PIN_BUSY) == LOW)
    {
        //yield();
        delayMicroseconds(5); //100
    }

    // send command to controller
    digitalWrite(PIN_CS, LOW);
    //yield();
    delayMicroseconds(10);
    /*for(int i = 0; i < commandLength; i++)
    {
      SPI.transfer(data[i]);
    }*/

    SPI.writeBytes(data, commandLength);
    /*for(int i = 0; i < commandLength + resultLength; i++)
    {
      Serial.println(out[i], HEX);
    }
    free(out);*/

    //yield();
    delayMicroseconds(5);
    digitalWrite(PIN_CS, HIGH);

    // wait until controller is ready
    while (digitalRead(PIN_BUSY) == LOW)
    {
        delayMicroseconds(5); //100
    }

    digitalWrite(PIN_CS, LOW);

    delayMicroseconds(10); //10
    // read answer from controller and save it to result pointer
    for (int i = 0; i < resultLength; i++)
    {
        result[i] = SPI.transfer(0x00);
    }
    delayMicroseconds(5); //10
    digitalWrite(PIN_CS, HIGH);

    spiTime += (micros() - lastSpiTime);
}

void SPIHandler::start()
{
    //start the device
    Serial.println("initializing...");

    digitalWrite(PIN_EN, HIGH);
    delay(100);

    Serial.println("set en to low...");
    digitalWrite(PIN_EN, LOW);

    Serial.println("wait for BUSY rising edge...");
    while (digitalRead(PIN_BUSY) == LOW)
    {
        delay(1);
    }

    Serial.println("set CS to high...");
    digitalWrite(PIN_CS, HIGH);

    Serial.println("wait for one second..."); //to give us time to open serial monitor
    delay(1000);

    Serial.println("starting...");
}

/**
 * @brief Uploads image data in EPD format to TCon image memory
 * 
 * Data needs to be divided into packets and transferred
 * with multiple UploadImageData commands.
 * Returns EP_FRAMEBUFFER_SLOT_OVERRUN if memory size is exceeded.
 *
 * If this command is used in partial update, do not include EPD header
 * and encode data in EPD formaty type 0
 *
 * Use ImageEraseFrameBuffer() once before uploading image data.
 * Update the display after you have uploaded all of your data.
 *
 * Command:
 * INS:  0x20
 * P1:   0x01
 * P2:   slot number
 * Lc:   data packet size (max 0xFA)
 * Data: Lc data bytes (max 251 bytes)
 * 
 * @param slotNumber 
 * @param packetSize 
 * @param data 
 * @return uint16_t 2-byte status code
 */
uint16_t SPIHandler::uploadImageData(byte slotNumber, byte packetSize, byte *data)
{
    uint16_t result;
    byte params[4] = {0x20, 0x01, slotNumber, packetSize};
    byte *input;
    char resultLength = 2;
    byte buffer[resultLength];

    //unsigned long lastTimeMalloc = 0;
    //unsigned long lastTimeMemcpy = 0;
    //unsigned long lastTimeSpiWrite = 0;

    //lastTimeMalloc = millis();
    input = (byte *)malloc((sizeof params + packetSize) * sizeof(byte));
    //Serial.print("time malloc: "); Serial.println(millis() - lastTimeMalloc);

    //lastTimeMemcpy = millis();
    memcpy(input, params, sizeof(params) * sizeof(byte));
    memcpy(input + sizeof(params), data, packetSize * sizeof(byte));
    //Serial.print("time cpy: "); Serial.println(millis() - lastTimeMemcpy);

    //lastTimeSpiWrite = millis();
    spiWrite(input, sizeof params + packetSize, buffer, resultLength);
    //Serial.print("time spi: "); Serial.println(millis() - lastTimeSpiWrite);

    // convert result code to uint16
    for (int i = 0; i < resultLength; i++)
    {
        result <<= 8;
        result |= buffer[i];
    }

    free(input);
    return result;
}

/**
 * @brief Ereases selected slot
 * 
 * Resets data pointer to beginning of selected memory slot index
 * and erases selected slot.
 * The erased slot is filled with 0xFF, which represents a black image.
 *
 * Command:
 * INS: 0x20
 * P1:  0x0E
 * P2:  memory slot index
 *
 * 
 * @param slotNumber 
 * @return uint16_t 2-byte status code
 */
uint16_t SPIHandler::imageEraseFrameBuffer(byte slotNumber)
{
    uint16_t result;
    byte input[3] = {0x20, 0x0E, slotNumber};
    char resultLength = 2;
    byte buffer[resultLength];

    spiWrite(input, sizeof(input), buffer, resultLength);

    // convert result code to uint16
    for (int i = 0; i < resultLength; i++)
    {
        result <<= 8;
        result |= buffer[i];
    }

    return result;
}

/**
 * @brief Updates display to show uploaded data
 * 
 * Starts the display refresh sequence displaying the current content of the image memory.
 * The transition sequence is chosen by INS parameter.
 * 
 * Command:
 * INS:  0x24 or 0x82: default (black -> white -> black), offers best quality
 *       0x85: flashless direct transition without blank screen; fast and energy efficient
 *       0x86: flashless inverted transition (inverted new image -> new image), compromise between quality and efficiency
 * P1:   0x01
 * P2:   memory slot index
 * Lc:   length
 * Data: temperature (optional)
 * 
 * @param updateMode 
 * @return uint16_t 2-byte status code
 */
uint16_t SPIHandler::displayUpdate(byte updateMode)
{
    uint16_t result;
    byte input[4] = {updateMode, 0x01, 0x00, 0x00};
    char resultLength = 2;
    byte buffer[resultLength];

    spiWrite(input, sizeof(input), buffer, resultLength);

    // convert result code to uint16
    for (int i = 0; i < resultLength; i++)
    {
        result <<= 8;
        result |= buffer[i];
    }

    return result;
}