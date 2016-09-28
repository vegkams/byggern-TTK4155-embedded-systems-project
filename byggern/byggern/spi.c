/*
 * spi.c
 *
 * Created: 28.09.2016 15.29.59
 *  Author: vegard
 */ 
#include <avr/io.h>
#include <stdio.h>
void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	// TODO: DDB5 = data ut, DDB4 = Klokkepuls
	DDRB = (1<<DDB5)|(1<<DDB4); 
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}
void SPI_MasterTransmit(char cData)
{
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)))
	;
}