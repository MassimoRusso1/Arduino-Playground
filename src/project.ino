#include "project.h"

namespace iot_grower
{
  iot_grower myGrower;
  LiquidCrystal_I2C lcd(0x27, 16, 2);

  // Status-Flags
  volatile bool buttonFlag = false;
  volatile bool executeReadDHT = false;
  volatile bool executeReadLight = false;
  volatile bool executeReadMoisture = false;
  volatile bool executeReadWaterLevel = false;

  iot_grower::iot_grower()
  {
    // Initialisierung der seriellen Kommunikation
    Serial.begin(9600);

    // Setzen der Pin-Modi
    DDRD |= (1 << 6);  // Setzen des Lampen-Pins als Ausgang
    PORTD |= (1 << 6); // Lampen-Pin auf HIGH setzen

    // Setzen des Lüfter Pins
    DDRD |= (1 << 3);
    PORTD |= (1 << 3);

    // Button Setup
    DDRD &= ~(1 << BUTTON_PIN); // Setzen des Button-Pins als Eingang
    PORTD |= (1 << BUTTON_PIN); // Pull-up-Widerstand aktivieren
    PCICR |= (1 << PCIE2);      // Pin Change Interrupt für Port D aktivieren
    PCMSK2 |= (1 << PCINT19);   // Pin Change Interrupt für Button-Pin aktivieren

    // Initialisierung des LCD-Bildschirms
    lcd.init();
    lcd.backlight();
    lcd.noDisplay(); // Display nach Initialisierung ausschalten

    cli();
    // Timer-Konfiguration
    unsigned long timer_overflow_count1 = (600 * 15625); // Calculate the timer overflow count for 10 minutes
    unsigned long timer_overflow_count2 = (1200 * 3125); // Calculate the timer overflow count for 20 minutes
    unsigned long timer_overflow_count3 = 15999;         // TImer overflow count for 1 millisecond
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

    TCCR0A = 0;
    TCCR0B = 0;
    TCNT0 = 0;
    OCR0A = 6249;
    TCCR0B |= (1 << WGM12);
    TCCR0B |= (1 << CS02) | (1 << CS00);
    TIMSK0 |= (1 << OCIE0A);
    // Globale Interrupts aktivieren
    sei();
  }

  ISR(TIMER1_COMPA_vect)
  {
    executeReadLight = true;
    executeReadMoisture = true;
  }

  ISR(TIMER2_COMPA_vect)
  {
    executeReadDHT = true;
    executeReadWaterLevel = true;
  }

  ISR(TIMER0_COMPA_vect)
  {
    buttonFlag = true;
    myGrower.executeTasks();
  }

  void iot_grower::button_interrupt()
  {
    if (!(PIND & (1 << BUTTON_PIN)))
    {
      lcd.display(); // Display einschalten
      const long interval = 2000;
      int values[] = {humiditySensor, temperatureSensor, light, moisture, waterLevel};
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
        while (millis() - previousMillis < interval)
        {
        }
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
    // Define the variables used in the function
    DHT dht(DHTPIN, DHTTYPE);
    // Read the temperature and humidity values
    humiditySensor = dht.readHumidity();
    temperatureSensor = dht.readTemperature();
    // Check if the values are valid
    if (isnan(humiditySensor) || isnan(temperatureSensor))
    {
      return;
    }
    // Send the values to the cloud
    Serial.println("H");
    Serial.println(humiditySensor);
    Serial.println("T");
    Serial.println(temperatureSensor);
  }

  void iot_grower::readLight()
  {
    // Define the variables used in the function
    const int analogInPin = A1;
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
    lightSensor = (high << 8) | low;
    // Evaluate the light value
    if (lightSensor < 100)
    {
      activateLamp();
    }
    else if (lightSensor < 200)
    {
      activateLamp();
    }
    else if (lightSensor < 300)
    {
      deactivateLamp();
    }
    else
    {
      deactivateLamp();
    }
    // Evaluate the light value to lux
    light = lightSensor;
    // Send the value to the cloud
    Serial.println("L");
    Serial.println(light);
  }

  void iot_grower::readMoisture()
  {
    // Define the variables used in the function
    const int analogInPin = A0;
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
    moistureSensor = (high << 8) | low;
    // Evaluate the moisture value to percentage
    moisture = moistureSensor;

    // Send the value to the cloud
    Serial.println("M");
    Serial.println(moisture);
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
    waterLevelSensor = (high << 8) | low;
    // Evaluate the water level value
    waterLevelSensorEmpty = 0;
    waterLevelSensorFull = 0;
    waterLevelEmpty = 0;
    waterLevelFull = 0;
    waterLevel = waterLevelSensor;
  }

  void iot_grower::activateLamp()
  {
    PORTD &= ~(1 << 6);
  }

  void iot_grower::deactivateLamp()
  {
    PORTD |= (1 << 6);
  }

  void iot_grower::activatePump()
  {
    PORTD &= ~(1 << 7);
  }

  void iot_grower::deactivatePump()
  {
    PORTD |= (1 << 7);
  }

  void iot_grower::executeTasks()
  {
    if (buttonFlag)
    {
      myGrower.button_interrupt();
      buttonFlag = false; // Flag zurücksetzen
    }
    if (executeReadDHT)
    {
      myGrower.readDHT();
      executeReadDHT = false; // Flag zurücksetzen
    }
    if (executeReadLight)
    {
      myGrower.readLight();
      executeReadLight = false; // Flag zurücksetzen
    }
    if (executeReadMoisture)
    {
      myGrower.readMoisture();
      executeReadMoisture = false; // Flag zurücksetzen
    }
    if (executeReadWaterLevel)
    {
      myGrower.readWaterLevel();
      executeReadWaterLevel = false; // Flag zurücksetzen
    }
  }
} // namespace iot_grower

void setup() { iot_grower::myGrower = iot_grower::iot_grower(); }

void loop() {}
