/*

  Evil doll
  
  Circuit:
  
  * Arduino Uno
  * Adafruit Wave Shield 1.0
  * HC-SR04 Ultrasonic sensor
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
  7 Ultrasonic sensor trig
  8 Ultrasonic sensor echo
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
#include <NewPing.h>
#include "CBLed.h"

#define TRIGGER_PIN 7
#define ECHO_PIN 8
#define MAX_DISTANCE 400

#define LED_PIN 6

#define MAX_DISTANCE 300
const int DISTANCE_CLOSE = round(MAX_DISTANCE/3);
const int DISTANCE_NEAR = round((MAX_DISTANCE/3)*2);

CBLed led1 = CBLed(LED_PIN);

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

SdReader card;
FatVolume vol;
FatReader root;
WaveHC wave;
FatReader f;

char *farSamples[1] = { "KOMLEK1.WAV" };
char *nearSamples[5] = { "KOMLEK2.WAV", "KOMLEK3.WAV", "REV1.WAV", "TYCKER1.WAV", "TYCKER2.WAV" };
char *closeSamples[9] = { "KOMLEK3.WAV", "KOMLEK4.WAV", "REV1.WAV", "REV2.WAV", "SKRATT1.WAV", "SKRATT2.WAV", "TYCKER1.WAV", "TYCKER2.WAV", "TYCKER3.WAV"  };


void setup() {
  Serial.begin(9600);
  Serial.print("CLOSE = ");
  Serial.println(DISTANCE_CLOSE);
  Serial.print("NEAR = ");
  Serial.println(DISTANCE_NEAR);
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
  int distance = sonar.ping_cm();
  bool isClose = (distance < DISTANCE_CLOSE);
  bool isNear = (!isClose && distance < DISTANCE_NEAR);
  bool isFar = (!isClose && !isNear);
  if (!wave.isplaying){
    if (isClose){
      playfile(getCloseSample());
    }
    else if (isNear){
      playfile(getNearSample());
    }
    else{
      playfile(getFarSample());
    }
  }
  
  //Serial.print(sonar.ping_cm());
  //Serial.println("cm");
  delay(50);
  if (isClose){
    led1.setIntensity(200);
    Serial.print("CLOSE ");
    Serial.println(sonar.ping_cm());
  }
  else if (isNear){
    led1.setIntensity(70);
    Serial.print("NEAR ");
    Serial.println(sonar.ping_cm());
  }
  else {
    led1.setIntensity(30);
    Serial.print("FAR ");
    Serial.println(sonar.ping_cm());
  }
}

char* getCloseSample(){
  return closeSamples[random (0, 9)];
}

char* getNearSample(){
  return nearSamples[random (0, 5)];
}

char* getFarSample(){
  return farSamples[0];
}



void playfile(char *name) {
  if (wave.isplaying) {
    wave.stop();
  }
  f.open(root, name);
  wave.create(f);
  wave.play();
}


