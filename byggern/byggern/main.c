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
uint8_t truemaddafakka=1;
#include "menu.h"

int main(void)
{
	initializations();
	menu_t * main_menu = menu_init();
	print_menu(main_menu);
	
	int mode = 0;
	while (truemaddafakka)
	{
		
		switch(mode){
			case 0:
			in_menus();
			break;
			
			case 1:
			playing_the_game();
			break;
		}
		//joyValues j;
		//read_joystick(&j);
		//float slider_left = joystick_get_left_slider_percentage();
		//float slider_right = joystick_get_right_slider_percentage();
		//direction d = joystick_getDirection(j.x_percentage,j.y_percentage);
		//int d1 = (int)d;
		//
		//printf("X prosent: %3.2f, Y prosent: %3.2f, Slider venstre: %3.2f, Slider høyre: %3.2f, Retning: %d ",j.x_percentage,j.y_percentage,slider_left,slider_right, d1);
		//printf("Knapp 1: %d, Knapp 2: %d, Joystick knapp: %d\n",j.left_button,j.right_button,j.joystick_button);
	}
	
	return 0;
}

void in_menus(){
	joyValues j;
	read_joystick(&j);
	direction d = joystick_getDirection(j.x_percentage,j.y_percentage);
	
	move_arrow(d,getCurrentLine());
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
}