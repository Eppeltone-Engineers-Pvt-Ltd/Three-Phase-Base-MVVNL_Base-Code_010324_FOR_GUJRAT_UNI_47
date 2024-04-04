#ifndef __LIB_64BIT_H
#define __LIB_64BIT_H

#include "typedef.h"

extern void CONV_ftod_asm(void *d, float32_t f);
extern void CONV_slltof_asm(void *f_output, void * sll_input);

#endif /* __LIB_64BIT_H */
