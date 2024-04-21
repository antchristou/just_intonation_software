// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library

#include "just_intonate.h"
#include <stdlib.h>
#include <math.h>
#include <Arduino.h>


//todo: possibly hard code this less badly...
// change these TODO 
const int LATTICE_X_DIM = 20;
const int LATTICE_Y_DIM = 30;
int Lattice_TUNING = FIVE_LIMIT_JUST;
const double FREQ_UPPER_BOUND = 1500;
const double FREQ_LOWER_BOUND = 120;
int GEN_COUNT = 0;

typedef struct {
  int closestMidiNote;
  double freq;
  
} latticeIndice;

latticeIndice*lattice[LATTICE_X_DIM][LATTICE_Y_DIM];

int key = 9; // 0 corresponds to C as tonic, 1 as Db, 9 as A etc 
double currTonicFreq = 440;
//int tuning_system = FIVE_LIMIT_JUST;

// ratios for calculating frequencies from base 
// TODO: maybe change to five limit? 
double justRatios[] = {1.0/1,17.0/16, 9.0/8,19.0/16, 5.0/4, 21.0/16, 11.0/8,3.0/2,13.0/8,27.0/16,7.0/4,15.0/8};

// base tuning is by default A=440
double baseTuning = 440.0;

double*chord;


//contains pure frequencies 
double*freqChord;

// array that contains indices corresponding shape of notes traced out 
// TODO: contains 4 elements for now
int latticeShape[8];
int indicesCollected = 0;

// true if notes are currently being played 
bool playing = false;

void setupChordArray()
{
  chord = (double*)malloc(4 * 2 * sizeof(double));
  freqChord = (double*)malloc(4 * sizeof(double));
}

double midi2Freq(double midiNote)
{
  int A4MIDI = 69;
  // calculate semitones from A4 
  int offsetFromA = midiNote-A4MIDI;

  // convert to frequency 

  double freq = pow(2,(offsetFromA/12.0))*baseTuning;
  return freq;
}

// used to determine scale degree given some tonic and some note
// ex. D in C is 2, F# in F is 1, etc (0 indexed)
int noteDistance(int tonic, int midiNote)
{
  midiNote = midiNote % 12;
  
  int distance = midiNote-tonic;

  // wrap around if negative 
  if(distance < 0)
    distance += 12; 
  
  return distance;
  
}

void printLattice()
{
  Serial.printf("Curr tuning is %d\n",Lattice_TUNING);
  for(int i = 0; i < LATTICE_X_DIM; i++)
  {
    for(int y = 0; y < LATTICE_Y_DIM; y++)
    {
      latticeIndice*entry = lattice[i][y];
       Serial.print(entry->freq);
      Serial.print(" ");
    }
    Serial.println();
  }
}


// given some freq in hz, find closest midi for playing later 
int calcClosestMidi(double freq) {
 
    double midiNoteFrequencies[128];
       
    // Calculate MIDI note frequencies
    for (int i = 0; i < 128; i++) {
        midiNoteFrequencies[i] = midi2Freq(i);
    }

    // Find the closest MIDI note
    int closestNote = 0;
    double minDifference = fabs(freq - midiNoteFrequencies[0]);

    for (int i = 1; i < 128; i++) {
        double difference = fabs(freq - midiNoteFrequencies[i]);
        if (difference < minDifference) {
            minDifference = difference;
            closestNote = i;
        }
    }

    return closestNote;
}

// used to ensure frequencies in matrix don't get too high or low 
double wrapFreq(double freq)
{
  while(freq > FREQ_UPPER_BOUND)
    freq = freq/2.0;
  while(freq < FREQ_LOWER_BOUND)
    freq = freq*2.0;
  return freq;
}

