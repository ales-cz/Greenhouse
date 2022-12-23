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

#define SSL_RAND 36 // Analog pin to get semi-random data from for SSL, don't connect it!
#define TFT_RST 32  // Display RST
#define TFT_DC 33   // Display DC
#define TFT_BL 25   // Display backlight
#define TS_CS 26    // Touchscreen CS
#define OW 27       // OneWire bus (DS18B20)
#define ETH_CS 5    // Ethernet CS
#define HEAT 17     // Heater output
#define CIRCUL 16   // Circulation output
#define DHT22EXT 4  // DHT22 external
#define DHT22INT 2  // DHT22 internal
#define TFT_CS 15   // Display CS

// ostatní konstanty
#define WDT_TIMEOUT 5 // watchdog timeout [s]

#endif