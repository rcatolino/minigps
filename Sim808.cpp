// Copyright (C) 2016 raphael.catolino@gmail.com
#include <Arduino.h>
#include "NSoftwareSerial/NSoftwareSerial.h"
#include "Sim808.h"
#include "utils.h"

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

void Sim808::buildCommand(const String &cmd_part) const {
  link.print(cmd_part);
}

int Sim808::getline(String &result) const {
  int read = link.available() ? 0 : -1;
  // A result is complete when : we run out of bytes to read, the max size is reached or an end of line is reached
  while (link.available() > 0 && result.length() < MAX_SIZE) {
    char lastchar = (char) link.read();
    if (lastchar == -1) {
      failure(2, link);
      break;
    } else if (lastchar == '\n' || lastchar == '\r') {
      break;
    }
    result += lastchar;
    read++;
  }

  return read;
}


