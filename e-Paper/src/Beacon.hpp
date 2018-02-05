#ifndef __BEACON_H_INCLUDED__
#define __BEACON_H_INCLUDED__

#include "main.hpp"
#include "Website.hpp"
#include <ESP8266WiFi.h>

#include <ESP8266mDNS.h>

class Beacon {
public:
    static bool hostAP();

    static char* getSSID();

    static char* getPassword();

private:

    static void hostWebsite();

    static void handleWebsite();

    static const char *ownSSID;
    static const char *ownPassword;

    static bool hosting;

    static char* nextID;
    static char* nextPassword;

};

#endif
