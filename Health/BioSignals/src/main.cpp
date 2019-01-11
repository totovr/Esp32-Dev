#include <Arduino.h>
// #include "BluetoothSerial.h"

// For GSR
// Declaration of functions
void GSRCalculation();
// Input PIN
const int GSRInput = 35;
// Variables
int sensorValue = 0;
int gsr_average = 0;
long sum = 0;
int userResistence = 0;

// For BPM
// Declaration of functions
int myTimer1(long delayTime, long currentMillis);
int myTimer2(long delayTime2, long currentMillis);
void BPMCalculation();
// Input PIN
const int BPMInput = 34;

// Variables
int UpperThreshold = 518;
int LowerThreshold = 490;
int reading = 0;
float BPM = 0.0;
int bpmInt;
bool IgnoreReading = false;
bool FirstPulseDetected = false;
unsigned long FirstPulseTime = 0;
unsigned long SecondPulseTime = 0;
unsigned long PulseInterval = 0;

// Measure every 500 seconds
const unsigned long delayTime = 10;
const unsigned long delayTime2 = 3000;
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;

// Save previous state
int bpmPrevious = 60;
int bpmIntCurrent;
int bpmIntPrevious;

// For Calories Calculator
int CaloriesBurned(unsigned long _exerciseTime);
int age = 26;
int weight = 72;
int metValue = 5; // https://sites.google.com/site/compendiumofphysicalactivities/Activity-Categories/conditioning-exercise
unsigned long exerciseTime;
int totalCalories;

void setup()
{
  Serial.begin(115200);
  pinMode(GSRInput, INPUT);
  pinMode(BPMInput, INPUT);
}

void loop()
{
  exerciseTime = millis();

  // GSR sensor
  GSRCalculation();

  // Calculate beat pear minute
  // BPMCalculation();
  // bpmIntCurrent = bpmInt;
  // if (bpmIntCurrent != bpmIntPrevious)
  // {
  //   Serial.print(bpmInt);
  //   Serial.println(" BPM");

  //   // // Calculate calories burned
  //   // totalCalories = CaloriesBurned(exerciseTime);
  //   // Serial.print(totalCalories);
  //   // Serial.println(" total calories");
  // }

  bpmIntPrevious = bpmIntCurrent;
}

void GSRCalculation()
{
  sum = 0;

  for (int i = 0; i < 10; i++) //Average the 10 measurements to remove the glitch
  {
    sensorValue = analogRead(GSRInput);
    sensorValue = map(sensorValue, 0, 4095, 0, 1023);
    sensorValue = sensorValue - 140;
    sum += sensorValue;
    delay(5);
  }
  gsr_average = sum / 10;

  // Serial.print("GSR Average ");
  // Serial.println(gsr_average);

  /*
  Human Resistance = ((1024+2*Serial_Port_Reading)*10000)/(512-Serial_Port_Reading), 
  unit is ohm, Serial_Port_Reading is the value display on Serial Port(between 0~1023)
  */

  userResistence = ((1024+2*gsr_average)*10000)/(512-gsr_average);
  Serial.print("User resistence ");
  Serial.println(userResistence);
}

void BPMCalculation()
{
  // Get current time
  unsigned long currentMillis = millis();

  // First event
  if (myTimer1(delayTime, currentMillis) == 1)
  {

    // Raw reading of the sensor
    reading = analogRead(BPMInput);
    // Serial.println(reading);

    // The ESP32 has a ADC of 12 bits so map again like if it were of 10 bits
    reading = map(reading, 0, 4095, 0, 1023);

    // Heart beat leading edge detected.
    if (reading > UpperThreshold && IgnoreReading == false)
    {
      if (FirstPulseDetected == false)
      {
        FirstPulseTime = millis();
        FirstPulseDetected = true;
      }
      else
      {
        SecondPulseTime = millis();
        PulseInterval = SecondPulseTime - FirstPulseTime;
        FirstPulseTime = SecondPulseTime;
      }
      IgnoreReading = true;
    }

    // Heart beat trailing edge detected.
    if (reading < LowerThreshold && IgnoreReading == true)
    {
      IgnoreReading = false;
    }

    /*If you know the interval between heart beats, you can calculate the 
    frequency using the formula: frequency = 1 / time. Now if you take 
    the frequency and multiply it by 60, you should get BPM.*/

    // Calculate Beats Per Minute.
    BPM = (1.0 / PulseInterval) * 60.0 * 1000;
  }

  // Second event
  if (myTimer2(delayTime2, currentMillis) == 1)
  {
    // Serial.print(reading);
    // Serial.print("\t");
    // Serial.print(PulseInterval);
    // Serial.print("\t");

    if (BPM > 220 || BPM < 59)
    {
      BPM = bpmPrevious;
    }

    // Cast to int
    bpmInt = (int)BPM;
    //Serial.println(bpmInt);
    //Serial.flush();

    bpmPrevious = BPM;
  }
}

// First event timer
int myTimer1(long delayTime, long currentMillis)
{
  if (currentMillis - previousMillis >= delayTime)
  {
    previousMillis = currentMillis;
    return 1;
  }
  else
  {
    return 0;
  }
}

// Second event timer
int myTimer2(long delayTime2, long currentMillis)
{
  if (currentMillis - previousMillis2 >= delayTime2)
  {
    previousMillis2 = currentMillis;
    return 1;
  }
  else
  {
    return 0;
  }
}

int CaloriesBurned(unsigned long _exerciseTime)
{
  // https://www.cmsfitnesscourses.co.uk/blog/53/using-mets-to-calculate-calories
  unsigned long exerciseTimeMinutes = _exerciseTime / 60000;
  int calories = metValue * weight / 200 * exerciseTimeMinutes;
  return calories;
}