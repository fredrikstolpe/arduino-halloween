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

//Todo: !movement never occurs because of timer - fix

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

char *calmSamples[3] = { "KOMLEK1.WAV", "KOMLEK2.WAV", "TYCKER1.WAV" };
char *upsetSamples[10] = { "SKRATT1.WAV", "KOMLEK3.WAV", "KOMLEK4.WAV", "REV1.WAV", "REV2.WAV", "SKRATT1.WAV", "SKRATT2.WAV", "REV2.WAV", "TYCKER2.WAV", "TYCKER3.WAV"  };

bool movement;
bool isUpset;
bool stateChanged;
unsigned long lastMovementTime;

int currentSequencePosition = -1;

//Doll sequence in calm mode
void (*calmSequence[5])(){
  calmEyesFadeUp,
  calmEyesLit,
  calmEyesFadeDown,
  calmEyesLow,
  calmSpeak
};

void (*upsetSequence[6])(){
  upsetEyesOff,
  upsetSpeak,
  upsetEyesFadeUp,
  upsetEyesLit,
  upsetEyesBlink,
  upsetEyesFadeDown
};

void setup() {
  Serial.begin(9600);
  initCard();
  doAnimation();
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

//Play a sound file from memory card
void playfile(char *name) {
  if (wave.isplaying) {
    return;
    //wave.stop();
  }
  Serial.print("Playing ");
  Serial.println(name);
  f.open(root, name);
  wave.create(f);
  wave.play();
}

char* getRandomUpsetSample(){
  return upsetSamples[random (0, 9)];
}

char* getRandomCalmSample(){
  return calmSamples[random (0, 3)];
}

void loop() {
  led1.doWork();
  timer1.doWork();
  pir1.doWork();
}

//PIR sensor callback - movement started or stopped
void pirCallback(CBPir* pir){
  if (pir->movement){
    lastMovementTime = millis();
  }
  if (!stateChanged){
    if (pir->movement){
      Serial.println("PIR - movement");
      movement = true;
      if (!isUpset){
        Serial.println("State changed");
        stateChanged = true;
      }
    }
    else{
      Serial.println("PIR - no movement");
      movement = false;
      if (timeElapsed(lastMovementTime, 10000)){
        Serial.println("Time had elapsed");
        if (isUpset){
          Serial.println("State changed");
          stateChanged = true;
        }
      }else{
        Serial.println("Time had not elapsed");
      }
    }
  }
}

bool timeElapsed(unsigned long since, int amount){
  return ((millis() - since) > amount);
}

//Move one step in the sequence
void doAnimation(){
  if (stateChanged){
    Serial.println("Do animation state changed");
    isUpset = !isUpset;
    Serial.print("Is upset: ");
    Serial.println(isUpset);
    currentSequencePosition = -1;
    stateChanged = false;
    if (isUpset){
      wave.stop();
    }
  }
  currentSequencePosition++;
  if (isUpset){
    if (currentSequencePosition == sizeof(upsetSequence)/sizeof(upsetSequence[0])){
      currentSequencePosition = 0;
    }
    (*upsetSequence[currentSequencePosition])();
  }
  else{
    if (currentSequencePosition == sizeof(calmSequence)/sizeof(calmSequence[0])){
      currentSequencePosition = 0;
    }
    (*calmSequence[currentSequencePosition])();
  }
}

void calmEyesFadeUp(){
  led1.fadeTo(20, 1, doAnimation);
}

void calmEyesLit(){
  led1.setIntensity(20);
  timer1.setTimeout(3000, doAnimation);
}

void calmEyesFadeDown(){
  led1.fadeTo(1, 1, doAnimation);
}

void calmEyesLow(){
  led1.setIntensity(1);
  timer1.setTimeout(3000, doAnimation);
}

void calmSpeak(){
  playfile(getRandomCalmSample());
  doAnimation();
}

//UPSET

void upsetEyesOff(){
  playfile(getRandomUpsetSample());
  led1.setIntensity(0);
  timer1.setTimeout(500, doAnimation);
}

void upsetSpeak(){
  playfile(getRandomUpsetSample());
  doAnimation();
}

void upsetEyesFadeUp(){
  playfile(getRandomUpsetSample());
  led1.fadeTo(120, 2, doAnimation);
}

void upsetEyesLit(){
  playfile(getRandomUpsetSample());
  led1.setIntensity(0);
  timer1.setTimeout(500, doAnimation);
}

void upsetEyesBlink(){
  playfile(getRandomUpsetSample());
  led1.blink(100);
  timer1.setTimeout(2000, doAnimation);
}

void upsetEyesFadeDown(){
  playfile(getRandomUpsetSample());
  led1.setIntensity(150);
  led1.fadeTo(1, 5, doAnimation);
}




