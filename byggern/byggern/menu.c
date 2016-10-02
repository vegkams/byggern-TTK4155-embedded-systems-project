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
#include <string.h>

direction previousDIR = NEUTRAL;
// Variabel for å holde på nåværende meny, kan være nyttig i main f. eks.
menu_t * current_menu;
difficulty GitGud = EASY;
uint8_t game_started = 0;

menu_t * menu_init (){
	//Main menu
	menu_t * main_menu = malloc(sizeof(menu_t));
	main_menu ->name = "MAIN MENU";
	
	main_menu -> child1 = malloc(sizeof(menu_t));
	main_menu -> child1 -> name = "NEW GAME";
	main_menu -> child1 -> parent = main_menu;
	main_menu -> child1 -> number_of_children = 0;
	
	main_menu -> child2 = malloc(sizeof(menu_t));
	main_menu -> child2->name = "HIGH SCORE";
	main_menu -> child2->parent = main_menu;
	main_menu -> child2 -> number_of_children = 1;
	
	main_menu -> child3 = malloc(sizeof(menu_t));
	main_menu -> child3->name = "SETTINGS";
	main_menu -> child3->parent = main_menu;
	main_menu ->child4 = NULL;

	main_menu -> number_of_children = 3;
	
	//High score menu
	main_menu ->child2->child1 = malloc(sizeof(menu_t));
	main_menu ->child2->child1->name = "RETURN";
	main_menu ->child2->child1->parent = main_menu ->child2;
	main_menu ->child2->child1->number_of_children = 0;
	main_menu ->child2->child2 =main_menu ->child2->child3=main_menu ->child2->child4 = NULL;
	
	//Settings menu
	main_menu ->child3->child1 = malloc(sizeof(menu_t));
	main_menu ->child3->child1->name = "SET DIFFICULTY";
	main_menu ->child3->child1->parent = main_menu->child3;
	
	// Difficulty settings
	main_menu -> child3 -> child1 -> child1 = malloc(sizeof(menu_t));
	main_menu -> child3 -> child1 -> child1 -> name = "EASY";
	main_menu -> child3 -> child1 -> child1 ->parent = main_menu ->child3 ->child1;
	main_menu -> child3 -> child1 -> child2 = malloc(sizeof(menu_t));
	main_menu -> child3 -> child1 -> child2 -> name = "MEDIUM";
	main_menu -> child3 -> child1 -> child2 ->parent = main_menu ->child3 ->child1;
	main_menu -> child3 -> child1 -> child3 = malloc(sizeof(menu_t));
	main_menu -> child3 -> child1 -> child3 -> name = "HARD";
	main_menu -> child3 -> child1 -> child3 ->parent = main_menu ->child3 ->child1;
	main_menu -> child3 -> child1 -> child4 = malloc(sizeof(menu_t));
	main_menu -> child3 -> child1 -> child4 -> name = "RETURN";
	main_menu -> child3 -> child1 -> child4 ->parent = main_menu ->child3 ->child1;
	main_menu -> child3 -> child1 -> number_of_children = 4;
	
	// Debugging (use?)
	main_menu -> child3 -> child2 = malloc(sizeof(menu_t));
	main_menu -> child3 -> child2 -> name = "DEBUGGING";
	main_menu -> child3 -> child2 -> parent = main_menu->child3;
	// Calibrate joystick
	main_menu -> child3 -> child3 = malloc(sizeof(menu_t));
	main_menu -> child3 -> child3->name = "CAL. JOYSTICK";
	main_menu -> child3 -> child3->parent = main_menu->child3;
	// Return to main menu
	main_menu -> child3 -> child4 = malloc(sizeof(menu_t));
	main_menu -> child3 -> child4 -> name = "RETURN";
	main_menu -> child3 -> child4 -> parent = main_menu->child3;
	
	main_menu -> child3 -> number_of_children = 4;
	current_menu = main_menu;
	return main_menu;
}
uint8_t move_arrow(direction dir ,uint8_t current_row){
	uint8_t number_of_rows = current_menu -> number_of_children;
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
		
		if (current_row <= 3 + number_of_rows)
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
	
	if (menu->child2 != NULL)
	{
		oled_pos(3,2);
		oled_print_string(menu->child2->name);
	}

	if (menu->child3 != NULL)
	{
		oled_pos(4,2);
		oled_print_string(menu->child3->name);
	}
	if (menu->child4 != NULL)
	{
		oled_pos(5,2);
		oled_print_string(menu->child4->name);
	}
	
	// Print the current difficulty setting.
	oled_pos(7,2);
	oled_print_string(("DFFCLTY: %s", stringFromDifficulty(GitGud)));
}

difficulty get_difficulty()
{
	return GitGud;
}

// Implements action based on joystick button presses
// Possibility for cleaner code, written in a hurry :-)
uint8_t button_action (uint8_t current_line) {
	// Fix "magic numbers", current_line = some menu etc.
	// Should be independent of the actual linked list
	// Change the struct to an array of children, and use current_line as index?
	// Think that should work
	if (strcmp(current_menu->name, "MAIN MENU") == 0)
	{
		if(current_line == 2)
		{
			game_started == 1;
		}
		else if (current_line == 3) {
			print_highscore();
			navigateMenu(current_line);
		}
		else navigateMenu(current_line);
	}
	else if (strcmp(current_menu->name, "SETTINGS") == 0)
	{
		if (current_line == 2)
		{
			navigateMenu(current_line);
		}
		else if (current_line == 3); // TODO Debugging
		
		else if (current_line == 4); // TODO Calibrate joystick
		
		else if (current_line == 5) {
			current_menu == current_menu -> parent;
			print_menu(current_menu);
		}
		
	}
	else if (strcmp(current_menu->name, "SET DIFFICULTY") == 0) {
		if (current_line == 2) GitGud == EASY;
		else if (current_line == 3) GitGud == MEDIUM;
		else if (current_line == 4) GitGud == HARD;
		current_menu == current_menu->parent;
		print_menu(current_menu);
	}
	return game_started;
}
	

// Traverses the linked list into submenus
void navigateMenu(uint8_t current_line) {
	switch(current_line) {
		case 2:
		if (current_menu->child1 != NULL) {
			current_menu = current_menu -> child1;
			oled_pos(current_line, 0);
			oled_print_string("  ");
			move_arrow(NEUTRAL, 2);
			print_menu(current_menu);
		}
		break;
		
		case 3:
		if (current_menu->child2 != NULL) {
			current_menu = current_menu -> child2;
			oled_pos(current_line, 0);
			oled_print_string("  ");
			move_arrow(NEUTRAL, 2);
			print_menu(current_menu);
		}
		break;
		
		case 4:
		if (current_menu->child3 != NULL) {
			current_menu = current_menu -> child3;
			oled_pos(current_line, 0);
			oled_print_string("  ");
			move_arrow(NEUTRAL, 2);
			print_menu(current_menu);
		}
	}
	
}

// TODO save highscore in EEPROM??
void print_highscore() {
	
}




