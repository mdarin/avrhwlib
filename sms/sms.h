#ifndef SMS_H
#define SMS_H

/*
 * Funcition prototypes
 */
unsigned char sms_hex2bin(unsigned char c);
unsigned char sms_bin2hex(unsigned char n);
unsigned int sms_win2uni(unsigned char c);
unsigned char sms_create_pdu(unsigned char *sms, const unsigned char *num, const unsigned char *msg);

#endif //SMS_H