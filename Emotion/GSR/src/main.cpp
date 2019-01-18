#include <Arduino.h>
const int GSR = 34;
int sensorValue = 0;
int gsr_average = 0;
long sum = 0;

int userResistence = 0;

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  sum = 0;

  for (int i = 0; i < 10; i++) //Average the 10 measurements to remove the glitch
  {
    sensorValue = analogRead(GSR);
    sensorValue = map(sensorValue, 0,4095, 0, 1023);
    sensorValue = sensorValue - 184;
    sum += sensorValue;
    delay(5);
  }
  gsr_average = sum / 10;
  Serial.println(gsr_average);
}
