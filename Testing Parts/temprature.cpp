#include <DHT.h>
#include <DHT_U.h>



#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(9600);
    Serial.println("DHT11 test!");
    dht.begin();
}

void loop() {
  
  delay(2000);

  
  float Luft = dht.readHumidity();
  float Temp = dht.readTemperature();
  
  Serial.print("Luftfeuchtigkeit: ");
  Serial.print(Luft);
  Serial.println(" %");
  Serial.print("Temperatur: ");
  Serial.print(Temp);
  Serial.println(" Grad Celsius");

}