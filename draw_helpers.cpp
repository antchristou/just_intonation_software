#include "draw_helpers.h"

byte drawingMemory[NUM_LEDS*3];         //  3 bytes per LED
DMAMEM byte displayMemory[NUM_LEDS*12]; // 12 bytes per LED


WS2812Serial matrix(NUM_LEDS, displayMemory, drawingMemory, PIN, WS2812_GRB);


// calculate which LED number a given x,y coord is for calling into matrix 

int calculateIndex(int row_index, int column_index) {
  //  Serial.printf("%d %d\n",row_index,column_index);
    // todo: fix this 
   if (column_index >= 8) {
    
      return (15 - column_index) * 16 + (row_index + 1);
      if (row_index % 2 == 0) { // Even row index
        
        return (row_index * CALC_COLUMNS + (CALC_COLUMNS - column_index - 1))+256;
    } else { // Odd row index
        return (row_index * CALC_COLUMNS + column_index)+255;
    }
   }


    // left matrix indexing 
    if (row_index % 2 == 0) { // Even row index
        return row_index * CALC_COLUMNS + (CALC_COLUMNS - column_index - 1);
    } else { // Odd row index
        return row_index * CALC_COLUMNS + column_index;
    }
}
// mapping functions for tiled LED matrices
int mapCoords(int x, int y) {

    int mapped_index = calculateIndex(x,y);
    //Serial.printf("mapped: %d\n", mapped_index);
    return mapped_index;
} 


// BLOCK_SIZExBLOCK_SIZE basic shapes
#define BLOCK_SIZE 3
void draw(int x, int y,  uint32_t color)
{
  // MAP VALUES as block expects 
//  Serial.printf("%d %d\n",x,y);
  int ledNum = -1;
  for(int i = (x*BLOCK_SIZE); i < ((x+1)*BLOCK_SIZE); i++)
  {
    for(int j = (y*BLOCK_SIZE); j < ((y+1)*BLOCK_SIZE); j++)
    {
     //    Serial.printf("DRAWING BIGGER AT %d %d\n",i,j);
        
        ledNum = mapCoords(i,j);
        matrix.setPixel(ledNum,color);
     }
    }
  matrix.show();
}

 // todo: move matrix code here, replace calls 

 void setup_matrix(uint32_t initial_text_color)
 {
    Serial.printf("INIT %d\n",matrix.numPixels());
    matrix.begin();
    matrix.clear();
//  matrix.setTextWrap(false);
  matrix.setBrightness(10);
//  matrix.setTextColor(initial_text_color);
 }

 
int getMatrixWidth()
{
  // todo: uh hopefully this is right...
  return COLUMNS;
}
// note: flipped to match LEDs
int getTrueHeight()
{
  return COLUMNS/BLOCK_SIZE;
}
int getTrueWidth()
{
  return ROWS/BLOCK_SIZE;
}
