int Sensor = A3;
// For the test set up calibrate the sensor to the water level
int empty_sensor = 369; // 500ml
int full_sensor =  384; // 1250ml
int empty = 500;
int full = 1250;
int mml = (full - empty)/(full_sensor - empty_sensor);

void setup()
{
Serial.begin(9600);
Serial.println(mml);
}

void loop()
{
int messwert = analogRead(Sensor);
if (messwert < empty)
{
Serial.println("Please fill the tank immediately!");
int water = (messwert - empty)*mml;
Serial.println("Water level is: ");
Serial.println(water);
}
else if (messwert >= empty && messwert < full)
{   
Serial.println("The water level is ok.");
int water = (messwert - empty)*mml;
Serial.println("Water level is: ");
Serial.println(water);
}
else if (messwert >= full)
{
Serial.println("The tank is full.");
int water = (messwert - empty)*mml;
Serial.println("Water level is: ");
Serial.println(water);
}
}

