/******************************** max31855k.cpp ********************************
 * Library to read temperature from a MAX31855K type K thermocouple digitizer  *
 * Implementation of functions defined in max31855k.h                          *
 ******************************************************************************/

#include <Arduino.h>
#include <SPI.h>
#include "max31855k.h"
 
////////////////////////////////////////////////////////////////////////////////
// Description  : This constructor does the required setup
// Input        : uint8_t _cs: The pin number of the chip select line for this
//                instance
// Output       : Instance of this class
// Return       : None
// Usage        : max31855k <name>(<pinNumber>);
////////////////////////////////////////////////////////////////////////////////
max31855k::max31855k(uint8_t _cs) : cs(_cs)
{
  pinMode(cs, OUTPUT);

  // Setup two GPIOs as power and ground
  pinMode(9, OUTPUT); // VCC
  digitalWrite(9, HIGH);
  pinMode(8, OUTPUT); // GND
  digitalWrite(8, LOW);

  SPI.begin();
}

////////////////////////////////////////////////////////////////////////////////
// Deconstructor does nothing.  It's up to the user to re-assign
// chip select pin if they want to use it for something else.  We don't call
// SPI.end() in case there is another SPI device we don't want to kill.
////////////////////////////////////////////////////////////////////////////////
 
////////////////////////////////////////////////////////////////////////////////
// Description  : This function reads the cold junction temperature
// Input        : None
// Output       : Loads member variables with data from the IC
// Return:      : None
// Usage        : <objectName>.readBytes();
// MAX31855K Memory Map:
//  D[31:18] Signed 14-bit thermocouple temperature data
//  D17      Reserved: Always reads 0
//  D16      Fault: 1 when any of the SCV, SCG, or OC faults are active, else 0
//  D[15:4]  Signed 12-bit internal temperature
//  D3       Reserved: Always reads 0
//  D2       SCV fault: Reads 1 when thermocouple is shorted to V_CC, else 0
//  D1       SCG fault: Reads 1 when thermocouple is shorted to gnd, else 0
//  D0       OC  fault: Reads 1 when thermocouple is open-circuit, else 0
////////////////////////////////////////////////////////////////////////////////
void max31855k::readBytes(void)
{
  select();

  bits31_24 = SPI.transfer(0x00); // Read bits D32-D24
  bits23_16 = SPI.transfer(0x00); // Read bits D23-D16
  bits15_8  = SPI.transfer(0x00); // Read bits D15-D8
  bits7_0   = SPI.transfer(0x00); // Read bits D7-D0

  deselect();
 
  //TODO Remove this debug code
  Serial.print("\nRaw bits: 0b");
  Serial.print(bits31_24, BIN);
  Serial.print(" ");
  Serial.print(bits23_16, BIN);
  Serial.print(" ");
  Serial.print(bits15_8, BIN);
  Serial.print(" ");
  Serial.println(bits7_0, BIN);

  return;
}


