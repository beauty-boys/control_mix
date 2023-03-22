/*=====================================================================================
 File name:        RAMPGEN.C                 
                    
 Originator:	Digital Control Systems Group
			Texas Instruments

 Description:  The Ramp Generation                   

=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20
-------------------------------------------------------------------------------------*/

#include "C28x_FPU_FastRTS.h"
#include "rampgen.h"
#include "parameter.h"
#include "math.h"
#pragma CODE_SECTION(rampgen_calc, ".TI.ramfunc");
void rampgen_calc(RAMPGEN *v)
{	

        v->Angle += v->StepAngleMax*v->Freq;       
        
        if (v->Angle>PI)
          v->Angle -= 2*PI;
        else if (v->Angle<-PI)
          v->Angle += 2*PI;

       v->Out = v->Angle*v->Gain + v->Offset;

       if (v->Out>PI)
          v->Out -= 2*PI;
        else if (v->Out<-PI)
          v->Out += 2*PI;

}




