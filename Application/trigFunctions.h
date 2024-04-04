// trigFunctions header file
#ifndef TRIG_FUNCTIONS_H
#define TRIG_FUNCTION_H

// angle can lie between 0 and 90, both included
// pf is to be multiplied by 10000
// findCos value is to be divided by 10000 to get the pf
int16_t findCos(int16_t angle);
int16_t findArcCos(int16_t signedPF, int16_t seedIndex);
#endif