// Copyright (C) 2016 raphael.catolino@gmail.com
#ifndef Network_H
#define Network_H

#include <Arduino.h>
#include "Sim808.h"

class Network {
  public:
    Network(const Sim808 &module) : sim808(module), ccid(F("0")) {}
    int init(String PIN);
    template <size_t n>
    int sendSMS(const String &dest, const ByteBuffer<n>& msg) const {
      return sendSMS(dest, String(msg.c_str()));
    }
    int sendSMS(const String &dest, const String &msg) const;
    void receiveSMS(const String &idx, String (&results)[2]) const;
    int popSMS(String &sms_txt) const;

  private:
    const Sim808 &sim808;
    String ccid;
};

#endif // Network_H
