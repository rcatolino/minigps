// Copyright (C) 2016 raphael.catolino@gmail.com
#include <Arduino.h>
#include "ByteBuffer/ByteBuffer.h"
#include "NSoftwareSerial/NSoftwareSerial.h"
#include "Sim808.h"
#include "utils.h"

#define DEBUG 1

void Sim808::init() {
  ByteBuffer<MAX_SIZE> result;
  unsigned int count = 0;
  do {
    if (count == 3) {
      // No answer, maybe the module is off
      loSwitchPower();
      count = 0;
    }
    sendCommand("AT", result);
    Serial.print(F("AT result: "));
    Serial.println(result.c_str());
    delay(3000);
    count++;
  } while (result != "OK");

#ifdef DEBUG
  // Set more verbose error reporting
  sendCommand("AT+CMEE=2");
#endif //DEBUG

  // Activate DTR power management (pull high to enter sleep mode)
  sendCommand("AT+CSCLK=1");
}

int Sim808::waitData(int timeout) const {
  do {
    if (link.available() > 0) {
      return 0;
    }
    delay(100);
    timeout -= 100;
  } while (timeout > 0);
  return -1;
}

