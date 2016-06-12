// Copyright (C) 2016 raphael.catolino@gmail.com
#include "GPS.h"
#include "Network.h"
#include "phonenumber.h"
#include "Sim808.h"
#include "State.h"
#include "utils.h"
#include "NSoftwareSerial/NSoftwareSerial.h"

#include <Arduino.h>

SoftwareSerial SerialLonet(RX_A1, TX_A0); // RX = D15/A1, TX = D14/A0
Sim808 sim808 = Sim808(SerialLonet);
Network net = Network(sim808);
GPS gps = GPS(sim808);
int last_event = 0; // Number of loop cycle since last event
volatile int lo_active = 0; // Wether the lonet is waiting with data

void lo_event() {
  lo_active = 1;
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(LO_INT, INPUT);
  pinMode(LO_SLEEP_CTL, OUTPUT);
  setLoSleep(0);
  powerSave();
  Serial.begin(9600);
  Serial.setTimeout(100);
  Serial.println(F("Serial FTDI setup done"));
  SerialLonet.begin(9600);
  SerialLonet.listen();

  sim808.init();
  // Create and initialize the network
  if (net.init(SIM_PIN) != 0) {
    failure(4, SerialLonet);
  }

  String results[] = {String()};
  results[0].reserve(MAX_SIZE);
  // Get battery stats
  sim808.sendCommand(F("AT+CBC"), results);
  String cbc = results[0];
  net.sendSMS(F(PHONE_NUMBER), "SMS module initialization successful. Battery state = " + cbc);
  attachInterrupt(digitalPinToInterrupt(LO_INT), lo_event, FALLING);
}

// Called when data is received from debug port
void serialEvent() {
  serialPipe(SerialLonet);
  last_event = 0;
}

void cmd_getstat() {
  String battery[] = {String()};
  String net_signal[] = {String()};
  digitalWrite(LED, HIGH);
  sim808.sendCommand(F("AT+CBC"), battery);
  sim808.sendCommand(F("AT+CSQ"), net_signal);
  net.sendSMS(F(PHONE_NUMBER), battery[0] + ", " + net_signal[0]);
  digitalWrite(LED, LOW);
}

void cmd_getpos() {
  String gps_data;
  unsigned int fix_attempt = 0;
  digitalWrite(LED, HIGH);

  do {
    gps.getData(gps_data);
    if (gps_data.startsWith("1,1")) {
      net.sendSMS(F(PHONE_NUMBER), gps_data);
      fix_attempt = MAX_FIX_ATTEMPT+1;
    } else {
      delay(20*GRACE_PERIOD);
      fix_attempt += 1;
    }
  } while (fix_attempt < MAX_FIX_ATTEMPT);
  digitalWrite(LED, LOW);
}

void discard_line() {
  String buffer;
  buffer.reserve(MAX_SIZE);
  if (sim808.getline(buffer) == 0) {
    sim808.getline(buffer);
  }
  Serial.print("Discarding : ");
  Serial.println(buffer);
}

void handle_notification(State &st) {
  Serial.println("Enter HANDLE NOTIFICATION state");
  String buffer;
  int sms_to_handle = 0;
  buffer.reserve(MAX_SIZE);
  while (sim808.getline(buffer) >= 0) {
    Serial.print("Event received : ");
    Serial.println(buffer);
    if (buffer.startsWith(F("+CMTI"))) {
      // We have a new SMS
      sms_to_handle++;
    } else if (buffer.startsWith(F("+CMT"))) {
      discard_line();
    } else if (buffer.startsWith(F("CBM"))) {
      // Ignore cell broadcasts
      discard_line();
    } else if (buffer.startsWith(F("CDS"))) {
      // SMS Status Report
      // TODO: verify the SMS status and try to resend it if failed
      discard_line();
    }

    buffer.remove(0);
  }

  while (net.popSMS(buffer)) {
    Serial.println(buffer);
    if (buffer == "getpos") {
      gps.powerOn();
      cmd_getpos();
      sms_to_handle--;
    } else if (buffer == "getstat") {
      cmd_getstat();
      sms_to_handle--;
    } else {
      Serial.print("Unkown command : ");
      Serial.println(buffer);
    }
  }

  if (sms_to_handle > 0) {
    // We have received an SMS but couldn't read it with net.popSMS
    failure(8, SerialLonet);
  }

  st.next = sleep_active;
  Serial.println("Leave HANDLE NOTIFICATION state");
}

void sleep_powerdown(State &st) {
  Serial.println("Enter SLEEP POWERDOWN state");
  // Power down the lonet
  String results[] = {String()};
  results[0].reserve(MAX_SIZE);
  do {
    sim808.sendCommand(F("AT+CPOWD=1"), results);
    delay(10*GRACE_PERIOD);
  } while (results[0] != "NORMAL POWER DOWN");
  // Power down the arduino, until TIME_POWD has run out
  sleep(TIME_POWD);
  loSwitchPower(); // Restart the lonet
  sim808.init();
  net.init(SIM_PIN);
  st.next = sleep_active;
  Serial.println("Leave SLEEP POWERDOWN state");
}

void sleep_active(State &st) {
  Serial.println("Enter SLEEP ACTIVE state");
  // Enable sleep mode on the lonet. Still pulls ~15 mA.
  setLoSleep(1);
  // Turn everything off on the mcu until next interrupt or until TIME_ACTIVE has run out
  // Draws ~300 uA with raw power
  sleep(TIME_ACTIVE);

  setLoSleep(0);
  if (lo_active > 0) {
    lo_active = 0;
    st.next = handle_notification;
  } else {
    st.next = sleep_powerdown;
  }
  Serial.println("Leave SLEEP ACTIVE state");
}

State st = State(handle_notification);
void loop() {
  st.next(st);
  //LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_ON, TWI_OFF); // Turn CPU and selected subsystems off, Draws 1.01 mA
}

