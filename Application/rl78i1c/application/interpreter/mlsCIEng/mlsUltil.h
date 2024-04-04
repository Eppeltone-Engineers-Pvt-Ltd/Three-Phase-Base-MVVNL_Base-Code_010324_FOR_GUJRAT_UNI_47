
/**
* @defgroup mlsCIHandler
* @brief  mlsCIHandler interface to EM Core library to service the request from
* Terminal
* @{
*/

/**
* @file mlsCIHandler.h
* @brief contains API prototype declaration of mlsCIHandler module
*
* This is the mlsCIHandler component
*
*/

#ifndef _ULTIL_H_
#define _ULTIL_H_
#ifdef __cplusplus
extern "C"
{
#endif
/* define prototype and struct */
#define POLY 0x8408
/*
// note: when the crc is included in the message, the valid crc is:
//    0xF0B8, before the compliment and byte swap,
//    0x0F47, after compliment, before the byte swap,
//    0x470F, after the compliment and the byte swap.
*/
#define CRC_OK 0x470F;
/**************************************************************************
//
// crc16() - generate a 16 bit crc
//
//
// PURPOSE
//    This routine generates the 16 bit remainder of a block of
//    data using the ccitt polynomial generator.
//
// CALLING SEQUENCE
//    crc = crc16(data, len);
//
// PARAMETERS
//    data  <-- address of start of data block
//    len   <-- length of data block
//
// RETURNED VALUE
//    crc16 value. data is calcuated using the 16 bit ccitt polynomial.
//
// NOTES
//    The CRC is preset to all 1's to detect errors involving a loss
//      of leading zero's.
//    The CRC (a 16 bit value) is generated in LSB MSB order.
//    Two ways to verify the integrity of a received message
//      or block of data:
//      1) Calculate the crc on the data, and compare it to the crc
//         calculated previously. The location of the saved crc must be
//         known.
/        2) Append the calculated crc to the end of the data. Now calculate
//         the crc of the data and its crc. If the new crc equals the
//         value in "crc_ok", the data is valid.
//
// PSEUDO CODE:
//    initialize crc (-1)
//    DO WHILE count NE zero
//      DO FOR each bit in the data byte, from LSB to MSB
//        IF (LSB of crc) EOR (LSB of data)
//          crc := (crc / 2) EOR polynomial
//        ELSE
//          crc := (crc / 2)
//        FI
//      OD
//    OD
//    1's compliment and swap bytes in crc
//    RETURN crc
//
**************************************************************************/

/**
 * @brief mlsUltil_CcittCrc16Calc function
 *
 * Calculate the checksum16 of a buffer with a length
 * @param[in]: param1 is Byte* data_p. The checksum buffer
 * @param[in]: param2 is UInt16 length. The length of checksum buffer 
 * @return checksum result
 */
UInt16 mlsUltil_CcittCrc16Calc(Byte* data_p, UInt16 length);

#ifdef __cplusplus
}
#endif

#endif
