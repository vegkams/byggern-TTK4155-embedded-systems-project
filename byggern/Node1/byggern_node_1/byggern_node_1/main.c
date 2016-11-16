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
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
menu_t * main_menu;
uint8_t arrow_line = 2;
uint8_t previous_joystick_button;
uint8_t game_on = 0;
mode mode_t = MENU;
//uint8_t mode;
uint8_t menu_printed;

int main(void)
{
	initializations();
	CAN_enable_normal_mode();
	menu_printed = 0;
	
	volatile uint8_t c;
	while (TRUE)
	{
		_delay_ms(50);
		if(get_game_mode() ==0){
			mode_t = MENU;
		}
		else if(get_game_mode()==1){
			mode_t=PLAYING;
		}
		//uint8_t ok = send_joystick_data();
		
		//if (ok == 1)
		//{
		//printf("Message was sent!\n");
		//}
		
		switch(mode_t){
			case MENU:
			in_menus();
			break;
			
			case PLAYING:
			playing_the_game();
			break;
		}
		
		if(can_data_received() > 0) {
			can_message* receivemessage = malloc(sizeof(can_message));
			can_receive_message(receivemessage);
			if(receivemessage->ID == 3) {
				printf("Number of lives is: %d", (3-receivemessage->data[0]));
			}
			
			if(receivemessage->ID == 2) {
				printf("GAME LOST, number of lives is %d\n", (3-receivemessage->data[0]));
				printf("Press button to start again\n");
			}

			printf("\n");
			free(receivemessage);
		}

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
		mode_t = button_action(arrow_line);
		previous_joystick_button = j.joystick_button;
		arrow_line = 2;
	}
	
	
}


void playing_the_game(){
	uint8_t ok = send_joystick_data();
	joyValues j;
	read_joystick(&j);
	printf("button: %d\n", j.right_button);
	if(j.right_button == 1){
		restart_game_mode();
		mode_t = MENU;
		arrow_line=2;
		button_action(arrow_line);
	}
	
}


uint8_t send_joystick_data() {
	joyValues j;
	read_joystick(&j);
	axis_int_bytes x_axis;
	axis_int_bytes y_axis;
	uint8_t dir = (uint8_t) joystick_getDirection(j.x_percentage,j.y_percentage);
	x_axis.int_axis = (int) j.x_percentage;
	y_axis.int_axis = (int) j.y_percentage;
	uint8_t slider = joystick_get_right_slider();
	can_message sendmessage;
	sendmessage.ID = 1;
	sendmessage.length=7;

	sendmessage.data[0] = j.left_button;
	sendmessage.data[1] = j.right_button;
	sendmessage.data[2] = j.joystick_button;
	sendmessage.data[3] = dir;
	sendmessage.data[4] = x_axis.bytes_axis[0];
	sendmessage.data[5] = x_axis.bytes_axis[1];
	//printf("X axis data 1: %d data 2: %d\n", (x_axis.bytes_axis[0]), (x_axis.bytes_axis[1]));
	sendmessage.data[6] = slider;
	return can_send_message(&sendmessage);
	
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