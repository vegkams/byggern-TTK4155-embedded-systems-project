/*
 * setup.h
 *
 * Created: 04.09.2016 15:01:47
 *  Author: vegarkam
 */ 

#include <avr/io.h>
#ifndef SETUP_H_
#define SETUP_H_
#define F_OSC 16000000L // Crystal oscillating frequency
#define F_CPU 16000000L // For use in _delay_ms
#define BAUD 9600 // Baud rate for RS 232
#define MYUBRR ((((long)F_OSC)/(16L * (long)BAUD)) -1L)
#define TRUE 1
#define FALSE 0

#define set_bit( reg, bit ) (reg |= (1 << bit))
#define clear_bit( reg, bit ) (reg &= ~(1 << bit))
#define test_bit( reg, bit ) (reg & (1 << bit))
#define loop_until_bit_is_set( reg, bit ) while( !test_bit( reg, bit ) )
#define loop_until_bit_is_clear( reg, bit ) while( test_bit( reg, bit ) )

void init();
uint8_t score_keeper();
void goal_scored();
typedef union axis_int_bytes{
	int int_axis;
	unsigned char bytes_axis[2];
}axis_int_bytes;
#endif /* SETUP_H_ */