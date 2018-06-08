//*******************************************************
//* Заголовок для rtc.c                                 *
//* Работа с частам                                     *
//*******************************************************
//Controller: ATmega64
//Compiler	: AVR-GCC (winAVR with AVRStudio)
//Version 	: 1.0
//Author	: CC Dharmani, Chennai (India)
//                www.dharmanitech.com
//Date		: 17 May 2010
//Adaptation    : Головейко Александр  
//                www.avr-mc.ru
//Date		: 7.09.2012
//*******************************************************
//
#ifndef _RTC_ROUTINES_H_
#define _RTC_ROUTINES_H_

#define		SECONDS			rtc_register[0]
#define		MINUTES			rtc_register[1]
#define		HOURS			rtc_register[2]
#define		DAY    			rtc_register[3]
#define		DATE   			rtc_register[4]
#define		MONTH  			rtc_register[5]
#define		YEAR   			rtc_register[6]

unsigned int dateFAT, timeFAT;
unsigned char rtc_register[7];

unsigned char rtc_setstartaddress(void);
unsigned char rtc_read(void);
unsigned char rtc_write(void);
unsigned char rtc_gettime(void);
unsigned char rtc_getdate(void);
unsigned char write_time(void); // Вывод на дисплей времени.
unsigned char write_date(void); // Вывод на дисплей даты.
unsigned char write_day(void); // Вывод на дисплей дня недели.
void rtc_updateregisters(void);
unsigned char rtc_writetime(void);
unsigned char rtc_writedate(void);
unsigned char rtc_updatetime(void);
unsigned char rtc_updatedate(void);
unsigned char getdatetimefat(void);

#endif
	
