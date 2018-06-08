/* ***********************************************************************
**
**  Copyright (C) 2005 Christian Kranz
**
** Siemens S65 Display Control
*********************************************************************** */

#ifndef LCD_H
#define LCD_H


#define LCD_CS	   PB0//PB3
#define LCD_RESET  PB4//PB2
#define LCD_RS	   PB3//PB0
#define LCD_MOSI   PB5//PB1
#define LCD_MISO   PB6//PB5
#define LCD_SCK    PB7//PB6



#ifndef LCD_ASM

 void mswait(uint16_t ms);

 void lcd_init(void);			// LCD initialise
 void lcd_clrscr(void);		// LCD power down

 void lcd_wrdata(uint8_t);
 void lcd_wrcmd(uint8_t);

#endif

#endif // LCD_H

