/***************** MAX31855K_Thermocouple_Breakout_Example.ino *****************
 *                                                                             *
 * MAX31855K Thermocouple Breakout Example Code                                *
 * brent.wilkins@sparkfun.com                                                  *
 * March 26th 2015                                                             *
 * https://github.com/sparkfun/MAX31855K_Thermocouple_Breakout                 *
 *                                                                             *
 * Use the "serial monitor" window to read a temperature sensor.               *
 *                                                                             *
 * Circuit:                                                                    *
 * MAX31855K breakout attached to the following pins                           *
 *  VCC:  pin 14                                                               *
 *  SS:   pin 10                                                               *
 *  MOSI: pin 12 (NC)                                                          *
 *  MISO: pin 11                                                               *
 *  SCK:  pin 13                                                               *
 *  GND:  pin 15                                                               *
 *                                                                             *
 *                                                                             *
 * Development environment specifics:                                          *
 * 1.6.3                                                                       *
 * Arduino Pro Mini 328 3.3V/8MHz                                              *
 *                                                                             *
 * This code is beerware; if you see me (or any other SparkFun employee) at    *
 * the local, and you've found our code helpful, please buy us a round!        *
 * Distributed as-is; no warranty is given.                                    *
 ******************************************************************************/

#include <SparkFunMAX31855k.h> // Using the max31855k driver
#include <SPI.h>       // Included here too due Arduino IDE; Used in max31855k.h

// Define SPI Arduino pin numbers for the Arduino Pro Mini
const uint8_t CHIP_SELECT_PIN = 10; // Using standard CS line (SS)
// SCK & MISO are defined by Arduiino
const uint8_t VCC = 14; // Powering board straight from Arduino Pro Mini
const uint8_t GND = 15;

// Instantiate an instance of the SparkFunMAX31855k class
SparkFunMAX31855k probe(CHIP_SELECT_PIN, SCK, MISO, VCC, GND);

void setup() {
  Serial.begin(9600);
  Serial.println("\nBeginning...");
}

void loop() {
  probe.readBytes();
/*
  Serial.print("CJT is (˚C): ");
  Serial.println(probe.readCJT());
  
  // Read the temperature in Celsius
  float temperature = probe.readTempC();
  Serial.print("Temp[C]=");
  Serial.print(temperature);

  // Read the temperature in Fahrenheit
  temperature = probe.readTempF();
  Serial.print("\tTemp[F]=");
  Serial.println(temperature);
*/

  delay(750);
}
