#include "as7262.h"
#include "i2c.h"
#include "uart.h"
#include <util/delay.h> 
#include <string.h>
#include <float.h>
// Define additional macros and registers based on the Adafruit implementation
#define AS726X_HW_VERSION 0x00
#define AS726X_CONTROL_SETUP 0x04
#define AS726X_INT_T 0x05
#define AS726X_LED_CONTROL 0x07

#define AS726X_SLAVE_WRITE_REG 0x01
#define AS726X_SLAVE_READ_REG 0x02

#define AS726X_SLAVE_TX_VALID 0x02
#define AS726X_SLAVE_RX_VALID 0x01

// Define constants for initialization
#define AS726X_DEVICE_TYPE 0x40
#define AS726X_HW_VERSION_REG 0x00

// Function declarations
uint8_t virtualRead(uint8_t virtualAddr);
void virtualWrite(uint8_t virtualAddr, uint8_t dataToWrite);
void initiateOneShotMode(void);
uint8_t virtualRead(uint8_t virtualAddr);
void virtualWrite(uint8_t virtualAddr, uint8_t dataToWrite);
uint8_t waitForDataReady(void);
float as7262_readCalibratedViolet() {
    return as7262_readCalibratedValue(AS7262_V_CAL);
}

float as7262_readCalibratedBlue() {
    return as7262_readCalibratedValue(AS7262_B_CAL);
}

float as7262_readCalibratedGreen() {
    return as7262_readCalibratedValue(AS7262_G_CAL);
}

float as7262_readCalibratedYellow() {
    return as7262_readCalibratedValue(AS7262_Y_CAL);
}

float as7262_readCalibratedOrange() {
    return as7262_readCalibratedValue(AS7262_O_CAL);
}

float as7262_readCalibratedRed() {
    return as7262_readCalibratedValue(AS7262_R_CAL);
}

void initiateOneShotMode(void) {
    // Set to one-shot mode (BANK Mode 3)
    virtualWrite(AS726X_CONTROL_SETUP, 0x08); 
    // UART_putstring("Setting to One-Shot Mode.\n");
    _delay_ms(100); 

    // Read back the mode to confirm
    uint8_t mode = virtualRead(AS726X_CONTROL_SETUP);
    char modeStr[50];
    // sprintf(modeStr, "Current Mode (After Setting): 0x%02X\n", mode);
    UART_putstring(modeStr);

    // Wait for data to be ready
    uint8_t status;
    do {
        status = virtualRead(AS726X_CONTROL_SETUP);
        _delay_ms(100); // Polling delay
    } while (!(status & 0x02)); // Check DATA_RDY bit
}


// Initialization function adapted from Adafruit's begin() method
int as7262_init() {
    UART_putstring("AS7262 Initialization Start.\n");

    // Initialize I2C at 100kHz
    i2c_init(100000UL); 
    UART_putstring("I2C Initialized.\n");

    // Perform a soft reset
    virtualWrite(AS726X_CONTROL_SETUP, 0x80); // Set RST bit to 1 to initiate a soft reset
    _delay_ms(1000); // Wait for the reset to complete, according to the datasheet

    // Check hardware version to ensure we can communicate with the device
    uint8_t version = virtualRead(AS726X_HW_VERSION_REG);
    if(version != AS726X_DEVICE_TYPE) {
        UART_putstring("AS7262 Hardware Version Mismatch.\n");
        return -1; // Hardware version mismatch
    }

    // Configure LED control register
    virtualWrite(AS726X_LED_CONTROL, 0x00); // All LEDs off initially

    // Set the measurement mode to continuous reading
    // virtualWrite(AS726X_CONTROL_SETUP, 0x02); // Set BANK bits to continuous reading mode
    // virtualWrite(AS726X_INT_T, 0xFF);
    UART_putstring("AS7262 Initialization Complete.\n");
    return 0; // Initialization successful
}
// Virtual register read operation
uint8_t virtualRead(uint8_t virtualAddr) {
    uint8_t status;
    
    // Wait for the data to be ready
    do {
        status = i2c_read_byte(AS7262_DEVICE_ADDRESS, AS726X_SLAVE_STATUS_REG);
    } while((status & AS726X_SLAVE_TX_VALID) != 0);
    
    // Write the address
    i2c_write(AS7262_DEVICE_ADDRESS, 1, AS726X_SLAVE_WRITE_REG, &virtualAddr);
    
    // Wait for the data to be ready
    do {
        status = i2c_read_byte(AS7262_DEVICE_ADDRESS, AS726X_SLAVE_STATUS_REG);
    } while((status & AS726X_SLAVE_RX_VALID) == 0);
    
    uint8_t data;
    i2c_read(AS7262_DEVICE_ADDRESS, 1, AS726X_SLAVE_READ_REG, &data);
    
    return data;
}

