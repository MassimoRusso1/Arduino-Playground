#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int button = 7;

#define DHTPIN 10
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int Sensor = A0;
int alterMesswert = 0;
char printBuffer[100];

int LDR = A2;
int LDRValue = 0;

const char* ssid = "Massimo";
const char* password = "12345678";

void setup()
{
    Serial.begin(9600);
    dht.begin();
    lcd.init();
    lcd.backlight();
    pinMode(button, INPUT_PULLUP);
    pinMode(Sensor, INPUT);
    pinMode(LDR, INPUT);

}

void loop()
{ 
    Output();
    delay(2000);
    if (button == HIGH)
    {
        pump();
    }
}

void Einrichtung()
{

}

void Output()
{
    lcd.setCursor(0,0);
    lcd.print("The output is:");
    lcd.setCursor(0,1);
    lcd.print("ON");
    delay(2000);
    lcd.clear();

    Temperature();
    delay(2000);
    lcd.clear();

    Air_Humidity();
    delay(2000);
    lcd.clear();

    Wasserstand();
    delay(2000);
    lcd.clear();

    Licht();
    delay(2000);
    lcd.clear();

}

void Temperature()
{
    float Temperature = dht.readTemperature();
    lcd.setCursor(0, 0);
    lcd.print("The Temperature");
    lcd.setCursor(0, 1);
    lcd.print("is:");
    lcd.print(Temperature);

    Serial.print("Temperature: ");
    Serial.println(Temperature);
}

void Air_Humidity()
{
    float Humidity = dht.readHumidity();
    lcd.setCursor(0, 0);
    lcd.print("The Air Humidity");
    lcd.setCursor(0, 1);
    lcd.print("is:");
    lcd.print(Humidity);

    Serial.print("Humidity: ");
    Serial.println(Humidity);
}

void Wasserstand()
{
    int measured_value = analogRead(Sensor);
    int water_full = 520;
    int water_empty = 370;

    if (measured_value <= water_full)
    {
        lcd.setCursor(0, 0);
        lcd.print("The water level");
        lcd.setCursor(0, 1);
        lcd.print("is: Full");
    }
    else if (measured_value <= water_empty)
    {
        lcd.setCursor(0, 0);
        lcd.print("The water level");
        lcd.setCursor(0, 1);
        lcd.print("is: Empty");
        buzzer();
    }
    else
    {
        lcd.setCursor(0, 0);
        lcd.print("The water level");
        lcd.setCursor(0, 1);
        lcd.print("is: OK");
    }
}

void Licht()
{
    Serial.println("Check");
    LDRValue = analogRead(LDR);
    lcd.setCursor(0, 0);
    lcd.print("The light level");
    lcd.setCursor(0, 1);
    lcd.print("is:");
    lcd.print(LDRValue);
}

void connectToWiFi()
{
    WiFi.begin(const_cast<char*>(ssid), const_cast<char*>(password));

    while (WiFi.status() != WL_CONNECTED)
    {
        lcd.setCursor(0,0);
        lcd.print("Connecting");
        delay(500);
        lcd.clear();
    }
    lcd.setCursor(0, 0);
    lcd.print("Connected to:");
    lcd.setCursor(0, 1);
    lcd.print(ssid);
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IP Address:");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(2000);
    lcd.clear();
}

void buzzer()
{
    int buzzer = 8;
    pinMode(buzzer, OUTPUT);
    digitalWrite(buzzer, HIGH);
    delay(1000);
    digitalWrite(buzzer, LOW);
    delay(1000);
}

void feuchtigkeit()
{
    int feuchtigkeit = 0;
    int feuchtigkeitPin = A2;
    feuchtigkeit = analogRead(feuchtigkeitPin);
    Serial.println(feuchtigkeit);
    delay(1000);
}

void pump ()
{
    int pump = 0;
    pinMode(pump, OUTPUT);
    digitalWrite(pump, HIGH);
    delay(1000);
    digitalWrite(pump, LOW);
}