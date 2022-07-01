#include "Arduino.h"
#include "chronos.h"

Chronos chrono1;
Chronos chrono2;

uint8_t value=0;

void onceElapsedTimeDo(uint8_t *val) {
  digitalWrite(D13,*val);
  chrono2.start();
  *val=0;
}

void thenDoThatAfter(uint8_t *val) {
  digitalWrite(D13,*val);
}

void setup() {
  pinMode(D13,OUTPUT);
  digitalWrite(D13,1);
  delay(500);
  digitalWrite(D13,value);
  chrono1.attachInterrupt(4000,std::bind(onceElapsedTimeDo, &value));
  chrono2.attachInterrupt(2000,std::bind(thenDoThatAfter, &value));
  chrono1.start();
  delay(2000);
  value=1;
}

void loop() {}