#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Fonts/FreeSans9pt7b.h>

// Define oled labels
const char* oledLabels[12][24] = {
  {"Exposure", "Contrast", "Temperature", "Tint", "Highlights", "Shadows", "Whites", "Blacks", "Exposure2", "Contrast2", "Temperature2", "Tint2", "Highlights2", "Shadows2", "Whites2", "Blacks2", "Exposure3", "Contrast3", "Temperature3", "Tint3", "Highlights3", "Shadows3", "Whites3", "Blacks3"},
  {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
  {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
  {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
  {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
  {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
  {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
  {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
  {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
  {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
  {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""},
  {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""}
};

const char* ipsLabels[12] = {"Basic", "Detail", "Hue", "Saturation", "Luminance", "Effects", "Transform", "Layer 8","Layer 9", "Layer 10", "Layer 11", "Layer 12"};

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
Adafruit_ST7789 ipsDisplay = Adafruit_ST7789(&SPI, ipsCSPin, dcPin, ipsRstPin);

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

void setup() {
  // put your setup code here, to run once:

  // Setup encoder pins
  pinMode(encPin0, INPUT);
  pinMode(encPin1, INPUT);
  pinMode(encPin2, INPUT);
  pinMode(encPin3, INPUT);

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
    if(!oledDisplay.begin(SSD1306_SWITCHCAPVCC, 0, false)) {
      // Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
  }

  // Setup the IPS display
  ipsDisplay.init(240, 320, SPI_MODE2); // Needs to initialize in SPI_MODE2 (found this out after it didnt work with defauly SPI_MODE0)
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
    //oledDisplay.display();
    oledDisplay.setTextSize(1);
    oledDisplay.setFont(&FreeSans9pt7b);
    oledDisplay.setTextColor(SSD1306_WHITE);
    oledDisplay.setCursor(10,25);
    oledDisplay.println(oledLabels[encoderState-1][i]);
    oledDisplay.display();
  }
}

void drawIPSMenu(void) {
  int yCursorPos = 25;
  ipsDisplay.fillScreen(ST77XX_BLACK); // Clear Screen
  ipsDisplay.setTextColor(ST77XX_WHITE);
  ipsDisplay.setFont(&FreeSans9pt7b);
  ipsDisplay.setTextSize(1);
  ipsDisplay.setRotation(1); // Needs to get called before new display operation

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
  ipsDisplay.setRotation(1); // Needs to get called before new display operation
  // Clear the old selector rectangle
  ipsDisplay.drawRect(((prevEncoderState-1)/6)*160, (((prevEncoderState-1)%6)*40)+4, 160, 30, ST77XX_BLACK);
  // Draw the new one
  ipsDisplay.drawRect(((newEncoderState-1)/6)*160, (((newEncoderState-1)%6)*40)+4, 160, 30, ST77XX_WHITE);
}

