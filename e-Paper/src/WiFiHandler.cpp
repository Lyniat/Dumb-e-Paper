#include "WiFiHandler.hpp"

WiFiServer server(PORT);
WiFiClient client;

const byte WiFiHandler::O = 0x4F;
const byte WiFiHandler::K = 0x4B;

char *WiFiHandler::ip = (char *)"";

bool WiFiHandler::init(String ssid, String password)
{
    WiFi.mode(WIFI_STA);
    Serial.println(ssid);
    Serial.println(password);

    char new_ssid[ssid.length() + 1];
    char new_password[password.length() + 1];
    ssid.toCharArray(new_ssid, ssid.length() + 1);
    password.toCharArray(new_password, password.length() + 1);

    Serial.println(new_password);
    Serial.println(new_ssid);
    WiFi.begin(new_ssid, new_password);

    Serial.println("connecting");

    int counter = 0;

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        counter++;

        if (counter >= 25)
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
