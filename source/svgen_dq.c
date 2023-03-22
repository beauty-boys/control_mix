
/*=====================================================================================
 File name:        SVGEN_DQ.C  (IQ version)                  
                    
 Originator:	Digital Control Systems Group
			Texas Instruments

 Description:  Space-vector PWM generation based on d-q components                    

=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20
-------------------------------------------------------------------------------------*/


// Don't forget to set a proper GLOBAL_Q in "IQmathLib.h" file

#include "C28x_FPU_FastRTS.h"
#include "svgen_dq.h"

#pragma CODE_SECTION(svgendq_calc, ".TI.ramfunc");

void svgendq_calc(SVGENDQ *v)
{	

	float32 Va,Vb,Vc,t1,t2;
	Uint32 Sector = 0;  // Sector is treated as Q0 - independently with global Q
																	
// Inverse clarke transformation  ÉÈÇøÅĞ¶Ï
    Va = v->Ubeta;
    Vb = -0.5*v->Ubeta + (0.8660254)*v->Ualpha;  // 0.8660254 = sqrt(3)/2 
    Vc = (-0.5)*v->Ubeta -(0.8660254)*v->Ualpha;  // 0.8660254 = sqrt(3)/2

// 60 degree Sector determination
    if (Va>0)
       Sector = 1; 
    if (Vb>0)
       Sector = Sector + 2;
    if (Vc>0)   
       Sector = Sector + 4;
       
// X,Y,Z (Va,Vb,Vc) calculations
    Va = v->Ubeta;                                 // X = Va U1
    Vb = (0.5)*v->Ubeta + (0.8660254)*v->Ualpha;   // Y = Vb -U3
    Vc = (0.5)*v->Ubeta - (0.8660254)*v->Ualpha;   // Z = Vc -U2
    
    if (Sector==0)  // Sector 0: this is special case for (Ualpha,Ubeta) = (0,0)
    {
       v->Ta = 0.5;
       v->Tb = 0.5;
       v->Tc = 0.5;
    }
    if (Sector==1)  // Sector 1: t1=Z and t2=Y (abc ---> Tb,Ta,Tc)
    {
       t1 = Vc;
       t2 = Vb;
       v->Tb = (0.5)*(1-t1-t2);      // tbon = (1-t1-t2)/2
       v->Ta = v->Tb+t1;                             // taon = tbon+t1
       v->Tc = v->Ta+t2;                             // tcon = taon+t2
    }
    else if (Sector==2)  // Sector 2: t1=Y and t2=-X (abc ---> Ta,Tc,Tb)
    {
       t1 = Vb;
       t2 = -Va;
       v->Ta = (0.5)*(1-t1-t2);      // taon = (1-t1-t2)/2
       v->Tc = v->Ta+t1;                             // tcon = taon+t1
       v->Tb = v->Tc+t2;                             // tbon = tcon+t2
    }      
    else if (Sector==3)  // Sector 3: t1=-Z and t2=X (abc ---> Ta,Tb,Tc)
    {
       t1 = -Vc;
       t2 = Va;
       v->Ta =(0.5)*(1-t1-t2);      // taon = (1-t1-t2)/2
       v->Tb = v->Ta+t1;                             // tbon = taon+t1
       v->Tc = v->Tb+t2;                             // tcon = tbon+t2
    }   
    else if (Sector==4)  // Sector 4: t1=-X and t2=Z (abc ---> Tc,Tb,Ta)
    {
       t1 = -Va;
       t2 = Vc;
       v->Tc =(0.5)*(1-t1-t2);      // tcon = (1-t1-t2)/2
       v->Tb = v->Tc+t1;                             // tbon = tcon+t1
       v->Ta = v->Tb+t2;                             // taon = tbon+t2
    }   
    else if (Sector==5)  // Sector 5: t1=X and t2=-Y (abc ---> Tb,Tc,Ta)
    {
       t1 = Va;
       t2 = -Vb;
       v->Tb =(0.5)*(1-t1-t2);      // tbon = (1-t1-t2)/2
       v->Tc = v->Tb+t1;                             // tcon = tbon+t1
       v->Ta = v->Tc+t2;                             // taon = tcon+t2
    }   
    else if (Sector==6)  // Sector 6: t1=-Y and t2=-Z (abc ---> Tc,Ta,Tb)
    {
       t1 = -Vb;
       t2 = -Vc;
       v->Tc =(0.5)*(1-t1-t2);      // tcon = (1-t1-t2)/2
       v->Ta = v->Tc+t1;                             // taon = tcon+t1
       v->Tb = v->Ta+t2;                             // tbon = taon+t2 
    }
    Sector_out = judge_N(Sector);
}


Uint32 judge_N(Uint32 N)
{
    switch(N)
    {
    case 3:
        return 1;
    case 1:
        return 2;
    case 5:
        return 3;
    case 4:
        return 4;
    case 6:
        return 5;
    case 2:
        return 6;
    default:
        return 0;
    }
}