////////////////////////////////////////////////////////////////////////////////
// Description  : This function pretends to read the cold junction temperature
// Input        : 
// Output       : Loads member variables with data from the IC
// Return:      : None
// Usage        : <objectName>.readBytes();
// MAX31855K Memory Map:
//  D[31:18] Signed 14-bit thermocouple temperature data
//  D17      Reserved: Always reads 0
//  D16      Fault: 1 when any of the SCV, SCG, or OC faults are active, else 0
//  D[15:4]  Signed 12-bit internal temperature
//  D3       Reserved: Always reads 0
//  D2       SCV fault: Reads 1 when thermocouple is shorted to V_CC, else 0
//  D1       SCG fault: Reads 1 when thermocouple is shorted to gnd, else 0
//  D0       OC  fault: Reads 1 when thermocouple is open-circuit, else 0
////////////////////////////////////////////////////////////////////////////////
void max31855k::readBytes(fake_condition_t type)
{
  // D[31:18] = 0b00111110100000 == 0x0FA0 == +1000.00˚C
  // D[31:18] = 0b00000000000000 == 0x0000 ==     0.00˚C
  // D[31:18] = 0b11110000011000 == 0x3C18 ==  -250.00˚C
  // D[15:4]  =   0b011111110000 == 0x7F0  ==  +127.00˚C
  // D[15:4]  =   0b000000000000 == 0x000  ==     0.00˚C
  // D[15:4]  =   0b110010010000 == 0xC90  ==   -55.00˚C
  const uint16_t hh = 0x0FA0;
  const uint16_t  z = 0x0000;
  const uint16_t ch = 0x3C18;
  const uint16_t hc = 0x07F0;
  const uint16_t cc = 0x0C90;
  // Fault bits 
  // 0b10000000000000100 == 0x = SCV Fault
  // 0b10000000000000010 == 0x = SCG Fault
  // 0b10000000000000001 == 0x = SCV Fault
  const uint8_t SCV = 0x04;
  const uint8_t SCG = 0x02;
  const uint8_t OC  = 0x01;

  switch (type)
  {
    case HOT_HOT_NONE:
      bits31_24 = hh >> 6;
      bits23_16 = hh << 2 | 0;
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | 0;
      break;
    case HOT_HOT_SCV:
      bits31_24 = hh >> 6;
      bits23_16 = hh << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | SCV;
      break;
    case HOT_HOT_SCG:
      bits31_24 = hh >> 6;
      bits23_16 = hh << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | SCG;
      break;
    case HOT_HOT_OC:
      bits31_24 = hh >> 6;
      bits23_16 = hh << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | OC;
      break;
    case HOT_ZERO_NONE:
      bits31_24 = hh >> 6;
      bits23_16 = hh << 2 | 0;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | 0;
      break;
    case HOT_ZERO_SCV:
      bits31_24 = hh >> 6;
      bits23_16 = hh << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | SCV;
      break;
    case HOT_ZERO_SCG:
      bits31_24 = hh >> 6;
      bits23_16 = hh << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | SCG;
      break;
    case HOT_ZERO_OC:
      bits31_24 = hh >> 6;
      bits23_16 = hh << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | OC;
      break;
    case HOT_NEG_NONE:
      bits31_24 = hh >> 6;
      bits23_16 = hh << 2 | 0;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | 0;
      break;
    case HOT_NEG_SCV:
      bits31_24 = hh >> 6;
      bits23_16 = hh << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | SCV;
      break;
    case HOT_NEG_SCG:
      bits31_24 = hh >> 6;
      bits23_16 = hh << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | SCG;
      break;
    case HOT_NEG_OC:
      bits31_24 = hh >> 6;
      bits23_16 = hh << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | OC;
      break;
    case ZERO_HOT_NONE:
      bits31_24 = z >> 6;
      bits23_16 = z << 2 | 0;
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | 0;
      break;
    case ZERO_HOT_SCV:
      bits31_24 = z >> 6;
      bits23_16 = z << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | SCV;
      break;
    case ZERO_HOT_SCG:
      bits31_24 = z >> 6;
      bits23_16 = z << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | SCG;
      break;
    case ZERO_HOT_OC:
      bits31_24 = z >> 6;
      bits23_16 = z << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | OC;
      break;
    case ZERO_ZERO_NONE:
      bits31_24 = z >> 6;
      bits23_16 = z << 2 | 0;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | 0;
      break;
    case ZERO_ZERO_SCV:
      bits31_24 = z >> 6;
      bits23_16 = z << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | SCV;
      break;
    case ZERO_ZERO_SCG:
      bits31_24 = z >> 6;
      bits23_16 = z << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | SCG;
      break;
    case ZERO_ZERO_OC:
      bits31_24 = z >> 6;
      bits23_16 = z << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | OC;
      break;
    case ZERO_NEG_NONE:
      bits31_24 = z >> 6;
      bits23_16 = z << 2 | 0;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | 0;
      break;
    case ZERO_NEG_SCV:
      bits31_24 = z >> 6;
      bits23_16 = z << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | SCV;
      break;
    case ZERO_NEG_SCG:
      bits31_24 = z >> 6;
      bits23_16 = z << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | SCG;
      break;
    case ZERO_NEG_OC:
      bits31_24 = z >> 6;
      bits23_16 = z << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | OC;
      break;
    case NEG_HOT_NONE:
      bits31_24 = ch >> 6;
      bits23_16 = ch << 2 | 0;
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | 0;
      break;
    case NEG_HOT_SCV:
      bits31_24 = ch >> 6;
      bits23_16 = ch << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | SCV;
      break;
    case NEG_HOT_SCG:
      bits31_24 = ch >> 6;
      bits23_16 = ch << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | SCG;
      break;
    case NEG_HOT_OC:
      bits31_24 = ch >> 6;
      bits23_16 = ch << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | OC;
      break;
    case NEG_ZERO_NONE:
      bits31_24 = ch >> 6;
      bits23_16 = ch << 2 | 0;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | 0;
      break;
    case NEG_ZERO_SCV:
      bits31_24 = ch >> 6;
      bits23_16 = ch << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | SCV;
      break;
    case NEG_ZERO_SCG:
      bits31_24 = ch >> 6;
      bits23_16 = ch << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | SCG;
      break;
    case NEG_ZERO_OC:
      bits31_24 = ch >> 6;
      bits23_16 = ch << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | OC;
      break;
    case NEG_NEG_NONE:
      bits31_24 = ch >> 6;
      bits23_16 = ch << 2 | 0;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | 0;
      break;
    case NEG_NEG_SCV:
      bits31_24 = ch >> 6;
      bits23_16 = ch << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | SCV;
      break;
    case NEG_NEG_SCG:
      bits31_24 = ch >> 6;
      bits23_16 = ch << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | SCG;
      break;
    case NEG_NEG_OC:
      bits31_24 = ch >> 6;
      bits23_16 = ch << 2 | 1;  // Set LSB to indicate fault
      bits15_8  = hc >> 4;
      bits7_0   = hc << 4 | OC;
      break;
    default:
      Serial.println("Error");
  }

  return;
}


