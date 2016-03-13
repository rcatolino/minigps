// Copyright (C) 2016 raphael.catolino@gmail.com
#ifndef Network_H
#define Network_H

#include <Arduino.h>
#include "Sim808.h"

class Network {
  public:
    Network(const Sim808 &module) : sim808(module), ccid(F("0")), status(0) {}
    int init(String PIN);
    int sendSMS(const String &dest, const String &msg) const;

  private:
    const Sim808 &sim808;
    String ccid;
    int status;
};

#endif // Network_H
