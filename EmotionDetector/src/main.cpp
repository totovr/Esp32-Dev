#include <Arduino.h>

const int LED = 26;
const int GSR = 32;
long sumSensorValue = 0;
int threshold = 0;
int sensorValue;
int emotion = 0;
int temp = 0;
int emotionThreshold = 0;
int sumEmotionThreshold = 0;
int iterations = 500;
int samplesThresholdCalibration = 500;
int pastEmotionTreshold = 0;

bool calibrating = true;

void setup()
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
}

void loop()
{

  if (calibrating == true)
  {
    digitalWrite(LED, HIGH);
    // This is a calibration of the threshold
    for (int i = 0; i < samplesThresholdCalibration; i++)
    {
      sensorValue = analogRead(GSR);
      sumSensorValue += sensorValue;
      delay(5);
    }
    // Calibrate threshold value
    threshold = sumSensorValue / samplesThresholdCalibration;

    // This is for the calibration of the emotion threshold
    for (int i = 0; i < samplesThresholdCalibration; i++)
    {
      sensorValue = analogRead(GSR);
      temp = threshold - sensorValue;
      delay(5);
      sumEmotionThreshold += abs(temp);
    }

    // Set emotion threshold
    emotionThreshold = abs(sumEmotionThreshold / samplesThresholdCalibration + 100);
    sumEmotionThreshold = 0;
    calibrating = false;
    digitalWrite(LED, LOW);
  }

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

    delay(50);

    // After some time the acquired value change so we have to calibrate again the threshold
    if (i == iterations - 1)
    {
      // Clean the varibles of the previous threshold
      threshold = 0;
      sumSensorValue = 0;

      // Save the current emotion threshold
      pastEmotionTreshold = emotionThreshold;

      // We will calibrate again the threshold to avoid the error in the acquisition data
      for (int i = 0; i < samplesThresholdCalibration; i++)
      {
        sensorValue = analogRead(GSR);
        sumSensorValue += sensorValue;
        delay(5);
      }
      // Updated theshold
      threshold = sumSensorValue / samplesThresholdCalibration;
      // Updated emotion threshold
      emotionThreshold = abs(sumEmotionThreshold / iterations + 50);
      emotionThreshold = (emotionThreshold + pastEmotionTreshold) / 2;
      sumEmotionThreshold = 0;
    }
  }
}
