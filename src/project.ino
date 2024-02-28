#include "project.h"

namespace iot_grower
{
  iot_grower myGrower;
  LiquidCrystal_I2C lcd(0x27, 16, 2);

  // Status-Flags
  volatile bool buttonFlag = false;
  volatile bool executeSensors = false;

  iot_grower::iot_grower()
  {
    // Initialisierung der seriellen Kommunikation
    Serial.begin(9600);

    // Initialisierung des LCD-Bildschirms
    lcd.init();
    lcd.backlight();
    lcd.print("IoT-Grower");

    // Setzen der Pin-Modi
    DDRD |= (1 << 6);  // Setzen des Lampen-Pins als Ausgang
    PORTD |= (1 << 6); // Lampen-Pin auf HIGH setzen

    // Setzen des Lüfter Pins
    DDRD |= (1 << 3);
    PORTD |= (1 << 3);

    // Setzen des Pumpen Pins
    DDRD |= (1 << 7);
    PORTD &= ~(1 << 7);

    // Setzen der Refferenzspannung
    ADMUX |= (1 << REFS0);

    // Button Setup
    DDRD &= ~(1 << BUTTON_PIN); // Setzen des Button-Pins als Eingang
    PORTD |= (1 << BUTTON_PIN); // Pull-up-Widerstand aktivieren
    PCICR |= (1 << PCIE2);      // Pin Change Interrupt für Port D aktivieren
    PCMSK2 |= (1 << PCINT19);   // Pin Change Interrupt für Button-Pin aktivieren

    cli();
    // Timer-Konfiguration
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    OCR1A = 62499;
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS12) | (1 << CS10);
    TIMSK1 |= (1 << OCIE1A);

    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = 0;
    OCR2A = 249;
    TCCR2B |= (1 << WGM12);
    TCCR2B |= (1 << CS22) | (1 << CS20);
    TIMSK2 |= (1 << OCIE2A);

    // Globale Interrupts aktivieren
    sei();

    // Initialisierung der Sensoren
    DHT dht(DHTPIN, DHTTYPE);
    dht.begin();

    lcd.clear();
  }

  ISR(TIMER1_COMPA_vect)
  {
    i++;
    if (i == 20)
    {
      executeSensors = true;
      i = 0;
    }
  }
  ISR(TIMER2_COMPA_vect)
  {
    myGrower.executeTasks();
  }

  void iot_grower::button_interrupt()
  {
    if (!(PIND & (1 << BUTTON_PIN)))
    {
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

  void iot_grower::readSensors()
  {
    humiditySensor = dht.readHumidity();
    temperatureSensor = dht.readTemperature();
    if (isnan(humiditySensor) || isnan(temperatureSensor))
    {
      return;
    }
    Serial.print("H");
    Serial.println(humiditySensor);
    Serial.print("T");
    Serial.println(temperatureSensor);

    ADMUX = (ADMUX & 0xF0);
    ADCRSA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC))
    {
    }
    lightSensor = ADC;
    // Auswerten des gemessenen Wertes und Zuordnung zu einer Helligkeitsstufe
    if (lightSensor < 100)
    {
      light = 0;
    }
    else if (lightSensor < 200)
    {
      light = 1;
    }
    else if (lightSensor < 300)
    {
      light = 2;
    }
    else if (lightSensor < 400)
    {
      light = 3;
    }
    else if (lightSensor < 500)
    {
      light = 4;
    }
    else if (lightSensor < 600)
    {
      light = 5;
    }
    else if (lightSensor < 700)
    {
      light = 6;
    }
    else if (lightSensor < 800)
    {
      light = 7;
    }
    else if (lightSensor < 900)
    {
      light = 8;
    }
    else if (lightSensor < 1000)
    {
      light = 9;
    }
    else
    {
      light = 10;
    }
    Serial.print("L");
    Serial.println(light);

    ADMUX = (ADMUX & 0xF0) | 0x01;
    ADCRSA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC))
    {
    }
    moistureSensor = ADC;
    // Auswerten des gemessenen Wertes und umrechnen in Prozent
    moisture = map(moistureSensor, 0, 1023, 100, 0);
    Serial.print("M");
    Serial.println(moisture);

    ADMUX = (ADMUX & 0xF0) | 0x02;
    ADCRSA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC))
    {
    }
    waterLevelSensor = ADC;
    // Auswerten des gemessenen Wertes und umrechnen in Prozent
    waterLevelSensorEmpty = 0;
    waterLevelSensorFull = 1023;
    waterLevel = map(waterLevelSensor, waterLevelSensorEmpty, waterLevelSensorFull, 0, 100);
    Serial.print("W");
    Serial.println(waterLevel);

    // Gießen der Pflanze
    if (moisture < 30 && waterLevel > 30)
    {
      activatePump();
      unsigned long previousMillis = millis();
      while (millis() - previousMillis < 5000)
      {
      }
      deactivatePump();
      else if (moisture > 30 && waterLevel < 30)
      {
        activatePump();
        unsigned long previousMillis = millis();
        while (millis() - previousMillis < 5000)
        {
        }
        deactivatePump();
      }
      else if (moisture < 30 && waterLevel < 30)
      {
        activatePump();
        unsigned long previousMillis = millis();
        while (millis() - previousMillis < 5000)
        {
        }
        deactivatePump();
      }
      else if (moisture > 30 && waterLevel > 30)
      {
        deactivatePump();
      }
    }
  }

  void iot_grower::waterPlant()
  {
    if (moisture < 20 && waterLevel > 80)
    {
      PORTD |= (1 << 7);
      unsigned long previousMillis = millis();
      while (millis() - previousMillis < 2000)
      {
      }
      PORTD &= ~(1 << 7);
      else if (moisture > 30 && moisture < 50 && waterLevel > 30)
      {
        PORTD |= (1 << 7);
        unsigned long previousMillis = millis();
        while (millis() - previousMillis < 5000)
        {
        }
        PORTD &= ~(1 << 7);
      }
      else if (moisture < 30 && waterLevel < 30)
      {
        activatePump();
        unsigned long previousMillis = millis();
        while (millis() - previousMillis < 5000)
        {
        }
        PORTD &= ~(1 << 7);
      }
      else if (moisture > 30 && waterLevel > 30)
      {
        PORTD &= ~(1 << 7);
      }
    }
  }

  void iot_grower::activateLamp()
  {
    PORTD &= ~(1 << 6);
  }

  void iot_grower::deactivateLamp()
  {
    PORTD |= (1 << 6);
  }

  void iot_grower::activateFan()
  {
    PORTD &= ~(1 << 3);
  }

  void iot_grower::deactivateFan()
  {
    PORTD |= (1 << 3);
  }

  void iot_grower::executeTasks()
  {
    if (executeSensors)
    {
      readSensors();
      executeSensors = false;
    }
  }
} // namespace iot_grower

void setup() { iot_grower::myGrower = iot_grower::iot_grower(); }

void loop() {}
