/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using SPI to communicate
 4 or 5 pins are required to interface.

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_CS_0   5
#define OLED_CS_1   6
#define OLED_CS_2   7
#define OLED_CS_3   8
#define OLED_CS_4   9

//Comment out above, uncomment this block to use hardware SPI
#define OLED_DC     36
#define OLED_CS     10
#define OLED_RESET  39
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_RESET, OLED_CS, 1000000);

void setup() {
  Serial.begin(9600);

  pinMode(OLED_CS_0, OUTPUT);
  pinMode(OLED_CS_1, OUTPUT);
  pinMode(OLED_CS_2, OUTPUT);
  pinMode(OLED_CS_3, OUTPUT);
  pinMode(OLED_CS_4, OUTPUT);

  pinMode(OLED_RESET, OUTPUT);
  digitalWrite(OLED_RESET, HIGH);
  digitalWrite(OLED_RESET, LOW);
  digitalWrite(OLED_RESET, HIGH);

  for (int i = 0; i<24; i++){
    digitalWrite(OLED_CS_0, i & 0b00001);
    digitalWrite(OLED_CS_1, (i >> 1) & 0b00001);
    digitalWrite(OLED_CS_2, (i >> 2) & 0b00001);
    digitalWrite(OLED_CS_3, (i >> 3) & 0b00001);
    digitalWrite(OLED_CS_4, (i >> 4) & 0b00001);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0, false)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    } 

    

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display.display();
    //delay(4000); // Pause for 2 seconds
    }
}

void loop() {
}



void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}