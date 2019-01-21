#include <Arduino.h>
#include <SparkFunTMP102.h>
#include "BluetoothSerial.h"
#include <driver/adc.h>
#include "DFRobot_Heartrate.h"

BluetoothSerial SerialBT;

// For GSR
// Declaration of functions
void GSRCalculation();
// int GSRCalibration();
// Input PIN
// const int GSRInput = 34;
// Variables
int sensorValue = 0;
int gsr_average = 0;
long sum = 0;
long userResistence = 0;

int gsrPreviousReading = 520;
int gsrCurrentReading;
int gsrThresholdUp;
int gsrThresholdLow;

// For BPM
// Declaration of functions
int myTimer1(long delayTime, long currentMillis);
int myTimer2(long delayTime2, long currentMillis);
void BPMCalculation();

// Input PIN
const int BPMInput = 33;

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

// Measure every 2700 seconds
const unsigned long delayTime = 10;
const unsigned long delayTime2 = 4000;
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;

// Save previous state
float bpmPrevious = 60;
int bpmIntCurrent;
int bpmIntPrevious = 70;
int bpmThresholdUp;
int bpmThresholdLow;

// For Calories Calculator
int CaloriesBurned(unsigned long _exerciseTime);
int weight = 72;
int metValue = 5; // https://sites.google.com/site/compendiumofphysicalactivities/Activity-Categories/conditioning-exercise
unsigned long exerciseTime;
int totalCalories;

// For Temperature
void CalculateTemperature();
TMP102 sensor0(0x48); // Initialize sensor at I2C address 0x48
float temperature;

void setup()
{
  Serial.begin(115200);

  // ADC Configuration
  adc1_config_width(ADC_WIDTH_BIT_10); //Range 0-1023

  // GSR Setup
  adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11); //ADC_ATTEN_DB_11 = 0-3,6V

  // HeartSensor
  adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11); //ADC_ATTEN_DB_11 = 0-3,6V

  // Temperature sensor setup
  sensor0.begin(); // Join I2C bus
  // set the Conversion Rate (how quickly the sensor gets a new reading)
  //0-3: 0:0.25Hz, 1:1Hz, 2:4Hz, 3:8Hz
  sensor0.setConversionRate(3);
  //set Extended Mode.
  //0:12-bit Temperature(-55C to +128C) 1:13-bit Temperature(-55C to +150C)
  sensor0.setExtendedMode(0);

  SerialBT.begin("ESP32Emotion"); //Bluetooth device name
}

void loop()
{
  exerciseTime = millis();

  BPMCalculation(); //Calculate beat pear minute

  // SerialBT.print(bpmInt);
  // SerialBT.print(" , ");

  GSRCalculation();
  // SerialBT.print(gsr_average);
  // SerialBT.print(" , ");
  // SerialBT.print(userResistence);

  CalculateTemperature();
  // SerialBT.print(" , ");
  // SerialBT.print(temperature);

  totalCalories = CaloriesBurned(exerciseTime);
  // SerialBT.print(" , ");
  // SerialBT.println(totalCalories);

  bpmIntCurrent = bpmInt;
  bpmThresholdUp = bpmIntPrevious + 10;
  bpmThresholdLow = bpmIntPrevious - 10;

  // SerialBT.print(bpmIntCurrent);
  // SerialBT.print(" , ");
  // SerialBT.print(bpmThresholdUp);
  // SerialBT.print(" , ");
  // SerialBT.print(bpmThresholdUp);
  // SerialBT.print(" , ");
  // SerialBT.println(bpmThresholdLow);

  if (bpmIntCurrent != bpmIntPrevious && bpmIntCurrent > bpmThresholdLow && bpmIntCurrent < bpmThresholdUp)
  {

    // Bluetooth

    SerialBT.print(bpmIntCurrent);
    SerialBT.print(",");
    SerialBT.print(userResistence);
    SerialBT.print(",");
    SerialBT.print(temperature);
    SerialBT.print(",");
    SerialBT.println(totalCalories);
  }

  bpmIntPrevious = bpmIntCurrent;
}

void GSRCalculation()
{

  sum = 0;

  for (int i = 0; i < 10; i++) //Average the 10 measurements to remove the glitch
  {
    sensorValue = adc1_get_raw(ADC1_CHANNEL_4); //Read analog
    gsrCurrentReading = sensorValue;

    gsrThresholdUp = gsrPreviousReading + 50;
    gsrThresholdLow = gsrPreviousReading - 50;

    if (gsrCurrentReading > 800 || gsrCurrentReading < gsrThresholdLow || gsrCurrentReading > gsrThresholdUp)
    {
      gsrCurrentReading = gsrPreviousReading;
    }

    sum += gsrCurrentReading;
    delay(5);
    gsrPreviousReading = gsrCurrentReading;
  }

  gsr_average = sum / 10;

  /*
  Human Resistance = ((1024+2*Serial_Port_Reading)*10000)/(512-Serial_Port_Reading), 
  unit is ohm, Serial_Port_Reading is the value display on Serial Port(between 0~1023)
  */

  int adjust = 512 - gsr_average;
  if (adjust == 0)
  {
    adjust = 1;
  }

  userResistence = ((1024 + 2 * gsr_average) * 10000) / (adjust);
}

void BPMCalculation()
{
  // Get current time
  unsigned long currentMillis = millis();

  // First event
  if (myTimer1(delayTime, currentMillis) == 1)
  {

    // Raw reading of the sensor
    // reading = analogRead(BPMInput);
    reading = adc1_get_raw(ADC1_CHANNEL_5); //Read analog

    // The ESP32 has a ADC of 12 bits so map again like if it were of 10 bits
    // reading = map(reading, 0, 4095, 0, 1023);

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

    if (BPM > 220 || BPM < 59)
    {
      BPM = bpmPrevious;
    }

    // Cast to int
    bpmInt = (int)BPM;

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

void CalculateTemperature()
{
  // Turn sensor on to start temperature measurement.
  sensor0.wakeup();

  // read temperature data
  temperature = sensor0.readTempC();

  // Place sensor in sleep mode to save power.
  // Current consumtion typically <0.5uA.
  sensor0.sleep();
}