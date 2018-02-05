#include "Storage.hpp"

char* Storage::ssid = (char*)"                              ";
char* Storage::password = (char*)"                              ";

void Storage::read() {
    EEPROM.begin(512);
    EEPROM.get(0, ssid);
    EEPROM.get(0+sizeof(ssid), password);
    EEPROM.end();

    Serial.print("loaded ");
    Serial.print(ssid);
    Serial.print(" and ");
    Serial.println(password);
}

void Storage::write(char* ssid, char* password) {
    Serial.print("saving ");
    Serial.print(ssid);
    Serial.print(" and ");
    Serial.println(password);

    EEPROM.begin(512);
    EEPROM.put(0, ssid);
    EEPROM.put(0+sizeof(ssid), password);
    EEPROM.commit();
    EEPROM.end();
}

char* Storage::readSSID(){
    read();
    return ssid;
}

char* Storage::readPassword(){
    read();
    return password;
}
