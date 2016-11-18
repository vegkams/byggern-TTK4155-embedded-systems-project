/*
* motor_control.c
*
* Created: 09.11.2016 12:26:37
*  Author: vegarkam
*/
#include "motor_control.h"
#include "setup.h"
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <avr/interrupt.h>
#include "DAC_driver.h"
#include "USART.h"



#define vel_max 100
#define vel_min -100
#define dt 0.02

unsigned int encoder_value;
unsigned char MSB;
unsigned char LSB;
static int timer_flag;
static int playing_flag;
unsigned int encoder_max;
int clock_seconds;
uint8_t counter;
int16_t reference_value;
float error_sum;
float kp;
float kd;
float ki;
int integrator_max = 300;

uint8_t motor_control_init(){
	set_bit(DDRH, RST);
	set_bit(DDRH, OE);
	set_bit(DDRH, EN);
	set_bit(DDRH, DIR);
	set_bit(DDRH, SEL);
	// Port K Digital input
	DDRK = 0x00;

	setup_DAC();

	enable_encoder(1);
	encoder_reset();
	motor_control_set_speed(0);
	enable_motor(1);
	encoder_max = find_encoder_max();
	printf("Encoder max is : %d",encoder_max);
	clock_seconds = 0;
	counter = 0;
	motor_control_set_reference_pos(125);
	error_sum = 0;
	return 0;
}

void motor_control_init_clock(){
	//Trigger interrupt with interval of 50hz
	OCR1A = 5000;

	//Enable CTC mode
	TCCR1A |= (1 << COM1A0);

	//Prescale 64
	TCCR1B |= (1 << CS11) | (1 << CS10) | (1 << WGM12);
	
	//Enable compare match A interrupt
	TIMSK1 |= (1 << OCIE1A);
}

// read and return the encoder value
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
// Send the output to the motor
void motor_control_set_speed(uint8_t value){
	//printf("sending dac data: %d\n",value);
	send_DAC_data(value);
}

// Enable the motor
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

// Set the reference position
void motor_control_set_reference_pos(int pos)
{
	reference_value = -pos + 255;
	//printf("\tReference: %d\n", reference_value);
}

// Enable motor encoder
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

// Reset the motor encoder
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

// Set motor velocity, check for acceptable values and limit the output
void motor_control_set_velocity(int velocity)
{
	encoder_value = read_encoder();
	//printf("Encoder %d\n",encoder_value);
	

	
	int vel = saturate(velocity);
	motor_control_set_speed((uint8_t) abs(vel));
	if (vel > 0)
	{
		set_motor_direction(2);
	}
	else
	{
		set_motor_direction(1);
	}
	//else motor_control_set_speed(0);
	

	
}

// Toggles the timer flag
// Used for scoring the game
void motor_control_set_timer_flag(uint8_t flag)
{
	if (flag) timer_flag = TRUE;
	else timer_flag = FALSE;
}

// Toggle playing
void motor_control_set_playing_flag(uint8_t flag)
{
	printf("Set playing flag: %d\n", flag);
	if (flag) playing_flag = TRUE;
	else playing_flag = FALSE;
}

// Limit the motor velocity
int saturate(int vel) {
	if (vel > vel_max) return vel_max;
	else if (vel < vel_min) return vel_min;
	return vel;
}

// Resets and returns the score
int motor_control_get_played_time()
{
	return clock_seconds;
}

// Reset the score timer
void motor_control_reset_timer()
{
	clock_seconds = 0;
}

// Set the gains of the PID
void motor_control_set_pid_gains(float p, float i, float d)
{
	kp = p;
	ki = i;
	kd = d;
}

// ISR for the PID controller
ISR(TIMER1_COMPA_vect)
{
	// If the timer is on, count the score
	if (timer_flag)
	{
		counter++;
		// Running on 50Hz
		if (counter >= 50)
		{
			counter = 0;
			clock_seconds ++;
		}
	}
	
	// Use PID when playing
	if(!playing_flag)
	{
		return;
	}
	
	long encoder_value = read_encoder();
	
	if((int)encoder_value < 0){
		encoder_value = 0L;
	}
	
	float error;
	int16_t output;
	error = reference_value - ((encoder_value*255)/encoder_max);
	if (abs(error) < 3)
	{
		motor_control_set_speed(0);
		return;
	}
	if ((int) error_sum < integrator_max)
	{
		error_sum += dt*error;
	}
	int prop = -kp*error;
	int integral = - ki*error_sum;
	
	output = prop + integral;
	
	//printf("Sending ref\n");
	motor_control_set_velocity(output);
	
}

// Function for finding the encoder range
unsigned int find_encoder_max()
{
	printf("In find encoder max");
	motor_control_set_velocity(80);
	_delay_ms(1900);
	motor_control_set_velocity(0);
	_delay_ms(200);
	printf("Encoder value: %d\n",read_encoder());
	encoder_reset();
	motor_control_set_velocity(-80);
	_delay_ms(1200);
	motor_control_set_velocity(0);
	_delay_ms(100);
	printf("\tEncoder value2: %d\n",read_encoder());
	uint16_t encoder = read_encoder();
	motor_control_set_velocity(20);
	_delay_ms(200);
	motor_control_set_velocity(0);
	return encoder;
}

// Reset the integrator accumulated error
void motor_control_reset_integrator()
{
	error_sum = 0;
}
