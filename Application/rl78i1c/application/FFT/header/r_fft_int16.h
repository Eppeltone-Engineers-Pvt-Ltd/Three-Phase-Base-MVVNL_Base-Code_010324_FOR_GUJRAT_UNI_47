/*
     FFT Library

    Copyright (C) 2006, 2015 RENESAS ELECTRONICS CORPORATION AND
                   RENESAS SYSTEM DESIGN CO., LTD. ALL RIGHT RESERVED
*/

/* $Id$ */
/* $Date$ */

#if !defined(_R_FFT_INT16_H_)

#define _R_FFT_INT16_H_

//#include "typedef.h" 

typedef int16_t R_FFT_OUTDATA;
typedef int16_t R_FFT_DATA;
typedef int16_t R_FFT_INDATA;
typedef int16_t R_FFT_WINDATA;

#define RFFT512P (512)
#define RFFT256P (256)

void R_rfft512_int16(R_FFT_INDATA *, R_FFT_OUTDATA *, R_FFT_WINDATA *, R_FFT_DATA *);
void R_rfft256_int16(R_FFT_INDATA *, R_FFT_OUTDATA *, R_FFT_WINDATA *, R_FFT_DATA *);

#endif

/*
    Copyright (C) 2006, 2015 RENESAS ELECTRONICS CORPORATION AND
                   RENESAS SYSTEM DESIGN CO., LTD. ALL RIGHT RESERVED
*/
