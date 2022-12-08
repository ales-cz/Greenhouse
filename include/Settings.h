#ifndef SETTINGS_H
#define SETTINGS_H
#include <esp_task_wdt.h> // WatchDog
#include <Preferences.h>  // For storing key-value pairs
#include <SPI.h>          // SPI bus
#include <DS3232RTC.h>    // RTC DS3231 (real time clock)
#include "Display.h"
#include "Sensors.h"
#include "Actuators.h"
#include "Network.h"
#include "TimeSyncNTP.h"
#include "Cloud.h"

// nastavení PINů
#define TFT_CS 15   // Display CS
#define TFT_DC 26   // Display DC
#define TFT_RST 27  // Display RST
#define TFT_BL 25   // Display backlight
#define TS_CS 2     // Touchscreen CS
#define ETH_CS 5    // Ethernet CS
#define DHT22INT 32 // DHT22 internal
#define DHT22EXT 4  // DHT22 external
#define OW 33       // OneWire bus (DS18B20)
#define HEAT 16     // Heater output
#define CIRCUL 17   // Circulation output
#define SSL_RAND 36 // Analog pin to get semi-random data from for SSL, don't connect it!

// ostatní konstanty
#define WDT_TIMEOUT 5 // watchdog timeout [s]

#endif