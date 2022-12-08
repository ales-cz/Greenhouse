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

void Display::procTouch()
{
  touchPoint = ts.getPoint();

  btnX = touchPoint.x;
  btnY = touchPoint.y;

  btnX = map(btnX, 3720, 150, 319, 0);
  btnY = map(btnY, 3850, 250, 239, 0);
}

int Display::procBtnPress(ItemMenu &theMenu)
{
  int theItem = 0, newItem = 0;

  procTouch();
  theItem = theMenu.press(btnX, btnY);

  if (theItem == -1)
    return -1;

  theMenu.drawRow(theItem, BUTTON_PRESSED);

  delay(DEBOUNCE);

  while (ts.touched())
  {
    procTouch();
    newItem = theMenu.press(btnX, btnY);
    if (theItem != newItem)
    {
      theMenu.drawRow(theItem, BUTTON_NOTPRESSED);
      return -1;
    }
  }
  if (theItem == newItem)
  {
    theMenu.drawRow(theItem, BUTTON_NOTPRESSED);
    return theItem;
  }
  theMenu.drawRow(theItem, BUTTON_NOTPRESSED);
  return -1;
}

int Display::procBtnPress(EditMenu &theMenu)
{

  int theItem = 0, newItem = 0;

  procTouch();
  theItem = theMenu.press(btnX, btnY);

  if (theItem == -1)
    return -1;

  theMenu.drawRow(theItem);

  delay(DEBOUNCE);

  while (ts.touched())
  {
    procTouch();
    newItem = theMenu.press(btnX, btnY);
    if (theItem != newItem)
    {
      theMenu.drawRow(theItem);
      return -1;
    }
  }
  if (theItem == newItem)
  {
    theMenu.drawRow(theItem);
    return theItem;
  }
  return theItem;
}

void Display::procHeatMenu()
{
  int editMenuOption = 1;
  bool thermostat;
  float reqHeatTemp, heatHyst;

  tft.fillScreen(MENU_BACKGROUND);
  heatMenu.draw();

  while (editMenuOption != 0)
  {
    if (ts.touched())
      editMenuOption = procBtnPress(heatMenu);
  }

  // process the menu selections
  thermostat = (bool)heatMenu.value[menuThermostat];
  if (thermostat != actuators->getThermostat())
    actuators->setThermostat(thermostat);

  reqHeatTemp = heatMenu.value[menuReqHeatTemp];
  if (reqHeatTemp != actuators->getReqHeatTemp())
    actuators->setReqHeatTemp(reqHeatTemp);

  heatHyst = heatMenu.value[menuHeatHyst];
  if (heatHyst != actuators->getHeatHyst())
    actuators->setHeatHyst(heatHyst);
}

void Display::procCirculMenu()
{
  int editMenuOption = 1;
  bool circulation;
  float reqCirculDiff, circulHyst;

  tft.fillScreen(MENU_BACKGROUND);
  circulMenu.draw();

  while (editMenuOption != 0)
  {
    if (ts.touched())
      editMenuOption = procBtnPress(circulMenu);
  }

  // process the menu selections
  circulation = (bool)circulMenu.value[menuCirculation];
  if (circulation != actuators->getCirculation())
    actuators->setCirculation(circulation);

  reqCirculDiff = circulMenu.value[menuReqCirculDiff];
  if (reqCirculDiff != actuators->getReqCirculDiff())
    actuators->setReqCirculDiff(reqCirculDiff);

  circulHyst = circulMenu.value[menuCirculHyst];
  if (circulHyst != actuators->getCirculHyst())
    actuators->setCirculHyst(circulHyst);
}

void Display::procTimeMenu()
{
  int editMenuOption = 1;
  byte timeZone, timeDLS;

  tft.fillScreen(MENU_BACKGROUND);
  timeMenu.draw();

  while (editMenuOption != 0)
  {
    if (ts.touched())
      editMenuOption = procBtnPress(timeMenu);
  }

  // process the menu selections
  timeZone = (byte)timeMenu.value[menuTimeZone];
  if (timeZone != timeSyncNTP->getTimeZone())
    timeSyncNTP->setTimeZone(timeZone);

  timeDLS = (byte)timeMenu.value[menuTimeDLS];
  if (timeDLS != timeSyncNTP->getTimeDLS())
    timeSyncNTP->setTimeDLS(timeDLS);
}

