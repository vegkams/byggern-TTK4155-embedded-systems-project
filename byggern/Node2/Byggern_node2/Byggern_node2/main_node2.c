/*
* Main.c
*
* Created: 19.10.2016 11:53:08
*  Author: vegarkam
*/


#include <avr/io.h>
#include "can.h"
#include "setup.h"
#include "USART.h"
#include "joystick.h"
#include "pwm.h"
#include "ADC.h"
#include <util/delay.h>


can_message received;
can_message send;
uint8_t score = 0;
float joystick_x_percentage;
float joystick_y_percentage;
uint8_t paused = TRUE;
int main(void)
{
	init();
	volatile float_to_bytes x;
	volatile float_to_bytes y;
	can_message send_message;
	joyValues values;
	direction dir;
	int x_axis;
	int y_axis;
	uint8_t counter = 0;
	uint8_t adc_value = 0;
	int accumulated_value = 0;
	int averaged_value=0;

	while(1)
	{
		while(paused == FALSE) {
			adc_value = get_ADC_value();
			accumulated_value += adc_value;
			counter++;
			if (counter==5)
			{	counter = 0;
				printf("accumulated_value= %d \n",accumulated_value);
				averaged_value = accumulated_value/5;
				accumulated_value = 0;
				if (averaged_value < 100) {
					averaged_value=0;
					score = score_keeper();
					goal_scored();
				}
			}
		}
		
		if(can_data_received() > 0) {
			received = *can_receive_message();
			if(received.ID == 0) {
				// Control message, blablabla
			}
			if (received.ID == 1){
				// input data from controller
				values.left_button = received.data[0];
				values.right_button = received.data[1];
				values.joystick_button = received.data[2];
				dir = (direction) received.data[3];
				x_axis = received.data[4]<<8 | received.data[5];
				y_axis = received.data[6]<<8 | received.data[7];
				if(values.joystick_button == 1) paused = FALSE;
				pwm_set_angle(-x_axis);
			}
			
			
			printf("Received data: id: %d len: %d\n",received.ID,received.length);
			printf("left button: %d , right button: %d , joystick button: %d, direction: %s, x axis: %d, y axis: %d \n", values.right_button, values.right_button, values.joystick_button, stringFromDirection(dir), x_axis,y_axis);
			//Construct return data:
			if(score>=3) {
				// Game lost, send message
				send_message.ID = 2;
				send_message.length = 1;
				send_message.data[0]=score;
				//printf("ADC value: %d \n", get_ADC_value());
				uint8_t sent = can_send_message(&send_message);
				_delay_ms(50);
				if (sent == 1) {
					printf("Sent! \n");
				}
				else{
					printf("failed\n");
				}
			}
		}
	}
}

void init() {
	int baud = (int) MYUBRR;
	USART_Init(baud);
	can_init();
	CAN_enable_normal_mode();
	pwm_init();
	setup_ADC();
	start_ADC();
}
uint8_t score_keeper()
{
	score++;
	return score;
}

void goal_scored() {
	paused = TRUE;
}