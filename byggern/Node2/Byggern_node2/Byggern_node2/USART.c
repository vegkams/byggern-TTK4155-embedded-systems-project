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
    
	UBRR0H =(unsigned char) (ubrr>>8); // Flytt ubrr 8 bits til høyre, lagre i register UBRR0H
	UBRR0L =(unsigned char) (ubrr); // Lagre første 8 bits i UBRR0L
	// Enable mottaker og sender (sett bitt RXEN0 og TXEN0 =1)
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    // 8 data bits, 2 stop bits
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
    
	//Breakpoint
    uart = fdevopen(&USART_Transmit,&USART_Recieve);
}

int USART_Transmit(char data, struct __file* dummy)
{
    //if(data == '\n') USART_Transmit('\r', dummy);
	// Sjekk om bit UDRE0 i register UCSR0A er satt
	// UDRE0 = usart data register empty
	while(!(UCSR0A & (1<<UDRE0)))
		;
	UDR0=data;
    return 0;
}

int USART_Recieve(struct __file* dummy)
{
	// Sjekk om bit RXC0 i register UCSR0A
	while (!(UCSR0A & (1<<RXC0)))
		;
	return UDR0;
}
