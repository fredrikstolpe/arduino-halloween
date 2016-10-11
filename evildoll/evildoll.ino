/*

  Evil doll
  
  Circuit:
  
  * Arduino Uno
  * Adafruit Wave Shield 1.0
  * PIR motion sensor
  * Red leds (with a few hundres ohms resistor) for eyes
  * (White leds / PIR sensor)

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
  9 (White leds or PIR sensor)
  10 Wave shield CCS
  11 Wave shield SD card
  12 Wave shield SD card
  13 Wave shield SD card
  GND Ultrasonic sensor ground
  GND LED cathode
  
*/

#include <WaveHC.h>
#include <WaveUtil.h>
#include "CBLed.h"

#define PIR_PIN 7
#define LED_PIN 6

CBLed led1 = CBLed(LED_PIN);

SdReader card;
FatVolume vol;
FatReader root;
WaveHC wave;
FatReader f;

char *farSamples[2] = { "KOMLEK1.WAV", "TYCKER1.WAV" };
char *closeSamples[10] = { "KOMLEK2.WAV", "KOMLEK3.WAV", "KOMLEK4.WAV", "REV1.WAV", "REV2.WAV", "SKRATT1.WAV", "SKRATT2.WAV", "TYCKER1.WAV", "TYCKER2.WAV", "TYCKER3.WAV"  };

void setup() {
  Serial.begin(9600);
  card.init();
  card.partialBlockRead(true);
  uint8_t part;
  for (part = 0; part < 5; part++) {
    if (vol.init(card, part)) 
      break;
  }
  root.openRoot(vol);
  pinMode(PIR_PIN, INPUT);
}

void loop() {

  bool isFar = true;
  bool isNear = false;
  bool isClose = false;
  
  if (digitalRead(PIR_PIN) == HIGH){
    isClose = true;
    isNear = false;
    isFar = false;
  }
 
  if (!wave.isplaying){
    if (isClose){
      playfile(getCloseSample());
    }
    else{
      playfile(getFarSample());
    }
  }
  
  //Serial.print(sonar.ping_cm());
  //Serial.println("cm");
  delay(50);
  if (isClose){
    led1.setIntensity(150);
    Serial.println("CLOSE ");
  }
  else {
    led1.setIntensity(10);
    Serial.println("FAR ");
  }
}

char* getCloseSample(){
  return closeSamples[random (0, 9)];
}

char* getFarSample(){
  return farSamples[random (0, 1)];
}



void playfile(char *name) {
  if (wave.isplaying) {
    wave.stop();
  }
  f.open(root, name);
  wave.create(f);
  wave.play();
}


