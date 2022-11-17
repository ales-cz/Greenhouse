#include <esp_task_wdt.h>       // watchdog
#include <SPI.h>                // SPI bus
#include <Wire.h>               // IIC bus
#include <SimpleDHT.h>          // Sensor DHT22 (temperature & humidity)
#include <DallasTemperature.h>  // Sensor DS18B20 (temperature)
#include <BH1750.h>             // Sensor BH1750 (illuminance)
#include <DS3232RTC.h>          // RTC DS3231 (real time clock)
#include <Adafruit_ILI9341.h>   // LCD display
#include <Adafruit_GFX.h>       // LCD display
#include <Ethernet.h>           // Ethernet W5500
#include <ThingSpeak.h>         // ThingSpeak

// nastavení PINů
#define VSPI_SS   5   // Display
#define HSPI_SS   15  // Ethernet
#define TFT_BL    25  // Display backlight
#define TFT_DC    26  // Display
#define TFT_RST   27  // Display
#define DHT       32  // DHT22
#define OW        33  // OneWire bus (DS18B20)

// ostatní konstanty
#define WDT_TIMEOUT       4       // watchdog timeout [s]
#define DALLAS_RESOLUTION 11      // resolution of DS18B20 sensors (11 = 0,125 °C)
#define DELAY1            10      // ????
#define DELAY2            1000    // display
#define DELAY3            2500    // sensorsRead
#define DELAY4            20000   // cloudUpdate
#define BACKGROUND        0x0000  // black 
#define TEXTSIZE          3
#define TEXTCOLOR         0xFFFF  // white
#define LED_FREQ          5000    // display backlight PWM
#define LED_CHANNEL       0       // display backlight PWM
#define LED_RESOLUTION    8       // display backlight PWM

// SPI bus init
SPIClass  vspi(VSPI);
SPIClass  hspi(HSPI);

// Display init
Adafruit_ILI9341 lcd=Adafruit_ILI9341(&hspi, TFT_DC, HSPI_SS, TFT_RST);

// DHT22 init
SimpleDHT22 dht22(DHT);
int err = SimpleDHTErrSuccess;

// DS18B20 init
OneWire owBus(OW);
DallasTemperature ds18b20(&owBus);
DeviceAddress ds18b20Floor = { 0x28, 0x66, 0x7E, 0x72, 0x0B, 0x00, 0x00, 0xCE };
DeviceAddress ds18b20Ceiling = { 0x28, 0xCE, 0x2F, 0x72, 0x0B, 0x00, 0x00, 0x6B };
DS3232RTC myRTC;

// BH1750 init
BH1750 lightMeter;

// Ethernet init
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 2, 177);
IPAddress myDns(192, 168, 2, 1);
EthernetClient client;

// ThingSpeak init
unsigned long myChannelNumber = 1903776;
const char* myWriteAPIKey = "99A0T3QXV0LMP6MA";

// Variables
unsigned long timer1 = 0, timer2 = 0, timer3 = 0, timer4 = 0;           // Task timers
float tempInt, tempExt, tempFloor, tempCeiling, humInt, humExt, illumination;    // Measured values
float setHeatingTemp, setCirculationDiff, heatingHyst, circulationHyst; // Actuators settings
bool heatingActive, circulationActive;                                  // Actuators status
tmElements_t tm;
byte lcdBackLight = 100; // lcd backlight

void setup() {
  Serial.begin(115200);  // Serial monitor (debug)

  vspi.begin();
  hspi.begin();
  pinMode(VSPI_SS, OUTPUT);
  pinMode(HSPI_SS, OUTPUT);

  Wire.begin();
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
  }

  Ethernet.init(VSPI_SS);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP.");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet module was not found.");
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("DHCP assigned IP: ");
    Serial.println(Ethernet.localIP());
  }
  delay(1000); // Give the Ethernet module a second to initialize

  ds18b20.begin();
  ds18b20.setResolution(ds18b20Floor, DALLAS_RESOLUTION);
  ds18b20.setResolution(ds18b20Ceiling, DALLAS_RESOLUTION);
  ds18b20.setWaitForConversion(false);
  ds18b20.requestTemperatures();

  timer3 = millis();

  lcd.begin();
  lcd.setRotation(0);
  lcd.fillScreen(BACKGROUND);
  lcd.setTextSize(TEXTSIZE);
  lcd.setTextColor(TEXTCOLOR, BACKGROUND);
  
  myRTC.begin();
  if (myRTC.oscStopped(false)) {  // check the oscillator
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //rtc.adjust(DateTime(2014, 1, 21, 3, 3, 3));
    //Serial.println("Time adjusted.");
  }
  //rtc.disable32K(); // we don't need the 32K Pin, so disable it
  myRTC.squareWave(DS3232RTC::SQWAVE_NONE); // stop oscillating signals at SQW Pin
  
  ThingSpeak.begin(client); // initialize ThingSpeak

  // configure display backlight PWM
  ledcSetup(LED_CHANNEL, LED_FREQ, LED_RESOLUTION);
  ledcAttachPin(TFT_BL, LED_CHANNEL);

  
  delay(2000); // delay to complete all initial tasks before enable WDT

  esp_task_wdt_init(WDT_TIMEOUT, true); // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); // add current thread to WDT watch
}

void loop() {
  if (millis() - timer2 >= DELAY2) {
    timer2 = millis();
    display();
  }
  if (millis() - timer3 >= DELAY3) {
    timer3 = millis();
    sensorsRead();
  }
  if (millis() - timer4 >= DELAY4) {
    timer4 = millis();
    cloudUpdate();
  }
  esp_task_wdt_reset();
}