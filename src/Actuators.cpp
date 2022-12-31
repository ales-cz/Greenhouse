#include "Settings.h"
#include "Actuators.h"

void Actuators::setHeat(bool active)
{
  digitalWrite(HEAT, active);
  heating = active;
}

void Actuators::setCircul(bool active)
{
  digitalWrite(CIRCUL, active);
  circulating = active;
}

Actuators::Actuators()
{
  pinMode(HEAT, OUTPUT);
  pinMode(CIRCUL, OUTPUT);
}

void Actuators::begin(Preferences *prefs)
{
  this->prefs = prefs;
  thermostat = prefs->getBool("thermostat", 1);
  circulation = prefs->getBool("circulation", 1);
  reqHeatTemp = prefs->getFloat("reqHeatTemp", 24);
  reqCirculDiff = prefs->getFloat("reqCirculDiff", 2);
  heatHyst = prefs->getFloat("heatHyst", 1);
  circulHyst = prefs->getFloat("circulHyst", 1);
}

// Pokud je hystereze 1°C, požadovaná teplota 5°C, termostat vypne při 6°C a znovu sepne při 4°C
void Actuators::set(float tempInt, float tempFloor, float tempCeiling)
{
  if (thermostat)
  {
    if (tempInt < reqHeatTemp - heatHyst)
    {
      setHeat(1);
    }
    else if (tempInt > reqHeatTemp + heatHyst)
    {
      setHeat(0);
    }
  }
  if (circulation)
  {
    if (tempCeiling - tempFloor > reqCirculDiff + circulHyst)
    {
      setCircul(1);
    }
    else if (tempCeiling - tempFloor < reqCirculDiff - circulHyst)
    {
      setCircul(0);
    }
  }
}

bool Actuators::getThermostat()
{
  return thermostat;
}

void Actuators::setThermostat(bool state)
{
  thermostat = state;
  prefs->putBool("thermostat", thermostat);
}

bool Actuators::getCirculation()
{
  return circulation;
}

void Actuators::setCirculation(bool state)
{
  circulation = state;
  prefs->putBool("circulation", circulation);
}

float Actuators::getReqHeatTemp()
{
  return reqHeatTemp;
}

void Actuators::setReqHeatTemp(float temp)
{
  reqHeatTemp = temp;
  prefs->putFloat("reqHeatTemp", reqHeatTemp);
}

float Actuators::getReqCirculDiff()
{
  return reqCirculDiff;
}

void Actuators::setReqCirculDiff(float diff)
{
  reqCirculDiff = diff;
  prefs->putFloat("reqCirculDiff", reqCirculDiff);
}

float Actuators::getHeatHyst()
{
  return heatHyst;
}

void Actuators::setHeatHyst(float hyst)
{
  heatHyst = hyst;
  prefs->putFloat("heatHyst", heatHyst);
}

float Actuators::getCirculHyst()
{
  return circulHyst;
}

void Actuators::setCirculHyst(float hyst)
{
  circulHyst = hyst;
  prefs->putFloat("circulHyst", circulHyst);
}

bool Actuators::isThermostat()
{
  return thermostat;
}

bool Actuators::isCirculation()
{
  return circulation;
}

bool Actuators::isHeating()
{
  return heating;
}

bool Actuators::isCirculating()
{
  return circulating;
}
