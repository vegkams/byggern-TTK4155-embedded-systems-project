/*
* eeprom.c
*
* Created: 22.11.2016 16:45:44
*  Author: vegarkam
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "eeprom.h"
#include "setup.h"

void write_eeprom(unsigned int uiAddress, unsigned char ucData)
{
	// Store status register in temporary variable
	unsigned char cSREG = SREG;
	// Disable global interrupts during write sequence
	cli();
	// Wait for completion of prev. write
	while(EECR & (1<<EEWE));
	// Set up address and data registers
	EEAR = uiAddress;
	EEDR = ucData;
	// Write one to EEMWE
	set_bit(EECR,EEMWE);
	// Start eeprom write
	set_bit(EECR,EEWE);
	
	// Restore previous value of status register
	SREG = cSREG;
}

unsigned char EEPROM_read(unsigned int uiAddress)
{
	// Store status register in temporary variable
	unsigned char cSREG = SREG;
	// Disable global interrupts during read sequence
	cli();
	// Wait for completion of prev. write
	while(EECR & (1<<EEWE));
	// Set up address and data registers
	EEAR = uiAddress;
	// Start eeprom read
	set_bit(EECR,EERE);
	// Restore previous value of status register
	SREG = cSREG;
	return EEDR;

}