#include "project.hpp"

// Initialize the LCD screen
LiquidCrystal_I2C lcd(0x27, 16, 2);

iot_grower::iot_grower()
{
  // Initialize the serial communication
  Serial.begin(9600);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // Button setup
  DDRD &= ~(1 << BUTTON_PIN); // Set button pin as input
  PORTD |= (1 << BUTTON_PIN); // Enable pull-up resistor
  PCICR |= (1 << PCIE2);      // Enable pin change interrupt for port D
  PCMSK2 |= (1 << PCINT19);   // Enable pin change interrupt for button pin

  // Initialize the LCD screen
  lcd.init();
  lcd.backlight();
  lcd.noDisplay(); // Turn off the display after initialization
}

iot_grower::void button_interrupt()
{
  if (!(PIND & (1 << BUTTON_PIN)))
  {
    Serial.println("Interrupt activated and Button pressed.");
    lcd.display(); // Turn on the display
    cont long interval = 2000;
    int values[] = {humidity, temperature, light, moisture, water_level};
    for (int i = 0; i < 5; i++)
    {
      unsigned long previousMillis = 0;
      lcd.setCursor(0, 0);
      if (i == 0)
      {
        lcd.print("Humidity: ");
      }
      else if (i == 1)
      {
        lcd.print("Temperature: ");
      }
      else if (i == 2)
      {
        lcd.print("Light: ");
      }
      else if (i == 3)
      {
        lcd.print("Soil Moisture: ");
      }
      else if (i == 4)
      {
        lcd.print("Water Level: ");
      }
      lcd.setCursor(0, 1);
      lcd.print(values[i]);
      if (i == 0)
      {
        lcd.print("%");
      }
      else if (i == 1)
      {
        lcd.print("C");
      }
      else if (i == 2)
      {
        lcd.print("");
      }
      else if (i == 3)
      {
        lcd.print("");
      }
      else if (i == 4)
      {
        lcd.print("");
      }
      unsigned long currentMillis = millis(); // Get the current time
      // Wait for the interval to pass
      while (currentMillis - previousMillis < interval)
      {
        currentMillis = millis(); // update the current time
      }
    }
  }
  else
  {
    Serial.println("Interrupt activated, but button is not pressed.");
  }
  lcd.noDisplay(); // Turn off the display
}

void iot_grower::readDHT()
{
  // Define the variables used in the function
  DHT dht(DHTPIN, DHTTYPE);
  // Read the temperature and humidity values
  float humidity_sensor = dht.readHumidity();
  float temperature_sensor = dht.readTemperature();
  // Check if the values are valid
  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // Prints the temperature and humidity values on the serial monitor
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C");
  // Store the values in the variables
  humidity = humidity_sensor;
  temperature = temperature_sensor;
  // Store the values for the upload to the cloud
  humidity_cloud = humidity_sensor; // Not needed to initialize the variable in the class this will happen in the thingProperties.h (for all cloud variables)
  temperature_cloud = temperature_sensor;
  ArduinoCloud.update();
}


//path: project/code/project.cpp