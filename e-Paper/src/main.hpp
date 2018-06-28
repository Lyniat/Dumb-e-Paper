#ifndef __MAIN_H_INCLUDED__
#define __MAIN_H_INCLUDED__

/**
 * @defgroup Main
 */

/** @addtogroup Main */
/*@{*/

#include <M5Stack.h>
#include "main.hpp"
#include "SPIHandler.hpp"
#include "WiFiHandler.hpp"
#include "Beacon.hpp"
#include "MainStates.hpp"
#include "LED.hpp"
#include "Storage.hpp"
#include "Tools.hpp"

/*
 * PINS
 * https://nodemcu.readthedocs.io/en/master/en/modules/gpio/
 */

// esp-8266 pins
const uint8_t PIN_EN = 21;
const uint8_t PIN_BUSY = 22;
const uint8_t PIN_CS = 2;

// esp-8266 leds
const uint8_t PIN_RED = 3;
const uint8_t PIN_BLUE = 1;
const uint8_t PIN_YELLOW = 16;

void uploadChunk(byte *data, int length);

void waitForTCM();

/*@}*/
#endif
