#include <Arduino.h>
const int GSR = 27;
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
    // sensorValue = sensorValue - 588;
    sum += sensorValue;
    delay(5);
  }
  gsr_average = sum / 10;

  Serial.print("GSR Average ");
  Serial.println(gsr_average);

  /*
  Human Resistance = ((1024+2*Serial_Port_Reading)*10000)/(512-Serial_Port_Reading), 
  unit is ohm, Serial_Port_Reading is the value display on Serial Port(between 0~1023)
  */

  // userResistence = ((1024+2*gsr_average)*10000)/(512-gsr_average);
  // Serial.print("User resistence ");
  // Serial.println(userResistence);

}
