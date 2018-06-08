/* ***********************************************************************
**
**  Copyright (C) 2005  Christian Kranz
**
**  Siemens S65 Display Control
**
*********************************************************************** */
/*********************************************
* Chip type           : ATMEGA32
* Clock frequency     : clock 8 MHz
*********************************************/
#include <avr/io.h>
#include <inttypes.h>

#include "disp.h"
#include "lcd.h"


#define FCPU 8000000L

int main(void)
{
  uint8_t i;
  char txt[]={'H','e','l','l','o',' ','W','o','r','l','d',0};

  /* INITIALIZE */
  lcd_init();
  backcolor=0xA000;
  textcolor=0xFFFF;
  fill_screen(backcolor);
   
  i=0;
  while (txt[i]!=0)
  {
    // two possible text outputs, same lower left (0) upper left (90) corner pixel
//    put_char(10+i*CHAR_W,60,txt[i],1);  // 90 deg. rotated
    put_char(45+i*CHAR_W,60,txt[i],0);   // 0 deg. rotated
    i++;
  }


  while (1) 
  	continue;

  return(0);	  
}

