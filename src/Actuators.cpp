#include "Actuators.h"

Actuators::Actuators(int8_t pHeat, int8_t pCircul)
{
  this->pHeat = pHeat;
  this->pCircul = pCircul;
  pinMode(this->pHeat, OUTPUT);
  pinMode(this->pCircul, OUTPUT);
}

// Pokud je hystereze 1°C, požadovaná teplota 5°C, termostat vypne při 6°C a znovu sepne při 4°C
void Actuators::set(float tempInt, float tempFloor, float tempCeiling) {
  if (thermostat) {
    if (tempInt < reqHeatTemp - heatHyst) {
      setHeat(1);
    } else if (tempInt > reqHeatTemp + heatHyst) {
      setHeat(0);
    }
  }
  if (circulation) {
    if (tempCeiling - tempFloor > reqCirculDiff + circulHyst) {
      setCircul(1);
    } else if (tempCeiling - tempFloor < reqCirculDiff - circulHyst) {
      setCircul(0);
    }
  }
}

void Actuators::setHeat(bool active) {
  digitalWrite(pHeat, active);
  heating = active;
}

void Actuators::setCircul(bool active) {
  digitalWrite(pCircul, active);
  circulating = active;
}