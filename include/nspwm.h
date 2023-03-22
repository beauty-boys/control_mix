/*
 * nspwm.h
 *
 *  Created on: 2023Äê3ÔÂ22ÈÕ
 *      Author: Administrator
 */

#ifndef INCLUDE_NSPWM_H_
#define INCLUDE_NSPWM_H_

#include "parameter.h"
#include "C28x_FPU_FastRTS.h"
#include "F28x_Project.h"
#include <math.h>


#pragma CODE_SECTION(nsgendq_calc, ".TI.ramfunc");

extern Uint32 Sector_out;
extern Uint32 judge_N(Uint32 N);


typedef struct  { float  Ualpha;            // Input: reference alpha-axis phase voltage
                  float Ubeta;          // Input: reference beta-axis phase voltage
                  float  Ta;                // Output: reference phase-a switching function
                  float  Tb;                // Output: reference phase-b switching function
                  float  Tc;                // Output: reference phase-c switching function
                  void (*calc)();       // Pointer to calculation function
                } NSGENDQ;

typedef NSGENDQ *NSGENDQ_handle;
/*-----------------------------------------------------------------------------
Default initalizer for the SVGENDQ object.
-----------------------------------------------------------------------------*/
#define NSGENDQ_DEFAULTS { 0,0,0,0,0, \
                       (void (*)(Uint32))nsgendq_calc }

/*------------------------------------------------------------------------------
Prototypes for the functions in SVGEN_DQ.C
------------------------------------------------------------------------------*/
void nsgendq_calc(NSGENDQ_handle);


Uint32 judge_ns_sector(float theta);

void conter_time(float *timer,float m,Uint32 N,float theta);

#endif /* INCLUDE_NSPWM_H_ */
