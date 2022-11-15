#include <esp_task_wdt.h>
#include <SPI.h>
#include <Wire.h>
#include <SimpleDHT.h>
#include <DallasTemperature.h>
#include <BH1750.h>
#include <DS3232RTC.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <Ethernet.h>
#include <ThingSpeak.h>

// nastavení PINů
//#define VSPI_MISO   19//MISO
//#define VSPI_MOSI   23//MOSI
//#define VSPI_SCLK   18//SCK
#define VSPI_SS     5//SS

//#define HSPI_MISO   12
//#define HSPI_MOSI   13
//#define HSPI_SCLK   14
#define HSPI_SS     15

#define TFT_DC     26
#define TFT_RST  27

#define DHT 32
#define OW 33

// ostatní konstanty
#define WDT_TIMEOUT 4
#define DALLAS_RESOLUTION 11
#define DELAY1 10 // ovládání, zatím nevyužito
#define DELAY2 1000 // display
#define DELAY3 2500 // sensorsRead
#define DELAY4 20000 // cloudUpdate

//SPIClass * vspi = NULL;
//SPIClass * hspi = NULL;
//vspi = new SPIClass(VSPI);
//hspi = new SPIClass(HSPI);

SPIClass  vspi(VSPI);
SPIClass  hspi(HSPI);
Adafruit_ILI9341 lcd=Adafruit_ILI9341(&hspi, TFT_DC, HSPI_SS, TFT_RST);


SimpleDHT22 dht22(DHT);
int err = SimpleDHTErrSuccess;

OneWire OWBus(OW);
DallasTemperature ds18b20(&OWBus);
DeviceAddress ds18b20Floor = { 0x28, 0x66, 0x7E, 0x72, 0x0B, 0x00, 0x00, 0xCE };
DeviceAddress ds18b20Ceiling = { 0x28, 0xCE, 0x2F, 0x72, 0x0B, 0x00, 0x00, 0x6B };

//Arduino_ESP32SPI bus = Arduino_ESP32SPI(TFT_DC, HSPI_SS, HSPI_SCLK, HSPI_MOSI, HSPI_MISO);
//  Arduino_ILI9341 lcd = Arduino_ILI9341(hspi, TFT_RESET);


DS3232RTC myRTC;

BH1750 lightMeter;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 2, 177);
IPAddress myDns(192, 168, 2, 1);
EthernetClient client;

unsigned long myChannelNumber = 1903776;
const char* myWriteAPIKey = "99A0T3QXV0LMP6MA";

unsigned long timer1 = 0, timer2 = 0, timer3 = 0, timer4 = 0;
float tempInt, tempExt, tempFloor, tempCeiling, humInt, humExt, lux;
float heatingTemp, circulationDiff, heatingHyst, circulationHyst;
bool heatingActive, circulationActive;
tmElements_t tm;

void setup() {
  Serial.begin(115200);  // vypis do serial monitoru, nezapojovat piny 0, 1
  // inicializace modulů
  vspi.begin();
  hspi.begin();
  pinMode(VSPI_SS, OUTPUT);
  pinMode(HSPI_SS, OUTPUT);

  Wire.begin();
  lightMeter.begin();
  Ethernet.init(VSPI_SS);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to configure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);

  ds18b20.begin();
  ds18b20.setResolution(ds18b20Floor, DALLAS_RESOLUTION);
  ds18b20.setResolution(ds18b20Ceiling, DALLAS_RESOLUTION);
  ds18b20.setWaitForConversion(false);
  ds18b20.requestTemperatures();

  timer3 = millis();

  lcd.begin();
  lcd.fillScreen(255);
  lcd.setTextSize(2);
  lcd.setTextColor(0);
  lcd.setCursor(20, 20);
  lcd.print("Teplota a vlhkost");
  
  myRTC.begin();
  if (myRTC.oscStopped(false)) {  // check the oscillator
    // this will adjust to the date and time at compilation
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //rtc.adjust(DateTime(2014, 1, 21, 3, 3, 3));
    //Serial.println("Time adjusted.");
    //Serial.flush();
  }

  // we don't need the 32K Pin, so disable it
  //rtc.disable32K();

  // stop oscillating signals at SQW Pin
  myRTC.squareWave(DS3232RTC::SQWAVE_NONE);
  
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  // make a delay before enable WDT
  // this delay help to complete all initial tasks
  delay(2000);
//  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
//  esp_task_wdt_add(NULL); //add current thread to WDT watch
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
  //esp_task_wdt_reset();  // v loop lze použít vícekrát
}