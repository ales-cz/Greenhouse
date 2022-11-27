#include "Settings.h"

Preferences prefs; // Internal flash

Display display(TFT_DC, HSPI_SS, TFT_RST, TFT_BL);
Sensors sensors(DHT22INT, DHT22EXT, OW);
Actuators actuators(HEAT, CIRCUL);
Network network;
TimeSyncNTP timeSyncNTP;
Cloud cloud;

DS3232RTC myRTC;

// Variables
float tempInt, tempExt, tempFloor, tempCeiling, humInt, humExt, illum; // Measured values
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

  pinMode(VSPI_SS, OUTPUT); // VSPI SS Ethernet

  myRTC.begin();                            // IIC bus initialization
  myRTC.squareWave(DS3232RTC::SQWAVE_NONE); // stop oscillating signals at SQW Pin

  display.begin();
  sensors.begin();
  actuators.begin(&prefs);
  network.begin(&prefs);
  timeSyncNTP.begin(&prefs, &myRTC);
  cloud.begin(&prefs);

  setSyncProvider(myRTC.get);
  setSyncInterval(240);
  if (timeStatus() != timeSet)
    log_e("Unable to sync with the RTC");
  else
    log_i("RTC has set the system time");

  delay(1000);                          // delay to complete all initial tasks before enable WDT
  esp_task_wdt_init(WDT_TIMEOUT, true); // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);               // add current thread to WDT watch
}

void loop()
{
  timeSyncNTP.update();
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
          display.drawCloud(false);
        else
          display.drawCloud(true);
      }
    }

    display.drawHeat(actuators.isThermostat(), actuators.isHeating());
    display.drawCircul(actuators.isCirculation(), actuators.isCirulating());
  }

  esp_task_wdt_reset();
}