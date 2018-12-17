#include <Arduino.h>
#include <Servo.h>
#include <ArduinoJson.h>

#define SERVO_PIN 10
#define STEP 7.2f

Servo myServo;

String waitForString();
void writeLetter(char);
void writeString(String);
String ceasarEncrypt(String,int);

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
  writeString(ceasarEncrypt(value, 1));
}

String ceasarEncrypt(String s, int key) {
  String newString = String();
  for(char c : s) {
    char nc = c+key;
    newString.concat(nc);
  }
  Serial.println(newString);
}

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
