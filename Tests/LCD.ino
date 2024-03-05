#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
    // Initialize serial communication
    Serial.begin(9600);

    // Initialize LCD
    lcd.init();
    lcd.backlight();

    // Enable external interrupt for button
    DDRD &= ~(1 << DDD2);   //PD2 on input mode
    EICRA |= (1 << ISC00);  //any logic change
    PORTD |= (1 << PORTD2); //pull-up enable
    EIMSK |= (1 << INT0);   //turns on INT0

    // Initialize flags
    bButtonPressed = false;
}

// Interrupt service routine for button press
ISR(INT0_vect)
{
    bButtonPressed = true;
}

void loop()
{
    Start = millis();
    // Update LCD if button is pressed
    if (bButtonPressed)
    {
        updateLcd();
        bButtonPressed = false;
    }
    End = millis();
    Test_Time = End - Start;
    Serial.println("Test Time: ");
    Serial.println(Test_Time);
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

