// utility function for digital clock display
void printDigits(int digits, int i) {
  lcdBuf[i] = '0' + digits / 10;
  lcdBuf[i + 1] = '0' + digits % 10;
  if (i < 6)
    lcdBuf[i + 2] = ':';
}

// utility function for measured values display
void printValue(int x, int y, float value, int digits) {
  int16_t x1, y1;
  uint16_t w, h;

  dtostrf(value, 0, digits, lcdBuf);
  lcd.getTextBounds(lcdBuf, 0, CANVAS_Y - 3, &x1, &y1, &w, &h); // text buffer, position, top left corner, width & height
  canvas1.fillScreen(0);
  canvas1.setCursor(CANVAS_X1 - w - 5, CANVAS_Y - 3);
  canvas1.print(lcdBuf);
  lcd.drawBitmap(x, y + 2, canvas1.getBuffer(), CANVAS_X1, CANVAS_Y, TEXTCOLOR, BACKGROUND);
}

void display() {
  int16_t x1, y1;
  uint16_t w, h;

  ledcWrite(LED_CHANNEL, lcdBackLight);
  
  printDigits(hour(), 0);
  printDigits(minute(), 3);
  printDigits(second(), 6);
  canvas2.fillScreen(0);
  canvas2.setCursor(0, CANVAS_Y - 3);
  canvas2.print(lcdBuf);
  lcd.drawBitmap(113, 4, canvas2.getBuffer(), CANVAS_X2, CANVAS_Y, TEXTCOLOR, BACKGROUND);
  
  if (!(isnan(humInt) || isnan(tempInt))) {
    printValue(GRID_X1 + 25, GRID_Y2, tempInt, 1);
    printValue(GRID_X2 + 25, GRID_Y2, humInt, 1);
  }

  if (!(illum < 0)) {
    dtostrf(illum, 0, 1, lcdBuf);
    lcd.getTextBounds(lcdBuf, 0, CANVAS_Y - 3, &x1, &y1, &w, &h); // text buffer, position, top left corner, width & height
    canvas2.fillScreen(0);
    canvas2.setCursor(CANVAS_X2 - w - 5, CANVAS_Y - 3);
    canvas2.print(lcdBuf);
    lcd.drawBitmap(GRID_X1 + 25, GRID_Y3 + 2, canvas2.getBuffer(), CANVAS_X2, CANVAS_Y, TEXTCOLOR, BACKGROUND);
  }

  if (!(isnan(humExt) || isnan(tempExt))) {
    printValue(GRID_X1 + 25, GRID_Y5, tempExt, 1);
    printValue(GRID_X2 + 25, GRID_Y5, humExt, 1);
  }

  if (tempCeiling != DEVICE_DISCONNECTED_C) {
    printValue(GRID_X1 + 25, GRID_Y7, tempCeiling, 1);
  } else {
    Serial.println("Error: Could not read temperature data");
  }

  if (tempFloor != DEVICE_DISCONNECTED_C) {
    printValue(GRID_X2 + 25, GRID_Y7, tempFloor, 1);
  } else {
    Serial.println("Error: Could not read temperature data");
  }
}