/* ***********************************************************************
**
**  Copyright (C) 2005  Christian Kranz
**
**  Siemens S65 Display Control
**
*********************************************************************** */

#define __ASSEMBLER__ 1
#define __SFR_OFFSET 0
#include <avr/io.h>
#define LCD_ASM
#include "lcd.h"

#define __tmp_reg__ r0
#define __zero_reg__    r1

    .text
    
#ifndef XTAL
#define XTAL                        16000000                // must be changed
#endif

#define DELAY_ONE_MILLISECOND       (XTAL / 4 / 1000)       // must > 8


// void mswait(uint16_t ms);           
// wait some milliseconds, XTAL must be correct defined

        .global glcdWait                // used T0,T1 stack 0
mswait:
        sbiw    r24, 1                                   // 2
        brcc    mswait1                               // 2
        ret                                             // 2 -2 = 0 for no branch in both loops
mswait1:
        nop
        ldi     r26, lo8(DELAY_ONE_MILLISECOND -4)       // 1
        ldi     r27, hi8(DELAY_ONE_MILLISECOND -4)       // 1
mswait2:
        sbiw    r26, 1                                   // 2
        brne    mswait2                               // 2 = 4, = 3 for last loop
        rjmp    mswait                                // 2 = 8



;void port_init_io(void)
    .global port_init_io
port_init_io:
    ; setup serial data I/O pins

    cbi     PORTB, LCD_RESET        ; display reset
    sbi     DDRB, LCD_RESET         ; set reset as output
;   sbi     DDRB, LCD_MISO          ; MISO is always input in SPI
    sbi     DDRB, LCD_MOSI          ; MOSI must be set as output for SPI
    sbi         PORTB, LCD_CS           ; deselect display 
    sbi     DDRB, LCD_CS            ; SS must be output for Master mode to work


    cbi     PORTB, LCD_SCK          ; set SCK lo
    sbi     DDRB,  LCD_SCK          ; set SCK as output
    sbi     PORTB, LCD_MISO         ; enable pull up of MISO to avoid floating input

    sbi     PORTB, LCD_RS           ; start with RS=high
    sbi     DDRB, LCD_RS            ; RS signal is output

    ; setup serial data interface 
    ; select clock phase positive going in middle of data
    ; master mode
    ; enable SPI
    ; speed is CPUclock/2

    ldi     r24,(1<<MSTR) | (1<<SPE); | (1<<SPR0);  ; Master Mode,  Enable SPI, fCPU/16 clock
    out     SPCR,r24

    ldi     r24,1           ; double speed bit
    out     SPSR,r24

    ldi     r24,1
    ldi     r25,0
    rcall   mswait          ; wait one ms to have a nice reset
    sbi     PORTB, LCD_RESET    ; release reset
    in      r24,SPSR        ; clear status
    ret


;************************************************
; write table with r30/r31 pointer to table
; r22 number of bytes to read
wrtab:
    lpm     r24,Z+          ; load byte from table
    rcall   lcd_wrcmd
    dec     r22
    brne    wrtab
    ret


;************************************************
;initLCD
;        r22/r24
;void lcd_init(void)
    .global lcd_init

lcd_init:
    push    r30
    push    r31

    sbi     PORTB,LCD_CS    ;deselect Display
    sbi     DDRB,LCD_CS     ;
    call    port_init_io    ;init SPI
    
    ldi     r30,lo8(INIT1)  ;set pointer to table
    ldi     r31,hi8(INIT1)       
    ldi     r22,24      ;number of bytes to be transmitted
    rcall   wrtab
        
    ; wait for settling
    ldi     r24,7
    ldi     r25,0
    rcall   mswait

    ldi     r30,lo8(INIT2)  ;set pointer to table
    ldi     r31,hi8(INIT2)       
    ldi     r22,40          ;number of bytes to be transmitted
    rcall   wrtab

    ; wait for settling
    ldi     r24,50
    ldi     r25,0
    rcall   mswait

    ldi     r30,lo8(INIT3)  ;set pointer to table
    ldi     r31,hi8(INIT3)       
    ldi     r22,2          ;number of bytes to be transmitted
    rcall   wrtab


    ;   write initial screen command
    cbi     PORTB,LCD_CS    ;select Display

    ldi     r24,0xEF
    rcall   lcd_wrcmd
    ldi     r24,0x90
    rcall   lcd_wrcmd
    ldi     r24,0x00
    rcall   lcd_wrcmd
    ldi     r24,0x00
    rcall   lcd_wrcmd

    rcall   lcd_clrscr

    pop     r31
    pop     r30
    ret

;************************************************
;clear LCD
;        r24
;void lcd_clrscr(void)
    .global lcd_clrscr
lcd_clrscr:
    push    r30
    push    r31

    ; send window write start sequence
    ldi     r24,0xEF
    rcall   lcd_wrcmd
    ldi     r24,0x90
    rcall   lcd_wrcmd
    ldi     r24,0x05
    rcall   lcd_wrcmd
    ldi     r24,0x00
    rcall   lcd_wrcmd
    ldi     r24,0x06
    rcall   lcd_wrcmd
    ldi     r24,0x00
    rcall   lcd_wrcmd
    ldi     r24,0x07
    rcall   lcd_wrcmd
    ldi     r24,0x00
    rcall   lcd_wrcmd


    ; now 132*176*2 bytes are to be transmitted
    ldi     r31, 0xB5
    ldi     r30, 0x80
    ldi     r24, 0xFF
LCD_CLEAR_1:
    rcall   lcd_wrdata
    sbiw    r30,1
    brne    LCD_CLEAR_1

    pop r31
    pop r30
    ret


;************************************************
;send an LCD command
;        r24
    .global lcd_wrcmd
lcd_wrcmd:
    cbi     PORTB,LCD_CS    ;select Display
    sbi     PORTB,LCD_RS    ; set RS line
    rjmp    SPI_SEND

;************************************************
;send LCD data
;        r24
;void lcd_wrdata(u08 data)
    .global lcd_wrdata
lcd_wrdata:
    cbi     PORTB,LCD_CS    ;select Display
    cbi     PORTB,LCD_RS    ; clear RS line
    ;fall trough

;************************************************
;using the SPI for communication

SPI_SEND:   
    sbi SPCR,SPE        ;enable SPI
    out SPDR,r24        ;load byte in SPI data register
SPI_SEND_0: 
    sbis    SPSR,SPIF
    rjmp    SPI_SEND_0      ;transmitt byte to LCD
    in  r0,SPDR
    sbi PORTB,LCD_CS        ;deselect Display
    ret


; Init table

INIT1:  ; 24bytes
.byte 0xFD, 0xFD, 0xFD, 0xFD
.byte 0xEF, 0x00, 0xEE, 0x04, 0x1B, 0x04, 0xFE, 0xFE, 0xFE, 0xFE, 0xEF, 0x90, 0x4A, 0x04
.byte 0x7F, 0x3F, 0xEE, 0x04, 0x43, 0x06 

INIT2:  ; 40bytes
.byte 0xEF, 0x90,  0x09, 0x83,  0x08, 0x00,  0x0B, 0xAF,  0x0A, 0x00,  0x05, 0x00,  0x06, 0x00
.byte 0x07, 0x00,  0xEF, 0x00,  0xEE, 0x0C,  0xEF, 0x90,  0x00, 0x80,  0xEF, 0xB0,  0x49, 0x02 
.byte 0xEF, 0x00,  0x7F, 0x01,  0xE1, 0x81,  0xE2, 0x02,  0xE2, 0x76,  0xE1, 0x83

INIT3:  ; 2bytes
.byte  0x80, 0x01

; end of file
