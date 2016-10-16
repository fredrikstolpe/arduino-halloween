/*

  Evil doll
  
  Circuit:
  
  * Arduino Uno
  * Adafruit Wave Shield 1.0
  * PIR motion sensor
  * Red leds (with a few hundres ohms resistor) for eyes
  * (White leds)

  Pinout:

  0
  1
  2 Wave shield LCS
  3 Wave shield CLK
  4 Wave shield DI
  5 Wave shield LAT
  6 LED eyes
  7 PIR input
  8 
  9 (White leds)
  10 Wave shield CCS
  11 Wave shield SD card
  12 Wave shield SD card
  13 Wave shield SD card
  
*/

#include <WaveHC.h>
#include <WaveUtil.h>
#include "CBLed.h"
#include "CBTimer.h"
#include "CBPir.h"
//#include <Servo.h> //This can be needed for some reason when compiling

CBLed led1 = CBLed(6);
CBTimer timer1 = CBTimer();
CBPir pir1 = CBPir(7, &pirCallback);

SdReader card;
FatVolume vol;
FatReader root;
WaveHC wave;
FatReader f;

#define effectOn 0
#define effectOff 1
#define effectFadeup 2
#define effectFadedown 3
#define effectBlink 4

char *farSamples[3] = { "KOMLEK1.WAV", "KOMLEK2.WAV", "TYCKER1.WAV" };
char *closeSamples[10] = { "KOMLEK2.WAV", "KOMLEK3.WAV", "KOMLEK4.WAV", "REV1.WAV", "REV2.WAV", "SKRATT1.WAV", "SKRATT2.WAV", "TYCKER1.WAV", "TYCKER2.WAV", "TYCKER3.WAV"  };

bool animationRunning;
bool isNear;

int currentSequencePosition = 0;

//Doll sequence in calm mode
void (*calmSequence[5])(){
  eyesFadeUpSlow,
  eyesOn,
  eyesFadeDownSlow,
  eyesOff,
  saySomething
};

void setup() {
  Serial.begin(9600);
  initCard();
  sequenceAdvance();
}

//Init wave shield memory card
void initCard(){
  card.init();
  card.partialBlockRead(true);
  uint8_t part;
  for (part = 0; part < 5; part++) {
    if (vol.init(card, part)) 
      break;
  }
  root.openRoot(vol);
}

void loop() {
  led1.doWork();
  timer1.doWork();
  pir1.doWork();
}

//Movement started or stopped
void pirCallback(CBPir* pir){
  if (pir->movement){
    isNear = true;
  }
  else{
    isNear = false;
  }
}

//Play a sound file from memory card
void playfile(char *name) {
  if (wave.isplaying) {
    wave.stop();
  }
  f.open(root, name);
  wave.create(f);
  wave.play();
}

//Move one step in the sequence
void sequenceAdvance(){
  int currentPosition = currentSequencePosition;
  currentSequencePosition++;
  if (currentSequencePosition == sizeof(calmSequence)/sizeof(calmSequence[0])){
    currentSequencePosition = 0;
  }
  (*calmSequence[currentSequencePosition])();
}

void eyesFadeUpSlow(){
  Serial.println("fus");
  led1.fadeTo(20, 1, sequenceAdvance);
}

void eyesOn(){
  Serial.println("on");
  led1.setIntensity(20);
  timer1.setTimeout(3000, sequenceAdvance);
}

void eyesFadeDownSlow(){
  Serial.println("fd");
  led1.fadeTo(1, 1, sequenceAdvance);
}

void eyesOff(){
  Serial.println("off");
  led1.setIntensity(1);
  timer1.setTimeout(3000, sequenceAdvance);
}

void saySomething(){
  Serial.println("say");
  playfile(getRandomFarSample());
  sequenceAdvance();
}

char* getRandomCloseSample(){
  return closeSamples[random (0, 9)];
}

char* getRandomFarSample(){
  return farSamples[random (0, 3)];
}


