int pump = 0;

void setup()
{
    Serial.begin(9600);
    pinMode(pump, OUTPUT);
}

void loop()
{
    digitalWrite(pump, LOW);
    delay(10000);
    digitalWrite(pump, HIGH);
    delay(1000);
}
