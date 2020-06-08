// Adafruit_NeoMatrix example for single NeoPixel Shield.
// Scrolls 'Howdy' across the matrix in a portrait (vertical) orientation.

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

#define PIN 0
#define WIDTH 25
#define HEIGHT 20

#define BUTTONUP 17
#define BUTTONDOWN 16
#define BUTTONLEFT 14
#define BUTTONRIGHT 15
#define BUTTONSELECT 13

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_GRBW    Pixels are wired for GRBW bitstream (RGB+W NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)


// Example for NeoPixel Shield.  In this application we'd like to use it
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(WIDTH, HEIGHT, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_RGB            + NEO_KHZ800);


const uint16_t colors[] = {
  matrix.Color(255, 0, 0), 
  matrix.Color(0, 255, 0),
  matrix.Color(0, 0, 255),
  matrix.Color(255, 255, 255) 
};

void setup() {
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(40);
  matrix.setTextColor(colors[0]);
  pinMode(BUTTONUP, INPUT);
  pinMode(BUTTONDOWN, INPUT);
  pinMode(BUTTONLEFT, INPUT);
  pinMode(BUTTONRIGHT, INPUT);
  pinMode(BUTTONSELECT, INPUT);

}
char input = 0;
int x    = 0;
int y    = 0;
int direct = 0;
int pass = 0;
int colorOn = 0;

int buttonStateUp, buttonStateDown, buttonStateLeft, buttonStateRight, buttonStateSelect;
bool recieve = 0;
int prevX, prevY;

void loop() {
  buttonStateUp = digitalRead(BUTTONUP);
  buttonStateDown = digitalRead(BUTTONDOWN);
  buttonStateLeft = digitalRead(BUTTONLEFT);
  buttonStateRight = digitalRead(BUTTONRIGHT);
  buttonStateSelect = digitalRead(BUTTONSELECT);

  prevX = x;
  prevY = y;
  
  if(Serial.available() != 0){
    input = Serial.read();
    Serial.println(input + 1);
  }
 if(buttonStateUp == LOW && buttonStateDown == LOW && buttonStateLeft == LOW && buttonStateRight == LOW)
  recieve = true;
 if(recieve && buttonStateUp == HIGH){
    changeY(1);
    recieve = false;
 }
 if(recieve && buttonStateDown == HIGH){
    changeY(-1);
    recieve = false;
 }
 if(recieve && buttonStateLeft == HIGH){
    changeX(-1);
    recieve = false;
 }
 if(recieve && buttonStateRight == HIGH){
    changeX(1);
    recieve = false;
 }
 if(recieve && buttonStateSelect == HIGH){
    matrix.fillScreen(0);
    recieve = false;  
 }
  pixelDraw(prevX, prevY, colors[3]);
  pixelDraw(x,y,colors[0]);
  matrix.show();
}

//draw methods
void changeX (int deltaX){
  x = (x + deltaX + WIDTH) % WIDTH;
}

void changeY (int deltaY){
  y = (y - deltaY + HEIGHT) % HEIGHT;
}

void pixelDraw(int drawX, int drawY, uint16_t color){
  int i = drawX;
  int j = drawY;
  matrix.drawPixel(i, j, color);
}
