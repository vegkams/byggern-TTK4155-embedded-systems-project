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
unsigned int input_start;
unsigned int input_end;
unsigned int output_start;
unsigned int output_end;
uint8_t pwm_init(){
	//Mode 14
	TCCR3A |= (1 << WGM31)|(1 << COM3A1);
	TCCR3A &= ~(1 << WGM30)|(1 << COM3A0);
	TCCR3B |= (1 << WGM32)|(1 << WGM33)|(1 << CS31)|(1 << CS30);
	TCCR3B &= ~(1 << CS32);
	
	
	
	ICR3 = 5000;
	set_bit(DDRE,PE3);
	OCR3A = 375;

	input_start = -100;
	input_end = 100;
	output_start = 250;
	output_end = 500;
	input_range = input_end-input_start;
	output_range = output_end-output_start;
	return 0;

}

uint8_t pwm_set_angle(int angle){
	
	if(angle>=-100 && angle<= 100){
		OCR3A = (angle-input_start)*output_range / input_range + output_start;
		return 0;
	}
	
	return 1;
}


