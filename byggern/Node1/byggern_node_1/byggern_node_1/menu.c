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
#include "joystick.h"
#define OLED_OFFSET 0

direction    previousDIR = NEUTRAL;
menu_t *     current_menu;
menu_t       Mainmenu,Highscore, Newgame, Settings, Return;
uint8_t      game_started = 0;
char         first_place[8];
char         second_place[8];
char         third_place[8];
char         lives_int[8];
char         time_played[8];
int          high_score[3];
const char   playing[] = "GAME PLAYING";
const char   number_of_lives[] = "LIVES LEFT:";

menu_t * menu_init (){
	//initialize empty score list
	for (int i = 0; i < 3; i++)
	{
		high_score[i] = 0;
	}
	sort_list(high_score,3);
	//Main menu
	Mainmenu.name = "MAIN MENU";
	Mainmenu.child1 = &Newgame;
	Mainmenu.child2 = &Highscore;
	Mainmenu.child3 = &Settings;
	Mainmenu.number_of_children = 3;
	
	Newgame.name = "NEW GAME";
	Newgame.parent = &Mainmenu;
	Newgame.number_of_children = 1;
	
	Highscore.name = "HIGH SCORE";
	Highscore.parent = &Mainmenu;
	Highscore.number_of_children = 4;
	Highscore.child4 = &Return;
	Settings.name = "SETTINGS";
	Settings.parent = &Mainmenu;
	Settings.number_of_children = 3;
	Settings.child3 = &Return;
	Return.name = "RETURN";
	Return.parent = &Mainmenu;
	current_menu = &Mainmenu;
	// Get integer as string
	itoa(high_score[0],first_place,10);
	itoa(high_score[1],second_place,10);
	itoa(high_score[2],third_place,10);
	return &Mainmenu;
}


// Move the arrow on the OLED-screen
uint8_t move_arrow(direction dir ,uint8_t current_row){
	uint8_t row = current_row;
	uint8_t number_of_rows = current_menu -> number_of_children + OLED_OFFSET;
	if(dir == previousDIR){
		dir = NEUTRAL;
	}
	else{
		previousDIR = NEUTRAL;
	}
	switch(dir) {
		case NEUTRAL :
		{
			oled_print_arrow(row,0);
			break;
		}
		
		case UP  :
		{
			if (row >= 3)
			{
				oled_pos(row, 0);
				oled_print_string("  ");
				row--;
				oled_print_arrow(row,0);
				previousDIR=UP;
			}
			break;
		}
		
		case DOWN  :
		{
			if (row <= number_of_rows)
			{
				oled_pos(row, 0);
				oled_print_string("  ");
				if (row == 7)
				{
					row = 0;
				}
				row++;
				oled_print_arrow(row,0);
				previousDIR = DOWN;
			}
			break;
		}
	}
	return row;
}

// Print the current menu on the OLED
void print_menu (menu_t *menu){
	menu_t * current = menu;
	oled_reset();
	oled_home();
	oled_pos(0,2);
	oled_print_string(menu->name);
	
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
	
	if (strcmp(menu->name, "HIGH SCORE") == 0)
	{
		print_highscore();
	}
	
	if (strcmp(menu->name, "SETTINGS") == 0)
	{
		print_settings();
	}
	
}


