#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Fonts/FreeSans9pt7b.h>

// Define oled labels
const char* oledLabels[12][24] = {
  {"Temperature", "Tint", "Exposure", "Contrast", "Highlights", "Shadows", "Whites", "Blacks", "Texture", "Clarity", "Dehaze", "Vibrance", "Saturation", "B&W", "Crop Angle", "4 Stars", "Vignette", "Vign Mid", "Vign Round", "Vign Feath", "Vign High", "Grain", "Grain Size", "Grain Rough"},
  {"Red Hue", "Orange Hue", "Yellow Hue", "Green Hue", "Aqua Hue", "Blue Hue", "Purple Hue", "Magenta Hue", "Red Sat", "Orange Sat", "Yellow Sat", "Green Sat", "Aqua Sat", "Blue Sat", "Purple Sat", "Magenta Sat", "Red Lum", "Orange Lum", "Yellow Lum", "Green Lum", "Aqua Lum", "Blue Lum", "Purple Lum", "Magenta Lum"},
  {"Red B&W", "Orange B&W", "Yellow B&W", "Green B&W", "Aqua B&W", "Blue B&W", "Purple B&W", "Magenta B&W", "Temperature", "Tint", "Exposure", "Contrast", "Highlights", "Shadows", "Whites", "Blacks", "Texture", "Clarity", "Dehaze", "", "", "", "", ""},
  {"Global Hue", "Highlight Hue", "Midtone Hue", "Shadow Hue", "Blending", "", "", "", "Global Sat", "Highlight Sat ", "Midtone Sat", "Shadow Sat", "Balance", "", "", "", "Global Lum", "Highlight Lum", "Midtone Lum", "Shadow Lum", "", "", "", ""},
  {"Temperature", "Tint", "Exposure", "Contrast", "Highlights", "Shadows", "Whites", "Blacks", "Texture", "Clarity", "Dehaze", "Hue", "Saturation", "Sharpness", "Noise Red", "Moire", "Defringe", "Luminance", "Local Amount", "Invert", "Add Brush", "Sub Brush", "Brush Size", "Brush Feather"},
  {"Angle", "Move Horiz", "Move Vert", "All", "Bottom", "Left", "Top", "Right", "Bottom-Left", "Top-Left", "Top-Right", "Bottom-Right", "Trans Vert", "Trans Horiz", "Trans Rotate", "Trans Aspect", "Trans Scale", "Trans X", "Trans Y", "Trans Reset", "Upright Reset", "Constrain", "Trans Auto", "Trans Off"},
  {"Sharp Amount", "Sharp Radius", "Sharp Detail", "Sharp Masking", "", "", "", "", "Noise Lum", "Noise Detail", "Noise Contrast", "Color Amount", "Color Detail", "Color Smooth", "", "", "", "", "", "", "", "", "", ""},
  {"Distortion", "Defringe Prpl", "Prpl Hue Low", "Prpl Hue High", "Defringe Grn", "Grn Hue Low", "Grn Hue High", "Vingnetting", "Vign Mid", "", "", "", "", "", "", "", "Shad Tint", "Red Hue", "Red Sat", "Green Hue", "Green Sat", "Blue Hue", "Blue Sat", ""},
  {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
  {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
  {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
  {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""}
};

const char* ipsLabels[12] = {"Basic", "Color Mixer", "B&W", "Color Grade", "Local", "Crop/Trans", "Detail", "Lens/Calib","", "", "", "Off"};

// Define screen constants
const int oledWidth = 128;
const int oledHeight = 32;

const int oledCS0Pin = 5;
const int oledCS1Pin = 6;
const int oledCS2Pin = 7;
const int oledCS3Pin = 8;
const int oledCS4Pin = 9;
const int oledCSPin = 10;
const int oledRstPin = 39;

const int ipsCSPin = 37;
const int ipsRstPin = 38;
const int ipsBacklightPin = 3;

const int dcPin = 36;

Adafruit_SSD1306 oledDisplay(oledWidth, oledHeight, &SPI, dcPin, oledRstPin, oledCSPin);
Adafruit_ST7789 ipsDisplay = Adafruit_ST7789(ipsCSPin, dcPin, ipsRstPin);

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
int newEncoderState;

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
    digitalWrite(keyClkPin, HIGH);
    digitalWrite(keyClkPin, LOW);
    delayMicroseconds(10);
  }
  // Reset the shift register
  digitalWrite(keyClrPin, LOW);
  digitalWrite(keyClrPin, HIGH);

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

  // Setup OLED pins
  pinMode(oledCS0Pin, OUTPUT);
  pinMode(oledCS1Pin, OUTPUT);
  pinMode(oledCS2Pin, OUTPUT);
  pinMode(oledCS3Pin, OUTPUT);
  pinMode(oledCS4Pin, OUTPUT);

  // Reset all OLEDs at once
  pinMode(oledRstPin, OUTPUT);
  digitalWrite(oledRstPin, HIGH);
  digitalWrite(oledRstPin, LOW);
  digitalWrite(oledRstPin, HIGH);

  for (int i = 0; i<24; i++){
    digitalWrite(oledCS0Pin, i & 0b00001);
    digitalWrite(oledCS1Pin, (i >> 1) & 0b00001);
    digitalWrite(oledCS2Pin, (i >> 2) & 0b00001);
    digitalWrite(oledCS3Pin, (i >> 3) & 0b00001);
    digitalWrite(oledCS4Pin, (i >> 4) & 0b00001);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!oledDisplay.begin(SSD1306_SWITCHCAPVCC, 0, false)) { // Calls SPI.begin()
      // Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
  }

  // Configure IPS pins
  pinMode(ipsBacklightPin, OUTPUT);

  // Setup the IPS display
  ipsDisplay.init(240, 320, SPI_MODE2); // Needs to be in SPI_MODE2 to work (discovered empirically)
  drawIPSMenu();
  
  encoderState = readEncoder();
  updateOLEDs();
  updateIPSSelector(1, encoderState);
}

void loop() {
  // put your main code here, to run repeatedly:

  // Check encoder state
  int newEncoderState = readEncoder();
  if (newEncoderState != encoderState) {
    updateIPSSelector(encoderState, newEncoderState);
    encoderState = newEncoderState; // Change global encoder state before update oled
    updateOLEDs();
  }

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
          usbMIDI.sendNoteOn(buttonMap[row][col], 127, encoderState);
        } else {
          usbMIDI.sendNoteOff(buttonMap[row][col], 0, encoderState);
        }
        buttonStateArray[row][col] = currButtonState;
      }
    }
    // Tick the clock
    digitalWrite(keyClkPin, HIGH);
    digitalWrite(keyClkPin, LOW);
    delayMicroseconds(10);
  }
  // Reset the shift register
  digitalWrite(keyClrPin, LOW);
  digitalWrite(keyClrPin, HIGH);

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

        //midiVal = (potPosDiff/15) * (potLoopAccumulator/potLoopInterval) + 64; // Scale by actual time passed to make sure the sensitivity remains the same
        midiVal = (potPosDiff/15) + 64;

        // If there has been a change then send it!
        if (midiVal != 64) {
          usbMIDI.sendControlChange(potMap[i][j], midiVal, encoderState); // Use the pot number as the control number
        }

        while (usbMIDI.read()); // Ignore incoming MIDI messages (see Teensy docs)

        // Restart the potLoop
        potLoopStart = millis();
      }
    }
  }

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

