/*
* byggern_1.c
*
* Created: 31.08.2016 11:46:00
*  Author: vegarkam
*/
#include <avr/io.h>
#include "USART.h"
#include "setup.h"
#include "sram.h"
#include "joystick.h"
#include "OLED.h"
#include "menu.h"
#include "MCP2515.h"
#include "can.h"
#define TRUEMADDAFAKKA 1
menu_t * main_menu;
uint8_t arrow_line = 2;
uint8_t previous_joystick_button;
uint8_t game_on = 0;
uint8_t mode;
uint8_t menu_printed;
int main(void)
{
	initializations();
	CAN_enable_loopback();
	menu_printed = 0;
	mode = 0;
	volatile uint8_t c;
	while (TRUEMADDAFAKKA)
	{
		can_message sendmessage;
		can_message receivemessage;
		sendmessage.ID = 0xFFFF;
		sendmessage.length=5;
		for(int i = 0; i<sendmessage.length;i++){
			sendmessage.data[i] = 10*i;
		}
		uint8_t ok = can_send_message(&sendmessage);
		if (ok == 1)
		{
			printf("Message was sent!\n");
		}
		switch(mode){
			case 0:
			in_menus();
			break;
			
			case 1:
			playing_the_game();
			break;
		}
		if(can_data_received()) {
			receivemessage = *can_receive_message();
			printf("Message was received!\n");
			printf("Message ID: %u, message length: %d\n", receivemessage.ID, receivemessage.length);
			printf("message data 1: %d, message data 2: %d, message data 3: %d, message data 4: %d, message data 5: %d\n",receivemessage.data[0],receivemessage.data[1],receivemessage.data[2],receivemessage.data[3],receivemessage.data[4]);
		}

		
		//joyValues j;
		//read_joystick(&j);
		//float slider_left = joystick_get_left_slider_percentage();
		//float slider_right = joystick_get_right_slider_percentage();
		//direction d = joystick_getDirection(j.x_percentage,j.y_percentage);
		//int d1 = (int)d;
		//
		//printf("X prosent: %3.2f, Y prosent: %3.2f, Slider venstre: %3.2f, Slider høyre: %3.2f, Retning: %d ",j.x_percentage,j.y_percentage,slider_left,slider_right, d1);
		//printf("Knapp 1: %d,Knapp 2: %d, Joystick knapp: %d\n",j.left_button,j.right_button,j.joystick_button);
	}
	
	return 0;
}

void in_menus(){
	
	
	if (menu_printed == 0)
	{
		print_menu(main_menu);
		menu_printed = 1;
	}
	
	joyValues j;
	read_joystick(&j);
	direction d = joystick_getDirection(j.x_percentage,j.y_percentage);
	
	arrow_line = move_arrow(d,arrow_line);
	if(j.joystick_button == previous_joystick_button){
		j.joystick_button = 0;
	}
	else{
		previous_joystick_button = 0;
	}
	if (j.joystick_button == 1) {
		mode = button_action(arrow_line);
		previous_joystick_button = j.joystick_button;
		arrow_line = 2;
	}
}


void playing_the_game(){
	
}



void initializations(){
	//static volatile long temp = F_OSC; //For testing
	//Breakpoint x2
	// Enable external memory
	set_bit(MCUCR,SRE);
	// Mask PC7-PC4
	set_bit(SFIOR,XMM2);
	int baud = (int) MYUBRR;
	USART_Init(baud);
	sram_setup();
	SRAM_test();
	setup_ADC();
	joystick_init();
	oled_init();
	main_menu = menu_init();
	can_init();
	
}