int Display::procMainMenu()
{
  int mainMenuOption = 1;

  delay(DEBOUNCE);
  if (ts.touched())
  {
    esp_task_wdt_delete(NULL);

    tft.fillScreen(MENU_BACKGROUND);
    mainMenu.draw();

    while (mainMenuOption != 0)
    {
      if (ts.touched())
      {
        mainMenuOption = procBtnPress(mainMenu);

        if (mainMenuOption == mainMenuHeat)
        {
          procHeatMenu();
          tft.fillScreen(MENU_BACKGROUND);
          mainMenu.draw();
        }
        else if (mainMenuOption == mainMenuCircul)
        {
          procCirculMenu();
          tft.fillScreen(MENU_BACKGROUND);
          mainMenu.draw();
        }
        else if (mainMenuOption == mainMenuTime)
        {
          procTimeMenu();
          tft.fillScreen(MENU_BACKGROUND);
          mainMenu.draw();
        }
      }
    }
    esp_task_wdt_add(NULL);
    init();
    return mainMenuOption;
  }
  return -1;
}

Display::Display()
    : spiBus(HSPI),
      tft(&spiBus, TFT_DC, TFT_CS, TFT_RST),
      ts(TS_CS),
      canvas1(CANVAS_X1, CANVAS_Y),
      canvas2(CANVAS_X2, CANVAS_Y),
      mainMenu(&tft, true),
      heatMenu(&tft, true),
      circulMenu(&tft, true),
      timeMenu(&tft, true)
{
  // configure display backlight PWM
  ledcSetup(LED_CHANNEL, LED_FREQ, LED_RESOLUTION);
  ledcAttachPin(TFT_BL, LED_CHANNEL);
}

