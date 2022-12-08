#include "Settings.h"
#include "Sensors.h"

Sensors::Sensors()
    : dht22Int(DHT22INT, DHTTYPE), dht22Ext(DHT22EXT, DHTTYPE), owBus(OW), ds18b20(&owBus)
{
}

void Sensors::begin()
{
  dht22Int.begin();
  dht22Ext.begin();
  lastRequest = millis();

  ds18b20.begin();
  ds18b20.setResolution(ds18b20Floor, DALLAS_RESOLUTION);
  ds18b20.setResolution(ds18b20Ceiling, DALLAS_RESOLUTION);
  ds18b20.setWaitForConversion(false);
  ds18b20.requestTemperatures();

  // Wire.begin (already initialized by DS3232RTC)
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
    log_e("Error initialising illumination sensor BH1750");
}

bool Sensors::read(float *tempInt, float *tempExt, float *tempFloor, float *tempCeiling,
                   float *humInt, float *humExt, float *illum)
{
  if (millis() - lastRequest >= DELAY_REQUEST)
  {
    *tempInt = dht22Int.readTemperature();
    *humInt = dht22Int.readHumidity();

    *tempExt = dht22Ext.readTemperature();
    *humExt = dht22Ext.readHumidity();

    *tempFloor = ds18b20.getTempC(ds18b20Floor);
    if (*tempFloor == -127)
      *tempFloor = std::numeric_limits<float>::quiet_NaN();
    *tempCeiling = ds18b20.getTempC(ds18b20Ceiling);
    if (*tempCeiling == -127)
      *tempCeiling = std::numeric_limits<float>::quiet_NaN();

    ds18b20.requestTemperatures();

    if (lightMeter.measurementReady())
      *illum = lightMeter.readLightLevel();

    // lightMeter autoadjust
    if (*illum < 0)
      log_e("Error reading illumination sensor BH1750");
    else if (trunc(*illum) <= 10.0)
    {
      // very low light environment
      if (!lightMeter.setMTreg(138))
        log_e("Error setting illumination sensor BH1750 (MTReg = 138)");
    }
    else if (trunc(*illum) > 10.0 && *illum <= 40000.0)
    {
      // typical light environment
      if (!lightMeter.setMTreg(69))
        log_e("Error setting illumination sensor BH1750 (MTReg = 69)");
    }
    else if (trunc(*illum) > 40000.0)
    {
      // Reduce measurement time - needed in direct sun light
      if (!lightMeter.setMTreg(32))
        log_e("Error setting illumination sensor BH1750 (MTReg = 32)");
    }

    lastRequest = millis();
    return true;
  }
  else
    return false;
}