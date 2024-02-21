#include "project.hpp"

// Initialize the LCD screen
LiquidCrystal_I2C lcd(0x27, 16, 2);

const unsigned long interval = 300000UL; // Interval for the sensor readings

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

  TCCR1A = 0; // Set the Timer/Counter Control Register A to 0
  TCCR1B = 0; // Set the Timer/Counter Control Register B to 0
  TCNT1 = 0;  // Set the Timer/Counter Register to 0
  OCR1A = 15624; // Set the Output Compare Register A to 15624
  TCCR1B |= (1 << WGM12); // Set the WGM12 bit in the Timer/Counter Control Register B
  TCCR1B |= (1 << CS12) | (1 << CS10); // Set the CS12 and CS10 bits in the Timer/Counter Control Register B
  TIMSK1 |= (1 << OCIE1A); // Set the OCIE1A bit in the Timer/Counter Interrupt Mask Register

  sei(); // Enable global interrupts
}

ISR(TIMER1_COMPA_vect)
{
  // Call the function to read the DHT sensor
  iot_grower::readDHT();
  // Call the function to read the light sensor
  iot_grower::readLight();
  // Call the function to read the moisture sensor
  iot_grower::readMoisture();
  // Call the function to read the water level sensor
  iot_grower::readWaterLevel();
  // Call the function to update the cloud
  iot_grower::updateCloud();
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
  // Store the sensor valuse in the variables
  humidity = humidity_sensor;
  temperature = temperature_sensor;
  // Store the values for the upload to the cloud
  humidity_cloud = humidity_sensor; // Not needed to initialize the variable in the class this will happen in the thingProperties.h (for all cloud variables)
  temperature_cloud = temperature_sensor;
}

void iot_grower::readLight()
{
  // Define the variables used in the function
  const int analogInPin = A0;
  int sensorValue = 0;
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
  light_sensor = (high << 8) | low;
  // Prints the light value on the serial monitor
  Serial.print("Light: ");
  Serial.print(light);
  Serial.println("%");
  // Evaluate the light value
  if (light < 100) // TODO: Change the values to the ones that are going to be used in the project
  {
    Serial.println("It's dark");
    iot_grower::activatelamp();
  }
  else if (light < 200)
  {
    Serial.println("It's dim");
    iot_grower::activatelamp();
  }
  else if (light < 300)
  {
    Serial.println("It's bright");
    iot_grower::deactivatelamp();
  }
  else
  {
    Serial.println("It's very bright");
    iot_grower::deactivatelamp();
  }
  // Store the value for the upload to the cloud
  light_cloud = light;
}

void iot_grower::readMoisture()
{
  // Define the variables used in the function
  const int analogInPin = A1;
  int sensorValue = 0;
  int moisture = 0;
  // Read the moisture sensor
  ADMUX |= (1 << REFS0);
  ADMUX |= (1 << ADLAR);

  ADCSRA |= (1 << ADSC);
  while (ADCSRA & (1 << ADSC))
  {
  }
  byte low = ADCL;
  byte high = ADCH;
  moisture = (high << 8) | low;
  // Prints the moisture value on the serial monitor
  Serial.print("Moisture: ");
  Serial.print(moisture);
  Serial.println("%");
  // Store the value in the variable
  moisture_sensor = moisture;
  // Store the value for the upload to the cloud
  moisture_cloud = moisture;
}


void iot_grower::readWaterLevel()
{
  // Define the variables used in the function
  const int analogInPin = A2;
  int sensorValue = 0;
  int water_level = 0;
  // Read the water level sensor
  ADMUX |= (1 << REFS0);
  ADMUX |= (1 << ADLAR);

  ADCSRA |= (1 << ADSC);
  while (ADCSRA & (1 << ADSC))
  {
  }
  byte low = ADCL;
  byte high = ADCH;
  water_level = (high << 8) | low;
  // Prints the water level value on the serial monitor
  Serial.print("Water Level: ");
  Serial.print(water_level);
  Serial.println("%");
  // Store the value in the variable
  water_level_sensor = water_level;
  // Store the value for the upload to the cloud
  water_level_cloud = water_level;
}

void iot_grower::updateCloud()
{
  // Update the cloud with the values
  ArduinoCloud.update();
}

void iot_grower::activatelamp()
{
  PORTD &= ~(1 << 6);
}

void iot_grower::deactivatelamp()
{
  PORTD |= (1 << 6);
}

//path: project/code/project.cpp