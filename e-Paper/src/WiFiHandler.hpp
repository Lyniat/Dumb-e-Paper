#ifndef __WIFIHANDLER_H_INCLUDED__
#define __WIFIHANDLER_H_INCLUDED__

#include "main.hpp"


const int PORT = 1516;

typedef enum {
    WIFI_SUCCESS,
    WIFI_ERROR,
    WIFI_WAIT,
    SHOW_PICTURE_01,
    SHOW_PICTURE_02,
    CLEAR_SCREEN
} wifiResponse;

class WiFiHandler{
public:
    static bool init(String id, String password);

    static wifiResponse handle(byte *buffer, int le);

    static void requestDataChunk();

private:
    static const byte O;
    static const byte K;

    static char* ip;
};


#endif
