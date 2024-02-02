#define code_hpp
#ifndef code_hpp

#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>

class iot_grower
{
public:
    iot_grower();
    void readTemperature();
    void readLight();
    void readSoil();
    void readWater();
    void readAir();
    void readWaterLevel();
private:
    LiquidCrystal_I2C lcd (long, int, int);
    DHT dht (int, int);
    //unsigned long currentMillis;
    //unsigned long previousMillis;
    //const long interval;
    int water_full;
    int water_empty;
    int light;
    int soil;
    int water;
    float humidity;
    float temperature;
    bool should_pour_plant;
};


