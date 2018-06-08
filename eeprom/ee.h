//*******************************************************
//* Заголовок для ee.c                                  *
//* Работа с внешней памятью 24LC256                    *
//*******************************************************
//Controller: ATmega64
//Compiler	: AVR-GCC (winAVR with AVRStudio)
//Version 	: 1.0
//Author	: Головейко Александр         
//                www.avr-mc.ru
//Date		: 7.09.2012
//*******************************************************
//
#ifndef _EE_
#define _EE_

uint8_t ee_write8a(uint16_t address, uint8_t data); // Записать байт данных.
uint8_t ee_read8a(uint16_t address); // Прочитать байт данных.
uint8_t ee_sel(uint16_t address); // Выбрать память 24LC256 и передать ей стартовый адрес.

#endif
