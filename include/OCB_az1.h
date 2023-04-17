/*
 * OCB_az1.h
 *
 *  Created on: 2023Äê4ÔÂ17ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_OCB_AZ1_H_
#define INCLUDE_OCB_AZ1_H_

#include "parameter.h"
#include "C28x_FPU_FastRTS.h"
#include "F28x_Project.h"   // Device Headerfile and Examples Include File


#pragma CODE_SECTION(ocb_az1gendq_calc, ".TI.ramfunc");
extern int bdary;
extern Uint32 Sector_out;
extern Uint32 judge_N(Uint32 N);

typedef struct  { float  Ualpha;            // Input: reference alpha-axis phase voltage
                  float Ubeta;          // Input: reference beta-axis phase voltage
                  float  Ta;                // Output: reference phase-a switching function
                  float  Tb;                // Output: reference phase-b switching function
                  float  Tc;                // Output: reference phase-c switching function
                  void (*calc)();       // Pointer to calculation function
                } OCB_AZ1GENDQ;

typedef OCB_AZ1GENDQ *OCB_AZ1GENDQ_handle;
/*-----------------------------------------------------------------------------
Default initalizer for the SVGENDQ object.
-----------------------------------------------------------------------------*/
#define OCB_AZ1GENDQ_DEFAULTS { 0,0,0,0,0, \
                       (void (*)(Uint32))ocb_az1gendq_calc }

/*------------------------------------------------------------------------------
Prototypes for the functions in SVGEN_DQ.C
------------------------------------------------------------------------------*/
void ocb_az1gendq_calc(OCB_AZ1GENDQ_handle);




#endif /* INCLUDE_OCB_AZ1_H_ */
