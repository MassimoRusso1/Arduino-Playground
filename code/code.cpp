#include "code.hpp"

iot_grower::iot_grower()
{
    // Initializes the serial monitor
    Serial.begin(9600);
    // Initializes the LCD display
    lcd.init();
    lcd.backlight();
    lcd.scrollDisplayRight();
    lcd.autoscroll();
    // Sets the pins to input or output
    DDRD = DDRD | B00000000; // Sets the pins 0-7 to input
    DDRB = DDRB | B0000000;  // Sets the pins 8-13 to output
}

iot_grower::lcd_display()
{
    
}

iot_grower::void readTemperature()
{
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

iot_grower::void readLight()
{
    // Define the variables used in the function
    const int analogInPin = A0;
    const int adcPin = 0;
    int light = 0;
    // Read the light sensor
    ADMUX |= (1 << REFS0);
    ADMUX |= (1 << ADLAR);

    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC))
    {
    }
    byte low = ADCL;
    byte high = ADCH;
    light = (high << 8) | low;
    // Prints the light value on the serial monitor
    Serial.print("Light: ");
    Serial.println(light);
    // Prints the light value on the LCD display
    lcd.setCursor(0, 0);
    lcd.print("Light: ");
    lcd.print(light);
}
iot_grower::void readSoilMoisture()
{
    // Define the variables used in the function
    const int analogInPin = A1;
    const int adcPin = 1;
    int adc_value = 0;
    // Read the soil moisture sensor
    ADMUX |= (1 << REFS0);
    ADMUX |= (1 << ADLAR);

    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC))
    {
    }
    byte low = ADCL;
    byte high = ADCH;
    adc_value = (high << 8) | low;
    // Prints the soil moisture value on the serial monitor
    Serial.print("Soil Moisture: ");
    Serial.println(adc_value);
    // Prints the soil moisture value on the LCD display
    lcd.setCursor(0, 0);
    lcd.print("Soil Moisture: ");
    lcd.print(adc_value);
}

iot_grower::void readWaterLevel()
{
    // Define variables for the function
    const int analogInPin = A2;
    const int adcPin = 2;
    int water_level = 0;
    int sum = 0;
    int numReadings = 10;
    int full = 0;
    int empty = 0;
    int full_sensor = 0;
    int empty_sensor = 0;
    int level_in_mml = 0;
    // Set the time for the function to run
    unsigned long start_time = millis();
    while (millis() - start_time < 2000)
    {
    // Read water level sensor 10 times
    for (int i = 0; i < numReadings; ++i)
    {
        // Start ADC conversion
        ADMUX |= (1 << REFS0);
        ADMUX |= (1 << ADLAR);
        ADCSRA |= (1 << ADSC);

        // Wait for conversion to complete
        while (ADCSRA & (1 << ADSC))
        {
        }
        // Combine bytes to get water level value
        byte low = ADCL;
        byte high = ADCH;
        water_level = (high << 8) | low;
        // Add value to sum
        sum += water_level;
    }
    // Calculate average
    float average = (float)sum / numReadings;
    // Calculate the water level in mm
    full = 1250;
    empty = 500;
    full_sensor = 384;
    empty_sensor = 369;
    level_in_mml = (full - empty) / (full_sensor - empty_sensor)*(average - empty_sensor); 
    // Print average value on serial monitor
    if (average < empty_sensor)
    {
        Serial.println("Please fill the tank immediately!");
        lcd.setCursor(0, 0);
        lcd.print("Please fill the tank");
        lcd.setCursor(0, 1);
        lcd.print("")

    }
}
