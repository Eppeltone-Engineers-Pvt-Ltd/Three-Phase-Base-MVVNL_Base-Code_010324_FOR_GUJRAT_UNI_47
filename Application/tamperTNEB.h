#ifndef TAMPER_TNEB_H
#define TAMPER_TNEB_H

void loadControlTable(uint8_t whichGroup);
void createControlTable(uint32_t *wrt_ptr, uint32_t base_addr, uint32_t end_address, uint8_t whichGroup);
void TNEB_Tamper_Write(uint32_t *wrt_ptr, uint32_t base_addr, uint32_t end_addr, eventdata_t* eventData, uint8_t whichGroup);


#endif
