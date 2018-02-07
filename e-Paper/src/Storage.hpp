#ifndef __STORAGE_H_INCLUDED__
#define __STORAGE_H_INCLUDED__

#include <EEPROM.h>
#include "main.hpp"

class Storage
{
  public:
    static void read();
    static void write(char *ssid, char *password);
    static char *readSSID();
    static char *readPassword();

  private:
    static char *ssid;
    static char *password;
};

#endif
