int Sensor = A3;

void setup()
{
Serial.begin(9600);
}

void loop()
{
int messwert = analogRead(Sensor);
Serial.println(messwert);
delay(1000);
}