void generateLattice()
{

  Serial.print("GENERATING \n");
  // place A4 in middle 
  int middleXIndice = LATTICE_X_DIM/2;
  int middleYIndice = LATTICE_Y_DIM/2;

 // Serial.printf("MIDDLE %d %d \n", middleXIndice,middleYIndice);
  latticeIndice*middleEntry = (latticeIndice*) malloc(sizeof(latticeIndice));
  middleEntry->closestMidiNote = 69;
  middleEntry->freq = baseTuning;
  lattice[middleXIndice][middleYIndice] = middleEntry;
  

  int columnsFilled = 0;
  int currColumn = middleYIndice;
  int currCenter = middleXIndice;
  double currCenterFreq = baseTuning;

  double verticalRatio=1;
  double horizontalRatio=1;
  if(Lattice_TUNING == FIVE_LIMIT_JUST)
  {
    verticalRatio=3.0/2;
    horizontalRatio=5.0/4;
    Serial.println("MAKING LATTICE IN 5 LIMIT JUST MODE");
  }
  else if(Lattice_TUNING == LA_MONTE_SEVEN)
  {
    verticalRatio=3.0/2;
    horizontalRatio=7.0/4;
    Serial.println("MAKING LATTICE IN LA MONTE MODE");
  }
  
  
  // if tuning sytem is 5 limit just intonation, horizontal axis is thirds, vertical is fifths
  // in La monte young seven limit, horizontal axis is sevenths, vertical is fifths
    while(columnsFilled < LATTICE_Y_DIM)
    {
      
      // fill in from middle to end
      for(int i = 0; i < LATTICE_X_DIM; i++)
      {
        // calculate how many fifths apart we are 
        int powerVal = currCenter-i; 
        double freq = pow(verticalRatio, powerVal)*currCenterFreq;
        // keep within range of 440-880 hz
        freq = wrapFreq(freq);
        int closestMidi = calcClosestMidi(freq); 
        latticeIndice*entry = (latticeIndice*) malloc(sizeof(latticeIndice));
        entry->closestMidiNote = closestMidi;
        entry->freq = freq;
        lattice[i][currColumn] = entry;

      }
      currColumn += 1;
      columnsFilled += 1;


      // going over to next column, we move by a just major third / 
      currCenterFreq = horizontalRatio*currCenterFreq;
      // wrap around when reach end
      if(currColumn >= LATTICE_Y_DIM)
      {
        currColumn = 0;
        currCenterFreq = pow(horizontalRatio,-1*middleYIndice)*baseTuning;
      }
    
    }
  // TODO get rid of 
  GEN_COUNT += 1;
  //printLattice();
 
}

// in this case, just and equally tempered 
double calculateCentsOffset(double freq1,double freq2)
{
  // c = 1200 × log2 (f2 / f1)
   
  // calculate log base 2
  double log2 = log((freq2/freq1)) / log(2);

  double centsOffset = 1200*log2;
  return centsOffset;
}


// calculate MPE pitch bend value to send to achieve cents offset
// MPE ranges from -8192 to 8191 meaning that 
// the perfectly exact value can't be achieved but can get very close
// ASSUMPTION: on synth end, MPE pitch bend value is set to one whole tone 
// TODO: 2 octaves 
double calculateBend(double centsOffset)
{
  // assuming that pitch band max is 1 whole step (200 cents) 
  double centIncrement = 2400.0/8191.0;
  // TODO: this seems not accurate enough
  // calculate how many pitch bend increments approximate difference in cents;
  double doubleBend = centsOffset/centIncrement;
  // round to nearest integet as MPE only accepts integers 
  int pitchBendValue = round(doubleBend);
 // Serial.println(pitchBendValue);

  return pitchBendValue;
  
}

// calculate mpe pitch bend to play microtuned note
int calcBendAmount(latticeIndice* note)
{
  double desiredFreq = note->freq;
  int closestMidi = note->closestMidiNote;
  double tweleveTetFreq = midi2Freq(closestMidi);
  
  double centsOffset = calculateCentsOffset(tweleveTetFreq,desiredFreq);
   
  // Calculate closest possible pitch bend value to achieve that offset
  int bendAmount = calculateBend(centsOffset);
  return bendAmount;
}



