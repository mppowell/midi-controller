int analogPin1 = A6;
int analogPin2 = A7;

long int val1;
long int val2;

long int corrected_val1;
long int corrected_val2;

long int output;
long int prev_output = 0;
long int difference;

long int midi_val = 0;


void setup() {
  // put your setup code here, to run once:

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
  difference = prev_output - output;

  if (difference > 1024) {
    difference = difference - 0b11111111111;
  } else if (difference < -1024) {
    difference = difference + 0b11111111111;
  }

  midi_val = (difference/15) + 64;

  if (midi_val != 64) {
    //MIDI.sendControlChange(1, midi_val, 1);
    usbMIDI.sendControlChange(1, midi_val, 1);
  }
  
  // Delay is important to let difference build up enough to be measured (difference/15) is pretty good sensitivity (at least for exposure)
  delay(100);

  prev_output = output;

  while (usbMIDI.read()){
  } 
}

