/*
 * DAC_driver.c
 *
 * Created: 06.11.2016 18:28:26
 *  Author: vegarkam
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "TWI_Master.h"
#include "DAC_driver.h"
#define SLAVE_ADDRESS 0x5E

void setup_DAC() {
	TWI_Master_Initialise();
	//PD= scl, PD1 SDA
	DDRD |= (1<<PD0) | (1<<PD1);
	// TWI uses interrupt
	sei();
}

void send_DAC_data(unsigned char value) {
	unsigned char size = 3;
	unsigned char* data = malloc(3*sizeof(char));
	data[0] = SLAVE_ADDRESS;
	// Command byte: address dac 0, normal operational state
	data[1] = 0x00;
	data[2] = value;
	_delay_us(50);
	TWI_Start_Transceiver_With_Data(data,size);
	_delay_us(100);
	free(data);
}
