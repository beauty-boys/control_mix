/*
 * OCB_az1.c
 *
 *  Created on: 2023��4��17��
 *      Author: Administrator
 */
#include "OCB_az1.h"



void ocb_az1gendq_calc(OCB_AZ1GENDQ *v)
{
    float32 Va,Vb,Vc,t1,t2,t0,tup;
    Uint32 Sector = 0;  // Sector is treated as Q0 - independently with global Q

    // Inverse clarke transformation  �����ж�
        Va = v->Ubeta;                               //
        Vb = -0.5*v->Ubeta + (0.8660254)*v->Ualpha;  // 0.8660254 = sqrt(3)/2
        Vc = (-0.5)*v->Ubeta -(0.8660254)*v->Ualpha;  // 0.8660254 = sqrt(3)/2

    // 60 degree Sector determination 315462
        if (Va>0)
           Sector = 1;
        if (Vb>0)
           Sector = Sector + 2;
        if (Vc>0)
           Sector = Sector + 4;

        Va = v->Ubeta;                                                               // U1
        Vb = (0.5)*v->Ubeta + (0.8660254)*v->Ualpha;  // 0.8660254 = sqrt(3)/2       // -U3
        Vc = (0.5)*v->Ubeta - (0.8660254)*v->Ualpha;  // 0.8660254 = sqrt(3)/2       // -U2

        //����int bdary=0;���ж� 1 2 3 4 5 6
        switch(bdary)
        {
            case 1://Sectorֻ����3 2 ���˵����������
                //A
                t1 = -Vc;  //U2  T4 ����
                t2 = Va;   //U1  T6 ��0����ʱ��
                t0 = (1-t1-t2)/2; // 0����ʱ��
                t2 += t0;   //U1  T6 ��0����ʱ��

                v->Tb = t1/2;            //b��Ҫ�������Ƚ� һ����  //һ����
                v->Ta = t1/2+t2/2;       // taon = (1-t1-t2)/2
                v->Tc = t1/2+t2/2;            // tcon = tbon+t2

                break;
            case 2:
                t1 = Vb;  //-U3  T6
                t2 = Vc;  //-U2 T2
                t0 = (1-t1-t2)/2; // 0����ʱ��
                t2 += t0;   //U1  T6 ��0����ʱ��

                v->Ta = t1/2;                             // a��Ҫ�������Ƚ� һ����  //һ����
                v->Tb = t1/2+t2/2;                      // tbon = (1-t1-t2)/2
                v->Tc = t1/2+t2/2;                             // tcon = taon+t2

                break;
            case 3:
                t1 = Va;
                t2 = -Vb;
                t0 = (1-t1-t2)/2; // 0����ʱ��
                t2 += t0;   //U1  T6 ��0����ʱ��

                v->Tc = t1/2;                             // c��Ҫ�������Ƚ� һ����  //һ����
                v->Ta = t1/2+t2/2;                      // tbon = (1-t1-t2)/2
                v->Tb = t1/2+t2/2;                             // tcon = taon+t2

                break;
            case 4:
                t1 = Vc;  //-U2 T3
                t2 = -Va; //-U1 T1
                t0 = (1-t1-t2)/2; // 0����ʱ��
                t2 += t0;   //U1  T6 ��0����ʱ��

                v->Tb = t1/2;                             // b��Ҫ�������Ƚ� һ����  //һ����
                v->Ta = t1/2+t2/2;                      // tbon = (1-t1-t2)/2
                v->Tc = t1/2+t2/2;                             // tcon = taon+t2

                break;
            case 5:
                t1 = -Vb;//U3 T1
                t2 = -Vc;//U2   T5
                t0 = (1-t1-t2)/2; // 0����ʱ��
                t2 += t0;   //U1  T6 ��0����ʱ��

                v->Ta = t1/2;                             // a��Ҫ�������Ƚ� һ����  //һ����
                v->Tb = t1/2+t2/2;                      // tbon = (1-t1-t2)/2
                v->Tc = t1/2+t2/2;                             // tcon = taon+t2
                break;
            case 6:
                t1 = -Va;  //-U1 T5
                t2 = Vb;   //-U3 T4
                t0 = (1-t1-t2)/2; // 0����ʱ��
                t2 += t0;   //U1  T6 ��0����ʱ��

                v->Tc = t1/2;                             // c��Ҫ�������Ƚ� һ����  //һ����
                v->Ta = t1/2+t2/2;                      // tbon = (1-t1-t2)/2
                v->Tb = t1/2+t2/2;                             // tcon = taon+t2

            default:
                v->Ta = 0.5;
                v->Tb = 0.5;
                v->Tc = 0.5;
                break;
        }

        Sector_out =  judge_N(Sector);
}
