//*******************************************************
//* Заголовок для i2c.c                                 *
//* Работа по протоколу I2C                             *
//*******************************************************
//Controller: ATmega64
//Compiler	: AVR-GCC (winAVR with AVRStudio)
//Version 	: 1.0
//Author	: CC Dharmani, Chennai (India)
//                www.dharmanitech.com
//Date		: 17 May 2010
//Adaptation: Головейко Александр  
//                www.avr-mc.ru
//Date		: 7.09.2012
//*******************************************************
//
#ifndef _I2C_
#define _I2C_

#define	 START				0x08
#define  REPEAT_START		0x10
#define  MT_SLA_ACK			0x18
#define  MT_SLA_NACK		0x20
#define  MT_DATA_ACK		0x28
#define  MT_DATA_NACK		0x30
#define  MR_SLA_ACK			0x40
#define  MR_SLA_NACK		0x48
#define  MR_DATA_ACK		0x50
#define  MR_DATA_NACK		0x58
#define  ARB_LOST			0x38

#define  ERROR_CODE			0x7e
	
#define  DS1307_W			0b11010000 // адрес часов для записи
#define  DS1307_R			0b11010001 // адрес часов для чтения

#define  LC512_W			0b10100010	 // адрес EEPROM для записи
#define  LC512_R			0b10100011	 // адрес EEPROM для чтения

#define  EEPROM_W			0xa0
#define  EEPROM_R			0xa1

// частота, на котором работает устройство
#define sF_SCL100 100000 //100 Khz
#define sF_SCL400 400000 //400 Khz

uint8_t i2c_last_error;

#define i2c_getlasterror() i2c_last_error
#define i2c_setlasterror(error) i2c_last_error=error
void i2c_400_init(void);
void i2c_100_init(void);
unsigned char i2c_start(void);
unsigned char i2c_repeatstart(void);
unsigned char i2c_sendaddress(unsigned char);
unsigned char i2c_senddata8(unsigned char);
unsigned char i2c_senddata16(uint16_t data16);
unsigned char i2c_receivedata_ack(void);
unsigned char i2c_receivedata_nack(void);
void i2c_set_tt(void);
void i2c_stop(void);

#endif
	
