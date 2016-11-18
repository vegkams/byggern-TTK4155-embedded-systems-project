/*
 * setup.h
 *
 * Created: 04.09.2016 15:01:47
 *  Author: vegarkam
 */ 


#ifndef SETUP_H_
#define SETUP_H_
#define F_OSC 4915200L // Crystal oscillating frequency
#define F_CPU 4915200L
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

typedef union int_union_bytes{
	int int_value;
	unsigned char byte_value[2];
	}int_union_bytes;

typedef enum {MENU, PLAYING} mode;
		
void playing_the_game();
void in_menus();
void initializations();
uint8_t send_joystick_data();
void enable_can_timer();
void disable_can_timer();
ISR(TIMER1_COMPA_vect );
#endif /* SETUP_H_ */