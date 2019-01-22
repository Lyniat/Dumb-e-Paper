#ifndef __WIFIHANDLER_H_INCLUDED__
#define __WIFIHANDLER_H_INCLUDED__

/**
 * @defgroup WiFi Handler
 */

/** @addtogroup WiFi Handler */
/*@{*/

#include "main.hpp"
#include <WiFi.h>

/**
 * @brief Could be anything but must also be changed in python script
 */
const int PORT = 1516;

enum class WiFiStatus
{
    WIFI_SUCCESS,
    WIFI_ERROR,
    WIFI_WAIT,
    SHOW_PICTURE_01,
    SHOW_PICTURE_02,
    CLEAR_SCREEN
};

class WiFiHandler
{
  public:
    /**
     * @brief Start WiFi connection
     * 
     * @param id ssid to connect
     * @param password for network
     * @return true if WiFi could connect
     */
    static bool init(String id, String password);

    /**
     * @brief Receive data from WiFi and write to buffer
     * 
     * @param buffer gets WiFi data
     * @param le buffer length
     * @return WiFiStatus 
     */
    static WiFiStatus handle(byte *buffer, int le);

    /**
     * @brief Sends "OK" to server, to receive new data
     */
    static void requestDataChunk();

  private:
    static const byte O;
    static const byte K;

    static char *ip;
};

/*@}*/
#endif
