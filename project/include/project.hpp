#ifndef project_h
#define project_h

// Include all the libraries used in the project
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>
#include "thingProperties.h"

// Define the pins used in the project
#define BUTTON_PIN 3
#define DHTPIN 5
#define DHTTYPE DHT11

// Define the class used in the project
class iot_grower
{
    public:
        // Constructor
        iot_grower();
        // All the functions used in the project

        // All Interrupts used in the project
        void buttonInterrupt();

        // All the variables used in the project
};      