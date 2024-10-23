#include "joystick.h"
#include "just_intonate.h"
#include "draw_helpers.h"
#include "audio_synthesis_v2.h"
#include "matrix_mapping.h"
#include "start_screen.h"
#include <time.h>

#include <elapsedMillis.h>


elapsedMillis blinkTimer;
elapsedMillis moveTimer;
elapsedMillis colorFader;
const int BLINK_TIME = 500;
const int MOVE_RESET_TIME = 300;
const int FADE_OUT_TIME = 8000;
elapsedMillis fadeOutTimer;

elapsedMillis startColorTimer;
const int START_BLINK_TIME = 1200;
int currStartColor = -1;
bool startOff = false;
bool currRotating = false;

// for detecting state transitions
//#define LEFT_DIR 0
//#define RIGHT_DIR 1
//#define UP_DIR 2
//#define DOWN_DIR 3
//#define NOT_MOVING -1

//int currMoving = NOT_MOVING;

// keep tracks of which lights are on 
uint32_t lightsOn[ROWS][COLUMNS];
// keep copy of previous light colors for deletions 
uint32_t previousLightsOn[ROWS][COLUMNS];
// cycles colors for shapes 
int curr_color = 0;
int num_colors = 6;
int numShapes = 0;


bool cursorCurrentlyOn = false;
const bool MIDI_MODE = false;
bool currentlyMirroring = false;

// indicates state where start is being left 
bool transitioning = false;

bool startScreen = true;

 uint32_t colors[] = {
  matrix.Color(178, 0, 178), matrix.Color(133,235,217),matrix.Color(92, 92, 92),matrix.Color(253,162,255),matrix.Color(0, 191, 255),matrix.Color(116, 138, 166)};

 uint32_t la_monte_colors[] = {
  matrix.Color(255,0,0),matrix.Color(237,103,104), matrix.Color(92, 92, 92),matrix.Color(221,153,51),matrix.Color(242,204,132),matrix.Color(255,155,0)};

 uint32_t *colorPointer = colors;
 uint32_t *oppositeColorPointer = la_monte_colors;

 
// keeps track of num blocks in shape that is being drawn - 4 is max for now
int blocksInShape = 0;
// keep track of unique indices in currently drawn shape
int currentShapeIndices[4][2] = {{-1,-1},{-1,-1},{-1,-1},{-1,-1}};
int previousShapeIndices[4][2] = {{-1,-1},{-1,-1},{-1,-1},{-1,-1}};
int shiftShapeIndices[4][2] = {{-1,-1},{-1,-1},{-1,-1},{-1,-1}};

int newShape[4][2] = {{-1,-1},{-1,-1},{-1,-1},{-1,-1}};
// ensures shape has one or more elements of overlap with existing 
bool hasOverlap = false;

bool shiftMode = false;

// reads input from joystick
int joystickVal = -1;
int buttons = 0;

// chord that is currently playing 
double currentlyPlayingChord[8] = {0,0,0,0,0,0,0,0};
// chord that is about to be played, each indice is one channel of form
// [closest midi, pitch bend amount]
double playingChord[8] = {0,0,0,0,0,0,0,0};
double playingFreqChord[4] = {0,0,0,0};

bool nowPlaying = false;

unsigned long startTime;
unsigned long currTime;


void setup() {

  startTime = micros();
  srand(time(0));

  Serial1.begin(2000000);
 // while (!Serial) ; // wait for Arduino Serial Monitor

  setup_matrix(colorPointer[0]);
  Serial.println("\n\nUSB Host Joystick Testing");

  // all lights off to begin
  initMatrix();
  numShapes = 0;

  setupJoystick();
  setupLattice();
  setup_audio();

  generate_mapping();

  startColorTimer = 0;
}

int x    = getMatrixWidth(); 

int pass = 0;

int currCoords[2] = {ROWS/2/getBlockSize(),COLUMNS/2/getBlockSize()};//{ROWS/2,COLUMNS/2};

int prevDir = NOT_MOVING;

// copy lights for bookkeeping 
void copyLightsArray(uint32_t arr1[ROWS][COLUMNS], uint32_t arr2[ROWS][COLUMNS])
{
  for(int i =0; i< ROWS; i++)
  {
    for(int j = 0;j<COLUMNS; j++)
    {
      arr2[i][j] = arr1[i][j];
    }
  }
}


