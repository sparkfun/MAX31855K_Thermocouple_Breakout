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
#ifndef _MAX31855K_h_
#define _MAX31855K_h_

#include <SPI.h> // Have to include this in the main sketch too... (Using SPI)

class max31855k
{
public:
  // Simple Arduino API style guide functions
  inline float readTempC() { return readTemp(max31855k::C); } 
  inline float readTempF() { return readTemp(max31855k::F); }
  inline float readTempR() { return readTemp(max31855k::R); }
  inline float readTempK() { return readTemp(max31855k::K); }

  // More advanced code concepts used below
  enum units {
    F, C, K, R
  };
  // Returns the temperature in degrees F, K, R, or C (default if unspecified)
  float readTemp(max31855k::units _u=C);
  // Returns the cold junction temperature in ˚C
  float readCJT(void);

  max31855k(uint8_t);
  ~max31855k() {} // User is responsible for reassigning pins and stopping SPI
protected:
  uint8_t cs;
  uint8_t bits31_24;
  uint8_t bits23_16;
  uint8_t bits15_8;
  uint8_t bits7_0;

  typedef enum {  // HotJunction_ColdJunction_Fault
    HOT_HOT_NONE,
    HOT_HOT_SCV,
    HOT_HOT_SCG,
    HOT_HOT_OC,
    HOT_ZERO_NONE,
    HOT_ZERO_SCV,
    HOT_ZERO_SCG,
    HOT_ZERO_OC,
    HOT_NEG_NONE,
    HOT_NEG_SCV,
    HOT_NEG_SCG,
    HOT_NEG_OC,
    ZERO_HOT_NONE,
    ZERO_HOT_SCV,
    ZERO_HOT_SCG,
    ZERO_HOT_OC,
    ZERO_ZERO_NONE,
    ZERO_ZERO_SCV,
    ZERO_ZERO_SCG,
    ZERO_ZERO_OC,
    ZERO_NEG_NONE,
    ZERO_NEG_SCV,
    ZERO_NEG_SCG,
    ZERO_NEG_OC,
    NEG_HOT_NONE,
    NEG_HOT_SCV,
    NEG_HOT_SCG,
    NEG_HOT_OC,
    NEG_ZERO_NONE,
    NEG_ZERO_SCV,
    NEG_ZERO_SCG,
    NEG_ZERO_OC,
    NEG_NEG_NONE,
    NEG_NEG_SCV,
    NEG_NEG_SCG,
    NEG_NEG_OC,
  } fake_condition_t;


  void readBytes(void);
  void readBytes(fake_condition_t);
  void select(void);
  void deselect(void);
};
 
#endif
