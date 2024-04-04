
/**
* \addtogroup mlsCIEng
* @{
*/

/**
* @file mlsCIEng.cpp
* @brief mlsCIEng
*
* This port is purposely to debug the CI framework in windows os
*
*/

/*----------------------------------------------------------------------------
Standard include files:
These should always be included !
--------------------------------------------------------------------------*/
#include "mlsInclude.h"
/*---------------------------------------------------------------------------
Project include files:

-----------------------------------------------------------------------------*/
#include "mlsUltil.h"

/*---------------------------------------------------------------------------
Local Types and defines:
--------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
Global variables:
--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Static Variables:
--------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
Static Function protoypes:
--------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
Exported functions:
--------------------------------------------------------------------------*/

UInt16 mlsUltil_CcittCrc16Calc(Byte* data_p, UInt16 length)
{
    Byte i;
    UInt16 data;
    UInt16 crc;

    crc = 0xffff;

    if (length == 0)
        return (~crc);

    do 
    {
        data = (UInt16)*data_p++;
        for (i = 0;i < 8; i++) 
        {
            if ((crc & 0x0001) ^ (data & 0x0001))
                crc = (crc >> 1) ^ POLY;
            else
                crc >>= 1;
                
            data >>= 1;
        }
    } while (--length);

    crc = ~crc;

    data = crc;
    crc = (UInt16)(crc << 8) | (UInt16)(data >> 8 & 0xFF);

    return (crc);
}