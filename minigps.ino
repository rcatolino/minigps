// Copyright (C) 2016 raphael.catolino@gmail.com
#include "utils.h"
#include "Network.h"
#include "Sim808.h"

#include <Arduino.h>
#include <LowPower.h>
#include <SoftwareSerial.h>

const int wake_up_on = 2;
SoftwareSerial SerialLonet(RX_A0, TX_A1); // RX = D14/A0, TX = D15/A1

void setup() {
  pinMode(ledPin, OUTPUT);
  //pinMode(loSwitch, OUTPUT);
  Serial.begin(9600);
  Serial.setTimeout(100);
  //swithLonet();
  Serial.println(F("Serial FTDI setup done"));
  SerialLonet.begin(9600);
  SerialLonet.listen();
  //SerialLonet.println("This is the lonet serial line");
  Serial.println(F("Connecting to the Lonet"));

  Sim808 sim808 = Sim808(SerialLonet);
  String results[] = {String()};
  results[0].reserve(MAX_SIZE);
  do {
    sim808.sendCommand(F("AT"), results);
    delay(3000);
  } while (results[0] != "OK");

  // Get battery stats
  sim808.sendCommand(F("AT+CBC"), results);

  // Create and initialize the network
  Network net = Network(sim808);
  net.init("1234");
}

/*
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char) Serial.read();
    Serial.write(inChar);
    command += inChar;
    if (command.length >= 19 || inChar == '\n' || inChar == '\r') {
      debug("Read command : ");
      debug(command);
      command = "";
    }
  }
}
*/

void loop() {
  delay(20000);
  //digitalWrite(ledPin, LOW);
  //LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); // Turn everything off until next interrupt/wdt. Draws 0.38 mA
  //LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_ON, TWI_OFF); // Turn CPU and selected subsystems off, Draws 1.01 mA
}

