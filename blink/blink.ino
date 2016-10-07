#include <Servo.h>
#include "CBLed.h"
#include "CBTimer.h"
#include "animation.cpp"

#define effectOn 0
#define effectOff 1
#define effectFadeup 2
#define effectFadedown 3
#define effectBlink 4

CBLed led1 = CBLed(6);
CBLed led2 = CBLed(9);
CBTimer led1Timer = CBTimer();
CBTimer led2Timer = CBTimer();

Animation led1sequence [5] = {
   Animation(0, 5, effectFadeup),
   Animation(0, 1, effectFadedown),
   Animation(2000, 0, effectOff),
   Animation(2000, 200, effectBlink),
   Animation(2000, 0, effectOff)
};

int led1CurrentPosition;

void setup() {
  Serial.begin(9600); 
//  led1.blink(1000);
//  led2.fadeTo(255, 1, &fadeUpCallback);
}

void loop() {
  if (!led1.isFading && !led1.isBlinking && !led1Timer.isRunning){
    led1Advance();
  }
  led1.doWork();
  led2.doWork();
  led1Timer.doWork();
}

void led1Advance(){
  Animation a = led1sequence[led1CurrentPosition];
  if (a.effect == effectOn){
    led1.on();
    led1Timer.setTimeout(a.duration, &led1Callback);
  }
  else if (a.effect == effectOff){
    led1.off();
    led1Timer.setTimeout(a.duration, &led1Callback);
  }
  else if (a.effect == effectFadeup){
    led1.fadeTo(255, a.speedOrDelay, &led1Callback);
  }
  else if (a.effect == effectFadedown){
    led1.fadeTo(0, a.speedOrDelay, &led1Callback);
  }
  else if (a.effect == effectBlink){
    led1.blink(a.speedOrDelay);
    led1Timer.setTimeout(a.duration, &led1Callback);
  }
  led1CurrentPosition++;
  if (led1CurrentPosition == sizeof(led1sequence)/sizeof(led1sequence[0])){
    led1CurrentPosition = 0;
  }
}

void led1Callback(){
  led1Advance();
}

void fadeUpCallback(){
  led2.fadeTo(0, 5, &fadeDownCallback);
}

void fadeDownCallback(){
  led1Timer.setTimeout(2000, &timerCallback);
}

void timerCallback(){
  led2.blink(200);
  led1Timer.setTimeout(2000, &blinkCallback);
}

void blinkCallback(){
  led2.fadeTo(255, 5, &fadeUpCallback);
}
