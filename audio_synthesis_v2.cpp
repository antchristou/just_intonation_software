#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "audio_synthesis_v2.h"

// GUItool: begin automatically generated code
AudioSynthWaveformModulated waveformMod1;   //xy=81.5,148
AudioSynthWaveformModulated waveformMod2;   //xy=81.5,194
AudioSynthWaveformModulated waveformMod3; //xy=94,314
AudioSynthWaveformModulated waveformMod4; //xy=94,360
AudioSynthWaveformModulated waveformMod5; //xy=106,498
AudioSynthWaveformModulated waveformMod6; //xy=106,544
AudioSynthWaveformModulated waveformMod7; //xy=114,677
AudioSynthWaveformModulated waveformMod8; //xy=114,723
AudioMixer4              mixer5;         //xy=229,77
AudioMixer4              mixer11; //xy=241.5,243
AudioMixer4              mixer9; //xy=253.5,427
AudioMixer4              mixer10;  //xy=261.5,606
AudioFilterLadder        ladder1;        //xy=332,157
AudioFilterLadder        ladder2; //xy=344.5,323
AudioFilterLadder        ladder3; //xy=356.5,507
AudioFilterLadder        ladder4; //xy=364.5,686
AudioEffectEnvelope      envelope1;      //xy=490,159
AudioEffectEnvelope      envelope2; //xy=502.5,325
AudioEffectEnvelope      envelope3; //xy=514.5,509
AudioEffectEnvelope      envelope4; //xy=522.5,688
AudioMixer4              mixer8;         //xy=710,207
AudioEffectDelay         delay1;         //xy=796,516
AudioMixer4              mixer4;         //xy=999,616
AudioMixer4              mixer3;         //xy=1010,429
AudioMixer4              mixer1;         //xy=1162,430
AudioMixer4              mixer2;         //xy=1168,621
AudioEffectFlange        flange1;        //xy=1294,433
AudioEffectFlange        flange2;        //xy=1306,610
AudioEffectFreeverb      freeverb2;      //xy=1440,662
AudioEffectFreeverb      freeverb1;      //xy=1443,456
AudioMixer4              mixer7;         //xy=1590,604
AudioMixer4              mixer6;         //xy=1592,402
AudioOutputI2S           i2s1;           //xy=1734,504
AudioConnection          patchCord1(waveformMod1, 0, mixer5, 0);
AudioConnection          patchCord2(waveformMod2, 0, mixer5, 1);
AudioConnection          patchCord3(waveformMod3, 0, mixer11, 0);
AudioConnection          patchCord4(waveformMod4, 0, mixer11, 1);
AudioConnection          patchCord5(waveformMod5, 0, mixer9, 0);
AudioConnection          patchCord6(waveformMod6, 0, mixer9, 1);
AudioConnection          patchCord7(waveformMod7, 0, mixer10, 0);
AudioConnection          patchCord8(waveformMod8, 0, mixer10, 1);
AudioConnection          patchCord9(mixer5, 0, ladder1, 0);
AudioConnection          patchCord10(mixer11, 0, ladder2, 0);
AudioConnection          patchCord11(mixer9, 0, ladder3, 0);
AudioConnection          patchCord12(mixer10, 0, ladder4, 0);
AudioConnection          patchCord13(ladder1, envelope1);
AudioConnection          patchCord14(ladder2, envelope2);
AudioConnection          patchCord15(ladder3, envelope3);
AudioConnection          patchCord16(ladder4, envelope4);
AudioConnection          patchCord17(envelope1, 0, mixer8, 0);
AudioConnection          patchCord18(envelope2, 0, mixer8, 1);
AudioConnection          patchCord19(envelope3, 0, mixer8, 2);
AudioConnection          patchCord20(envelope4, 0, mixer8, 3);
AudioConnection          patchCord21(mixer8, delay1);
AudioConnection          patchCord22(mixer8, 0, mixer2, 0);
AudioConnection          patchCord23(mixer8, 0, mixer1, 0);
AudioConnection          patchCord24(mixer8, 0, mixer7, 0);
AudioConnection          patchCord25(mixer8, 0, mixer6, 0);
AudioConnection          patchCord26(delay1, 0, mixer3, 0);
AudioConnection          patchCord27(delay1, 1, mixer3, 1);
AudioConnection          patchCord28(delay1, 2, mixer3, 2);
AudioConnection          patchCord29(delay1, 3, mixer3, 3);
AudioConnection          patchCord30(delay1, 4, mixer4, 0);
AudioConnection          patchCord31(delay1, 5, mixer4, 1);
AudioConnection          patchCord32(delay1, 6, mixer4, 2);
AudioConnection          patchCord33(delay1, 7, mixer4, 3);
AudioConnection          patchCord34(mixer4, 0, mixer2, 1);
AudioConnection          patchCord35(mixer3, 0, mixer1, 1);
AudioConnection          patchCord36(mixer1, flange1);
AudioConnection          patchCord37(mixer2, flange2);
AudioConnection          patchCord38(flange1, freeverb1);
AudioConnection          patchCord39(flange1, 0, mixer6, 1);
AudioConnection          patchCord40(flange2, freeverb2);
AudioConnection          patchCord41(flange2, 0, mixer7, 1);
AudioConnection          patchCord42(freeverb2, 0, mixer7, 2);
AudioConnection          patchCord43(freeverb1, 0, mixer6, 2);
AudioConnection          patchCord44(mixer7, 0, i2s1, 1);
AudioConnection          patchCord45(mixer6, 0, i2s1, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=1668,695
// GUItool: end automatically generated code
//

float freq[4] = {440, 440, 440, 440};
float filter_cutoff = 10.0;

elapsedMillis slideTimer;


// GUItool: end automatically generated code
float base_freq = 220;
float amp_scale = 0.5;

double currPitches[4] = {0,0,0,0}; 
const double SLIDE_TIME = 500;

// Number of samples in each delay line
#define FLANGE_DELAY_LENGTH (12*AUDIO_BLOCK_SAMPLES)
// Allocate the delay lines for left and right channels
short l_delayline[FLANGE_DELAY_LENGTH];
short r_delayline[FLANGE_DELAY_LENGTH];


int s_idx1 = FLANGE_DELAY_LENGTH/4;
int s_depth1 = FLANGE_DELAY_LENGTH/16;  // keep the depth low to minimze pitch shifting effect
double s_freq1 = .5;

int s_idx2 = FLANGE_DELAY_LENGTH/3; // some variation in L flanger index
int s_depth2 = FLANGE_DELAY_LENGTH/16; // keep the depth low to minimze pitch shifting effect
double s_freq2 = .5; // some variation in L flanger rate

void setup_audio() {
  // put your setup code here, to run once:
 
  AudioMemory(300);
  
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.307); // careful with your ears..!


  // Set up the flange effect:
  // address of delayline
  // total number of samples in the delay line
  // Index (in samples) into the delay line for the added voice
  // Depth of the flange effect
  // frequency of the flange effect
  flange1.begin(l_delayline,FLANGE_DELAY_LENGTH,s_idx1,s_depth1,s_freq1);
  flange2.begin(r_delayline,FLANGE_DELAY_LENGTH,s_idx2,s_depth2,s_freq2);

//    flange1.voices(s_idx,s_depth,.5);
//    flange2.voices(s_idx,s_depth,.7);
    envelope1.delay(0);
    envelope1.attack(400);
    envelope1.hold(0);
    envelope1.decay(100);
    envelope1.sustain(0.7);
    envelope1.release(6000);

    envelope2.delay(0);
    envelope2.attack(400);
    envelope2.hold(0);
    envelope2.decay(100);
    envelope2.sustain(0.7);
    envelope2.release(6000);

    envelope3.delay(0);
    envelope3.attack(400);
    envelope3.hold(0);
    envelope3.decay(100);
    envelope3.sustain(0.7);
    envelope3.release(6000);

         
    envelope4.delay(0);
    envelope4.attack(400);
    envelope4.hold(0);
    envelope4.decay(100);
    envelope4.sustain(0.7);
    envelope4.release(6000);
    
    mixer5.gain(0, 1.0); // Main Oscillator
    mixer5.gain(1, 0.0); // Sub Oscillator

    mixer11.gain(0, 1.0); // Main Oscillator
    mixer11.gain(1, 0.0); // Sub Oscillator

    mixer9.gain(0, 1.0); // Main Oscillator
    mixer9.gain(1, 0.0); // Sub Oscillator

    mixer10.gain(0, 1.0); // Main Oscillator
    mixer10.gain(1, 0.0); // Sub Oscillator
    
    delay1.delay(0, 20);
    delay1.delay(1, 54);
    delay1.delay(2, 84);        
    delay1.delay(3, 328);

    mixer3.gain(0, .3); 
    mixer3.gain(1, .3); 
    mixer3.gain(2, .3); 
    mixer3.gain(3, .3); 
    
    delay1.delay(4, 50);
    delay1.delay(5, 74);
    delay1.delay(6, 190);
    delay1.delay(7, 303);
    
    mixer4.gain(0, .3); 
    mixer4.gain(1, .3); 
    mixer4.gain(2, .3); 
    mixer4.gain(3, .3); 
    
    mixer1.gain(0, 0.2); //left dry level
    mixer1.gain(1, .9); //left wet level (delay)
    
    mixer2.gain(0, 0.2);//right dry level
    mixer2.gain(1, .9);//right wet level (delay)   
                        
    waveformMod1.begin(.7, 220, WAVEFORM_BANDLIMIT_SAWTOOTH);
    waveformMod2.begin(.7, 110, WAVEFORM_BANDLIMIT_SQUARE);

    waveformMod3.begin(.7, 220, WAVEFORM_BANDLIMIT_SAWTOOTH);
    waveformMod4.begin(.7, 110, WAVEFORM_BANDLIMIT_SQUARE);
   
    waveformMod5.begin(.7, 220, WAVEFORM_BANDLIMIT_SAWTOOTH);
    waveformMod6.begin(.7, 110, WAVEFORM_BANDLIMIT_SQUARE);

    waveformMod7.begin(.7, 220, WAVEFORM_BANDLIMIT_SAWTOOTH);
    waveformMod8.begin(.7, 110, WAVEFORM_BANDLIMIT_SQUARE);

    
    ladder1.frequency(freq[0]*2);
    ladder1.resonance(.96);
    ladder2.frequency(freq[1]*2);
    ladder2.resonance(.96);
    ladder3.frequency(freq[2]*2);
    ladder3.resonance(.96);
    ladder4.frequency(freq[3]*2);
    ladder4.resonance(.96);

    freeverb1.roomsize(.99);
    freeverb1.damping(.99);
    
    freeverb2.roomsize(.99);
    freeverb2.damping(.99);

    mixer6.gain(0, 0.2);
    mixer6.gain(1, 1.0);
    mixer6.gain(1, 1.0);

    mixer7.gain(0, 0.2);
    mixer7.gain(1, 0.7);
    mixer7.gain(2, 1.0);

    mixer8.gain(0, 1.0);
        
   

   // myusb.begin();
 
}

