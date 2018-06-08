/*  
	74hc595.h
	leds indication on the 74hc595 shift register lib
	version: 0.1b
	date: 24.02.2014
		for gsm_pribor
*/
#ifndef _74HC595_H
#define _74HC595_H

#define ST_CP 	PC2 // strobe
#define DS 		PC3 // data
#define SH_CP 	PD6 // shift

// TODO: potr abstracs needs to be developed
//#define ST_PORT PORTC
//#define ST

// 74hc595 output pins
#define LOOP_1 6 // Q5
#define LOOP_2 5 // Q4
#define LOOP_3 4 // Q3
#define LOOP_4 3 // Q2
#define RED    2 // Q1
#define GREEN  1 // Q0

typedef unsigned char led_t;
typedef char shift_reg_t;

// function prototypes
void sr_init(void); 
void sr_set_reg(shift_reg_t reg);
void sr_unset_reg(shift_reg_t reg);
void sr_write_reg(void);
void sr_led_enable(led_t led);
void sr_led_disable(led_t led);

#endif // 74HC595_H
