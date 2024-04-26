// Adafruit_NeoMatrix example for single NeoPixel Shield.
// Scrolls 'Howdy' across the matrix in a portrait (vertical) orientation.
#include "USBHost_t36.h"
#include "joystick.h"

USBHost myusb;
USBHub hub1(myusb);
USBHIDParser hid1(myusb);

JoystickController joystick(myusb);

#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN 3

int currMoving = NOT_MOVING;

// if true, flip inputs to match layout of board 
const bool HORIZONTAL_MODE = true;


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

const int JOYSTICK_MIN_MOVEMENT = 0;
const int JOYSTICK_MAX_MOVEMENT = 255;

void setupJoystick()
{
  myusb.begin();
}
  
void readJoystick(int*joystickVal,int*buttonVal)
{
    myusb.Task();
    if (joystick.available()) {

      uint32_t buttons = joystick.getButtons();
      int axis0 = joystick.getAxis(0);
      int axis1 = joystick.getAxis(1);
            
      int closestToExtreme0 = 0;
      int closestToExtreme1 = 0;
//      Serial.print("button = ");
//      Serial.println(buttons);
////      Serial.print(" ");
//      Serial.print("axis0 = ");
//      Serial.println(axis0);
//      Serial.print("axis1 = ");
//      Serial.println(axis1);
      
      // calculate which direction is closer to max or min
      if ((axis0 - 0) <= (255 - axis0))
        closestToExtreme0 = axis0 - 0;
      else
        closestToExtreme0 = 255 - axis0;

      if ((axis1 - 0) <= (255 - axis1))
        closestToExtreme1 = axis1 - 0;
      else
        closestToExtreme1 = 255 - axis1;
       
      // each axis is either 0, 127 255 
      // axis0 = left (0) to right (255)
      // axis1 = up (0) to down (255)
      // center = 127 in each 
      // calculate dominant axis as can't move diagonally 
      if(closestToExtreme0 < closestToExtreme1)
      {
        if(axis0 == JOYSTICK_MIN_MOVEMENT)
        {
          if(currMoving != LEFT_DIR)
           {
            *joystickVal = LEFT_DIR;
            Serial.println("LEFT");
            currMoving = LEFT_DIR;
            }
        }
        else if(axis0 == JOYSTICK_MAX_MOVEMENT)
        {
         if(currMoving != RIGHT_DIR)
          {
            *joystickVal = RIGHT_DIR;
            Serial.println("RIGHT");
            currMoving = RIGHT_DIR;
          }
        }
      }
      else
      {
      if(axis1 == JOYSTICK_MIN_MOVEMENT)
      {
        if(currMoving != UP_DIR)
        {
           *joystickVal = UP_DIR;
          Serial.println("UP");
          currMoving = UP_DIR;
        }
        
      }
      else if(axis1 == JOYSTICK_MAX_MOVEMENT)
      {
        if(currMoving != DOWN_DIR)
        {
           *joystickVal = DOWN_DIR;
          Serial.println("DOWN");
          currMoving = DOWN_DIR;
        }
      }
      }

      // if both axis go back to neutral, reset mover
      if((currMoving != NOT_MOVING) && (axis1 == 127 && axis1 == 127) && (axis0 == 127 && axis0 == 127))
      {
        Serial.println("RESET POS");
        currMoving = NOT_MOVING;
         *joystickVal =  NOT_MOVING;
      }

  //    Serial.printf("%d %d\n",axis0,axis1);
      joystick.joystickDataClear();
      *buttonVal = buttons;
    }
   
}
