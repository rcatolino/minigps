#include "ByteBuffer.h"
#line 2 "bbtest.ino"
#include <Arduino.h>
#include <ArduinoUnit.h>


void setup() {
  Serial.begin(9600);
}

void loop() {
  Test::run();
}