// Virtual register write operation
void virtualWrite(uint8_t virtualAddr, uint8_t dataToWrite) {
    uint8_t status;
    
    // Wait for the device to be ready
    do {
        status = i2c_read_byte(AS7262_DEVICE_ADDRESS, AS726X_SLAVE_STATUS_REG);
    } while((status & AS726X_SLAVE_TX_VALID) != 0);
    
    // Write the address
    virtualAddr |= 0x80; // Set the write bit
    i2c_write(AS7262_DEVICE_ADDRESS, 1, AS726X_SLAVE_WRITE_REG, &virtualAddr);
    
    // Wait for the device to be ready
    do {
        status = i2c_read_byte(AS7262_DEVICE_ADDRESS, AS726X_SLAVE_STATUS_REG);
    } while((status & AS726X_SLAVE_TX_VALID) != 0);
    
    // Write the data
    i2c_write(AS7262_DEVICE_ADDRESS, 1, AS726X_SLAVE_WRITE_REG, &dataToWrite);
}

uint8_t waitForDataReady(void) {
    uint8_t status;
    // Wait for DATA_RDY bit to be set
    do {
        status = virtualRead(AS726X_CONTROL_SETUP);
       // _delay_ms(15); // Polling delay
    } while (!(status & 0x02));
    return status & 0x02; // return DATA_RDY status bit
}

uint16_t as7262_readSpectralValue(uint8_t highAddr, uint8_t lowAddr) {
    // Wait for data to be ready
    if(!waitForDataReady()) {
        UART_putstring("Data not ready for spectral value read.\n");
        return 0; // Data not ready
    }

    // Now we can read the high and low bytes
    uint16_t value = 0;
    uint8_t highByte = virtualRead(highAddr);
    uint8_t lowByte = virtualRead(lowAddr);
    value = (highByte << 8) | lowByte;
    return value;
}

// Function to read a 32-bit calibrated value from the sensor
float as7262_readCalibratedValue(uint8_t calAddressHigh) {
    // Wait for data to be ready
    // if (!waitForDataReady()) {
    //     // UART_putstring("Calibrated data not ready for reading.\n");
    //     return -1.0f; // Indicate an error
    // }

    uint8_t buffer[4];
    for (int i = 0; i < 4; i++) {
        buffer[i] = virtualRead(calAddressHigh + i);
    }

    // Debugging: Print raw byte values
    // UART_putstring("Raw bytes: ");
    // for (int i = 0; i < 4; i++) {
    //     char byteStr[10];
    //     sprintf(byteStr, "%02x ", buffer[i]);
    //     UART_putstring(byteStr);
    // }
    // // UART_putstring("\n");

    // Adjusting byte order for Little Endian
    float value;
    uint8_t temp;
    // Swap buffer[0] and buffer[3]
    temp = buffer[0];
    buffer[0] = buffer[3];
    buffer[3] = temp;
    // Swap buffer[1] and buffer[2]
    temp = buffer[1];
    buffer[1] = buffer[2];
    buffer[2] = temp;

    // Copying the bytes into the float
    memcpy(&value, buffer, sizeof(value));

    // Debug: Print the converted float value
    char valueStr[32];
    // sprintf(valueStr, "Converted float: %f\n", value);
    // UART_putstring(valueStr);

    return value;
}
