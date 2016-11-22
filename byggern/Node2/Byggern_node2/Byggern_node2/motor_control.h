/*
 * motor_control.h
 *
 * Created: 09.11.2016 12:26:53
 *  Author: vegarkam
 */ 


#ifndef MOTOR_CONTROL_H_
#define MOTOR_CONTROL_H_
#include <avr/io.h>
#define RST PH6
#define OE PH5
#define EN PH4
#define DIR PH1
#define SEL PH3
#define MJ2 PINK
#define MJ1 PORTH

uint8_t motor_control_init();
void motor_control_init_clock();
unsigned int read_encoder();
void set_motor_direction(uint8_t dir);
void motor_control_set_speed(uint8_t speed);
void enable_motor(uint8_t enable);
void enable_encoder(uint8_t enable);
void encoder_reset();
void motor_control_set_velocity(int velocity);
int saturate(int vel);
unsigned int find_encoder_max();
void motor_control_reset_timer();
int motor_control_get_played_time();
void motor_control_set_playing_flag(uint8_t flag);
void motor_control_set_timer_flag(uint8_t flag);
void motor_control_set_reference_pos(int pos);
void motor_control_set_pid_gains(float p, float i, float d);
void motor_control_reset_integrator();
void calculate_pid();

#endif /* MOTOR_CONTROL_H_ */