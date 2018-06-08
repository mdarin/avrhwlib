#include "poroto.h"

//------------------------------------------------------------------------------
char on_get_sms__act(frame_t* frame)
{
	char status = 1;
	if (NULL != frame) {
		frame_data_t* data = get_frame_data(frame);
		// получить данные
		strcpy(data, ";sms;");
		strcat(data, sms);
		// заполнить данными поле данных пакета
		//create_frame(frame, data);
		// отправить ответ с резултатом	
		//TODO: status = send_response(pacekt); так надо будет сделать	
		send_response(frame);
		status = 0;
	}
	return status;
}
//------------------------------------------------------------------------------
char on_get_phonebook__act(frame_t* frame)
{
	char status = 1;
	if (NULL != frame) {
		frame_data_t* data = get_frame_data(frame);
		// получить данные
		strcpy(data, ";phone book;");
		strcat(data, "1");	
		strcat(data, ";");	
		strcat(data, phonebook[0]);
		char buffer[8] = {0};	
		for (char i = 1; phonebook[i] != NULL; i++) {
			snprintf(buffer, 8, "%d", i+1);	
			strcat(data, ";");		
			strcat(data, buffer);		
			strcat(data, ";");		
			strcat(data, phonebook[i]);		
		}
		// занести данные в поле данных пакета
		//create_frame(pacekt, data);
		// отправить ответ с резултатом
		send_response(frame);
		status  = 0;
	}
	return status;
}
//------------------------------------------------------------------------------
char on_set_sms__act(frame_t* frame)
{
	char status = 1;
	if (NULL != frame) {
		frame_data_t* data = get_frame_data(frame);
		char* token = NULL;
		char* delims = ";";
		// здесь производится синтасический анализ данных
		// пока примитивно, но ресурсов тоже не бесконечно...:)	
		// убрать символ ETX наверное... надо пофиксить!
		data[strlen(data)-1] = '\0';
		token = strtok(data, delims);	  
		while (token != NULL) {  
			uart_puts("sms:");
			uart_puts(token);
			uart_puts("\r\n");
			// write sms to eeprom
			token = strtok(NULL, delims);
		}
		frame[0] = ACK; // TODO: Надо добавить обработку ошибок и слать NAK если что-то нетак :)	
		// занести данные в поле данных пакета
		//create_frame(pacekt, data);
		// отправить ответ с резултатом
		send_response(frame);
		status = 0;
	}
	return status;
}
//------------------------------------------------------------------------------
char on_set_phonebook__act(frame_t* frame)
{
	char status = 1;
	if (NULL != frame) {
		frame_data_t* data = get_frame_data(frame);
		char *token = NULL;
		char *delims = ";";
		unsigned char index = 0;
		// здесь производится синтасический анализ данных
		// пока примитивно, но ресурсов тоже не бесконечно...:)	
		// убрать символ ETX наверное... надо пофиксить!
		data[strlen(data)-1] = '\0';
		token = strtok(data, delims);  
		while (token != NULL) {  
			// если данные содержат номер телефона
			if ( ('+' == token[0] || isdigit(token[0])) 
			&& isdigit(token[1])  
			&& isdigit(token[2])
			&& isdigit(token[3])
			&& isdigit(token[4])
			&& isdigit(token[5])
			&& isdigit(token[6])) {
			   // полагаем, что это номре телефона
			   // записать номер по последнему считанному индексу
			   uart_puts("phno:");
			   uart_puts(token);
			   uart_puts("\r\n");
			   // write phonebook to eeprom
			// если данные содержать индекс в телефонной книге
			} else if (isdigit(token[0])) {
				uart_puts("idx[");
				uart_puts(token);
				uart_puts("]");
				index = atoi(token);
				uart_put_int(index);
				uart_puts("\r\n");
			}
			 
			token = strtok(NULL, delims);
		}
		data[0] = ACK;		
		// занести данные в поле данных пакета
		//create_frame(pacekt, data);
		// отправить ответ с резултатом
		send_response(frame);
		status = 0;
	}
	return status;
}
//------------------------------------------------------------------------------
char on_get_devinfo__act(frame_t* frame)
{
	char status = 1;
	if (NULL != frame) {
		frame_data_t* data = get_frame_data(frame);
		// получить данные
		strcpy(data, ";dev info;SW;1.02;HW;1.01;SN;123456789;ID;01;");
		// заполнить данными поле данных пакета
		//create_frame(frame, data);
		// отправить ответ с резултатом
		send_response(frame);
		status = 0;
	}
	return status;
}
//------------------------------------------------------------------------------




void process_frame(frame_t* frame, conf_cmd_t* cc_list) 
{
  // разобрать кард
  if (NULL != frame && NULL != cc_list) {
	// выбрать начало кадра
    if (SOH == frame[0]) {
	  // выбрать конец кадра, ялвятеся подтверждением достовернтсти кадра
	  if (ETX == frame[strlen(&(frame[3]))+2]) {
	    // если данные есть, то обработать
        if (frame[1] > 0) {
	      // если пакет правильный, то
	      // определить команду методом сопоставления со списком команд
	      for (char i = 0, done = 0; cc_list[i].comand != NULL && !done; i++) {
		    // если команда определена, то выполнить предписанные дейсвия
		    if (NULL != strstr(&(frame[3]), cc_list[i].comand)) {
		      cc_list[i].action(frame);	
			  done = 1;
		    }
          }
	    }
	  }
    }
  }
  return;
}

//------------------------------------------------------------------------------
char create_frame(frame_t* frame, frame_data_t* data)
{
	char status = 1;	
	if (NULL != frame && NULL != data) {
	  status = 0;
	  char pdl = strlen(data);
	  frame[0] = SOH;
	  frame[1] = 1;
	  frame[2] = pdl;
	  strcpy(&(frame[3]), data);
	  frame[4+pdl-1] = ETX; // -1 нужен чтобы отсечь символ \0 терминатор
	  status = 0;
	}
	return status;
}
//------------------------------------------------------------------------------
void send_response(frame_t* frame) 
{
    if (NULL == frame)
		return;
	
	switch (frame[0]) {
	case ACK:
	  uart_putc(ACK);
	  break;
	case NAK:
	  uart_putc(NAK);
	  break;
	default:
	  uart_puts(frame);
	  break;
	}	
	
	return;
}
//------------------------------------------------------------------------------
void receive(void) 
{	
	// вдимо надо структуру пакета сделать 
	// или сделать переменные указателями глобальными и реализовать интерфейс к ним
	char frame_pos = 0;
  	frame_t frame[FRAME_SIZE] = {0};
  	char c;
  	char done = 0;
		 
	switch (proto_state) {
	case ESTABLISH: // ----------------------
	  while (!done) {
	    wait_for_data(c);
	    if (ENQ == c) {
	      uart_putc(ACK);
	  	  done = 1;
		  proto_state = INTERACT;
	    }	 
	  }
	  break;
	case INTERACT: // ----------------------
	  while (!done) {
	    wait_for_data(c);
	    switch (c) {
		case SOH: //TODO: exit on success or timeout if needed
		  append_to_frame(c);
		  accumulate_frame(c);
		  process_frame(frame, cc_list);
		  frame_pos = 0;
		  done = TRUE;
		  break;
		case EOT:
		  frame_pos = 0;
		  proto_state = TERMINATE;
		  done = TRUE;
		  break;
		}  		 
	  }
	  break;
	case TERMINATE: // ----------------------
	  uart_putc(ACK);
	  proto_state = ESTABLISH;
	  break;
	}	
	return;
}