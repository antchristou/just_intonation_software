#include "draw_helpers.h"
#include "matrix_mapping.h"
byte drawingMemory[NUM_LEDS*3];         //  3 bytes per LED
DMAMEM byte displayMemory[NUM_LEDS*12]; // 12 bytes per LED

int STARTING_BRIGHTNESS = 10; 
WS2812Serial matrix(NUM_LEDS, displayMemory, drawingMemory, PIN, WS2812_GRB);


// calculate which LED number a given x,y coord is for calling into matrix 



// BLOCK_SIZExBLOCK_SIZE basic shapes
#define BLOCK_SIZE 3
void draw(int x, int y,  uint32_t color, bool doShow)
{

    if(x < 0)
         Serial.printf("NOT SUPPOSED TO %d %d!!!!!\n",x, y);
  // MAP VALUES as block expects 
//  Serial.printf("%d %d\n",x,y);
  int ledNum = -1;
  for(int i = (x*BLOCK_SIZE); i < ((x+1)*BLOCK_SIZE); i++)
  {
    for(int j = (y*BLOCK_SIZE); j < ((y+1)*BLOCK_SIZE); j++)
    {
     //    Serial.printf("DRAWING BIGGER AT %d %d\n",i,j);
        
        ledNum = lookup(i,j);
        matrix.setPixel(ledNum,color);
     }
    }
  if(doShow)
    matrix.show();
}


 void setup_matrix(uint32_t initial_text_color)
 {
    Serial.printf("INIT %d\n",matrix.numPixels());
    matrix.begin();
    matrix.clear();
//  matrix.setTextWrap(false);
  matrix.setBrightness(STARTING_BRIGHTNESS);
//  matrix.setTextColor(initial_text_color);

   // testLEDs();
 }

 
int getMatrixWidth()
{
  return COLUMNS;
}
// note: flipped to match LEDs
int getTrueHeight()
{
  return ROWS/BLOCK_SIZE;
}
int getTrueWidth()
{
  return COLUMNS/BLOCK_SIZE;
}

int getBlockSize()
{
  return BLOCK_SIZE;
}


// test all LEDS are working by lighting them up
void testLEDs()
{
  Serial.println("testing...");
  for(int i =0; i < NUM_LEDS;i++)
  {
    matrix.setPixel(i,matrix.Color(255,255,255));
    matrix.show();
    delay(100);
  }
}
