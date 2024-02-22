#ifndef project_h
#define project_h

// Include all the libraries used in the project
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>
#include "thingProperties.h"

namespace iot_grower
{
// Define the pins used for the temperature/humidity sensor
#define BUTTON_PIN 3
#define DHTPIN 5
#define DHTTYPE DHT11

// Initialize the LCD screen
LiquidCrystal_I2C lcd(0x27, 16, 2);

    class iot_grower
    {
    public:
        iot_grower(); // Constructor

        // Functions for reading the sensors
        void readDHT();
        void readLight();
        void readMoisture();
        void readWaterLevel();
        
        // Function to update the cloud
        void updateCloud();

        // Functions to activate or deactivate the lamp
        void activateLamp();
        void deactivateLamp();

        // Function for the button interrupt
        void buttonInterrupt();

    private:
        // Variables for the LCD
        float humidity;
        float temperature;
        int light;
        int moisture;
        int waterLevel;

        // Variables for the cloud
        float humidity_cloud;
        float temperature_cloud;
        int light_cloud;
        int moisture_cloud;
        int waterLevel_cloud;

        // Private funtions for the timers
        static void timer1ISR();
        static void timer2ISR();
    };
}

#endif // PROJECT_HPP