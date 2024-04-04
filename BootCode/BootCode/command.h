#ifndef _COMMAND_H
#define _COMMAND_H
#define Start_Block 0x10
#define No_Of_Blocks 0xFE // Last block is excluded 
#define	FIRST_USER_FLASH_BLK	0x10
#define FIRST_USER_FLASH_ADDR	0x4000 //0x5000

#define LAST_USER_FLASH_ADDR	0x3FBFF//0x3FBFF
#define LAST_USER_WRITE_FLASH_ADDR	0x3FBFF

void InitCommandHandler (void);
void RunCommandHandler (void);
void ShowMenu (void);
void Command_1 (void);
void Command_2 (void);
void Command_3 (void);
void Command_4 (void);
void Command_5 (void);

void reverse(char s[]);
void itoa(long n, char s[], unsigned char base);

typedef unsigned char (*pt2FunctionErase)(unsigned long, unsigned short * );

#endif

