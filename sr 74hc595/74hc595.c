/*  
	74hc595.c
	leds indication on the 74hc595 shift register lib
	version: 0.1b
	date: 24.02.2014
		for gsm_pribor
*/
#include <avr/io.h>
#include "74hc595.h"

// temporary medium register
static unsigned char shift_reg;

//--------------------------------------------------------------------
void sr_init(void)
{
  // set up shift register ports
  DDRC |= (1 << ST_CP) | (1 << DS);
  DDRD |= (1 << SH_CP);
  // clear register  
  PORTC &= ~(1 << ST_CP); 
  for (unsigned char bit = 8; bit > 0; bit--) {
    PORTD &= ~(1 << SH_CP); 	
	PORTC &= ~(1 << DS);
	PORTD |= (1 << SH_CP);
	PORTD &= ~(1 << SH_CP);    		
  }
  PORTC |= (1 << ST_CP); 
  PORTC &= ~(1 << ST_CP);
  // clear shift_reg temp
  shift_reg = 0; 
  return;
}
//--------------------------------------------------------------------                         
void sr_write_reg(void)
{ 
  PORTC &= ~(1 << ST_CP); 
  for (unsigned char bit = 8; bit > 0; bit--) {
    PORTD &= ~(1 << SH_CP); 
	if (bit_is_set(shift_reg, bit)) {
	  PORTC |= (1 << DS);
	} else {
	  PORTC &= ~(1 << DS);
	}
	PORTD |= (1 << SH_CP);
	PORTD &= ~(1 << SH_CP); 
  }
  PORTC |= (1 << ST_CP);
  PORTC &= ~(1 << ST_CP); 
  return;
}
//--------------------------------------------------------------------                         
void sr_set_reg(shift_reg_t reg)
{ 
  shift_reg |= reg;
  return;
}
//--------------------------------------------------------------------                         
void sr_unset_reg(shift_reg_t reg)
{
  shift_reg &= reg;
  return;
}
//--------------------------------------------------------------------                         
// experimental

// led is a bit number
void sr_led_enable(unsigned char led) 
{
  unsigned char reg = 0;
  reg |= (1 << led);
  sr_set_reg(reg);
  sr_write_reg();	
  return;
}
//--------------------------------------------------------------------
void sr_led_disable(unsigned char led)
{
  unsigned char reg = 0;
  reg &= ~(1 << led);
  sr_set_reg(reg);
  sr_write_reg();
  return;
}