void loop() {
  readJoystick(&joystickVal, &buttons);

  //matrix.fillScreen(0);
  //matrix.setCursor(x, 0);
  //  Serial.printf("MOVING COR %d %d %d\n", shiftMode, currCoords[0], currCoords[1]);
    if(startScreen)
     {
      // blink random colors
      if(startColorTimer > START_BLINK_TIME && startColorTimer < START_BLINK_TIME*2)
      {
          draw_start_screen(OFF_COL);
         
      }
      else if(startColorTimer > START_BLINK_TIME*2)
       {
         startColorTimer = 0;
         startOff = true;
       }
       
       
       if(startOff)
          {
                     
          int random_number = -1;
         
          random_number =  rand() % num_colors;
          
          while(random_number == currStartColor)
              random_number =  rand() % num_colors;
            
          draw_start_screen(colors[random_number]);
          startOff = false;
          currStartColor = random_number;
          }
         // go into gameplay if button pressed
        if(buttons > 0)
        {
             draw_start_screen(WHITE_COL);
             delay(2000);
             
             draw_start_screen(OFF_COL);
             startScreen = false;
             transitioning = true;
        }
      }
     
    if(!startScreen && !transitioning)
    {
    if(!shiftMode)
    {
      movePixel();
    }
    else
      moveShape();

      // if parallel shift button is held, switch modes 
      // shift current coords to most recently drawn shape
      if(buttons == 16 && !shiftMode && numShapes != 0)
      {

        draw(currCoords[0],currCoords[1],previousLightsOn[currCoords[0]][currCoords[1]],true);
        //revert colors to before current shape was drawn
        turnOnPrevious();
        copyLightsArray(previousLightsOn,lightsOn);
        shiftMode = true;
   
   
        currCoords[0] = previousShapeIndices[0][0];
        currCoords[1] = previousShapeIndices[0][1];
        // turn off cursor
       

        // keep track of previously drawn shape
        
        copyArray(previousShapeIndices,shiftShapeIndices);
        copyArray(shiftShapeIndices,newShape);
      }
  
      // if delete button is held unlight curr shape
      if(buttons == 32)
      {
        Serial.printf("DELETE this num of blocks %d\n",blocksInShape);
        deleteShape();
        blocksInShape = 0;
        clearIndices();
      }
      
        // if button is held, draw shape by adding color

  //      Serial.printf("checking %d %d %d %d \n", shiftMode,buttons,blocksInShape,numShapes);
      if(!transitioning && !shiftMode && buttons == 8 && blocksInShape < 4 && 
      (checkOneBlockAway(currCoords[0],currCoords[1]) || (numShapes == 0 && blocksInShape == 0) ))
      {
        if(checkIndices(currCoords[0],currCoords[1]))
        {
        // if shape has color, overlap exists
        if(lightsOn[currCoords[0]][currCoords[1]] != OFF_COL)
        {
          hasOverlap = true;
        }
        draw(currCoords[0], currCoords[1],  colorPointer[curr_color],true);
        Serial.printf("DRAWING %d %d\n",currCoords[0],currCoords[1]);
        lightsOn[currCoords[0]][currCoords[1]] = colorPointer[curr_color];
        currentShapeIndices[blocksInShape][0] = currCoords[0];
        currentShapeIndices[blocksInShape][1] = currCoords[1];
        blocksInShape += 1;
        }
      }
     // submit shape in shift shape mode
     if(buttons == 1 && shiftMode)
     {
      for(int i = 0; i < 4; i++)
      {
          draw(shiftShapeIndices[i][0],shiftShapeIndices[i][1],colorPointer[curr_color],true);
         // matrix.drawPixel(shiftShapeIndices[i][0],shiftShapeIndices[i][1],colors[curr_color]);
          lightsOn[shiftShapeIndices[i][0]][shiftShapeIndices[i][1]] = colorPointer[curr_color];
      }
          blocksInShape = 0;
          numShapes += 1;
          shiftMode = false;
          cycle_color();
          
          copyArray(shiftShapeIndices,previousShapeIndices);
          if(MIDI_MODE == false)
          {
            copyDoubleArray(sonifyFreqMode(shiftShapeIndices),playingFreqChord);
            play_notes(nowPlaying,playingFreqChord);
          }
          else
          {
            copyDoubleFreqArray(sonify(shiftShapeIndices),playingChord);
            sendMidiChord();
          }
          nowPlaying = true;
          
         currCoords[0] = previousShapeIndices[0][0];
        currCoords[1] = previousShapeIndices[0][1];

        
          copyLightsArray(lightsOn,previousLightsOn);
          clearNewShape();

          Serial.printf("COLOR AS OF SHIFT SHAPE SUBMIT %d",lightsOn[shiftShapeIndices[0][0]][shiftShapeIndices[0][1]]);      
          if(currentlyMirroring == true)
            currentlyMirroring = false;
          if(currRotating == true)
            currRotating = false;

          
     }
     // lock shape in, sonify 
     else if(buttons == 1 && blocksInShape == 4  && (hasOverlap == true || numShapes == 0))
     {
      Serial.println("locking in...");
     
      //cycle color for next shape
      cycle_color();
      numShapes++;
      blocksInShape = 0;
      copyArray(currentShapeIndices,previousShapeIndices);
      hasOverlap = false;
//      // get array of notes
//      Serial.println("TRYING TO SONIFY..");
      if(MIDI_MODE == false)
      {
           copyDoubleArray(sonifyFreqMode(currentShapeIndices),playingFreqChord);
           play_notes(nowPlaying,playingFreqChord);
      }
      else
      {
         copyDoubleFreqArray(sonify(currentShapeIndices),playingChord);
         sendMidiChord();
      }
   
         Serial.printf("PRE returned tuning: %d\n", getCurrTuning());
      // voice lead from curr chord to next 
     // voiceLead(playingChord);
      // play chord 
      //sendMidiChord();
      Serial.println("PLAYING!");
    
      clearIndices();
      nowPlaying = true;
      copyLightsArray(lightsOn,previousLightsOn);
    
     }
     // shift tuning systems, trigger transition
     // wipe board in flash of light, except for most recent shape 
     else if(buttons == 2 && numShapes > 0)
     {
        swapTuningSystems();
     }
     // end game 
     else if(buttons == 4 && numShapes > 0)
     {
      endGame();
     }
     // mirror
     else if(buttons == 128 && numShapes > 0 && !currentlyMirroring)
     {
      Serial.println("MIRRORING");
      mirror();
     
     }
     else if(buttons == 64 && numShapes > 0)
     {
      Serial.println("ROTATING");
      rotate();
     }
    
      
     else if(!startScreen)
      {
//        currTime = micros();
        blinkCursor();
      //  unsigned long endTime = micros();
      //  Serial.printf("ELASPED %.8f\n",endTime-currTime);
      }

   //   currTime = micros();
    //  matrix.show();
    }
    else
      transitioning = false;
    delay(10); 
}

