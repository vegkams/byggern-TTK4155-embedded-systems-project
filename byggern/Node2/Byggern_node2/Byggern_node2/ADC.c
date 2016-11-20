/*
* ADC.c
*
* Created: 06.11.2016 14:31:45
*  Author: vegarkam
*/
#include <avr/io.h>
void setup_ADC()
{
	PRR0 &= ~(1<<PRADC);
	// left adjusted
	ADMUX |= (1<<REFS0) | (1<<ADLAR);
	// Use AVCC as ref, ADC0 as input
	ADMUX &= ~(1<<REFS1) & ~(1<<MUX4) & ~(1<<MUX3) & ~(1<<MUX2) & ~(1<<MUX1) &~(1<<MUX0);

	// enable adc, use freerunning
	ADCSRA |= (1<<ADEN) | (1<<ADATE);
	ADCSRB &= ~(1<<ADTS2) &~(1<<ADTS1) & ~(1<<ADTS0);
}

void start_ADC() {
	ADCSRA |= (1<<ADEN) | (1<<ADATE);
	ADCSRA |= (1<<ADSC);
}

void stop_ADC() {
	ADCSRA &= ~(1<<ADEN);
}

uint8_t get_ADC_value()
{
	return ADCH;
}