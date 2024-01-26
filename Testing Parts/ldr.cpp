int LDR = A1;
int LDRValue = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LDR, INPUT);
}

void loop() {
  delay(1000);
  LDRValue = analogRead(LDR);
  Serial.print("LDR Value: ");
  Serial.print(LDRValue);
  delay(1000);
}