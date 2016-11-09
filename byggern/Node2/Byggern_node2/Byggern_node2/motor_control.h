/*
 * motor_control.h
 *
 * Created: 09.11.2016 12:26:53
 *  Author: vegarkam
 */ 


#ifndef MOTOR_CONTROL_H_
#define MOTOR_CONTROL_H_
#include <stdint.h>
#define RST PH6
#define OE PH5
#define EN PH4
#define DIR PH1
#define SEL PH3
#define MJ2 PINC
#define MJ1 PORTH


typedef enum MotorDir MotorDir;
enum MotorDir {RIGHT,LEFT}; 
uint8_t motor_control_init();
int read_encoder();
void set_motor_direction(MotorDir dir);
void motor_control_set_speed(uint8_t speed);
void enable_motor(uint8_t enable);
void enable_encoder(uint8_t enable);
void encoder_reset();
void motor_control_set_velocity(float velocity);
uint8_t reverse_bits(char x);
float saturate(vel, min, max);

	


#endif /* MOTOR_CONTROL_H_ */