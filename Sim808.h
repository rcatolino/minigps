// Copyright (C) 2016 raphael.catolino@gmail.com
#ifndef Sim808_H
#define Sim808_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "utils.h"

class Sim808 {
  public:
    Sim808(SoftwareSerial &sim_link) : link(sim_link) {}

    void buildCommand(const String &cmd_part) const;

    int waitData(int timeout) const;

    template<size_t N>
    int getResults(String (&results)[N]) const {
      if (N < 1) {
        failure(3, link);
      }

      int ret = 0;
      delay(GRACE_PERIOD); // Make sure the sim module has time to answer
      do {
        if (ret >= N) {
          while (link.available() > 0) {
            Serial.write(link.read());
          }
        } else {
          String &result = results[ret];
          result.remove(0);
          getline(result);

          if (result.length() == 0) {
            // ignore empty line
          } else if (result.startsWith(F("AT"))) {
            Serial.print(F("Not keeping : "));
            Serial.println(result);
          } else {
            Serial.print(F("Keeping : "));
            Serial.println(String(ret + 1) + "/" + String(N));
            Serial.println(result);
            ret++;
          }
        }
      } while(link.available() > 0);

      return ret;
    }

    template<size_t N>
    int sendCommand(const String& cmd, String (&results)[N]) const {
      link.println(cmd);
      Serial.print(F("Sending command : "));
      Serial.println(cmd);
      return getResults(results);
    }

    int getline(String &result) const;

  private:
    SoftwareSerial &link;

};

#endif // Sim808_H