////////////////////////////////////////////////////////////////////////////////
// Description  : This function reads the current temperature
// Input        : max31855k::units _u: The units of temperature to return.
//                C (default), K, R, or F
// Output       : Error messages before freezing
// Return:      : float: The temperature in requested units.
// Usage        : float tempC = <objectName>.read_temp();
//                float tempC = <objectName>.read_temp(max31855k::C);
//                float tempF = <objectName>.read_temp(max31855k::F);
//                float tempK = <objectName>.read_temp(max31855k::K);
//                float tempR = <objectName>.read_temp(max31855k::R);
////////////////////////////////////////////////////////////////////////////////
float max31855k::readTemp(max31855k::units _u)
{
  int16_t value;
  float temp;

  readBytes(NEG_HOT_NONE);

  if (bits23_16 & 1) { // Bit D16 is high => fault
    if (bits7_0 & 1) {
      Serial.println("\nMAX31855K::OC Fault: No Probe");
      while(1);
    } else if (bits7_0 & (1<<1)) {
      Serial.println("\nMAX31855K::SCG Fault: Thermocouple is shorted to GND");
      while(1);
    } else if (bits7_0 & (1<<2)) {
      Serial.println("\nMAX31855K::SCV Fault: Thermocouple is shorted to VCC");
      while(1);
    } else {
      Serial.println("\nMAX31855K::Unknown Fault");
      while(1);
    }
  } else {
    // Bits D[31:18] are the signed 14-bit thermocouple temperature value
    // 8 bits from D[31:24] & 6 bits from D[23:16]
    value = (bits31_24 << 6 | bits23_16 >> 2);
    // If negative fix bits
    if (value & 1<<13)
    {
      value = -((~value & 0x3FFF) + 1);
    } 
  }
 
  temp = value/4.0;
  switch (_u) {
  case F:
    temp = (temp * 9.0 / 5.0) + 32.0; 
    break;
  case K:
    temp += 273.15;
    break;
  case R:
    temp = (temp + 273.15) * 9.0 / 5.0;
  case C:
  default:
    break;
}
Serial.print("Temperature (˚C): ");
Serial.println(temp);
    
return temp;
}

////////////////////////////////////////////////////////////////////////////////
// Description  : This function reads the cold junction temperature
// Input        : None
// Output       : None
// Return:      : float: The temperature in ˚C
// Usage        : float tempC = <objectName>.readCJT();
////////////////////////////////////////////////////////////////////////////////
float max31855k::readCJT(void)
{
  readBytes(NEG_HOT_NONE);
  
  // D[15:4] is the 12-bit internal temperature
  // Combine the bits and then shift them into the lower 12 bits
  // TODO Does this cause a sign error? 
  uint16_t ret;
  ret = (bits15_8 << 4 | bits7_0 >> 4);
  // If negative fix bits
    if (ret & 1<<11)
    {
      ret = -((~ret & 0xFFF) + 1);
    } 
  return (float)ret/16.0;
}
 
////////////////////////////////////////////////////////////////////////////////
// This function selects the chip tied to the instance of this object
// Usage: select();
////////////////////////////////////////////////////////////////////////////////
void max31855k::select(void)
{
  // Take the chip select low to select the device:
  digitalWrite(cs, LOW);
  delayMicroseconds(250); // Let change happen
}
 
////////////////////////////////////////////////////////////////////////////////
// This function deselects the chip tied to the instance of this object
// Usage: deselect();
////////////////////////////////////////////////////////////////////////////////
void max31855k::deselect(void)
{
  digitalWrite(cs, HIGH);
}
