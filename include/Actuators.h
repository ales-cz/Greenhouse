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
    Actuators(int8_t pHeat, int8_t pCircul, Preferences* prefs);
    void set(float tempInt, float tempFloor, float tempCeiling);
    void setThermostat(bool state);
    void setCirculation(bool state);
    void setReqHeatTemp(float temp);
    void setReqCirculDiff(float diff);
    void setHeatHyst(float hyst);
    void setCirculHyst(float hyst);
    bool isHeating();
    bool isCirulating();
};
#endif