

#ifndef MLS_DUMMY_H
#define MLS_DUMMY_H
#include "mlsTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief mlsDummyFill function
 *
 * Fill data in buff with a length
 * @param[in] Byte *buff. A buffer that be filled in
 * @param[in] UInt8 len. The length expect to fill dummmy data to a buffer
 * @return none
 */
void mlsDummyFill(Byte * buff, UInt8 len);
void mlsDummyFillEnergyLogged(void);
void mlsDummyFillTamperLogged(void);


#ifdef __cplusplus
}
#endif  

#endif
