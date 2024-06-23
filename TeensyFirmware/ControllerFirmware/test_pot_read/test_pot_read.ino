const int potSelPin0 = 26;
const int potSelPin1 = 27;
const int potSelPinEn = 28;

long int val1;
long int val2;

void setup() {
  // put your setup code here, to run once:
  pinMode(potSelPin0, OUTPUT);
  pinMode(potSelPin1, OUTPUT);
  pinMode(potSelPinEn, OUTPUT);

  analogReadResolution(12);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  
  digitalWrite(potSelPin0, LOW);
  digitalWrite(potSelPin1, LOW);
  digitalWrite(potSelPinEn, HIGH);
  //delay(10);

  val1 = analogRead(A0);
  val2 = analogRead(A1);

  Serial.print(val1);
  Serial.print(',');
  Serial.print(val2);
  Serial.print(',');
  

  
  digitalWrite(potSelPin0, HIGH);
  digitalWrite(potSelPin1, LOW);
  digitalWrite(potSelPinEn, HIGH);
  //delay(10);

  val1 = analogRead(A0);
  val2 = analogRead(A1);

  Serial.print(val1);
  Serial.print(',');
  Serial.print(val2);
  Serial.print(',');


  Serial.println();

  delay(100);


}
