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
	// PB4 = !SS, PB5 = MOSI, PB6 = MISO, PB7 = SCK
	set_bit(DDRB, MOSI);
	set_bit(DDRB, SCK);
	set_bit(DDRB, SPI_SS);
	 
	/* Enable SPI, Master, set clock rate fck/16 */
	set_bit(SPCR, SPE);
	set_bit(SPCR, MSTR);
	set_bit(SPCR, SPR0);
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
	clear_bit(PORTB,SPI_SS);
}

void spi_disable()
{
	set_bit(PORTB,SPI_SS);
}