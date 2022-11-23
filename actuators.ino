// Pokud je hystereze 1°C, požadovaná teplota 5°C, termostat vypne při 6°C a znovu sepne při 4°C

void actuators() {
  if (thermostat) {
    if (tempInt < reqHeatTemp - heatHyst) {
      digitalWrite(HEAT, HIGH);
      reader.drawBMP("/gheat.bmp", lcd, STAT_X, STAT_Y1);
      heatOut = 1;
    } else if (tempInt > reqHeatTemp + heatHyst) {
      digitalWrite(HEAT, LOW);
      reader.drawBMP("/heat.bmp", lcd, STAT_X, STAT_Y1);
      heatOut = 0;
    }
  }
  if (circulation) {
    if (tempCeiling - tempFloor > reqCirculDiff + circulHyst) {
      digitalWrite(CIRCUL, HIGH);
      reader.drawBMP("/gcircul.bmp", lcd, STAT_X, STAT_Y2);
      circulOut = 1;
    } else if (tempCeiling - tempFloor < reqCirculDiff - circulHyst) {
      digitalWrite(CIRCUL, LOW);
      reader.drawBMP("/circul.bmp", lcd, STAT_X, STAT_Y2);
      circulOut = 0;
    }
  }
}