void OnNoteOn(float freqIn, int voice) {

    Serial.println("turning note on!");
 
    float newLadderFreq = freqIn*filter_cutoff;
    if(freqIn > 500 && freqIn < 900)
      newLadderFreq *= 0.4;
    else if(freqIn > 1000)
       newLadderFreq *= 0.20;
    else if(freqIn > 300)
        newLadderFreq *= 0.70;
    Serial.println(freqIn);
    if(voice==0) {
    waveformMod1.frequency(freqIn);
    waveformMod2.frequency(freqIn*0.501);// sub-oscillator down an octave, but slightly detuned
    ladder1.frequency(newLadderFreq);
    envelope1.noteOn();
    }
   
    if(voice==1) {
    waveformMod3.frequency(freqIn);
    waveformMod4.frequency(freqIn*0.501);// sub-oscillator down an octave, but slightly detuned
    ladder2.frequency(newLadderFreq);
    envelope2.noteOn();
    }

    if(voice==2) {
    waveformMod5.frequency(freqIn);
    waveformMod6.frequency(freqIn*0.501);// sub-oscillator down an octave, but slightly detuned
    ladder3.frequency(newLadderFreq);
    envelope3.noteOn();
    }

    if(voice==3) {
    waveformMod7.frequency(freqIn);
    waveformMod8.frequency(freqIn*0.501);// sub-oscillator down an octave, but slightly detuned
    ladder4.frequency(newLadderFreq);
    envelope4.noteOn();
    }
}