void updateOLEDs(void) {
  for (int i = 0; i<24; i++){
    digitalWrite(oledCS0Pin, i & 0b00001);
    digitalWrite(oledCS1Pin, (i >> 1) & 0b00001);
    digitalWrite(oledCS2Pin, (i >> 2) & 0b00001);
    digitalWrite(oledCS3Pin, (i >> 3) & 0b00001);
    digitalWrite(oledCS4Pin, (i >> 4) & 0b00001);

    oledDisplay.clearDisplay();
    oledDisplay.setTextSize(1);
    oledDisplay.setFont(&FreeSans9pt7b);
    oledDisplay.setTextColor(SSD1306_WHITE);
    oledDisplay.setCursor(7,20);
    oledDisplay.println(oledLabels[encoderState-1][i]);
    oledDisplay.display();

    // If encoder is set to position 12, then turn off the display
    if (encoderState == 12) {
      oledDisplay.clearDisplay();
      oledDisplay.display();
    }
  }
}

void drawIPSMenu(void) {
  int yCursorPos = 25;
  ipsDisplay.fillScreen(ST77XX_BLACK); // Clear Screen
  ipsDisplay.setTextColor(ST77XX_WHITE);
  ipsDisplay.setFont(&FreeSans9pt7b);
  ipsDisplay.setTextSize(1);
  ipsDisplay.setRotation(1);

  for (int i=0; i<6; i++) {
    ipsDisplay.setCursor(10, yCursorPos);
    ipsDisplay.print(i+1);
    ipsDisplay.print(". ");
    ipsDisplay.print(ipsLabels[i]);
    yCursorPos += 40;
  }

  yCursorPos = 25;

  for (int i=6; i<12; i++) {
    ipsDisplay.setCursor(170, yCursorPos);
    ipsDisplay.print(i+1);
    ipsDisplay.print(". ");
    ipsDisplay.print(ipsLabels[i]);
    yCursorPos += 40;
  }
}

void updateIPSSelector(int prevEncoderState, int newEncoderState) {
  
  // If 12 was the last position, redraw the menu. Need to do this before the rectangle is drawn so that it shows up too
  if (prevEncoderState == 12) {
    digitalWrite(ipsBacklightPin, HIGH);
    drawIPSMenu();
  }

  ipsDisplay.setRotation(1); // Set rotation before any new operation on screen
  // Clear the old selector rectangle
  ipsDisplay.drawRect(((prevEncoderState-1)/6)*160, (((prevEncoderState-1)%6)*40)+4, 160, 30, ST77XX_BLACK);
  // Draw the new one
  ipsDisplay.drawRect(((newEncoderState-1)/6)*160, (((newEncoderState-1)%6)*40)+4, 160, 30, ST77XX_WHITE);

  // If encoder is set to position 12, then turn off the display
  if (newEncoderState == 12) {
    ipsDisplay.fillScreen(ST77XX_BLACK);
    digitalWrite(ipsBacklightPin, LOW);
  }


}
