#include "chronos.h"
#include <Bistable.h>

Chronos chronotemps;

Bistable Bouton_bleue;

void chronoCallbacktemps(void) {
  chronotemps.reset();

}

void setup() {
  Serial.begin(115200);
  Bouton_bleue.begin(USER_BTN,Bistable::FALLING_EDGE, INPUT_PULLUP, LOW,50);
  chronotemps.attachInterrupt(1000,chronoCallbacktemps);
  chronotemps.start(true);
}

void loop() {
  if (Bouton_bleue.isChanged()) {
    if (Bouton_bleue.getState()) {
      chronotemps.start(false);
    } else {
      chronotemps.pause();
    }
  }
  Serial.print("chrono:");
  Serial.print(chronotemps.isRunning(),DEC);
  Serial.print(",");
  Serial.println(chronotemps.getElapsedTime(),DEC);
}