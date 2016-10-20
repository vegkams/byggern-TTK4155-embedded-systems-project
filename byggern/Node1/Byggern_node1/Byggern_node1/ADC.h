/*
 * ADC.h
 *
 * Created: 14.09.2016 14:51:00
 *  Author: vegarkam
 */ 


#ifndef ADC_H_
#define ADC_H_


void setup_ADC();
uint8_t read_ADC(uint8_t channel);
void ADC_initiate_conversion(uint8_t channel);


#endif /* ADC_H_ */