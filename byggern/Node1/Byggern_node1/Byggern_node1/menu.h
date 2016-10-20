/*
 * menu.h
 *
 * Created: 28.09.2016 11:52:24
 *  Author: vegarkam
 */ 


#ifndef MENU_H_
#define MENU_H_
#include "joystick.h"

typedef struct menu{
	struct menu * parent;
	struct menu * child1;
	struct menu * child2;
	struct menu * child3;
	struct menu * child4;
	uint8_t number_of_children;
	char * name;
}menu_t;

typedef enum {EASY, MEDIUM, HARD} difficulty;
	
static inline char *stringFromDifficulty(difficulty d)
{
	static const char *strings[] = { "EASY", "MEDIUM", "HARD"};
	return strings[d];
}

menu_t * menu_init();
void print_menu (menu_t *menu);
uint8_t move_arrow(direction dir, uint8_t current_row);
difficulty get_difficulty();
uint8_t button_action(uint8_t current_line);
void print_highscore();

#endif /* MENU_H_ */