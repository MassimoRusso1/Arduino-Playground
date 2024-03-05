
volatile bool bSensorsEvaluated = true;

// Sensor variables
int MoistureSensor;
int LightSensor = 99;
int WaterLevelSensor;

// Processed sensor values
int Moisture = 19;
int Light;
int WaterLevel = 80;
int PumpTime;

// Temperature and humidity variables
float Humidity = 62;
float Temperature = 26;

void setup()
{
    // Initialize serial communication
    Serial.begin(9600);

    // Set pins for actuators
    DDRD |= (1 << 6); // Light
    PORTD |= (1 << 6);
    DDRD |= (1 << 4); // Fan
    PORTD &= ~(1 << 4);
    DDRD |= (1 << 7); // Pump
    PORTD &= ~(1 << 7);
}

void loop()
{
    Start = millis();
    if (bSensorsEvaluated)
    {
        updateActuators();
        bSensorsEvaluated = false;
    }
    End = millis();
    Test_Time = End - Start;
    Serial.println("Test Time: ");
    Serial.println(Test_Time);
}

// Toggle light actuator
void toggleLight()
{
    if (LightSensor < 100)
    {
        PORTD &= ~(1 << 6);
    }
    else if (LightSensor >= 100)
    {
        PORTD |= (1 << 6);
    }
}

// Toggle pump actuator
void togglePump()
{
    if (Moisture < 20 && WaterLevel > 30)
    {
        int MoistureFactor = map(Moisture, 0, 100, 500, 1000);
        int WaterLevelFactor = map(WaterLevel, 0, 100, 1, 5);
        PumpTime = MoistureFactor * WaterLevelFactor;
        if (PumpTime > 1000)
        {
            PumpTime = 1000;
        }
        unsigned long previousMillis = millis();
        while (millis() - previousMillis < PumpTime)
        {
            PORTD |= (1 << 7);
        }
        PORTD &= ~(1 << 7);
    }
    else if (Moisture >= 50 || WaterLevel <= 30)
    {
        return;
    }
    else if (Moisture >= 20 && Moisture <= 50 && WaterLevel > 20)
    {
        PumpTime = 500;
        unsigned long previousMillis = millis();
        while (millis() - previousMillis < PumpTime)
        {
            PORTD |= (1 << 7);
        }
        PORTD &= ~(1 << 7);
    }
}

// Toggle fan actuator
void toggleFan()
{
    if (Temperature > 25 || Humidity > 60)
    {
        PORTD |= (1 << 4);
    }
    else if (Temperature < 25 && Humidity < 60)
    {
        PORTD &= ~(1 << 4);
    }
}
