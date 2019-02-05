#include <Arduino.h>

int pin = 36;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int w = analogRead(pin);
  Serial.println(w);
  delay(500);
}