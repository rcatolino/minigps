#ifndef Sim808_H
#define Sim808_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "utils.h"

class Sim808 {
  public:
    Sim808(SoftwareSerial &sim_link) : link(sim_link) {}

    template<size_t N>
    int sendCommand(const String& cmd, String (&results)[N]) const {
      link.println(cmd);
      if (N < 1) {
        failure(3);
      }

      int ret = 0;
      Serial.print(F("Sending command : "));
      Serial.println(cmd);
      delay(1000);

      do {
        if (ret >= N) {
          while (link.available() > 0) {
            Serial.write(link.read());
          }
        } else {
          String &result = results[ret];
          result.remove(0);
          // A result is complete when : we run out of bytes to read, the buffer is full or an end of line is reached
          while (link.available() > 0 && result.length() < 19) {
            char lastchar = (char) link.read();
            if (lastchar == -1) {
              failure(2);
              break;
            } else if (lastchar == '\n' || lastchar == '\r') {
              break;
            }
            result += lastchar;
          }

          if (result.length() == 0) {
            // ignore empty line
          } else if (result.startsWith(F("AT"))) {
            Serial.print(F("Not keeping : "));
            Serial.println(result);
          } else {
            Serial.print(F("Keeping : "));
            Serial.println(result);
            Serial.println(results[ret]);
            ret++;
          }
        }
      } while(link.available() > 0);

      return ret;
    }

  private:
    SoftwareSerial &link;

};

#endif // Sim808_H
