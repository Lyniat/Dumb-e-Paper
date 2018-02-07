#ifndef __MAIN_H_INCLUDED__
#define __MAIN_H_INCLUDED__

#include <Arduino.h>
#include "main.hpp"
#include "SPIHandler.hpp"
#include "WiFiHandler.hpp"
#include "Beacon.hpp"
#include "MainStates.hpp"
#include "LED.hpp"
#include "Storage.hpp"
#include "Tools.hpp"

#define U16_MSB_TO_U8(u16) ((u16 >> 8) & 0xff)
#define U16_LSB_TO_U8(u16) (u16 & 0xff)

/*
 * PINS
 * https://nodemcu.readthedocs.io/en/master/en/modules/gpio/
 */

// esp-8266 pins
const uint8_t PIN_EN = 5;   // D1
const uint8_t PIN_BUSY = 4; // D2
const uint8_t PIN_CS = 2;   // D4

// esp-8266 leds
const uint8_t PIN_RED = 16;   // D0
const uint8_t PIN_BLUE = 15;  // D8
const uint8_t PIN_YELLOW = 0; // D3

void uploadChunk(byte *data, int length);

void waitForTCM();

#endif
