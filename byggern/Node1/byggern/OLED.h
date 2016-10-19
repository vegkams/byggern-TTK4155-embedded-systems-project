/*
 * OLED.h
 *
 * Created: 24.09.2016 12:51:36
 *  Author: vegarkam
 */ 


#ifndef OLED_H_
#define OLED_H_

void oled_init();
void write_c(unsigned char char_command);
void write_d(unsigned char char_data);

void oled_reset();
void oled_home();
void oled_goto_line(uint8_t line);
void oled_goto_column(uint8_t column);
void oled_clear_line( uint8_t line);
void oled_pos(uint8_t row, uint8_t column);
void oled_print_char(char character);
void oled_print_arrow(uint8_t row,uint8_t col);
void oled_print_string(char* data);
uint8_t getCurrentLine();





#endif /* OLED_H_ */