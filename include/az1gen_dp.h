/*
 * az1gen_dp.h
 *
 *  Created on: 2023Äê3ÔÂ22ÈÕ
 *      Author: Administrator
 */

#ifndef AZ1GEN_DP_H_
#define AZ1GEN_DP_H_


#include "parameter.h"
#include "C28x_FPU_FastRTS.h"
#include "F28x_Project.h"   // Device Headerfile and Examples Include File

#pragma CODE_SECTION(az1gendq_calc, ".TI.ramfunc");
extern Uint32 Sector_out;

typedef struct  { float  Ualpha;            // Input: reference alpha-axis phase voltage
                  float Ubeta;          // Input: reference beta-axis phase voltage
                  float  Ta;                // Output: reference phase-a switching function
                  float  Tb;                // Output: reference phase-b switching function
                  float  Tc;                // Output: reference phase-c switching function
                  void (*calc)();       // Pointer to calculation function
                } AZ1GENDQ;

typedef AZ1GENDQ *AZ1GENDQ_handle;
/*-----------------------------------------------------------------------------
Default initalizer for the SVGENDQ object.
-----------------------------------------------------------------------------*/
#define AZ1GENDQ_DEFAULTS { 0,0,0,0,0, \
                       (void (*)(Uint32))az1gendq_calc }

/*------------------------------------------------------------------------------
Prototypes for the functions in SVGEN_DQ.C
------------------------------------------------------------------------------*/
void az1gendq_calc(AZ1GENDQ_handle);


#endif /* INCLUDE_AZ1GEN_DP_H_ */
