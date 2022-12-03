#include "Settings.h"
#include "Display.h"

// utility function for digital clock display
void Display::printDigits(int digits, int i)
{
  lcdBuf[i] = '0' + digits / 10;
  lcdBuf[i + 1] = '0' + digits % 10;
  if (i < 6)
    lcdBuf[i + 2] = ':';
}

// utility function for measured values display
void Display::printValue(int x, int y, float value, int digits)
{
  int16_t x1, y1;
  uint16_t w, h;
  if (!(isnan(value)))
    dtostrf(value, 0, digits, lcdBuf);
  else
    strncpy(lcdBuf, "---", LCD_BUF_LEN);
  tft.getTextBounds(lcdBuf, 0, CANVAS_Y - 3, &x1, &y1, &w, &h); // text buffer, position, top left corner, width & height
  canvas1.fillScreen(0);
  canvas1.setCursor(CANVAS_X1 - w - 5, CANVAS_Y - 3);
  canvas1.print(lcdBuf);
  tft.drawBitmap(x, y + 2, canvas1.getBuffer(), CANVAS_X1, CANVAS_Y, TEXTCOLOR, BACKGROUND);
}

Display::Display(int8_t tftDC, int8_t hspiSS, int8_t tftRST, byte tftBL)
    : spiBus(HSPI), tft(&spiBus, tftDC, hspiSS, tftRST), canvas1(CANVAS_X1, CANVAS_Y), canvas2(CANVAS_X2, CANVAS_Y)
{
  // configure display backlight PWM
  ledcSetup(LED_CHANNEL, LED_FREQ, LED_RESOLUTION);
  ledcAttachPin(tftBL, LED_CHANNEL);
}

void Display::begin()
{
  spiBus.begin(); // Display
  pinMode(HSPI_SS, OUTPUT);
  tft.begin();
  // initialize SPIFFS
  if (!SPIFFS.begin())
    log_e("SPIFFS initialisation failed!");
  init();
}

void Display::init()
{
  tft.setRotation(1);
  tft.fillScreen(BACKGROUND);
  tft.setTextSize(1);
  tft.setTextColor(TEXTCOLOR);
  tft.setFont(&FreeSans12pt7b);
  canvas1.setFont(&FreeSans12pt7b);
  canvas2.setFont(&FreeSans12pt7b);
  // Frames
  tft.drawRoundRect(GRID_X1 - 2, GRID_Y1, STAT_X - 6, GRID_Y3, 6, TEXTCOLOR);
  tft.drawRoundRect(GRID_X1 - 2, GRID_Y4, STAT_X - 6, GRID_Y2, 6, TEXTCOLOR);
  tft.drawRoundRect(GRID_X1 - 2, GRID_Y6, STAT_X - 6, GRID_Y2, 6, TEXTCOLOR);

  tft.setCursor(GRID_X1, GRID_Y1 + CANVAS_Y);
  tft.print("Greenhouse");

  imgReader.drawBMP((char *)"/temp.bmp", tft, GRID_X1, GRID_Y2);
  tft.drawCircle(GRID_X1 + 98, GRID_Y2 + 6, 3, TEXTCOLOR);
  tft.setCursor(GRID_X1 + 103, GRID_Y2 + CANVAS_Y);
  tft.print('C');

  imgReader.drawBMP((char *)"/hum.bmp", tft, GRID_X2, GRID_Y2);
  tft.setCursor(GRID_X2 + 95, GRID_Y2 + CANVAS_Y);
  tft.print('%');

  imgReader.drawBMP((char *)"/illum.bmp", tft, GRID_X1, GRID_Y3);
  tft.setCursor(GRID_X1 + 125, GRID_Y3 + CANVAS_Y);
  tft.print("lx");

  tft.setCursor(GRID_X1, GRID_Y4 + CANVAS_Y);
  tft.print("Outside");

  imgReader.drawBMP((char *)"/temp.bmp", tft, GRID_X1, GRID_Y5);
  tft.drawCircle(GRID_X1 + 98, GRID_Y5 + 6, 3, TEXTCOLOR);
  tft.setCursor(GRID_X1 + 103, GRID_Y5 + CANVAS_Y);
  tft.print('C');

  imgReader.drawBMP((char *)"/hum.bmp", tft, GRID_X2, GRID_Y5);
  tft.setCursor(GRID_X2 + 95, GRID_Y5 + CANVAS_Y);
  tft.print('%');

  tft.setCursor(GRID_X1, GRID_Y6 + CANVAS_Y);
  tft.print("Ceiling");
  tft.setCursor(GRID_X2, GRID_Y6 + CANVAS_Y);
  tft.print("Floor");

  imgReader.drawBMP((char *)"/temp.bmp", tft, GRID_X1, GRID_Y7);
  tft.drawCircle(GRID_X1 + 98, GRID_Y7 + 6, 3, TEXTCOLOR);
  tft.setCursor(GRID_X1 + 103, GRID_Y7 + CANVAS_Y);
  tft.print('C');

  imgReader.drawBMP((char *)"/temp.bmp", tft, GRID_X2, GRID_Y7);
  tft.drawCircle(GRID_X2 + 98, GRID_Y7 + 6, 3, TEXTCOLOR);
  tft.setCursor(GRID_X2 + 103, GRID_Y7 + CANVAS_Y);
  tft.print('C');

  drawLAN(LinkON);
  drawCloud(200);
}

