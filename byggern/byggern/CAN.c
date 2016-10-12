/*
* CAN.c
*
* Created: 05.10.2016 15:02:26
*  Author: vegarkam
*/
#include "can.h"
#include "MCP2515.h"

uint8_t can_init(){
	mcp_2515_init();
	
}
/************************************************************************/
/* BUILD AND SEND CAN MESSAGE                                                                     */
/************************************************************************/
uint8_t can_send_message(can_message *can_message){
	unsigned int buffer = MCP_TXB0D;
	int id= can_message->ID;

	
	//Create array to store one byte each from a 16-bit integer
	uint8_t array[2];
	//stores the high part
	array[0]= ID & 0xFF;
	//stores the low part
	array[1] = (ID >> 8);
	
	//writes the ID to the ID High and ID LOW
	mcp_2515_write(MCP_TXB0SIDH, array[0]);
	mcp_2515_write(MCP_RXF0SIDL, array[1]);
	
	mcp_2515_write(MCP_TXB0DLC, can_message->length);
	for(uint8_t i=0; i< can_message->length-1;i++){
		mcp_2515_write(buffer, can_message->data[i]);
		buffer++;
	}
	
	mcp_2515_request_to_send(MCP_RTS_TX0);
	clear_bit(MCP_TXB0D,);
	return 0;
}




uint8_t can_error(){
	return 0;
}
uint8_t can_transmitt_complete(){
	return 0;
}
uint8_t can_data_received(){
	return 0;
}
uint8_t can_int_vect(){
	return 0;
}
