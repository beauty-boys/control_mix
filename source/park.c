/*=====================================================================================
 File name:        PARK.C                   
                    
 Originator:	Digital Control Systems Group
			Texas Instruments

 Description:  Park Transformation               

=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20
-------------------------------------------------------------------------------------*/

#include "C28x_FPU_FastRTS.h"
#include "park.h"


#pragma CODE_SECTION(park_calc, ".TI.ramfunc");
void park_calc(PARK *v)
{	
  
  

     v->Ds = v->Alpha*v->Cosine + v->Beta*v->Sine;
     v->Qs = v->Beta*v->Cosine - v->Alpha*v->Sine;

}


