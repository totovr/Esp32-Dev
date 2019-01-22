#include <Arduino.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "BluetoothSerial.h"
#include <driver/adc.h>

BluetoothSerial SerialBT;
MAX30105 particleSensor;

// BPM calculation
void BPMCalculation();

const byte RATE_SIZE = 6; //Increase this for more averaging. 6 is good.
byte rates[RATE_SIZE];    //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;

// Temperature
float temperature = 27;

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
}

void BPMCalculation()
{
  long irValue = particleSensor.getIR();
  temperature = particleSensor.readTemperature();

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

  if (beatAvg > 59 || beatAvg < 220)
  {
    SerialBT.print("IR=");
    SerialBT.print(irValue);
    SerialBT.print(" Avg BPM=");
    SerialBT.print(beatAvg);
    SerialBT.print(" , ");
    SerialBT.print("Temp=");
    SerialBT.println(temperature);
  }
  else if (irValue < 106500)
  {
    SerialBT.println(" No finger?");
  }
}
