int analogPin1 = A0;
int analogPin2 = A1;

long int val1;
long int val2;

long int corrected_val1;
long int corrected_val2;

long int output;
long int prev_output = 0;
long int difference;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  // read analog values from both resistors
  val1 = analogRead(analogPin1);
  val2 = analogRead(analogPin2);

  // Invert the negative slope on each
  if (val2 > 0b1000000000) {
    corrected_val1 = 0b11111111111 - val1;
  } else {
    corrected_val1 = val1;
  }

  if (val1 < 0b1000000000) {
    corrected_val2 = 0b11111111111 - val2 + 0b1000000000;
  } else {
    corrected_val2 = val2 + 0b1000000000;
  }

  if ((corrected_val2 - corrected_val1) > 0b1000000000) {
    corrected_val2 = corrected_val2 - 0b11111111111;
  }

  output = (corrected_val1 + corrected_val2) / 2;

  Serial.print(2048);
  Serial.print(",");
  Serial.print(0);
  Serial.print(",");
  Serial.print(corrected_val1);
  Serial.print(",");
  Serial.print(corrected_val2);
  Serial.print(",");
  Serial.print(output);

  Serial.print(",");
  difference = prev_output - output;
  if (difference > 1024) {
    difference = difference - 0b11111111111;
  } else if (difference < -1024) {
    difference = difference + 0b11111111111;
  }

  Serial.println(difference);
  

  prev_output = output;

}
