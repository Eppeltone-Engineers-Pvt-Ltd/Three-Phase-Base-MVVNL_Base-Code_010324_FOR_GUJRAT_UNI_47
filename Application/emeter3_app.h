// emeter3_app.h
#ifndef EMETER3_APP_H

#include "em_type.h"
// verify these numbers


float32_t getInstantaneousParams(uint8_t param, EM_LINE line);
int32_t	compute_high_resolution_active_energy(void);

void per_second_activity(void);
void per_minute_activity(void);
void do_initial_things(void);
void update_saved_energies(void);

void generate_bill(uint8_t dat, uint8_t mth, uint8_t yr, uint8_t hrs, uint8_t mnts, uint8_t secs, uint8_t bill_type);
void updatePresentZoneTodEnergies(uint8_t dType);// used in billing
void updateBothPresentZoneTodEnergies(void);	// used when zone changes

void updateEnergyDataZoneEnergies(void);
//void clearZoneLastSavedEnergies(void);








#define EMTER3_APP_H
#endif