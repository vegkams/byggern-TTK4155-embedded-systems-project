/*
* motor_control.c
*
* Created: 09.11.2016 12:26:37
*  Author: vegarkam
*/
#include "motor_control.h"
#include "setup.h"
#include "DAC_driver.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>

#define vel_max 1.0
#define vel_min -1.0

unsigned int input_range_motor;
unsigned int output_range_motor;
unsigned int input_start_motor;
unsigned int input_end_motor;
unsigned int output_start_motor;
unsigned int output_end_motor;
unsigned char MSB;
unsigned char LSB;
unsigned int encoder_value;

uint8_t motor_control_init(){
	set_bit(DDRH, RST);
	set_bit(DDRH, OE);
	set_bit(DDRH, EN);
	set_bit(DDRH, DIR);
	set_bit(DDRH, SEL);
	// Port A Digital input
	DDRC = 0x00;
	
	//input_start_motor = 0;
	//input_end_motor = 100;
	//output_start_motor = 0;
	//output_end_motor = 255;
	//input_range_motor = input_end_motor-input_start_motor;
	//output_range_motor = output_end_motor-output_start_motor;
	setup_DAC();

	enable_encoder(1);
	encoder_reset();
	motor_control_set_speed(0);
	enable_motor(1);
	
	return 0;
}

int read_encoder()
{
	enable_encoder(1);
	// Select msb
	clear_bit(MJ1,SEL);
	_delay_us(20);
	uint8_t  MSB = reverse_bits(MJ2);
	// select lsb
	set_bit(MJ1,SEL);
	_delay_us(20);
	uint8_t LSB = reverse_bits(MJ2);
	enable_encoder(0);
	return ((MSB << 8) | LSB);
	printf("Encoder value %d\n", encoder_value);
}

void set_motor_direction(MotorDir dir)
{
	switch (dir){
		case LEFT:  set_bit(MJ1,DIR); break;
		case RIGHT: clear_bit(MJ1,DIR); break;
		default: break;
	}
}
//
void motor_control_set_speed(uint8_t value){
	send_DAC_data(value);
}

void enable_motor(uint8_t enable)
{
	enable ? set_bit(MJ1,EN) : clear_bit(MJ1,EN);
}

void enable_encoder(uint8_t enable)
{
	enable ? clear_bit(MJ1,OE) : set_bit(MJ1,OE);
}

void encoder_reset()
{
	clear_bit(MJ1,RST);
	_delay_us(20);
	set_bit(MJ1,RST);
}


uint8_t reverse_bits(char x){
	x = ((x & 0x55) << 1) | ((x & 0xaa) >> 1);
	x = ((x & 0x33) << 2) | ((x & 0xcc) >> 2);
	x = ((x & 0x0f) << 4) | ((x & 0xf0) >> 4);
	return x;
}

void motor_control_set_velocity(float velocity)
{
	float vel = saturate(velocity, vel_min, vel_max);
	printf("\tvel = %3.2f\n", vel);
	motor_control_set_speed((uint8_t) fabs(vel)*250);
	set_motor_direction(vel > 0 ? RIGHT:LEFT);
}

float saturate(vel, min, max) {
	if (vel > max) return max;
	else if (vel < min) return min;
	return vel;
}