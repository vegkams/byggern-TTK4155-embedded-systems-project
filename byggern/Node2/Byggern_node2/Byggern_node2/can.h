/*
 * can.h
 *
 * Created: 05.10.2016 12.58.42
 *  Author: vegard
 */ 

#include <avr/io.h>
#include <stdio.h>

#ifndef CAN_H_
#define CAN_H_

typedef struct can_message {
	unsigned int ID;
	uint8_t length;
	uint8_t data[8];
	} can_message;

uint8_t can_transmit_complete();
uint8_t can_error();
uint8_t can_data_received();
can_message* can_receive_message();
uint8_t can_send_message(can_message *can_message);
uint8_t CAN_enable_normal_mode();
uint8_t CAN_enable_loopback();
uint8_t can_init();

#endif /* CAN_H_ */