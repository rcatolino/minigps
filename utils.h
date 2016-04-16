// Copyright (C) 2016 raphael.catolino@gmail.com
#ifndef utils_H
#define utils_H

#include <Arduino.h>

#define LED 13
#define LO_POW_CTL 17
#define RX_A0 14
#define TX_A1 15
#define LO_INT 2
#define MAX_SIZE 160
#define GRACE_PERIOD 200

void setLoSleep(int mode);
void switchLed(int length);
void serialPipe(SoftwareSerial &dst);
void failure(int code, SoftwareSerial &dst);
void error(const String& result);
void ok(const String& result);
void nothing();

#endif // utils_H
