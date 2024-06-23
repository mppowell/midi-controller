const int keyClrPin = 0;
const int keySerPin = 1;
const int keyClkPin = 2;
const int row1Pin = 35;
const int row2Pin = 34;
const int row3Pin = 33;

void setup() {
  // put your setup code here, to run once:

  pinMode(keyClrPin, OUTPUT);
  pinMode(keySerPin, OUTPUT);
  pinMode(keyClkPin, OUTPUT);

  pinMode(row1Pin, INPUT_PULLDOWN);
  pinMode(row2Pin, INPUT_PULLDOWN);
  pinMode(row3Pin, INPUT_PULLDOWN);

  Serial.begin(9600);

  digitalWrite(keySerPin, LOW);
  digitalWrite(keyClkPin, LOW);
  digitalWrite(keyClrPin, HIGH);
  digitalWrite(keyClrPin, LOW);
  digitalWrite(keyClrPin, HIGH);

  digitalWrite(keySerPin, HIGH);
  digitalWrite(keyClkPin, HIGH);
  digitalWrite(keyClkPin, LOW);
  digitalWrite(keySerPin, LOW);
  digitalWrite(keyClkPin, HIGH);
  digitalWrite(keyClkPin, LOW);
  digitalWrite(keyClkPin, HIGH);
  digitalWrite(keyClkPin, LOW);
 
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.print(digitalRead(row1Pin));
  Serial.print('\t');
  Serial.print(digitalRead(row2Pin));
  Serial.print('\t');
  Serial.println(digitalRead(row3Pin));

  delay(10);


}
