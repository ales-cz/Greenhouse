#include "Settings.h"
#include "Sensors.h"

Sensors::Sensors(int8_t pDHT22_1, int8_t pOW)
    : owBus(pOW), ds18b20(&owBus), dht22_1(pDHT22_1)
{
}

void Sensors::begin()
{
  ds18b20.begin();
  ds18b20.setResolution(ds18b20Floor, DALLAS_RESOLUTION);
  ds18b20.setResolution(ds18b20Ceiling, DALLAS_RESOLUTION);
  ds18b20.setWaitForConversion(false);
  lastRequest = millis();
  ds18b20.requestTemperatures();

  Wire.begin();
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
    log_e("Error initialising illumination sensor BH1750");
}

bool Sensors::read(float *tempInt, float *tempExt, float *tempFloor, float *tempCeiling,
                   float *humInt, float *humExt, float *illum)
{
  if (millis() - lastRequest < DELAY_REQUEST)
    return false;

  lastRequest = millis();

  int err = SimpleDHTErrSuccess;
  if ((err = dht22_1.read2(tempInt, humInt, NULL)) != SimpleDHTErrSuccess)
  {
    log_e("Error reading temperature & humidity sensor DHT22, err: %d,%d",
          SimpleDHTErrCode(err), SimpleDHTErrDuration(err));
    *tempInt = std::numeric_limits<float>::quiet_NaN();
    *humInt = std::numeric_limits<float>::quiet_NaN();
  }

  *tempFloor = ds18b20.getTempC(ds18b20Floor);
  if (*tempFloor = -127)
    *tempFloor = std::numeric_limits<float>::quiet_NaN();
  *tempCeiling = ds18b20.getTempC(ds18b20Ceiling);
  if (*tempCeiling = -127)
    *tempCeiling = std::numeric_limits<float>::quiet_NaN();

  ds18b20.requestTemperatures();

  if (lightMeter.measurementReady())
    *illum = lightMeter.readLightLevel();

  // lightMeter autoadjust
  if (*illum < 0)
    log_e("Error reading illumination sensor BH1750");
  else if (*illum <= 10.0)
  {
    // very low light environment
    if (!lightMeter.setMTreg(138))
      log_i("Error setting illumination sensor BH1750 MTReg to high value (low light environment)");
  }
  else if (*illum > 11.0 && *illum < 39000.0)
  {
    // typical light environment
    if (!lightMeter.setMTreg(69))
      log_i("Error setting illumination sensor BH1750 MTReg to default value (normal light environment)");
  }
  else if (*illum >= 40000.0)
  {
    // Reduce measurement time - needed in direct sun light
    if (!lightMeter.setMTreg(32))
      log_i("Error setting illumination sensor BH1750 MTReg to low value (high light environment)");
  }

  return true;
}