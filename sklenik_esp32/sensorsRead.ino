void sensorsRead() {
  tempFloor = ds18b20.getTempC(ds18b20Floor);
  tempCeiling = ds18b20.getTempC(ds18b20Ceiling);
  ds18b20.requestTemperatures();
  if ((err = dht22.read2(&tempInt, &humInt, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err=");
    Serial.println(err);
  }
  illumination = lightMeter.readLightLevel();
}