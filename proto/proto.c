#include "poroto.h"

//------------------------------------------------------------------------------
char on_get_sms__act(frame_t* frame)
{
	char status = 1;
	if (NULL != frame) {
		frame_data_t* data = get_frame_data(frame);
		// �������� ������
		strcpy(data, ";sms;");
		strcat(data, sms);
		// ��������� ������� ���� ������ ������
		//create_frame(frame, data);
		// ��������� ����� � ����������	
		//TODO: status = send_response(pacekt); ��� ���� ����� �������	
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
		// �������� ������
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
		// ������� ������ � ���� ������ ������
		//create_frame(pacekt, data);
		// ��������� ����� � ����������
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
		// ����� ������������ ������������� ������ ������
		// ���� ����������, �� �������� ���� �� ����������...:)	
		// ������ ������ ETX ��������... ���� ���������!
		data[strlen(data)-1] = '\0';
		token = strtok(data, delims);	  
		while (token != NULL) {  
			uart_puts("sms:");
			uart_puts(token);
			uart_puts("\r\n");
			// write sms to eeprom
			token = strtok(NULL, delims);
		}
		frame[0] = ACK; // TODO: ���� �������� ��������� ������ � ����� NAK ���� ���-�� ����� :)	
		// ������� ������ � ���� ������ ������
		//create_frame(pacekt, data);
		// ��������� ����� � ����������
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
		// ����� ������������ ������������� ������ ������
		// ���� ����������, �� �������� ���� �� ����������...:)	
		// ������ ������ ETX ��������... ���� ���������!
		data[strlen(data)-1] = '\0';
		token = strtok(data, delims);  
		while (token != NULL) {  
			// ���� ������ �������� ����� ��������
			if ( ('+' == token[0] || isdigit(token[0])) 
			&& isdigit(token[1])  
			&& isdigit(token[2])
			&& isdigit(token[3])
			&& isdigit(token[4])
			&& isdigit(token[5])
			&& isdigit(token[6])) {
			   // ��������, ��� ��� ����� ��������
			   // �������� ����� �� ���������� ���������� �������
			   uart_puts("phno:");
			   uart_puts(token);
			   uart_puts("\r\n");
			   // write phonebook to eeprom
			// ���� ������ ��������� ������ � ���������� �����
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
		// ������� ������ � ���� ������ ������
		//create_frame(pacekt, data);
		// ��������� ����� � ����������
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
		// �������� ������
		strcpy(data, ";dev info;SW;1.02;HW;1.01;SN;123456789;ID;01;");
		// ��������� ������� ���� ������ ������
		//create_frame(frame, data);
		// ��������� ����� � ����������
		send_response(frame);
		status = 0;
	}
	return status;
}
//------------------------------------------------------------------------------




void process_frame(frame_t* frame, conf_cmd_t* cc_list) 
{
  // ��������� ����
  if (NULL != frame && NULL != cc_list) {
	// ������� ������ �����
    if (SOH == frame[0]) {
	  // ������� ����� �����, �������� �������������� ������������� �����
	  if (ETX == frame[strlen(&(frame[3]))+2]) {
	    // ���� ������ ����, �� ����������
        if (frame[1] > 0) {
	      // ���� ����� ����������, ��
	      // ���������� ������� ������� ������������� �� ������� ������
	      for (char i = 0, done = 0; cc_list[i].comand != NULL && !done; i++) {
		    // ���� ������� ����������, �� ��������� ������������ �������
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
	  frame[4+pdl-1] = ETX; // -1 ����� ����� ������ ������ \0 ����������
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
	// ����� ���� ��������� ������ ������� 
	// ��� ������� ���������� ����������� ����������� � ����������� ��������� � ���
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