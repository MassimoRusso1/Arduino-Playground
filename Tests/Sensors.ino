#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 5
#define DHTTYPE DHT11

// Flags for sensor readings and button press
volatile bool bNewSensorValue;
volatile bool bConverted;
volatile int SensorValues[3];
volatile int i;
volatile bool bSensorsRead;
volatile bool bSensorsEvaluated;

// Sensor variables
int MoistureSensor;
int LightSensor;
int WaterLevelSensor;

// Processed sensor values
int Moisture;
int Light;
int WaterLevel;

// Temperature and humidity variables
float Humidity;
float Temperature;

void setup()
{
    // Initialize serial communication
    Serial.begin(9600);

    // Initialize I2C communication
    Wire.begin();

    // Set pins for actuators
    DDRD |= (1 << 6); // Light
    PORTD |= (1 << 6);
    DDRD |= (1 << 4); // Fan
    PORTD &= ~(1 << 4);
    DDRD |= (1 << 7); // Pump
    PORTD &= ~(1 << 7);

    // Configure analog pins as inputs
    DDRC &= ~(1 << PC0); // A0
    DDRC &= ~(1 << PC1); // A1
    DDRC &= ~(1 << PC2); // A2

    // Configure Timer1
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    OCR1A = 15624;
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
}
// Interrupt service routine for Timer1
ISR(TIMER1_COMPA_vect)
{
    if (bConverted)
    {
        ADCSRA |= bit(ADSC) | bit(ADIE);
        bConverted = false;
    }
}

// Interrupt service routine for ADC
ISR(ADC_vect)
{
    SensorValues[i] = ADC;
    i = (i + 1) % 3;        // Make sure i stays within range 0 to 2
    ADMUX = 0b01000000 | i; // Select the next ADC channel
    if (i == 0)
    {
        bNewSensorValue = true;
        bConverted = true;
    }
    else
    {
        ADCSRA |= bit(ADSC); // Start ADC conversion for the next channel
    }
}

void loop()
{
    Start = millis();
    // Process new sensor values if available
    if (bNewSensorValue)
    {
        processSensorValues();
        bNewSensorValue = false;
        bSensorsRead = true;
    }

    // Evaluate sensor readings
    if (bSensorsRead)
    {
        EvaluateSensorValues();
        bSensorsRead = false;
        bSensorsEvaluated = true;
    }
    End = millis();
    Test_Time = End - Start;
    Serial.println("Test Time: ");
    Serial.println(Test_Time);
}

// Process raw sensor values
void processSensorValues() {
  for (int i = 0; i < 3; i++) {
    MoistureSensor = SensorValues[0];
    LightSensor = SensorValues[1];
    WaterLevelSensor = SensorValues[2];
  }
}

void EvaluateSensorValues() {
  Moisture = map(MoistureSensor, 1022, 500, 0, 100);
  Light = LightSensor / 100;
  WaterLevel = map(WaterLevelSensor, 421, 589, 0, 100);
  readDHT();
}

void readDHT()
{
    DHT dht(DHTPIN, DHTTYPE);
    dht.begin();
    Humidity = dht.readHumidity();
    Temperature = dht.readTemperature();
    if (isnan(Humidity) || isnan(Temperature))
    {
        return;
    }
    HumidityINT = int(Humidity);
    TemperatureINT = int(Temperature);
}