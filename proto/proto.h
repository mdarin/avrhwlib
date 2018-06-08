#ifndef PROTO_H
#define PROTO_H

// Version 0.2
// Date 17.03.2014

#include "arv/io.h"
#include "uart.h"
#include "uart_addon.h"
// Добавить сюда заголовочники из рабочего проекта
// а вообще лучше работчий под этот подтачить :) 
// этот можно считать эталоном

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FALSE 
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#define wait_for_data(c) do{\
 c = uart_getc();\
 } while (!c);
 
#define append_to_frame(c) frame[frame_pos++] = c	
	
#define accumulate_frame(c) do { \
	wait_for_data(c);\
	append_to_frame(c); \
	} while (ETX != c && frame_pos < FRAME_SIZE)
 
#define get_frame_data(paket) &paket[3]
 
#define FRAME_SIZE 132 
 
#define SOH 0x01
#define ACK 0x06
#define NAK 0x15
#define EOT 0x04
#define ENQ 0x05
#define ETX 0x03

#define ESTABLISH 0
#define INTERACT 1
#define TERMINATE 2

typedef char frame_t;
typedef char frame_data_t;

typedef struct {
	char *comand;
	char (*action)(frame_t*);
} conf_cmd_t;

// Прототипы функций

// Команды
char on_get_sms__act(frame_t* frame);
char on_get_phonebook__act(frame_t* frame);
char on_set_sms__act(frame_t* frame);
char on_set_phonebook__act(frame_t* frame);
char on_get_devinfo__act(frame_t* frame);

// Транспорт протокола
void process_frame(frame_t* frame, conf_cmd_t* cc_list); 
void send_response(frame_t* frame);
char create_frame(frame_t* frame, frame_data_t* data);
// Пользовательская функция
void receive(void);

// Состояние взаимодействия 
static char proto_state = ESTABLISH;

// Перечень команд
static conf_cmd_t cc_list[] = {	
	{"get dev info", on_get_devinfo__act}, 
	{"set sms", on_set_sms__act},
	{"set phone book", on_set_phonebook__act},
	{"get sms", on_get_sms__act},
	{"get phone book", on_get_phonebook__act},
	{NULL, NULL},// end of list
};

#ifdef __cplusplus
}
#endif

#endif //PROTO_H