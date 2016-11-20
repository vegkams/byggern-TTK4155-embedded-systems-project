/*
 * USART.c
 *
 * Created: 31.08.2016 13:47:13
 *  Author: vegarkam
 */ 
#include "USART.h"
#include <avr/io.h>
#include "setup.h"




FILE *uart;


void USART_Init(unsigned int ubrr)
{
    // Split ubrr into two bytes, write to UBRRH/L
	UBRR0H =(unsigned char) (ubrr>>8);
	UBRR0L =(unsigned char) (ubrr);
	// Enable sender and receiver
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (1<<URSEL0)|(1<<USBS0)|(3<<UCSZ00);
    
    uart = fdevopen(&USART_Transmit,&USART_Recieve);
}

int USART_Transmit(char data, struct __file* dummy)
{
	// Check if bit UDRE0 in register UCSR0A is set
	// UDRE0 = usart data register empty
	while(!(UCSR0A & (1<<UDRE0)))
		;
	UDR0=data;
    return 0;
}

int USART_Recieve(struct __file* dummy)
{
	// Check bit RXC0 in register UCSR0A
	// RXC0 = 1 -> byte received
	while (!(UCSR0A & (1<<RXC0)))
		;
	return UDR0;
}
