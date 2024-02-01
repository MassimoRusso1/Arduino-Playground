#include "code.hpp"

iot_grower::void readTemperature(){
    // Defines the DHT sensor
    #define DHTPIN 10
    #define DHTTYPE DHT11
    DHT dht(DHTPIN, DHTTYPE);
    dht.begin();
    // Defines the variables used in the function
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    // Check if any reads failed 
    if (isnan(humidity) || isnan(temperature))
    {
        Serial.println("Failed to read Data from DHT sensor!");
        return;
    }
    // Prints the temperature and humidity on the serial monitor
    Serial.print("Humidity: ");
    Serial.print(humidity);
    serial.println("%");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C ");
    // Prints the temperature and humidity on the LCD display
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print(" C");
    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(humidity);
    lcd.print(" %");
}