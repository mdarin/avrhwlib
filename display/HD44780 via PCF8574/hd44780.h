/* * * * *
 * Library for Hitachi HD44780 chip or a different one compatible with the HD44780 
 * via I2C PCF8574 expander.
 * 
 * Version: 0.1
 * Date: Сб. янв. 25 18:15:45 VOLT 2014
 *
 * Copyright (C) 2014  Michael DARIN
 * 
 * This program is distributed under the of the GNU Lesser Public License. 
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Change log:
 * 	0.1 lib created
 *
 * * */
#include <avr/io.h>
#include "i2c.h"
/*
Any parallelly interfaced character LCD you get these days will have a Hitachi HD44780 chip or a different one compatible with the HD44780. These usually have 14 pins (16 if have backlight)

    D0-D7 is the bi-directional data bus

    R/W determines if we read from or write to the LCD

    RS stands for "register select". RS=0 means that the instruction register is selected. RS=1 means that the data register is selected. In other words, according to the status of RS pin, the data on the data bus is treated either as a command or character data.

    E pin enables or disables the LCD module. When Enable is low the LCD is disabled and the status of RS,R/W and the data bus will be ignored. When Enable pin is high the LCD is enabled and the status of the other control pins and data bus will be processed by the LCD. When writing to the display, data is transferred only on the high to low transition of this signal. 
    Vo pin is for adjusting the contrast of the display. Usually, when this pin is grounded the pixels will be the darkest.
    Vcc is the power supply pin
*/

#define LCD_D0 
#define LCD_D1
#define LCD_D2
#define LCD_D3
#define LCD_D4
#define LCD_D5
#define LCD_D6
#define LCD_D7
#define LCD_RW
#define LCD_RS
#define LCD_E
#define LCD_LED

/*
Commands
The commands for HD44780 chip are:
*/
// Function set (8-bit interface, 2 lines, 5*7 Pixels): 0x38
#define 8BIT_2LINE_5x7PX 0x38
// Function set (8-bit interface, 1 line, 5*7 Pixels): 0x30
#define 8BIT_1LINE_5x7PX 0x30
// Function set (4-bit interface, 2 lines, 5*7 Pixels): 0x28
#define 4BIT_2LINE_5x7Px 0x28
// Function set (4-bit interface, 1 line, 5*7 Pixels): 0x20
#define 4BIT_2LINE_5x7 0x20
// Scroll display one character right (all lines): 0x1E
#define SCROLL_DISP_RIGHT 0x1E
// Scroll display one character left (all lines): 0x18
#define SCROLL_DISP_LEFT 0x18
// Home (move cursor to top/left character position):0x02
#define GO_HOME 0x02
// Move cursor one character left: 0x10
#define MOVE_CURS_LEFT 0x10
// Move cursor one character right: 0x14
#define MOVE_CURS_RIGHT 0x14
// Turn on visible underline cursor: 0x0E
#define TURN_ON_UNDERLINE_CURS 0x0E
// Turn on visible blinking-block cursor: 0x0F
#define TURN_ON_BLINK_BLOCK_CURS 0x0F
// Make cursor invisible: 0x0C
#define MAKE_CURS_INVIS  0x0C
// Blank the display (without clearing): 0x08
#define BLANK_DISP 0x08
// Restore the display (with cursor hidden): 0x0C
#define RESTORE_DISP 0x0C
// Clear Screen: 0x01
#define CLEAR_SCREEN 0x01
// Set cursor position (DDRAM address): 0x80 + address
#define SET_CURS_POS(address) 0x80 + address
// Set pointer in character-generator RAM (CG RAM address): 0x40+ address 
#define SET_PTR_IN_CGEN(address) 0x40 + address

#define LCD_I
#define LCD_S
// Entry mode set: 0x04, 0x05, 0x06, 0x07
// i.e. : %000001IS
// where
// I : 0 = Dec Cursor    1 = Inc Cursor
// S : 0 = Cursor Move   1 = Display Shift

/*
To send a command: 
	set R/W pin to 0 (write), 
	set RS pin to 0 (command selected), 
	put the command to data bus D0-D7.
	Set E pin to 1 then to 0 (remember: data is transferred only on the high to low transition of this signal).

To send data: 
	set R/W pin to0 (write), 
	set RS pin to 1 (data selected), 
	put the data to bus D0-D7, 
	rise E and then back to 0.
*/
void lcd_send_cmd();
//example:
// 	lcd_send_cmd(CLEAR_SCREEN);
//	lcd_send_cmd(TURN_ON_UNDERLINE_CURS);			
void lcd_send_data();

/*
Initialization

HD44780 based LCD displays MUST be initialized.
An internal reset circuit automatically initializes the HD44780U when the power is turned on.

The data sheet warns that under certain conditions, the lcd may fail to initialize properly when power is first applied. This is particulary likely if the Vdd supply does not rise to its correct operating voltage quickly enough. It is recommended that  after power is applied a command sequence of 3 bytes of values $30 is sent to the module. This will guarantee that the module is in 8 bit mode and properly initialised.

The HDD44780 lcd control chip was designed to be compatible with 4-bit processor. Once the display is put in 4 bit mode, using the Function Set command, it is a simple matter of sending two nibbles instead of one byte, for each subsequent command or character. When using 4 bit mode only data lines D4 to D7 are used. Note that the Function set command for 4-bit interface, 2 lines, 5*7 Pixels is 0x28. So first use the Function set command $20 (4-bit interface, 1 line, 5*7 Pixels); from now on, all commands and data must be sent in two halves, the upper four bits first.
Now you can send the Function set command 0x28.


Take now the well-known PCF8574P. This is a general purpose eight-bit input/output chip.
The hard-coded address is: 0100.A2A1A0
The 0100 part is hard-coded in the PCF8574P.
The A2A1A0 is for us to choose. Make these bits one or zero by tying the corresponding pins to Vcc or Ground:

NOTE: Philips produces two variants of this IC: the PCF8574 and the PCF8574A. The only difference is the I2C chip address!
PCF8574 0100.A2A1A0
PCF8574A 0111.A2A1A0

Important: the I2C standard prescribes pull-up resistors on the SDA and SCL line. This is one of the most common mistakes when first using an I2C interface. Forget the pull-ups and your interface will not work (sigh!). Picaxe manual show resistors of 4k7 ohms.
Arduino don't need pull up resistor.

Now connect all together!
*/
void lcd_init(void); // may be lcd_init(lcd_settings);


/*
 User API
 insert functions...
 for example:
*/
void lcd_putc();
void lcd_puts();
void lcd_printf();
//...
