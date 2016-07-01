// Copyright (C) 2016 raphael.catolino@gmail.com
#include <Arduino.h>
#include <LowPower.h>
#include <avr/power.h>
#include <limits.h>
#include "NSoftwareSerial/NSoftwareSerial.h"
#include "utils.h"

void powerSave() {
  power_spi_disable();
  power_twi_disable();
}

void sleep(const unsigned int units) {
  // units are multiples of 8s
  Serial.end();
  for (unsigned int slept = 0; slept < units && slept != UINT_MAX; slept++) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
  Serial.begin(9600);
  delay(4*GRACE_PERIOD);
}

void loSwitchPower() {
    pinMode(LO_POWER_SWITCH, OUTPUT);
    digitalWrite(LO_POWER_SWITCH, LOW);
    delay(2500);
    digitalWrite(LO_POWER_SWITCH, HIGH);
    pinMode(LO_POWER_SWITCH, INPUT);
}

void setLoSleep(int mode) {
  if (mode) {
    digitalWrite(LO_SLEEP_CTL, HIGH);
  } else {
    digitalWrite(LO_SLEEP_CTL, LOW);
  }
}

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
  Serial.println(F("Waiting for input"));
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

void nothing() {
  Serial.println(F("No answer"));
}

void printMemStats() {
  extern int __heap_start, *__brkval;
  int stack;
  int heap = __brkval == 0 ? (int) &__heap_start : (int) __brkval;

  Serial.print(F("Free ram "));
  Serial.print(heap);
  Serial.print(" ");
  Serial.println((int)&stack);
}

