
#include <WS2812Serial.h>


//#define ROWS 32
//#define COLUMNS 16
//
//#define ACTUAL_ROWS 64
// used for indexing 
#define CALC_COLUMNS 8

#define ROWS 32
#define COLUMNS 8//16

#define WHITE_COL matrix.Color(255,255,255)
#define OFF_COL matrix.Color(0,0,0)

#define PIN 1

#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif


const int NUM_LEDS = 256; 


extern WS2812Serial matrix;

// interpolates raw coordinates and maps onto larger shape that gets drawn 
// ex: if block size is 3x3, increase by 1 moves by 3 etc
void draw(int x, int y,  uint32_t color);

void setup_matrix(uint32_t initial_text_color);

int getMatrixWidth(); 

// get height and width accounting for block size
int getTrueHeight();
int getTrueWidth();
