#ifndef MY_NETWORK_H
#define MY_NETWORK_H
#include <Arduino.h>
#include <Preferences.h>
#include <Ethernet.h> // W5500 module

class Network
{

private:
    byte mac[6] = {0xA8, 0x61, 0xFE, 0x3F, 0x58, 0x37};
    bool dhcp;

public:
    Network();
    void begin(Preferences *prefs);
};
#endif