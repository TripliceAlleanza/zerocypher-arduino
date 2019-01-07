#include <Arduino.h>
#include <Servo.h>
#include <ArduinoJson.h>

#define DEBUG
#define DEBUG_OUTPUT Serial

#ifdef DEBUG
#define DEBUG_PRINTLN(...) DEBUG_OUTPUT.println(__VA_ARGS__)
#define DEBUG_PRINT(...) DEBUG_OUTPUT.print(__VA_ARGS__)
#else
#define DEBUG_PRINTLN(...) {}
#define DEBUG_PRINT(...) {}
#endif

#define SERVO_PIN 10
#define STEP 6.15f

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
InPacket processJSON(String json);
String getWriteString(InPacket packet);
String serializeOutput(OutPacket packet);

void setup() {
  // put your setup code here, to run once:
  pinMode(SERVO_PIN, OUTPUT);
  myServo.attach(SERVO_PIN);
  myServo.write(180);
  Serial.begin(9600);
}

void loop() {
  String value = waitForString();
  
  InPacket packet = processJSON(value);
  OutPacket writing {packet.id, "writing"};

  Serial.println(serializeOutput(writing));

  String string = getWriteString(packet);
  writeString(string);
  
  OutPacket out {packet.id, "done"};
  Serial.println(serializeOutput(out));
}

#pragma region processing
InPacket processJSON(String json) {
  StaticJsonBuffer<300> buffer;
  JsonObject& root = buffer.parseObject(json.c_str());
  
  InPacket packet {root["id"], root["message"], root["key"], root["mode"], root["algorithm"]};
  return packet; 
}

String serializeOutput(OutPacket packet) {
  char buffer[100];
  sprintf(buffer, "{\"id\"=%d,\"status\"=\"%s\"}", packet.id, packet.status.c_str());
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
  return "encrypted";
}

String ceasarDecrypt(String e, int key) {
  return "notimpl";
}

String transpositionEncrypt(String d, String key) {
  return "notimpl";
}

String transpositionDecrypt(String e, String key) {
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
  float fvalue = 21.0 + (float)(STEP * writeValue);
  myServo.write(180 - round(fvalue));
}

String waitForString() {
  String str = "";
  OutPacket ready {0, "ready."};
  Serial.println(serializeOutput(ready))
  while(Serial.available() == 0){;}
  str = Serial.readStringUntil('\n');
  return str;
}
#pragma endregion
