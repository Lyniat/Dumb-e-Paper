#ifndef __SPIHANDLER_H_INCLUDED__
#define __SPIHANDLER_H_INCLUDED__

#include <SPI.h>
#include "main.hpp"

class SPIHandler
{
  public:
    // SPI functions
    static void printSpiTime();

    static void init();

    static void spiWrite(byte *data, char length, byte *result, char resultLength);

    static void spiRead(byte *data, char commandLength, byte *result, char resultLength);

    static void start();

    // TCM2 functions
    static uint16_t uploadImageData(byte slotNumber, byte packetSize, byte *data);
    static uint16_t imageEraseFrameBuffer(byte slotNumber);                       // works
    static uint16_t displayUpdate(byte updateMode = DISPLAY_UPDATE_MODE_DEFAULT); // works

    // possible result codes
    static const uint16_t EP_SW_NORMAL_PROCESSING = 0x9000;
    static const uint16_t EP_SW_MEMORY_FAILURE = 0x6581;
    static const uint16_t EP_SW_WRONG_LENGTH = 0x6700;
    static const uint16_t EP_FRAMEBUFFER_SLOT_NOT_AVAILABLE = 0x6981;
    static const uint16_t EP_SW_WRONG_PARAMETERS_P1P2 = 0x6A00;
    static const uint16_t EP_FRAMEBUFFER_SLOT_OVERRUN = 0x6A84;
    static const uint16_t EP_SW_INVALID_LE = 0x6C00;
    static const uint16_t EP_SW_INSTRUCTION_NOT_SUPPORTED = 0x6D00;
    static const uint16_t EP_SW_GENERAL_ERROR = 0x6F00;

    // diplay update modes
    static const byte DISPLAY_UPDATE_MODE_DEFAULT = 0x82;            // best quality
    static const byte DISPLAY_UPDATE_MODE_FLASHLESS = 0x85;          // fastest
    static const byte DISPLAY_UPDATE_MODE_FLASHLESS_INVERTED = 0x86; // compromise between speed and quality

    /*
    static const byte EPDheader[16] = {0x3D, 0x04, 0x00, 0x05,
                                       0x00, 0x01, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00};
                                       */

  private:
    static long spiTime;
    static long lastSpiTime;
};

#endif
