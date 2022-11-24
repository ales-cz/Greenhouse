#ifndef MY_SENSORS_H
#define MY_SENSORS_H
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>               // Sensor DHT22 (temperature & humidity)
#include <DallasTemperature.h> // Sensor DS18B20 (temperature)
#include <BH1750.h>            // Sensor BH1750 (illuminance)

#define DHTTYPE DHT22
#define DALLAS_RESOLUTION 11 // resolution of DS18B20 sensors (11 = 0,125 °C)
#define DELAY_REQUEST 2000   // Delay before the next measurement request
// DHT22 min 2000ms, DS18B20 max 750ms (for 12 bit resolution), BH1750 max 120ms

class Sensors
{

private:
    DHT dht22Int, dht22Ext;    // DHT22 internal sensors
    OneWire owBus;             // OneWire bus for all DS18B20 sensors
    DallasTemperature ds18b20; // DS18B20 sensors
    BH1750 lightMeter;         // BH1750 illumination sensor
    DeviceAddress ds18b20Floor = {0x28, 0x66, 0x7E, 0x72, 0x0B, 0x00, 0x00, 0xCE};
    DeviceAddress ds18b20Ceiling = {0x28, 0xCE, 0x2F, 0x72, 0x0B, 0x00, 0x00, 0x6B};
    u_long lastRequest;

public:
    Sensors(int8_t pDHT22Int, int8_t pDHT22Ext, int8_t pOW);
    void begin();
    bool read(float *tempInt, float *tempExt, float *tempFloor, float *tempCeiling,
              float *humInt, float *humExt, float *illum);
};
#endif