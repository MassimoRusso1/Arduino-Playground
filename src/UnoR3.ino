#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>

#define DHTPIN 5
#define DHTTYPE DHT11

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Flags for sensor readings and button press
volatile bool bNewSensorValue;
volatile bool bConverted;
volatile int SensorValues[3];
volatile int i;
volatile bool bSensorsRead;
volatile bool bSensorsEvaluated;
volatile bool bButtonPressed;

// Sensor variables
int MoistureSensor;
int LightSensor;
int WaterLevelSensor;

// Processed sensor values
int Moisture;
int Light;
int WaterLevel;
int PumpTime;
int Empfaenger = 2;
int TemperatureINT;
int HumidityINT;

// Temperature and humidity variables
float Humidity;
float Temperature;

// Time variables
unsigned long Start;
unsigned long End;
unsigned long LoopTime;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Initialize I2C communication
  Wire.begin();

  // Set pins for actuators
  DDRD |= (1 << 6);  // Light
  PORTD |= (1 << 6);
  DDRD |= (1 << 4);  // Fan
  PORTD &= ~(1 << 4);
  DDRD |= (1 << 7);  // Pump
  PORTD &= ~(1 << 7);

  // Set pin for button
  DDRD &= ~(1 << 5);

  // Configure analog pins as inputs
  DDRC &= ~(1 << PC0);  // A0
  DDRC &= ~(1 << PC1);  // A1
  DDRC &= ~(1 << PC2);  // A2

  // Enable external interrupt for button
  DDRD &= ~(1 << DDD2);    //PD2 on input mode
  EICRA |= (1 << ISC00);   //any logic change
  PORTD |= (1 << PORTD2);  //pull-up enable
  EIMSK |= (1 << INT0);    //turns on INT0

  // Configure Timer1
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 62499;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);

  // Configure ADC
  ADCSRA = 0b10000111;
  ADMUX = 0b01000000;

  // Enable global interrupts
  sei();

  // Initialize flags
  bConverted = true;
  bNewSensorValue = false;
  i = 0;
  bSensorsRead = false;
  bSensorsEvaluated = false;
  bButtonPressed = false;
}

// Interrupt service routine for Timer1
ISR(TIMER1_COMPA_vect) {
  if (bConverted) {
    ADCSRA |= bit(ADSC) | bit(ADIE);
    bConverted = false;
  }
}

// Interrupt service routine for ADC
ISR(ADC_vect) {
  SensorValues[i] = ADC;
  i = (i + 1) % 3;         // Make sure i stays within range 0 to 2
  ADMUX = 0b01000000 | i;  // Select the next ADC channel
  if (i == 0) {
    bNewSensorValue = true;
    bConverted = true;
  } else {
    ADCSRA |= bit(ADSC);  // Start ADC conversion for the next channel
  }
}

// Interrupt service routine for button press
ISR(INT0_vect) {
  bButtonPressed = true;
}

void loop() {

  // Process new sensor values if available
  if (bNewSensorValue) {
    processSensorValues();
    bNewSensorValue = false;
    bSensorsRead = true;
  }

  // Evaluate sensor readings
  if (bSensorsRead) {
    EvaluateSensorValues();
    bSensorsRead = false;
    bSensorsEvaluated = true;
  }

  // Toggle actuators and send data if sensors are evaluated
  if (bSensorsEvaluated) {
    toggleLight();
    togglePump();
    toggleFan();
    sendData();
    bSensorsEvaluated = false;
  }

  // Update LCD if button is pressed
  if (bButtonPressed) {
    updateLcd();
    bButtonPressed = false;
  }

}

// Process raw sensor values
void processSensorValues() {
  for (int i = 0; i < 3; i++) {
    MoistureSensor = SensorValues[0];
    LightSensor = SensorValues[1];
    WaterLevelSensor = SensorValues[2];
  }
}

// Read DHT sensor
void readDHT() {
  DHT dht(DHTPIN, DHTTYPE);
  dht.begin();
  Humidity = dht.readHumidity();
  Temperature = dht.readTemperature();
  if (isnan(Humidity) || isnan(Temperature)) {
    return;
  }
  HumidityINT = int(Humidity);
  TemperatureINT = int(Temperature);
}

// Evaluate processed sensor values
void EvaluateSensorValues() {
  Moisture = map(MoistureSensor, 1022, 500, 0, 100);
  Light = LightSensor / 100;
  WaterLevel = map(WaterLevelSensor, 421, 589, 0, 100);
  readDHT();
}

// Toggle light actuator
void toggleLight() {
  if (LightSensor < 100) {
    PORTD &= ~(1 << 6);
  } else if (LightSensor >= 100) {
    PORTD |= (1 << 6);
  }
}

// Toggle pump actuator
void togglePump() {
  if (Moisture < 20 && WaterLevel > 30) {
    int MoistureFactor = map(Moisture, 0, 100, 500, 1000);
    int WaterLevelFactor = map(WaterLevel, 0, 100, 1, 5);
    PumpTime = MoistureFactor * WaterLevelFactor;
    if (PumpTime > 1000) {
      PumpTime = 1000;
    }
    unsigned long previousMillis = millis();
    while (millis() - previousMillis < PumpTime) {
      PORTD |= (1 << 7);
    }
    PORTD &= ~(1 << 7);
  } else if (Moisture >= 50 || WaterLevel <= 30) {
    return;
  } else if (Moisture >= 20 && Moisture <= 50 && WaterLevel > 20) {
    PumpTime = 500;
    unsigned long previousMillis = millis();
    while (millis() - previousMillis < PumpTime) {
      PORTD |= (1 << 7);
    }
    PORTD &= ~(1 << 7);
  }
}

// Toggle fan actuator
void toggleFan() {
  if (Temperature > 25 || Humidity > 60) {
    PORTD |= (1 << 4);
  } else if (Temperature < 25 && Humidity < 60) {
    PORTD &= ~(1 << 4);
  }
}

// Send sensor data via I2C
void sendData() {
  byte dataToSend[] = { HumidityINT, TemperatureINT, Moisture, Light, WaterLevel };
  int dataSize = sizeof(dataToSend);

  for (int i = 0; i < dataSize; i++) {
    Wire.beginTransmission(Empfaenger);
    Wire.write(dataToSend[i]);
    Wire.endTransmission();
  }
}

// Update LCD display
void updateLcd() {
  lcd.clear();
  int LcdData[] = { Moisture, Light, WaterLevel, Temperature, Humidity };
  const char *LcdLabel[] = { "Moisture: ", "Light Level: ", "Water Level", "Temperature: ", "Humidity: " };
  const char *LcdUnit[] = { "%", " ", "%", "C", "%" };
  for (int i = 0; i < 5; i++) {
    lcd.setCursor(0, 0);
    lcd.print(LcdLabel[i]);
    lcd.setCursor(0, 1);
    lcd.print(LcdData[i]);
    lcd.print(LcdUnit[i]);
        unsigned long previousMillis = millis();
    while (millis() - previousMillis < 750) {}
    lcd.clear();
  }
}
