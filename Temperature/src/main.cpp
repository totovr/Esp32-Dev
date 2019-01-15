#include <Arduino.h>
#include <SparkFunTMP102.h>

TMP102 sensor0(0x48); // Initialize sensor at I2C address 0x48
// Sensor address can be changed with an external jumper to:
// ADD0 - Address
//  VCC - 0x49
//  SDA - 0x4A
//  SCL - 0x4B

float temperature;

void setup() {
  Serial.begin(9600); // Start serial communication at 9600 baud
  sensor0.begin();  // Join I2C bus

  // set the Conversion Rate (how quickly the sensor gets a new reading)
  //0-3: 0:0.25Hz, 1:1Hz, 2:4Hz, 3:8Hz
  sensor0.setConversionRate(3);

  //set Extended Mode.
  //0:12-bit Temperature(-55C to +128C) 1:13-bit Temperature(-55C to +150C)
  sensor0.setExtendedMode(0);

}

void loop()
{
  // Turn sensor on to start temperature measurement.
  sensor0.wakeup();

  // read temperature data
  temperature = sensor0.readTempC();


  // Place sensor in sleep mode to save power.
  // Current consumtion typically <0.5uA.
  sensor0.sleep();

  // Print temperature and alarm state
  Serial.print("Temperature: ");
  Serial.println(temperature);

  //delay(1000);  // Wait 1000ms
}