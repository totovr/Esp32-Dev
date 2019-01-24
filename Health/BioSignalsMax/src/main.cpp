#include <Arduino.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "BluetoothSerial.h"
#include <driver/adc.h>

BluetoothSerial SerialBT;
MAX30105 particleSensor;

// For GSR
// Declaration of functions
void GSRCalculation();
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

// BPM calculation
void BPMCalculation();
// Save previous state
float bpmPrevious = 60;
int bpmIntCurrent;
int bpmIntPrevious = 70;
int bpmThresholdUp;
int bpmThresholdLow;
// Variables used to calculate the BPM
const byte RATE_SIZE = 6; //Increase this for more averaging. 6 is good.
byte rates[RATE_SIZE];    //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
long irValue;
int beatAvg;
int beatPrevious;

// Temperature
float temperature = 27;

// For Calories Calculator
int CaloriesBurned(unsigned long _exerciseTime);
int weight = 72;
int metValue = 5; // https://sites.google.com/site/compendiumofphysicalactivities/Activity-Categories/conditioning-exercise
unsigned long exerciseTime;
int totalCalories;

void setup()
{
  Serial.begin(115200);
  SerialBT.begin("ESP32Emotion"); //Bluetooth device name

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1)
      ;
  }

  particleSensor.setup();                    //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0);  //Turn off Green LED
}

void loop()
{
  BPMCalculation();
  bpmIntCurrent = beatAvg;
  bpmThresholdUp = bpmIntPrevious + 10;
  bpmThresholdLow = bpmIntPrevious - 10;

  if (bpmIntCurrent != bpmIntPrevious && bpmIntCurrent > bpmThresholdLow && bpmIntCurrent < bpmThresholdUp)
  {
    int bpmAverage = (bpmIntCurrent + bpmIntPrevious) / 2;
    SerialBT.print("IR=");
    SerialBT.print(irValue);
    SerialBT.print(" , ");
    SerialBT.print("BPM=");
    SerialBT.print(beatsPerMinute);
    SerialBT.print(" , ");
    SerialBT.print(" Avg BPM=");
    SerialBT.print(bpmAverage);
    SerialBT.print(" , ");
    SerialBT.print("Temp=");
    SerialBT.println(temperature);
  } 

  bpmIntPrevious = bpmIntCurrent;
}

void BPMCalculation()
{

  irValue = particleSensor.getIR();
  // temperature = particleSensor.readTemperature();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE;                    //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++)
      {
        beatAvg += rates[x];
      }
      beatAvg /= RATE_SIZE;
    }
  }

  if (beatAvg > 220 || beatAvg < 59)
  {
    beatAvg = bpmPrevious;
  }

  // SerialBT.print("IR=");
  // SerialBT.print(irValue);
  // SerialBT.print(" , ");
  // SerialBT.print("BPM=");
  // SerialBT.print(beatsPerMinute);
  // SerialBT.print(" , ");
  // SerialBT.print(" Avg BPM=");
  // SerialBT.print(beatAvg);
  // SerialBT.print(" , ");
  // SerialBT.print("Temp=");
  // SerialBT.println(temperature);

  bpmPrevious = beatAvg;
}

