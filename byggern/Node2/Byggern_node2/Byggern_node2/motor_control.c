/*
* motor_control.c
*
* Created: 09.11.2016 12:26:37
*  Author: vegarkam
*/
#include "motor_control.h"
#include "setup.h"
#include "DAC_driver.h"
#include "USART.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define vel_max 125
#define vel_min -125
#define dt 0.01

unsigned int encoder_value;
unsigned char MSB;
unsigned char LSB;
static int timer_flag;
static int playing_flag;
unsigned int encoder_max;
int clock_seconds;
uint8_t counter;
int16_t reference_value;
int16_t error_sum;
float kp;
float kd;
float ki;

uint8_t motor_control_init(){
	set_bit(DDRH, RST);
	set_bit(DDRH, OE);
	set_bit(DDRH, EN);
	set_bit(DDRH, DIR);
	set_bit(DDRH, SEL);
	// Port K Digital input
	DDRK = 0x00;
	
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
	encoder_max = find_encoder_max();
	printf("Encoder max is : %d",encoder_max);
	clock_seconds = 0;
	counter = 0;
	motor_control_set_reference_pos(0);
	error_sum = 0;
	return 0;
}

void motor_control_init_clock(){
	//Trigger interrupt with interval of 100hz FQ
	OCR1A = 2500;

	//Enable CTC mode
	TCCR1A |= (1 << COM1A0);

	//Prescale 64
	TCCR1B = (1 << CS22) | (1 << WGM22);
	
	//Enable compare match A interrupt
	TIMSK1 |= (1 << OCIE1A);
}

unsigned int read_encoder()
{
	enable_encoder(1);
	// Select msb
	clear_bit(MJ1,SEL);
	_delay_us(20);

	uint8_t MSB = MJ2;
	// select lsb
	set_bit(MJ1,SEL);
	_delay_us(20);

	uint8_t LSB = MJ2;
	enable_encoder(0);
	return (unsigned int) ((MSB << 8) | LSB);
	//printf("Encoder value %d\n", encoder_value);
}

void set_motor_direction(uint8_t dir)
{
	if(dir == 2) 
	{
		//printf("Direction: LEFT\n");
		set_bit(MJ1,DIR);
	}
	else if(dir == 1) 
	{
		//printf("Direction: RIGHT\n");
		clear_bit(MJ1,DIR);
	}
}
//
void motor_control_set_speed(uint8_t value){
	//printf("sending dac data: %d\n",value);
	send_DAC_data(value);
}

void enable_motor(uint8_t enable)
{
	if (enable)
	{
		set_bit(MJ1,EN);
		printf("motor enabled\n");
	}
	else
	{
		printf("Motor disabled\n");
		clear_bit(MJ1,EN);
	}
}

void motor_control_set_reference_pos(int pos)
{
	reference_value = -pos + 100;
}

void enable_encoder(uint8_t enable)
{
	if(enable)
	{
		clear_bit(MJ1,OE);
	}
	else
	{
		set_bit(MJ1,OE);
	}
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

void motor_control_set_velocity(int velocity)
{
	encoder_value = read_encoder();
	//printf("Encoder value: %d\n", encoder_value);
	if ((encoder_value <= encoder_max && velocity > 0) || (encoder_value >= 0 && velocity < 0))
	{
		int vel = saturate(velocity);
		motor_control_set_speed((uint8_t) abs(vel)*2);
		if (vel > 0)
		{
			set_motor_direction(2);
		}
		else
		{
			set_motor_direction(1);
		}
	}
	else motor_control_set_speed(0);
	
	
}

void motor_control_set_timer_flag(uint8_t flag) 
{
	if (flag) timer_flag = TRUE;
	else timer_flag = FALSE;
}

void motor_control_set_playing_flag(uint8_t flag)
{
	if (flag) playing_flag = TRUE;
	else playing_flag = FALSE;
}

int saturate(int vel) {
	if (vel > vel_max) return vel_max;
	else if (vel < vel_min) return vel_min;
	return vel;
}

int motor_control_get_played_time()
{
	clock_seconds = 0;
	return clock_seconds;
}

void motor_control_reset_timer()
{
	clock_seconds = 0;
}

void motor_control_set_pid_gains(float p, float i, float d)
{
	kp = p;
	ki = i;
	kd = d;
}

// ISR for the PID controller
ISR(TIMER2_COMPA_vect)
{
	printf("Timer triggered interrupt \n");
	if (timer_flag)
	{
		counter++;
		if (counter >= 100)
		{
			counter = 0;
			clock_seconds ++; 
		}
	}
	
	if(!playing_flag)
	{
		return;
	}
	
	encoder_value = read_encoder();
	int16_t error, output;
	
	error = reference_value - (encoder_value/encoder_max)*200;
	if (error < 3)
	{
		motor_control_set_speed(0);
		return;
	}
	
	error_sum += dt*error;
	
	output = kp*error + ki*error_sum;
	motor_control_set_velocity(output);
		
}

unsigned int find_encoder_max()
{
	printf("In find encoder max");
	motor_control_set_velocity(50);
	_delay_ms(1500);
	motor_control_set_velocity(0);
	_delay_ms(200);
	printf("Encoder value: %d\n",read_encoder());
	encoder_reset();
	motor_control_set_velocity(-50);
	_delay_ms(1500);
	motor_control_set_velocity(0);
	_delay_ms(100);
	printf("\tEncoder value2: %d\n",read_encoder());
	return read_encoder();
}