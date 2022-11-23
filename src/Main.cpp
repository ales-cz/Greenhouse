#include <esp_task_wdt.h> // WatchDog
#include <Preferences.h>  // For storing key-value pairs
#include <DS3232RTC.h>    // RTC DS3231 (real time clock)
#include "Display.h"
#include "Sensors.h"
#include "Actuators.h"
#include "Cloud.h"
#include <Ethernet.h>   // Ethernet W5500

// nastavení PINů
#define VSPI_SS 5  // Display CS
#define HSPI_SS 15 // Ethernet CS
#define HEAT 16    // Heater output
#define CIRCUL 17  // Circulation output
#define TFT_BL 25  // Display backlight
#define TFT_DC 26  // Display DC
#define TFT_RST 27 // Display RST
#define DHT22_1 32 // DHT22
#define OW 33      // OneWire bus (DS18B20)

// ostatní konstanty
#define WDT_TIMEOUT 4 // watchdog timeout [s]
#define DELAY1 10     // ????
#define DELAY2 1000   // display
#define DELAY3 2500   // sensorsRead
#define DELAY4 20000  // cloudUpdate

// Preferences init
Preferences preferences;

// SPI bus init
SPIClass vspi(VSPI);
SPIClass hspi(HSPI);

// RTC DS3231
DS3232RTC myRTC;

// Ethernet init
EthernetClient client;
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// Variables
unsigned long timer1 = 0, timer2 = 0, timer3 = 0, timer4 = 0;          // Task timers
float tempInt, tempExt, tempFloor, tempCeiling, humInt, humExt, illum; // Measured values
bool dhcp;

Display display(&hspi, TFT_DC, HSPI_SS, TFT_RST, TFT_BL);
Sensors sensors(DHT22_1, OW);
Actuators actuators(HEAT, CIRCUL);
Cloud cloud;

void setup()
{
  Serial.begin(115200); // Serial monitor (debug)

  // Preferences: RW-mode (second parameter has to be false).
  preferences.begin("greenhouse", false);
  // Remove all preferences under the opened namespace
  // preferences.clear();
  // if (!preferences.isKey("writeAPIKey")) { // Create if not exist
  //  preferences.putString("writeAPIKey", "99A0T3QXV0LMP6MA");
  //}
  // Close the Preferences
  // preferences.end();
  // Restart ESP
  // ESP.restart();

  // Actuators settings
  /*thermostat = preferences.getBool("thermostat", 1);
  circulation = preferences.getBool("circulation", 1);
  reqHeatTemp = preferences.getFloat("reqHeatTemp", 24);
  reqCirculDiff = preferences.getFloat("reqCirculDiff", 2);
  heatHyst = preferences.getFloat("heatHyst", 1);
  circulHyst = preferences.getFloat("circulHyst", 1);
*/
  // IP configuration
  dhcp = preferences.getBool("dhcp", 1);
  IPAddress ip(
      preferences.getUChar("ip1", 192),
      preferences.getUChar("ip2", 168),
      preferences.getUChar("ip3", 2),
      preferences.getUChar("ip4", 177));
  IPAddress dns(
      preferences.getUChar("dns1", 192),
      preferences.getUChar("dns2", 168),
      preferences.getUChar("dns3", 2),
      preferences.getUChar("dns4", 1));

    // Close the Preferences
  preferences.end();

  vspi.begin();
  hspi.begin();
  pinMode(VSPI_SS, OUTPUT);
  pinMode(HSPI_SS, OUTPUT);

  display.begin();
  sensors.begin();

  Ethernet.init(VSPI_SS);
  if (dhcp)
  {
    if (Ethernet.begin(mac) != 0)
    {
      Serial.println("DHCP assigned IP.");
    }
    else
    {
      Serial.println("Failed to configure Ethernet using DHCP.");
      Ethernet.begin(mac, ip, dns);
    }
  }
  else
  {
    Ethernet.begin(mac, ip, dns);
  }
  if (Ethernet.hardwareStatus() == EthernetNoHardware)
  {
    Serial.println("Ethernet module was not found.");
  }
  else if (Ethernet.linkStatus() == LinkOFF)
  {
    Serial.println("Ethernet cable is not connected.");
  }
  else
  {
    Serial.println("IP address: ");
    Serial.println(Ethernet.localIP());
  }
  delay(1000); // Give the Ethernet module a second to initialize

  timer3 = millis();

  myRTC.begin();
  setSyncProvider(myRTC.get);
  if (timeStatus() != timeSet)
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time");

  if (myRTC.oscStopped(false))
  { // check the oscillator
    // myRTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // myRTC.adjust(DateTime(2014, 1, 21, 3, 3, 3));
    // Serial.println("Time adjusted.");
  }
  // rtc.disable32K(); // we don't need the 32K Pin, so disable it
  myRTC.squareWave(DS3232RTC::SQWAVE_NONE); // stop oscillating signals at SQW Pin

  delay(2000); // delay to complete all initial tasks before enable WDT

  esp_task_wdt_init(WDT_TIMEOUT, true); // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);               // add current thread to WDT watch
}

void loop()
{
  if (millis() - timer2 >= DELAY2)
  {
    timer2 = millis();
    display.draw(hour(), minute(), second(), humInt, tempInt, illum, humExt, tempExt, tempCeiling, tempFloor);
  }
  if (millis() - timer3 >= DELAY3)
  {
    timer3 = millis();
    sensors.read();
    actuators.set(tempInt, tempFloor, tempCeiling);
  }
  if (millis() - timer4 >= DELAY4)
  {
    timer4 = millis();
    cloud.update();
  }
  esp_task_wdt_reset();
}