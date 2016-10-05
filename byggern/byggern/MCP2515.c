/*
 * MCP2515.c
 *
 * Created: 05.10.2016 12:07:27
 *  Author: vegarkam
 */ 

#include "MCP2515.h"
#include "spi.h"
#include "USART.h"

uint8_t mcp_2515_init(){
	uint8_t value;
	
	spi_init();
	mcp_2515_reset();
	
	//Self-test
	value = mcp_2515_read(MCP_CANSTAT);
	if ((value & MODE_MASK) != MODE_CONFIG)
	{
		printf("MCP2515 is NOT in configuration mode after reset!\n");
		return 1;
	}
	// Interrupt enable when receive buffer 0 is full
	mcp_2515_bit_modify(MCP_CANINTE,0x01,0x01)
	//More initialization??
	
	return 0;
}

uint8_t mcp_2515_read(uint8_t address){
	uint8_t result;
	spi_enable();
	spi_send(MCP_READ); 
	spi_send(address);
	result = spi_read();
	spi_disable();
	return result;
}

void mcp_2515_write(uint8_t address, uint8_t data){
	spi_enable();
	spi_send(MCP_WRITE);
	spi_send(address);
	spi_send(data);
	spi_disable();
}

void mcp_2515_request_to_send(uint8_t buffer){
	spi_enable();
	spi_send(buffer);
	spi_disable();
}

void mcp_2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data){
	spi_enable();
	spi_send(MCP_BITMOD);
	spi_send(address);
	spi_send(mask);
	spi_send(data);
	spi_disable();
}

void mcp_2515_reset(){
	spi_enable();
	spi_send(MCP_RESET);
	spi_disable();
}

char mcp_2515_read_status(){
	spi_enable();
	spi_send(MCP_READ_STATUS);
	char status = spi_read();
	spi_disable();
	return status;
}
