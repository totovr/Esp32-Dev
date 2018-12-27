#include <Arduino.h>

const int LED = 26;
const int GSR = 32;
long sum = 0;
int threshold = 0;
int sensorValue;
int emotion = 0;
int temp;
int emotionThreshold = 240;
int sumEmotionThreshold = 0;
int iterations = 800;

void setup()
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  // This is a calibration
  for (int i = 0; i < 500; i++)
  {
    sensorValue = analogRead(GSR);
    sum += sensorValue;
    delay(5);
  }
  threshold = sum / 500;
}

void loop()
{
  // Serial.print("Num Sample,");
  // Serial.print("Sensor value,");
  // Serial.print("Threshold");
  // Serial.println("temp");

  for (int i = 0; i < iterations; i++)
  {
    Serial.print(i);

    Serial.print(",");

    Serial.print(threshold);

    Serial.print(",");
    sensorValue = analogRead(GSR);

    Serial.print(sensorValue);

    temp = threshold - sensorValue;

    Serial.print(",");
    Serial.print(abs(temp));

    Serial.print(",");
    Serial.print(emotionThreshold);
    
    // Used to take the average of the differences of the threshold and the sumatory of the sensor values
    sumEmotionThreshold += abs(temp);

    // emotionThreslhold can be changed depending of the emotion that we want to measure 
    if (abs(temp) > emotionThreshold)
    {
      Serial.print(",");
      emotion = 1;
      Serial.println(emotion);
    }
    else
    {
      Serial.print(",");
      emotion = 0;
      Serial.println(emotion);
    }

    // After some time the acquired value change so we have to calibrate again the threshold
    if (i == 799)
    {
      threshold = 0;
      sum = 0;

      for (int i = 0; i < 500; i++)
      {
        sensorValue = analogRead(GSR);
        sum += sensorValue;
        delay(5);
      }
      threshold = sum / 500;
      emotionThreshold = sumEmotionThreshold / 500;
      sumEmotionThreshold = 0;
    }
  }
}
