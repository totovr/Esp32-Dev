#include <Arduino.h>
#include <SparkFunTMP102.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

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
const unsigned long delayTime2 = 2700;
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;

// Save previous state
int bpmPrevious = 60;
int bpmIntCurrent;
int bpmIntPrevious;

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

  pinMode(GSRInput, INPUT);
  pinMode(BPMInput, INPUT);

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

  bpmIntCurrent = bpmInt;
  if (bpmIntCurrent != bpmIntPrevious && exerciseTime > 5000)
  {
    // Serial.print(bpmInt);
    // Serial.println(" BPM");

    // Serial.print(",");

    GSRCalculation();
    // Serial.print("User resistence ");
    // Serial.print(userResistence);

    // Serial.print(",");

    CalculateTemperature();
    // Serial.print("Temperature: ");
    // Serial.print(temperature);

    // Serial.print(",");

    totalCalories = CaloriesBurned(exerciseTime);
    // Serial.println(totalCalories);
    // Serial.println(" total calories");

    // Bluetooth
    SerialBT.print(bpmInt);
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
    sensorValue = analogRead(GSRInput);
    sensorValue = map(sensorValue, 0, 4095, 0, 1023);
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

  userResistence = ((1024 + 2 * gsr_average) * 10000) / (512 - gsr_average);
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