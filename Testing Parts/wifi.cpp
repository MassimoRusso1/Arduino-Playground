//Scan for WiFi networks
#include <WiFiS3.h>

void setup() {
    Serial.begin(9600);
    while (!Serial)
    {
        ; 
    }
    if (WiFi.status()) == WL_NO_MODULE)
    {
        Serial.println("Communication with WiFi module failed!");
        while (true);
    }
    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION)
    {
        Serial.println("Please upgrade the firmware");
    }
    Serial.println();
    Serial.println("Scanning available networks...");
    listNetworks();

    byte mac[6];
    WiFi.macAddress(mac);
    Serial.print("MAC: ");
    printMacAddress(mac);

}

void loop() {
    delay(10000);
    Serial.println("Scanning available networks...");
    listNetworks();
}

void listNetworks() {
    Serial.println(** Scan Networks **);
    int numSsid = WiFi.scanNetworks();
    if (numSsid == -1)
    {
        Serial.println("Couldn't get a wifi connection");
        while (true);
    }

    Serial.print("Number of available networks:");
    Serial.println(numSsid);

    for (int thisNet = 0; thisNet < numSsid; thisNet++)
    {
        Serial.print(thisNet+1);
        Serial.print(") ");
        Serial.print("Signal: ");
        Serial.print(WiFi.RSSI(thisNet));
        Serial.print(" dBm");
        Serial.print("\tChannel: ");
        Serial.print(WiFi.channel(thisNet));
        byte bssid[6];
        Serial.print("\tBSSID: ");
        printMacAddress(WiFi.BSSID(thisNet, bssid));
        Serial.print("\tEncryption: ");
        printEncryptionType(WiFi.encryptionType(thisNet));
        Serial.print("\tSSID: ");
        Serial.println(WiFi.SSID(thisNet));
        Serial.flush();
    }
    Serial.println();
}

void printEncryptionType(int thisType) {

  switch (thisType) {

    case ENC_TYPE_WEP:

      Serial.print("WEP");

      break;

    case ENC_TYPE_WPA:

      Serial.print("WPA");

      break;

    case ENC_TYPE_WPA2:

      Serial.print("WPA2");

      break;

    case ENC_TYPE_WPA3:

      Serial.print("WPA3");

      break;  

    case ENC_TYPE_NONE:

      Serial.print("None");

      break;

    case ENC_TYPE_AUTO:

      Serial.print("Auto");

      break;

    case ENC_TYPE_UNKNOWN:

    default:

      Serial.print("Unknown");

      break;

  }

void print2Digits(byte thisByte) {

  if (thisByte < 0xF) {

    Serial.print("0");

  }

  Serial.print(thisByte, HEX);

}

void printMacAddress(byte mac[]) {

  for (int i = 0; i < 6; i++) {

    if (i > 0) {

      Serial.print(":");

    }

    if (mac[i] < 16) {

      Serial.print("0");

    }

    Serial.print(mac[i], HEX);

  }

  Serial.println();

}

// Connect to a preferred WiFi network
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <Wire.h>

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2) int buttonPin = 0;
const char* ssid = "TP-LINK_387D";
const char* password = "79810609";

void setup()
{
    Serial.begin(9600);
    pinMode(buttonPin, INPUT);
    lcd.init();
    lcd.backlight();
    dht.begin();
    Serial.println("Setup done");
}

void loop()
{
    lcd.setCursor(0, 0);
    lcd.print("Hello, User!");
    lcd.setCursor(0, 1);
    lcd.print("Press the button to connect to WiFi");
    if (buttonPin == HIGH)
    {
        connectToWifi("ssid", "password");
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("press the button to see the temperature");
    if (buttonPin == HIGH)
    {
        getTemperature();
    }
}

void connectToWifi(const char* ssid, const char* password)
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(250);
    }
    lcd.print("Connected to WiFi");
    lcd.print("IP address: ");
    lcd.print(WiFi.localIP());
}

void getTemperature()
{
    for (size_t i = 0; i < 10; i++)
    {
        deleay(2000);
        float Luft = dht.readHumidity();
        float Temp = dht.readTemperature();
        lcd.print("Luftfeuchtigkeit: ");
        lcd.print(Luft);
        lcd.println(" %");
        lcd.print("Temperatur: ");
        lcd.print(Temp);
        lcd.println(" Grad Celsius");
    }
}

//Simple Wifi connection to a network
#include <WiFi.h>

const char* ssid = "TP-LINK_387D";
const char* password = "79810609";

void setup()
{
    Serial.begin(9600);
    connectToWiFi();
}

void loop()
{
}

void connectToWiFi()
{
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(250);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("Erfolgreich mit dem WLAN verbunden!");
    Serial.print("IP-Adresse: ");
    Serial.println(WiFi.localIP());
}