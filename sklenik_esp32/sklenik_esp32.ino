#include <esp_task_wdt.h>        // WatchDog
#include <Preferences.h>         // For storing key-value pairs
#include <SPI.h>                 // SPI bus
#include <SPIFFS.h>              // SPI Flash File Storage
#include <Wire.h>                // IIC bus
#include <SimpleDHT.h>           // Sensor DHT22 (temperature & humidity)
#include <DallasTemperature.h>   // Sensor DS18B20 (temperature)
#include <BH1750.h>              // Sensor BH1750 (illuminance)
#include <DS3232RTC.h>           // RTC DS3231 (real time clock)
#include <Adafruit_ILI9341.h>    // LCD display
#include <Adafruit_GFX.h>        // LCD display
#include <SPIFFS_ImageReader.h>  // Load images from SPIFFS partition for Adafruit_GFX
#include <Fonts/FreeSans12pt7b.h>
#include <Ethernet.h>    // Ethernet W5500
#include <ThingSpeak.h>  // ThingSpeak

// nastavení PINů
#define VSPI_SS 5   // Display CS
#define HSPI_SS 15  // Ethernet CS
#define HEAT 16     // Heater output
#define CIRCUL 17   // Circulation output
#define TFT_BL 25   // Display backlight
#define TFT_DC 26   // Display DC
#define TFT_RST 27  // Display RST
#define DHT 32      // DHT22
#define OW 33       // OneWire bus (DS18B20)

// ostatní konstanty
#define WDT_TIMEOUT 4         // watchdog timeout [s]
#define DALLAS_RESOLUTION 11  // resolution of DS18B20 sensors (11 = 0,125 °C)
#define DELAY1 10             // ????
#define DELAY2 1000           // display
#define DELAY3 2500           // sensorsRead
#define DELAY4 20000          // cloudUpdate
#define BACKGROUND 0xFFFF     // white
#define TEXTSIZE 1
#define TEXTCOLOR 0x0000  // black
#define LED_FREQ 10000    // display backlight PWM
#define LED_CHANNEL 0     // display backlight PWM
#define LED_RESOLUTION 8  // display backlight PWM
#define CANVAS_X1 65      // temperature & humidity
#define CANVAS_X2 95      // clock & illumination
#define CANVAS_Y 20
#define GRID_X1 5  //position of display grid
#define GRID_X2 140
#define GRID_Y1 27
#define GRID_Y2 57
#define GRID_Y3 87
#define GRID_Y4 117
#define GRID_Y5 147
#define GRID_Y6 177
#define GRID_Y7 207
#define STAT_X 269  //position of status icons
#define STAT_Y1 5
#define STAT_Y2 65
#define STAT_Y3 125
#define STAT_Y4 185

// Preferences init
Preferences preferences;

// SPI bus init
SPIClass vspi(VSPI);
SPIClass hspi(HSPI);

// Display init
Adafruit_ILI9341 lcd = Adafruit_ILI9341(&hspi, TFT_DC, HSPI_SS, TFT_RST);
SPIFFS_ImageReader reader;

// DHT22 init
SimpleDHT22 dht22(DHT);

// DS18B20 init
OneWire owBus(OW);
DallasTemperature ds18b20(&owBus);
DeviceAddress ds18b20Floor = { 0x28, 0x66, 0x7E, 0x72, 0x0B, 0x00, 0x00, 0xCE };
DeviceAddress ds18b20Ceiling = { 0x28, 0xCE, 0x2F, 0x72, 0x0B, 0x00, 0x00, 0x6B };

// RTC DS3231
DS3232RTC myRTC;

// BH1750 init
BH1750 lightMeter;

// Ethernet init
EthernetClient client;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Variables
unsigned long timer1 = 0, timer2 = 0, timer3 = 0, timer4 = 0; // Task timers
float tempInt, tempExt, tempFloor, tempCeiling, humInt, humExt, illum; // Measured values
float reqHeatTemp, reqCirculDiff, heatHyst, circulHyst; // Actuators settings
bool thermostat, circulation, heatOut, circulOut; // Actuators status
bool dhcp;
byte lcdBackLight = 100; // LCD backlight
char lcdBuf[8];
GFXcanvas1 canvas1(CANVAS_X1, CANVAS_Y);
GFXcanvas1 canvas2(CANVAS_X2, CANVAS_Y);

// ThingSpeak channel configuration
unsigned long channelNumber;
char writeAPIKey[17] = "99A0T3QXV0LMP6MA";
int cloudLastUpdateStatus = 200;

