#include "LED.hpp"

void LED::setRed(bool state) {
    digitalWrite(PIN_RED, (uint8_t)state);
};

void LED::setBlue(bool state) {
    digitalWrite(PIN_BLUE, (uint8_t)state);
};

void LED::setYellow(bool state) {
    digitalWrite(PIN_YELLOW, (uint8_t)state);
};

void LED::setOnlyRed() {
    setRed(true);
    setBlue(false);
    setYellow(false);
};

void LED::setOnlyBlue() {
    setBlue(true);
    setRed(false);
    setYellow(false);

};

void LED::setOnlyYellow() {
    setYellow(true);
    setRed(false);
    setBlue(false);
};

