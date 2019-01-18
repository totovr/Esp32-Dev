#include <Arduino.h>
#include <SparkFunTMP102.h>
#include "BluetoothSerial.h"
#include "DFRobot_Heartrate.h"

BluetoothSerial SerialBT;

// For GSR
// Declaration of functions
void GSRCalculation();
// Input PIN
const int GSRInput = 34;
// Variables
int sensorValue = 0;
int gsr_average = 0;
long sum = 0;
int userResistence = 0;

int gsrPreviousReading = 580;
int gsrCurrentReading;
int gsrThresholdUp;
int gsrThresholdLow;

// For BPM
void BPMCalculation();
#define heartRatePin 35
DFRobot_Heartrate heartrate(DIGITAL_MODE); ///< ANALOG_MODE or DIGITAL_MODE
uint8_t rateValue;
int bpmPrevious = 60; 
int BPM;
int bpmIntCurrent;
int bpmIntPrevious;
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

  pinMode(GSRInput, INPUT);

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

  bpmIntCurrent = BPM;
  bpmThresholdUp = bpmIntPrevious + 10;
  bpmThresholdLow = bpmIntPrevious - 10;

  if (bpmIntCurrent != bpmIntPrevious && bpmIntCurrent > bpmThresholdLow && bpmIntCurrent < bpmThresholdUp)
  {

    GSRCalculation();
    CalculateTemperature();
    totalCalories = CaloriesBurned(exerciseTime);

    // Bluetooth
    SerialBT.print(BPM);
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
    sensorValue = sensorValue - 194;
    gsrCurrentReading = sensorValue;

    gsrThresholdUp = gsrPreviousReading + 50;
    gsrThresholdLow = gsrPreviousReading - 50;

    if (gsrCurrentReading > 850 || gsrCurrentReading < gsrThresholdLow || gsrCurrentReading > gsrThresholdUp)
    {
      gsrCurrentReading = gsrPreviousReading;
    }

    sum += sensorValue;
    delay(5);
    gsrPreviousReading = gsrCurrentReading;
  }
  gsr_average = sum / 10;

  /*
  Human Resistance = ((1024+2*Serial_Port_Reading)*10000)/(512-Serial_Port_Reading), 
  unit is ohm, Serial_Port_Reading is the value display on Serial Port(between 0~1023)
  */

  int adjust = 512 - gsrCurrentReading;
  if (adjust == 0)
  {
    adjust = 1;
  }

  userResistence = ((1024 + 2 * gsrCurrentReading) * 10000) / (adjust);

  gsrPreviousReading = gsrCurrentReading;
}

void BPMCalculation()
{
  heartrate.getValue(heartRatePin);
  rateValue = heartrate.getRate(); ///< Get heart rate value

  if (rateValue > 220 || rateValue < 59)
  {
    rateValue = bpmPrevious;
  }

  if (rateValue)
  {
    // Serial.println(rateValue);
    BPM = rateValue;
    
  }
  delay(10);
  bpmPrevious = BPM;
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