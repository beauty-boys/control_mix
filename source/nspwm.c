/*
 * nspwm.c
 *
 *  Created on: 2023年3月22日
 *      Author: Administrator
 */

#include"nspwm.h"





void nsgendq_calc(NSGENDQ *v)
{
    float the,m;
    Uint32 Sector = 0;  // Sector is treated as Q0 - independently with global Q
    float time[3]={0};

// Inverse clarke transformation  B扇区判断
    the = atan(v->Ubeta/v->Ualpha);
    if(v->Ualpha<0)
    {
        the += PI/2;
    }
//    m=0.75;
    m = sqrt((v->Ualpha*v->Ualpha+v->Ualpha*v->Ualpha));
    Sector=judge_ns_sector(the);
    Sector_out = judge_nsN(Sector);//判断扇区N->数字
    conter_time(time,m,Sector_out,the);
    switch(Sector_out)
    {
    case 1://110-100-101-100-110 B- C+ A1
        //上下是一样的 因为t1t2有区别
        v->Tb = 1/2-time[2]/2;
        v->Tc = (time[0]+time[1])/2;
        v->Ta = -1;

        EPwm4Regs.TBPHS.bit.TBPHS = 0;        // Phase is 0
        EPwm2Regs.TBPHS.bit.TBPHS = EPWM1_TIMER_TBPRD;        // Phase is 0
        EPwm3Regs.TBPHS.bit.TBPHS = 0;        // Phase is 0

        break;
    case 2://010-110-100-110-010 A+ B- C0
        v->Ta = time[2]/2;
        v->Tb = (1-time[2]-time[1])/2;
        v->Tc = 1;

        EPwm4Regs.TBPHS.bit.TBPHS = 0;        // Phase is 0
        EPwm2Regs.TBPHS.bit.TBPHS = EPWM1_TIMER_TBPRD;        // Phase is 0
        EPwm3Regs.TBPHS.bit.TBPHS = 0;
        break;
    case 3://011-010-110-010-011 C- A+ B1
        v->Tc = 1/2-time[2]/2;
        v->Ta = (time[0]+time[1])/2;
        v->Tb = -1;

        EPwm4Regs.TBPHS.bit.TBPHS = 0;        // Phase is 0
        EPwm2Regs.TBPHS.bit.TBPHS =0 ;        // Phase is 0
        EPwm3Regs.TBPHS.bit.TBPHS = EPWM1_TIMER_TBPRD;        // Phase is 0
        break;
    case 4://001-011-010-011-001 B+ C- A0

        v->Tb = time[2]/2;
        v->Tc = (1-time[2]-time[1])/2;
        v->Ta = 1;

        EPwm4Regs.TBPHS.bit.TBPHS = 0;        // Phase is 0
        EPwm2Regs.TBPHS.bit.TBPHS = 0 ;        // Phase is 0
        EPwm3Regs.TBPHS.bit.TBPHS = EPWM1_TIMER_TBPRD;        // Phase is 0
        break;
    case 5://101-001-011-001-101 A- B+ C1

        v->Ta = 1/2-time[2]/2;
        v->Tb = (time[0]+time[1])/2;
        v->Tc = -1;

        EPwm4Regs.TBPHS.bit.TBPHS = EPWM1_TIMER_TBPRD;        // Phase is 0
        EPwm2Regs.TBPHS.bit.TBPHS = 0 ;        // Phase is 0
        EPwm3Regs.TBPHS.bit.TBPHS = 0;        // Phase is 0
        break;
    case 6://100-101-001-101-100 C+ A- B0

        v->Ta = time[2]/2;
        v->Ta = (1-time[2]-time[1])/2;
        v->Tb = 1;

        EPwm4Regs.TBPHS.bit.TBPHS = EPWM1_TIMER_TBPRD;        // Phase is 0
        EPwm2Regs.TBPHS.bit.TBPHS = 0;        // Phase is 0
        EPwm3Regs.TBPHS.bit.TBPHS = 0;        // Phase is 0
        break;
    default:
        v->Ta = 0.5;
        v->Tb = 0.5;
        v->Tc = 0.5;
        EPwm4Regs.TBPHS.bit.TBPHS = 0;        // Phase is 0
        EPwm2Regs.TBPHS.bit.TBPHS =0 ;        // Phase is 0
        EPwm3Regs.TBPHS.bit.TBPHS = 0;        // Phase is 0
        break;
    }
}


Uint32 judge_ns_sector(float theta)
{

        if(theta>-PI/6&&theta<PI/6)
        {
            return 3;
        }
        else if(theta>-PI/6&&theta<PI/6)
        {
            return 1;
        }
        else if(theta>-PI/6&&theta<PI/6)
        {
            return 5;
        }
        else if(theta>-PI/6&&theta<PI/6)
        {
            return 4;
        }
        else if(theta>-PI/6&&theta<PI/6)
        {
            return 6;
        }
        else if(theta>-PI/6&&theta<PI/6)
        {
            return 2;
        }
        else
        {
            return 0;
        }
}

Uint32 judge_nsN(Uint32 N)
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
void conter_time(float *timer,float m,Uint32 N,float theta)
{
    *timer = 1-m*sin(theta-PI/3*N+PI*2/3);
    timer++;
    *timer = -1+sqrt(3)/2*m*cos(theta-PI/3*N+PI*2/3)+3/2*m*sin(theta-PI*N/3+2*PI/3);
    timer++;
    *timer = 1-sqrt(3)/2*m*cos(theta-PI/3*N+PI*2/3)-1/2*m*sin(theta-PI*N/3+2*PI/3);
}
