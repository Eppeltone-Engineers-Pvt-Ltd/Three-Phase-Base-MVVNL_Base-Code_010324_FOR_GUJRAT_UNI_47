
#ifndef STATUS_H
#define STATUS_H


#include "iodefine.h"
#include "r_cg_userdefine.h"

//#pragma sfr
#pragma DI
#pragma EI
#pragma NOP
#pragma HALT
#pragma STOP

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#ifndef __TYPEDEF__
/* Status list definition */
#define MD_STATUSBASE       0x00U
#define MD_OK               (MD_STATUSBASE + 0x00U) /* register setting OK */
#define MD_SPT              (MD_STATUSBASE + 0x01U) /* IIC stop */
#define MD_NACK             (MD_STATUSBASE + 0x02U) /* IIC no ACK */
#define MD_BUSY1            (MD_STATUSBASE + 0x03U) /* busy 1 */
#define MD_BUSY2            (MD_STATUSBASE + 0x04U) /* busy 2 */

/* Error list definition */
#define MD_ERRORBASE        0x80U
#define MD_ERROR            (MD_ERRORBASE + 0x00U)  /* error */
#define MD_ARGERROR         (MD_ERRORBASE + 0x01U)  /* error agrument input error */
#define MD_ERROR1           (MD_ERRORBASE + 0x02U)  /* error 1 */
#define MD_ERROR2           (MD_ERRORBASE + 0x03U)  /* error 2 */
#define MD_ERROR3           (MD_ERRORBASE + 0x04U)  /* error 3 */
#define MD_ERROR4           (MD_ERRORBASE + 0x05U)  /* error 4 */
#define MD_DATAEXISTS       (MD_ERRORBASE + 0x06U)  /* data to be transferred next exists in TXBn register */
#endif

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
#ifndef __TYPEDEF__
typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef signed short        int16_t;
typedef unsigned short      uint16_t;
typedef signed long         int32_t;
typedef unsigned long       uint32_t;
typedef signed long long    int64_t;
typedef unsigned long long  uint64_t;
typedef unsigned short      MD_STATUS;
#define __TYPEDEF__
#endif

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/

#endif
