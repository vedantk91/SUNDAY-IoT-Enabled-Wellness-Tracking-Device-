#ifndef AS7262_H
#define AS7262_H

#include <stdint.h>
#include <string.h>
#include <float.h>
// Register Addresses
#define AS7262_CONTROL_SETUP       0x04
#define AS7262_INT_T               0x05
#define AS7262_DEVICE_ADDRESS      0x49

#define AS7262_V_HIGH              0x08
#define AS7262_V_LOW               0x09
#define AS7262_B_HIGH              0x0A
#define AS7262_B_LOW               0x0B
#define AS7262_G_HIGH              0x0C
#define AS7262_G_LOW               0x0D
#define AS7262_Y_HIGH              0x0E
#define AS7262_Y_LOW               0x0F
#define AS7262_O_HIGH              0x10
#define AS7262_O_LOW               0x11
#define AS7262_R_HIGH              0x12
#define AS7262_R_LOW               0x13

// Virtual Register Addresses
#define AS726X_SLAVE_STATUS_REG    0x00
#define AS726X_SLAVE_WRITE_REG     0x01
#define AS726X_SLAVE_READ_REG      0x02

// Status Register Bit Masks
#define AS726X_SLAVE_TX_VALID      0x02
#define AS726X_SLAVE_RX_VALID      0x01
#define AS726X_CONTROL_SETUP 0x04
// Control Setup Masks for Desired Operations
// Add the actual values from the datasheet for these definitions

#define AS7262_V_CAL  0x14
#define AS7262_B_CAL  0x18
#define AS7262_G_CAL  0x1C
#define AS7262_Y_CAL  0x20
#define AS7262_O_CAL  0x24
#define AS7262_R_CAL  0x28


#define AS7262_SOFT_RESET 0x80
#define AS7262_CONTINUOUS_READING 0x00

// Function Prototypes
int as7262_init();
uint8_t as7262_readRegister(uint8_t reg);
int as7262_writeRegister(uint8_t reg, uint8_t value);
uint16_t as7262_readSpectralValue(uint8_t highReg, uint8_t lowReg);
float as7262_readCalibratedValue(uint8_t calAddressHigh);
float as7262_readCalibratedViolet();
float as7262_readCalibratedBlue();
float as7262_readCalibratedGreen();
float as7262_readCalibratedYellow();
float as7262_readCalibratedOrange();
float as7262_readCalibratedRed();
void virtualWrite(uint8_t virtualAddr, uint8_t dataToWrite);
void initiateOneShotMode(void);
#endif /* AS7262_H */
