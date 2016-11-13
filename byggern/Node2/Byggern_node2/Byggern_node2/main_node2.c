/*
* Main.c
*
* Created: 19.10.2016 11:53:08
*  Author: vegarkam
*/



#include "can.h"
#include "setup.h"
#include "USART.h"
#include "joystick.h"
#include "pwm.h"
#include "ADC.h"
#include "DAC_driver.h"
#include "motor_control.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>


can_message send;
uint8_t score = 0;

uint8_t paused = FALSE;
uint8_t message_sent = FALSE;



int main(void)
{
	
	init();
	can_message send_message;
	joyValues values;
	direction dir;
	axis_int_bytes x_axis;
	int y_axis;
	uint8_t counter = 0;
	uint8_t adc_value = 0;
	int accumulated_value = 0;
	int averaged_value = 0;
	can_message* received;
	uint8_t previous_button = 0;
	float joystick_x_percentage;
	float joystick_y_percentage;
	uint8_t slider = 50;
	uint8_t prev_slider;

	
	while(1)
	{
		
		if(paused == FALSE) {
			adc_value = get_ADC_value();
			accumulated_value += adc_value;
			counter++;
			if (counter==5)
			{	counter = 0;
				//printf("accumulated_value= %d \n",accumulated_value);
				averaged_value = accumulated_value/5;
				accumulated_value = 0;
				if (averaged_value < 80) {
					averaged_value=0;
					score = score_keeper();
					goal_scored();
				}
			}
		}
		

		can_message* received = malloc(sizeof(can_message));
		can_receive_message(received);
		if(received->ID == 0) {
			//printf("no message\n");
		}
		if (received->ID == 1){
			// input data from controller
			values.left_button = received->data[0];
			values.right_button = received->data[1];
			values.joystick_button = received->data[2];
			dir = (direction) received->data[3];
			x_axis.bytes_axis[0] = received->data[4];
			x_axis.bytes_axis[1] = received->data[5];
			slider = received->data[6];
			if(values.left_button == previous_button){
				values.left_button = 0;
			}
			else{
				previous_button = 0;
			}
			if (values.left_button == 1) {
				previous_button = values.left_button;
				paused = FALSE;
				message_sent = FALSE;
			}
			motor_control_set_velocity(x_axis.int_axis);
			//printf("Motor control vel: %i\n", x_axis.int_axis);
			//_delay_ms(100);
			//printf("slider value: %d\n",slider);
			if(abs(slider-prev_slider)>6) pwm_set_angle(slider);
			prev_slider = slider;
		}
		
		
		
		if(score < 3 && !message_sent) {
			// Send number of hits
			send_message.ID = 3;
			send_message.length = 1;
			send_message.data[0]=score;
			//printf("ADC value: %d \n", get_ADC_value());
			uint8_t sent = can_send_message(&send_message);
			_delay_ms(10);
			if (sent == 1) {
				message_sent = TRUE;
			}
			else{
				message_sent = FALSE;
			}
			
		}
		
		
		//printf("Received data: id: %d len: %d\n",received->ID,received->length);
		//printf("left button: %d , right button: %d , joystick button: %d, direction: %s, x axis: %d, y axis: %d \n", values.right_button, values.right_button, values.joystick_button, stringFromDirection(dir), x_axis,y_axis);
		free(received);
		//Construct return data:
		if(score>=3 && !message_sent) {
			// Game lost, send message
			send_message.ID = 2;
			send_message.length = 1;
			send_message.data[0]=score;
			//printf("ADC value: %d \n", get_ADC_value());
			//printf("HEre\n");
			uint8_t sent = can_send_message(&send_message);
			_delay_ms(10);
			if (sent == 1) {
				message_sent = TRUE;
			}
			else{
				message_sent = FALSE;
			}
			score = 0;
			paused = TRUE;
		}
		_delay_ms(50);


	}
}


void init() {
	
	int baud = (int) MYUBRR;
	USART_Init(baud);
	_delay_ms(20);
	printf("Init called \n");
	can_init();
	CAN_enable_normal_mode();
	pwm_init();
	setup_ADC();
	start_ADC();
	motor_control_init();
	motor_control_init_clock();

	motor_control_set_playing_flag(TRUE);
	motor_control_set_pid_gains(0.5,0.1,0.0);
	
}
uint8_t score_keeper()
{
	score++;
	message_sent = FALSE;
	printf("Score is: %d\n",score);
	return score;
}

void goal_scored() {
	paused = TRUE;
}

