#include <Arduino.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup()
{
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
}

void loop()
{

  SerialBT.println("Hello world");
  SerialBT.flush();
  delay(20);
}