/*
 * ADC.c
 *
 * Created: 14.09.2016 13:00:57
 *  Author: vegarkam
 *  Driver for interfacing with ADC0844CCN
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "setup.h"
#include "ADC.h"


void setup_ADC()
{
	// Set PD2 as input
	clear_bit(DDRD,PD2);
}



uint8_t ADC_initiate_conversion(uint8_t channel)
{
	volatile char *address = (char *) 0x1400; // ADC address space 0x1400 ->
	if(channel > 3) return 1; // invalid channel
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
