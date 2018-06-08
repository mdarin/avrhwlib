//*******************************************************
//* Работа с частам                                     *
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
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "rtc.h"
#include "uart0.h"
#include "i2c.h"
#include "write.h"

unsigned char time[10]; 		//xxam:xx:xx;
unsigned char date[12];		    //xx/xx/xxxx;
unsigned char day;



//*******************************************************
//* Function to set initial address of the RTC for      *
//* subsequent reading / writing                        *
//*******************************************************
//
unsigned char rtc_setstartaddress(void)
{
   unsigned char errorStatus;
   i2c_100_init();
   errorStatus = i2c_start();
   if(errorStatus == 1)
   {
     //transmitString_F(PSTR("RTC start1 failed.."));
   	 i2c_stop();
	 return 1;
   } 
   
   errorStatus = i2c_sendaddress(DS1307_W);
   
   if(errorStatus == 1)
   {
     //transmitString_F(PSTR("RTC sendAddress1 failed.."));
	 i2c_stop();
	 return 1;
   } 
   
   errorStatus = i2c_senddata8(0x00);
   if(errorStatus == 1)
   {
     //transmitString_F(PSTR("RTC write-2 failed.."));
	 i2c_stop();
	 return 1;
   } 

   i2c_stop();
   return 0;
}

//*******************************************************
//* Function to read RTC registers and store them       *
//* in buffer rtc_register[]                            * 
//*******************************************************
//
unsigned char rtc_read(void)
{

  unsigned char errorStatus, i, data;
  i2c_100_init();
  errorStatus = i2c_start();
   if(errorStatus == 1)
   {
     //transmitString_F(PSTR("RTC start1 failed.."));
   	 i2c_stop();
	 return 1;
   } 
   
   errorStatus = i2c_sendaddress(DS1307_W);
   
   if(errorStatus == 1)
   {
     //transmitString_F(PSTR("RTC sendAddress1 failed.."));
	 i2c_stop();
	 return 1;
   } 
   
   errorStatus = i2c_senddata8(0x00);
   if(errorStatus == 1)
   {
     //transmitString_F(PSTR("RTC write-1 failed.."));
	 i2c_stop();
	 return 1;
   } 

    errorStatus = i2c_repeatstart();
   if(errorStatus == 1)
   {
     //transmitString_F(PSTR("RTC repeat start failed.."));
   	 i2c_stop();
	 return 1;
   } 
   
    errorStatus = i2c_sendaddress(DS1307_R);
   
   if(errorStatus == 1)
   {
     //transmitString_F(PSTR("RTC sendAddress2 failed.."));
	 i2c_stop();
	 return 1;
   } 
 
    for(i=0;i<7;i++)
   {
      if(i == 6)  	 //no Acknowledge after receiving the last byte
	   	  data = i2c_receivedata_nack();
	  else
	  	  data = i2c_receivedata_ack();
		  
   	  if(i2c_getlasterror() == ERROR_CODE)
   	  {
       		//transmitString_F(PSTR("RTC receive failed.."));
			i2c_stop();
	   		return 1;
   	  }
	  
	  rtc_register[i] = data;
	}
	
	i2c_stop();
	return 0;
}	  

//*******************************************************
//* Function to form time string for sending            *
//* it to LCD & UART                                    *
//*******************************************************
//
unsigned char rtc_gettime(void)
{
   unsigned char error;
   error = rtc_read();
   if(error) return 1;

   rtc_read();
   time[8] = 0x00;	  //NIL
   time[7] = (SECONDS & 0x0f) | 0x30;		 //seconds(1's)
   time[6] = ((SECONDS & 0x70) >> 4) | 0x30; //seconds(10's)
   time[5] = ':';
   
   time[4] = (MINUTES & 0x0f) | 0x30;
   time[3] = ((MINUTES & 0x70) >> 4) | 0x30;
   time[2] = ':'; 
   
   time[1] = (HOURS & 0x0f) | 0x30;	
   time[0] = ((HOURS & 0x30) >> 4) | 0x30;

   return 0;
}