bool checkCoords(int x, int y)
{
  // note flipped width and height to match with LED matrix expectations 
  // prevent movement off of board
  int trueWidth = getTrueWidth();
  int trueHeight = getTrueHeight();
 // Serial.printf("BOUNDS: %d %d %d %d\n", trueHeight,trueWidth,x,y);
  if( (x < 0 || x >= trueHeight) || (y < 0 || y >= trueWidth))
    return false;

//  // and prevent movement off of preexisting shape if not first shape
//  if(numShapes != 0)
//  {
//    if(lightsOn[x][y] == OFF_COL && blocksInShape == 0)
//      return false;
//    // restrict movement if not one block away from light that is on
//    else if(lightsOn[x][y] == OFF_COL && !checkOneBlockAway(x,y))
//      return false;
//  }
  return true;
}
bool checkOneBlockAway(int x, int y)
{

  if((x-1 >= 0 && lightsOn[x-1][y] != OFF_COL) || 
     (x+1 < ROWS && lightsOn[x+1][y] != OFF_COL) || 
     (y-1 >= 0 && lightsOn[x][y-1] != OFF_COL) || 
     (y+1 < COLUMNS && lightsOn[x][y+1] != OFF_COL))
     {
      return true;
     }
   return false;
}
// init matrix for keeping track of which ligthts are on
// 0 = off, 1 = on
void initMatrix()
{
  for(int i =0; i< ROWS; i++)
  {
    for(int y =0; y < COLUMNS;y++)
    {
      lightsOn[i][y] = 0;
      previousLightsOn[i][y] = 0;
    }
  }
}

void printLightMatrix()
{
  for(int i =0; i < ROWS;i++)
  {
    for(int y = 0; y< COLUMNS;y++)
    {
    //  Serial.printf("ROWCOL: %d %d",ROWS,COLUMNS);
      Serial.printf("%d ",lightsOn[i][y]);
    }
    Serial.println();
  }
}
// ensure indices are unique in shape
// false indicates position isn't unique and not to add as it's already part of shape
bool checkIndices(int x, int y)
{
  for(int i = 0;i<blocksInShape;i++)
  {
      if(currentShapeIndices[i][0] == x && currentShapeIndices[i][1] == y)
        return false;
  }
  return true;
}
// search for x,y coord in current shape, delete if finds 
bool searchIndices(int x, int y)
{
  for(int i = 0;i<4;i++)
  {
      if(currentShapeIndices[i][0] == x && currentShapeIndices[i][1] == y)
      {
        currentShapeIndices[i][0] = -1;
        currentShapeIndices[i][1] = -1;
        return true;
      }
  }
  return false;
}

bool searchPreviousArray(int x, int y)
{
  for(int i = 0;i<4;i++)
  {
//      Serial.printf("checking for...%d %d %d %d\n",x,y,previousShapeIndices[i][0],(previousShapeIndices[i][1]));
      if(previousShapeIndices[i][0] == x && previousShapeIndices[i][1] == y)
      {
        return true;
      }
  }
  return false;
}
void clearIndices()
{
  for(int x = 0;x<blocksInShape;x++)
  {
    for(int y= 0; y<2;y++)
    {
      currentShapeIndices[x][y] = -1;
    }
  }
}

// for clearing array post shift mode 
void clearNewShape()
{
  for(int x = 0;x<4;x++)
  {
    for(int y= 0; y<2;y++)
    {
      newShape[x][y] = -1;
    }
  }
}

// when shape is submitted, ensure each point in shape is one away from another 
//bool checkContiguous()
//{
//  int p0[2] = currentShapeIndice[0];
//  int p1[2] = currentShapeIndice[1];
//  int p2[2] = currentShapeIndice[2];
//  int p3[2] = currentShapeIndice[3];
//  
//  for(int i = 0; i< 4;i++)
//  {
//    if(currentShapeIndice[i][0]+1 == currentShapeIndice[i+1][0])
//      
//  }
//}


// copy contents of arr1 to arr2
void copyDoubleArray(double arr1[4], double arr2[4])
{


  Serial.printf("THE TUNING IS %d\n",Lattice_TUNING);
  for(int i =0; i< 4; i++)
  {
      arr2[i] = arr1[i];
  }
  Serial.printf("%.2f %.2f \n",arr2[0],arr2[1]);
  Serial.printf("THE TUNING IS NOW %d\n",Lattice_TUNING);
}


// copy contents of arr1 to arr2
void copyDoubleFreqArray(double arr1[8], double arr2[8])
{


  Serial.printf("THE TUNING IS %d\n",Lattice_TUNING);
  for(int i =0; i< 4; i++)
  {
      arr2[i] = arr1[i];
  }
  Serial.printf("%.2f %.2f \n",arr2[0],arr2[1]);
  Serial.printf("THE TUNING IS NOW %d\n",Lattice_TUNING);
}


