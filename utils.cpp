// Copyright (C) 2016 raphael.catolino@gmail.com
#include <Arduino.h>
#include "utils.h"

void switchLed(int length) {
  digitalWrite(LED, HIGH);
  delay(length);
  digitalWrite(LED, LOW);
}

void failure(int code) {
  Serial.print(F("FAIL, CODE "));
  Serial.println(code);
  while (true) {
    switchLed(2000);
    delay(1000);
    for (int i = 0; i < code; i++) {
      switchLed(200);
    }
    delay(1000);
  }
}

void error(const String& result) {
  Serial.print(F("ERROR : "));
  Serial.println(result);
}

void ok(const String& result) {
  Serial.print(F("Command result : "));
  Serial.println(result);
}

void nothing() {
  Serial.println(F("No answer"));
}


