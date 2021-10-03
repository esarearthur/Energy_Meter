#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ctype.h>
#include <string.h>

// Select your modem:
#define DUMP_AT_COMMANDS
#define TINY_GSM_MODEM_SIM800

// Set phone numbers, if you want to test SMS and Calls
#define SMS_TARGET "0267766253"
#define READ_SMS

HardwareSerial &SerialMon = Serial;
#define TINY_GSM_DEBUG SerialMon
#include <TinyGSM.h>

SoftwareSerial SerialAT(2, 3);

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

void setup() {
  // put your setup code here, to run once:
  // Set debug baud rate
  SerialMon.begin(9600);
  while (!SerialMon);

  SerialAT.listen();

  // Set GSM module baud rate
  if (TinyGsmAutoBaud(SerialAT) > 0) {
    modem.restart();
    delay(3000);

    String modemInfo = modem.getModemInfo();
    DBG("\r\nModem: ", modemInfo);

    DBG("Waiting for network...");
    if (!modem.waitForNetwork(10000L)) {
      DBG("GSM Network Error");
    }
  }

  SerialMon.println("Complete");

  String imei = modem.getIMEI();
  DBG("IMEI:", imei);
}

bool res = false;
unsigned long countdown = 0;

void loop() {
  // put your main code here, to run repeatedly:
  int *idxbuf;
  SMSmessage msg;
  String sms;

  String sendmsg;

  res = modem.sendSMS(SMS_TARGET, sendmsg);
  DBG("SMS:", res ? "OK" : "fail");
  countdown = millis();

  for (unsigned long start = millis(); millis() - start < 5000;) {
    int msgcnt = modem.getUnreadSMSLocs(idxbuf);
    for (int j = 0; j < msgcnt; j++) {
      msg = modem.readSMS(msgcount[j]);
      DBG(msg.message);
    }
  }
}
