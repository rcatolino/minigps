// Copyright (C) 2016 raphael.catolino@gmail.com
#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include "ByteBuffer/ByteBuffer.h"
#include "Sim808.h"

// The GPS module draws between 25 and 35 mA.

class GPS {
  public:
    GPS(const Sim808 &module) : sim808(module) {}
    int getStatus() const;
    int powerOn() const;
    int powerOff() const;
    template <size_t n>
    int getData(ByteBuffer<n>& data) const {
      sim808.sendCommand("AT+CGNSINF", data);

      if (data.length() <= 14) {
        Serial.println(data.c_str());
        Serial.println(F("Error, CGNSINF answer is too short to be valid"));
        return -1;
      }

      if (!data.startsWith("+CGNSINF: ")) {
        Serial.print(F("Error, CGNSINF answer doesn't start with +CGNSINF: "));
        return -2;
      }

      data = data.substring(10);
      return 0;
    }

  private:
    const Sim808 &sim808;
};

#endif // GPS_H
