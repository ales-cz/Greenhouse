#ifndef MY_CLOUD_H
#define MY_CLOUD_H

#define TS_ENABLE_SSL // For HTTPS SSL connection
#define WIFISSLCLIENT_H

#include <Arduino.h>
#include <Ethernet.h>
#include <SSLClient.h>
#include "Certificates.h"
#include <ThingSpeak.h>

#define DELAY_UPDATE 60000 // cloudUpdate

class Cloud
{

private:
    EthernetClient base_client;
    SSLClient client;
    unsigned long channelNumber;
    char writeAPIKey[17] = "99A0T3QXV0LMP6MA";
    u_long lastUpdate;

public:
    Cloud();
    void begin(Preferences *prefs);
    byte update(byte *status, float tempInt, float tempExt, float tempFloor, float tempCeiling,
                float humInt, float humExt, float illum, bool heating, bool circulating);
};
#endif