/*
 * eeprom.h
 *
 * Created: 22.11.2016 17:00:55
 *  Author: vegarkam
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

void write_eeprom(unsigned int uiAddress, unsigned char ucData);
unsigned char EEPROM_read(unsigned int uiAddress);




#endif /* EEPROM_H_ */