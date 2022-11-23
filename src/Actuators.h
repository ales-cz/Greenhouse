#ifndef MY_ACTUATORS_H
#define MY_ACTUATORS_H
#include <Arduino.h>

class Actuators
{

private:
    int8_t pHeat, pCircul;
    bool thermostat, circulation, heating, circulating; 
    float reqHeatTemp, reqCirculDiff, heatHyst, circulHyst; // Actuators settings
    void setHeat(bool active);
    void setCircul(bool active);

public:
    Actuators(int8_t pHeat, int8_t pCircul);
    void set(float tempInt, float tempFloor, float tempCeiling);
    void isHeating();
    void isCirulating();
};
#endif