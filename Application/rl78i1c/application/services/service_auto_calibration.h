/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : service.h
* Version      : 1.00
* Description  : 
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 02.01.2013 
******************************************************************************/

#ifndef __SERVICE_EMAUTOCALIB_H
#define __SERVICE_EMAUTOCALIB_H

/******************************************************************************
Macro definitions
******************************************************************************/
#define EM_DATA_ID_AUTO_CALIBRATION_STEP1_1PHASE    (0x01)
#define EM_DATA_ID_AUTO_CALIBRATION_STEP1           (0x21)
#define EM_DATA_ID_AUTO_CALIBRATION_STEP1_2         (0x22)
#define EM_DATA_ID_AUTO_CALIBRATION_STEP1_3         (0x23)
#define EM_DATA_ID_AUTO_CALIBRATION_STEP1_4         (0x24)
#define EM_DATA_ID_AUTO_CALIBRATION_STEP2           (0x02)

#define CONVERT_ID_TO_CALIB_LINE(id)                ((id & 0x0F)-1)

#endif /*  __SERVICE_EMAUTOCALIB_H */