// copy contents of arr1 to arr2
void copyArray(int arr1[4][2], int arr2[4][2])
{
  for(int i =0; i< 4; i++)
  {
    for(int j = 0;j<2; j++)
    {
      arr2[i][j] = arr1[i][j];
    }
  }
}

void movePixel()
{
    //  Serial.println("moving pixel...");
     
      int newCoordX = currCoords[0];
      int newCoordY = currCoords[1];
      bool moved = false;
     // Serial.printf("%d %d\n", joystickVal,currMoving);

      if(joystickVal == LEFT_DIR && (prevDir != LEFT_DIR))
      {
        newCoordY -= 1;
        moved = true;
        moveTimer = 0;
      }
      else if(joystickVal == RIGHT_DIR && (prevDir != RIGHT_DIR))
      {
        moved =  true;
        newCoordY += 1;
        moveTimer = 0;
      }
      else if(joystickVal == UP_DIR && (prevDir != UP_DIR))
      {
        moved = true;
        newCoordX -= 1;
        moveTimer = 0;
      }
      else if(joystickVal == DOWN_DIR && (prevDir != DOWN_DIR))
      {
        moved = true;
        newCoordX += 1;
        moveTimer = 0;
      }

    //  Serial.printf("%d %d \n",newCoordX,newCoordY);
      if(moved)
      {
      if(lightsOn[currCoords[0]][currCoords[1]] == OFF_COL)
      {
        draw(currCoords[0],currCoords[1],OFF_COL,true);
     //   matrix.show();
       // matrix.drawPixel(currCoords[0],currCoords[1],OFF_COL);
      }
      
      // light is currently part of shape, revert to earlier color
      if(lightsOn[currCoords[0]][currCoords[1]] != OFF_COL)
      {
        draw(currCoords[0],currCoords[1],lightsOn[currCoords[0]][currCoords[1]],true);
    //    matrix.show();
        //matrix.drawPixel(currCoords[0],currCoords[1],lightsOn[currCoords[0]][currCoords[1]]);
      }
      
      if(checkCoords(newCoordX, newCoordY))
            {
              if(lightsOn[currCoords[0]][currCoords[1]] == OFF_COL)
              {
            
                Serial.printf("THINKS WHAT YOURE MOVING OFF IS LIGHTS OFF\n");
                draw(currCoords[0],currCoords[1],OFF_COL,true);
               // matrix.show();
            //  matrix.drawPixel(currCoords[0],currCoords[1],OFF_COL);      
              }
              if(lightsOn[currCoords[0]][currCoords[1]] != OFF_COL)
              {
                  
                Serial.printf("COLOR AS OF MOVING OFF %d",lightsOn[currCoords[0]][currCoords[1]]);
                draw(currCoords[0],currCoords[1],lightsOn[currCoords[0]][currCoords[1]],true);
            //    matrix.show();
               // matrix.drawPixel(currCoords[0],currCoords[1],lightsOn[currCoords[0]][currCoords[1]]);
              }
              currCoords[0] = newCoordX;
              currCoords[1] = newCoordY;
            }
        else
          Serial.println("BOUNDARY FAIL");
      }

       prevDir = currMoving;
  
}       

// move shape itself for shift mode
 void moveShape()
 {
     
      bool outOfBounds = false;
      // shape has 4 blocks
      int x_shift = 0;
      int y_shift = 0;
     // Serial.println("copying..");
      copyArray(shiftShapeIndices,newShape);
    
      if(joystickVal == LEFT_DIR && (prevDir != LEFT_DIR || moveTimer > MOVE_RESET_TIME))
      {
        y_shift = -1;
        moveTimer = 0;
      }
      else if(joystickVal == RIGHT_DIR && (prevDir != RIGHT_DIR || moveTimer > MOVE_RESET_TIME))
      {
        y_shift = 1;
        moveTimer = 0;
      }
      else if(joystickVal == UP_DIR && (prevDir != UP_DIR || moveTimer > MOVE_RESET_TIME))
      {
        x_shift = -1;
        moveTimer = 0;
      }
      else if(joystickVal == DOWN_DIR && (prevDir != DOWN_DIR || moveTimer > MOVE_RESET_TIME))
      {
        x_shift = 1;
        moveTimer = 0;
      }
      
      //Serial.printf("%d %d \n",newCoordX,newCoordY);
      
      // check boundaries of new shape
      for(int j = 0; j < 4; j++)
      {
        if(!checkCoords(newShape[j][0]+x_shift, newShape[j][1]+y_shift))
        {
          outOfBounds = true;
          Serial.println("BAD FAIL"); 
        }
    
      }
      
      if(!outOfBounds && (x_shift != 0 || y_shift != 0))
      {
           // process lights for entire existing shape
          for(int i =0; i <4;i++)
          {
            if(lightsOn[newShape[i][0]][newShape[i][1]] == OFF_COL)
            {
           
              draw(newShape[i][0],newShape[i][1],OFF_COL,true);
            //  matrix.show();
        //      matrix.drawPixel(newShape[i][0],newShape[i][1],OFF_COL);
            }
          }
          
          // light is currently part of shape, revert to earlier color
          
    
          for(int i =0; i < 4;i++)
          {
            if(lightsOn[newShape[i][0]][newShape[i][1]] != OFF_COL)
            {
                
             // Serial.printf("%d %d\n",newShape[i][0],newShape[i][1]);
             draw(newShape[i][0],newShape[i][1],lightsOn[newShape[i][0]][newShape[i][1]],true);
          //   matrix.show();
            //  matrix.drawPixel(newShape[i][0],newShape[i][1],lightsOn[newShape[i][0]][newShape[i][1]]);
            }
          }

     //     Serial.printf("PRE SHIFT %d %d %d %d %d %d %d %d\n",newShape[0][0],newShape[0][1],newShape[1][0],newShape[1][1],newShape[2][0],newShape[2][1],newShape[3][0],newShape[3][1]);
          // move shape
          for(int z = 0; z < 4;z++)
          {
            shiftShapeIndices[z][0] += x_shift;
            shiftShapeIndices[z][1] += y_shift;
            
            newShape[z][0] += x_shift;
            newShape[z][1] += y_shift;
              
          }
       //   Serial.printf("POST SHIFT %d %d %d %d %d %d %d %d\n",newShape[0][0],newShape[0][1],newShape[1][0],newShape[1][1],newShape[2][0],newShape[2][1],newShape[3][0],newShape[3][1]);
        //  Serial.println(newShape[0][0]);

          // current coord gets set arbitrarily to first indice of shape
          currCoords[0] = newShape[0][0];
          currCoords[1] = newShape[0][1];
      }
    
      
      prevDir = currMoving;
    //  delay(1000);

}

