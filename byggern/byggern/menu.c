/*
* menu.c
*
* Created: 28.09.2016 11:52:07
*  Author: vegarkam
*/

#include <avr/io.h>
#include <stdio.h>
#include "setup.h"
#include "OLED.h"
#include "menu.h"

direction previousDIR =NEUTRAL;
menu_t * menu_init (){
	//Main menu
	menu_t * main_menu = malloc(sizeof(menu_t));
	main_menu ->name = "MAIN MENU";
	
	main_menu ->child1 = malloc(sizeof(menu_t));
	main_menu ->child1->name = "NEW GAME";
	main_menu ->child1->parent = main_menu;
	
	main_menu ->child2 = malloc(sizeof(menu_t));
	main_menu ->child2->name = "HIGH SCORE";
	main_menu ->child2->parent = main_menu;
	
	main_menu ->child3 = malloc(sizeof(menu_t));
	main_menu ->child3->name = "SETTINGS";
	main_menu ->child3->parent = main_menu;
	
	//Settings menu
	main_menu ->child3->child1 = malloc(sizeof(menu_t));
	main_menu ->child3->child1->name = "SET DIFFICULTY";
	main_menu ->child3->child1->parent = main_menu->child3;
	
	main_menu ->child3->child2 = malloc(sizeof(menu_t));
	main_menu ->child3->child2->name = "DEBUGGING";
	main_menu ->child3->child2->parent = main_menu->child3;
	
	main_menu ->child3->child3 = malloc(sizeof(menu_t));
	main_menu ->child3->child3->name = "CAL. JOYSTICK";
	main_menu ->child3->child3->parent = main_menu->child3;
	return main_menu;
}
unsigned int move_arrow(direction dir ,uint8_t current_row){
	if(dir == previousDIR){
		dir = NEUTRAL;
	}
	else{
		previousDIR=NEUTRAL;
	}
	switch(dir) {
		case NEUTRAL :
		oled_print_arrow(current_row,0);
		break;
		
		case UP  :
		if (current_row >= 3)
		{
			oled_pos(current_row, 0);
			oled_print_string("  ");
			current_row--;
			oled_print_arrow(current_row,0);
			previousDIR=UP;
		}

		break;
		
		case DOWN  :
		
		if (current_row <= 6)
		{
			oled_pos(current_row, 0);
			oled_print_string("  ");
			current_row++;
			oled_print_arrow(current_row,0);
			previousDIR=DOWN;
		}	
		break;
	}
	return current_row;
}

void print_menu (menu_t *menu){
	menu_t * current = menu;
	oled_reset();
	oled_home();
	oled_pos(0,2);
	oled_print_string(menu->name); //Skriver ut overskrift
	
	if (menu->child1 != NULL)
	{
		oled_pos(2,2);
		oled_print_string(menu->child1->name);
	}
	
	if (menu->child1 != NULL)
	{
		oled_pos(3,2);
		oled_print_string(menu->child2->name);
	}

	if (menu->child1 != NULL)
	{
		oled_pos(4,2);
		oled_print_string(menu->child3->name);
	}
}

