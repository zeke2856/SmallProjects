#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <cmath>
#include <ESP32Servo.h>
#include <Adafruit_TCS34725.h>
#include "ballIndex.h"

#define sorterserverpin 14
// for a common anode LED, connect the common pin to +5V
// for common cathode, connect the common to ground

// set to false if using a common cathode LED
#define commonAnode true

// our RGB -> eye-recognized gamma color
byte gammatable[256];

void sortballs();
int what_color_ball();
int difference_from(int color_state, int r, int g, int b);
int get_ball_value(int color_state, char diode);

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_1X);
Servo sorter1;
//Sorter airsortone;

// Published values for SG90 servos; adjust if needed
int minUs = 1000;
int maxUs = 2000;

void setup() {
  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	sorter1.setPeriodHertz(50);
	sorter1.attach(sorterserverpin, 500, 2500);

  Serial.begin(9600);
  Serial.println("Color View Test!");

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  // thanks PhilB for this gamma table!
  // it helps convert RGB colors to what humans see
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    if (commonAnode) {
      gammatable[i] = 255 - x;
    } else {
      gammatable[i] = x;
    }
    //Serial.println(gammatable[i]);
  }
}

// The commented out code in loop is example of getRawData with clear value.
// Processing example colorview.pde can work with this kind of data too, but It requires manual conversion to 
// [0-255] RGB value. You can still uncomments parts of colorview.pde and play with clear value.
void loop() {
  float red, green, blue;
  
  tcs.setInterrupt(false);  // turn on LED

   // takes 50ms to read

  while(true){
    // for (int i = 500; i < 2500; i+=100){
    //     sorter1.write(i);
    //     Serial.println(i);
    //     delay(100);
    // }
    // for (int i = 2500; i > 500; i-=100){
    //     sorter1.write(i);
    //     Serial.println(i);
    //     delay(100);
    // }

    tcs.getRGB(&red, &green, &blue);
    delay(100);
    Serial.print("R:\t"); Serial.println(int(red)); 
    Serial.print("G:\t"); Serial.println(int(green)); 
    Serial.print("B:\t"); Serial.println(int(blue));

    Serial.print("\n");
    sortballs();
  }
  /*
  delay(15000);
  Serial.println("Blank_ball ");
  Serial.print("r "); Serial.println(get_ball_value(Blank_ball, 'r'));
  Serial.print("g "); Serial.println(get_ball_value(Blank_ball, 'g'));
  Serial.print("b "); Serial.println(get_ball_value(Blank_ball, 'b'));
  */
}

//sort ball que
void sortballs(){
  static int ball_color = Blank_ball;
  bool go = true;
  while(go){
    sorter1.write(1550);
    delay(400); 
    ball_color = what_color_ball();
    switch(ball_color){
      case Blank_ball:
        Serial.println("BLANK BALL");
        go = false;
        break;
      case Black_ball:
        Serial.println("BLACK BALL");
        sorter1.write(1900);
        delay(100);
        break;
      case White_ball:
        Serial.println("WHITE BALL");
        sorter1.write(1100);
        delay(100);
        break;
    }
    Serial.println();
  }
}

//returns the color of the ball in the chamber
int what_color_ball(){
  //get ball color from tcs (color sensor)
  float red, green, blue;
  tcs.setInterrupt(false);  // turn on LED
  tcs.getRGB(&red, &green, &blue);
  
  Serial.print("R:\t"); Serial.println(int(red)); 
  Serial.print("G:\t"); Serial.println(int(green)); 
  Serial.print("B:\t"); Serial.println(int(blue));
  
  //gets difference from a color ball from 
  int diff_blank = difference_from(Blank_ball, int(red), int(green), int(blue));
  int diff_black = difference_from(Black_ball, int(red), int(green), int(blue));
  int diff_white = difference_from(White_ball, int(red), int(green), int(blue));
  //Serial.print("diff blank "); Serial.println(diff_blank);
  //Serial.print("diff black "); Serial.println(diff_black);
  //Serial.print("diff white "); Serial.println(diff_white);

  int diffs[] = {diff_blank, diff_black, diff_white};
  int closest = 0;
  for(int i = 1; i < num_color_states; i++){
    if (diffs[i] < diffs[closest])
      closest = i;
  }
  return closest;
}

//returns difference of color from 
int difference_from(int color_state, int r, int g, int b){
  int difference = 0;
  difference += abs(get_ball_value(color_state, 'r') - r);
  //difference += abs(get_ball_value(color_state, 'g') - g);
  difference += abs(get_ball_value(color_state, 'b') - b);
  return difference;
}

//get diode reading for color_ball
int get_ball_value(int color_state, char diode){
  //converts diode char to color_state index
  int color_wanted = -1;
  switch(diode){
    case 'r':
      color_wanted = 0;
      break;
    case 'g':
      color_wanted = 1;
      break;
    case 'b':
      color_wanted = 2;
      break;
    default:
      Serial.println("get_ball_value asked for wrong diode");
    }

  //gets correct color reference behind color_state 
  switch(color_state){
    case Blank_ball:
      return BLANK[color_wanted];
      break;
    case Black_ball:
      return BLACK[color_wanted];
      break;
    case White_ball:
      return WHITE[color_wanted];
      break;
    default:
      Serial.println("non exsistent ball asked for");
    }
  return -1;
}