//*******************************************************
//* Function to form date string for sending            *
//* it to LCD & UART                                    *
//*******************************************************
//
unsigned char rtc_getdate(void)
{
  unsigned char error;
  error = rtc_read();
  if(error) return 1;

  date[11] = 0x00;
  date[10] = 0x00;
  date[9] = (YEAR & 0x0f) | 0x30;
  date[8] = ((YEAR & 0xf0) >> 4) | 0x30;
  date[7] = '0';
  date[6] = '2';
  date[5] = '/';
  date[4] = (MONTH & 0x0f) | 0x30;
  date[3] = ((MONTH & 0x10) >> 4) | 0x30;
  date[2] = '/';
  date[1] = (DATE & 0x0f) | 0x30;
  date[0] = ((DATE & 0x30) >> 4) | 0x30;
  return 0;
}  

//*******************************************************
//* Вывод на дисплей времени                            *
//*******************************************************
//
unsigned char write_time(void)
{
  unsigned char error;
  error = rtc_gettime();
  if(error) {
	write_str_pgm((PSTR("ОШИБКА")));
  			return 1;
	}
	write_str(time);
  return 0;
}

//*******************************************************
//* Вывод на дисплей даты                               *
//*******************************************************
//
unsigned char write_date(void)
{
  unsigned char error;
  error = rtc_getdate();
  if(error) {
	write_str_pgm((PSTR("ОШИБКА")));
  			return 1;
	}
	write_str(date);
  return 0; 
}

//*******************************************************
//* Вывод на дисплей дня недели                         *
//*******************************************************
//
unsigned char write_day(void)
{
  unsigned char error;
  error = rtc_getdate();
  if(error) {
	write_str_pgm((PSTR("ОШИБКА")));

  			return 1;

	}
  
  switch(DAY)
  {
   case 0:write_str_pgm((PSTR("Воскресенье")));
          break; 
   case 1:write_str_pgm((PSTR("Понедельник")));
          break; 
   case 2:write_str_pgm((PSTR("Вторник")));
          break; 
   case 3:write_str_pgm((PSTR("Среда")));
          break; 
   case 4:write_str_pgm((PSTR("Четверг")));
          break; 		  
   case 5:write_str_pgm((PSTR("Пятница")));
          break; 		  
   case 6:write_str_pgm((PSTR("Суббота")));
          break; 
   default:	write_str_pgm((PSTR("Неизвестное")));  
  }
  return 0;
}	  		  
		  		     	  
//*******************************************************
//* Function to update buffer rtc_register[]            *
//* for next writing to RTC                             *
//*******************************************************
//
void rtc_updateregisters(void)
{
  SECONDS = ((time[6] & 0x07) << 4) | (time[7] & 0x0f);
  MINUTES = ((time[3] & 0x07) << 4) | (time[4] & 0x0f);
  HOURS = ((time[0] & 0x03) << 4) | (time[1] & 0x0f);  
  DAY = date[10];
  DATE = ((date[0] & 0x03) << 4) | (date[1] & 0x0f);
  MONTH = ((date[3] & 0x01) << 4) | (date[4] & 0x0f);
  YEAR = ((date[8] & 0x0f) << 4) | (date[9] & 0x0f);
}  