void turnAllOff() {
   
   envelope1.noteOff();
   envelope2.noteOff();
   envelope3.noteOff();
   envelope4.noteOff();
      
}


// ------------- old code integration


// compare function for sorting 
int compare(const void *a, const void *b) {
    double double_a = *((double*)a);
    double double_b = *((double*)b);

    if (double_a < double_b) return 1;
    if (double_a > double_b) return -1;
    return 0;
}

double* voiceLead(double*newChord, double currPitches[])
{
  // begin by sorting two chords
  qsort(currPitches, 4, sizeof(double), compare);
  qsort(newChord, 4, sizeof(double), compare);
  // determine closest new note to each existing one 

  // will contain parallel elements of new chord to old 
  double *voiceLeadChord = malloc(4*sizeof(double));

  // make copy of new chord
  double newChordCopy[4] = {-1,-1,-1,-1};
  for(int z = 0; z < 4; z++)
  {
    newChordCopy[z] = newChord[z];
  }
  
  for(int i = 0; i < 4; i++)
  {
    // find closest element to each
    float distance = 0;
    float minDistance = INFINITY;
    int minIndex = -1;
    for(int j = 0; j < 4; j++)
    {
      distance = fabs(newChordCopy[j]-currPitches[i]);
      if(distance < minDistance) 
      {
        minDistance = distance;
        minIndex = j;
      }
    }
    voiceLeadChord[i] = newChordCopy[minIndex];
    newChordCopy[minIndex] = INFINITY;
  }
  Serial.printf("curr CHORD: %.2f %.2f %.2f %.2f\n",currPitches[0],currPitches[1],currPitches[2],currPitches[3]);
  return voiceLeadChord;
}

