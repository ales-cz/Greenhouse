#ifndef MY_DISPLAY_H
#define MY_DISPLAY_H
#include <Arduino.h>
#include <Adafruit_ILI9341.h> // LCD display
#include <Adafruit_GFX.h>     // LCD display
#include <Fonts/FreeSans12pt7b.h>
#include <SPI.h>                // SPI bus
#include <SPIFFS_ImageReader.h> // Load images from SPIFFS partition for Adafruit_GFX

#define BACKGROUND 0xFFFF // white
#define TEXTSIZE 1
#define TEXTCOLOR 0x0000 // black
#define LED_FREQ 10000   // display backlight PWM
#define LED_CHANNEL 0    // display backlight PWM
#define LED_RESOLUTION 8 // display backlight PWM
#define CANVAS_X1 65     // temperature & humidity
#define CANVAS_X2 95     // clock & illumination
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

class Display
{

private:
  Adafruit_ILI9341 tft;
  GFXcanvas1 canvas1;
  GFXcanvas1 canvas2;
  SPIFFS_ImageReader imgReader;
  byte lcdBackLight = 100; // LCD backlight
  char lcdBuf[8];
  void printDigits(int digits, int i);
  void printValue(int x, int y, float value, int digits);

public:
  Display(SPIClass *spiClass, int8_t tftDC, int8_t hspiSS, int8_t tftRST, byte tftBL);
  void begin();
  void init();
  void draw(byte hour, byte minute, byte second, byte humInt, byte tempInt, byte illum, byte humExt, byte tempExt, byte tempCeiling, byte tempFloor);
  void drawHeat(bool active);
  void drawCircul(bool active);
  void drawLAN(bool fault);
  void drawCloud(bool fault);
};
#endif