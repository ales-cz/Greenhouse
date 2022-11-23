#include "Settings.h"

// Preferences init
Preferences prefs;

// SPI bus init
SPIClass vspi(VSPI);
SPIClass hspi(HSPI);

// RTC DS3231
DS3232RTC myRTC;

// Ethernet init
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// Variables
float tempInt, tempExt, tempFloor, tempCeiling, humInt, humExt, illum; // Measured values
bool lastHeating = 0, lastCirculating = 0;
byte cloudUpdateStatus, cloudLastUpdateStatus = 200;
bool dhcp;

Display display(&hspi, TFT_DC, HSPI_SS, TFT_RST, TFT_BL);
Sensors sensors(DHT22_1, OW);
Actuators actuators(HEAT, CIRCUL, &prefs);
Cloud cloud(&prefs);

void setup()
{
  Serial.begin(115200); // Serial monitor (debug)

  // Preferences: RW-mode (second parameter has to be false).
  //prefs.begin("greenhouse", false);
  // Remove all preferences under the opened namespace
  // prefs.clear();
  // if (!prefs.isKey("writeAPIKey")) { // Create if not exist
  //  prefs.putString("writeAPIKey", "99A0T3QXV0LMP6MA");
  //}
  // Close the Preferences
  // prefs.end();
  // Restart ESP
  // ESP.restart();

  // IP configuration
  dhcp = prefs.getBool("dhcp", 1);
  IPAddress ip(
      prefs.getUChar("ip1", 192),
      prefs.getUChar("ip2", 168),
      prefs.getUChar("ip3", 2),
      prefs.getUChar("ip4", 177));
  IPAddress dns(
      prefs.getUChar("dns1", 192),
      prefs.getUChar("dns2", 168),
      prefs.getUChar("dns3", 2),
      prefs.getUChar("dns4", 1));

  // Close the Preferences
  //prefs.end();

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
  if (sensors.read(&tempInt, &tempExt, &tempFloor, &tempCeiling, &humInt, &humExt, &illum))
  {
    display.draw(hour(), minute(), second(), tempInt, tempExt, tempFloor, tempCeiling, humInt, humExt, illum);
    actuators.set(tempInt, tempFloor, tempCeiling);
    if (cloud.update(&cloudUpdateStatus, tempInt, tempExt, tempFloor, tempCeiling, humInt, humExt, illum,
                     actuators.isHeating(), actuators.isCirulating()))
    {
      if (cloudUpdateStatus != cloudLastUpdateStatus)
      {
        if (cloudUpdateStatus == 200)
          display.drawCloud(0);
        else
          display.drawCloud(1);
      }
    }
    if (lastHeating != actuators.isHeating())
    {
      lastHeating = actuators.isHeating();
      display.drawHeat(actuators.isHeating());
    }
    if (lastCirculating != actuators.isCirulating())
    {
      lastCirculating = actuators.isCirulating();
      display.drawCircul(actuators.isCirulating());
    }
  }

  esp_task_wdt_reset();
}