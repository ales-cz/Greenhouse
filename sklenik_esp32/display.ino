// utility function for digital clock display
void printDigits(int digits, char ch = ' ') {
  if (digits < 10)
    lcd.print('0');
  lcd.print(digits);
  if (ch != ' ')
    lcd.print(ch);
}

// utility function for measured values display
void printValue(float value, int length, int digits, char u1, char u2 = ' ') {
  char lcdBuf[12];
  dtostrf(value, length, digits, lcdBuf);
  lcd.print(lcdBuf);
  lcd.print(' ');
  lcd.print(u1);
  if (u2 != ' ')
    lcd.print(u2);
}

void display() {
  ledcWrite(LED_CHANNEL, lcdBackLight);
  
  lcd.setCursor(30, 22);
  myRTC.read(tm);
  printDigits(tm.Hour, ':');
  printDigits(tm.Minute, ':');
  printDigits(tm.Second);
  
  if (!(isnan(humInt) || isnan(tempInt))) {
    lcd.setCursor(30, 52);
    printValue(tempInt, 5, 1, char(247), 'C');
    lcd.setCursor(30, 82);
    printValue(humInt, 5, 1, '%');
  }

  if (!(illumination < 0)) {
    lcd.setCursor(30, 112);
    printValue(illumination, 5, 0, 'l', 'x');
  }

  if (!(isnan(humExt) || isnan(tempExt))) {
    lcd.setCursor(30, 142);
    printValue(tempExt, 5, 1, char(247), 'C');
    lcd.setCursor(30, 172);
    printValue(humExt, 5, 1, '%');
  }

  if (tempCeiling != DEVICE_DISCONNECTED_C) {
    lcd.setCursor(30, 202);
    printValue(tempCeiling, 5, 1, char(247), 'C');
  } else {
    Serial.println("Error: Could not read temperature data");
  }

  if (tempFloor != DEVICE_DISCONNECTED_C) {
    lcd.setCursor(30, 232);
    printValue(tempFloor, 5, 1, char(247), 'C');
  } else {
    Serial.println("Error: Could not read temperature data");
  }
  /*
  lcd.setCursor(0, 242);
  lcd.print("Vytapeni");
  lcd.setCursor(0, 272);
  lcd.print("   ");
  lcd.setCursor(0, 272);
  lcd.print(lcdBackLight);//"Cirkulace");
  */
}