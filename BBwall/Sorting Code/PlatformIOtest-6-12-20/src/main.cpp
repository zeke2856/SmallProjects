#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <ESP32Servo.h>
#include "Adafruit_TCS34725.h"

enum COLOR {Blank_ball, Black_ball, White_ball};
int num_color_states = 3;
const int BLANK[] = {58, 96, 94};
const int BLACK[] = {52, 98, 92};
const int WHITE[] = {57, 99, 83};

#define sorterserverpin 14
// for a common anode LED, connect the common pin to +5V
// for common cathode, connect the common to ground

// set to false if using a common cathode LED
#define commonAnode true

// our RGB -> eye-recognized gamma color
byte gammatable[256];


Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_1X);
Servo sorter1;

// Published values for SG90 servos; adjust if needed
int minUs = 1000;
int maxUs = 2000;

void setup() {
  Serial.begin(9600);
  Serial.println("Color View Test!");

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  // use these three pins to drive an LED
#if defined(ARDUINO_ARCH_ESP32)
  Serial.println("pin setup");
  // ledcAttachPin(redpin, 1);
  // ledcSetup(1, 12000, 8);
  // ledcAttachPin(greenpin, 2);
  // ledcSetup(2, 12000, 8);
  // ledcAttachPin(bluepin, 3);
  // ledcSetup(3, 12000, 8);
  Serial.println("pin setdown");

#else
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
#endif

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
  
  
    delay(3000); 
    tcs.getRGB(&red, &green, &blue);
    delay(100);

    Serial.print("R:\t"); Serial.println(int(red)); 
    Serial.print("G:\t"); Serial.println(int(green)); 
    Serial.print("B:\t"); Serial.println(int(blue));

    Serial.print("\n");
  }
}

void sortballs(){
  static int ball_color = Blank_ball;
  while(true){
    ball_color = what_color_ball();
    break;
  }

}

int what_color_ball(){
  float red, green, blue;
  tcs.setInterrupt(false);  // turn on LED
  tcs.getRGB(&red, &green, &blue);
  int diff_blank = difference_from(Blank_ball, int(red), int(green), int(blue));
  int diff_black = difference_from(Black_ball, int(red), int(green), int(blue));
  int diff_white = difference_from(White_ball, int(red), int(green), int(blue));
  int diffs[] = {diff_blank, diff_black, diff_white};
  int least = 255;
  for(int i; i < num_color_states; i++){
    
  }


}

//returns difference of color from 
int difference_from(int color_state, int r, int g, int b){
  int difference = 0;
  difference += abs(get_ball_value(color_state, 'r') - r);
  difference += abs(get_ball_value(color_state, 'g') - g);
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
    case White_ball:
      return BLANK[color_wanted];
      break;
    case Blank_ball:
      return BLACK[color_wanted];
      break;
    case Black_ball:
      return WHITE[color_wanted];
      break;
    default:
      Serial.println("non exsistent ball asked for");
  }
}

/*
#if defined(ARDUINO_ARCH_ESP32)
  ledcWrite(1, gammatable[(int)red]);
  ledcWrite(2, gammatable[(int)green]);
  ledcWrite(3, gammatable[(int)blue]);
#else
  analogWrite(redpin, gammatable[(int)red]);
  analogWrite(greenpin, gammatable[(int)green]);
  analogWrite(bluepin, gammatable[(int)blue]);
#endif
*/

//  uint16_t red, green, blue, clear;
//  
//  tcs.setInterrupt(false);  // turn on LED
//
//  delay(60);  // takes 50ms to read
//
//  tcs.getRawData(&red, &green, &blue, &clear);
//  
//  tcs.setInterrupt(true);  // turn off LED
//
//  Serial.print("C:\t"); Serial.print(int(clear)); 
//  Serial.print("R:\t"); Serial.print(int(red)); 
//  Serial.print("\tG:\t"); Serial.print(int(green)); 
//  Serial.print("\tB:\t"); Serial.print(int(blue));
//  Serial.println();

