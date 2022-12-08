#include "Settings.h"

Preferences prefs; // Internal flash

DS3232RTC myRTC;

Sensors sensors;
Actuators actuators;
Network network;
TimeSyncNTP timeSyncNTP;
Cloud cloud;
Display display;

// Variables
float tempInt, tempExt, tempFloor, tempCeiling, humInt, humExt, illum; // Measured values
byte cloudUpdateStatus;

void setup()
{
  Serial.begin(115200); // Serial monitor (debug)

  prefs.begin("greenhouse", false); // Preferences: RW-mode (second parameter = false)

  // prefs.clear(); // Remove all preferences under the opened namespace
  // if (!prefs.isKey("writeAPIKey"))
  //  prefs.putString("writeAPIKey", "99A0T3QXV0LMP6MA"); // Create preference if not exist
  // ESP.restart(); // Restart ESP

  myRTC.begin();                            // IIC bus initialization
  myRTC.squareWave(DS3232RTC::SQWAVE_NONE); // stop oscillating signals at SQW Pin

  sensors.begin();
  actuators.begin(&prefs);
  network.begin(&prefs);
  timeSyncNTP.begin(&prefs, &myRTC);
  cloud.begin(&prefs);
  display.begin(&actuators, &timeSyncNTP);

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

    display.drawHeat(actuators.isThermostat(), actuators.isHeating());
    display.drawCircul(actuators.isCirculation(), actuators.isCirulating());
    display.drawLAN(Ethernet.linkStatus());

    if (cloud.update(&cloudUpdateStatus, tempInt, tempExt, tempFloor, tempCeiling, humInt, humExt, illum,
                     actuators.isHeating(), actuators.isCirulating()))
      display.drawCloud(cloudUpdateStatus);
  }

  esp_task_wdt_reset();
}