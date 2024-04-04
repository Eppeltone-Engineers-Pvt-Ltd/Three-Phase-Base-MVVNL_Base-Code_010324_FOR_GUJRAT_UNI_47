/* CA78K0R C Source Converter V1.00.01.01 [25 Aug 2016] */
/*****************************************************************************
 DISCLAIMER
 This software is supplied by Renesas Electronics Corporation and is only
 intended for use with Renesas products. No other uses are authorized. This
 software is owned by Renesas Electronics Corporation and is protected under
 all applicable laws, including copyright laws.
 THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
 LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
 TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
 ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
 FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
 ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
 BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 Renesas reserves the right, without notice, to make changes to this software
 and to discontinue the availability of this software. By using this software,
 you agree to the additional terms and conditions found by accessing the
 following link:
 http://www.renesas.com/disclaimer
 Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
******************************************************************************/
/****************************************************************************************/
/*                                                                     					*/
/*  M16C/62P Group Program Collection                                  					*/
/*                                                                     					*/
/*  FILE NAME : xmodem.h                                       							*/
/*  CPU       : This program is the Serial flash boot loader by Xmodem data transfer	*/
/*  HISTORY   : 2006.12.08 Ver 0.01                                    					*/
/*                                                                    	 				*/
/*  Copyright (C) 2006. Renesas Technology Corp.                       					*/
/*  Copyright (C) 2006. Renesas Solutions Corp.                        					*/
/*  All right reserved.                                                					*/
/*                                                                     					*/
/*****************************************************************************************/

//#include "serial.h"

#ifndef _XMODEM_H
#define _XMODEM_H

#define SOH					0x01
#define EOT					0x04
#define ACK					0x06
#define	NAK					0x15
#define CAN					0x18

#define XM_OK				0x00
#define XM_ADDRESS_ERROR	0x01
#define XM_COMMS_ERROR		0x02
#define XM_TIMEOUT			0x04
#define XM_PROG_FAIL		0x08

// function prototypes


#endif
