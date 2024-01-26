

void setup()
{
    Serial.begin(9600);
    pinMode(13, INPUT_PULLUP);
}

void loop()
{
    if (digitalRead(13) == LOW)
    {
        Serial.println("Button is pressed");
        tone(11, 40, 1000);
    }
    else
    {
        Serial.println("Button is not pressed");
    }
    delay(1000);
}

