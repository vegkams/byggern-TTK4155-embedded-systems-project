/*
 * joystick.h
 *
 * Created: 21.09.2016 08:57:07
 *  Author: vegarkam
 */ 


#ifndef JOYSTICK_H_
#define JOYSTICK_H_

typedef enum {UP,DOWN,LEFT,RIGHT,NEUTRAL} direction;
	
typedef struct joyValues{
	float x_percentage;
	float y_percentage;
	int left_button;
	int right_button;
	int joystick_button;
}joyValues;

void joystick_init();
void read_joystick(joyValues *joy);
float joystick_get_x_percentage();
float joystick_get_y_percentage();
float joystick_get_x_voltage();
float joystick_get_y_voltage();
float joystick_get_left_slider_percentage();
float joystick_get_right_slider_percentage();
float convert_to_voltage(uint8_t digital_value);
float convert_to_percentage(uint8_t digital_value);
direction joystick_getDirection(float x, float y);
float convert_to_sliderpercentage(uint8_t digital_value);
static inline char *stringFromDirection(direction d)
{
	static const char *strings[] = { "UP", "DOWN", "LEFT", "RIGHT", "NEUTRAL"};
	return strings[d];
}
	



#endif /* JOYSTICK_H_ */