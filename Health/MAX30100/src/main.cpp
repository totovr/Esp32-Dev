#include <Arduino.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include "MAX30100.h"

// For temperature
MAX30100 sensor;
float temperature;
uint32_t tsTempSampStart;

// For hearth monitoring

#define REPORTING_PERIOD_MS 3000

// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;

uint32_t tsLastReport = 0;

// Callback (registered below) fired when a pulse is detected
// void onBeatDetected()
// {
//   Serial.println("Beat!");
// }

void setup()
{
  Serial.begin(115200);

  Serial.print("Initializing pulse oximeter..");

  if (!pox.begin())
  {
    Serial.println("FAILED");
    for (;;)
      ;
  }
  else
  {
    Serial.println("SUCCESS");
  }

  // Register a callback for the beat detection
  // pox.setOnBeatDetectedCallback(onBeatDetected);

  uint32_t tsTempSampStart = millis();
  Serial.print("Sampling die temperature..");
  sensor.startTemperatureSampling();
  while (!sensor.isTemperatureReady())
  {
    if (millis() - tsTempSampStart > 1000)
    {
      Serial.println("ERROR: timeout");
      // Stop here
      for (;;)
        ;
    }
  }

  temperature = sensor.retrieveTemperature();
  Serial.print("done, temp=");
  Serial.print(temperature);
  Serial.println("C");
}

void loop()
{

  // Make sure to call update as fast as possible
  pox.update();

  // Asynchronously dump heart rate and oxidation levels to the serial
  // For both, a value of 0 means "invalid"
  if (millis() - tsLastReport > REPORTING_PERIOD_MS)
  {
    Serial.print("Heart rate:");
    Serial.print(pox.getHeartRate());
    Serial.print("bpm / SpO2:");
    Serial.print(pox.getSpO2());
    Serial.println("%");

    tsLastReport = millis();
  }

}