/*
* pwm.c
*
* Created: 02.11.2016 10:45:17
*  Author: vegarkam
*/

#include "pwm.h"
#include "setup.h"
unsigned int input_range;
unsigned int output_range;
int input_joystick_start;
int input_joystick_end;
uint8_t input_slider_start;
uint8_t input_slider_end;
uint8_t input_joystick_range;
uint8_t input_slider_range;
int output_start;
int output_end;
uint8_t pwm_init(){
	//Mode 14
	TCCR3A |= (1 << WGM31)|(1 << COM3A1);
	TCCR3A &= ~(1 << WGM30)|(1 << COM3A0);
	TCCR3B |= (1 << WGM32)|(1 << WGM33)|(1 << CS31)|(1 << CS30);
	TCCR3B &= ~(1 << CS32);
	
	
	
	ICR3 = 5000;
	set_bit(DDRE,PE3);
	OCR3A = 375;

	input_joystick_start = -100;
	input_joystick_end = 100;
	input_slider_start = 0;
	input_slider_end = 255;
	output_start = 250;
	output_end = 500;
	input_joystick_range = input_joystick_end-input_joystick_start;
	input_slider_range = input_slider_end - input_slider_start;
	output_range = output_end-output_start;
	return 0;

}

uint8_t pwm_set_angle(int angle, uint8_t mode){
	//int scaled_angle = abs(angle-255);
	switch(mode)
	{
		case 1:
		{
			if(angle >= input_joystick_start && angle <= input_joystick_end){
				OCR3A = (angle-input_joystick_start)*output_range / input_joystick_range + output_start;
				return 0;
			}
			break;
		}
		
		
		case 2:
		{
			if(angle >= input_slider_start && angle <= input_slider_end){
				OCR3A = (angle-input_slider_start)*output_range / input_slider_range + output_start;
				return 0;
			}
			break;
		}
		
	}
	
	
	return 1;
}


