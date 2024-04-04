#ifndef EM_DISPLAY_TN3P_HELPER_H
#define EM_DISPLAY_TN3P_HELPER_H

void d_COPEN(void);
void d_DNLD(void);
void d_COMM_ON(void);

// Current variables
void d_netCumKWH(void);

void d_CumKWH_Import(void);
void d_CumKVAH_Import(void);
void d_CumKVARH_Lag_Import(void);
void d_CumKVARH_Lead_Import(void);
void d_AVGPF_Current_Import(void);
void d_MD_Current_Import(void);
void d_CumKWH_TOD1_Import(void);
void d_CumKWH_TOD2_Import(void);
void d_CumKWH_TOD3_Import(void);
void d_CumKWH_TOD4_Import(void);
void d_CumKWH_TOD5_Import(void);


void d_CumKWH_Export(void);
void d_CumKVAH_Export(void);
void d_CumKVARH_Lag_Export(void);
void d_CumKVARH_Lead_Export(void);
void d_AVGPF_Current_Export(void);
void d_MD_Current_Export(void);
void d_CumKWH_TOD1_Export(void);
void d_CumKWH_TOD2_Export(void);
void d_CumKWH_TOD3_Export(void);
void d_CumKWH_TOD4_Export(void);
void d_CumKWH_TOD5_Export(void);

void d_MD_L1_Import(void);
void d_MD_BillDate_Import(void);
void d_MD_BillTime_Import(void);
void d_CumMD_Import(void);

void d_MD_L1_Export(void);
void d_MD_BillDate_Export(void);
void d_MD_BillTime_Export(void);
void d_CumMD_Export(void);

void d_Vrms_R(void);
void d_Vrms_Y(void);
void d_Vrms_B(void);
void d_Irms_R(void);
void d_Irms_Y(void);
void d_Irms_B(void);
void d_Irms_Neutral(void);
void d_SystemSignedPF(void);
void d_SignedPF_R(void);
void d_SignedPF_Y(void);
void d_SignedPF_B(void);
void d_LCDcheck(void);
void d_MeterSlNo(void);
void d_CurrentDate(void);
void d_CurrentTime(void);
void d_PhaseSequence(void);
void d_Frequency(void);
void d_ActivePower_R(void);
void d_ActivePower_Y(void);
void d_ActivePower_B(void);
void d_ActivePower_Total(void);
void d_ApparentPower_Total(void);
void d_RisingDemand_Import(void);
void d_RisingDemand_Export(void);

// Last Bill -L1
void d_CumKWH_L1_Import(void);
void d_CumKWH_TOD1_L1_Import(void);
void d_CumKWH_TOD2_L1_Import(void);
void d_CumKWH_TOD3_L1_Import(void);
void d_CumKWH_TOD4_L1_Import(void);
void d_CumKWH_TOD5_L1_Import(void);

void d_CumKWH_L1_Export(void);
void d_CumKWH_TOD1_L1_Export(void);
void d_CumKWH_TOD2_L1_Export(void);
void d_CumKWH_TOD3_L1_Export(void);
void d_CumKWH_TOD4_L1_Export(void);
void d_CumKWH_TOD5_L1_Export(void);

void d_CumKVAH_L1_Import(void);
void d_CumKVAH_L1_Export(void);

void d_AVGPF_L1_Import(void);
void d_AVGPF_L1_Export(void);

// prior to Last Bill - L2
void d_CumKWH_L2_Import(void);
void d_CumKVAH_L2_Import(void);
void d_AVGPF_L2_Import(void);
void d_MD_L2_Import(void);

void d_CumKWH_L2_Export(void);
void d_CumKVAH_L2_Export(void);
void d_AVGPF_L2_Export(void);
void d_MD_L2_Export(void);

void d_PowerOff_Bill_L2(void);

void d_resetCount(void);
void d_PowerOff_Current(void);
void d_PowerOff_Cum(void);

void d_LastBillDate(void);
void d_LastBillTime(void);

void d_Tampers(void);
void d_Last_Restored_Tamper(void);
void d_Tamper_Count(void);
void d_Magnet_Tamper_Indication(void);


void d_HighRes_KVARH_LEAD_Export(void);
void d_HighRes_KVARH_LAG_Export(void);
void d_HighRes_KVAH_Export(void);
void d_HighRes_KWH_Export(void);
void d_HighRes_KVARH_LEAD_Import(void);
void d_HighRes_KVARH_LAG_Import(void);
void d_HighRes_KVAH_Import(void);
void d_HighRes_KWH_Import(void);


int32_t get_Energy_Billing(uint8_t imp_ex_type, uint8_t whichBill, uint8_t whichEnergy);
int32_t get_Energy_Billing_TOD(uint8_t imp_ex_type, uint8_t whichBill, uint8_t whichZone, uint8_t whichEnergy);
int32_t getCumMD_uptillNow(uint8_t imp_ex_type, uint8_t mdType);
//int32_t findNumberOfElapsedMinutes(extrtc *currentDtTm, extrtc *prevDtTm);


// private functions
void displayApparentPower(EM_LINE line);
void displayActivePower(EM_LINE line);
void displayRisingDemand(uint8_t imp_ex_type);
void displaySignedPF(EM_LINE line);
void display_Vrms(EM_LINE line);
void display_Irms(EM_LINE line);
void display_MD_Billing(uint8_t imp_ex_type, uint8_t whichBill);
void display_MD_BillingDateTime_L1(uint8_t imp_ex_type, uint8_t dtOrTm);
ext_rtc_t get_MD_BillingDateTime(uint8_t imp_ex_type, uint8_t whichBill, uint8_t mdtype);
uint16_t get_MD_Billing(uint8_t imp_ex_type, uint8_t whichBill);
void display_MD_Current(uint8_t imp_ex_type);
//void displayMD(uint16_t md, uint8_t mdtype, uint8_t imp_ex_type);
void displayMD(uint16_t md, uint8_t mdtype, uint8_t imp_ex_type, uint8_t whichBill);

void display_CumMD(uint8_t imp_ex_type);

void display_Current_AVG_PF(uint8_t imp_ex_type);
void display_CumKVARH(uint8_t imp_ex_type, uint8_t lgld);
void display_CumKVAH(uint8_t imp_ex_type);
void displayEnergy(int32_t energy, uint8_t whichEnergy);
void display_CumKWH(uint8_t imp_ex_type);

void EM_DisplayInstantTime(void);
void EM_DisplayDate(void);

void display_PowerOff(uint8_t currOrCum);

void display_Energy_Billing(uint8_t imp_ex_type, uint8_t whichBill, uint8_t whichEnergy);

void display_Energy_Billing_TOD(uint8_t imp_ex_type, uint8_t whichBill, uint8_t whichIndex, uint8_t whichEnergy);
void display_Billing_AVG_PF(uint8_t imp_ex_type, uint8_t whichBill);
void displayBabyDigits_IE(uint8_t imp_ex_type);

void display_specific_tamper(uint8_t eventcodebit);



#endif /* EM_DISPLAY_TN3P_HELPER_H */
