/*
 * pwm.h
 *
 * Created: 02.11.2016 10:45:30
 *  Author: vegarkam
 */ 

#include <avr/io.h>
#include <stdio.h>

#ifndef PWM_H_
#define PWM_H_


uint8_t pwm_init();
uint8_t pwm_set_angle(int angle,uint8_t mode);

#endif /* PWM_H_ */