#include "sound.h"

//-------------------------------------------------------------
void play_music(void)
{
  init_spi_master(); 
  pwm_init();
  dgen_init(); 
  
  char done = 0;
  unsigned long addr;
  buf = wbuf_init(PAGE_SIZE);
  if (NULL != buf ) { 
    // at the begining fillin two buffer atonce
     addr = 0;
	 while (SetWriteProtectedArea(NONE) == BUSY){} // Disable Software Protection (ensure that #WP is high)		
	 while (GetCharArray(addr, buf->unload, buf->length) != TRANSFER_COMPLETED){}
     buf->block++;
   } else {
     // loop on memory alloc  
	 red_led_on();
	 while (1) continue;
   }
   
   while (!done) {
   	 green_led_on();
     red_led_off(); // не уберать надо нопы или какуй хуйню сюда замонтажить!
	 //-----------------------------------------
     // загругрузка порожнего буфера следующим блоком
     if (empty == buf->status) {	    
	   addr = addr + buf->length;
	   while (GetCharArray(addr, buf->reload, buf->length) != TRANSFER_COMPLETED){}				
	   buf->status = full;
	   buf->block++;  
	   // dependendent part... neet some mind force :)
	   if (addr >= DATA_LEN) {
	     stop_playing();
	     buf->block = 0;
		 done = 1;
	   } 
	 }
	 continue;
   }
   wbuf_destroy(buf);
   return;
}


void stop_playing(void)
{
  TCCR1B &= ~(1 << CS10);
  TCCR0B &= ~(1 << CS00);
  green_led_off();
  return; 
}


//-------------------------------------------------------------
void dgen_init(void)
{
  // T1 as 11-22 generator init 
  TIMSK1 |= (1 << OCIE1A);
  TCCR1B |= (1 << WGM12) | (1 << CS10);
  TCNT1 = 0;
  OCR1AH = 0x03; // 22 KHz
  OCR1AL = 0x8C;
  return;
}
//-------------------------------------------------------------
void pwm_init(void)
{
  // T0 as fast PWM on OC0B(PD5) pin
  TIMSK0 |= (1 << OCIE0B);
  TCCR0A |= (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
  TCCR0B |= (1 << CS00);  
  // PWM output ON
  DDRD |= (1 << PD5);
  return;
}


//-------------------------------------------------------------
ISR (TIMER0_COMPB_vect) 
{
  // continue...   
  return;
}
//-------------------------------------------------------------
ISR (TIMER1_COMPA_vect)
{
  wbuf_check_toggle(buf);
  OCR0B = wbuf_get_value(buf);
  buf = wbuf_get_next(buf);       	    
  return;
}