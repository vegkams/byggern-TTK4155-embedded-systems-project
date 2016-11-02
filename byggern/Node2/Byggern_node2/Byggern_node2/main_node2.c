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
#include <util/delay.h>


can_message received;
can_message send;

float joystick_x_percentage;
float joystick_y_percentage;
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
	while(1)
	{
		
		
		
		
		if(can_data_received() > 0) {
			received = *can_receive_message();
			values.left_button = received.data[0];
			values.right_button = received.data[1];
			values.joystick_button = received.data[2];
			dir = (direction) received.data[3];
			x_axis = received.data[4]<<8 | received.data[5];
			y_axis = received.data[6]<<8 | received.data[7];
			
			pwm_set_angle(-x_axis);
			
			printf("Received data: id: %d len: %d\n",received.ID,received.length);
			printf("left button: %d , right button: %d , joystick button: %d, direction: %s, x axis: %d, y axis: %d \n", values.right_button, values.right_button, values.joystick_button, stringFromDirection(dir), x_axis,y_axis);
			// Construct return data:
			//send_message.ID = 100;
			//send_message.length = 8;
			//for (int i = 0; i < send_message.length; i++) {
				//send_message.data[i] = 5*i;
				////printf("Data: %d ", send_message.data[i]);
			//}
			////printf("\n");
			////
			//uint8_t sent = can_send_message(&send_message);
			////_delay_ms(50);
			//if (sent == 1) {
				////printf("Sent! \n");
			//}
			//else{
				////printf("failed\n");
			//}
		}
	}
}

void init() {
	int baud = (int) MYUBRR;
	USART_Init(baud);
	can_init();
	CAN_enable_normal_mode();
	pwm_init();
}