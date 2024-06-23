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

// Pot mux select pins
const int potSelPin0 = 26;
const int potSelPin1 = 27;
const int potSelPinEn = 28;

// Initialize an empty array for all the knobs positions
int potPosArray[4][6];
int potPosCurrent;
int potPosDiff;
int midiVal;

// This is the timestamp of the previous loop
unsigned long potLoopStart;
unsigned long potLoopInterval; // Calculates how long has actually passed (used to scale accumulated difference)
const unsigned long potLoopAccumulator = 100; // Loop time change to get the right sample rate/accumulation time

// Encoder pins
const int encPin0 = 29;
const int encPin1 = 30;
const int encPin2 = 31;
const int encPin3 = 32;

// Encoder lookup table
const int encoderMap[16] = {0, 10, 0, 1, 0, 11, 0, 12, 8, 9, 7, 2, 5, 4, 6, 3};

// Encoder state
int encoderState;

void setup() {
  // put your setup code here, to run once:

  // Setup pin modes
  pinMode(potSelPin0, OUTPUT);
  pinMode(potSelPin1, OUTPUT);
  pinMode(potSelPinEn, OUTPUT);
  digitalWrite(potSelPinEn, HIGH);

  // Setup encoder pins
  pinMode(encPin0, INPUT);
  pinMode(encPin1, INPUT);
  pinMode(encPin2, INPUT);
  pinMode(encPin3, INPUT);

  // Read initial potentiometer positions
  // For each of the 4 multiplexer settings 
  for (int i=0; i<4; i++) {
    // Set the muxes appropriately
    digitalWrite(potSelPin0, i & 0b01);
    digitalWrite(potSelPin1, i>>1 & 0b01);
    // Read values from each potentiometer
    for (int j=0; j<6; j++) {
      potPosArray[i][j] = computeAbsolutePos(analogRead(analogPins[0][j]), analogRead(analogPins[1][j]));
    }
  }

  // Start the loop timer at the end of setup
  potLoopStart = millis();

  // Read initial encoder state
  encoderState = readEncoder();
}

void loop() {
  // put your main code here, to run repeatedly:

  // If the potentiometer timer has accumulated enough time, then read them again
  potLoopInterval = millis() - potLoopStart;
  if (potLoopInterval >= potLoopAccumulator) {
    // Read current potentiometer positions and calculate the difference, send update if there is a difference
    // For each of the 4 multiplexer settings 
    for (int i=0; i<4; i++) {
      // Set the muxes appropriately
      digitalWrite(potSelPin0, i & 0b01);
      digitalWrite(potSelPin1, i>>1 & 0b01);
      // Read values from each potentiometer
      for (int j=0; j<6; j++) {
        potPosCurrent = computeAbsolutePos(analogRead(analogPins[0][j]), analogRead(analogPins[1][j]));
        potPosDiff = potPosArray[i][j] - potPosCurrent;
        potPosArray[i][j] = potPosCurrent;

        if (potPosDiff > 1024) {
          potPosDiff = potPosDiff - 0b11111111111;
        } else if (potPosDiff < -1024) {
          potPosDiff = potPosDiff + 0b11111111111;
        }

        midiVal = (potPosDiff/15) * (potLoopAccumulator/potLoopInterval) + 64; // Scale by actual time passed to make sure the sensitivity remains the same

        // If there has been a change then send it!
        if (midiVal != 64) {
          usbMIDI.sendControlChange(potMap[i][j]+1, midiVal, encoderState); // Use the pot number as the control number
        }

        while (usbMIDI.read()); // Ignore incoming MIDI messages (see Teensy docs)

        // Restart the potLoop
        potLoopStart = millis();
      }
    }
  }

  // Check encoder state
  encoderState = readEncoder();
  

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

int readEncoder(void) {

  // Convert pin readout into an index for the encoderMap
  int val = digitalRead(encPin0);
  val = val | digitalRead(encPin1) << 1;
  val = val | digitalRead(encPin2) << 2;
  val = val | digitalRead(encPin3) << 3;

  return encoderMap[val];
}
