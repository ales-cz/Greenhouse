void display() {
  if (!(isnan(humInt) || isnan(tempInt))) {
    lcd.setCursor(20, 20);
    lcd.print("Teplota a vlhkost");
    lcd.setCursor(20, 40);
    lcd.print(tempInt, 1);
    lcd.print(" ");
    lcd.print(char(247));
    lcd.print("C  ");
    lcd.print(humInt, 1);
    lcd.print(" %");
  }

  if (tempCeiling != DEVICE_DISCONNECTED_C) {
    lcd.setCursor(20, 60);
    lcd.print("Strop: ");
    lcd.print(tempCeiling, 1);
    lcd.print(" ");
    lcd.print(char(247));
    lcd.print("C");
  } else {
    Serial.println("Error: Could not read temperature data");
  }

  if (tempFloor != DEVICE_DISCONNECTED_C) {
    lcd.setCursor(20, 80);
    lcd.print("Podlaha: ");
    lcd.print(tempFloor, 1);
    lcd.print(" ");
    lcd.print(char(247));
    lcd.print("C  ");
  } else {
    Serial.println("Error: Could not read temperature data");
  }

  lcd.setCursor(20, 100);
  lcd.print("Svetlo: ");
  lcd.print(illumination);
  lcd.println(" lx");

  /*
  char buf[40];
  time_t t = myRTC.get();
  sprintf(buf, "%.2d:%.2d:%.2d %.2d%s%d ",
      hour(t), minute(t), second(t), day(t), monthShortStr(month(t)), year(t));
  Serial.print(buf);
  */

  myRTC.read(tm);
  Serial.print(tm.Hour, DEC);
  Serial.print(':');
  Serial.print(tm.Minute, DEC);
  Serial.print(':');
  Serial.println(tm.Second, DEC);
}

// asi smazat
void printDigits(int digits)
{
    // utility function for digital clock display: prints preceding colon and leading 0
    Serial.print(':');
    if(digits < 10)
        Serial.print('0');
    Serial.print(digits);
}