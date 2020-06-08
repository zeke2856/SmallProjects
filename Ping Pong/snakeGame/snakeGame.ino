// Adafruit_NeoMatrix example for single NeoPixel Shield.

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <array>
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

#define PIN 0
#define WIDTH 25
#define HEIGHT 20

#define STARTLENGTHSNAKE 3

#define BUTTONUP 17
#define BUTTONDOWN 16
#define BUTTONLEFT 14
#define BUTTONRIGHT 15
#define BUTTONSELECT 13
#define SNAKEMAXLENGTH 256

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
  matrix.Color(255, 255, 255),
  matrix.Color(0,0,0)
};
char input = 0;
int red [2] = {0,0};
int coords [SNAKEMAXLENGTH][2];

int x, y, direct, pass, colorOn, snakeTl, snakeHd;

int buttonStateUp, buttonStateDown, buttonStateLeft, buttonStateRight, buttonStateSelect;
bool recieve = 0;
bool go = true;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(21));
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(255);
  matrix.setTextColor(colors[0]);
  pinMode(BUTTONUP, INPUT);
  pinMode(BUTTONDOWN, INPUT);
  pinMode(BUTTONLEFT, INPUT);
  pinMode(BUTTONRIGHT, INPUT);
  pinMode(BUTTONSELECT, INPUT);
}

void loop(){
  resetGame();
  playGame();
  while(!go){
    readButtons();
    if(buttonStateSelect == HIGH){
      matrix.fillScreen(0);
      go = true;
      Serial.println("reset");
      setup();
    }
  }
}
void resetGame(){
  x    = 12;
  y    = 15;
  direct = 0;
  pass = 0;
  colorOn = 0;
  snakeTl = 0;
  snakeHd = STARTLENGTHSNAKE - 1;
  for(int i = 0; i < SNAKEMAXLENGTH; i++){
    coords[i][0] = 0;
    coords[i][1] = 0;
  }
  spawnNewRed();
}

void playGame(){
  while(go){
    for(int i = 0; i < 15; i ++){
      readButtons();
      noButtonsPressed();
      delay(10);    
      readButtons();
      updateDirect();
    }
    
    //move 
    updateXY();
    moveSnakeAlong();
    
    if (dead()){//checks loss condition
      go = false;
      Serial.println("dead");
    }else if(onRed()){
      spawnNewRed();
    }else {
      removeTail();
    }

    pixelDraw(x,y,colors[3]);
    matrix.show();
  }
}

bool dead(){
  // check if x & y are out of bounds or overlapping snake
  if (x < 0 || x > WIDTH-1)
    return true;
  if (y < 0 || y > HEIGHT-1)
    return true;
  if (onSnake(x, y))
    return true;
  return false;
}

bool onRed(){
  if(x == red[0] && y == red[1])
    return true;
  return false;
}

void spawnNewRed(){
  boolean w = true;
  while(w){
    red[0] = random(100)%25;
    red[1] = random(100)%20;
    w = onSnake(red[0], red[1]);
  }
  pixelDraw(red[0], red[1], colors[1]);
}

bool onSnake(int xCheck, int yCheck){
  for(int i = 0; i < (snakeHd - snakeTl)%SNAKEMAXLENGTH; i++){
    if(xCheck == coords[(snakeTl + i)% SNAKEMAXLENGTH][0] && yCheck == coords[(snakeTl + i)% SNAKEMAXLENGTH][1])
      return true;
  }
  return false;
}

void moveSnakeAlong(){
  pixelDraw(x, y, colors[3]);
  snakeHd = (snakeHd + 1) % SNAKEMAXLENGTH;
  coords[snakeHd][0] = x;
  coords[snakeHd][1] = y;
}

void removeTail(){
  pixelDraw(coords[snakeTl][0], coords[snakeTl][1], colors[4]);
  snakeTl = (snakeTl + 1) % SNAKEMAXLENGTH;
}

void readButtons(){
  // digital reads
  buttonStateUp = digitalRead(BUTTONUP);
  buttonStateDown = digitalRead(BUTTONDOWN);
  buttonStateLeft = digitalRead(BUTTONLEFT);
  buttonStateRight = digitalRead(BUTTONRIGHT);
  buttonStateSelect = digitalRead(BUTTONSELECT);

}

void noButtonsPressed(){
  if(buttonStateUp == LOW && buttonStateDown == LOW && buttonStateLeft == LOW && buttonStateRight == LOW)
   recieve = true;
}

void updateDirect(){
 //change direction
 if(recieve && buttonStateUp == HIGH){
    //direct = 0;
    Serial.println("up");
    recieve = false;
 }
 if(recieve && buttonStateDown == HIGH){
    //direct = 2;
    recieve = false;
 }
 if(recieve && buttonStateLeft == HIGH){
    //direct = 3;
    direct = (direct + 3)%4;
    recieve = false;
 }
 if(recieve && buttonStateRight == HIGH){
    //direct = 1;
    direct = (direct + 1)%4;
    recieve = false;
 }
 if(recieve && buttonStateSelect == HIGH){
    matrix.fillScreen(0);
    recieve = false;  
    Serial.println("reset");
    resetGame();
 }
}

void updateXY(){
  switch(direct){
    case 0:
      changeY(-1);
      break;
    case 1:
      changeX(1);
      break; 
    case 2:
      changeY(1);
      break;
    case 3:
      changeX(-1);
      break;
    default:
      break;
  }
  
}

//draw methods
void changeX (int delta){
  //x = (x + delta + WIDTH) % WIDTH;
  x += delta;
}

void changeY (int delta){
  //y = (y - delta + HEIGHT) % HEIGHT;
  y += delta;
}

void pixelDraw(int drawX, int drawY, uint16_t color){
  int i = drawX;
  int j = drawY;
  matrix.drawPixel(i, j, color);
}
