#include <Arduino.h>
#include "BluetoothSerial.h"

// Create one object of the BluetoothSerialClass
BluetoothSerial SerialBT;

// Pin for ESP
#define right_elbow 32
#define left_elbow 33
#define right_shoulder 26
#define left_shoulder 25

// Variable for the readings
int elbow_value_left     = 0;
int shoulder_value_left  = 2;
int elbow_value_right    = 4;
int shoulder_value_right = 6;

int c = 5;
 
void setup() {
  Serial.begin(115200);
 
  // Pin mode
  pinMode(left_shoulder, OUTPUT);
  pinMode(right_shoulder, OUTPUT);
  pinMode(left_elbow, OUTPUT);
  pinMode(right_elbow, OUTPUT);
  

  digitalWrite(left_shoulder, LOW);
  digitalWrite(right_shoulder, LOW);
  digitalWrite(left_elbow, LOW);
  digitalWrite(right_elbow, LOW);

  // This will stat the Bluetooth 
  SerialBT.begin("ESP32Jacket");

}
 
void loop() {
  
  // Start to read
  SerialBT.read();
  // Read the data and change it to an int value
  c = SerialBT.parseInt();

  if(c == 1) {
    digitalWrite(left_elbow, HIGH);
  } else if(c == 0) {
    digitalWrite(left_elbow, LOW);
  }
  if(c == 3) {
    digitalWrite(left_shoulder, HIGH);
  } else if(c == 2) {
    digitalWrite(left_shoulder, LOW);
  }
  if(c == 5) {
    digitalWrite(right_elbow, HIGH);
  } else if(c == 4) {
    digitalWrite(right_elbow, LOW);
  }
  if(c == 7) {
    digitalWrite(right_shoulder, HIGH);
  } else if(c == 6) {
    digitalWrite(right_shoulder, LOW);
  }

}
