/*
 * spi.c
 *
 * Created: 28.09.2016 15.29.59
 *  Author: vegard
 */ 
#include <avr/io.h>
#include <stdio.h>
#include "spi.h"
#include "setup.h"


void spi_init()
{
	/* Set MOSI and SCK output, all others input */
	// PB7 = !SS, PB2 = MOSI, PB3 = MISO, PB1 = SCK
	DDRB = (1<<PB7)|(1<<PB2)|(1<<PB1)|(1<<PB0);
	// MISO as input
	clear_bit(DDRB,PB3);
	set_bit(PORTB,PB3);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	// Read SPSR and SPDR to clear interrupt flag
	char c1 = SPSR;
	char c2 = SPDR;
}
void spi_send(char cData)
{
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
}

char spi_read() 
{

	SPDR = 0x00; // send dummy data
	while(!(SPSR & (1<<SPIF))); // Wait until data is shifted into SPDR
	return SPDR;
}

void spi_enable()
{
	clear_bit(PORTB,PB7);
	clear_bit(PORTB,PB0);
}

void spi_disable()
{
	set_bit(PORTB,PB7);
	set_bit(PORTB,PB0);
}