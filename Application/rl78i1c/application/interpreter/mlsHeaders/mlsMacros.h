/**
 * @defgroup mlsHeaderFile
 * @brief brief description of the module
 *
 * Detail description of the module
 * @{
 */
 
 /**
* @file mlsMacro.h
* @brief project wide defination 
*
* This header file contains project wide defination
*
*/

#ifndef __MLMACRO_H__
#define __MLMACRO_H__

#include "mlsTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*! \def MLS_MACRO_CHAR_TO_INT32 (a,b,c,d)                                                                                
    Char to int32                                                                            
 */
#define MLS_MACRO_CHAR_TO_INT32(a,b,c,d)     \
    ((UInt32) ((UInt8) (a) << 24 | (UInt8) (b) << 16 | (UInt8) (c) << 8 | (UInt8) (d)))

    
/*! \def MLS_MACRO_MAX( __x, __y )                                                                               
    Max of 2 number                                                                            
 */
#define MLS_MACRO_MAX( __x, __y )  ((__x) > (__y) ? (__x) : (__y))


/*! \def MLS_MACRO_MIN( __x, __y )                                                                               
    Min of 2 number                                                                            
 */
#define MLS_MACRO_MIN( __x, __y ) ((__x) < (__y) ? (__x) : (__y))


/*! \def MLS_MACRO_ABS( __x )                                                                               
    ABS calculate                                                                            
 */
#define MLS_MACRO_ABS( __x ) (((__x) > 0) ? (__x) : -(__x))


/*! \def MLS_MACRO_LIMIT( __x, __y, __z )                                                                             
    The value __x or the bound __y or __z if outside them                                                                            
 */
#define MLS_MACRO_LIMIT( __x, __y, __z ) MAX( MIN( (__x), MAX( (__y), (__z) )), MIN( (__y), (__z) ))


/*! \def MLS_ASSERT(__x, __y)                                                                            
                                                                                
 */
#define MLS_ASSERT(__x, __y) {while(__x == __y);}


/*! \def MLS_UNUSED_PARAMETER(param)                                                                           
                                                                                
 */
#define MLS_UNUSED_PARAMETER(param) ((void)(param))


/*! \def MLS_ARRAY_SIZE(x)                                                                          
                                                                                
 */
#define MLS_ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))


#ifndef MAX
#define MAX MAX MLS_MACRO_MAX
#endif

#ifndef MIN
#define MIN MLS_MACRO_MIN
#endif

#ifndef ABS
#define ABS MLS_MACRO_ABS
#endif

#ifndef LIMIT
#define LIMIT MLS_MACRO_LIMIT
#endif

#ifndef ASSERT
#define ASSERT(a,b)
#endif

#ifdef assert
#undef assert
#endif
#define assert(e) if(!(e))while(1)

/*! \def SWAP_16(x)                                                                         
    
 */
#define SWAP_16(x) ((((x) >> 8)&0xFF) | (((x) << 8)&0xFF00))

/*! \def SWAP_32(x)                                                                        
    
 */
#define SWAP_32(x) ((((x) >> 24)&0xFF) | (((x) >> 8) & 0x0000FF00)\
                        |(((x) << 8) & 0x00FF0000) | (((x) << 24)& 0xFF000000))
                        
/*! \def MLS_BYTE_0(x)                                                                        
    
 */
#define MLS_BYTE_0(x) (UInt8(x))

/*! \def MLS_BYTE_1(x)                                                                        
    
 */
#define MLS_BYTE_1(x) (UInt8(x>>8))

/*! \def MLS_BYTE_2(x)                                                                        
    
 */
#define MLS_BYTE_2(x) (UInt8(x>>16))

/*! \def MLS_BYTE_3(x)                                                                        
    
 */
#define MLS_BYTE_3(x) (UInt8(x>>24))

#ifdef __cplusplus
}
#endif

#endif /*__MLMACRO_H__  */
/** @} */
