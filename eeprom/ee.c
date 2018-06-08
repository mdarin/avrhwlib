//*******************************************************
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
#include <avr/io.h>
#include "ee.h"
#include "i2c.h"

//*******************************************************
//* ������� ������ � �������� �� ��������� �����        *
//*******************************************************
//
uint8_t ee_sel(uint16_t address)
{
   i2c_400_init();
   if(i2c_start() == 1)
   {
     //transmitString_F(PSTR("RTC start1 failed.."));
   	 i2c_stop();
	 return 1;
   } 
   if(i2c_sendaddress(LC512_W) == 1)
   {
     //transmitString_F(PSTR("RTC sendAddress1 failed.."));
	 i2c_stop();
	 return 1;
   }
   if (i2c_senddata16(address)==1) return 1;
   return 0; 
}

//*******************************************************
//* �������� ���� ������        						*
//*******************************************************
//
uint8_t ee_write8a(uint16_t address, uint8_t data)
{
// ����������� eeprom �� ������ � �������� �����
if (ee_sel(address)==1) return 1;
// ���������� ����
if (i2c_senddata8(data)==1) return 1;
// ���������� ������ ����
i2c_stop();
return 0;
}

//*******************************************************
//* ��������� ���� ������        						*
//*******************************************************
//
uint8_t ee_read8a(uint16_t address)
{
uint8_t data;
i2c_setlasterror(ERROR_CODE);
// ����������� eeprom �� ������ � �������� �����
if (ee_sel(address)==1) return 0;
// ��������� �������� ������
if (i2c_repeatstart()==1) return 0;
// ����������� eeprom ������ ������
if (i2c_sendaddress(LC512_R)==1) return 0;
// ������ ������
data=i2c_receivedata_nack();
// ���������� ������ ����
i2c_stop();
return data;
}
