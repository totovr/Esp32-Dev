#include <Arduino.h>
#include <driver/adc.h>

int sensorValue = 0;
float gsr_average = 0;
long sum = 0;
int w = 0;

int userResistence = 0;

void setup()
{
  Serial.begin(115200);

  // adc1_config_width(ADC_WIDTH_BIT_10);                        //Range 0-1023
  // adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11); //ADC_ATTEN_DB_11 = 0-3,6V
}

void loop()
{
  while (w < 100)
  {
    sum = 0;

    for (int i = 0; i < 10; i++) //Average the 10 measurements to remove the glitch
    {
      // sensorValue = adc1_get_raw(ADC1_CHANNEL_4);
      sensorValue = analogRead(32);
      sum += sensorValue;
      delay(5);
    }
    gsr_average = sum / 10;
    /*
  Human Resistance = ((1024+2*Serial_Port_Reading)*10000)/(512-Serial_Port_Reading), 
  unit is ohm, Serial_Port_Reading is the value display on Serial Port(between 0~1023)
  */

    float voltage = (gsr_average * 5) / 4096;

    // Print results

    // Serial.print(gsr_average);
    // Serial.print(" , ");
    Serial.println(voltage);
    w++;
  }
}
