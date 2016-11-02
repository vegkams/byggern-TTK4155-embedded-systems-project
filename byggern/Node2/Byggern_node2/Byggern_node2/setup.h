/*
 * setup.h
 *
 * Created: 04.09.2016 15:01:47
 *  Author: vegarkam
 */ 


#ifndef SETUP_H_
#define SETUP_H_
#define F_OSC 16000000L // Crystal oscillating frequency
#define BAUD 9600 // Baud rate for RS 232
#define MYUBRR ((((long)F_OSC)/(16L * (long)BAUD)) -1L)
#define TRUE 1
#define FALSE 0

#define set_bit( reg, bit ) (reg |= (1 << bit))
#define clear_bit( reg, bit ) (reg &= ~(1 << bit))
#define test_bit( reg, bit ) (reg & (1 << bit))
#define loop_until_bit_is_set( reg, bit ) while( !test_bit( reg, bit ) )
#define loop_until_bit_is_clear( reg, bit ) while( test_bit( reg, bit ) )
typedef union float_to_bytes{
	float data_f;
	uint8_t bytes[4];
} float_to_bytes;
void init();
#endif /* SETUP_H_ */