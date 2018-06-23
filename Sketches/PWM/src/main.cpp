#include <Arduino.h>

// Pin for ESP
uint8_t led = 32;

void setup() {
  Serial.begin(115200);
  // ledcSetup(ledChannel, freq, resolution);
  // channels 0-15, resolution 1-16 bits, freq limits depend on resolution
  // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
  ledcAttachPin(led, 1);
  ledcSetup(1, 12000, 8); // 12 kHz PWM, 8-bit resolution
}

void loop() {
  // ledcWrite(uint8_t channel, DutyCycle);
  ledcWrite(1, 255);
}
