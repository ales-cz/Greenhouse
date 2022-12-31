#ifndef MY_ACTUATORS_H
#define MY_ACTUATORS_H
#include <Arduino.h>

class Actuators
{

private:
    Preferences *prefs;
    bool thermostat, circulation, heating, circulating;
    float reqHeatTemp, reqCirculDiff, heatHyst, circulHyst; // Actuators settings
    void setHeat(bool active);
    void setCircul(bool active);

public:
    Actuators();
    void begin(Preferences *prefs);
    void set(float tempInt, float tempFloor, float tempCeiling);
    bool getThermostat();
    void setThermostat(bool state);
    bool getCirculation();
    void setCirculation(bool state);
    float getReqHeatTemp();
    void setReqHeatTemp(float temp);
    float getReqCirculDiff();
    void setReqCirculDiff(float diff);
    float getHeatHyst();
    void setHeatHyst(float hyst);
    float getCirculHyst();
    void setCirculHyst(float hyst);
    bool isThermostat();
    bool isCirculation();
    bool isHeating();
    bool isCirculating();
};
#endif