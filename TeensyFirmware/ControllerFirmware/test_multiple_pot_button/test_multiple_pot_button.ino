// Pushbutton pins
const int keyClrPin = 0;
const int keySerPin = 1;
const int keyClkPin = 2;
const int row1Pin = 35;
const int row2Pin = 34;
const int row3Pin = 33;

// Pushbutton map
const int buttonMap[3][8] = {
  {0, 1, 2, 3, 4, 5, 6, 7},
  {8, 9, 10, 11, 12, 13, 14, 15},
  {16, 17, 18, 19, 20, 21, 22, 23}
};

const int rowPins[3] = {row1Pin, row2Pin, row3Pin};

bool buttonStateArray[3][8];
bool currButtonState;

void setup() {
  // put your setup code here, to run once:

  // Setup pushbotton pins
  pinMode(keyClrPin, OUTPUT);
  pinMode(keySerPin, OUTPUT);
  pinMode(keyClkPin, OUTPUT);

  pinMode(row1Pin, INPUT_PULLDOWN);
  pinMode(row2Pin, INPUT_PULLDOWN);
  pinMode(row3Pin, INPUT_PULLDOWN);

  // Set intial state for shift register
  digitalWrite(keySerPin, LOW);
  digitalWrite(keyClkPin, LOW);
  // Clear shift register
  digitalWrite(keyClrPin, HIGH);
  digitalWrite(keyClrPin, LOW);
  digitalWrite(keyClrPin, HIGH);

  // Read initial button states
  digitalWrite(keySerPin, HIGH);
  digitalWrite(keyClkPin, HIGH);
  digitalWrite(keyClkPin, LOW);
  digitalWrite(keySerPin, LOW);
  for (int col=0; col<8; col++) {
    for(int row=0; row<3; row++){
      buttonStateArray[row][col] = digitalRead(rowPins[row]);
    }
  }
  // Reset the shift register
  digitalWrite(keyClrPin, LOW);
  digitalWrite(keyClrPin, HIGH);

  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:

  // Check pushbuttons, clock in the high value to the first position on the shift register
  digitalWrite(keySerPin, HIGH);
  digitalWrite(keyClkPin, HIGH);
  digitalWrite(keyClkPin, LOW);
  digitalWrite(keySerPin, LOW);
  // For each column read all three rows
  for (int col=0; col<8; col++) {
    for(int row=0; row<3; row++){
      currButtonState = digitalRead(rowPins[row]);
      if (currButtonState != buttonStateArray[row][col]) {
        if (currButtonState) {
          Serial.print(buttonMap[row][col]+1);
          Serial.println(": ON");
        } else {
          Serial.print(buttonMap[row][col]+1);
          Serial.println(": OFF");
        }
        buttonStateArray[row][col] = currButtonState;
      }
    }
    // Tick the clock
    digitalWrite(keyClkPin, HIGH);
    digitalWrite(keyClkPin, LOW);
    delay(10);
  }
  // Reset the shift register
  digitalWrite(keyClrPin, LOW);
  digitalWrite(keyClrPin, HIGH);

}
