#include "soft.h"

#include <Arduino.h>
#include <LowPower.h>
#include <SoftwareSerial.h>

#define ledPin 13
#define loSwitch 17
#define RX_A0 14
#define TX_A1 15

const int wake_up_on = 2;
SoftwareSerial SerialLonet(RX_A0, TX_A1); // RX = D14/A0, TX = D15/A1

void debug(String str) {
  Serial.print(F("Debug : "));
  Serial.println(str);
}

void switchLed(int length) {
  digitalWrite(ledPin, HIGH);
  delay(length);
  digitalWrite(ledPin, LOW);
}

void swithLonet() {
  digitalWrite(loSwitch, HIGH);
  delay(4000);
  digitalWrite(loSwitch, LOW);
}

void failure(int code) {
  Serial.print(F("FAIL, CODE "));
  Serial.println(code);
  while (true) {
    switchLed(2000);
    delay(1000);
    for (int i = 0; i < code; i++) {
      switchLed(200);
    }
    delay(1000);
  }
}

void error(const String& result) {
  Serial.print(F("ERROR : "));
  Serial.println(result);
}

void ok(const String& result) {
  Serial.print(F("Command result : "));
  Serial.println(result);
}

void nothing() {
  Serial.println(F("No answer"));
}

int checkNetworkStatus() {
  String results[] = {String()};
  results[0].reserve(30);
  // Check SIM card status
  sendCommand(F("AT+CPIN?"), results);
  if (results[0].endsWith(F("READY"))) {
    Serial.println("SIM card inserted");
  } else if (results[0].endsWith(F("SIM PIN"))) {
    Serial.println("Inserting PIN 1234");
    sendCommand(F("AT+CPIN=1234"), results);
    if (results[0] != "OK") {
      return 1;
    }
  } else {
    return 2;
  }

  sendCommand(F("AT+CCID"), results);
  const String ccid = results[0];
  Serial.print("SIM ICCID : ");
  Serial.println(ccid);
  // Check GSM signal power
  delay(2000);
  sendCommand(F("AT+CSQ"), results);
  const String strength = results[0].substring(6);
  if (strength.startsWith(F("0")) || strength.startsWith(F("99"))) {
    Serial.println("Warning, no signal");
  }
  // Check GSM network registration
  sendCommand(F("AT+CREG"), results);
  return 0;
}

void setup() {
  pinMode(ledPin, OUTPUT);
  //pinMode(loSwitch, OUTPUT);
  Serial.begin(9600);
  Serial.setTimeout(100);
  //swithLonet();
  debug(F("Serial FTDI setup done"));
  SerialLonet.begin(9600);
  SerialLonet.listen();
  //SerialLonet.println("This is the lonet serial line");
  debug(F("Connecting to the Lonet"));
  String results[] = {String()};
  results[0].reserve(20);
  do {
    sendCommand(F("AT"), results);
    delay(3000);
  } while (results[0] != "OK");

  // Get battery stats
  sendCommand(F("AT+CBC"), results);
  checkNetworkStatus();
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

template<size_t N>
int sendCommand(const String& cmd, String (&results)[N]) {
  SerialLonet.println(cmd);
  if (N < 1) {
    failure(3);
  }

  int ret = 0;
  Serial.print(F("Sending command : "));
  Serial.println(cmd);
  delay(1000);

  do {
    if (ret >= N) {
      //Serial.println("Not keeping further data");
      while (SerialLonet.available() > 0) {
        Serial.write(SerialLonet.read());
      }
    } else {

      String &result = results[ret];
      result.remove(0);
      // A result is complete when : we run out of bytes to read, the buffer is full, an end of line is reached
      while (SerialLonet.available() > 0 && result.length() < 19) {
        char lastchar = (char) SerialLonet.read();
        if (lastchar == -1) {
          failure(2);
          break;
        } else if (lastchar == '\n' || lastchar == '\r') {
          break;
        }
        result += lastchar;
      }

      if (result.length() == 0) {
        // ignore empty line
      } else if (result.startsWith(F("AT"))) {
        Serial.print(F("Not keeping : "));
        Serial.println(result);
      } else {
        Serial.print(F("Keeping : "));
        Serial.println(result);
        Serial.println(results[ret]);
        ret++;
      }
    }
  } while(SerialLonet.available() > 0);

  return ret;
}

void loop() {
  delay(20000);
  //digitalWrite(ledPin, LOW);
  //LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); // Turn everything off until next interrupt/wdt. Draws 0.38 mA
  //LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_ON, TWI_OFF); // Turn CPU and selected subsystems off, Draws 1.01 mA
}

