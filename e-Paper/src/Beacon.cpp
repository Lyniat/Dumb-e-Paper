#include "Beacon.hpp"

const char *Beacon::ownSSID = "e-Paper";
const char *Beacon::ownPassword = "MyPassword";

bool Beacon::hosting = false;

char *Beacon::nextID = new char[32];
char *Beacon::nextPassword = new char[32];

WiFiServer webServer(80);

// code partially taken from https://github.com/nhatuan84/esp32-webserver
void Beacon::handleWebsite()
{
    WiFiClient client = webServer.available();
    if (!client)
    {
        return;
    }
    Serial.println("");
    if (client)
    {
        bool looping = true;
        Serial.println("new client");
        while (true)
        {
            String req = client.readStringUntil('\r');
            
            int addr_start = req.indexOf(' ');
            int addr_end = req.indexOf(' ', addr_start + 1);
            if (addr_start == -1 || addr_end == -1)
            {
                Serial.print("Invalid request: ");
                Serial.println(req);
                return;
            }
            req = req.substring(addr_start + 1, addr_end);
            Serial.print("Request: ");
            Serial.println(req);
            //client.flush();

            String s;
            if (req == "/")
            {
                s = Website::WEBSITE;
                Serial.println("Sending 200");
            }
            else
            {
                char *request = Tools::getValueFromString(strdup(req.c_str()), '?', 1);
                Serial.println(request);

                char *ssid = Tools::getValueFromString(request, '&', 0);

                char *pwd = Tools::getValueFromString(request, '&', 1);

                nextID = Tools::getValueFromString(ssid, '=', 1);

                nextPassword = Tools::getValueFromString(pwd, '=', 1);

            }
            client.print(s);
        }
        client.stop();
    }
    Serial.println("Done with client");
}

void Beacon::hostWebsite()
{

    // set up mDNS
    if (!MDNS.begin("esp32"))
    {
        Serial.println("Error setting up MDNS responder!");
        return;
    }
    Serial.println("mDNS responder started");
    /* Start Web Server server */
    webServer.begin();
    Serial.println("Web server started");

    /* Add HTTP service to MDNS-SD */
    MDNS.addService("http", "tcp", 80);
}

bool Beacon::hostAP()
{
    nextID = "";
    nextPassword = "";
    handleWebsite();

    if (hosting)
    {
        if (strlen(nextID) != 0 && strlen(nextPassword) != 0)
        {
            Serial.println("returning network info");
            return true;
        }
        return false;
    }

    hosting = true;

    WiFi.mode(WIFI_AP_STA);

    //access point part
    Serial.println("Creating Accesspoint");
    WiFi.softAP(ownSSID, ownPassword);
    //WiFi.softAP()
    Serial.print("IP address:\t");
    Serial.println(WiFi.softAPIP());

    hostWebsite();

    return false;
}

char *Beacon::getSSID()
{
    return nextID;
}

char *Beacon::getPassword()
{
    return nextPassword;
}
