#include "project.h"
//#include "thingProperties.h"

namespace iot_grower
{
  LiquidCrystal_I2C lcd(0x27, 16, 2);
  iot_grower myGrower;

  iot_grower::iot_grower()
  {
    // Initialisierung der seriellen Kommunikation
    Serial.begin(9600);

    // Initialisierung der Eigenschaften
    //initProperties();

    // Verbindung zum Arduino IoT Cloud herstellen
    //ArduinoCloud.begin(ArduinoIoTPreferredConnection);
    //setDebugMessageLevel(2);
    //ArduinoCloud.printDebugInfo();

    // Setzen der Pin-Modi
    DDRD |= (1 << 6);  // Setzen des Lampen-Pins als Ausgang
    PORTD |= (1 << 6); // Lampen-Pin auf HIGH setzen

    // Button Setup
    DDRD &= ~(1 << BUTTON_PIN); // Setzen des Button-Pins als Eingang
    PORTD |= (1 << BUTTON_PIN); // Pull-up-Widerstand aktivieren
    PCICR |= (1 << PCIE2);      // Pin Change Interrupt für Port D aktivieren
    PCMSK2 |= (1 << PCINT19);   // Pin Change Interrupt für Button-Pin aktivieren

    // Initialisierung des LCD-Bildschirms
    lcd.init();
    lcd.backlight();
    lcd.noDisplay(); // Display nach Initialisierung ausschalten

    // Timer-Konfiguration

    unsigned long timer_overflow_count1 = (10 * 60 * 16000000) / 1024;
    unsigned long timer_overflow_count2 = (20 * 60 * 16000000) / 1024;
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    OCR1A = timer_overflow_count1;
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS12) | (1 << CS10);
    TIMSK1 |= (1 << OCIE1A);

    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = 0;
    OCR2A = timer_overflow_count2;
    TCCR2B |= (1 << WGM12);
    TCCR2B |= (1 << CS22) | (1 << CS20);
    TIMSK2 |= (1 << OCIE2A);

    // Globale Interrupts aktivieren
    sei();
  }

  void iot_grower::timer1_ISR()
  {
    myGrower.readLight();
    myGrower.readMoisture();
    //updateCloud();
  }

  void iot_grower::timer2_ISR()
  {
    myGrower.readDHT();
    myGrower.readWaterLevel();
    //updateCloud();
  }

  void iot_grower::button_interrupt()
  {
    iot_grower myGrower; // Instanz der Klasse erstellen
    if (!(PIND & (1 << BUTTON_PIN)))
    {
      Serial.println("Interrupt aktiviert und Taste gedrückt.");
      lcd.display(); // Display einschalten
      const long interval = 2000;
      int values[] = {humidity, temperature, light, moisture, water_level};
      for (int i = 0; i < 5; i++)
      {
        lcd.setCursor(0, 0);
        switch (i)
        {
        case 0:
          lcd.print("Luftfeuchtigkeit: ");
          lcd.print("%");
          break;
        case 1:
          lcd.print("Temperatur: ");
          lcd.print("C");
          break;
        case 2:
          lcd.print("Licht: ");
          break;
        case 3:
          lcd.print("Bodenfeuchtigkeit: ");
          break;
        case 4:
          lcd.print("Wasserstand: ");
          break;
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
        unsigned long previousMillis = millis();
        while (millis() - previousMillis < interval)
          ;
      }
    }
    else
    {
      Serial.println("Interrupt aktiviert, aber Taste nicht gedrückt.");
    }
    lcd.noDisplay(); // Display ausschalten
  }

  void iot_grower::readDHT()
  {
    iot_grower myGrower; // Instanz der Klasse erstellen
    // Define the variables used in the function
    DHT dht(DHTPIN, DHTTYPE);
    // Read the temperature and humidity values
    float humidity_sensor = dht.readHumidity();
    float temperature_sensor = dht.readTemperature();
    // Check if the values are valid
    if (isnan(humidity_sensor) || isnan(temperature_sensor))
    {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    // Prints the temperature and humidity values on the serial monitor
    Serial.print("Humidity: ");
    Serial.print(humidity_sensor);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temperature_sensor);
    Serial.println(" *C");
    // Store the sensor valuse in the variables
    humidity = humidity_sensor;
    temperature = temperature_sensor;
    // Store the values for the upload to the cloud
    humidity_cloud =
        humidity_sensor; // Not needed to initialize the variable in the class
                         // this will happen in the thingProperties.h (for all
                         // cloud variables)
    temperature_cloud = temperature_sensor;
  }

  void iot_grower::readLight()
  {
    iot_grower myGrower; // Instanz der Klasse erstellen
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
    light = (high << 8) | low;
    // Prints the light value on the serial monitor
    Serial.print("Light: ");
    Serial.print(light);
    Serial.println("%");
    // Evaluate the light value

    if (light < 100) // TODO: Change the values to the ones that are going to be
                     // used in the project
    {
      Serial.println("It's dark");
      myGrower.activateLamp();
    }
    else if (light < 200)
    {
      Serial.println("It's dim");
      myGrower.activateLamp();
    }
    else if (light < 300)
    {
      Serial.println("It's bright");
      myGrower.deactivateLamp();
    }
    else
    {
      Serial.println("It's very bright");
      myGrower.deactivateLamp();
    }
    // Store the value for the upload to the cloud
    light_cloud = light;
  }

  void iot_grower::readMoisture()
  {
    iot_grower myGrower; // Instanz der Klasse erstellen
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
    iot_grower myGrower; // Instanz der Klasse erstellen
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
    Serial.println(water_level);
    // Evaluate the water level value
    water_level_sensor_empty = 0;
    water_level_sensor_full = 0;
    water_level_empty = 0;
    water_level_full = 0;
    water_level_cloud = water_level; // Store the value for the upload to the cloud
  }

  //void iot_grower::updateCloud() { ArduinoCloud.update(); }

  void iot_grower::activateLamp() { PORTD &= ~(1 << 6); }

  void iot_grower::deactivateLamp() { PORTD |= (1 << 6); }
} // namespace iot_grower

void setup() {
   iot_grower::iot_grower();
}

void loop() {}
