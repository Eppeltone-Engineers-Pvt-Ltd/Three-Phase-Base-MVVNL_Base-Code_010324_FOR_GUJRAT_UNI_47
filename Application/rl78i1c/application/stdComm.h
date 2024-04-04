#ifndef PVK_COMM_H
#define PVK_COMM_H

#define USE_DLMS

void PVK_Comm_Init(void);
//void PVK_ReceiveByte(uint8_t g_received_byte);
void PVK_ReceiveByte(uint8_t g_received_byte, uint8_t whichPort);
void PVK_Comm_Start(void);


//----------------- rs232 transmit functions -----------------
void asendcrlf(void);

//---------------- rs232 receive functions --------------------
void agetmsg(void);
void aanalmsg(void);
void PVK_TX_Complete(void);
uint8_t nowcheck(uint8_t* mbuf, uint8_t* conststr);
//uint8_t asendHexBufferInAscii(uint8_t* e2buf, uint16_t nBytes);	// avoid this function - use only in development
uint8_t asendHexMultiple(uint8_t* e2buf, uint16_t nBytes);
void asendbare(uint8_t* a);


//------------------ general stuff ----------------------
void strtoulong(char str[], int32_t *lng);
char hexToDec(char he);
char char2bin(char ch);

void copyArr(char a[],char b[]);
//char hexchars2byte(char cupper,char clower);
//char hexchar2nibble(char c);
char calcChksum(char buf[]);
void sendERROR(void);
void sendok(void);
void sendok2(void);
#endif
