#ifndef __BEACON_H_INCLUDED__
#define __BEACON_H_INCLUDED__

/**
 * @defgroup Beacon
 */

/** @addtogroup Beacon */
/*@{*/

#include "main.hpp"
#include "Website.hpp"
#include <ESP8266WiFi.h>

#include <ESP8266mDNS.h>

/**
 * @brief Hosts setup website and parses ssid and password
 */
class Beacon
{
  public:
    /**
     * @brief Called from main class to host AP
     * @return true if ssid and password are fetched from web interface
     */
    static bool hostAP();

    /**
     * @return ssid from web interface
     */
    static char *getSSID();

    /**
     * @return password from web interface
     */
    static char *getPassword();

  private:
    /**
     * @brief Creates MDSN responder
     */
    static void hostWebsite();

    static void handleWebsite();

    static const char *ownSSID;
    static const char *ownPassword;

    static bool hosting;

    static char *nextID;
    static char *nextPassword;
};

/*@}*/
#endif
