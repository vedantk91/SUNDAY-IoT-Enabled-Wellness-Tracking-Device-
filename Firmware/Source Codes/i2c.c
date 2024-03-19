

#include <avr/io.h>
#include <util/twi.h>

#define MAX_RESTARTS 20


void i2c_init(unsigned long i2c_speed)
{

  DDRC &= 0b11001111; /* make SDA and SCL inputs */
  PORTC &= 0b11001111;/* disable internal pullups on SDA and SCL */
  
  TWSR = 0; /*clear the prescaler bits: prescale = 1 */

#if F_CPU < 1600000UL
  TWBR = 0;
#else
  TWBR = ((F_CPU / i2c_speed) - 16) / 2;
#endif

  TWCR = _BV(TWEN); /* enable the TWI */

}/* end i2c_init() */

/*
 * i2c_start()
 *
 * Generate a Start condition on the I2C bus.  Returns an error code if the bus 
 * is not idle.
 */
uint8_t i2c_start(void)
{

  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* send start condition */

  while ((TWCR & _BV(TWINT)) == 0) /* wait for transmission */
  {
  }   
  
  switch(TW_STATUS)
  {
    case TW_START:
    case TW_REP_START:
      return(0);        /* START condition was accepted, no error */

    default:
      return(1);  /* bus collision, return error code */
  }
}/* end i2c_start() */

/*
 * i2c_stop()
 *
 * Generate a Stop condition on the I2C bus.
 */

void i2c_stop(void){

  TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* send stop condition */

  while (TWCR & _BV(TWSTO)) /* wait for stop to be applied */
  {
  }

}/* end i2c_stop() */

/*
 * i2c_putchar()
 *
 * Put a byte on the I2C bus.  Returns an error code if the bus is not idle.
 */
uint8_t i2c_putchar(uint8_t c)
{

  TWDR = c;
  TWCR = _BV(TWINT) | _BV(TWEN);

  while ((TWCR & _BV(TWINT)) == 0)
  {
  }

  switch(TW_STATUS)
  {
    case TW_MT_SLA_ACK:
    case TW_MT_DATA_ACK:
    case TW_MR_SLA_ACK:
    case TW_MR_DATA_ACK:
      return(0);

    case TW_BUS_ERROR:
    case TW_MT_SLA_NACK:
    case TW_MT_DATA_NACK:
    case TW_MT_ARB_LOST:
    case TW_MR_SLA_NACK:
    case TW_MR_DATA_NACK:
	    return(1);
		
    default:
      return(2);

  }/* end switch(TW_STATUS) */

}/* end i2c_putchar() */

/*
 * i2c_getchar_ack()
 *
 * Receive a byte from then put an Acknowledge on the I2C bus.
 */
uint8_t i2c_getchar_ack(void)
{

  TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN);

  while ((TWCR & _BV(TWINT)) == 0)
  {
  }

  return(TWDR);

}/* end i2c_getchar_ack() */

/*
 * i2c_getchar_nack()
 *
 * Receive a byte from then put a Not Acknowledge on the I2C bus.
 */
uint8_t i2c_getchar_nack(void)
{

  TWCR = _BV(TWINT) | _BV(TWEN);

  while ((TWCR & _BV(TWINT)) == 0)
  {
  }

  return(TWDR);

}/* end i2c_getchar_nack() */


uint8_t i2c_write(uint8_t SlvAdrs, /* device slave address */
                  uint8_t len,     /* number of bytes to read */
                  uint8_t adrs,    /* device register to start reading from */
                  uint8_t *buf)    /* RAM address of where to put read bytes */
{

  uint8_t restarts = MAX_RESTARTS;

/* do nothing if there is no data to write */
  if(len <= 0)
  {
    return(0);
  }

WriteRestart:
/* Loop here until the bus accepts Start condition */

  if(i2c_start())
  {
    if(restarts-- == 0)
    {
      i2c_stop();
      return(1);
    }
    goto WriteRestart;
  }


  if(i2c_putchar((SlvAdrs<<1)|TW_WRITE))
  {
    if(restarts-- == 0)
    {
      i2c_stop();
      return(2); /* count exceeded, exit with an error code */

    } /* end if(Restarts-- == 0) */

    goto WriteRestart; /* count not exceeded, retry */

  }/* end if(i2c_putchar((SlvAdrs<<1)|TW_WRITE)) */

/* Write the byte address to the slave device */
  if(i2c_putchar(adrs))
  {
    return(3);
  }

/* Write the data from the buffer to the slave device */
  for(; len > 0; len--)
  {
    if(i2c_putchar(*buf++))
    {
      return(4);
    }      
  }/* end for(; len > 0; len--) */

/* Apply a Stop condition on the bus */

  i2c_stop();

  return(0);
  
}/* end i2c_write() */


uint8_t i2c_read(uint8_t SlvAdrs, /* device slave address */
                 uint8_t len,     /* number of bytes to read */
                 uint8_t adrs,    /* device register to start reading from */
                 uint8_t *buf)    /* RAM address of where to put read bytes */
{
  uint8_t restarts = MAX_RESTARTS;

/* do nothing if there is no data to read */
  if(len == 0)
  {
    return(0);
  }

ReadRRestart:
/* Loop here until the bus accepts Start condition */

  if(i2c_start())
  {
    if(--restarts == 0)
    {
      return(1);
    }/* end if(--restarts == 0) */

    goto ReadRRestart;

  }/* end if(i2c_start()) */


  if(i2c_putchar((SlvAdrs<<1)|TW_WRITE))
  {
    if(--restarts == 0)
    {
      i2c_stop();
      return(2);/* return error code */

    }/* end if(--restarts == 0) */

    goto ReadRRestart;

  }/* end if(i2c_putchar((SlvAdrs<<1)|TW_WRITE)) */

/* write the device address to read from */
  if(i2c_putchar(adrs))
  {
    return(3);
  }  
  
/* apply a repeated start then the slave address with read */
  if(i2c_start()) // repeated Start
  {
    return(4);
  }
  if(i2c_putchar((SlvAdrs<<1)|TW_READ))
  {
    return(5);
  }
  
/* receive the data bytes from slave device and put them into the buffer */
  for(; len > 1; len--)
  {
    *(buf++) = i2c_getchar_ack();/* send ACK for all bytes but the last */
  }

  *(buf++) = i2c_getchar_nack();/* send NACK for last byte */

/* Apply a Stop condition on the bus and return 0 */
  i2c_stop();

  return(0);

}/* end i2c_read() */

uint8_t i2c_read_byte(uint8_t device_address, uint8_t register_address) {
    uint8_t data;

    i2c_start();
    i2c_putchar(device_address << 1); // Write mode
    i2c_putchar(register_address);
    i2c_start(); // Repeated start
    i2c_putchar((device_address << 1) | 1); // Read mode
    data = i2c_getchar_nack();
    i2c_stop();

    return data;
}
