// Copyright (C) 2016 raphael.catolino@gmail.com
#include <Arduino.h>
#include <string.h>

#include "GPS.h"
#include "Sim808.h"
#include "utils.h"

int GPS::getStatus() const {
  ByteBuffer<MAX_SIZE> result;
  sim808.sendCommand("AT+CGNSPWR?", result);
  if (result.startsWith("1")) {
    return 1;
  }

  return 0;
}

int GPS::powerOn() const {
  ByteBuffer<MAX_SIZE> result;
  sim808.sendCommand("AT+CGNSPWR=1", result);
  if (result != "OK") {
    return -1;
  }

  return 0;
}

int GPS::powerOff() const {
  ByteBuffer<MAX_SIZE> result;
  sim808.sendCommand("AT+CGNSPWR=0", result);
  if (result != "OK") {
    return -1;
  }

  return 0;
}