float map_value(float value, float start1, float stop1, float start2, float stop2) {
    return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}


void slideNotes(double*voiceLeadChord)
{
  Serial.println("WE SLIDING");
  // slowly slide into each new tone
  double differences[4] = {-1,-1,-1,-1};
  double rates_of_change[4] = {-1,-1,-1,-1};
  for(int i = 0; i < 4; i++)
  {
    // calculate difference and then rate of change for each note
    differences[i] = voiceLeadChord[i]-currPitches[i]; // todo: sign?
    rates_of_change[i] = differences[i]/SLIDE_TIME;
  }
  slideTimer = 0;

  double freqTracker0;
  double freqTracker1;
  double freqTracker2;
  double freqTracker3;
  
  while(slideTimer < SLIDE_TIME)
  {

      if(slideTimer % 100 == 0)
      {
      
      freqTracker0 = map_value(slideTimer,0,SLIDE_TIME,currPitches[0],voiceLeadChord[0]);
      freqTracker1 = map_value(slideTimer,0,SLIDE_TIME,currPitches[1],voiceLeadChord[1]);
      freqTracker2 = map_value(slideTimer,0,SLIDE_TIME,currPitches[2],voiceLeadChord[2]);
      freqTracker3 = map_value(slideTimer,0,SLIDE_TIME,currPitches[3],voiceLeadChord[3]);
     

      // set each voice 
      waveformMod1.frequency(freqTracker0);
      waveformMod2.frequency(freqTracker0*0.501);// sub-oscillator down an octave, but slightly detuned
      ladder1.frequency(freqTracker0*filter_cutoff);
     
      // voice 2 
      waveformMod3.frequency(freqTracker1);
      waveformMod4.frequency(freqTracker1*0.501);// sub-oscillator down an octave, but slightly detuned
      ladder2.frequency(freqTracker1*filter_cutoff);
      // voice 3 
      waveformMod5.frequency(freqTracker2);
      waveformMod6.frequency(freqTracker2*0.501);// sub-oscillator down an octave, but slightly detuned
      ladder3.frequency(freqTracker2*filter_cutoff);
      // voice 4
      waveformMod7.frequency(freqTracker3);
      waveformMod8.frequency(freqTracker3*0.501);// sub-oscillator down an octave, but slightly detuned
      ladder4.frequency(freqTracker3*filter_cutoff);
      
     

 //     Serial.printf("SLIDE TIMER %.2f GOAL %.2f\n",freqTracker0,voiceLeadChord[0]);
      }
  }

  // set to final new freq
     waveformMod1.frequency(voiceLeadChord[0]);
      waveformMod2.frequency(voiceLeadChord[0]*0.501);// sub-oscillator down an octave, but slightly detuned
      ladder1.frequency(voiceLeadChord[0]*filter_cutoff);
     
      // voice 2 
      waveformMod3.frequency(voiceLeadChord[1]);
      waveformMod4.frequency(voiceLeadChord[1]*0.501);// sub-oscillator down an octave, but slightly detuned
      ladder2.frequency(voiceLeadChord[1]*filter_cutoff);
      // voice 3 
      waveformMod5.frequency(voiceLeadChord[2]);
      waveformMod6.frequency(voiceLeadChord[2]*0.501);// sub-oscillator down an octave, but slightly detuned
      ladder3.frequency(voiceLeadChord[2]*filter_cutoff);
      // voice 4
      waveformMod7.frequency(voiceLeadChord[3]);
      waveformMod8.frequency(voiceLeadChord[3]*0.501);// sub-oscillator down an octave, but slightly detuned
      ladder4.frequency(voiceLeadChord[3]*filter_cutoff);
      

    // set current chord
    currPitches[0] = voiceLeadChord[0];
    currPitches[1] = voiceLeadChord[1];
    currPitches[2] = voiceLeadChord[2];
    currPitches[3] = voiceLeadChord[3];

    Serial.printf("SLIDING TO: %.2f %.2f %.2f %.2f\n",voiceLeadChord[0],voiceLeadChord[1],voiceLeadChord[2],voiceLeadChord[3]);
  
}


