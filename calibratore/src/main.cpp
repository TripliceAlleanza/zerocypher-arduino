#include <Arduino.h>
#include <Servo.h>
#include <math.h>

Servo myServo;

#define A_VALUE 25
#define Z_VALUE 175

const float STEP = (Z_VALUE - A_VALUE) / 25.0;


void writeLetter(char c) {
  int value = (int)c;
  int writeValue = value - 97;
  float fvalue = A_VALUE + (float)(STEP * writeValue);
  myServo.write(180 - round(fvalue));
}

String waitForString() {
  String str = "";
  while(Serial.available() == 0){;}
  str = Serial.readStringUntil('\n');
  Serial.println("String letta: " + str);
  return str;
}

void writeString(String str) {
  for(int i = 0; i<str.length(); ++i) {
    if((int)str[i] == 13) continue;
    writeLetter(str[i]);
    tone(3, 1000, 200);
    delay(1000);
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(10, OUTPUT);
  myServo.attach(10);
  myServo.write(180);
  Serial.begin(9600); 
}

void loop() {
  /*
  String s = waitForString();
  writeString(s);
  
  /*
  char current = 'a';
  while(current < 'z' + 1) {
    Serial.println(current);
    writeLetter(current);
    current++;
    tone(3, 1000, 200);
    delay(500);
  }
  */
  
  /*
  myServo.write(180 - A_VALUE);
  delay(500);
  myServo.write(180 - 0);
  delay(500);

  myServo.write(180 - Z_VALUE);
  delay(500);
  myServo.write(180 - 180);
  delay(500);
  */
  /*
  
  tone(3, 1000, 200);
  writeLetter(current);
  current++;
  delay(500);
  */
}