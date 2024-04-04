/*******************************************************************************
* Library       : Flash Data Library T04 (PicoFDL)
*
* File Name     : $Source: pfdl.h $
* Lib. Version  : $RL78_FDL_LIB_VERSION_T04_CCRL: V1.05 $
* Mod. Revision : $Revision: 1.3 $
* Mod. Date     : $Date: 2014/12/12 00:50:07JST $
* Device(s)     : RL78
* Description   : C language API definition of the Flash Data Library
*******************************************************************************/


#ifndef __PFDL_H_INCLUDED
#define __PFDL_H_INCLUDED


/*================================================================================================*/
/* include files list                                                                             */
/*================================================================================================*/
#include "pfdl_types.h"


/* ---------------------------------------------------------------------------------------------------------------*/
/*  Block type:   standard function                                                                               */
/* ---------------------------------------------------------------------------------------------------------------*/
/*  Purpose:      Unconditional activation of the Data Flash Library.                                             */
/*                Based on the descriptor data:                                                                   */
/*                - the flash firmware will be initialized for data-flash access only                             */
/*                - the internal timing and low-voltage capability will be configured according to the descriptor */
/*                After successful initialization the data flash clock is ON and the PFDL is ready to use.        */
/*                                                                                                                */
/*                CAUTION:                                                                                        */
/*                Due to the code size minimization no plausibility checks are done by the PicoFDL.               */
/*                Neither configuration, frequency range nor data flash size will be checked by the library.      */
/*                                                                                                                */
/*  Input:        address of the PFDL descriptor variable  (RAM only)                                             */
/*  Output:       -                                                                                               */
/*  Return:       PFDL status                                                                                     */
/* ---------------------------------------------------------------------------------------------------------------*/
extern  pfdl_status_t  __far PFDL_Open(__near pfdl_descriptor_t* descriptor_pstr);


/* ---------------------------------------------------------------------------------------------------------------*/
/*  Block type:   standard function                                                                               */
/* ---------------------------------------------------------------------------------------------------------------*/
/*  Purpose:      Disable data flash access unconditionally.                                                      */
/*                If any command is just executed, PFDL_Close will stop it immediately.                           */
/*                After return the data flash clock is switched OFF.                                              */
/*  Input:        -                                                                                               */
/*  Output:       -                                                                                               */
/*  Return:       -                                                                                               */
/* ---------------------------------------------------------------------------------------------------------------*/
extern  void  __far PFDL_Close(void);


/* ---------------------------------------------------------------------------------------------------------------*/
/*  Block type:   standard function                                                                               */
/* ---------------------------------------------------------------------------------------------------------------*/
/*  Purpose:      Initiating execution of the PFDL request related to the data flash.                             */
/*                The corresponding request variable has to be parametrized before.                               */
/*                                                                                                                */
/*                request_pstr->index_u16        :   byte-index or block-number within PFDL-pool                  */
/*                request_pstr->data_pu08        :   start address of the RAM data that should be read/written    */
/*                request_pstr->bytecount_u16    :   number of bytes has to be read/written                       */
/*                request_pstr->command_enu      :   command code                                                 */
/*                                                                                                                */
/*                CAUTION:                                                                                        */
/*                Due to the code size minimization no plausibility checks are done by the PFDL.                  */
/*                                                                                                                */
/*  Input:        &request_pstr - pointer to PFDL request variable                                                */
/*  Output:       -                                                                                               */
/*  Return:       status of the request                                                                           */
/* ---------------------------------------------------------------------------------------------------------------*/
extern  pfdl_status_t  __far PFDL_Execute(__near pfdl_request_t* request_pstr);


/* ---------------------------------------------------------------------------------------------------------------*/
/*  Block type:   standard function                                                                               */
/* ---------------------------------------------------------------------------------------------------------------*/
/*  Purpose:      Continuation of command execution and status update of requests being under execution.          */
/*  Input:        -                                                                                               */
/*  Output:       -                                                                                               */
/*  Return:       PFDL status =                                                                                   */
/*                  PFDL_IDLE        -  no request is processed by PFDL, PFDL is ready to receive new requests    */
/*                  PFDL_OK          -  processed request/command finished without problems                       */
/*                  PFDL_BUSY        -  request/command is still being processed                                  */
/*                  other            -  flash or firmware related errors                                          */
/* ---------------------------------------------------------------------------------------------------------------*/
extern  pfdl_status_t  __far PFDL_Handler(void);


/* ---------------------------------------------------------------------------------------------------------------*/
/*  Block type:   standard function                                                                               */
/* ---------------------------------------------------------------------------------------------------------------*/
/*  Purpose:      Returns the start address of the library version string                                         */
/*                                                                                                                */
/*  Input:        -                                                                                               */
/*  Output:       -                                                                                               */
/*  Return:       starting address of the zero-terminated version string                                          */
/* ---------------------------------------------------------------------------------------------------------------*/
extern  __far pfdl_u08* __far PFDL_GetVersionString(void);


#endif
