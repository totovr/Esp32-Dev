/*
In this moment Im just considering when the threshold is over the first previous
threshold to calculate the new one
*/

#include <Arduino.h>

const int LED = 26;
const int GSR = 32;

// Thershold of the resistance of the user
int threshold = 0;
// Store the reading of a sample
int sensorValue;
// Store the sum of the readings of the sensor
long sumSensorValue = 0;
// 1 or 0 depending if the emotion was detected
int emotion = 0;
// Hold the difference of the threshold and the average of the readings
int temp = 0;
// Emotion theshold variables
int emotionThreshold = 0;
int sumEmotionThreshold = 0;
// Iterations for the samples and the calibration of the threshold
int iterations = 10;
int samplesThresholdCalibration = 10;
// Hold the last emotion threshold
int pastEmotionTreshold = 0;
bool calibrating = true;

// Sensitiviness
int sensitiviness = 100;

// Just to check if is better to take the average once is detected
int counterEmotions;

void setup()
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

    if (calibrating == true)
  {
    Serial.print("Calibrating");

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
    emotionThreshold = abs((sumEmotionThreshold / samplesThresholdCalibration) + sensitiviness);
    sumEmotionThreshold = 0;
    calibrating = false;
    digitalWrite(LED, LOW);
  }

}

void loop()
{

  for (int i = 0; i < iterations; i++)
  {

    // Reading the values
    sensorValue = analogRead(GSR);
    temp = threshold - sensorValue;

    // Format of the output

    // Serial.print(i);
    // Serial.print(",");
    //Serial.print(threshold);
    // Serial.print(",");
    // Serial.println(sensorValue);
    // Serial.print(",");
    Serial.print(abs(temp));
    Serial.print(",");
    Serial.println(emotionThreshold);

    // emotionThreslhold can be changed depending of the emotion that we want to measure
    if (abs(temp) > emotionThreshold)
    {
      //Serial.print(",");
      emotion = 1;
      //Serial.println(emotion);
      // Check if the accurary augment if we just take into account the emotion
      sumEmotionThreshold += abs(temp);
      counterEmotions = 1 + counterEmotions;
    }
    else
    {
      //Serial.print(",");
      emotion = 0;
      //Serial.println(emotion);
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
      for (int i = 0; i < iterations; i++)
      {
        sensorValue = analogRead(GSR);
        sumSensorValue += sensorValue;
        delay(5);
      }
      // Updated theshold
      threshold = sumSensorValue / iterations;
      // Updated emotion threshold
      emotionThreshold = abs((sumEmotionThreshold / counterEmotions) + sensitiviness); // When we use the emotion counter
      // Take the average of the new and the past emotion threshold
      emotionThreshold = (emotionThreshold + pastEmotionTreshold) / 2;

      sumEmotionThreshold = 0;
    }
  }
}