// Implements action based on joystick button presses
// Possibility for cleaner code, written in a hurry :-)
uint8_t button_action (uint8_t current_line) {
	// Fix "magic numbers", current_line = some menu etc.
	// Should be independent of the actual linked list
	
	if (strcmp(current_menu->name, "MAIN MENU") == 0)
	{
		if(current_line == 2)
		{
			game_started = 1;
			navigateMenu(current_line);
			menu_playing(3);
		}
		else if (current_line == 3) {
			navigateMenu(current_line);
		}
		else navigateMenu(current_line);
	}
	
	else if(strcmp(current_menu->name, "NEW GAME") == 0){
		if(current_line==2){
			current_menu=current_menu->parent;
			oled_pos(current_line, 0);
			oled_print_string("  ");
			print_menu(current_menu);
		}
	}
	
	else if (strcmp(current_menu->name, "SETTINGS") == 0)
	{
		// On debugging line
		if (current_line == 2) {
			oled_pos(current_line, 0);
			oled_print_string("  ");
		} // TODO Debugging
		
		
		// On calibrate joystick line
		else if (current_line == 3) {
			oled_pos(current_line, 0);
			oled_print_string("  ");
			calibrate_joystick();
			current_menu = current_menu->parent;
			
		}
		// Return on line 4
		else if (current_line == 4) {
			current_menu = current_menu -> parent;
			oled_pos(current_line, 0);
			oled_print_string("  ");
			print_menu(current_menu);
		}
		
	}
	
	else if (strcmp(current_menu->name, "HIGH SCORE") == 0) {
		// Return on line 5
		if (current_line == 5)
		{
			current_menu = current_menu -> parent;
		}
		oled_pos(current_line, 0);
		oled_print_string("  ");
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

// Print the highscore list
void print_highscore()
{
	oled_pos(2,2);
	oled_print_string("1: ");
	oled_pos(2,6);
	oled_print_string(first_place);
	oled_pos(3,2);
	oled_print_string("2: ");
	oled_pos(3,6);
	oled_print_string(second_place);
	oled_pos(4,2);
	oled_print_string("3: ");
	oled_pos(4,6);
	oled_print_string(third_place);
	oled_pos(5,2);
	oled_print_string("RETURN");
}

// Print settings menu
void print_settings()
{
	oled_pos(2,2);
	oled_print_string("DEBUGGING");
	oled_pos(3,2);
	oled_print_string("CAL. JOYSTCK.");
	oled_pos(4,2);
	oled_print_string("RETURN");
}

int get_game_mode(){
	return game_started;
}
void restart_game_mode(){
	game_started = 0;
}

// Insert new score into the highscore list (if new score is high score)
void set_high_score_list(int score)
{
	for (uint8_t i=0; i < 3; i++)
	{
		if (score > high_score[i]) // insert new highscore in correct position in list
		{
			if (i == 2) // Last element, simply replace
			{
				high_score[i] = score;
			}
			else // Move lower values down the list, insert in correct position
			{
				int k = 2;
				while(k > i)
				{
					high_score[k] = high_score[k-1];
					k--;
				}
				high_score[i] = score;
			}
			break;
		}
		
	}
	// Sort list just in case
	sort_list(high_score,3);

	// Convert integers to strings
	itoa(high_score[0], first_place,10);
	itoa(high_score[1], second_place,10);
	itoa(high_score[2], third_place,10);
}

// Insertion sort in descending order
void sort_list(int * list[], int size)
{
	int length = size;
	uint8_t d;
	int t;
	for (uint8_t c = 1 ; c <= length - 1; c++) {
		d = c;
		
		while ( d > 0 && list[d] > list[d-1]) {
			t          = list[d];
			list[d]    = list[d-1];
			list[d-1]  = t;
			
			d--;
		}
	}
}

// Print game playing menu
void menu_playing(uint8_t lives)
{
	oled_reset();
	oled_home();
	oled_pos(3,2);
	oled_print_string(playing);
	oled_pos(5,2);
	oled_print_string(number_of_lives);
	oled_pos(5,sizeof(number_of_lives)+1);
	oled_print_string(itoa(lives,lives_int,10));
}

// Print the elapsed time
void menu_print_played_time(int time)
{
	oled_pos(6,2);
	oled_print_string("TIME:");
	oled_pos(6,8);
	oled_print_string(itoa(time,time_played,10));
}

// Clear the elapsed time on OLED
void menu_reset_played_time()
{
	oled_pos(6,8);
	oled_print_string("     ");
}