void cycle_color()
{
   curr_color += 1;
   if(curr_color >= num_colors)
    curr_color = 0;
}
void sendMidiChord()
{
   // turn off current chord if one is playing
   if(nowPlaying)
   {
      Serial.println("TURNING OFF");
    for(int i =0;i<8;i+=2)
    {
    
    usbMIDI.sendNoteOff(currentlyPlayingChord[i], 99, i);
    }
   }
   for(int i =0;i<8;i+=2)
   {
   Serial.printf("SENDING,%.1f %.2f \n",playingChord[i],playingChord[i+1]);
   
   usbMIDI.sendNoteOn(playingChord[i], 99, i);
   usbMIDI.sendPitchBend(playingChord[i+1],i);
   }
   copyDoubleArray(playingChord,currentlyPlayingChord);
}

void deleteShape()
{
  Serial.printf("DELETING CURR SHAPE %d\n",blocksInShape);
  for(int i =0; i < blocksInShape;i++)
  {
     
      // revert to what was there originally 
      draw(currentShapeIndices[i][0],currentShapeIndices[i][1],
      previousLightsOn[currentShapeIndices[i][0]][currentShapeIndices[i][1]],true);
   
      lightsOn[currentShapeIndices[i][0]][currentShapeIndices[i][1]] = previousLightsOn[currentShapeIndices[i][0]][currentShapeIndices[i][1]];

  }
 //  matrix.show();
}

void blinkCursor()
{
  
  if(blinkTimer >= BLINK_TIME*1.5)
      blinkTimer = 0;
  else if (blinkTimer >= BLINK_TIME) 
  {
      if(shiftMode)
      { 
         // Serial.printf("ok 8 .. %d %d ",currCoords[0],currCoords[1]);
     //  Serial.printf("new shape %d \n",newShape[0][0]);
        for(int z = 0; z < 4; z++)
            draw(newShape[z][0],newShape[z][1],OFF_COL,false);
        matrix.show();
          //  matrix.drawPixel(newShape[z][0],newShape[z][1],OFF_COL);
  //      cursorCurrentlyOn = false;
      //  matrix.show();
      }
       else
       {
           // Serial.printf("making NON WHITE in new shape bklin\n");
          //    Serial.println("trying to draw off...");
//                   Serial.printf("ok.9. %d %d ",currCoords[0],currCoords[1]);
              draw(currCoords[0],currCoords[1],OFF_COL,true);
       //       cursorCurrentlyOn = false; 
          
           
         // matrix.show();
       }
        //  matrix.drawPixel(currCoords[0],currCoords[1],OFF_COL);
             
  }
  else
  {
     if(shiftMode)
      {
        for(int z = 0; z < 4; z++)
        {
//            Serial.printf("ok 10.. %d %d \n",newShape[z][0],newShape[z][1]);
            draw(newShape[z][0],newShape[z][1],WHITE_COL,true);
       //     matrix.drawPixel(newShape[z][0],newShape[z][1],WHITE_COL);
        }
      //  cursorCurrentlyOn = true;
       
      }
      else
      {
//            Serial.printf("ok 11.. %d %d\n ",currCoords[0],currCoords[1]);  // Serial.printf("making white in blink %d %d\n",currCoords[0],currCoords[1]);
      //   Serial.println("trying to draw white...");
       draw(currCoords[0],currCoords[1],WHITE_COL,true);
     //  cursorCurrentlyOn = true; 
      
     //   matrix.drawPixel(currCoords[0],currCoords[1],WHITE_COL);
      }
  }
      
}

