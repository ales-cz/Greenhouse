void displayInit() {
  lcd.setRotation(1);
  lcd.fillScreen(BACKGROUND);
  lcd.setTextSize(TEXTSIZE);
  lcd.setTextColor(TEXTCOLOR);
  lcd.setFont(&FreeSans12pt7b);
  canvas1.setFont(&FreeSans12pt7b);
  canvas2.setFont(&FreeSans12pt7b);

  lcd.drawRoundRect(GRID_X1 - 2, GRID_Y1, STAT_X - 6, GRID_Y3, 6, TEXTCOLOR);
  lcd.drawRoundRect(GRID_X1 - 2, GRID_Y4, STAT_X - 6, GRID_Y2, 6, TEXTCOLOR);
  lcd.drawRoundRect(GRID_X1 - 2, GRID_Y6, STAT_X - 6, GRID_Y2, 6, TEXTCOLOR);

  lcd.setCursor(GRID_X1, GRID_Y1 + CANVAS_Y);
  lcd.print("Greenhouse");

  reader.drawBMP("/temp.bmp", lcd, GRID_X1, GRID_Y2);
  lcd.drawCircle(GRID_X1 + 98, GRID_Y2 + 6, 3, TEXTCOLOR);
  lcd.setCursor(GRID_X1 + 103, GRID_Y2 + CANVAS_Y);
  lcd.print('C');

  reader.drawBMP("/hum.bmp", lcd, GRID_X2, GRID_Y2);
  lcd.setCursor(GRID_X2 + 95, GRID_Y2 + CANVAS_Y);
  lcd.print('%');

  reader.drawBMP("/illum.bmp", lcd, GRID_X1, GRID_Y3);
  lcd.setCursor(GRID_X1 + 125, GRID_Y3 + CANVAS_Y);
  lcd.print("lx");

  lcd.setCursor(GRID_X1, GRID_Y4 + CANVAS_Y);
  lcd.print("Outside");

  reader.drawBMP("/temp.bmp", lcd, GRID_X1, GRID_Y5);
  lcd.drawCircle(GRID_X1 + 98, GRID_Y5 + 6, 3, TEXTCOLOR);
  lcd.setCursor(GRID_X1 + 103, GRID_Y5 + CANVAS_Y);
  lcd.print('C');

  reader.drawBMP("/hum.bmp", lcd, GRID_X2, GRID_Y5);
  lcd.setCursor(GRID_X2 + 95, GRID_Y5 + CANVAS_Y);
  lcd.print('%');

  lcd.setCursor(GRID_X1, GRID_Y6 + CANVAS_Y);
  lcd.print("Ceiling");
  lcd.setCursor(GRID_X2, GRID_Y6 + CANVAS_Y);
  lcd.print("Floor");

  reader.drawBMP("/temp.bmp", lcd, GRID_X1, GRID_Y7);
  lcd.drawCircle(GRID_X1 + 98, GRID_Y7 + 6, 3, TEXTCOLOR);
  lcd.setCursor(GRID_X1 + 103, GRID_Y7 + CANVAS_Y);
  lcd.print('C');

  reader.drawBMP("/temp.bmp", lcd, GRID_X2, GRID_Y7);
  lcd.drawCircle(GRID_X2 + 98, GRID_Y7 + 6, 3, TEXTCOLOR);
  lcd.setCursor(GRID_X2 + 103, GRID_Y7 + CANVAS_Y);
  lcd.print('C');

  reader.drawBMP("/heat.bmp", lcd, STAT_X, STAT_Y1);
  reader.drawBMP("/circul.bmp", lcd, STAT_X, STAT_Y2);
  reader.drawBMP("/lan.bmp", lcd, STAT_X, STAT_Y3);
  reader.drawBMP("/cloud.bmp", lcd, STAT_X, STAT_Y4);
}