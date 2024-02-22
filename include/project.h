#ifndef PROJECT_H
#define PROJECT_H

#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 5        // Pin, an dem der DHT-Sensor angeschlossen ist
#define DHTTYPE DHT11   // Der verwendete DHT-Typ

#define BUTTON_PIN 2    // Pin für den Taster

#define TIMER1_DURATION 10 // Dauer des Timers 1 in Minuten
#define TIMER2_DURATION 20 // Dauer des Timers 2 in Minuten

namespace iot_grower {
  extern LiquidCrystal_I2C lcd;

  class iot_grower {
  public:
    iot_grower();

    void configureTimer(unsigned long duration, void (*ISR)());
    static void timer1_ISR();
    static void timer2_ISR();
    void button_interrupt();
    void readDHT();
    void readLight();
    void readMoisture();
    void readWaterLevel();
    void updateCloud();
    void activateLamp();
    void deactivateLamp();
  private:
    float humidity;
    float temperature;
    int light;
    int moisture;
    int water_level;
    float humidity_cloud;
    float temperature_cloud;
    int light_cloud;
    int moisture_cloud;
    int water_level_cloud;
    int light_sensor;
    int moisture_sensor;
    int water_level_sensor;
    int water_level_sensor_empty;
    int water_level_sensor_full;
    int water_level_empty;
    int water_level_full;
  };
}

#endif /* PROJECT_H */