void swapTuningSystems()
{

  // highlight most recent shape no matter where cursor is 
  draw(currCoords[0],currCoords[1],previousLightsOn[currCoords[0]][currCoords[1]],true);
  for(int z = 0; z < 4; z++)
    draw(previousShapeIndices[z][0],previousShapeIndices[z][1],previousLightsOn[previousShapeIndices[z][0]][previousShapeIndices[z][1]],true);

  Serial.println("pre flash!");
  
  // trigger clearing animation 
  flashWhite();
  Serial.println("Done flashing!");

  // bookkeeping for new tuning system 
  // last shape from before persists
  numShapes = 1;
  initMatrix();
  
  // change tuning lattice to reflect new system
  swapTuning();
  setupLattice();
  
  // slide pitches into what it represnets in new system - should always need to slide
  
  Serial.printf("indices: %d %d %d %d %d %d %d %d\n",previousShapeIndices[0][0],previousShapeIndices[0][1],previousShapeIndices[1][0],previousShapeIndices[1][1],previousShapeIndices[2][0],
  previousShapeIndices[2][1],previousShapeIndices[3][0],previousShapeIndices[3][1]);


   // swap color schemes
   if(curr_color > 0)
      fadeIntoColor(colorPointer[curr_color-1],oppositeColorPointer[0]);
    else
      fadeIntoColor(colorPointer[num_colors-1],oppositeColorPointer[0]);
  if(MIDI_MODE == false)
  {


  copyDoubleArray(sonifyFreqMode(previousShapeIndices),playingFreqChord);
  play_notes(nowPlaying,playingFreqChord);
  }
  else
  {
     copyDoubleFreqArray(sonify(previousShapeIndices),playingChord);
     sendMidiChord();
  }
  curr_color = 1;


  // swap color pointers 
  uint32_t *temp = colorPointer;
  colorPointer = oppositeColorPointer;
  oppositeColorPointer = temp;


  // clear lights array 
  clearLights();
  Serial.printf("PREV SHAPE: %d %d %d %d %d %d %d %d\n",previousShapeIndices[0][0],previousShapeIndices[0][1],previousShapeIndices[1][0],previousShapeIndices[1][1],
  previousShapeIndices[2][0],previousShapeIndices[2][1],previousShapeIndices[3][0],previousShapeIndices[3][1]);
for(int i = 0; i < 4;i++)
  {
    lightsOn[previousShapeIndices[i][0]][previousShapeIndices[i][1]] = colorPointer[0];
    Serial.printf("COLOR NIMBER AS OF %d %d\n", lightsOn[previousShapeIndices[i][0]][previousShapeIndices[i][1]],colorPointer[0]);
    previousLightsOn[previousShapeIndices[i][0]][previousShapeIndices[i][1]] = colorPointer[0];
    draw(previousShapeIndices[i][0],previousShapeIndices[i][1],colorPointer[0],true);
  }
  copyLightsArray(lightsOn,previousLightsOn);
  

  Serial.printf("Swapping to %d!\n",Lattice_TUNING);
  Serial.printf("COLOR NIMBER %d\n",colorPointer[0]);
  Serial.printf("COLOR NIMBER 2 %d\n", lightsOn[previousShapeIndices[0][0]][previousShapeIndices[0][1]]);
} 

void clearLights()
{
  for(int i =0; i< ROWS; i++)
  {
    for(int j = 0; j < COLUMNS;j++)
    {
      lightsOn[i][j] = OFF_COL;
      previousLightsOn[i][j] = OFF_COL;
    }
  }
}

// flash board white for a moment, clearing it except for most previous shape drawn
void flashWhite()
{
    currCoords[0] = previousShapeIndices[0][0];
    currCoords[1] = previousShapeIndices[0][1];
    Serial.println("pre white!");

    int blockSize = getBlockSize();
    for(int x = 0; x < ROWS/blockSize; x++)
    {
      for(int y = 0; y < COLUMNS/blockSize; y++)
      {
        if (!searchPreviousArray(x,y))
        {
          draw(x,y,WHITE_COL,true);
        }
       
      }
    }
  

    delay(1000);
    Serial.println("post white!");
  
   
    for(int x = 0; x < ROWS/blockSize; x++)
    {
      for(int y = 0; y < COLUMNS/blockSize; y++)
      {
        if (!searchPreviousArray(x,y))
        {
          draw(x,y,OFF_COL,true);
        }
      }
    }

   //  matrix.show();
}

// fade previous shape from color1 to color2
void fadeIntoColor(uint32_t color1,uint32_t color2)
{
 // colorFader = 0;
  //int FADE_TIME = 2000;
 
//  Serial.println(color1);
//  //extract out RGB values, calcualte difference
//  uint16_t red1 =  (color1 >> 8) & 0xF8;  
//  uint16_t green1 =  (color1 >> 3) & 0xFC;  
//  uint16_t blue1 =  (color1 << 3) & 0xF8;
//
//  uint8_t  red2 = (color2 >> 8) & 0xF8;
//  uint8_t   green2 = (color2 >> 3) & 0xFC;  
//  uint8_t   blue2 = (color2 << 3) & 0xF8;
     // Serial.printf("GOAL COLOR %d %d %d \n",red2,green2,blue2);
//  
//  int distances[3] = {red1-red2,green1-green2,blue1-blue2};
//  Serial.printf("dist %d %d %d\n",distances[0], distances[1],distances[2]);
//  double unitChange[3] = {((double)distances[0]/FADE_TIME),((double)distances[1]/FADE_TIME),((double)distances[2]/FADE_TIME)};
//  unitChange[0] *= 100;
//  unitChange[1] *= 100;
//  unitChange[2] *= 100;      
//  Serial.printf("unit change: %.2f %.2f %.2f\n",unitChange[0], unitChange[1],unitChange[2]);
//  uint16_t newColor;
//  while(colorFader < FADE_TIME)
//  {
//    if(colorFader % 100==0)
//    {
//    red1 = red1+(int)unitChange[0];
//    green1 = green1+(int)unitChange[1];
//    blue1 = blue1+(int)unitChange[2];
//    Serial.printf("NEW %d %d %d \n",red1,green1,blue1);
//    for(int i =0; i< 4;i++)
//    {
//    
//      newColor = matrix.Color(red1,green1,blue1);
//    //  Serial.printf("NEW COLOR %d\n",newColor);
//      draw(previousShapeIndices[i][0], previousShapeIndices[i][1],newColor);
//    }
//    }
//  }
// todo fix color fading
  for(int i =0; i< 4;i++)
    {
      
      draw(previousShapeIndices[i][0], previousShapeIndices[i][1],color2,false);
    }
     matrix.show();
 
  Serial.println("DONE FADING!");
  
  
}

