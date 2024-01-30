// Includes all the libraries used in the project
#include <DHT.h>   // DHT sensor library from Adafruit
#include <DHT_U.h> // DHT sensor library from Adafruit
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <Wire.h>

// Defines the LCD display
/**
 * @brief LiquidCrystal_I2C class for controlling LCD displays using I2C
 * communication.
 *
 * The LiquidCrystal_I2C class provides a convenient interface for controlling
 * LCD displays that use I2C communication. It allows you to initialize the LCD
 * display with the I2C address and the number of columns and rows. You can then
 * use various methods to control the display, such as writing text, setting the
 * cursor position, and controlling the backlight.
 *
 * @param address The I2C address of the LCD display.
 * @param columns The number of columns in the LCD display.
 * @param rows The number of rows in the LCD display.
 */
LiquidCrystal_I2C lcd(0x27, 16, 2);

/**
 * @brief This function is called once at the beginning of the program.
 * It is used to initialize the necessary components and variables.
 */
void setup() {
  // Initializes the serial monitor
  Serial.begin(9600);
  // Initializes the LCD display
  lcd.init();
  lcd.backlight();
  lcd.scrollDisplayRight();
  lcd.autoscroll();
  // Sets the pins to input or output
  DDRD &= ~(1 << DDD7);   // Sets the pin 7 to input
  PORTD |= (1 << PORTD7); // Sets the pin 7 to high
  DDRB &= ~(1 << DDB2);   // Sets the pin 10 to input
  DDRB &= ~(1 << DDB4);   // Sets the pin 12 to input
}

/**
 * @brief Reads the temperature from a sensor.
 *
 * This function is responsible for reading the temperature from a sensor and
 * performing any necessary calculations or conversions.
 *
 * @return void
 */
void readTemperature() {
  // Defines the variables used in the function
  unsigned long currentMillis = millis();
  unsigned long previousMillis = 0;
  const long interval = 2000;

// Defines the DHT sensor
#define DHTPIN 10
#define DHTTYPE DHT11
  DHT dht(DHTPIN, DHTTYPE);
  dht.begin();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read Data from DHT sensor!");
    return;
  }
  // Defines the interval between each reading
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // Prints the temperature and humidity on the serial monitor
    Serial.print("Humidity: ");
    Serial.println(h);
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C ");
    // Prints the temperature and humidity on the LCD display
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(t);
    lcd.print(" C");
    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(h);
    lcd.print(" %");
  }
  lcd.clear();
  previousMillis = currentMillis;
}

/**
 * @brief Reads the water level.
 *
 * This function is responsible for reading the water level in the system.
 * It performs the necessary operations to retrieve the water level data.
 *
 * @return void
 */
void readWaterLevel() {
  // Defines the variables used in the function
  unsigned long currentMillis = millis();
  unsigned long previousMillis = 0;
  const long interval = 2000;
  // Defines the sensor
  int water_sensor = A0;
  int water_full = 520;
  int water_empty = 370;
  // Defines the interval between each reading
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // Reads the sensor
    int measured_value = analogRead(Sensor);
    if (measured_value <= water_full) {
      lcd.setCursor(0, 0);
      lcd.print("The water level");
      lcd.setCursor(0, 1);
      lcd.print("is: Full");
    } else if (measured_value >= water_empty) {
      lcd.setCursor(0, 0);
      lcd.print("The water level");
      lcd.setCursor(0, 1);
      lcd.print("is: Empty");
    } else {
      lcd.setCursor(0, 0);
      lcd.print("The water level");
      lcd.setCursor(0, 1);
      lcd.print("is: Medium");
    }
  }
  lcd.clear();
  previousMillis = currentMillis;
}

/**
 * @brief Reads the light level.
 *
 * This function is responsible for reading the light level in the system.
 * It performs the necessary operations to retrieve the light level data.
 *
 * @return void
 */
void readLightLevel() {
  // Defines the variables used in the function
  unsigned long currentMillis = millis();
  unsigned long previousMillis = 0;
  const long interval = 2000;
// Defines the sensor
#define LightSensor A1
  // Defines the interval between each reading
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // Reads the sensor
    int measured_value = analogRead(A1);
    // TODO: Encode the light level in Lux
    // ...
    if (measured_value <= 100) {
      lcd.setCursor(0, 0);
      lcd.print("The light level");
      lcd.setCursor(0, 1);
      lcd.print("is: ", measured_value);
    } else if (measured_value >= 200) {
      lcd.setCursor(0, 0);
      lcd.print("The light level");
      lcd.setCursor(0, 1);
      lcd.print("is: ", measured_value);
    } else {
      lcd.setCursor(0, 0);
      lcd.print("The light level");
      lcd.setCursor(0, 1);
      lcd.print("is: ", measured_value);
    }
    // Prints the light level on the serial monitor
    Serial.print("Light level: ");
    Serial.println(measured_value);
  }
  lcd.clear();
  previousMillis = currentMillis;
}

/**
 * @brief Reads the soil moisture.
 *
 * This function is responsible for reading the soil moisture in the system.
 * It performs the necessary operations to retrieve the soil moisture data.
 *
 * @return void
 */
void readSoilMoisture() {
  // Defines the variables used in the function
  unsigned long currentMillis = millis();
  unsigned long previousMillis = 0;
  const long interval = 2000;
// Defines the sensor
#define SoilMoistureSensor A2
  // Defines the interval between each reading
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // Reads the sensor
    int measured_value = analogRead(A2);
    if (measured_value <= 100) {
      lcd.setCursor(0, 0);
      lcd.print("The soil moisture");
      lcd.setCursor(0, 1);
      lcd.print("is: ", measured_value);
    } else if (measured_value >= 200) {
      lcd.setCursor(0, 0);
      lcd.print("The soil moisture");
      lcd.setCursor(0, 1);
      lcd.print("is: ", measured_value);
    } else {
      lcd.setCursor(0, 0);
      lcd.print("The soil moisture");
      lcd.setCursor(0, 1);
      lcd.print("is: ", measured_value);
    }
    // Prints the soil moisture on the serial monitor
    Serial.print("Soil moisture: ");
    Serial.println(measured_value);
  }
  lcd.clear();
  previousMillis = currentMillis;
}