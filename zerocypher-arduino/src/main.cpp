#include <Arduino.h>
#include <Servo.h>
#include <ArduinoJson.h>
#include <SPI.h>

#include "debug.h"
#include "crypthograpy.h"

#define SERVO_PIN 10
#define TONE_PIN 3

int A_VALUE = 25;
int Z_VALUE = 175;

float STEP = (Z_VALUE - A_VALUE) / 25.0;

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

void setup() {
  // put your setup code here, to run once:
  pinMode(SERVO_PIN, OUTPUT);
  myServo.attach(SERVO_PIN);
  myServo.write(180);
  Serial.begin(9600);
}

void loop() {
  String value = waitForString();
  if(value == "") return;
  InPacket packet = processJSON(value);

  if(packet.status="calibration") {
    OutPacket writing {packet.id, "calibrating"};
    calibrate(packet.message);
  }

  if(packet.status == "request") {
    OutPacket writing {packet.id, "writing"};

    Serial.println(serializeOutput(writing));

    String string = getWriteString(packet);
    writeString(string);
  }
  
  OutPacket out {packet.id, "done"};
  Serial.println(serializeOutput(out));
}

#pragma region processing
InPacket processJSON(String json) {
  StaticJsonBuffer<300> buffer;
  JsonObject& root = buffer.parseObject(json.c_str());
  
  InPacket packet {root["id"], root["message"], root["key"], root["mode"], root["algorithm"], root["status"]};
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
    tone(TONE_PIN, 2000, 200);
    return;
  }

  tone(TONE_PIN, 1000, 200);
  int writeValue = value - 97;
  float fvalue = A_VALUE + (float)(STEP * writeValue);
  myServo.write(180 - round(fvalue));
}

String waitForString() {
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

  if(a_v < 0 || z_v < 0 || a_v > 180 || z_v > 180) {
    return;
  }

  A_VALUE = a_v;
  Z_VALUE = z_v;

  DEBUG_PRINTLN(A_VALUE);
  DEBUG_PRINTLN(Z_VALUE);


  myServo.write(0);
  //tone(TONE_PIN, 1000, 100);
  myServo.write(A_VALUE);
  //tone(TONE_PIN, 1000, 1000);

  myServo.write(180);
  //tone(TONE_PIN, 1000, 100);
  myServo.write(Z_VALUE);
  //tone(TONE_PIN, 1000, 1000);
}
