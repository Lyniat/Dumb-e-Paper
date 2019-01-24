#include "WiFiHandler.hpp"

WiFiServer server(PORT);
WiFiClient client;
WiFiMulti wifiMulti;

const byte WiFiHandler::O = 0x4F;
const byte WiFiHandler::K = 0x4B;

char *WiFiHandler::ip = (char *)"";

bool WiFiHandler::init(String ssid, String password)
{
    Serial.println(ssid);
    Serial.println(password);

    char new_ssid[ssid.length() + 1];
    char new_password[password.length() + 1];
    ssid.toCharArray(new_ssid, ssid.length() + 1);
    password.toCharArray(new_password, password.length() + 1);

    Serial.println(new_password);
    Serial.println(new_ssid);

    WiFi.persistent(false);
    WiFi.disconnect(true,true);
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_STA);

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
    delay(1000);
    wifiMulti.addAP(new_ssid, new_password);
    //WiFi.begin(new_ssid, new_password);
    //WiFi.setSleep(false);

    Serial.println("connecting");

    int counter = 0;

    while (wifiMulti.run() != WL_CONNECTED)
    {
        delay(500);
        //Serial.print(".");
        //Serial.println(WiFi.status());
        //WiFi.begin(new_ssid, new_password);
        counter++;

        if (counter >= 20)
        {
            return false;
        }
    }
    Serial.println("");
    Serial.println("WiFi connected");

    server.begin();
    Serial.println("Server started");

    Serial.print("Use this URL : ");
    Serial.print("http://");
    ip = strdup(WiFi.localIP().toString().c_str());
    Serial.print(ip);
    Serial.println("/");

    return true;
}

WiFiStatus WiFiHandler::handle(byte *buffer, int le)
{
    do
    {
        yield();
        delayMicroseconds(5); // evtl normales delay
        client = server.available();
        //sendIP();
    } while (!client);

    // Wait until the client sends some data
    //Serial.println("new client");
    while (!client.available())
    {
        yield();
        delayMicroseconds(5); // evtl normales delay
    }

    client.readBytes(buffer, le);

    //client.flush();

    return WiFiStatus::WIFI_SUCCESS;
}

void WiFiHandler::requestDataChunk()
{
    client.write(O);
    client.write(K);
}
