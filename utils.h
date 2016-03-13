// Copyright (C) 2016 raphael.catolino@gmail.com
#ifndef utils_H
#define utils_H

#include <Arduino.h>

#define ledPin 13
#define loSwitch 17
#define RX_A0 14
#define TX_A1 15
#define MAX_SIZE 160
#define GRACE_PERIOD 200

void switchLed(int length);
void failure(int code);
void error(const String& result);
void ok(const String& result);
void nothing();

#endif // utils_H