void setup() {
  Serial.begin(115200);  // Serial monitor (debug)

  // Preferences: RW-mode (second parameter has to be false).
  preferences.begin("greenhouse", false);
  // Remove all preferences under the opened namespace
  //preferences.clear();
  //if (!preferences.isKey("writeAPIKey")) { // Create if not exist
  //  preferences.putString("writeAPIKey", "99A0T3QXV0LMP6MA");
  //}
  // Close the Preferences
  //preferences.end();
  // Restart ESP
  //ESP.restart();

  // Actuators settings
  thermostat = preferences.getBool("thermostat", 1);
  circulation = preferences.getBool("circulation", 1);
  reqHeatTemp = preferences.getFloat("reqHeatTemp", 24);
  reqCirculDiff = preferences.getFloat("reqCirculDiff", 2);
  heatHyst = preferences.getFloat("heatHyst", 1);
  circulHyst = preferences.getFloat("circulHyst", 1);

  // IP configuration
  dhcp = preferences.getBool("dhcp", 1);
  IPAddress ip(
    preferences.getUChar("ip1", 192),
    preferences.getUChar("ip2", 168),
    preferences.getUChar("ip3", 2),
    preferences.getUChar("ip4", 177)
  );
  IPAddress dns(
    preferences.getUChar("dns1", 192),
    preferences.getUChar("dns2", 168),
    preferences.getUChar("dns3", 2),
    preferences.getUChar("dns4", 1)
  );

  // ThingSpeak channel configuration
  channelNumber = preferences.getULong("channelNumber", 1903776);
  preferences.getString("writeAPIKey", writeAPIKey, sizeof(writeAPIKey));

  // Close the Preferences
  preferences.end();

  vspi.begin();
  hspi.begin();
  pinMode(VSPI_SS, OUTPUT);
  pinMode(HSPI_SS, OUTPUT);

  pinMode(HEAT, OUTPUT);
  pinMode(CIRCUL, OUTPUT);

  Wire.begin();
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("BH1750 begin");
  } else {
    Serial.println("Error initialising BH1750");
  }

  Ethernet.init(VSPI_SS);
  if (dhcp) {
    if (Ethernet.begin(mac) != 0) {
      Serial.println("DHCP assigned IP.");
    } else {
      Serial.println("Failed to configure Ethernet using DHCP.");
      Ethernet.begin(mac, ip, dns);
    }
  } else {
    Ethernet.begin(mac, ip, dns);
  }
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet module was not found.");
  } else if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  } else {
    Serial.println("IP address: ");
    Serial.println(Ethernet.localIP());
  }
  delay(1000);  // Give the Ethernet module a second to initialize

  ds18b20.begin();
  ds18b20.setResolution(ds18b20Floor, DALLAS_RESOLUTION);
  ds18b20.setResolution(ds18b20Ceiling, DALLAS_RESOLUTION);
  ds18b20.setWaitForConversion(false);
  ds18b20.requestTemperatures();

  timer3 = millis();

  // initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1)
      ;
  }

  lcd.begin();
  displayInit();

  myRTC.begin();
  setSyncProvider(myRTC.get);
  if (timeStatus() != timeSet)
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time");

  if (myRTC.oscStopped(false)) {  // check the oscillator
    //myRTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //myRTC.adjust(DateTime(2014, 1, 21, 3, 3, 3));
    //Serial.println("Time adjusted.");
  }
  //rtc.disable32K(); // we don't need the 32K Pin, so disable it
  myRTC.squareWave(DS3232RTC::SQWAVE_NONE);  // stop oscillating signals at SQW Pin

  ThingSpeak.begin(client);  // initialize ThingSpeak

  // configure display backlight PWM
  ledcSetup(LED_CHANNEL, LED_FREQ, LED_RESOLUTION);
  ledcAttachPin(TFT_BL, LED_CHANNEL);


  delay(2000);  // delay to complete all initial tasks before enable WDT

  esp_task_wdt_init(WDT_TIMEOUT, true);  // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);                // add current thread to WDT watch
}

void loop() {
  if (millis() - timer2 >= DELAY2) {
    timer2 = millis();
    display();
  }
  if (millis() - timer3 >= DELAY3) {
    timer3 = millis();
    sensors();
    actuators();
  }
  if (millis() - timer4 >= DELAY4) {
    timer4 = millis();
    cloudUpdate();
  }
  esp_task_wdt_reset();
}