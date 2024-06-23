long int val1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //analogReadResolution(12);
}

void loop() {
  // put your main code here, to run repeatedly:
  val1 = analogRead(A6);
  Serial.println(val1);
  delay(50);
}
