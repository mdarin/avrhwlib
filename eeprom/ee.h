//*******************************************************
//* ��������� ��� ee.c                                  *
//* ������ � ������� ������� 24LC256                    *
//*******************************************************
//Controller: ATmega64
//Compiler	: AVR-GCC (winAVR with AVRStudio)
//Version 	: 1.0
//Author	: ��������� ���������         
//                www.avr-mc.ru
//Date		: 7.09.2012
//*******************************************************
//
#ifndef _EE_
#define _EE_

uint8_t ee_write8a(uint16_t address, uint8_t data); // �������� ���� ������.
uint8_t ee_read8a(uint16_t address); // ��������� ���� ������.
uint8_t ee_sel(uint16_t address); // ������� ������ 24LC256 � �������� �� ��������� �����.

#endif
