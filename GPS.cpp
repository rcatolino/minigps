// Copyright (C) 2016 raphael.catolino@gmail.com
#include <Arduino.h>
#include <string.h>

#include "GPS.h"
#include "Sim808.h"
#include "utils.h"

int GPS::getStatus() const {
  String results[] = {String()};
  results[0].reserve(MAX_SIZE);
  sim808.sendCommand(F("AT+CGNSPWR?"), results);
  if (results[0].startsWith(F("1"))) {
    return 1;
  }

  return 0;
}

int GPS::powerOn() const {
  String results[] = {String()};
  results[0].reserve(MAX_SIZE);
  sim808.sendCommand(F("AT+CGNSPWR=1"), results);
  if (results[0] != F("OK")) {
    return -1;
  }

  return 0;
}

int GPS::powerOff() const {
  String results[] = {String()};
  results[0].reserve(MAX_SIZE);
  sim808.sendCommand(F("AT+CGNSPWR=0"), results);
  if (results[0] != F("OK")) {
    return -1;
  }

  return 0;
}

/*
int GPS::setSeqType(const String &seq) {
  // Seq can be :
  // GGA : time, position and fix type
  // GSA : operating mode, satellites used, dillution of precision
  // GSV : satellites in view, elevation, azimuth, SNR per satellite
  // RMC : time, position, speed, azimuth
  String results[] = {String()};
  results[0].reserve(MAX_SIZE);
  sim808.sendCommand("AT+CGNSSEQ=" + seq, results);
  if (results[0] != F("OK")) {
    return -1;
  }

  return 0;
}
*/

int GPS::getData(String &data) const {
  String results[] = {String()};
  results[0].reserve(MAX_SIZE);
  sim808.sendCommand("AT+CGNSINF", results);

  if (results[0].length() <= 14) {
    Serial.println(results[0]);
    Serial.println("Error, CGNSINF answer is too short to be valid");
    return -1;
  }

  if (!results[0].startsWith(F("+CGNSINF: "))) {
    Serial.print("Error, CGNSINF answer doesn't start with +CGNSINF: ");
    return -2;
  }

  data = results[0].substring(10);
  return 0;
}
