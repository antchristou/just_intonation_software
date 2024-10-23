# NeoPixel Ring simple sketch (c) 2013 Shae Erisson
# Released under the GPLv3 license to match the rest of the
# Adafruit NeoPixel library

#include "just_intonate.h"
#include <stdlib.h>
#include <math.h>
#include <Arduino.h>


#todo: possibly hard code self less badly...
 LATTICE_X_DIM = 20
 LATTICE_Y_DIM = 30
Lattice_TUNING = FIVE_LIMIT_JUST
 FREQ_UPPER_BOUND = 1500
 FREQ_LOWER_BOUND = 100
GEN_COUNT = 0

typedef struct  int closestMidiNote
  double freq
  
} latticeIndice

latticeIndice*lattice[LATTICE_X_DIM][LATTICE_Y_DIM]

key = 9; # 0 corresponds to C as tonic, as Db, as A etc 
currTonicFreq = 440
#tuning_system = FIVE_LIMIT_JUST

# ratios for calculating frequencies from base 
double justRatios[] = {1.0/1,17.0/16, 9.0/8,19.0/16, 5.0/4, 21.0/16, 11.0/8,3.0/2,13.0/8,27.0/16,7.0/4,15.0/8

# base tuning is by default A=440
baseTuning = 440.0

double*chord


#contains pure frequencies 
double*freqChord

# array that contains indices corresponding shape of notes traced out 
int latticeShape[8]
indicesCollected = 0

# True if notes are currently being played 
playing = False

def setupChordArray(self):
  chord = (double*)malloc(4 * 2 * sizeof(double))
  freqChord = (double*)malloc(4 * sizeof(double))


def midi2Freq(self, midiNote):
  A4MIDI = 69
  # calculate semitones from A4 
  offsetFromA = midiNote-A4MIDI

  # convert to frequency 

  freq = pow(2,(offsetFromA/12.0))*baseTuning
  return freq


# used to determine scale degree given some tonic and some note
# ex. D in C is 2, F# in F is 1, etc (0 indexed)
def noteDistance(self, tonic, midiNote):
  midiNote = midiNote % 12
  
  distance = midiNote-tonic

  # wrap around if negative 
  if distance < 0:
    distance += 12; 
  
  return distance
  


def printLattice(self):
  Serial.printf("Curr tuning is %d\n",Lattice_TUNING)
  for(i = 0; i < LATTICE_X_DIM; i++)
    for(y = 0; y < LATTICE_Y_DIM; y++)
      latticeIndice*entry = lattice[i][y]
       Serial.print(entry.freq)
      Serial.print(" ")

    Serial.println()




# given some freq in hz, closest midi for playing later 
def calcClosestMidi(self, freq): 
    double midiNoteFrequencies[128]
       
    # Calculate MIDI note frequencies
    for (i = 0; i < 128; i++)        midiNoteFrequencies[i] = midi2Freq(i)


    # Find the closest MIDI note
    closestNote = 0
    minDifference = fabs(freq - midiNoteFrequencies[0])

    for (i = 1; i < 128; i++)        difference = fabs(freq - midiNoteFrequencies[i])
        if difference < minDifference:            minDifference = difference
            closestNote = i



    return closestNote


# used to ensure frequencies in matrix don't get too high or low 
def wrapFreq(self, freq):
  while(freq > FREQ_UPPER_BOUND)
    freq = freq/2.0
  while(freq < FREQ_LOWER_BOUND)
    freq = freq*2.0
  return freq


def generateLattice(self):

  Serial.print("GENERATING \n")
  # place A4 in middle 
  middleXIndice = LATTICE_X_DIM/2
  middleYIndice = LATTICE_Y_DIM/2

 # Serial.printf("MIDDLE %d %d \n", middleXIndice,middleYIndice)
  latticeIndice*middleEntry = (latticeIndice*) malloc(sizeof(latticeIndice))
  middleEntry.closestMidiNote = 69
  middleEntry.freq = baseTuning
  lattice[middleXIndice][middleYIndice] = middleEntry
  

  columnsFilled = 0
  currColumn = middleYIndice
  currCenter = middleXIndice
  currCenterFreq = baseTuning

  double verticalRatio=1
  double horizontalRatio=1
  if Lattice_TUNING == FIVE_LIMIT_JUST:
    verticalRatio=3.0/2
    horizontalRatio=5.0/4
    Serial.println("MAKING LATTICE IN 5 LIMIT JUST MODE")

  elif Lattice_TUNING == LA_MONTE_SEVEN:
    verticalRatio=3.0/2
    horizontalRatio=7.0/4
    Serial.println("MAKING LATTICE IN LA MONTE MODE")

  
  
  # if tuning sytem is 5 limit just intonation, axis is thirds, is fifths
  # in La monte young seven limit, axis is sevenths, is fifths
    while(columnsFilled < LATTICE_Y_DIM)
      
      # fill in from middle to end
      for(i = 0; i < LATTICE_X_DIM; i++)
        # calculate how many fifths apart we are 
        powerVal = currCenter-i; 
        freq = pow(verticalRatio, powerVal)*currCenterFreq
        # keep within range of 440-880 hz
        freq = wrapFreq(freq)
        closestMidi = calcClosestMidi(freq); 
        latticeIndice*entry = (latticeIndice*) malloc(sizeof(latticeIndice))
        entry.closestMidiNote = closestMidi
        entry.freq = freq
        lattice[i][currColumn] = entry


      currColumn += 1
      columnsFilled += 1


      # going over to next column, move by a just major third / 
      currCenterFreq = horizontalRatio*currCenterFreq
      # wrap around when reach end
      if currColumn >= LATTICE_Y_DIM:
        currColumn = 0
        currCenterFreq = pow(horizontalRatio,-1*middleYIndice)*baseTuning

    

  # TODO get rid of 
  GEN_COUNT += 1
  #printLattice()
 


# in self case, and equally tempered 
def calculateCentsOffset(self, freq1, freq2):
  # c = 1200 × log2 (f2 / f1)
   
  # calculate log base 2
  log2 = log((freq2/freq1)) / log(2)

  centsOffset = 1200*log2
  return centsOffset



# calculate MPE pitch bend value to send to achieve cents offset
# MPE ranges from -8192 to 8191 meaning that 
# the perfectly exact value can't be achieved but can get very close
# ASSUMPTION: on synth end, pitch bend value is set to one whole tone 
def calculateBend(self, centsOffset):
  # assuming that pitch band max is 1 whole step (200 cents) 
  centIncrement = 2400.0/8191.0
  # calculate how many pitch bend increments approximate difference in cents
  doubleBend = centsOffset/centIncrement
  # round to nearest integet as MPE only accepts integers 
  pitchBendValue = round(doubleBend)
 # Serial.println(pitchBendValue)

  return pitchBendValue
  


# calculate mpe pitch bend to play microtuned note
def calcBendAmount(self, note):
  desiredFreq = note.freq
  closestMidi = note.closestMidiNote
  tweleveTetFreq = midi2Freq(closestMidi)
  
  centsOffset = calculateCentsOffset(tweleveTetFreq,desiredFreq)
   
  # Calculate closest possible pitch bend value to achieve that offset
  bendAmount = calculateBend(centsOffset)
  return bendAmount




def setupLattice(self):  generateLattice()
  setupChordArray()
 # Serial.println("GENERATED Lattice")
  printLattice()


double*sonify(int coords[4][2])
 for(int i =0; i<8;i+=2)
   mapped = i*4/8
   #Serial.printf("COORDS %d %d", coords[mapped][0],coords[mapped][1])
   latticeIndice*selectedEntry1 = lattice[coords[mapped][0]][coords[mapped][1]]
   bendAmount1 = calcBendAmount(selectedEntry1)
   
   Serial.printf("CURR CLOSEST MIDI+DESIRED FREQ %d %.2f\n", selectedEntry1.closestMidiNote,selectedEntry1.freq)
#   Serial.printf("MIDI FREQ IN 12 TET %.2f \n", midi2Freq(selectedEntry1.closestMidiNote))
   Serial.printf("BEND AMOUNT %d\n", bendAmount1)
#
   Serial.println("SENDING MIDI")
    chord[i] = selectedEntry1.closestMidiNote
    chord[i+1] = bendAmount1


  playing = True
  
  return chord
 


double*sonifyFreqMode(int coords[4][2])
 
 Serial.println("trying to sonify...")
 Serial.printf("%d %d %d %d %d %d %d %d\n", coords[0][0],coords[0][1],coords[1][0],coords[1][1],coords[2][0],coords[2][1],coords[3][0],coords[3][1])
 for(int i =0; i<8;i+=2)
   mapped = i*4/8
   #Serial.printf("COORDS %d %d", coords[mapped][0],coords[mapped][1])
   latticeIndice*selectedEntry1 = lattice[coords[mapped][0]][coords[mapped][1]]
   

   freqChord[mapped] = selectedEntry1.freq


  playing = True
  return freqChord
 



def correctIntonation(self, key, note):
  # Convert played midi note to pitch above tonic 
  # ex. G in C is 8, C# in E is 10
  distance = noteDistance(key,note)

  # Given scale degree relation from note to key, appropriate ratio  
  ratio = justRatios[distance]

  tweleveTetFreq = midi2Freq(note)
  desiredFreq = ratio*currTonicFreq
  
  # Calculate cents offset from 12TET
  # Interpretation of value is that self is amount of cents we must shift pitch to correct intonation
  centsOffset = calculateCentsOffset(tweleveTetFreq,desiredFreq)
  #Serial.println(centsOffset)
   
  # Calculate closest possible pitch bend value to achieve that offset
  bendAmount = calculateBend(centsOffset)
  return bendAmount


def swapTuning(self):
  Serial.printf("what os the tuning...%d\n",Lattice_TUNING)
  if Lattice_TUNING == FIVE_LIMIT_JUST:
    Serial.println("swapping tuningnot ")
    Lattice_TUNING = LA_MONTE_SEVEN

  else:
    Lattice_TUNING = FIVE_LIMIT_JUST


def getCurrTuning(self):
  return Lattice_TUNING

