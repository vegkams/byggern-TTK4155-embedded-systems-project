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


can_message received;
can_message send;

float joystick_x_percentage;
float joystick_y_percentage;
int main(void)
{
	init();
    while(1)
    {
		uint8_t sent; 
        if(can_data_received()) {
			received = *can_receive_message();

			if(received.ID == 0x0020) {
				joystick_x_percentage = (received.data[0] << 24 | received.data[1] << 16 | received.data[2] << 8 | received.data[3]);
				joystick_y_percentage = (received.data[4] << 24 | received.data[5] << 16 | received.data[6] << 8 | received.data[7]);
			}
			
			printf("Received data: id: %d len: %n\n");
			printf("Joystick values: x percent: %.2f, y percent %.2f");
			// Construct return data:
			send.ID = 0x0020;
			send.length = 8;
			for (int i = 0; i < send.length; i++) {
			send.data[i] = 5*i;
			}
			sent = can_send_message(&send);
			if (sent == 0) {
				printf("Message was sent \n");
			}
		}
    }
}
void init() {
	int baud = (int) MYUBRR;
	USART_Init(baud);
	can_init();
	CAN_enable_normal_mode();
}