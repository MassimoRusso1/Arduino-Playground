#ifndef PROJECT_H
#define PROJECT_H

#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 5       // Pin für den DHT-Sensor
#define DHTTYPE DHT11  // Der verwendete DHT-Typ
#define BUTTON_PIN 3   // Pin für den Taster

namespace iot_grower {
    // Externe Definition der LCD-Variable
    extern LiquidCrystal_I2C lcd;

    // Klasse für den IoT-Gärtner
    class iot_grower {
    public:
        // Konstruktor
        iot_grower();

        // Methoden zur Datenverarbeitung
        void readDHT();
        void readLight();
        void readMoisture();
        void readWaterLevel();
        void button_interrupt();
        void activateLamp();
        void deactivateLamp();
        void activatePump();
        void deactivatePump();
        void executeTasks();

    private:
        // Sensordaten
        int light;
        int moisture;
        int waterLevel;

        // Sensorwerte
        volatile int lightSensor;
        volatile int moistureSensor;
        volatile int waterLevelSensor;
        int waterLevelSensorEmpty;
        int waterLevelSensorFull;
        int waterLevelEmpty;
        int waterLevelFull;

        // Umgebungsdaten
        volatile float humiditySensor;
        volatile float temperatureSensor;
    };

    // Externe Definition des IoT-Gärtner-Objekts
    extern iot_grower myGrower;
}

#endif /* PROJECT_H */
