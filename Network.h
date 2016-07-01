// Copyright (C) 2016 raphael.catolino@gmail.com
#ifndef Network_H
#define Network_H

#include <Arduino.h>
#include "Sim808.h"

#define POP_SMS_MAX_RETRY 3

class Network {
  public:
    Network(const Sim808 &module) : sim808(module) {}

    template <typename T>
    int init(const T& PIN) {
      int ret = 0;

      ByteBuffer<MAX_SIZE> result;
      sim808.sendCommand("AT+CPIN?", result);
      if (result.endsWith("READY")) {
        Serial.println("SIM card inserted");
      } else if (result.endsWith("SIM PIN")) {
        Serial.print("Inserting PIN ");
        Serial.println(PIN);
        ByteBuffer<MAX_SIZE> cmd;
        cmd.push("AT+CPIN=");
        cmd.push(PIN);
        sim808.sendCommand(cmd);
        sim808.sendCommand("AT+CPIN?", result);
        if (!result.endsWith("READY")) {
          return 1;
        }
      } else {
        return 2;
      }

      sim808.sendCommand("AT+CCID", result);
      Serial.print("SIM ICCID : ");
      Serial.println(result.c_str());

      sim808.sendCommand("AT+CMGF=1");
#define interval 5*GRACE_PERIOD
      int timeout = 10*interval;
      ByteBuffer<MAX_SIZE> strength;
      do {
        sim808.sendCommand("AT+CSQ", result);
        strength = result.substring(6);
        delay(interval);
        timeout -= interval;
      } while (timeout > 0 && (strength.startsWith("0") || strength.startsWith("99")));

      if (timeout <= 0) {
        Serial.print("Warning, no signal.");
        ret = 3;
      }

      sim808.sendCommand("AT+CREG?", result);
      if (result.endsWith("5") || result.endsWith("2")) {
        Serial.println("Registered");
      }

      return ret;
    }

    template <size_t n, typename T>
    int sendSMS(const T& dest, const ByteBuffer<n>& msg) const {
      sim808.buildCommand(F("AT+CMGS=\""));
      sim808.buildCommand(dest);
      sim808.buildCommand(F("\"\r"));
      delay(GRACE_PERIOD);
      sim808.buildCommand(msg);
      sim808.buildCommand(F("\x1a"));
      delay(GRACE_PERIOD);
      sim808.buildCommand(F("\r\n"));

      ByteBuffer<MAX_SIZE> result,status;
      sim808.getResults(result, status);
      // We only get sending result when the network acknowledges us
      if (sim808.waitData(10000) != 0) {
        Serial.println("Error, no answer from network");
      }

      sim808.getResults(result, status);
      if (status == "OK") {
        return 0;
      }
      return -1;
    }

    template <size_t n>
    int popSMS(ByteBuffer<n>& sms_txt) const {
      unsigned int count = 0;
      ByteBuffer<MAX_SIZE> status;
      // List all unread SMS but don't mark them as read yet.
      do {
        sim808.sendCommand("AT+CMGL=\"REC UNREAD\",1", status, sms_txt);
        // This sometimes fails to return anything, we have to retry a few times.
        count++;
        delay(5*GRACE_PERIOD);
      } while (status == "" && count != POP_SMS_MAX_RETRY);

      if (status == "OK" || status == "" || status == "ERROR") {
        // No unread SMS
        return 0;
      }

      auto idx = status.substring(6, status.indexOf(','));
      Serial.print("New SMS at index : ");
      Serial.println(idx.c_str());
      // Delete the SMS
      ByteBuffer<20> deletecmd;
      deletecmd.push("AT+CMGD=");
      deletecmd.push(idx);
      sim808.sendCommand(deletecmd);
      return 1;
    }

  private:
    const Sim808 &sim808;
};

#endif // Network_H
