// Copyright (C) 2016 raphael.catolino@gmail.com
#ifndef utils_H
#define utils_H

#include <Arduino.h>

// Communication PINs
#define LED 13    // LED pin
#define TX_A0 14  // Software serial transmit pin
#define RX_A1 15  // Software serial receive pin
#define LO_POWER_SWITCH 16 // Power switch pin
#define LO_SLEEP_CTL 17 // Sleep control pin
#define LO_INT 2 // Message received interrupt line

// Control constants
#define MAX_SIZE 160    // Max serial line size
#define GRACE_PERIOD 200
#define MAX_FIX_ATTEMPT 5 // Number of attempt before giving up on GNSS
#define TIME_POWD 20      // Time to wait in power down state (multiple of 8s)
#define TIME_ACTIVE 10    // Time to wait in sleep state (multiple of 8s)
#define SERIAL_TIMEOUT 10 // Time to wait an answer of the lonet module (multiple of GRACE_PERIOD)
#define SIM_PIN "1234"

void powerSave();
void sleep(const unsigned int units);
void loSwitchPower();
void setLoSleep(int mode);
void switchLed(int length);
void serialPipe(SoftwareSerial &dst);
void failure(int code, SoftwareSerial &dst);
void error(const String& result);
void ok(const String& result);
void nothing();

#endif // utils_H
