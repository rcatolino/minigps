// Copyright (C) 2016 raphael.catolino@gmail.com
#include <Arduino.h>
#include "NSoftwareSerial/NSoftwareSerial.h"
#include "utils.h"

void switchLed(int length) {
  digitalWrite(LED, HIGH);
  delay(length);
  digitalWrite(LED, LOW);
}

void serialPipe(SoftwareSerial &dst) {
  while (Serial.available()) {
    dst.write(Serial.read());
  }

  delay(GRACE_PERIOD);
  while (dst.available()) {
    Serial.write(dst.read());
  }
}

void failure(int code, SoftwareSerial &dst) {
  Serial.print(F("FAIL, CODE "));
  Serial.println(code);
  Serial.println("Waiting for input");
  int c = 0;
  while (true) {
    delay(100);
    c += 100;
    c = c % 5000;
    serialPipe(dst);
    if (c == 0) {
      for (int i = 0; i < code; i++) {
        switchLed(100);
        delay(100);
      }
    }
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


