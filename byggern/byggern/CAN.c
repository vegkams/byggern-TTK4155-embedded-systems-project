/*
* CAN.c
*
* Created: 05.10.2016 15:02:26
*  Author: vegarkam
*/

#include "can.h"
#include "MCP2515.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "can.h"
#include "MCP2515.h"
#include "setup.h"
#define CAN_SEND_STATUS_MASK 0x78
#define CAN_LOW_ID_MASK 0xE0
#define RXB0_INTERRUPT 0x01

uint8_t message_received = FALSE;
uint8_t transmit_complete = FALSE;


uint8_t can_init(){
	mcp_2515_init();
	

	// Mask/filter off, no rollover in receive buffer
	mcp_2515_bit_modify(MCP_RXB0CTRL, 0x64, 0x60);
	
	// Interrupt enable when receive buffer 0 is full
	mcp_2515_bit_modify(MCP_CANINTE,0xFF,MCP_RX0IF);
	
	// Enable interrupt on INT1
	set_bit(GICR,INT1);
	// Interrupt on falling edge INT1
	set_bit(MCUCR,ISC11);
	clear_bit(MCUCR,ISC10);
	
	// Enable global interrupts
	sei();
	return 0;

}


// Enable loopback mode, for testing without node 2
uint8_t CAN_enable_loopback() {
	return mcp_2515_enable_loopback();
}

// Enable normal operation for transmission between node 1 and node 2
uint8_t CAN_enable_normal_mode() {
	return mcp_2515_enable_normal_operation();
}
/************************************************************************/
/* BUILD AND SEND CAN MESSAGE                                                                     */
/************************************************************************/
uint8_t can_send_message(can_message *can_message){
	// Check if previous message was sent
	if (can_transmit_complete()) {
		int id= can_message->ID;

		
		uint8_t low_ID = id & 0xFF;
		
		// Standard frame, mask out lowest five bits of low_ID
		low_ID = low_ID & CAN_LOW_ID_MASK;
		uint8_t high_ID = (id >> 8);
		
		//writes the ID to the ID High and ID LOW
		mcp_2515_write(MCP_TXB0SIDH, high_ID);
		mcp_2515_write(MCP_TXB0SIDL, low_ID);
		
		mcp_2515_write(MCP_TXB0DLC, can_message->length);
		
		for(uint8_t i=0; i< can_message->length-1;i++){
			mcp_2515_write(MCP_TXB0D+i, can_message->data[i]);
		}
		
		mcp_2515_request_to_send(MCP_RTS_TX0);
	}
	
	else {
		if (can_error() > 0) {
			printf("CAN MESSAGE SEND FAILED!");
			return -1;
		}
	}

	return 0;
}
/*************************************/
/* RECEIVE AND CONSTRUCT CAN MESSAGE */
/*************************************/

can_message* can_receive_message() {
	can_message *the_message;
	int id;
	// Check if received flag was set
	if(message_received) {
		id = mcp_2515_read(MCP_RXB0SIDH) << 8 | mcp_2515_read(MCP_RXB0SIDL);
		// Mask out lowest 5 bits (only used for extended frames)
		the_message->ID = id & 0xFFC0;
		the_message->length = mcp_2515_read(MCP_RXB0DLC);
		for(int i = 0; i < the_message->length; i++) {
			the_message->data[i] = mcp_2515_read(MCP_RXB0D+i);
		}
		
		message_received = FALSE;
	}
	else {
		// No message in the buffer
		the_message->length = 0;
	}
	return the_message;
	
}

// Check if there is a pending message in the receive buffer
uint8_t can_data_received(){
	return message_received;
}
// Check error flags in the transmit buffer control register TXB0CTRL
uint8_t can_error(){
	uint8_t error_flags = mcp_2515_read(MCP_TXB0CTRL);
	if(test_bit(error_flags,5)) return 1;
	if(test_bit(error_flags,4)) return 2;
	return FALSE;
}


uint8_t can_transmit_complete(){
	if (test_bit(mcp_2515_read(MCP_TXB0CTRL),3)) {
		return FALSE;
	}
	else return TRUE;
}


ISR(INT1_vect) {
	volatile uint8_t interrupt_code = mcp_2515_read(MCP_CANINTF);
	if(interrupt_code == RXB0_INTERRUPT) {
		// Reset Receive buffer full interrupt flag
		mcp_2515_bit_modify(MCP_CANINTF, MCP_RX0IF,0);
		message_received = TRUE;
	}
	
}

