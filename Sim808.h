// Copyright (C) 2016 raphael.catolino@gmail.com
#ifndef Sim808_H
#define Sim808_H

#include <Arduino.h>
#include "ByteBuffer/ByteBuffer.h"
#include "NSoftwareSerial/NSoftwareSerial.h"
#include "utils.h"

class Sim808 {
  public:
    Sim808(SoftwareSerial &sim_link) : link(sim_link) {}

    void init();

    void buildCommand(const String &cmd_part) const;

    int waitData(int timeout) const;

    // Returns -2 if the link is broken, -1 if there is no data available, 0 for an empty line
    // and the number of char read otherwise
    template <size_t n>
    int getline(ByteBuffer<n>& result) const {
      int read = link.available();
      if (read == -1) {
        read = -2;
      } else if (read == 0) {
        read = -1;
      } else {
        read = 0;
      }

      result.remove();
      // A result is complete when : we run out of bytes to read, the max size is reached or an end of line is reached
      while (link.available() > 0 && result.length() < result.maxSize()) {
        char lastchar = (char) link.read();
        if (lastchar == -1) {
          Serial.println(F("lonet serial interrupted"));
          break;
        } else if (lastchar == '\n' || lastchar == '\r') {
          break;
        }
        result.push(lastchar);
        read++;
      }

      return read;
    }

    int getResult(int nb_line_kept) const {
      // Base case
      // There's no more buffers to fill, just print out the rest of the answer
      while (link.available() > 0) {
        Serial.write(link.read());
      }

      return nb_line_kept;
    }

    template<size_t n, typename... Buffers>
    int getResult(int nb_line_kept, ByteBuffer<n>& buff, Buffers... bs) const {
      // Recursion case
      // Fill in this buffer and move on to the next
      buff.remove();
      while (link.available() > 0 && buff.length() == 0) {
        getline(buff);
        if (buff.startsWith("AT")) {
          Serial.print(F("Not keeping : "));
          Serial.println(buff.c_str());
          buff.remove();
        } else {
          nb_line_kept++;
          Serial.print(F("Keeping line nb "));
          Serial.println(String(nb_line_kept));
          Serial.println(buff.c_str());
        }
        delay(2*GRACE_PERIOD);
      }

      return getResult(nb_line_kept, bs...);
    }

    template<typename... Buffers>
    int getResults(Buffers... bs) const {
      int timeout = 0;
      while (link.available() == 0 && timeout < SERIAL_TIMEOUT) {
        delay(GRACE_PERIOD); // Wait for data
        timeout++;
      }
      Serial.print(link.available());
      Serial.println(" char available on serial line");
      return getResult(0, bs...);
    }

    template<size_t N>
    int getResults(String (&results)[N]) const {
      if (N < 1) {
        failure(3, link);
      }

      int ret = 0;
      int timeout = 0;
      while (link.available() == 0 && timeout < SERIAL_TIMEOUT) {
        delay(GRACE_PERIOD); // Wait for data
        timeout++;
      }
      Serial.print(link.available());
      Serial.println(" char available on serial line");
      do {
        if (ret >= N) {
          while (link.available() > 0) {
            Serial.write(link.read());
          }
        } else {
          String &result = results[ret];
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
        delay(2*GRACE_PERIOD);
      } while(link.available() > 0);

      return ret;
    }

    template<typename... Buffers>
    int sendCommand(const String& cmd, Buffers... bs) const {
      link.println(cmd);
      Serial.print(F("Sending command : "));
      Serial.println(cmd);
      return getResults(bs...);
    }

    template<size_t N>
    int sendCommand(const String& cmd, String (&results)[N]) const {
      link.println(cmd);
      Serial.print(F("Sending command : "));
      Serial.println(cmd);
      return getResults(results);
    }

    int getline(String &result) const;

    SoftwareSerial &link;
  private:

};

#endif // Sim808_H
