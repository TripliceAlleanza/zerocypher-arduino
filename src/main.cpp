#include <Arduino.h>
#include <Servo.h>

#define SERVO_PIN 10

Servo myServo;

void setup() {
  // put your setup code here, to run once:
  pinMode(SERVO_PIN, OUTPUT);
  myServo.attach(SERVO_PIN);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

}