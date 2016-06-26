// Copyright (C) 2016 raphael.catolino@gmail.com
#ifndef Network_H
#define Network_H

#include <Arduino.h>
#include "Sim808.h"

#define POP_SMS_MAX_RETRY 3

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
      sim808.sendCommand(deletecmd.c_str());
      return 1;
    }

    int popSMS(String &sms_txt) const;

  private:
    const Sim808 &sim808;
    String ccid;
};

#endif // Network_H
