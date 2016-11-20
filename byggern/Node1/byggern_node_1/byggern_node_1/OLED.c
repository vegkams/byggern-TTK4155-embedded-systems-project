/*
* OLED.c
*
* Created: 24.09.2016 12:51:06
*  Author: vegarkam
*/
#include <avr/io.h>
#include <stdio.h>
#include "font.h"
#include "setup.h"
#include "OLED.h"

// Command adress space 0x1000 - 0x1200
volatile char *command_address = (char *) 0x1000;
// Data adress space 0x1200 - 0x1400
volatile char *data_address = (char *) 0x1200;
uint8_t currentLine;
uint8_t current_column;

uint8_t getCurrentLine(){
	return currentLine;
}

// Init functions copied from datasheet
void oled_init(){
	
	
	write_c(0xae);        //  display  off
	write_c(0xa1);        //segment  remap
	write_c(0xda);        //common  pads  hardware:  alternative
	write_c(0x12);
	write_c(0xc8);        //common output scan direction:com63~com0
	write_c(0xa8);        //multiplex  ration  mode:63
	write_c(0x3f);
	write_c(0xd5);        //display divide ratio/osc. freq. mode
	write_c(0x80);
	write_c(0x81);        //contrast  control
	write_c(0x50);
	write_c(0xd9);        //set  pre-charge  period
	write_c(0x21);
	write_c(0x20);        //Set  Memory  Addressing  Mode
	write_c(0x02);
	write_c(0xdb);        //VCOM  deselect  level  mode
	write_c(0x30);
	write_c(0xad);        //master  configuration
	write_c(0x00);
	write_c(0xa4);        //out  follows  RAM  content
	write_c(0xa6);        //set  normal  display
	write_c(0xaf);        //  display  on
	oled_reset();
	oled_home();
	
}

// Write single command byte
void write_c(unsigned char char_command){
	*command_address= char_command;
}

// Write single data byte
void write_d(unsigned char char_data){
	*data_address = char_data;
}

// Clear entire display
void oled_reset(){
	for (int i = 0; i<8;i++){
		oled_clear_line(i);
	}
}

void oled_home(){
	write_c(0x21);//starting command(set column address)
	write_c(0x00);//start of column address
	write_c(0x7F);//end of column address
	
	write_c(0x22);//starting command (set page address)
	write_c(0x00);// start of page address
	write_c(0x07);//end of page address
	currentLine=0;
}

void oled_goto_line(uint8_t line){
	if (line < 8){
		currentLine=line;
		write_c(0xb0 + line);
	}
}

void oled_goto_column(uint8_t column){	
	if(column*FONTWIDTH <= 127)
	{
		column = column*FONTWIDTH;
		current_column = column;
		char c1 = 0x00 + (column & 0x0F);
		char c2 = 0x10 + ((column & 0xF0)>>4);
		write_c(c1);
		write_c(c2);
	}
}
void oled_clear_line(uint8_t line){
	oled_goto_line(line);
	for (int i = 0; i<128;i++)
	{
		write_d(0x00);
	}
}
void oled_pos(uint8_t row, uint8_t column){
	oled_goto_line(row);
	oled_goto_column(column);
}
void oled_print_char(char character){
	
	for (int i = 0; i<FONTWIDTH;i++)
	{
		write_d(pgm_read_byte(&font[character - ' '][i]));
	}
}

void oled_print_string(char* data)
{
	int i = 0;
	while(data[i] != '\0') {
		oled_print_char(data[i]);
		i++;
		if (i==16 || i==32)
		{
			oled_goto_line(currentLine+1);
		}
	}
}

// Prints an arrow pointing left on the given row and column
void oled_print_arrow(uint8_t row,uint8_t col)
{
	oled_pos(row, col);
	write_d(0b00011000);
	write_d(0b00011000);
	write_d(0b01111110);
	write_d(0b00111100);
	write_d(0b00011000);
}


