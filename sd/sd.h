//**************************************************************
// ****** FUNCTIONS FOR SD RAW DATA TRANSFER *******
//**************************************************************
//Controller: ATmega64
//Compiler	: AVR-GCC (winAVR with AVRStudio)
//Version 	: 2.3
//Author	: CC Dharmani, Chennai (India)
//                www.dharmanitech.com
//Date		: 08 May 2010
//**************************************************************

//**************************************************
// ***** HEADER FILE : SD_routines.h ******
//**************************************************
#ifndef _SD_
#define _SD_

//Use following macro if you don't want to activate the multiple block access functions
//those functions are not required for FAT32

#define FAT_TESTING_ONLY         

// нога на которую подключен сигнал SS карты
// для ATmega32 (PB0)
#if defined(__AVR_ATmega32__)
	#define SD_CS_ASSERT     PORTB &= ~0x01 //(00000001)
	#define SD_CS_DEASSERT   PORTB |= 0x01	//(00000001)
#endif

// для ATmega64 (PB7)
#if defined(__AVR_ATmega64__)
	#define SD_CS_ASSERT     PORTB &= ~0x80 //(10000000)
	#define SD_CS_DEASSERT   PORTB |= 0x80	//(10000000)
#endif



//use following macros if SS (PB4) pin is used for Chip Select of SD
//#define SD_CS_ASSERT     PORTB &= ~0x10
//#define SD_CS_DEASSERT   PORTB |= 0x10

//SD commands, many of these are not used here
#define GO_IDLE_STATE            0
#define SEND_OP_COND             1
#define SEND_IF_COND			 8
#define SEND_CSD                 9
#define STOP_TRANSMISSION        12
#define SEND_STATUS              13
#define SET_BLOCK_LEN            16
#define READ_SINGLE_BLOCK        17
#define READ_MULTIPLE_BLOCKS     18
#define WRITE_SINGLE_BLOCK       24
#define WRITE_MULTIPLE_BLOCKS    25
#define ERASE_BLOCK_START_ADDR   32
#define ERASE_BLOCK_END_ADDR     33
#define ERASE_SELECTED_BLOCKS    38
#define SD_SEND_OP_COND			 41   //ACMD
#define APP_CMD					 55
#define READ_OCR				 58
#define CRC_ON_OFF               59


#define ON     1
#define OFF    0

volatile unsigned long startBlock, totalBlocks; 
volatile unsigned char SDHC_flag, cardType, buffer[512];

unsigned char sd_init(void);
unsigned char sd_init_a(void);
unsigned char SD_sendCommand(unsigned char cmd, unsigned long arg);
unsigned char SD_readSingleBlock(unsigned long startBlock);
unsigned char SD_writeSingleBlock(unsigned long startBlock);
unsigned char SD_readMultipleBlock (unsigned long startBlock, unsigned long totalBlocks);
unsigned char SD_writeMultipleBlock(unsigned long startBlock, unsigned long totalBlocks);
unsigned char SD_erase (unsigned long startBlock, unsigned long totalBlocks);



#endif
