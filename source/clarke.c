/*=====================================================================================
 File name:        CLARKE.C                   
                    
 Originator:	Digital Control Systems Group
			Texas Instruments

 Description:  The Clarke Transformation                 

=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20
-------------------------------------------------------------------------------------*/

#include "C28x_FPU_FastRTS.h"
#include "clarke.h"
#pragma CODE_SECTION(clarke_calc, ".TI.ramfunc");
void clarke_calc(CLARKE *v)
{	
   v->Alpha = 0.33333333*(2*v->As-v->Bs-v->Cs);

   v->Beta = (v->Bs - v->Cs)*0.57735027;  // 1/sqrt(3) = 0.57735026918963
}                            


