/*
* main.c
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
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
menu_t *        main_menu;
uint8_t         arrow_line = 2;
uint8_t         previous_joystick_button;
uint8_t         game_on = 0;
mode            mode_t = MENU;
uint8_t         menu_printed;
uint8_t         lives = 3;
uint8_t         pause = TRUE;
int_union_bytes score;
uint8_t         game_initialized = 0;
can_message *   sendmessage;
uint8_t         can_flag;
uint8_t         counter;
int             time_seconds;
joyValues       j;

int main(void)
{
	initializations();
	menu_printed = 0;
	while (TRUE)
	{
		sendmessage = malloc(sizeof(can_message));

		if(get_game_mode() == 0){
			mode_t = MENU;
		}
		else if(get_game_mode() == 1){
			mode_t = PLAYING;
			if (!game_initialized)
			{
				enable_can_timer();
				// Send game start message to node 2
				sendmessage->ID = 2;
				sendmessage->length = 1;
				sendmessage->data[0] = 1;
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
	read_joystick(&j);
	direction d = joystick_getDirection(j.x_percentage,j.y_percentage);
	arrow_line = move_arrow(d,arrow_line);
	// Register button on rising edge
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
	send_joystick_data();
	menu_print_played_time(time_seconds);
	if(can_data_received() > 0) {
		// Receive the message
		can_message* receivemessage = malloc(sizeof(can_message));
		can_receive_message(receivemessage);
		if(receivemessage->ID == 3) {
			// Message ID = 3 -> Loss of point in game
			lives--;
			menu_playing(lives);
			// Receive played time
			score.byte_value[0] = receivemessage -> data[0];
			score.byte_value[1] = receivemessage -> data[1];
			// Update high score list with new score (if new highscore)
			set_high_score_list(score.int_value);
			// Send message back informing node 2 that we're not playing anymore
			sendmessage->ID = 2;
			sendmessage->length = 1;
			sendmessage->data[0] = 0;
			can_send_message(sendmessage);
			pause = TRUE;
			// Disable the timer interrupt for sending joystick data while paused
			disable_can_timer();
		}

		printf("\n");
		free(receivemessage);
	}
	
	// Exit game if right button is pressed
	if(j.right_button == 1){
		disable_can_timer();
		menu_reset_played_time();
		// Tell node 2 to stop the game
		sendmessage->ID = 2;
		sendmessage->length = 1;
		sendmessage->data[0] = 0;
		can_send_message(sendmessage);
		restart_game_mode();
		game_initialized = FALSE;
		mode_t = MENU;
		arrow_line=2;
		lives = 3;
		button_action(arrow_line);
	}
	// Start again with joystick button
	if(j.joystick_button == 1 && pause)
	{
		enable_can_timer();
		menu_reset_played_time();
		time_seconds = 0;
		sendmessage->ID = 2;
		sendmessage->length = 1;
		sendmessage->data[0] = 1;
		can_send_message(sendmessage);
		pause = FALSE;
		
	}
	else if (lives == 0) // Out of lives, game lost
	{
		disable_can_timer();
		sendmessage->ID = 2;
		sendmessage->length = 1;
		sendmessage->data[0] = 0;
		can_send_message(sendmessage);
		game_initialized = FALSE;
		restart_game_mode();
		mode_t = MENU;
		lives = 3;
		time_seconds = 0;
		menu_reset_played_time();
		button_action(2); //Go to main menu
		navigateMenu(3); //Go to highscore
	}
}

/*
* Read ADC, send joystick data to node 2
*/
uint8_t send_joystick_data() {
	// If flag is set by timer
	if (can_flag)
	{
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
		sendmessage->data[6] = slider;
		return can_send_message(sendmessage);
		can_flag = FALSE;
	}

	
}


void initializations(){
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
	printf("Enable CAN normal mode: %d\n",CAN_enable_normal_mode());
	// Run CAN timer on 50Hz
	//Trigger interrupt with interval of 50hz
	OCR1A = 1536;

	//Enable CTC mode
	set_bit(TCCR1A,COM1A0);

	//Prescale 64
	set_bit(TCCR1B, CS11);
	set_bit(TCCR1B, CS10);
	set_bit(TCCR1B, WGM12);
	
	//Enable global interrupts	
	sei();

}

void enable_can_timer()
{
	// Enable interrupt on output compare match timer 1
	set_bit(TIMSK, OCIE1A);
}
void disable_can_timer()
{
	// Disable interrupt on output compare match timer 1
	clear_bit(TIMSK, OCIE1A);

}

// ISR for CAN timre
ISR(TIMER1_COMPA_vect )
{
	// Enable sending of joystick data
	can_flag = TRUE;
	counter++;
	if (counter == 50)
	{
		// 1 second passed
		counter = 0;
		time_seconds ++;
		menu_reset_played_time();
		menu_print_played_time(time_seconds);
	}
	
}