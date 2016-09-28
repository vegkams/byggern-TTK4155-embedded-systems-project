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
	char * name;
}menu_t;

menu_t * menu_init();
void print_menu (menu_t *menu);
void move_arrow(direction dir, uint8_t current_row);

#endif /* MENU_H_ */