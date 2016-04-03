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
int clock = 0;
int fix_sent = 0;
volatile int lo_active = 0;

void lo_event() {
  lo_active = 1;
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(LO_INT, INPUT);
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

  // Set more verbose error reporting
  sim808.sendCommand(F("AT+CMEE=2"), results);
  // Get battery stats
  sim808.sendCommand(F("AT+CBC"), results);
  String cbc = results[0];

  // Create and initialize the network
  if (net.init("1234") != 0) {
    failure(4, SerialLonet);
  }

  gps.init();
  net.sendSMS(F(PHONE_NUMBER), "SMS module initialization successful. Battery state = " + cbc);

  attachInterrupt(digitalPinToInterrupt(LO_INT), lo_event, FALLING);
}

void serialEvent() {
  serialPipe(SerialLonet);
}

void loop() {
  String gps_data;
  delay(100);
  clock += 100;
  if (clock < 0) {
    clock = 0;
  }

  if ((clock % 1000) == 0) {
    if (lo_active) {
      lo_active = 0;
      Serial.println("Event Received from lonet");
      String buffer;
      buffer.reserve(MAX_SIZE);
      delay(2*GRACE_PERIOD);
      while (sim808.getline(buffer) != -1) {
        Serial.println(buffer);
        if (buffer.startsWith(F("+CMTI"))) {
          buffer.remove(0, 7);
          int coma = buffer.indexOf(',');
          String mem = buffer.substring(0, coma);
          String idx = buffer.substring(coma+1);
          Serial.println("New SMS in " + mem + " memory. Index : " + idx);
          String results[] = {String(), String()};
          net.receiveSMS(idx, results);
          Serial.println("trololo");
          Serial.println(results[0]);
          Serial.println(results[1]);
        }
        buffer.remove(0);
      }
      //net.readSMS();
    }
  /*
    digitalWrite(ledPin, HIGH);
    gps.getData(gps_data);
    if (gps_data.startsWith("1,1") && fix_sent == 0) {
      fix_sent = 1;
      net.sendSMS(F(PHONE_NUMBER), gps_data);
    }
    digitalWrite(ledPin, LOW);
  */
  }
  //digitalWrite(ledPin, LOW);
  //LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); // Turn everything off until next interrupt/wdt. Draws 0.38 mA
  //LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_ON, TWI_OFF); // Turn CPU and selected subsystems off, Draws 1.01 mA
}

