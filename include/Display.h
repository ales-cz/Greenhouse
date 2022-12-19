#ifndef MY_DISPLAY_H
#define MY_DISPLAY_H
#include <Arduino.h>
#include <SPI.h>                   // SPI bus
#include <Adafruit_GFX.h>          // LCD display
#include <Adafruit_ILI9341.h>      // LCD display
#include <XPT2046_Touchscreen.h>   // touch driver for a TFT display
#include "Adafruit_ILI9341_Menu.h" // Menu
#include <Fonts/FreeSans12pt7b.h>
#include <SPIFFS_ImageReader.h> // Load images from SPIFFS partition for Adafruit_GFX
#include "Actuators.h"
#include "TimeSyncNTP.h"

// main app colors
#define BACKGROUND 0xFFFF // white
#define TEXTCOLOR 0x0000  // black

// menu colors
#define MENU_TEXT 0X0000          // Black
#define MENU_BACKGROUND 0XFFFF    // White
#define MENU_HIGHLIGHTTEXT 0X0000 // Black
#define MENU_HIGHLIGHT 0x07E9     // Lt Green
#define MENU_HIGHBORDER 0X0000    // Black
#define MENU_SELECTTEXT 0X0000    // Black
#define MENU_SELECT 0x07E9        // Lt Green
#define MENU_SELECTBORDER 0X0000  // Black
#define MENU_DISABLE 0XC618       // Grey
#define TITLE_TEXT 0X0000         // Black
#define TITLE_BACK 0x07E9         // Lt Green

// main app layout
#define CANVAS_X1 65 // temperature & humidity
#define CANVAS_X2 95 // clock & illumination
#define CANVAS_Y 20
#define GRID_X1 5 // position of display grid
#define GRID_X2 140
#define GRID_Y1 27
#define GRID_Y2 57
#define GRID_Y3 87
#define GRID_Y4 117
#define GRID_Y5 147
#define GRID_Y6 177
#define GRID_Y7 207
#define STAT_X 269 // position of status icons
#define STAT_Y1 5
#define STAT_Y2 65
#define STAT_Y3 125
#define STAT_Y4 185

// main app buffer length
#define LCD_BUF_LEN 8

// menu auxiliary constants
#define TOUCH_TRESHOLD 1000
#define INCREMENT_DELAY 150
#define ROW_HEIGHT 40
#define ROWS 3
#define DATA_COLUMN 230
#define TITLE_BAR_WIDTH 320
#define TITLE_BAR_HEIGHT 40
#define TITLE_TOP_MARGIN 28
#define ITEM_LEFT_MARGIN 10
#define ITEM_TOP_MARGIN 28
#define ITEM_MENU_MARGIN 15
#define BAR_LEFT_MARGIN 10
#define BAR_WIDTH 320
#define BAR_BORDER_RADIUS 4
#define BAR_BORDER_THICKNESS 1
#define FONT_TITLE FreeSans12pt7b // font for all headings
#define FONT_ITEM FreeSans12pt7b  // font for menus

// display backlight PWM
#define LED_FREQ 10000
#define LED_CHANNEL 0
#define LED_RESOLUTION 8

class Display
{

private:
  Actuators *actuators;
  TimeSyncNTP *timeSyncNTP;
  SPIClass spiBus;
  Adafruit_ILI9341 tft;
  GFXcanvas1 canvas1;
  GFXcanvas1 canvas2;
  XPT2046_Touchscreen ts;
  SPIFFS_ImageReader imgReader;
  byte lcdBackLight = 100; // LCD backlight
  char lcdBuf[LCD_BUF_LEN];
  bool lastHeatOn = 0, lastHeatActive = 0, lastCirculOn = 0, lastCirculActive = 0;
  byte lastCloudUpdateStatus = 0, lastLANStatus = 0;
  time_t lastDrawClock;

  // nenu variables
  int btnX, btnY; // global x y for touch stuff

  // variables for each menu item
  int mainMenuHeat = 0, mainMenuCircul = 0, mainMenuTime = 0;
  int menuThermostat = 0, menuReqHeatTemp = 0, menuHeatHyst = 0;
  int menuCirculation = 0, menuReqCirculDiff = 0, menuCirculHyst = 0;
  int menuTimeZone = 0, menuTimeDLS = 0;

  const char *OffOnItems[2] = {"Off", "On"};

  ItemMenu mainMenu;
  EditMenu heatMenu;
  EditMenu circulMenu;
  EditMenu timeMenu;

  TS_Point touchPoint;

  void printDigits(int digits, int i);
  void printValue(int x, int y, float value, int digits);
  bool tsTouched();
  int procBtnPress(ItemMenu &theMenu);
  int procBtnPress(EditMenu &theMenu);
  void procHeatMenu();
  void procCirculMenu();
  void procTimeMenu();
  int procMainMenu();

public:
  Display();
  void begin(Actuators *actuators, TimeSyncNTP *timeSyncNTP);
  void init();
  bool drawClock();
  void drawMeasured(float tempInt, float tempExt, float tempFloor, float tempCeiling,
                    float humInt, float humExt, float illum);
  void drawHeat(bool on, bool active);
  void drawCircul(bool on, bool active);
  void drawLAN(byte status, bool force = false);
  void drawCloud(byte status, bool force = false);
  void drawMenu();
};
#endif