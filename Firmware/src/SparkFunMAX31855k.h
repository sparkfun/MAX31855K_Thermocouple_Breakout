/********************************** max31855k.h *********************************
 * Library to read temperature from a MAX6675 type K thermocouple digitizer   *
 *                                                                            *
 * SO output format:                                                          *
 * 16 bits                                                                    *
 * The first bit, D15, is a dummy sign bit and is always zero.                *
 * Bits D14–D3 contain the converted temperature in the order of MSB to LSB.  *
 * Bit D2 is normally low and goes high when the thermocouple input is open.  *
 * D1 is low to provide a device ID for the MAX6675 and bit D0 is tri-state.  *
 *                                                                            *
 * The method read_temp(max31855k::unit) takes a parameter which determines the *
 * unit of temperature returned.                                              *
 * max31855k::C will return degrees Celsius                                     *
 * max31855k::K will return degrees Kelvin                                      *
 * max31855k::F will return degrees Fahrenheit                                  *
 * max31855k::R will return degrees Rankine                                     *
 *                                                                            *
 *                                                                            *
 *****************************************************************************/
#ifndef _SPARKFUN_MAX31855K_h_
#define _SPARKFUN_MAX31855K_h_

#include <SPI.h> // Have to include this in the main sketch too... (Using SPI)

// Note: Need to define these somewhere. Doing so at the top of the main sketch.
//const uint8_t CHIP_SELECT_PIN = 10;
//const uint8_t VCC = 14;
//const uint8_t GND = 15;

class SparkFunMAX31855k
{
public:
  // Simple Arduino API style guide functions
  inline float readTempC() { return readTemp(SparkFunMAX31855k::C); } 
  inline float readTempF() { return readTemp(SparkFunMAX31855k::F); }
  inline float readTempR() { return readTemp(SparkFunMAX31855k::R); }
  inline float readTempK() { return readTemp(SparkFunMAX31855k::K); }

  // More advanced code concepts used below
  enum units {
    F, C, K, R
  };
  // Returns the temperature in degrees F, K, R, or C (default if unspecified)
  float readTemp(SparkFunMAX31855k::units _u=C);
  // Returns the cold junction temperature in ˚C
  float readCJT(void);

  SparkFunMAX31855k(const uint8_t, const uint8_t, const uint8_t);
  ~SparkFunMAX31855k() {} // User responsible 4 reassigning pins & stopping SPI
protected:
  union {
    uint8_t bytes[4];
    uint32_t uint32;
  } data;
  uint8_t cs;

  void readBytes(void);
  uint8_t checkHasFault(void);
  void select(void);
  void deselect(void);
};
 
#endif
