#include "project.h"

namespace iot_grower
{
  LiquidCrystal_I2C lcd(0x27, 16, 2);
  iot_grower myGrower;

  iot_grower::iot_grower()
  {
    iot_grower myGrower;
    // Initialisierung der seriellen Kommunikation
    Serial.begin(9600);

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
  }

  void iot_grower::timer2_ISR()
  {
    myGrower.readDHT();
    myGrower.readWaterLevel();
  }

  void iot_grower::button_interrupt()
  {
    iot_grower myGrower; // Instanz der Klasse erstellen
    if (!(PIND & (1 << BUTTON_PIN)))
    {
      lcd.display(); // Display einschalten
      const long interval = 2000;
      int values[] = {humidity_sensor, temperature_sensor, light, moisture, water_level};
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
        else if (i == 2)
        {
          lcd.print("lx");
        }
        else if (i == 3)
        {
          lcd.print("%");
        }
        else if (i == 4)
        {
          lcd.print("%");
        }
        unsigned long previousMillis = millis();
        while (millis() - previousMillis < interval){}
        lcd.clear();
      }
    }
    else
    {
      return;
    }
    lcd.noDisplay(); // Display ausschalten
  }

  void iot_grower::readDHT()
  {
    iot_grower myGrower; // Instanz der Klasse erstellen
    // Define the variables used in the function
    DHT dht(DHTPIN, DHTTYPE);
    // Read the temperature and humidity values
    humidity_sensor = dht.readHumidity();
    temperature_sensor = dht.readTemperature();
    // Check if the values are valid
    if (isnan(humidity_sensor) || isnan(temperature_sensor))
    {
      return;
    }
    // Send the values to the cloud
    Serial.println("H");
    Serial.println(humidity_sensor);
    Serial.println("T");
    Serial.println(temperature_sensor);
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
    light_sensor = (high << 8) | low;
    // Evaluate the light value
    if (light_sensor < 100) // TODO: Change the values to the ones that are going to be
                            // used in the project
    {
      myGrower.activate_Lamp();
    }
    else if (light_sensor < 200)
    {
      myGrower.activate_Lamp();
    }
    else if (light_sensor < 300)
    {
      myGrower.deactivate_Lamp();
    }
    else
    {
      myGrower.deactivateLamp();
    }
    // Evaluate the light value to lux
    light = light_sensor;
    // Send the value to the cloud
    Serial.println("L");
    Serial.println(light);
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
    moisture_sensor = (high << 8) | low;
    // Evaluate the moisture value
    moisture = moisture_sensor;
    // Send the value to the cloud
    Serial.println("M");
    Serial.println(moisture);
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
    // Evaluate the water level value
    water_level_sensor_empty = 0;
    water_level_sensor_full = 0;
    water_level_empty = 0;
    water_level_full = 0;
    water_level_cloud = water_level; // Store the value for the upload to the cloud
  }

  void iot_grower::activate_Lamp() { PORTD &= ~(1 << 6); }
  void iot_grower::deactivate_Lamp() { PORTD |= (1 << 6); }
  void iot_grower::activate_pump() { PORTD &= ~(1 << 7); }
  void iot_grower::deactivate_pump() { PORTD |= (1 << 7); }
} // namespace iot_grower

void setup()
{
  iot_grower::iot_grower();
}

void loop() {}
