// Copyright (C) 2016 raphael.catolino@gmail.com
#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include "Sim808.h"

class GPS {
  public:
    GPS(const Sim808 &module) : sim808(module) {}
    int init() const;
    int getStatus() const;
    int setStatus(int status) const;
    int getData(String &data) const;

  private:
    const Sim808 &sim808;
};

#endif // GPS_H