void play_notes(bool nowPlaying,double*chord) {
  Serial.println("Playing chord!");
  qsort(chord,4,sizeof(double),compare);
  if(!nowPlaying)
  {
      Serial.printf("NOW PLAYING %.2f %.2f %.2f %.2f\n",chord[0],chord[1],chord[2],chord[3]);
    // if not playing, simply assign pitches arbitrarily
      for(int i =0; i< 4; i++)
      {
        // turn on note in each voice
        OnNoteOn(chord[i],i);
      }


    // todo: amps? 
//      waveform2.frequency(chord[1]);
//      waveform2.amplitude(amp_scale);
//    
//      waveform3.frequency(chord[2]);
//      waveform3.amplitude(amp_scale);
//    
//      waveform4.frequency(chord[3]);
//      waveform4.amplitude(amp_scale);

  currPitches[0] = chord[0];
  currPitches[1] = chord[1];
  currPitches[2] = chord[2];
  currPitches[3] = chord[3];

// waveform1.begin(WAVEFORM_SINE);
//  waveform2.begin(WAVEFORM_SINE);
//  waveform3.begin(WAVEFORM_SINE);
//  waveform4.begin(WAVEFORM_SINE);
  }
  else
  {

     Serial.printf("Going to play: %.2f %.2f %.2f %.2f\n",chord[0],chord[1],chord[2],chord[3]);
    // returns closest pitches to each current note
    double*voiceLeadChord = voiceLead(chord,currPitches);
    slideNotes(voiceLeadChord);
  }
 
}
