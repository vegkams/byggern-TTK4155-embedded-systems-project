/*
 * can.h
 *
 * Created: 05.10.2016 12.58.42
 *  Author: vegard
 */ 


#ifndef CAN_H_
#define CAN_H_

typedef struct can_message {
	unsigned int ID;
	uint8_t length;
	uint8_t data[8]; 
	} can_message;



#endif /* CAN_H_ */