
#include <util/delay.h>
#include <stdio.h>

#ifndef UART_H
#define UART_H
void UART_init(int prescale);
void UART_send(unsigned char data);
void UART_putstring(char* StringPtr);
void UART_puthex(uint8_t num);

#endif 