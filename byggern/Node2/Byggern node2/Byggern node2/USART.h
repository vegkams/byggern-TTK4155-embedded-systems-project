/*
 * USART.h
 *
 * Created: 02.09.2016 15.27.27
 *  Author: vegard
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_
#include <stdio.h>

void USART_Init(unsigned int ubrr);
int USART_Transmit(char data,struct __file* dummy);
int USART_Recieve(struct __file* dummy);




#endif /* INCFILE1_H_ */