// Copyright (C) 2016 raphael.catolino@gmail.com
#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include "Sim808.h"

// The GPS module draws between 25 and 35 mA.
// DTR sleep mode does not turn off the GPS module, it has to be turned off manually
// using AT+CGNSPWR=0

class GPS {
  public:
    GPS(const Sim808 &module) : sim808(module) {}
    int init() const;
    int getStatus() const;
    int powerOn() const;
    int powerOff() const;
    int getData(String &data) const;

  private:
    const Sim808 &sim808;
};

#endif // GPS_H
