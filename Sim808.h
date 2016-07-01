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

    template <typename T>
    void buildCommand(const T& cmd_part) const {
      link.print(cmd_part);
    }
    template <size_t n>
    void buildCommand(const ByteBuffer<n>& cmd_part) const {
      link.print(cmd_part.c_str());
    }


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

    template<size_t n, typename... Buffers>
    int sendCommand(const ByteBuffer<n>& cmd, Buffers... bs) const {
      link.println(cmd.c_str());
      Serial.print(F("Sending command : "));
      Serial.println(cmd.c_str());
      return getResults(bs...);
    }

    template<size_t n, typename... Buffers>
    int sendCommand(const char (&cmd)[n], Buffers... bs) const {
      link.println(cmd);
      Serial.print(F("Sending command : "));
      Serial.println(cmd);
      return getResults(bs...);
    }

    SoftwareSerial &link;
  private:

};

#endif // Sim808_H
