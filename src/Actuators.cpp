#include "Settings.h"
#include "Actuators.h"

void Actuators::setHeat(bool active)
{
  digitalWrite(pHeat, active);
  heating = active;
}

void Actuators::setCircul(bool active)
{
  digitalWrite(pCircul, active);
  circulating = active;
}

Actuators::Actuators(int8_t pHeat, int8_t pCircul)
{
  this->pHeat = pHeat;
  this->pCircul = pCircul;
  pinMode(pHeat, OUTPUT);
  pinMode(pCircul, OUTPUT);
}

void Actuators::begin(Preferences *prefs)
{
  setThermostat(prefs->getBool("thermostat", 1));
  setCirculation(prefs->getBool("circulation", 1));
  setReqHeatTemp(prefs->getFloat("reqHeatTemp", 24));
  setReqCirculDiff(prefs->getFloat("reqCirculDiff", 2));
  setHeatHyst(prefs->getFloat("heatHyst", 1));
  setCirculHyst(prefs->getFloat("circulHyst", 1));
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

void Actuators::setThermostat(bool state)
{
  thermostat = state;
}

void Actuators::setCirculation(bool state)
{
  circulation = state;
}

void Actuators::setReqHeatTemp(float temp)
{
  reqHeatTemp = temp;
}

void Actuators::setReqCirculDiff(float diff)
{
  reqCirculDiff = diff;
}

void Actuators::setHeatHyst(float hyst)
{
  heatHyst = hyst;
}

void Actuators::setCirculHyst(float hyst)
{
  circulHyst = hyst;
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

bool Actuators::isCirulating()
{
  return circulating;
}