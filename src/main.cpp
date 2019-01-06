#include <Arduino.h>
#include <Servo.h>
#include <ArduinoJson.h>

#define SERVO_PIN 10
#define STEP 7.2f

#pragma region typedef
struct InPacket {
  int id;
  String message;
  int key;
  bool mode;
  String algorithm; // TODO not used
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

void setup() {
  // put your setup code here, to run once:
  pinMode(SERVO_PIN, OUTPUT);
  myServo.attach(SERVO_PIN);
  myServo.write(180);
  Serial.begin(9600);
}

void loop() {
  String value = waitForString();
  Serial.println(value.c_str());
  InPacket packet = processJSON(value);
  
  auto string = getWriteString(packet);
  writeString(string);
  
  OutPacket out {packet.id, "OK"};
  Serial.println(serializeOutput(out));
}

#pragma region processing
InPacket processJSON(String json) {
  StaticJsonBuffer<300> buffer;
  JsonObject& root = buffer.parseObject(json.c_str());
  
  InPacket packet {root["id"], root["msg"], root["key"], root["mode"], root["algorithm"]};
  return packet; 
}

String serializeOutput(OutPacket packet) {
  char buffer[100];
  sprintf(buffer, "{\"id\"=%d,\"status\"=%s", packet.id, packet.status);
  return String(buffer);
}

String getWriteString(InPacket packet) {
  if(packet.algorithm == "ceasar") {
    return packet.mode ? ceasarEncrypt(packet.message, packet.key) : ceasarDecrypt(packet.message, packet.key);
  }
}
#pragma endregion

#pragma region criptatura
String ceasarEncrypt(String s, int key) {
  String newString = String();
  for(char c : s) {
    auto nc = c+key;
    newString.concat(nc);
  }
  Serial.println(newString);
}

String ceasarDecrypt(String e, int key) {
  return "notimpl";
}

#pragma endregion

#pragma region i-o
void writeString(String str) {
  for(int i = 0; i<str.length(); ++i) {
    if((int)str[i] == 13) continue;
    writeLetter(str[i]);
    tone(3, 1000, 200);
    delay(1000);
  }
}

void writeLetter(char c) {
  int value = (int)c;
  int writeValue = value - 97;
  myServo.write(180 - STEP*writeValue);
}

String waitForString() {
  String str = "";
  while(Serial.available() == 0){;}
  str = Serial.readStringUntil('\n');
  Serial.println("String letta:" + str);
  return str;
}
#pragma endregion
