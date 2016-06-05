// Copyright (C) 2016 raphael.catolino@gmail.com
#include "GPS.h"
#include "Network.h"
#include "phonenumber.h"
#include "Sim808.h"
#include "State.h"
#include "utils.h"
#include "NSoftwareSerial/NSoftwareSerial.h"

#include <Arduino.h>
#include <LowPower.h>

#define DEBUG 1

const int wake_up_on = 2;
SoftwareSerial SerialLonet(RX_A0, TX_A1); // RX = D14/A0, TX = D15/A1
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
  pinMode(LO_POW_CTL, OUTPUT);
  setLoSleep(0);
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

#ifdef DEBUG
  // Set more verbose error reporting
  sim808.sendCommand(F("AT+CMEE=2"), results);
#endif //DEBUG

  // Activate DTR power management (pull high to enter sleep mode)
  sim808.sendCommand(F("AT+CSCLK=1"), results);
  // Get battery stats
  sim808.sendCommand(F("AT+CBC"), results);
  String cbc = results[0];

  // Create and initialize the network
  if (net.init("1234") != 0) {
    failure(4, SerialLonet);
  }

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
  if (!gps.getStatus()) {
    gps.powerOn();
  }

  do {
    gps.getData(gps_data);
    if (gps_data.startsWith("1,1")) {
      net.sendSMS(F(PHONE_NUMBER), gps_data);
      fix_attempt = MAX_FIX_ATTEMPT+1;
    } else {
      setLoSleep(1);
      delay(20*GRACE_PERIOD);
      setLoSleep(0);
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
  Serial.println(buffer);
}

void handle_notification(State &st) {
  Serial.println("Enter HANDLE NOTIFICATION state");
  String buffer;
  buffer.reserve(MAX_SIZE);
  while (sim808.getline(buffer) != -1) {
    Serial.println(buffer);
    if (buffer.startsWith(F("+CMTI"))) {
      // We have a new SMS
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
      cmd_getpos();
    } else if (buffer == "getstat") {
      cmd_getstat();
    } else {
      Serial.print("Unkown command : ");
      Serial.println(buffer);
    }
  }

  st.next = sleep_active;
  Serial.println("Leave HANDLE NOTIFICATION state");
}

void sleep_active(State &st) {
    Serial.println("Enter SLEEP ACTIVE state");
    Serial.end();
    // Enable sleep mode on the lonet. Still pulls ~15 mA.
    setLoSleep(1);
    // Turn everything off on the mcu until next interrupt/wdt. Draws ~300 uA with raw power
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

    if (lo_active > 0) {
      lo_active = 0;
      setLoSleep(0);
      Serial.begin(9600);
      st.next = handle_notification;
      delay(2*GRACE_PERIOD);
    } else {
//      st.next = sleep_powerdown;
      st.next = sleep_active;
    }
    Serial.println("Leave SLEEP ACTIVE state");
}

State st = State(handle_notification);
void loop() {
  st.next(st);
  //LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_ON, TWI_OFF); // Turn CPU and selected subsystems off, Draws 1.01 mA
}

