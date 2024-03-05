#include "thingProperties.h"
#include <Wire.h>

#define NODE_ADDRESS 2

// Enum to represent data indices
enum DataIndex {
  HUMIDITY_INDEX,
  TEMPERATURE_INDEX,
  MOISTURE_INDEX,
  LIGHT_INDEX,
  WATER_LEVEL_INDEX
};

// Define variables for sensor data
int HumiditySensor = 0;
int TemperatureSensor = 0;
int Light = 0;
int Moisture = 0;
int WaterLevel = 0;
int receivedBytes = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  // Initialize IoT properties
  initProperties();
  // Begin Arduino Cloud connection
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  // Set debug message level
  setDebugMessageLevel(2);
  // Print debug information
  ArduinoCloud.printDebugInfo();
  // Initialize I2C communication with given node address
  Wire.begin(NODE_ADDRESS);
}

void loop() {
  // Function to receive sensor data
  receiveData();
}

void receiveData() {
  // Check if data is available on I2C bus
  if (Wire.available() > 0) {
    // Read the next byte
    int receivedByte = Wire.read();
    // Assign the received byte to the corresponding variable based on the current data index
    switch (receivedBytes) {
      case HUMIDITY_INDEX:
        HumiditySensor = receivedByte;
        break;
      case TEMPERATURE_INDEX:
        TemperatureSensor = receivedByte;
        break;
      case MOISTURE_INDEX:
        Moisture = receivedByte;
        break;
      case LIGHT_INDEX:
        Light = receivedByte;
        break;
      case WATER_LEVEL_INDEX:
        WaterLevel = receivedByte;
        break;
      default:
        break;
    }
    // Increment the data index
    receivedBytes++;
    // If all 5 bytes have been received, reset the data index and process the data
    if (receivedBytes >= 5) {
      // Print received sensor data for debugging
      Serial.println("Humidity:");
      Serial.println(HumiditySensor);
      Serial.println("Temperature:");
      Serial.println(TemperatureSensor);
      Serial.println("Moisture:");
      Serial.println(Moisture);
      Serial.println("Light:");
      Serial.println(Light);
      Serial.println("WaterLevel:");
      Serial.println(WaterLevel);
      // Upload sensor data to the cloud
      uploadToCloud();
      // Reset the data index
      receivedBytes = 0;
    }
  }
}

void uploadToCloud() {
  // Upload sensor data to the cloud
  humidity_cloud = HumiditySensor;
  temperature_cloud = TemperatureSensor;
  light_cloud = Light;
  moisture_cloud = Moisture;
  water_level_cloud = WaterLevel;
  ArduinoCloud.update();
}
