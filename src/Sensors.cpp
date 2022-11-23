#include "Sensors.h"

Sensors::Sensors(int8_t pDHT22_1, int8_t pOW)
    : dht22_1(pDHT22_1), owBus(pOW), ds18b20(&owBus)
{
}

void Sensors::begin()
{
  ds18b20.begin();
  ds18b20.setResolution(ds18b20Floor, DALLAS_RESOLUTION);
  ds18b20.setResolution(ds18b20Ceiling, DALLAS_RESOLUTION);
  ds18b20.setWaitForConversion(false);
  ds18b20.requestTemperatures();

  Wire.begin();
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
  {
    Serial.println("BH1750 begin");
  }
  else
  {
    Serial.println("Error initialising BH1750");
  }
}

void Sensors::read()
{
  int err = SimpleDHTErrSuccess;

  tempFloor = ds18b20.getTempC(ds18b20Floor);
  tempCeiling = ds18b20.getTempC(ds18b20Ceiling);
  ds18b20.requestTemperatures();
  if ((err = dht22_1.read2(&tempInt, &humInt, NULL)) != SimpleDHTErrSuccess)
  {
    Serial.print("Read DHT22 failed, err=");
    Serial.println(err);
  }
  if (lightMeter.measurementReady())
    illum = lightMeter.readLightLevel();

  // lightMeter autoadjust & display baclight control
  if (illum < 0)
  {
    Serial.println("Error condition detected");
  }
  else if (illum > 40000.0)
  {
    // reduce measurement time - needed in direct sun light
    if (lightMeter.setMTreg(32))
    {
      Serial.println("Setting MTReg to low value for high light environment");
    }
    else
    {
      Serial.println("Error setting MTReg to low value for high light environment");
    }
    //lcdBackLight = 255;
  }
  else if (illum > 30000.0)
  {
    //lcdBackLight = 225;
  }
  else if (illum > 10000.0)
  {
    //lcdBackLight = 200;
  }
  else if (illum > 5000.0)
  {
    //lcdBackLight = 175;
  }
  else if (illum > 1000.0)
  {
    //lcdBackLight = 150;
  }
  else if (illum > 400.0)
  {
    //lcdBackLight = 125;
  }
  else if (illum > 200.0)
  {
    //lcdBackLight = 100;
  }
  else if (illum > 50.0)
  {
    //lcdBackLight = 75;
  }
  else if (illum > 10.0)
  {
    // typical light environment
    if (lightMeter.setMTreg(69))
    {
      Serial.println("Setting MTReg to default value for normal light environment");
    }
    else
    {
      Serial.println("Error setting MTReg to default value for normal light environment");
    }
    //lcdBackLight = 50;
  }
  else if (illum <= 10.0)
  {
    // very low light environment
    if (lightMeter.setMTreg(138))
    {
      Serial.println("Setting MTReg to high value for low light environment");
    }
    else
    {
      Serial.println("Error setting MTReg to high value for low light environment");
    }
    //lcdBackLight = 25;
  }
}