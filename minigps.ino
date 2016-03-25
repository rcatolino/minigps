// Copyright (C) 2016 raphael.catolino@gmail.com
#include "GPS.h"
#include "Network.h"
#include "phonenumber.h"
#include "Sim808.h"
#include "utils.h"

#include <Arduino.h>
#include <LowPower.h>
#include <SoftwareSerial.h>

const int wake_up_on = 2;
SoftwareSerial SerialLonet(RX_A0, TX_A1); // RX = D14/A0, TX = D15/A1
Sim808 sim808 = Sim808(SerialLonet);
Network net = Network(sim808);
GPS gps = GPS(sim808);

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  Serial.setTimeout(100);
  Serial.println(F("Serial FTDI setup done"));
  SerialLonet.begin(9600);
  SerialLonet.listen();

  String results[] = {String()};
  results[0].reserve(MAX_SIZE);
  do {
    sim808.sendCommand(F("AT"), results);
    delay(3000);
  } while (results[0] != "OK");

  // Get battery stats
  sim808.sendCommand(F("AT+CBC"), results);

  // Create and initialize the network
  if (net.init("1234") != 0) {
    failure(4);
  }

  gps.init();
  net.sendSMS(F(PHONE_NUMBER), "SMS module initialization successful");
  String gps_data;
  gps.getData(gps_data);
  net.sendSMS(F(PHONE_NUMBER), gps_data);
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char) Serial.read();
    SerialLonet.write(inChar);
  }

  delay(GRACE_PERIOD);
  while (SerialLonet.available()) {
    char inChar = (char) SerialLonet.read();
    Serial.write(inChar);
  }
}

void loop() {
  delay(100);
  //digitalWrite(ledPin, LOW);
  //LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); // Turn everything off until next interrupt/wdt. Draws 0.38 mA
  //LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_ON, TWI_OFF); // Turn CPU and selected subsystems off, Draws 1.01 mA
}

