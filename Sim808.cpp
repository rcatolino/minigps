// Copyright (C) 2016 raphael.catolino@gmail.com
#include <Arduino.h>
#include "ByteBuffer/ByteBuffer.h"
#include "NSoftwareSerial/NSoftwareSerial.h"
#include "Sim808.h"
#include "utils.h"

#define DEBUG 1

void Sim808::init() {
  String results[] = {String()};
  results[0].reserve(MAX_SIZE);
  unsigned int count = 0;
  do {
    if (count == 3) {
      // No answer, maybe the module is off
      loSwitchPower();
      count = 0;
    }
    sendCommand(F("AT"), results);
    delay(3000);
    count++;
  } while (results[0] != "OK");

#ifdef DEBUG
  // Set more verbose error reporting
  sendCommand(F("AT+CMEE=2"), results);
#endif //DEBUG

  // Activate DTR power management (pull high to enter sleep mode)
  sendCommand(F("AT+CSCLK=1"), results);
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

void Sim808::buildCommand(const String& cmd_part) const {
  link.print(cmd_part);
}

int Sim808::getline(String& result) const {
  int read = link.available();
  if (read == -1) {
    read = -2;
  } else if (read == 0) {
    read = -1;
  } else {
    read = 0;
  }
  result.remove(0);
  // A result is complete when : we run out of bytes to read, the max size is reached or an end of line is reached
  while (link.available() > 0 && result.length() < MAX_SIZE) {
    char lastchar = (char) link.read();
    if (lastchar == -1) {
      Serial.println(F("lonet serial interrupted"));
      break;
    } else if (lastchar == '\n' || lastchar == '\r') {
      break;
    }
    result += lastchar;
    read++;
  }

  return read;
}


