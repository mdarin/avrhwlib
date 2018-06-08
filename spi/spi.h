//**************************************************************
// ****** FUNCTIONS FOR SPI COMMUNICATION *******
//**************************************************************
//Controller: ATmega64
//Compiler	: AVR-GCC (winAVR with AVRStudio)
//Version 	: 2.3
//Author	: CC Dharmani, Chennai (India)
//                www.dharmanitech.com
//Date		: 08 May 2010
//**************************************************************


//**************************************************
// ***** HEADER FILE : SPI_routines.h ******
//**************************************************
#ifndef _SPI_
#define _SPI_

#define SPI_SD             SPCR = 0x52
#define SPI_HIGH_SPEED     SPCR = 0x50; SPSR |= (1<<SPI2X)
#define spi_init_disp()     SPCR = (1<<MSTR)|(1<<SPE); SPSR |= (1<<SPI2X) // настройка SPI для работы с дисплеем
#define spi_init_sd_low()  SPCR = 0x52;SPSR = 0x00// настройка SPI для работы с картой на низкой скорости
#define spi_init_sd_high()  SPCR = (1<<MSTR)|(1<<SPE); SPSR |= (1<<SPI2X) // настройка SPI для работы с картой на высокой скорости

void spi_init(void);
unsigned char spi_transmit(unsigned char);
unsigned char spi_receive(void);

#endif
