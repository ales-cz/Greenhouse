void sensorsRead() {
  tempFloor = ds18b20.getTempC(ds18b20Floor);
  tempCeiling = ds18b20.getTempC(ds18b20Ceiling);
  ds18b20.requestTemperatures();
  if ((err = dht22.read2(&tempInt, &humInt, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err=");
    Serial.println(err);
  }
  if (lightMeter.measurementReady())
    illumination = lightMeter.readLightLevel();

  // lightMeter autoadjust & display baclight control
  if (illumination < 0) {
    Serial.println("Error condition detected");
  } else if (illumination > 40000.0) {
    // reduce measurement time - needed in direct sun light
    if (lightMeter.setMTreg(32)) {
      Serial.println("Setting MTReg to low value for high light environment");
    } else {
      Serial.println("Error setting MTReg to low value for high light environment");
    }
    lcdBackLight = 255;
  } else if (illumination > 30000.0) {
    lcdBackLight = 225;
  } else if (illumination > 10000.0) {
    lcdBackLight = 200;
  } else if (illumination > 5000.0) {
    lcdBackLight = 175;
  } else if (illumination > 1000.0) {
    lcdBackLight = 150;
  } else if (illumination > 400.0) {
    lcdBackLight = 125;
  } else if (illumination > 200.0) {
    lcdBackLight = 100;
  } else if (illumination > 50.0) {
    lcdBackLight = 75;
  } else if (illumination > 10.0) {
    // typical light environment
    if (lightMeter.setMTreg(69)) {
      Serial.println("Setting MTReg to default value for normal light environment");
    } else {
      Serial.println("Error setting MTReg to default value for normal light environment");
    }
    lcdBackLight = 50;
  } else if (illumination <= 10.0) {
    // very low light environment
    if (lightMeter.setMTreg(138)) {
      Serial.println("Setting MTReg to high value for low light environment");
    } else {
      Serial.println("Error setting MTReg to high value for low light environment");
    }
    lcdBackLight = 25;
  }
}