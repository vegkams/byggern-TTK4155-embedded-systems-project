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
#include "solenoid.h"
#include <string.h>


can_message send;
uint8_t score = 0;

uint8_t paused = FALSE;
uint8_t message_sent = FALSE;

float ps4_values[5];


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
	uint8_t previous_joystick_button = 0;
	float joystick_x_percentage;
	float joystick_y_percentage;
	uint8_t slider = 50;
	int8_t prev_x_axis;
	uint8_t prev_slider;
	uint8_t closed_loop = TRUE;
	
	// Default multifunction board
	Control_mode  control_mode = MFB;
	uint8_t message_id;
	uint8_t ps4_trigger;
	uint8_t previous_ps4_trigger;
	uint8_t uart_data_received;

	
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
		message_id = received->ID;
		switch(message_id)
		{
			case 0:
			{
				// no message
				break;
			}
			
			
			case 1:
			{
				// input data from node 1
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
				if(values.joystick_button == previous_joystick_button){
					values.joystick_button = 0;
				}
				else{
					previous_joystick_button = 0;
				}
				if (values.joystick_button == 1) {
					previous_joystick_button = values.joystick_button;
					solenoid_shoot();
				}
				//printf("Slider is :%d\n",slider);
				
				break;
			}
			
			
			case 2:
			{
				// Control message: data[0] = control mode, data[1] = paused/playing, data[2] =
				// 0 = ps4, 1 = multifunction board
				if(received->data[0])
				{
					control_mode = MFB;
				}
				else 
				{
					control_mode = PS4;
				}
				break;
			}
			
		}
		
		switch(control_mode)
		{
			case PS4:
			{
				uart_data_received = read_ps4_controller();
				if(uart_data_received)
				{
					// Data received over uart
					int angle = (int) ps4_values[0]*100;
					int pos_ref = mapped_ps4_pos(ps4_values[2]);
					ps4_trigger = (uint8_t) ps4_values[4];
					if(ps4_trigger == previous_ps4_trigger){
						ps4_trigger = 0;
					}
					else{
						previous_ps4_trigger = 0;
					}
					if (ps4_trigger == 1) {
						previous_ps4_trigger = ps4_trigger;
						solenoid_shoot();
					}
					motor_control_set_reference_pos(pos_ref);
					pwm_set_angle(-angle,1);
				}
				break;
			}
			
			
			case MFB:
			{
				if (closed_loop) {
					motor_control_set_reference_pos(slider);
					if(abs(x_axis.int_axis - prev_x_axis)>3) pwm_set_angle(-x_axis.int_axis,1);
					prev_x_axis = x_axis.int_axis;
				}
				else
				{
					motor_control_set_velocity(x_axis.int_axis);
					if (abs(slider - prev_slider)>3)
					{
						pwm_set_angle(slider,2);
					}
					
				}
				break;
			}
			
			
		}		
		
		
		if(score < 3 && !message_sent) {
			// Send number of hits
			send_message.ID = 3;
			send_message.length = 1;
			send_message.data[0]=score;
			//printf("ADC value: %d \n", get_ADC_value());
			uint8_t sent = can_send_message(&send_message);
			if (sent == 1) {
				message_sent = TRUE;
			}
			else{
				message_sent = FALSE;
			}
			
		}
		
		
		
		free(received);
		//Construct return data:
		if(score>=3 && !message_sent) {
			// Game lost, send message
			send_message.ID = 4;
			send_message.length = 1;
			send_message.data[0]=score;
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
	motor_control_init();
	setup_ADC();
	start_ADC();
	setup_solenoid();
	motor_control_init_clock();

	motor_control_set_playing_flag(TRUE);
	motor_control_set_pid_gains(1.0,0.3,0.0);
	
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

// Parse string from UART
uint8_t read_ps4_controller()
{
	
	if (!(UCSR0A & (1<<RXC0)))
	{
		// No data in UART receive register
		return 0;
	}
	unsigned char string[100];
	scanf("%s",string);
	char *token;
	uint8_t i = 0;
	while ((token = strsep(&string, ":")) && i < 5)
	{
		ps4_values[i] = atof(token);
		i++;
	}
	return 1;
}

int mapped_ps4_pos(float value)
{
	int integer_value = (int) value * 100;
	return (integer_value + 100) * 255/ 200;
}

