// Copyright (C) 2016 raphael.catolino@gmail.com
#include <Arduino.h>
#include "Network.h"
#include "Sim808.h"
#include "utils.h"

#define POP_SMS_MAX_RETRY 3
int Network::popSMS(String &sms_txt) const {
  unsigned int count = 0;
  String results[] = {String(), String()};
  results[0].reserve(MAX_SIZE);
  results[1].reserve(MAX_SIZE);
  // List all unread SMS but don't mark them as read yet.
  do {
    sim808.sendCommand("AT+CMGL=\"REC UNREAD\",1", results);
    // This sometimes fails to return anything, we have to retry a few times.
    count++;
    delay(5*GRACE_PERIOD);
  } while (results[0] == F("") && count != POP_SMS_MAX_RETRY);

  if (results[0] == F("OK") || results[0] == F("") || results[0] == F("ERROR")) {
    // No unread SMS
    return 0;
  }

  sms_txt = results[1];
  results[0].remove(0, 7);
  int coma = results[0].indexOf(',');
  String idx = results[0].substring(0, coma);
  Serial.println("New SMS at index : " + idx);
  // Mark the SMS as read
  String discard[] = {String()};
  sim808.sendCommand("AT+CMGR="+idx, discard);
  return 1;
}

void Network::receiveSMS(const String &idx, String (&results)[2]) const {
  // Returns the number of unread messages
  sim808.sendCommand("AT+CMGR=" + idx, results);
}

int Network::sendSMS(const String &dest, const String &msg) const {
  sim808.buildCommand(F("AT+CMGS=\""));
  sim808.buildCommand(dest);
  sim808.buildCommand(F("\"\r"));
  delay(GRACE_PERIOD);
  sim808.buildCommand(msg);
  sim808.buildCommand(F("\x1a"));
  delay(GRACE_PERIOD);
  sim808.buildCommand(F("\r\n"));

  String results[] = {String(), String()};
  results[0].reserve(MAX_SIZE);
  results[1].reserve(MAX_SIZE);
  sim808.getResults(results);
  // We only get sending result when the network acknowledges us
  if (sim808.waitData(10000) != 0) {
    Serial.println("Error, no answer from network");
  }
  sim808.getResults(results);
  if (results[1] == F("OK")) {
    return 0;
  }
  return -1;
}

int Network::init(String PIN) {
  int ret = 0;

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
#define interval 5*GRACE_PERIOD
  int timeout = 10*interval;
  String strength;
  do {
    sim808.sendCommand(F("AT+CSQ"), results);
    strength = results[0].substring(6);
    delay(interval);
    timeout -= interval;
  } while (timeout > 0 && (strength.startsWith(F("0")) || strength.startsWith(F("99"))));

  if (timeout <= 0) {
    Serial.print("Warning, no signal.");
    ret = 3;
  }

  sim808.sendCommand(F("AT+CREG?"), results);
  if (results[0].endsWith(F("5")) || results[0].endsWith(F("2"))) {
    Serial.println("Registered");
  }
  return ret;
}
