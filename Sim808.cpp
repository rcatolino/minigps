// Copyright (C) 2016 raphael.catolino@gmail.com
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Sim808.h"
#include "utils.h"

void Sim808::buildCommand(const String &cmd_part) const {
  link.print(cmd_part);
}

void Sim808::getline(String &result) const {
  // A result is complete when : we run out of bytes to read, the max size is reached or an end of line is reached
  while (link.available() > 0 && result.length() < MAX_SIZE) {
    char lastchar = (char) link.read();
    if (lastchar == -1) {
      failure(2);
      break;
    } else if (lastchar == '\n' || lastchar == '\r') {
      break;
    }
    result += lastchar;
  }
}


