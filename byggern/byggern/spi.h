/*
 * spi.h
 *
 * Created: 05.10.2016 11.48.53
 *  Author: vegard
 */ 


#ifndef SPI_H_
#define SPI_H_
#define MOSI PB5
#define MISO PB6
#define SCK PB7
#define SPI_SS PB4

void spi_init(void);
void spi_send(char cData);
char spi_read();
void spi_enable();
void spi_disable();




#endif /* SPI_H_ */