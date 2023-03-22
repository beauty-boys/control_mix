/*=====================================================================================
 File name:        IPARK.C                   
                    
 Originator:	Digital Control Systems Group
			Texas Instruments

 Description:  Inverse Park Transformation               

=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20
-------------------------------------------------------------------------------------*/

#include "C28x_FPU_FastRTS.h"
#include "ipark.h"
#include "parameter.h"
#include "math.h"
#pragma CODE_SECTION(ipark_calc, ".TI.ramfunc");
void ipark_calc(IPARK *v)
{	
     v->Limit = sqrt(v->Ds* v->Ds+v->Qs*v->Qs);
     if(v->Limit<=1)
     {
        v->Alpha = v->Ds*v->Cosine - v->Qs*v->Sine;
        v->Beta = v->Qs*v->Cosine + v->Ds*v->Sine;
     }
     else
     {
	    v->Alpha = (v->Ds*v->Cosine - v->Qs*v->Sine)/v->Limit;
        v->Beta = (v->Qs*v->Cosine + v->Ds*v->Sine)/v->Limit;
     }     
}
