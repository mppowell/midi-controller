// Analog pins are paired to read from each of the two potentiometers in each knob
const int analogPins[2][6] = {
  {A0, A2, A4, A6, A8, A10},
  {A1, A3, A5, A7, A9, A11}
};

// There are 6 multiplexers with 4 channels each, potentiometers are organized according to this map
const int potMap[4][6] = {
  {0, 4, 8, 12, 16, 20},
  {1, 5, 9, 13, 17, 21},
  {2, 6, 10, 14, 18, 22},
  {3, 7, 11, 15, 19, 23}
};

int potVals[24];

// Pot mux select pins
const int potSelPin0 = 26;
const int potSelPin1 = 27;
const int potSelPinEn = 28;

void setup() {
  // put your setup code here, to run once:

  // Setup pin modes
  pinMode(potSelPin0, OUTPUT);
  pinMode(potSelPin1, OUTPUT);
  pinMode(potSelPinEn, OUTPUT);

  digitalWrite(potSelPinEn, HIGH);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

    for (int i=0; i<4; i++) {
      // Set the muxes appropriately
      digitalWrite(potSelPinEn, LOW);
      digitalWrite(potSelPin0, i & 0b01);
      digitalWrite(potSelPin1, i>>1 & 0b01);
      delay(10);
      digitalWrite(potSelPinEn, HIGH);
      // Read values from each potentiometer
      for (int j=0; j<6; j++) {
        potVals[potMap[i][j]] = computeAbsolutePos(analogRead(analogPins[0][j]), analogRead(analogPins[1][j]));
        //Serial.print(analogRead(analogPins[0][j]));
        //Serial.print(',');
        //Serial.print(analogRead(analogPins[1][j]));
        //Serial.print(',');
      }
    } 

    for (int k=0; k<8; k++) {
      Serial.print(potVals[k]);
      Serial.print(',');
    }

    Serial.println();
   //delay(100);
    

}

int computeAbsolutePos(int val1, int val2) {
  int corrected_val1;
  int corrected_val2;

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

  return (corrected_val1 + corrected_val2) / 2;
}
