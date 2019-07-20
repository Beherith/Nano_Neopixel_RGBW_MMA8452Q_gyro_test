/******************************************************************************
MMA8452Q_Basic.ino
SFE_MMA8452Q Library Basic Example Sketch
Jim Lindblom @ SparkFun Electronics
Original Creation Date: June 3, 2014
https://github.com/sparkfun/MMA8452_Accelerometer

This sketch uses the SparkFun_MMA8452Q library to initialize the
accelerometer, and stream values from it.

Hardware hookup:
  Arduino --------------- MMA8452Q Breakout
    3.3V  ---------------     3.3V
    GND   ---------------     GND
  SDA (A4) --\/330 Ohm\/--    SDA
  SCL (A5) --\/330 Ohm\/--    SCL

The MMA8452Q is a 3.3V max sensor, so you'll need to do some 
level-shifting between the Arduino and the breakout. Series
resistors on the SDA and SCL lines should do the trick.

Development environment specifics:
  IDE: Arduino 1.0.5
  Hardware Platform: Arduino Uno
  
  **Updated for Arduino 1.6.4 5/2015**

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
******************************************************************************/
#include <Wire.h> // Must include Wire library for I2C
#include <SparkFun_MMA8452Q.h> // Includes the SFE_MMA8452Q library
MMA8452Q accel;
String line; 

#include <Adafruit_NeoPixel.h>
// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            2
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      60
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRBW + NEO_KHZ800);

// POTENTIOMETER:
#define POT_LOW A0
#define POT_PIN A1
#define POT_HIGH A2
int pot_value = 0;
// Switch:
#define SW_LOW 3
#define SW_PIN 4
#define SW_HIGH 5
int switch_value =0;

float mean_g = 1.0; // acceleration vector length
float delta_g = 0.0; //how different than 1G accel do we have?

int delayval = 500; // delay for half a second

void setup() {
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.setBrightness(32); // VERY IMPORTANT: the led strip draws up to 60*60mA = 3.6A when fully lit! Set a brightness multiplier lower for USB testing [0-255]
  pinMode(POT_LOW,OUTPUT);
  digitalWrite(POT_LOW,LOW);
  pinMode(POT_HIGH,OUTPUT);
  digitalWrite(POT_HIGH,HIGH);
  pinMode(SW_LOW,OUTPUT);
  digitalWrite(SW_LOW,LOW);
  pinMode(SW_HIGH,OUTPUT);
  digitalWrite(SW_HIGH,HIGH);  
  
  Serial.begin(230400);
  Serial.println("Testing Accelerometer and Neopixel");
  Serial.println("T\tPot\tX\tY\tZ\tDeltaG");
  line.reserve(128);

  //  1. Default init. This will set the accelerometer up
  //     with a full-scale range of +/-2g, and an output data rate
  //     of 200 Hz (medium speed).
  accel.init(SCALE_2G, ODR_200); // that means 12 bits of 2g-> 1G=2048

}

void catprint(){ //faster than serial.print individually
  line = "";
  line += millis() %1000;
  line += '\t';
  line += pot_value;
  line += '\t';
  line += accel.x;
  line += '\t';
  line += accel.y;
  line += '\t';
  line += accel.z;
  line += '\t';
  line += delta_g;
  Serial.println(line);
}

void loop() {
   // accel.read() will update two sets of variables. 
    // * int's x, y, and z will store the signed 12-bit values 
    //   read out of the accelerometer.
    // * floats cx, cy, and cz will store the calculated 
    //   acceleration from those 12-bit values. These variables 
    //   are in units of g's.
  accel.read();
  pot_value = analogRead(POT_PIN);
  switch_value = digitalRead(SW_PIN);
  //sqrt(cx^2+cy^2+cz^2) should be == 1 if accelerometer isnt moving.
  //the sqrt isnt needed, and is quite expensive so we will refrain from using it. 
  
  mean_g =  accel.cx*accel.cx+accel.cy*accel.cy+accel.cz*accel.cz;
  delta_g = mean_g - 1.0;
  int rc,gc,bc,wc=0; //indexes of how far along the strip these colors should be lit
  rc = NUMPIXELS * accel.cx *0.5; //at 1G half of red pixels are lit, at 0G none are lit, at 2G all are lit
  gc = NUMPIXELS * accel.cy *0.5;
  bc = NUMPIXELS * accel.cz *0.5;
  wc = NUMPIXELS * delta_g;
  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
  
  for(int i=0;i<NUMPIXELS;i++){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(255*(i<rc),255*(i<gc),255*(i<bc),255*(i<wc))); 
  }
  pixels.show(); // This sends the updated pixel color to the hardware. 
  catprint();
  delay(6); //The whole loop takes about 4 ms :P, add 6 to make it 100hz
}
