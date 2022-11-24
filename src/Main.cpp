#include "Settings.h"

SPIClass vspi(VSPI); // VSPI bus (Ethernet)
Preferences prefs;   // Internal flash
DS3232RTC myRTC;     // RTC module

Display display(TFT_DC, HSPI_SS, TFT_RST, TFT_BL);
Sensors sensors(DHT22INT, DHT22EXT, OW);
Actuators actuators(HEAT, CIRCUL);
Network network;
Cloud cloud;

// Variables
float tempInt, tempExt, tempFloor, tempCeiling, humInt, humExt, illum; // Measured values
bool lastHeating = 0, lastCirculating = 0;
byte cloudUpdateStatus, cloudLastUpdateStatus = 200;

void setup()
{
  Serial.begin(115200); // Serial monitor (debug)

  prefs.begin("greenhouse", false); // Preferences: RW-mode (second parameter = false)

  // prefs.clear(); // Remove all preferences under the opened namespace
  // if (!prefs.isKey("writeAPIKey")) { // Create if not exist
  //  prefs.putString("writeAPIKey", "99A0T3QXV0LMP6MA");
  //}
  // prefs.end(); // Close the Preferences
  // ESP.restart(); // Restart ESP

  vspi.begin(); // ethernet si to asi konfiguruje sám?
  pinMode(VSPI_SS, OUTPUT);

  display.begin();
  sensors.begin();
  actuators.begin(&prefs);
  network.begin(&prefs);
  cloud.begin(&prefs);

  // myRTC.begin(); // asi zbytečné, inicializuje IIC
  myRTC.squareWave(DS3232RTC::SQWAVE_NONE); // stop oscillating signals at SQW Pin
  setSyncProvider(myRTC.get);
  if (timeStatus() != timeSet)
    log_e("Unable to sync with the RTC");
  else
    log_i("RTC has set the system time");
  /*tmElements_t tm;
  tm.Hour = 21;
  tm.Minute = 29;
  tm.Second = 00;
  tm.Day = 24;
  tm.Month = 11;
  tm.Year = 2022 - 1970;
  myRTC.write(tm);*/
  if (myRTC.oscStopped(false))
  { // check the oscillator
    // myRTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // myRTC.adjust(DateTime(2014, 1, 21, 3, 3, 3));
    // Serial.println("Time adjusted.");
  }

  delay(2000);                          // delay to complete all initial tasks before enable WDT
  esp_task_wdt_init(WDT_TIMEOUT, true); // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);               // add current thread to WDT watch
}

void loop()
{
  display.drawClock();
  if (sensors.read(&tempInt, &tempExt, &tempFloor, &tempCeiling, &humInt, &humExt, &illum))
  {
    display.drawMeasured(tempInt, tempExt, tempFloor, tempCeiling, humInt, humExt, illum);
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