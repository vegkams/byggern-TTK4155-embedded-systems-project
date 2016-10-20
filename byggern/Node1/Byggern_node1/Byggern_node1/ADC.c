/*
 * ADC.c
 *
 * Created: 14.09.2016 13:00:57
 *  Author: vegarkam
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "setup.h"
#include "ADC.h"


void setup_ADC()
{
	// Sett PD2 som input (INT0)
	clear_bit(DDRD,PD2);
	// pullup på PD2
	// set_bit(PORTD,PD2);
	// Enable interrupt on INT0
	//set_bit(GICR,INT0);
	// Interrupt på fallende flanke INT0
	//set_bit(MCUCR,ISC01);
	//clear_bit(MCUCR,ISC00);

	//sei(); // enable global interrupt	
}

//ISR(INT0_vect)
//{
	//// Les data på bussen
//}

void ADC_initiate_conversion(uint8_t channel)
{
	volatile char *address = (char *) 0x1400; // ADC address space 0x1400 ->
	if(channel > 3) return 0; // invalid channel
	int x = 0x04 | channel;
	address[0] = x; // ch1 = 3, ch2 = 4....
}

uint8_t read_ADC(uint8_t channel)
{
	volatile char *address = (char *) 0x1400; // ADC address space 0x1400 ->
	
	if(channel > 3) return 0; // invalid channel
	int x = 0x04 | channel;
	address[0] = x; // ch1 = 3, ch2 = 4....
	loop_until_bit_is_clear(PIND,PIND2); // Busy wait...
	return address[x];
	
}
