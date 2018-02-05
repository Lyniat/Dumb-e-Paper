#ifndef __LED_H_INCLUDED__
#define __LED_H_INCLUDED__

#include "main.hpp"

class LED{
public:
    static void setRed(bool state);
    static void setYellow(bool state);
    static void setBlue(bool state);

    static void setOnlyRed();
    static void setOnlyYellow();
    static void setOnlyBlue();
};

#endif
