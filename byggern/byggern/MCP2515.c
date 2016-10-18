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
	volatile uint8_t value;
	
	spi_init();
	mcp_2515_reset();
	
	//Self-test
	value = mcp_2515_read(MCP_CANSTAT);
	if ((value & MODE_MASK) != MODE_CONFIG)
	{
		printf("MCP2515 is NOT in configuration mode after reset!\n");
		return 1;
	}
	printf("MCP2515 mode after init: %d\n", value);
	// Interrupt enable when receive buffer 0 is full
	mcp_2515_bit_modify(MCP_CANINTE,0x01,0x01);
	//More initialization??
	
	return 0;
}

uint8_t mcp_2515_enable_loopback()
{
	// Enable loopback mode in the CANCTRL-register
	mcp_2515_bit_modify(MCP_CANCTRL,MODE_MASK,MODE_LOOPBACK);
	volatile uint8_t value = mcp_2515_read(MCP_CANSTAT);
	// Check if loopback mode is enabled
	if ((value & MODE_MASK) != MODE_LOOPBACK)
	{
		return 1;
	}
	return 0;
}

uint8_t mcp_2515_enable_normal_operation()
{
	// Enable normal operation in the CANCTRL-register
	mcp_2515_bit_modify(MCP_CANCTRL,MODE_MASK,MODE_NORMAL);
	// Check if normal mode is enabled
	volatile uint8_t value = mcp_2515_read(MCP_CANSTAT);
	if ((value & MODE_MASK) != MODE_NORMAL)
	{
		return 1;
	}
	return 0;
}


uint8_t mcp_2515_read(uint8_t address){
	uint8_t result;
	// Enable slave
	spi_enable();
	
	// Send read-command and address to be read
	spi_send(MCP_READ); 
	spi_send(address);
	// Read back result
	result = spi_read();
	
	// Disable slave
	spi_disable();
	return result;
}

void mcp_2515_write(uint8_t address, uint8_t data){
	// Enable slave
	spi_enable();
	
	// Send write-command, address and data
	spi_send(MCP_WRITE);
	spi_send(address);
	spi_send(data);
	
	// Disable slave
	spi_disable();
}

void mcp_2515_request_to_send(uint8_t buffer){
	// Enable slave
	spi_enable();
	
	//// Check three last bits of buffer-byte
	//if(buffer <= 7) {
		//// Set the RTS-command with the register we want to send from
		//buffer = MCP_RTS | buffer;
	//}
	//// Invalid buffer, command will be ignored in MCP2515
	//else buffer = MCP_RTS;
	
	spi_send(buffer);
	
	// Disable slave
	spi_disable();
}

void mcp_2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data){
	// Enable slave
	spi_enable();
	
	// Send bitmodification command
	spi_send(MCP_BITMOD);
	// Send the address of the register we want to change
	spi_send(address);
	// Send the mask which decides what bits in the register can be modified
	spi_send(mask);
	// Send the new bit values
	// Only the bits enabled by the mask will be modified
	spi_send(data);
	
	// Disable slave
	spi_disable();
}

void mcp_2515_reset(){
	// Enable slave
	spi_enable();
	
	// Send reset-command
	spi_send(MCP_RESET);
	
	// Disable slave
	spi_disable();
}

uint8_t mcp_2515_read_status(){
	// Enable slave
	spi_enable();
	
	// Send read status command
	spi_send(MCP_READ_STATUS);
	// Read back the status from SPI
	char status = spi_read();
	
	// Disable the slave and return the result
	spi_disable();
	return status;
}
