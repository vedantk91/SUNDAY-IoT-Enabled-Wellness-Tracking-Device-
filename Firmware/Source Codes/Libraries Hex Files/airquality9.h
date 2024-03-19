#ifndef AIRQUALITY9_H
#define AIRQUALITY9_H

#include <stdint.h>
#include "i2c.h"  // Include the new I2C header

// Define the I2C address and necessary registers
#define AIRQUALITY9_DEVICE_ADDRESS  0x53  // Adjust if needed
#define AIRQUALITY9_REG_OPMODE      0x10
#define AIRQUALITY9_REG_DATA_STATUS 0x20
#define AIRQUALITY9_REG_DATA_AQI    0x21
#define AIRQUALITY_STATUS_VALID_NORMAL_OP 0x00

// Define operating modes
#define AIRQUALITY9_OPMODE_STANDARD 0x02

typedef struct {
    uint8_t slave_address;
} airquality9_t;

// Function prototypes
void airquality9_init(airquality9_t *ctx, uint8_t slave_address);
int airquality9_set_operating_mode(airquality9_t *ctx, uint8_t opmode);
int airquality9_check_status(airquality9_t *ctx);
int airquality9_read_aqi_uba(airquality9_t *ctx, uint8_t *aqi_uba);

#endif // AIRQUALITY9_H