void turnOnPrevious()
{
  for(int i =0; i < ROWS;i++)
  {
    for(int j = 0; j < COLUMNS; j++)
    {
      if(lightsOn[i][j] != previousLightsOn[i][j])
        draw(i,j,previousLightsOn[i][j],true);
    }
  }
}

void endGame()
{

  
  // remove cursor
  draw(currCoords[0],currCoords[1], lightsOn[currCoords[0]][currCoords[1]],true);

  Serial.println("ending synths..");
  // trigger end envelope of synths
  turnAllOff();

  // change back to 5 limit

  numShapes = 1;
  if(Lattice_TUNING != FIVE_LIMIT_JUST)
  {
    initMatrix();
    swapTuning();
    setupLattice();
  
    curr_color = 1;
    // swap color pointers 
    uint32_t *temp = colorPointer;
    colorPointer = oppositeColorPointer;
    oppositeColorPointer = temp;
  }

  // end code to change back to 5 limit ^ 

  
  // fade out lights 
  fadeOutTimer = 0;
  int currBrightness = STARTING_BRIGHTNESS;
  Serial.println("fading lights...");
  while(fadeOutTimer < FADE_OUT_TIME)
  {
    int mappedBrightness = map((int)fadeOutTimer,0,FADE_OUT_TIME,STARTING_BRIGHTNESS,0);
    matrix.setBrightness(mappedBrightness);
    if(fadeOutTimer % 50 == 0)
      matrix.show();
  }


  shiftMode = false;
  // return to start screen and clear
  startScreen = true;
  startColorTimer = 0;
  delay(5000);
  matrix.clear();
  clearIndices();
  clearNewShape();
  clearLights();
  numShapes = 0;
  blocksInShape = 0;
  nowPlaying  = false;
  currCoords[0] = ROWS/2/getBlockSize();
  currCoords[1] = COLUMNS/2/getBlockSize();
  matrix.setBrightness(STARTING_BRIGHTNESS);

}

void mirror()
{
   draw(currCoords[0],currCoords[1],previousLightsOn[currCoords[0]][currCoords[1]],true);
        //revert colors to before current shape was drawn
        turnOnPrevious();
        copyLightsArray(previousLightsOn,lightsOn);
        shiftMode = true;
   
   
        currCoords[0] = previousShapeIndices[0][0];
        currCoords[1] = previousShapeIndices[0][1];
        // turn off cursor
       
        // keep track of previously drawn shape
        // mirror shape and use that as shift shape 
        mirrorCoords(previousShapeIndices,shiftShapeIndices);
      //  copyArray(previousShapeIndices,shiftShapeIndices);
        copyArray(shiftShapeIndices,newShape);

        currentlyMirroring = true;

}

void mirrorCoords(int previousShapeIndices[4][2],int shiftShapeIndices[4][2])
{
  // mirror coords towards center axis 
  int line_x = 5; // center line 
  copyArray(previousShapeIndices,shiftShapeIndices);

  Serial.printf("OLD MIRROR COORDS %d %d %d %d %d %d %d %d \n",shiftShapeIndices[0][0],shiftShapeIndices[0][1],
  shiftShapeIndices[1][0],shiftShapeIndices[1][1],shiftShapeIndices[2][0],shiftShapeIndices[2][1],shiftShapeIndices[3][0],
  shiftShapeIndices[3][1]);
   for (int i = 0; i < 4; i++) {
        shiftShapeIndices[i][1] = (2 * line_x - shiftShapeIndices[i][1])-1; // Reflect x-coordinate
    }

  Serial.printf("NEW MIRROR COORDS %d %d %d %d %d %d %d %d \n",shiftShapeIndices[0][0],shiftShapeIndices[0][1],
  shiftShapeIndices[1][0],shiftShapeIndices[1][1],shiftShapeIndices[2][0],shiftShapeIndices[2][1],shiftShapeIndices[3][0],
  shiftShapeIndices[3][1]);

    
}

void rotate()
{

    draw(currCoords[0],currCoords[1],previousLightsOn[currCoords[0]][currCoords[1]],true);
      turnOnPrevious();
        copyLightsArray(previousLightsOn,lightsOn);

        // keep track of previously drawn shape
        // rotate 90 degrees clockwise
        bool rotatePassed;
    
          rotatePassed = rotateCoords(previousShapeIndices,shiftShapeIndices);
          currCoords[0] = previousShapeIndices[0][0];
          currCoords[1] = previousShapeIndices[0][1];
        
     
           
        if(rotatePassed)
        {
          shiftMode = true;
        
           draw(currCoords[0],currCoords[1],previousLightsOn[currCoords[0]][currCoords[1]],true);
        //revert colors to before current shape was drawn
        turnOnPrevious();
        copyLightsArray(previousLightsOn,lightsOn);
     

     
        // turn off cursor
     
        copyArray(shiftShapeIndices,newShape);

        currRotating = true;

        }
  
}

