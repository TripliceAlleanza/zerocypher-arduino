#include <Arduino.h>
#include <Servo.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <EEPROM.h>

#include "debug.h"
#include "crypthograpy.h"

#define SERVO_PIN 10
#define TONE_PIN 3

#define EEPROM_A_ADDRESS 0xA0
#define EEPROM_Z_ADDRESS 0xA4

int A_VALUE = 25;
int Z_VALUE = 175;

float STEP() { return (Z_VALUE - A_VALUE) / 25.0; }

#pragma region typedef
struct InPacket {
  int id;
  String message;
  String key;
  bool mode;
  String algorithm;
  String status;
};

struct OutPacket {
  int id;
  String status;
};

#pragma endregion

Servo myServo;

String waitForString();
void writeLetter(char);
void writeString(String);
String ceasarEncrypt(String,int);
String ceasarDecrypt(String,int);
InPacket processJSON(String json);
String getWriteString(InPacket packet);
String serializeOutput(OutPacket packet);
void calibrate(String);
void __EEPROM_WRITE_ANGLE(int a, int z);
void __EEPROM_GET_ANGLE();

void setup() {
  // put your setup code here, to run once:
  EEPROM.begin();
#ifndef DEBUG
  Serial.begin(9600, SERIAL_8E2);
#else
  Serial.begin(9600);
#endif
  pinMode(SERVO_PIN, OUTPUT);
  myServo.attach(SERVO_PIN);
  myServo.write(180); // go to 0
  __EEPROM_GET_ANGLE();

  DEBUG_PRINT("A,Z");
  DEBUG_PRINTLN(A_VALUE);
  DEBUG_PRINTLN(Z_VALUE);
}

void loop() {
  String value = waitForString();
  if(value == "") return;
  InPacket packet = processJSON(value);

  DEBUG_PRINTLN(packet.status);

  if(packet.status == "ping") {
    OutPacket ready {0, "ready."};
    Serial.println(serializeOutput(ready));
    return;
  }

  if(packet.status == "calibration") {
    OutPacket writing {packet.id, "wait"};
    Serial.println(serializeOutput(writing));
    calibrate(packet.message);
  }

  if(packet.status == "request") {
    OutPacket writing {packet.id, "writing"};

    Serial.println(serializeOutput(writing));

    String string = getWriteString(packet);
    DEBUG_PRINTLN(string);
    writeString(string);
  }
  
  OutPacket out {packet.id, "done"};
  Serial.println(serializeOutput(out));
}

#pragma region processing
InPacket processJSON(String json) {
  StaticJsonBuffer<300> buffer;
  JsonObject& root = buffer.parseObject(json.c_str());
  
  InPacket packet {root["id"], root["message"], root["key"], root["mode"], root["algorithm"], String((const char*)root["status"])};
  return packet; 
}

String serializeOutput(OutPacket packet) {
  char buffer[100];
  sprintf(buffer, "{\"id\":%d,\"status\":\"%s\"}", packet.id, packet.status.c_str());
  return String(buffer);
}

String getWriteString(InPacket packet) {
  if(packet.algorithm == "ceasar" || packet.algorithm == "cesare") {
    return packet.mode ? ceasarEncrypt(packet.message, packet.key.toInt()) : ceasarDecrypt(packet.message, packet.key.toInt());
  }
  if(packet.algorithm == "transposition" || packet.algorithm == "trasposizione") {
    return packet.mode ? transpositionEncrypt(packet.message, packet.key) : transpositionDecrypt(packet.message, packet.key);
  }
  if(packet.algorithm == "plaintext" || packet.algorithm == "testo") {
    return packet.message;
  }
  if(packet.algorithm == "showalphabet" || packet.algorithm == "alfabeto") {
    return "abcdefghijklmnopqrstuvwxyz *";
  }
}
#pragma endregion

#pragma region i-o
void writeString(String str) {
  // Serial.println(str);
  for(int i = 0; i<str.length(); ++i) {
    if((int)str[i] == 13) continue;
    writeLetter(str[i]);
    delay(1000);
  }
}

void writeLetter(char c) {
  int value = (int)c;
  if(value == 32) {
    tone(TONE_PIN, 1300, 100);
    delay(100);
    tone(TONE_PIN, 1300, 100);
    return;
  }

  if(c == '*') {
    tone(TONE_PIN, 2000, 300);
    return;
  }

  tone(TONE_PIN, 1000, 200);
  int writeValue = value - 97;
  float fvalue = A_VALUE + (float)(STEP() * writeValue);
  myServo.write(180 - round(fvalue));
}

String waitForString() {
  while(!Serial){;}
  String str = "";
  OutPacket ready {0, "ready."};
  Serial.println(serializeOutput(ready));
  while(Serial.available() == 0){;}
  str = Serial.readStringUntil('\n');
  return str;
}
#pragma endregion

void calibrate(String values) {
  if(values.indexOf(";") <= 0) {
    return;
  }


  char str[30];
  strcpy(str, values.c_str());

  int a_v = atoi(strtok(str, ";"));
  char* cz_v = strtok(NULL, ";");
  int z_v = atoi(cz_v);

  /*if(a_v < 0 || z_v < 0 || a_v > 180 || z_v > 180) {
    return;
  }*/

  A_VALUE = a_v;
  Z_VALUE = z_v;

  DEBUG_PRINTLN(A_VALUE);
  DEBUG_PRINTLN(Z_VALUE);


  // write value of A
  myServo.write(180 - A_VALUE);
  tone(TONE_PIN, 1500, 100);
  delay(1000);
  

  // write 0
  myServo.write(180 - 0);
  tone(TONE_PIN, 1000, 100);
  delay(1000);
  

  // write value of Z
  myServo.write(180 - Z_VALUE);
  tone(TONE_PIN, 1500, 100);
  delay(1000);


  // write value of 180
  myServo.write(180 - 180);
  tone(TONE_PIN, 1000, 100);
  delay(1000);
  
  __EEPROM_WRITE_ANGLE(A_VALUE, Z_VALUE); 
}

void __EEPROM_WRITE_ANGLE(int a, int z) {
  EEPROM.put(EEPROM_A_ADDRESS, a);
  EEPROM.put(EEPROM_Z_ADDRESS, z);
}

void __EEPROM_GET_ANGLE() {
  int a,z;

  EEPROM.get(EEPROM_A_ADDRESS, a);
  EEPROM.get(EEPROM_Z_ADDRESS, z);

  DEBUG_PRINTLN(a);
  DEBUG_PRINTLN(z);

  bool flag = false;

  if(a < 0 || a > 180) {
    a = 25;
    flag = true;
  }
  
  if(z < 0 || z > 180) {
    z = 175;
    flag = true;
  }
  
  A_VALUE = a;
  Z_VALUE = z;
  
  if(flag)
    __EEPROM_WRITE_ANGLE(a,z);
}