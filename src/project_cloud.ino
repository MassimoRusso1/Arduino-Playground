#include <Arduino.h>
#include "thingProperties.h"

// Define the variables for the cloud
float humidity = 0.0;
float temperature = 0.0;
int light = 0;
int moisture = 0;
int water_level = 0;

void setup() {
    Serial.begin(9600);
    initProperties();
    ArduinoCloud.begin(ArduinoIoTPreferredConnection);
    setDebugMessageLevel(2);
    ArduinoCloud.printDebugInfo();
}

void loop() {
    if (Serial.available() > 0) {
        char data = Serial.read();
        if (data == 'H') {
            humidity = Serial.parseFloat();
            Serial.println(humidity);
        } else if (data == 'T') {
            temperature = Serial.parseFloat();
            Serial.println(temperature);
        } else if (data == 'L') {
            light = Serial.parseInt();
            Serial.println(light);
        } else if (data == 'M') {
            moisture = Serial.parseInt();
            Serial.println(moisture);
        } else if (data == 'W') {
            water_level = Serial.parseInt();
            Serial.println(water_level);
        }
        uploadToCloud();
    }
}

void uploadToCloud() {
        humidity_cloud = humidity; // Not required to initialize the variable, this will be done by the cloud in the thingProperties.h
        temperature_cloud = temperature;
        light_cloud = light;
        moisture_cloud = moisture;
        water_level_cloud = water_level;
        ArduinoCloud.update();
}