void setupLattice() {
  generateLattice();
  setupChordArray();
 // Serial.println("GENERATED Lattice");
  printLattice();
}

double*sonify(int coords[4][2]) {

 for(int i =0; i<8;i+=2)
  {
   int mapped = i*4/8;
   //Serial.printf("COORDS %d %d", coords[mapped][0],coords[mapped][1]);
   latticeIndice*selectedEntry1 = lattice[coords[mapped][0]][coords[mapped][1]];
   int bendAmount1 = calcBendAmount(selectedEntry1);
   
   Serial.printf("CURR CLOSEST MIDI+DESIRED FREQ %d %.2f\n", selectedEntry1->closestMidiNote,selectedEntry1->freq);
//   Serial.printf("MIDI FREQ IN 12 TET %.2f \n", midi2Freq(selectedEntry1->closestMidiNote));
   Serial.printf("BEND AMOUNT %d\n", bendAmount1);
//
   Serial.println("SENDING MIDI");
    chord[i] = selectedEntry1->closestMidiNote;
    chord[i+1] = bendAmount1;
  }

  playing = true;
  
  return chord;
 
}

double*sonifyFreqMode(int coords[4][2]) {

 
 Serial.println("trying to sonify...");
 Serial.printf("%d %d %d %d %d %d %d %d\n", coords[0][0],coords[0][1],coords[1][0],coords[1][1],coords[2][0],coords[2][1],coords[3][0],coords[3][1]);
 for(int i =0; i<8;i+=2)
  {
   int mapped = i*4/8;
   //Serial.printf("COORDS %d %d", coords[mapped][0],coords[mapped][1]);
   latticeIndice*selectedEntry1 = lattice[coords[mapped][0]][coords[mapped][1]];
   

   freqChord[mapped] = selectedEntry1->freq;
  }

  playing = true;
  return freqChord;
 
}


int correctIntonation(int key,int note) {

  // Convert played midi note to pitch above tonic 
  // ex. G in C is 8, C# in E is 10
  int distance = noteDistance(key,note);

  // Given scale degree relation from note to key, calculate appropriate ratio  
  double ratio = justRatios[distance];


  // TODO: what if we modualate? then curr frequency isnt just 12 TET
  double tweleveTetFreq = midi2Freq(note);
  double desiredFreq = ratio*currTonicFreq;
  
  // Calculate cents offset from 12TET
  // Interpretation of value is that this is amount of cents we must shift pitch to correct intonation
  double centsOffset = calculateCentsOffset(tweleveTetFreq,desiredFreq);
  //Serial.println(centsOffset);
   
  // Calculate closest possible pitch bend value to achieve that offset
  int bendAmount = calculateBend(centsOffset);
  return bendAmount;
}

// todo: probably need to handle at time of calculating bend at all 
// todo: see about speed of pitch bend 
// sort so that notes are voice lead 
//double* voiceLead(double*chord)
//{
//  // if no chord is playing, just play chord as is 
//  if(!playing)
//    return chord;
//   // else sort so that closest voice is in position of voice to shift to
//  return chord;
//  // TODO
//}

//void playChord(double* chord)
//{
//  // usbMIDI.sendNoteOn(selectedEntry1->closestMidiNote, 99, i);
//   //usbMIDI.sendPitchBend(bendAmount1,i);
//}

void swapTuning()
{
  Serial.printf("what os the tuning...%d\n",Lattice_TUNING);
  if(Lattice_TUNING == FIVE_LIMIT_JUST)
  {
    Serial.println("swapping tuning!");
    Lattice_TUNING = LA_MONTE_SEVEN;
  }
  else
    Lattice_TUNING = FIVE_LIMIT_JUST;
}

int getCurrTuning()
{
  return Lattice_TUNING;
}
