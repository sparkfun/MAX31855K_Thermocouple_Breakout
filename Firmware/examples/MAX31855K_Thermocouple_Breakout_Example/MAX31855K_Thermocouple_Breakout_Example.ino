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
 *  VCC:  pin  9                                                               *
 *  SS:   pin 10                                                               *
 *  MOSI: pin 11 (NC)                                                          *
 *  MISO: pin 12                                                               *
 *  SCK:  pin 13                                                               *
 *  GND:  pin GND                                                              *
 *                                                                             *
 *                                                                             *
 * Development environment specifics:                                          *
 * 1.6.1                                                                       *
 * Arduino Pro Mini 328 3.3V/8MHz                                              *
 *                                                                             *
 * This code is beerware; if you see me (or any other SparkFun employee) at    *
 * the local, and you've found our code helpful, please buy us a round!        *
 * Distributed as-is; no warranty is given.                                    *
 ******************************************************************************/

#include <max31855k.h> // Using the max31855k driver
#include <SPI.h>       // Included here too due Arduino IDE; Used in max31855k.h

const uint8_t CHIPSELECTPIN = 10;

void setup() {
  Serial.begin(9600);
  Serial.println("\nBeginning...");
}

void loop() {
  // Select/activate the SPI device to talk to (only 1 in this case) 
  max31855k maxIC(CHIPSELECTPIN);

  Serial.print("CJT is (˚C): ");
  Serial.println(maxIC.readCJT());
  
  // Read the temperature in Celsius
  float temperature = maxIC.readTempC();
  Serial.print("Temp[C]=");
  Serial.print(temperature);
while(1);

  // Read the temperature in Fahrenheit
  temperature = maxIC.readTempF();
  Serial.print("\tTemp[F]=");
  Serial.println(temperature);

  delay(750);
}
