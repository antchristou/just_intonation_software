#include "draw_helpers.h"
#include "matrix_mapping.h"
byte drawingMemory[NUM_LEDS*3];         #  3 bytes per LED
DMAMEM byte displayMemory[NUM_LEDS*12]; # 12 bytes per LED

STARTING_BRIGHTNESS = 10; 
def matrix(self, NUM_LEDS, displayMemory, drawingMemory, PIN, WS2812_GRB):


# calculate which LED number a given x, coord is for calling into matrix 



# BLOCK_SIZExBLOCK_SIZE basic shapes
#define BLOCK_SIZE 3
def draw(self, x, y, color, doShow):

    if x < 0:
         Serial.printf("NOT SUPPOSED TO %d %dnot !not !not \n",x, y)
  # MAP VALUES as block expects 
#  Serial.printf("%d %d\n",x,y)
  ledNum = -1
  for(i = (x*BLOCK_SIZE); i < ((x+1)*BLOCK_SIZE); i++)
    for(j = (y*BLOCK_SIZE); j < ((y+1)*BLOCK_SIZE); j++)
     #    Serial.printf("DRAWING BIGGER AT %d %d\n",i,j)
        
        ledNum = lookup(i,j)
        matrix.setPixel(ledNum,color)


  if doShow:
    matrix.show()



 void setup_matrix(uint32_t initial_text_color)
    Serial.printf("INIT %d\n",matrix.numPixels())
    matrix.begin()
    matrix.clear()
#  matrix.setTextWrap(False)
  matrix.setBrightness(STARTING_BRIGHTNESS)
#  matrix.setTextColor(initial_text_color)

   # testLEDs()


 
def getMatrixWidth(self):
  return COLUMNS

# note: flipped to match LEDs
def getTrueHeight(self):
  return ROWS/BLOCK_SIZE

def getTrueWidth(self):
  return COLUMNS/BLOCK_SIZE


def getBlockSize(self):
  return BLOCK_SIZE



# test all LEDS are working by lighting them up
def testLEDs(self):
  Serial.println("testing...")
  for(int i =0; i < NUM_LEDS;i++)
    matrix.setPixel(i,matrix.Color(255,255,255))
    matrix.show()
    delay(100)


