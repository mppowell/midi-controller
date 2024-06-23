const int encPin0 = 29;
const int encPin1 = 30;
const int encPin2 = 31;
const int encPin3 = 32;

int encoderMap[16] = {0, 10, 0, 1, 0, 11, 0, 12, 8, 9, 7, 2, 5, 4, 6, 3};

void setup() {
  // put your setup code here, to run once:
  pinMode(encPin0, INPUT);
  pinMode(encPin1, INPUT);
  pinMode(encPin2, INPUT);
  pinMode(encPin3, INPUT);

  Serial.begin(9600);

  delay(500);

}

void loop() {
  // put your main code here, to run repeatedly:
  
  /*
  Serial.print(!digitalRead(encPin0));
  Serial.print('\t');
  Serial.print(!digitalRead(encPin1));
  Serial.print('\t');
  Serial.print(!digitalRead(encPin2));
  Serial.print('\t');
  Serial.println(!digitalRead(encPin3));
  */
  
  
  Serial.println(readEncoder());
}

int readEncoder(void) {
  int val = digitalRead(encPin0);
  val = val | digitalRead(encPin1) << 1;
  val = val | digitalRead(encPin2) << 2;
  val = val | digitalRead(encPin3) << 3;

  return encoderMap[val];

  /* Gray code converstion
  int pin0Val = !digitalRead(encPin0);
  int pin1Val = !digitalRead(encPin1);
  int pin2Val = !digitalRead(encPin2);
  int pin3Val = !digitalRead(encPin3);

  // Convert gray code to binary
  pin2Val ^= pin3Val;
  pin1Val ^= pin2Val;
  pin0Val ^= pin1Val;

  return (pin0Val | (pin1Val<<1) | (pin2Val<<2) | (pin3Val<<3));
  */
}

