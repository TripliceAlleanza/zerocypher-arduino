#include <Arduino.h>
#include <Servo.h>
#include <ArduinoJson.h>

#define SERVO_PIN 10

Servo myServo;

String waitForString();

void setup() {
  // put your setup code here, to run once:
  pinMode(SERVO_PIN, OUTPUT);
  myServo.attach(SERVO_PIN);
  myServo.write(0);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  // while(!Serial.available());
  // String json = waitForString();
  
  // Serial.println(json);
  // myServo.write(0);

  // for(int i = 700;i<=2300;i+=64) {
  //   myServo.writeMicroseconds(i);
  //   Serial.println(i);
  //   delay(500);
  // }
  myServo.writeMicroseconds(700);
  delay(2000);
  myServo.writeMicroseconds(2600);
  delay(2000);
}

String waitForString() {
  String str = "";
  while(Serial.available() > 0) {
    str += String(Serial.read());
    if(str=="\0") break;
  }
  return str;
}