//*******************************************************
//Function to write new time in the RTC                 *
//*******************************************************
//
unsigned char rtc_writetime(void)
{
  unsigned char errorStatus, i;
   i2c_100_init();
   errorStatus = i2c_start();
   if(errorStatus == 1)
   {
     //transmitString_F(PSTR("RTC start1 failed.."));
   	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_sendaddress(DS1307_W);
   
   if(errorStatus == 1)
   {
     //transmitString_F(PSTR("RTC sendAddress1 failed.."));
	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_senddata8(0x00);
   if(errorStatus == 1)
   {
     //transmitString_F(PSTR("RTC write-1 failed.."));
	 i2c_stop();
	 return(1);
   } 

    for(i=0;i<3;i++)
    {
	  errorStatus = i2c_senddata8(rtc_register[i]);  
   	  if(errorStatus == 1)
   	  {
       		//transmitString_F(PSTR("RTC write time failed.."));
			i2c_stop();
	   		return(1);
   	  }
    }
	
	i2c_stop();
	return(0);
}

//*******************************************************
//Function to write new date in the RTC                 *
//*******************************************************
//
unsigned char rtc_writedate(void)
{
  unsigned char errorStatus, i;
   i2c_100_init();
   errorStatus = i2c_start();
   if(errorStatus == 1)
   {
     //transmitString_F(PSTR("RTC start1 failed.."));
   	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_sendaddress(DS1307_W);
   
   if(errorStatus == 1)
   {
     //transmitString_F(PSTR("RTC sendAddress1 failed.."));
	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_senddata8(0x03);
   if(errorStatus == 1)
   {
     //transmitString_F(PSTR("RTC write-1 failed.."));
	 i2c_stop();
	 return(1);
   } 

    for(i=3;i<7;i++)
    {
	  errorStatus = i2c_senddata8(rtc_register[i]);  
   	  if(errorStatus == 1)
   	  {
       		//transmitString_F(PSTR("RTC write date failed.."));
			i2c_stop();
	   		return(1);
   	  }
    }
	
	i2c_stop();
	return(0);
}
  
//*******************************************************
//* Function to update RTC time by entering             *
//* it at hyper terminal                                *
//*******************************************************
//
unsigned char rtc_updatetime(void)
{
  unsigned char data;
  TX_NEWLINE;
  transmitString_F(PSTR("Enter Time in 24h format(hh:mm:ss):")); 
  
    data = receiveByte(); 	   	  	  				//receive hours
	transmitByte(data);
	if(data < 0x30 || data > 0x32)
	   goto TIME_ERROR;
	   
	time[0]= data;
	 
	data = receiveByte();
	transmitByte(data);
	if(data < 0x30 || data > 0x39)
	   goto TIME_ERROR;
	 
	time[1]= data;
	
	transmitByte(':');
	
	if(((time[1] & 0x0f) + ((time[0] & 0x03)*10)) > 23)
	   goto TIME_ERROR;
	 
	 data = receiveByte();			   			  //receive minutes
	 transmitByte(data);
	if(data < 0x30 || data > 0x35)
	   goto TIME_ERROR;
	   
	time[3]= data; 
	
	data = receiveByte();
	transmitByte(data);
	if(data < 0x30 || data > 0x39)
	   goto TIME_ERROR;
	   
	time[4]= data; 
	
	transmitByte(':');
	
	 data = receiveByte();			   			  //receive seconds
	 transmitByte(data);
	if(data < 0x30 || data > 0x35)
	   goto TIME_ERROR;
	   
	time[6]= data; 
	
	data = receiveByte();
	transmitByte(data);
	if(data < 0x30 || data > 0x39)
	   goto TIME_ERROR;
	   
	time[7]= data; 
	
	 	  
  rtc_updateregisters(); 
  data = rtc_writetime();

  TX_NEWLINE;
  if(data == 0)
  {     
  	 transmitString_F(PSTR("Time Updated sucessfully!")); 
	 return 0;
  }	
  else 
  { 
     transmitString_F(PSTR("Time Update Failed.."));
	 return 1;
  }
TIME_ERROR:

  TX_NEWLINE;
  transmitString_F(PSTR("Invalid Entry..")); 
  return 1;
}  
   
//*******************************************************
//* Function to update RTC date by entering             *
//* it at hyper terminal                                * 
//*******************************************************
//
unsigned char rtc_updatedate(void)
{
  unsigned char data;
  TX_NEWLINE;
  transmitString_F(PSTR("Enter Date (dd/mm/yy):")); 
  
    data = receiveByte(); 	   				  		//receive date
	transmitByte(data); 	   	  	  				
	if(data < 0x30 || data > 0x33)
	   goto DATE_ERROR;
	   
	date[0]= data;
	 
	data = receiveByte();
	transmitByte(data);
	if(data < 0x30 || data > 0x39)
	   goto DATE_ERROR;
	   
	date[1]= data;
	
	if(((date[1] & 0x0f) + ((date[0] & 0x03)*10)) > 31)
	   goto DATE_ERROR;
    transmitByte('/');
	
	date[2] = '/';
	   
	
	data = receiveByte();			   			  //receive month
	transmitByte(data);
	if(data < 0x30 || data > 0x31)
	  goto DATE_ERROR;
	  
	date[3]= data; 
	
	data = receiveByte();
	transmitByte(data);
	if(data < 0x30 || data > 0x39)
	  goto DATE_ERROR;
	  
	date[4] = data; 
	
	if(((date[4] & 0x0f) + ((date[3] & 0x03)*10)) > 12)
	   goto DATE_ERROR;
	transmitByte('/');
	   
	
	date[5] = '/';
	
	date[6] = '2'; 	   	   	  	  //year is 20xx
	date[7] = '0';
	
	data = receiveByte();			   			 
	transmitByte(data);
	if(data < 0x30 || data > 0x39)
	   goto DATE_ERROR;
	   
	date[8]= data; 
	
	data = receiveByte();
	transmitByte(data);
	if(data < 0x30 || data > 0x39)
	   goto DATE_ERROR;
	   
	date[9]= data; 
	
	TX_NEWLINE;
    transmitString_F(PSTR("Enter Day (Sunday:0, Monday:1...) (0-6):")); 
	
	data = receiveByte();				   //receive Day of the week
	transmitByte(data);
	if(data < 0x30 || data > 0x36)
	   goto DATE_ERROR;
	   
	date[10] = data & 0x0f;
		 	  
  rtc_updateregisters(); 
  data = rtc_writedate();

  TX_NEWLINE;
  if(data == 0)
  {
     
  	 transmitString_F(PSTR("Date Updated sucessfully!")); 
	 return 0;
  }	  
  else
  {
     transmitString_F(PSTR("Date Update Failed.."));
     return 1;
  }

  DATE_ERROR:

  TX_NEWLINE;
  transmitString_F(PSTR("Invalid Entry..")); 
  return 1;
}  

//*******************************************************
//* Function to get RTC date & time in FAT32 format     *
//*******************************************************
//
unsigned char getdatetimefat(void)
{
  
   unsigned char mth, dt, hr, min, sec, error; 
   unsigned int yr;

   error = rtc_read();
   if(error) return 1;

   yr = (YEAR & 0xf0) >> 4;
   yr = (yr * 10)+(YEAR & 0x0f);
   yr = yr+2000;
   yr = yr - 1980;

   dateFAT = yr;

   mth = (MONTH & 0xf0) >> 4;
   mth = (mth * 10)+(MONTH & 0x0f);

   dateFAT = (dateFAT << 4) | mth;

   dt = (DATE & 0xf0) >> 4;
   dt = (dt * 10)+(DATE & 0x0f);

   dateFAT = (dateFAT << 5) | dt;


   hr = (HOURS & 0xf0) >> 4;
   hr = (hr * 10)+(HOURS & 0x0f);

   timeFAT = hr;

   min = (MINUTES & 0xf0) >> 4;
   min = (min * 10)+(MINUTES & 0x0f);

   timeFAT = (timeFAT << 6) | min;

   sec = (SECONDS & 0xf0) >> 4;
   sec = (sec * 10)+(SECONDS & 0x0f);
   sec = sec / 2;    //FAT32 fromat accepts dates with 2sec resolution (e.g. value 5 => 10sec)

   timeFAT = (timeFAT << 5) | sec;
   
   return 0;
}
	  
 
 

 
