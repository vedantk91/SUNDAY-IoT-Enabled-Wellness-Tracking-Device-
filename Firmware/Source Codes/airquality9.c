#include "airquality9.h"
#include "uart.h"
#include <util/delay.h>
#include <stdio.h>

// Additional definitions
#define ENS160_BOOTING_DELAY 10
#define ENS160_PART_ID 0x0160
#define ENS160_I2CADDR_0 0x52
#define ENS160_I2CADDR_1 0x53
#define ENS160_REG_PART_ID 0x00  // Part ID register address
void airquality9_init(airquality9_t *ctx, uint8_t slave_address) {
    ctx->slave_address = slave_address;
    // UART_putstring("Initializing I2C...\r\n");
    i2c_init(400000L); // Initialize I2C with desired speed, e.g., 400 kHz
    _delay_ms(ENS160_BOOTING_DELAY); // Delay for sensor initialization
    // UART_putstring("I2C Initialization complete. Air Quality Sensor Initialized\r\n");
    // Additional initialization steps if necessary
}

int airquality9_check_part_id(airquality9_t *ctx) {
    uint8_t part_id[2];
    i2c_start();
    i2c_putchar(ctx->slave_address << 1);
    i2c_putchar(ENS160_REG_PART_ID);
    i2c_start(); // Repeated start
    i2c_putchar((ctx->slave_address << 1) | 1); // Read operation
    part_id[0] = i2c_getchar_ack(); // Read lower byte with ACK
    part_id[1] = i2c_getchar_nack(); // Read higher byte with NACK
    i2c_stop();
    uint16_t combined_part_id = (part_id[1] << 8) | part_id[0];
    return (combined_part_id == ENS160_PART_ID);
}
int airquality9_set_operating_mode(airquality9_t *ctx, uint8_t opmode) {
    UART_putstring("Setting operating mode...\r\n");
    i2c_start();
    UART_putstring("I2C Start condition sent.\r\n");

    if (i2c_putchar(ctx->slave_address << 1) != 0) {
        UART_putstring("Error: Failed to send slave address for write.\r\n");
        return -1;
    }
    UART_putstring("Slave address sent.\r\n");

    if (i2c_putchar(AIRQUALITY9_REG_OPMODE) != 0) {
        UART_putstring("Error: Failed to send operating mode register address.\r\n");
        return -1;
    }
    UART_putstring("Operating mode register address sent.\r\n");

    if (i2c_putchar(opmode) != 0) {
        UART_putstring("Error: Failed to send operating mode data.\r\n");
        return -1;
    }
    UART_putstring("Operating mode data sent.\r\n");
    
    UART_putstring("I2C Stop condition sent. Operating mode set.\r\n");
    _delay_ms(100); // Add a delay to allow sensor to transition to the new mode
	    i2c_stop();

    return 0;
}

int airquality9_check_status(airquality9_t *ctx) {
    uint8_t status;
    i2c_start();
    i2c_putchar(ctx->slave_address << 1);
    i2c_putchar(AIRQUALITY9_REG_DATA_STATUS);
    i2c_start(); // Repeated start
    i2c_putchar((ctx->slave_address << 1) | 1); // Read operation
   status = i2c_getchar_nack(); // Read with NACK
    i2c_stop();
    // UART_putstring("Status checked. Status: 0x");
    // UART_puthex(status);
    // UART_putstring("\r\n");

    // Add more detailed status interpretation here
    if (status & 0x20) { // Replace SOME_STATUS_BIT with the actual bit mask for ready status
        // UART_putstring("Sensor is ready.\r\n");
        return 1; // Or some other appropriate value
    } else {
        // UART_putstring("Sensor not ready.\r\n");
        return 0; // Or some other appropriate value
}
}

int airquality9_read_aqi_uba(airquality9_t *ctx, uint8_t *aqi_uba) {
    // UART_putstring("Reading AQI UBA...\r\n");
    i2c_start();
    // UART_putstring("I2C Start condition sent for AQI UBA.\r\n");

    i2c_putchar(ctx->slave_address << 1);
    // UART_putstring("Slave address sent for AQI UBA.\r\n");

    i2c_putchar(AIRQUALITY9_REG_DATA_AQI);
    // UART_putstring("Data AQI register address sent.\r\n");

    i2c_start(); // Repeated start
    // UART_putstring("I2C Repeated Start condition sent for AQI UBA read.\r\n");

    i2c_putchar((ctx->slave_address << 1) | 1); // Read operation
    // UART_putstring("Slave address sent for reading AQI UBA.\r\n");

    *aqi_uba = i2c_getchar_nack(); // Read with NACK
    // UART_putstring("AQI UBA data received.\r\n");

    i2c_stop();
    // UART_putstring("I2C Stop condition sent after reading AQI UBA.\r\n");

    return 0;
}
