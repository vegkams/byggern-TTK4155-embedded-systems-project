/*
 * solenoid.c
 *
 * Created: 16.11.2016 13:00:42
 *  Author: vegarkam
 */ 

#include "setup.h"
#include <avr/io.h>
#include <util/delay.h>


#define SOLENOID PA1

void setup_solenoid()
{
	// PA1 as solenoid output
	set_bit(DDRA,SOLENOID);
	set_bit(PORTA,SOLENOID);
	//1024 prescaler
	TCCR0B |= (1<<CS02)|(1<<CS00);
	solenoid_shoot();
}

void solenoid_shoot()
{
	clear_bit(PORTA,SOLENOID);
	uint8_t counter= 0;
	while(counter<30)
	{
		if (TCNT0 == 100)
		{
			counter++;
			TCNT0 = 0;
		}
	}
	set_bit(PORTA,SOLENOID);	
}

