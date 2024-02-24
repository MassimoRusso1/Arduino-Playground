#ifndef PROJECT_H
#define PROJECT_H

#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 5        // Pin, an dem der DHT-Sensor angeschlossen ist
#define DHTTYPE DHT11   // Der verwendete DHT-Typ

#define BUTTON_PIN 2    // Pin für den Taster

namespace iot_grower {
  extern LiquidCrystal_I2C lcd;

  class iot_grower {
  public:
    iot_grower();

    static void timer1_ISR();
    static void timer2_ISR();
    void button_interrupt();
    void readDHT();
    void readLight();
    void readMoisture();
    void readWaterLevel();
    void updateCloud();
    void activate_Lamp();
    void deactivate_Lamp();
    void activate_Pump();
    void deactivate_Pump();
  private:
    int light;
    int moisture;
    int water_level;
    int light_sensor;
    int moisture_sensor;
    int water_level_sensor;
    int water_level_sensor_empty;
    int water_level_sensor_full;
    int water_level_empty;
    int water_level_full;
    float humidity_sensor;
    float temperature_sensor;
  };
}

#endif /* PROJECT_H */
