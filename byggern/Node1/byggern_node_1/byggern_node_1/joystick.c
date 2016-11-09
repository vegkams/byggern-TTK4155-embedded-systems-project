/*
 * joystick.c
 *
 * Created: 21.09.2016 08:47:04
 *  Author: vegarkam
 */
#include <avr/io.h>
#include "setup.h"
#include "ADC.h"
#include "joystick.h"

void joystick_init()
{
	// Set PB0-1 as input
	clear_bit(DDRB,PB0);
	clear_bit(DDRB,PB1);
	// Pull-up on PB2
	set_bit(PORTB,PB2);
	//Set PB2 as input
	clear_bit(DDRB,PB2);
}

void read_joystick(joyValues *joy)
{
	//joyValues joystick;
	joy->x_percentage = joystick_get_x_percentage();
	joy->y_percentage = joystick_get_y_percentage();
	// If PB0 || PB1 =1, button is pressed
	if (test_bit(PINB,PB0)) joy->left_button = 1;
	else joy->left_button = 0;
	if (test_bit(PINB,PB1)) joy->right_button = 1;
	else joy->right_button = 0;
	
	// If PB2=0, button is pressed
	if (test_bit(PINB,PB2)) joy->joystick_button = 0;
	else joy->joystick_button = 1;
}

direction joystick_getDirection(float x, float y)
{
	direction d = NEUTRAL;
	if (!(fabsf(x)<50 && fabsf(y)<50)) 
	{
		if (fabsf(x)>50 && fabsf(y)<50)
		{
			if (x<0)
			{
				d = LEFT;
			}
			else
			{
				d = RIGHT;
			}
		}
		else if(fabsf(x)<50 && fabsf(y)>50)
		{
			if (y<0)
			{
				d = DOWN;
			}
			else
			{
				d = UP;
			}
		}
		else
		{
			if (fabsf(y/x)>1)
			{
				if (y<0)
				{
					d = DOWN;
				}
				else
				{
					d = UP;
				}
			}
			else
			{
				if (x<0)
				{
					d = LEFT;
				}
				else
				{
					d = RIGHT;
				}
			}
		}
		
	}
	return d;
}

// Get Joystick position in percentage (center = 50%)
float joystick_get_x_percentage()
{
	uint8_t data = read_ADC(1);
	return convert_to_percentage(data);
} 

// Get Joystick position in percentage (center = 50%)
float joystick_get_y_percentage()
{
	uint8_t data = read_ADC(0);
	return convert_to_percentage(data);
}

float joystick_get_left_slider_percentage()
{
	uint8_t data = read_ADC(2);
	return convert_to_sliderpercentage(data);
}

float joystick_get_right_slider_percentage()
{
	uint8_t data = read_ADC(3);
	return convert_to_sliderpercentage(data);
}

uint8_t joystick_get_right_slider()
{
	uint8_t data = read_ADC(3);
	return data;
}

// Get joystick axis voltage (center = 2,5V)
float joystick_get_x_voltage()
{
	uint8_t data = read_ADC(1);
	return convert_to_voltage(data);
}

// Get joystick axis voltage (center = 2,5V)
float joystick_get_y_voltage()
{
	uint8_t data = read_ADC(0);
	return convert_to_voltage(data);
}

// Convert digital value to voltage (0-255 -> 0-5V)
float convert_to_voltage(uint8_t digital_value)
{
	float value = digital_value;
	return (value/255)*5;
}

// Convert digital value to percentage (0-255 -> 0-100%)
float convert_to_percentage(uint8_t digital_value)
{
	float value = digital_value;
	return ((value-128)/128)*100;
}

float convert_to_sliderpercentage(uint8_t digital_value)
{
	float value = digital_value;
	return ((value)/255)*100;
}