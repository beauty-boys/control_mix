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
#include "angle.h"
#include "math.h"
#pragma CODE_SECTION(angle_calc, ".TI.ramfunc");


void angle_calc(Angle *v)
{	
  
  

     v->Sine = sin(v->Angle);
     v->Cosine =cos(v->Angle);

}
