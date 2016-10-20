#include <Servo.h>
#include "CBLed.h"
#include "CBTimer.h"

CBLed led1 = CBLed(6);
CBLed led2 = CBLed(9);
CBTimer timer1 = CBTimer();
CBTimer timer2 = CBTimer();
int sequencePosition1;
int sequencePosition2;

void (*sequence1[4])(){
  led1FadeUp,
  led1On,
  led1FadeDown,
  led1Off
};

void (*sequence2[2])(){
  led2Off,
  led2Blink
};

void setup() {
  Serial.begin(9600);
  doLed1Animation();
  doLed2Animation();
}

void loop() {
  led1.doWork();
  led2.doWork();
  timer1.doWork();
  timer2.doWork();
}

void doLed1Animation(){
  sequencePosition1++;
  if (sequencePosition1 == sizeof(sequence1)/sizeof(sequence1[0])){
    sequencePosition1 = 0;
  }
  (*sequence1[sequencePosition1])();
}

void doLed2Animation(){
  sequencePosition2++;
  if (sequencePosition2 == sizeof(sequence2)/sizeof(sequence2[0])){
    sequencePosition2 = 0;
  }
  (*sequence2[sequencePosition2])();
}

void led1FadeUp(){
  led1.fadeTo(255, 1, doLed1Animation);
}

void led1FadeDown(){
  led1.fadeTo(0, 1, doLed1Animation);
}

void led1On(){
  led1.on();
  timer1.setTimeout(2000, doLed1Animation);
}

void led1Off(){
  led1.off();
  timer1.setTimeout(2000, doLed1Animation);
}

void led2Off(){
  led2.off();
  timer2.setTimeout(2000, doLed2Animation);
}

void led2Blink(){
  led2.blink(100);
  timer2.setTimeout(1000, doLed2Animation);
}


