#ifndef __SPIHANDLER_H_INCLUDED__
#define __SPIHANDLER_H_INCLUDED__

/**
 * @defgroup SPI Handler
 */

/** @addtogroup SPI Handler */
/*@{*/

#include <SPI.h>
#include "main.hpp"

class SPIHandler
{
  public:
    // SPI functions
    static void printSpiTime();

    /**
     * @brief initializes SPI
     */
    static void init();

    /**
     * @brief Sends bytes over SPI to display
     * 
     * @param data command
     * @param commandLength 
     * @param result pointer to save result to
     * @param resultLength expected length of result
     */
    static void spiWrite(byte *data, char length, byte *result, char resultLength);

    static void spiRead(byte *data, char commandLength, byte *result, char resultLength);

    static void start();

    // TCM2 functions

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
    static uint16_t uploadImageData(byte slotNumber, byte packetSize, byte *data);

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
    static uint16_t imageEraseFrameBuffer(byte slotNumber);


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
    static uint16_t displayUpdate(byte updateMode = DISPLAY_UPDATE_MODE_DEFAULT);

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

/*@}*/
#endif
