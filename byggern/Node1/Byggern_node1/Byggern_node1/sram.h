/*
 * sram.h
 *
 * Created: 07.09.2016 14.40.02
 *  Author: vegard
 */ 


#ifndef SRAM_H_
#define SRAM_H_
int sram_setup(void);
int sram_write_to_address(unsigned int address, unsigned int data);
uint8_t sram_read_from_address(unsigned int address);
void SRAM_test(void);

#endif /* SRAM_H_ */