void Display::begin(Actuators *actuators, TimeSyncNTP *timeSyncNTP)
{
  this->actuators = actuators;
  this->timeSyncNTP = timeSyncNTP;

  spiBus.begin(); // Display

  tft.begin();
  tft.setRotation(1);

  ts.begin(spiBus);
  ts.setRotation(3);

  // initialize SPIFFS
  if (!SPIFFS.begin())
    log_e("SPIFFS initialisation failed!");

  mainMenu.init(MENU_TEXT, MENU_BACKGROUND, MENU_HIGHLIGHTTEXT, MENU_HIGHLIGHT, ROW_HEIGHT, ROWS, "Settings", FONT_ITEM, FONT_TITLE);

  mainMenuHeat = mainMenu.addNI("Heating");
  mainMenuCircul = mainMenu.addNI("Circulation");
  mainMenuTime = mainMenu.addNI("Time");

  mainMenu.setTitleColors(TITLE_TEXT, TITLE_BACK);
  mainMenu.setTitleBarSize(0, 0, TITLE_BAR_WIDTH, TITLE_BAR_HEIGHT);
  mainMenu.setTitleTextMargins(115, TITLE_TOP_MARGIN);
  mainMenu.setItemTextMargins(ITEM_LEFT_MARGIN, ITEM_TOP_MARGIN, ITEM_MENU_MARGIN);
  mainMenu.setMenuBarMargins(BAR_LEFT_MARGIN, BAR_WIDTH, BAR_BORDER_RADIUS, BAR_BORDER_THICKNESS);

  heatMenu.init(MENU_TEXT, MENU_BACKGROUND, MENU_HIGHLIGHTTEXT, MENU_HIGHLIGHT, MENU_SELECTTEXT, MENU_SELECT, DATA_COLUMN,
                ROW_HEIGHT, ROWS, "Heating", FONT_ITEM, FONT_TITLE);

  menuThermostat = heatMenu.addNI("Heating", actuators->getThermostat(), 0, sizeof(OffOnItems) / sizeof(OffOnItems[0]), 1, 0, OffOnItems);
  menuReqHeatTemp = heatMenu.addNI("Temperature", actuators->getReqHeatTemp(), 0, 30, .5, 1, NULL);
  menuHeatHyst = heatMenu.addNI("Hystheresis", actuators->getHeatHyst(), 0, 5, .5, 1, NULL);

  heatMenu.setTitleColors(TITLE_TEXT, TITLE_BACK);
  heatMenu.setTitleBarSize(0, 0, TITLE_BAR_WIDTH, TITLE_BAR_HEIGHT);
  heatMenu.setTitleTextMargins(115, TITLE_TOP_MARGIN);
  heatMenu.setItemTextMargins(ITEM_LEFT_MARGIN, ITEM_TOP_MARGIN, ITEM_MENU_MARGIN);
  heatMenu.setMenuBarMargins(BAR_LEFT_MARGIN, BAR_WIDTH, BAR_BORDER_RADIUS, BAR_BORDER_THICKNESS);
  heatMenu.setItemColors(MENU_DISABLE, MENU_HIGHBORDER, MENU_SELECTBORDER);
  heatMenu.setIncrementDelay(INCREMENT_DELAY);

  circulMenu.init(MENU_TEXT, MENU_BACKGROUND, MENU_HIGHLIGHTTEXT, MENU_HIGHLIGHT, MENU_SELECTTEXT, MENU_SELECT, DATA_COLUMN,
                  ROW_HEIGHT, ROWS, "Circulation", FONT_ITEM, FONT_TITLE);

  menuCirculation = circulMenu.addNI("Circulation", actuators->getCirculation(), 0, sizeof(OffOnItems) / sizeof(OffOnItems[0]), 1, 0, OffOnItems);
  menuReqCirculDiff = circulMenu.addNI("Difference", actuators->getReqCirculDiff(), 0, 15, 0.5, 1, NULL);
  menuCirculHyst = circulMenu.addNI("Hystheresis", actuators->getCirculHyst(), 0, 15, 0.5, 1, NULL);

  circulMenu.setTitleColors(TITLE_TEXT, TITLE_BACK);
  circulMenu.setTitleBarSize(0, 0, TITLE_BAR_WIDTH, TITLE_BAR_HEIGHT);
  circulMenu.setTitleTextMargins(100, TITLE_TOP_MARGIN);
  circulMenu.setItemTextMargins(ITEM_LEFT_MARGIN, ITEM_TOP_MARGIN, ITEM_MENU_MARGIN);
  circulMenu.setMenuBarMargins(BAR_LEFT_MARGIN, BAR_WIDTH, BAR_BORDER_RADIUS, BAR_BORDER_THICKNESS);
  circulMenu.setItemColors(MENU_DISABLE, MENU_HIGHBORDER, MENU_SELECTBORDER);
  circulMenu.setIncrementDelay(INCREMENT_DELAY);

  timeMenu.init(MENU_TEXT, MENU_BACKGROUND, MENU_HIGHLIGHTTEXT, MENU_HIGHLIGHT, MENU_SELECTTEXT, MENU_SELECT, DATA_COLUMN,
                ROW_HEIGHT, ROWS, "Time", FONT_ITEM, FONT_TITLE);

  menuTimeDLS = timeMenu.addNI("Daylight saving", timeSyncNTP->getTimeDLS(), 0, 1, 1, 0, NULL);
  menuTimeZone = timeMenu.addNI("Time zone", timeSyncNTP->getTimeZone(), -12, 12, 1, 0);

  timeMenu.setTitleColors(TITLE_TEXT, TITLE_BACK);
  timeMenu.setTitleBarSize(0, 0, TITLE_BAR_WIDTH, TITLE_BAR_HEIGHT);
  timeMenu.setTitleTextMargins(132, TITLE_TOP_MARGIN);
  timeMenu.setItemTextMargins(ITEM_LEFT_MARGIN, ITEM_TOP_MARGIN, ITEM_MENU_MARGIN);
  timeMenu.setMenuBarMargins(BAR_LEFT_MARGIN, BAR_WIDTH, BAR_BORDER_RADIUS, BAR_BORDER_THICKNESS);
  timeMenu.setItemColors(MENU_DISABLE, MENU_HIGHBORDER, MENU_SELECTBORDER);
  timeMenu.setIncrementDelay(INCREMENT_DELAY);

  init();
}

void Display::init()
{
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

  drawLAN(LinkON, true);
  drawCloud(200, true);
}

bool Display::drawClock()
{
  if (ts.touched())
    procMainMenu();
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

void Display::drawLAN(byte status, bool force)
{
  if (status != lastLANStatus || force)
  {
    if (status == LinkON)
      imgReader.drawBMP((char *)"/lan.bmp", tft, STAT_X, STAT_Y3);
    else
      imgReader.drawBMP((char *)"/ylan.bmp", tft, STAT_X, STAT_Y3);
  }
  lastLANStatus = status;
}

void Display::drawCloud(byte status, bool force)
{
  if (status != lastCloudUpdateStatus || force)
  {
    if (status == 200)
      imgReader.drawBMP((char *)"/cloud.bmp", tft, STAT_X, STAT_Y4);
    else
      imgReader.drawBMP((char *)"/ycloud.bmp", tft, STAT_X, STAT_Y4);
  }
  lastCloudUpdateStatus = status;
}
