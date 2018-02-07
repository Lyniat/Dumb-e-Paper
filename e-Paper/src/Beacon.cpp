#include "Beacon.hpp"

const char *Beacon::ownSSID = "e-Paper";
const char *Beacon::ownPassword = "MyPassword";

bool Beacon::hosting = false;

char *Beacon::nextID = new char[32];
char *Beacon::nextPassword = new char[32];

WiFiServer webServer(80);

/**
 * @brief Hosts setup website and parses ssid and password
 * 
 */
void Beacon::handleWebsite()
{
    WiFiClient client = webServer.available();
    if (!client)
    {
        return;
    }
    Serial.println("");
    Serial.println("New client");
    if (client)
    {
        Serial.println("new client");
        // as long as client is connected
        while (client.connected())
        {
            if (client.available())
            {
                /* request end with '\r' -> this is HTTP protocol format */
                String req = client.readStringUntil('\r');
                /* First line of HTTP request is "GET / HTTP/1.1"
                  here "GET /" is a request to get the first page at root "/"
                  "HTTP/1.1" is HTTP version 1.1
                */
                /* now we parse the request to see which page the client want */
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
                client.flush();

                String s;
                /* if request is "/" then client request the first page at root "/" -> we process this by return "Hello world"*/
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
                /* send response back to client and then close connect since HTTP do not keep connection*/
                client.print(s);
                client.stop();
            }
        }
    }
    Serial.println("Done with client");
}

/**
 * @brief Creates MDSN respnder
 * 
 */
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

/**
 * @brief Called from main class to host AP
 * @return true if ssid and password are fetched from web interface
 */
bool Beacon::hostAP()
{
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

/**
 * @return ssid from web interface
 */
char *Beacon::getSSID()
{
    return nextID;
}

/**
 * @return password from web interface
 */
char *Beacon::getPassword()
{
    return nextPassword;
}
