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
#include "Cloud.h"

// nastavení PINů
#define VSPI_SS 5   // Ethernet CS
#define HSPI_SS 15  // Display CS
#define HEAT 16     // Heater output
#define CIRCUL 17   // Circulation output
#define TFT_BL 25   // Display backlight
#define TFT_DC 26   // Display DC
#define TFT_RST 27  // Display RST
#define DHT22INT 32 // DHT22 internal
#define DHT22EXT 4  // DHT22 external
#define OW 33       // OneWire bus (DS18B20)

// ostatní konstanty
#define WDT_TIMEOUT 4 // watchdog timeout [s]

#endif