bool Display::drawClock()
{
  if (now() != lastDrawClock)
  {
    printDigits(hour(), 0);
    printDigits(minute(), 3);
    printDigits(second(), 6);

    canvas2.fillScreen(0);
    canvas2.setCursor(0, CANVAS_Y - 3);
    canvas2.print(lcdBuf);
    tft.drawBitmap(113, 4, canvas2.getBuffer(), CANVAS_X2, CANVAS_Y, TEXTCOLOR, BACKGROUND);

    lastDrawClock = now();
    return true;
  }
  else
    return false;
}

void Display::drawMeasured(float tempInt, float tempExt, float tempFloor, float tempCeiling,
                           float humInt, float humExt, float illum)
{
  int16_t x1, y1;
  uint16_t w, h;

  // Backlight control
  if (illum <= 10.0)
    lcdBackLight = 25;
  else if (illum > 10.0 && illum <= 50)
    lcdBackLight = 50;
  else if (illum > 50.0 && illum <= 200)
    lcdBackLight = 75;
  else if (illum > 200.0 && illum <= 400)
    lcdBackLight = 100;
  else if (illum > 400.0 && illum <= 1000)
    lcdBackLight = 125;
  else if (illum > 1000.0 && illum <= 5000.0)
    lcdBackLight = 150;
  else if (illum > 5000.0 && illum <= 10000.0)
    lcdBackLight = 175;
  else if (illum > 10000.0 && illum <= 30000.0)
    lcdBackLight = 200;
  else if (illum > 30000.0 && illum <= 40000.0)
    lcdBackLight = 225;
  else if (illum > 40000.0)
    lcdBackLight = 255;

  ledcWrite(LED_CHANNEL, lcdBackLight);

  // Greenhouse internal temperature & humidity
  printValue(GRID_X1 + 25, GRID_Y2, tempInt, 1);
  printValue(GRID_X2 + 25, GRID_Y2, humInt, 1);
  // Greenhouse internal illumination
  if (!(illum < 0))
    dtostrf(illum, 0, 1, lcdBuf);
  else
    strncpy(lcdBuf, "---", LCD_BUF_LEN);
  tft.getTextBounds(lcdBuf, 0, CANVAS_Y - 3, &x1, &y1, &w, &h); // text buffer, position, top left corner, width & height
  canvas2.fillScreen(0);
  canvas2.setCursor(CANVAS_X2 - w - 5, CANVAS_Y - 3);
  canvas2.print(lcdBuf);
  tft.drawBitmap(GRID_X1 + 25, GRID_Y3 + 2, canvas2.getBuffer(), CANVAS_X2, CANVAS_Y, TEXTCOLOR, BACKGROUND);

  // Greenhouse external temperature & humidity
  printValue(GRID_X1 + 25, GRID_Y5, tempExt, 1);
  printValue(GRID_X2 + 25, GRID_Y5, humExt, 1);

  // Greenhouse ceiling & floor temperature
  printValue(GRID_X1 + 25, GRID_Y7, tempCeiling, 1);
  printValue(GRID_X2 + 25, GRID_Y7, tempFloor, 1);
}

void Display::drawHeat(bool on, bool active)
{
  if (on)
  {
    if (active)
    {
      if (!lastHeatOn || lastHeatOn && !lastHeatActive)
        imgReader.drawBMP((char *)"/gheat.bmp", tft, STAT_X, STAT_Y1);
    }
    else
    {
      if (!lastHeatOn || lastHeatOn && lastHeatActive)
        imgReader.drawBMP((char *)"/heat.bmp", tft, STAT_X, STAT_Y1);
    }
  }
  else if (lastHeatOn)
    tft.fillRect(STAT_X, STAT_Y1, 48, 48, BACKGROUND);
}

void Display::drawCircul(bool on, bool active)
{
  if (on)
  {
    if (active)
    {
      if (!lastCirculOn || lastCirculOn && !lastCirculActive)
        imgReader.drawBMP((char *)"/gcircul.bmp", tft, STAT_X, STAT_Y2);
    }
    else
    {
      if (!lastCirculOn || lastCirculOn && lastCirculActive)
        imgReader.drawBMP((char *)"/circul.bmp", tft, STAT_X, STAT_Y2);
    }
  }
  else if (lastCirculOn)
    tft.fillRect(STAT_X, STAT_Y2, 48, 48, BACKGROUND);
}

void Display::drawLAN(byte status)
{
  if (status != lastLANStatus)
  {
    if (status == LinkON)
      imgReader.drawBMP((char *)"/lan.bmp", tft, STAT_X, STAT_Y3);
    else
      imgReader.drawBMP((char *)"/ylan.bmp", tft, STAT_X, STAT_Y3);
  }
  lastLANStatus = status;
}

void Display::drawCloud(byte status)
{
  if (status != lastCloudUpdateStatus)
  {
    if (status == 200)
      imgReader.drawBMP((char *)"/cloud.bmp", tft, STAT_X, STAT_Y4);
    else
      imgReader.drawBMP((char *)"/ycloud.bmp", tft, STAT_X, STAT_Y4);
  }
  lastCloudUpdateStatus = status;
}