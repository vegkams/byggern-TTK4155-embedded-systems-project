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
menu_t *        main_menu;
uint8_t         arrow_line = 5;
uint8_t         previous_joystick_button;
uint8_t         game_on = 0;
mode            mode_t = MENU;
//uint8_t mode;
uint8_t         menu_printed;
uint8_t         lives = 3;
uint8_t         pause = TRUE;
int_union_bytes score;
uint8_t game_initialized = 0;
can_message * sendmessage;

int main(void)
{
	initializations();
	menu_printed = 0;
	while (TRUE)
	{
		sendmessage = malloc(sizeof(can_message));
		_delay_ms(10);
		//printf("Game mode: %d",get_game_mode());
		if(get_game_mode() == 0){
			mode_t = MENU;
		}
		else if(get_game_mode() == 1){
			mode_t = PLAYING;
			if (!game_initialized)
			{
				sendmessage->ID = 2;
				sendmessage->length = 1;
				sendmessage->data[0] = 1;
				
				printf("Starting game %d\n",can_send_message(sendmessage));
				_delay_ms(20);
				game_initialized = TRUE;
				pause = FALSE;			
			}			
		}
		
		switch(mode_t){
			case MENU:
			in_menus();
			break;
			
			case PLAYING:
			playing_the_game();
			break;
		}
		free(sendmessage);
	}
	
	return 0;
}

void in_menus(){	
	if (menu_printed == 0)
	{
		print_menu(main_menu);
		menu_printed = 1;
	}
	direction i = NEUTRAL;
	joyValues j;
	read_joystick(&j);
	direction d = joystick_getDirection(j.x_percentage,j.y_percentage);
	//printf("Joystic dir: %d\n",d);
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
		arrow_line = 5;
	}
}


void playing_the_game(){
	send_joystick_data();
	joyValues j;
	read_joystick(&j);
	
	//Mottar melding om liv
	if(can_data_received() > 0) {
		can_message* receivemessage = malloc(sizeof(can_message));
		can_receive_message(receivemessage);
		if(receivemessage->ID == 3) {
			lives--;
			menu_playing(lives);
			score.byte_value[0] = receivemessage -> data[0];
			score.byte_value[1] = receivemessage -> data[1];
			set_high_score_list(score.int_value);
			printf("Number of lives is: %d", (3-receivemessage->data[0]));
			sendmessage->ID = 2;
			sendmessage->length = 1;
			sendmessage->data[0] = 0;
			can_send_message(sendmessage);
			pause = TRUE;
		}

		printf("\n");
		free(receivemessage);
	}
	
	// Avslutter spill om man er tom for liv eller trykker på høyre knapp
	if(j.right_button == 1){
		sendmessage->ID = 2;
		sendmessage->length = 1;
		sendmessage->data[0] = 0;
		can_send_message(sendmessage);
		restart_game_mode();
		game_initialized = FALSE;
		mode_t = MENU;
		arrow_line=5;
		button_action(arrow_line);
	}
	// Start again with left button
	if(j.left_button == 1) 
	{
		sendmessage->ID = 2;
		sendmessage->length = 1;
		sendmessage->data[0] = 1;
		can_send_message(sendmessage);
		pause = FALSE;
		
	}
	else if (lives == 0)
	{
		sendmessage->ID = 2;
		sendmessage->length = 1;
		sendmessage->data[0] = 0;
		can_send_message(sendmessage);
		game_initialized = FALSE;
		restart_game_mode();
		mode_t = MENU;
		button_action(5); //Går til main menu
		navigateMenu(6); //Går til highscore
	}
	
}


uint8_t send_joystick_data() {
	joyValues j;
	read_joystick(&j);
	int_union_bytes x_axis;
	int_union_bytes y_axis;
	uint8_t dir = (uint8_t) joystick_getDirection(j.x_percentage,j.y_percentage);
	x_axis.int_value = (int) j.x_percentage;
	y_axis.int_value = (int) j.y_percentage;
	uint8_t slider = joystick_get_right_slider();
	sendmessage->ID = 1;
	sendmessage->length=7;

	sendmessage->data[0] = j.left_button;
	sendmessage->data[1] = j.right_button;
	sendmessage->data[2] = j.joystick_button;
	sendmessage->data[3] = dir;
	sendmessage->data[4] = x_axis.byte_value[0];
	sendmessage->data[5] = x_axis.byte_value[1];
	//printf("X axis data 1: %d data 2: %d\n", (x_axis.bytes_axis[0]), (x_axis.bytes_axis[1]));
	sendmessage->data[6] = slider;
	return can_send_message(sendmessage);
	
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
