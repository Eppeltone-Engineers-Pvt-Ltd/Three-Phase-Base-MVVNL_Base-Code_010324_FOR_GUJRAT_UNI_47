/**
 * @defgroup mlsHeaderFile
 * @brief brief description of the module
 *
 * Detail description of the module
 * @{
 */
 
 /**
* @file mlsTypes.h.h
* @brief project wide defination 
*
* This header file contains project wide defination
*
*/


#ifndef __MLS_TYPES_H__
#define __MLS_TYPES_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*-----------------------------------------------------------------------------
 * Types and defines:
 *-----------------------------------------------------------------------------*/
#ifndef False
    #define False           0
#endif

#ifndef True
    #define True            1
#endif

#ifndef Null
    #define Null            ((Void *) 0)
#endif

#undef NULL
#ifndef NULL
    #define NULL            (0)
#endif

/*****************************************************************************
 *      Standard Types
 *****************************************************************************/
 /*! \def const                                                                            
    keyword const                                                                         
 */
#define Const const

 /*! \typedef Byte                                                                            
    data type Byte                                                                         
 */
typedef signed char Byte; 

 /*! \typedef Int8,   *pInt8                                                                         
    data type Int8,   *pInt8                                                                         
 */
typedef signed   char   Int8,   *pInt8;     /*  8 bit   signed integer */

 /*! \typedef Int16,  *pInt16                                                                        
    data type Int16,  *pInt16                                                                         
 */
typedef signed   short  Int16,  *pInt16;    /* 16 bit   signed integer */

 /*! \typedef Int32,  *pInt32                                                                       
    data type Int32,  *pInt32                                                                          
 */
typedef signed   long   Int32,  *pInt32;    /* 32 bit   signed integer */

 /*! \typedef UInt8,  *pUInt8                                                                       
    data type UInt8,  *pUInt8                                                                          
 */
typedef unsigned char   UInt8,  *pUInt8;    /*  8 bit unsigned integer */

 /*! \typedef UInt16, *pUInt16                                                                       
    data type UInt16, *pUInt16                                                                         
 */
typedef unsigned short  UInt16, *pUInt16;   /* 16 bit unsigned integer */

 /*! \typedef UInt32, *pUInt32                                                                       
    data type UInt32, *pUInt32                                                                        
 */
typedef unsigned long   UInt32, *pUInt32;   /* 32 bit unsigned integer */

 /*! \typedef Void,   *pVoid                                                                       
    keyword Void,   *pVoid                                                                       
 */
typedef void            Void,   *pVoid;     /* Void (typeless) */

 /*! \typedef Float,  *pFloat                                                                       
    data type Float,  *pFloat                                                                      
 */
typedef float           Float,  *pFloat;    /* 32 bit floating point */

 /*! \typedef Double, *pDouble                                                                       
    data type Double, *pDouble                                                                      
 */
typedef double          Double, *pDouble;   /* 32/64 bit floating point */
//typedef unsigned char   Bool,   *pBool;     /* Boolean (True/False) */

 /*! \typedef Char,   *pChar                                                                      
    data type Char,   *pChar                                                                     
 */
typedef char            Char,   *pChar;     /* character, character array ptr */
typedef char           *String, **pString;  /* Null terminated 8 bit char str, */
 
 /*! \typedef *ConstString                                                                      
    data type *ConstString                                                                     
 */                                           
typedef char const     *ConstString;        /* Null term 8 bit char str ptr */

//typedef signed   long long int Int64,  *pInt64;  /* 64-bit integer    */
//typedef unsigned long long int UInt64, *pUInt64; /* 64-bit bitmask */

#ifdef __cplusplus
}
#endif

#endif /*__MLSTYPES_H__  */
/** @} */
