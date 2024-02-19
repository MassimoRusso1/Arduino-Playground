
void setup()
{
    Serial.begin(9600);
}

void loop()
{
    tone(11, 40, 1000);
    tone(10, 40, 1000);
    delay(1000);
}