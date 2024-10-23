##include <Audio.h>
##include <Wire.h>
##include <SPI.h>
##include <SD.h>
##include <SerialFlash.h>
#
## GUItool: begin automatically generated code
#AudioSynthWaveform       waveform4;      #xy=119,409
#AudioSynthWaveform       waveform3;      #xy=122,335
#AudioSynthWaveform       waveform1;      #xy=130,78
#AudioSynthWaveform       waveform2;      #xy=136,281
#AudioMixer4              mixer1;         #xy=467,112
#AudioFilterStateVariable filter1;        #xy=561,205
#AudioEffectFreeverb      freeverb1;      #xy=696,181
#AudioOutputUSB           usb1;           #xy=823,60
#AudioConnection          patchCord1(waveform4, 0, mixer1, 3)
#AudioConnection          patchCord2(waveform3, 0, mixer1, 2)
#AudioConnection          patchCord3(waveform1, 0, mixer1, 0)
#AudioConnection          patchCord4(waveform2, 0, mixer1, 1)
#AudioConnection          patchCord5(mixer1, 0, filter1, 0)
#AudioConnection          patchCord6(mixer1, 0, filter1, 1)
#AudioConnection          patchCord7(filter1, 0, freeverb1, 0)
#AudioConnection          patchCord8(freeverb1, 0, usb1, 0)
#AudioConnection          patchCord9(freeverb1, 0, usb1, 1)
#AudioControlSGTL5000     sgtl5000_1;     #xy=584,339
## GUItool: end automatically generated code
#
#
#
##include <elapsedMillis.h>
##include "audio_synthesis.h"
#
#elapsedMillis slideTimer
#
## GUItool: end automatically generated code
#
#
## GUItool: end automatically generated code
#base_freq = 220
#amp_scale = 0.5
#
#double currPitches[4] = {0,0,0,0}; 
#
# SLIDE_TIME = 200
#void setup_audio()#  Serial.println("Setting up audio")
#  # put your setup code here, run once:
# AudioMemory(20)
#  sgtl5000_1.enable()
#  sgtl5000_1.volume(0.6)
#  
# # freeverb1.roomsize(1.0)
# # freeverb1.damping(1.0)
#
#  filter1.frequency(1000)
##
##   waveform1.frequency(440)
##   waveform1.amplitude(1.0)
#
# #  mixer1.gain(0,0.8)
##   
#  waveform1.begin(WAVEFORM_SINE)
#    waveform2.begin(WAVEFORM_SINE)
#      waveform3.begin(WAVEFORM_SINE)
#        waveform4.begin(WAVEFORM_SINE)
#
#
#
## compare function for sorting 
#int compare( void *a, *b)#    double_a = *((double*)a)
#    double_b = *((double*)b)
#
#    if (double_a < double_b) return 1
#    if (double_a > double_b) return -1
#    return 0
#
#
#double* voiceLead(double*newChord, currPitches[])
##  # begin by sorting two chords
#  qsort(currPitches, 4, sizeof(double), compare)
#  qsort(newChord, 4, sizeof(double), compare)
#  # determine closest note to each existing one 
#
#  # will contain parallel elements of chord to old 
#  double *voiceLeadChord = malloc(4*sizeof(double))
#
#  # make copy of chord
#  double newChordCopy[4] = {-1,-1,-1,-1
#  for(z = 0; z < 4; z++)
##    newChordCopy[z] = newChord[z]
#
#  
#  for(i = 0; i < 4; i++)
##    # find closest element to each
#    distance = 0
#    minDistance = INFINITY
#    minIndex = -1
#    for(j = 0; j < 4; j++)
##      distance = fabs(newChordCopy[j]-currPitches[i])
#      if(distance < minDistance) 
##        minDistance = distance
#        minIndex = j
#
#
#    voiceLeadChord[i] = newChordCopy[minIndex]
#    newChordCopy[minIndex] = INFINITY
#
#  Serial.printf("curr CHORD: %.2f %.2f %.2f %.2f\n",currPitches[0],currPitches[1],currPitches[2],currPitches[3])
#  return voiceLeadChord
#
#
#float map_value(float value, start1, stop1, start2, stop2)#    return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1))
#
#
#
#void slideNotes(double*voiceLeadChord)
##  Serial.println("WE SLIDING")
#  # slowly slide into each tone
#  double differences[4] = {-1,-1,-1,-1
#  double rates_of_change[4] = {-1,-1,-1,-1
#  for(i = 0; i < 4; i++)
##    # calculate difference and then rate of change for each note
#    differences[i] = voiceLeadChord[i]-currPitches[i]; 
#    rates_of_change[i] = differences[i]/SLIDE_TIME
#
#  slideTimer = 0
#
#  double freqTracker0
#  double freqTracker1
#  double freqTracker2
#  double freqTracker3
#  
#  while(slideTimer < SLIDE_TIME)
##
#      if slideTimer % 100 == 0:
##
#      freqTracker0 = map_value(slideTimer,0,SLIDE_TIME,currPitches[0],voiceLeadChord[0])
#      freqTracker1 = map_value(slideTimer,0,SLIDE_TIME,currPitches[1],voiceLeadChord[1])
#      freqTracker2 = map_value(slideTimer,0,SLIDE_TIME,currPitches[2],voiceLeadChord[2])
#      freqTracker3 = map_value(slideTimer,0,SLIDE_TIME,currPitches[3],voiceLeadChord[3])
#     
#      
#      waveform1.frequency(freqTracker0)
#   
#      waveform2.frequency(freqTracker1)
#
#      waveform3.frequency(freqTracker2)
#      
#      waveform4.frequency(freqTracker3)
#
# #     Serial.printf("SLIDE TIMER %.2f GOAL %.2f\n",freqTracker0,voiceLeadChord[0])
#
#
#
#  # set to final freq
#    waveform1.frequency(voiceLeadChord[0])
#   
#    waveform2.frequency(voiceLeadChord[1])
#
#    waveform3.frequency(voiceLeadChord[2])
#      
#    waveform4.frequency(voiceLeadChord[3])
#
#    # set current chord
#    currPitches[0] = voiceLeadChord[0]
#    currPitches[1] = voiceLeadChord[1]
#    currPitches[2] = voiceLeadChord[2]
#    currPitches[3] = voiceLeadChord[3]
#
#    Serial.printf("SLIDING TO: %.2f %.2f %.2f %.2f\n",voiceLeadChord[0],voiceLeadChord[1],voiceLeadChord[2],voiceLeadChord[3])
#  
#
#
#
#void play_notes(bool nowPlaying,double*chord)#  Serial.println("Playing chordnot ")
#  qsort(chord,4,sizeof(double),compare)
#  if not nowPlaying:
##      Serial.printf("NOW PLAYING %.2f %.2f %.2f %.2f\n",chord[0],chord[1],chord[2],chord[3])
#    # if not playing, assign pitches arbitrarily
#      waveform1.frequency(chord[0])
#      waveform1.amplitude(amp_scale)
#    
#      waveform2.frequency(chord[1])
#      waveform2.amplitude(amp_scale)
#    
#      waveform3.frequency(chord[2])
#      waveform3.amplitude(amp_scale)
#    
#      waveform4.frequency(chord[3])
#      waveform4.amplitude(amp_scale)
#
#  currPitches[0] = chord[0]
#  currPitches[1] = chord[1]
#  currPitches[2] = chord[2]
#  currPitches[3] = chord[3]
#
## waveform1.begin(WAVEFORM_SINE)
##  waveform2.begin(WAVEFORM_SINE)
##  waveform3.begin(WAVEFORM_SINE)
##  waveform4.begin(WAVEFORM_SINE)
#
#  else:
##
#     Serial.printf("Going to play: %.2f %.2f %.2f %.2f\n",chord[0],chord[1],chord[2],chord[3])
#    # returns closest pitches to each current note
#    double*voiceLeadChord = voiceLead(chord,currPitches)
#    slideNotes(voiceLeadChord)
#
# 
#
