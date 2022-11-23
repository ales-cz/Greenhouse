#ifndef MY_CLOUD_H
#define MY_CLOUD_H
#include <Arduino.h>
#include <ThingSpeak.h> // ThingSpeak

class Cloud
{

private:
    unsigned long channelNumber;
    char writeAPIKey[17] = "99A0T3QXV0LMP6MA";
    int cloudLastUpdateStatus = 200;

public:
    Cloud();
    // void begin();
    void update();
    // void init();
};
#endif