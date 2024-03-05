#include <Wire.h>

int Empfaenger = 2;

int TemperatureINT;
int HumidityINT;
int Moisture;
int Light;
int WaterLevel;

void setup()
{
    // Initialize I2C communication
    Wire.begin();
}

void loop()
{
    Start = millis();
    // Send sensor data via I2C
    sendData();
    End = millis();
    Test_Time = End - Start;
    Serial.println("Test Time: ");
    Serial.println(Test_Time);
}

// Send sensor data via I2C
void sendData()
{
    byte dataToSend[] = {HumidityINT, TemperatureINT, Moisture, Light, WaterLevel};
    int dataSize = sizeof(dataToSend);

    for (int i = 0; i < dataSize; i++)
    {
        Wire.beginTransmission(Empfaenger);
        Wire.write(dataToSend[i]);
        Wire.endTransmission();
    }
}
