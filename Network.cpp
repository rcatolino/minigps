// Copyright (C) 2016 raphael.catolino@gmail.com
#include <Arduino.h>
#include "Network.h"
#include "Sim808.h"
#include "utils.h"

int Network::sendSMS(const String &dest, const String &msg) const {
  sim808.buildCommand(F("AT+CMGS="));
  sim808.buildCommand(dest);
  sim808.buildCommand(F("\r"));
  sim808.buildCommand(msg);
  sim808.buildCommand(F("\x1a"));

  String results[] = {String()};
  results[0].reserve(MAX_SIZE);
  return sim808.getResults(results);
}

int Network::init(String PIN) {
  int ret = 0;
  if (status == 1) {
    // Already registered
    return -1;
  }

  String results[] = {String()};
  results[0].reserve(MAX_SIZE);
  sim808.sendCommand(F("AT+CPIN?"), results);
  if (results[0].endsWith(F("READY"))) {
    Serial.println("SIM card inserted");
  } else if (results[0].endsWith(F("SIM PIN"))) {
    Serial.print("Inserting PIN ");
    Serial.println(PIN);
    sim808.sendCommand("AT+CPIN=" + PIN, results);
    if (results[0] != "OK") {
      return 1;
    }
  } else {
    return 2;
  }

  sim808.sendCommand(F("AT+CCID"), results);
  const String ccid = results[0];
  Serial.print("SIM ICCID : ");
  Serial.println(ccid);
  sim808.sendCommand(F("AT+CMGF=1"), results);
  delay(2*GRACE_PERIOD);
  sim808.sendCommand(F("AT+CSQ"), results);
  const String strength = results[0].substring(6);
  if (strength.startsWith(F("0")) || strength.startsWith(F("99"))) {
    Serial.println("Warning, no signal");
    ret = 3;
  }
  sim808.sendCommand(F("AT+CREG?"), results);
  if (results[0].endsWith(F("5")) || results[0].endsWith(F("2"))) {
    Serial.println("Registered");
    status = 1;
  }
  return ret;
}