bool rotateCoords(int previousShapeIndices[4][2],int shiftShapeIndices[4][2])
{
  // rotate 90 degrees clockwise UNLESS WOULD BE OUT OF BOUND
  

  Serial.printf("OLD ROTATE COORDS %d %d %d %d %d %d %d %d \n",previousShapeIndices[0][0],previousShapeIndices[0][1],
   previousShapeIndices[1][0],previousShapeIndices[1][1],previousShapeIndices[2][0],previousShapeIndices[2][1],previousShapeIndices[3][0],
  previousShapeIndices[3][1]);


   // make first point the rotation axis
  int pivot_x =  previousShapeIndices[0][0];
  int pivot_y =  previousShapeIndices[0][1];
  
  int rotateCoords[4][2];

    // Rotate coordinates 90 degrees clockwise
    for (int i = 0; i < 4; i++) {
   
        int dx = previousShapeIndices[i][0] - pivot_x;
        int dy = previousShapeIndices[i][1] - pivot_y;
        rotateCoords[i][0] = pivot_x + dy;
        rotateCoords[i][1] = pivot_y - dx;
    }

    
   Serial.printf("TRYING NEW ROTATE COORDS %d %d %d %d %d %d %d %d \n",rotateCoords[0][0],rotateCoords[0][1],
   rotateCoords[1][0],rotateCoords[1][1],rotateCoords[2][0],rotateCoords[2][1],rotateCoords[3][0],
  rotateCoords[3][1]);

   // check if rotation is in bounds
   bool rotateTest = true;
   for(int i = 0; i < 4; i++)
    {
      if(!checkCoords(rotateCoords[i][0],rotateCoords[i][1]))
        rotateTest = false;
    }
    if(rotateTest)
    {
      copyArray(previousShapeIndices,shiftShapeIndices);
      Serial.println("PASSED ROTATION CHECK");
      for(int i = 0; i < 4; i++)
      {
      shiftShapeIndices[i][0] =  rotateCoords[i][0];
      shiftShapeIndices[i][1] =  rotateCoords[i][1];
      }

    copyArray(shiftShapeIndices,newShape);
    }
    else
    {
       Serial.println("INITIALLY FAILED ROTATION CHECK");
      // move rotation over so it fits 
       
      int maxOutOfBoundsX = 0;
      int maxOutOfBoundsY = 0;
      // calculate amount out of bounds
      for(int i = 0; i < 4; i++)
      {
        if(rotateCoords[i][0] < 0 || rotateCoords[i][0] > 4)
        {
          if(abs(rotateCoords[i][0]) > maxOutOfBoundsX)
            maxOutOfBoundsX = rotateCoords[i][0];
        }
        if(rotateCoords[i][1] < 0 || rotateCoords[i][1] > 9)
        {
          if(abs(rotateCoords[i][1]) > maxOutOfBoundsY)
            maxOutOfBoundsY = rotateCoords[i][1];
        }
      }
      Serial.printf("correction: %d %d\n",maxOutOfBoundsX,maxOutOfBoundsY);
      for(int i = 0; i<4;i++)
      {
         if(maxOutOfBoundsX < 0)
          rotateCoords[i][0] =  rotateCoords[i][0]+maxOutOfBoundsX*-1;
         if(maxOutOfBoundsY < 0)
          rotateCoords[i][1] =  rotateCoords[i][1]+maxOutOfBoundsY*-1;

         if(maxOutOfBoundsX > 0)
          rotateCoords[i][0] =  rotateCoords[i][0]+(maxOutOfBoundsX-4)*-1;
         if(maxOutOfBoundsY > 0)
          rotateCoords[i][1] =  rotateCoords[i][1]+(maxOutOfBoundsY-4)*-1;
      }

      for(int i =0; i < 4;i++)
        {
          shiftShapeIndices[i][0] = rotateCoords[i][0]; 
          shiftShapeIndices[i][1] =  rotateCoords[i][1];
        }
     Serial.printf("MOVED SHAPE OVER TO FIT: %d %d %d %d %d %d %d %d\n",  shiftShapeIndices[0][0],shiftShapeIndices[0][1],
   shiftShapeIndices[1][0],shiftShapeIndices[1][1],shiftShapeIndices[2][0],shiftShapeIndices[2][1],shiftShapeIndices[3][0],
  shiftShapeIndices[3][1]);
      
    copyArray(shiftShapeIndices,newShape);
      
      
    }
      
   

   return true;
}
//
//void setup_audio2() {
//  Serial.println("Setting up audio");
//  // put your setup code here, to run once:
// AudioMemory(20);
//  sgtl5000_1.enable();
//  sgtl5000_1.volume(0.2);
//  
////
////  freeverb1.roomsize(0.8);
////  freeverb1.damping(0.8);
//
//   waveform1.frequency(440);
//   waveform1.amplitude(1.0);
////   
//  waveform1.begin(WAVEFORM_SINE);
//}
