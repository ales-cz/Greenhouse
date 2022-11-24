#ifndef MY_CLOUD_H
#define MY_CLOUD_H
#include <Arduino.h>
#include <Ethernet.h>
#include <ThingSpeak.h>

#define DELAY_UPDATE 20000 // cloudUpdate

class Cloud
{

private:
    unsigned long channelNumber;
    char writeAPIKey[17] = "99A0T3QXV0LMP6MA";
    EthernetClient client;
    u_long lastUpdate;

public:
    Cloud();
    void begin(Preferences *prefs);
    byte update(byte *status, float tempInt, float tempExt, float tempFloor, float tempCeiling,
                float humInt, float humExt, float illum, bool heating, bool circulating);
};
#endif