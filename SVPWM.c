//###########################################################################
//
// 程序说明：
// 韩守亮轴承电流测试电机SVPWM控制程序
//编写人：杨明亮
//最后更改日期：2023——2——7
//最后使用日期：2023_3_18      内容：控制轴电流电机
//更改内容：将A相上下管改为PWM4AB控制，将XA1改为GPIO167控制。
/********************************************
 //*************** 修改记录*****************
//最后一次使用：2_7_驱动韩守亮未改造电机，驱动正常（之前的问题是正方向和cos+-反了，目前驱动的零位，电机是顺时针旋转的）
// 2_16_加过流保护、测试50kHz，母线可达210V

**********************************************/
// Included Files
//

#include "F28x_Project.h"   // Device Headerfile and Examples Include File
#include "bianliang.h"
#include "C28x_FPU_FastRTS.h"   // FastRTS
#include "SV_335.h"
#include "math.h"
#include "parameter.h"
#include "stdlib.h"



//YML_2022_11_5   韩守亮电机零位：1035（没改造电机）

//#pragma CODE_SECTION(epwm1_isr, ".TI.ramfunc");
//#pragma CODE_SECTION(EXADREAD, ".TI.ramfunc");
//#pragma CODE_SECTION(SAMPLEPROCESS, ".TI.ramfunc");
//#pragma CODE_SECTION(setupMessageObject, ".TI.ramfunc");
//#pragma CODE_SECTION(getCANMessage, ".TI.ramfunc");
//#pragma CODE_SECTION(setCANBitRate, ".TI.ramfunc");
//#pragma CODE_SECTION(scic_xmit, ".TI.ramfunc");
//#pragma CODE_SECTION(scic_msg, ".TI.ramfunc");
//#pragma CODE_SECTION(sendCANMessage, ".TI.ramfunc");
//#pragma CODE_SECTION(jieshou, ".TI.ramfunc");



//#define FREQUENCY 200000000/EPWM1_TIMER_TBPRD/2  这个不能用，这个数据可能溢出了，算的时候算不对
//extern Uint16 RamfuncsLoadStart;
//extern Uint16 RamfuncsLoadSize;
//extern Uint16 RamfuncsRunStart;


int16 DAHALF=0x0800;
int flagggg=0;

int flag_mix=0;//混合调制标志位

struct EXAD EXADRESULTS;

struct SAMPLE SAMPLERUSELTS;

struct LOCRES    //YML__2021_1_16
{
    Uint16 LOT0;
    Uint16 DOS0;
    Uint16 DOS1;
    Uint16 DOS2;
    Uint16 DOS3;
    Uint16 LOT1;
    Uint16 LOT2;
    Uint16 FREQ;
    Uint16 CONTROL;
    Uint16 FAULT;
};
struct LOCRES AD2S;  //YML__2021_1_16
float32 IdRef = 0;                // Id reference (pu)
float32 IqRef = 0;              // Iq reference (pu)

float32 T = 0.0001;  // Samping period (sec), see parameter.h
float32 iqtest=0;
Uint16 i,j,k;
Uint16 iss=0;
Uint16 sendChar = 0x02;//,i,j,k;

Uint16 Error_Flag=0;//YML_2023_2_11
Uint16 Over_Current_Flag=0;//YML_2023_2_11
Uint16 Clear_Error_Flag=0;//
float32 IA=0;
float32 IB=0;
float32 IC=0;

char *msg6;
Uint32 bianma=0;
Uint32 bianma1=0;
int16 XXX=0;//YML//测试，用后可删除
int16 AngleInit=0;//编码器变量，2^23
int32 Anglezero=3078;//韩守亮没改造电机：3063（旋变2对极，电机4对）__YML_2023_2_7拉零;
//韩守亮轴承电流测试电机：零位：655（旋变2对极，电机4对）；
float32 temp;
float32 temp1;
float32 AngleReal;
Uint16 SpeedLoopPrescaler = 20;      // Speed loop prescaler
Uint16 SpeedLoopCount = 0;           // Speed loop counter
Uint16 zhouqi = 0;
float32 Speedrpm = 0;
float32 SpeedRef = 0;           // Speed reference (pu)
float32 AngleStore[2] = {0,0};
float32 OmegaStore[2] = {0,0};
float32 OmegaM = 0;
float32 idcan = 0;
float32 idref = 0;
float32 iqref = 0;
float32 iqref1 = 0;
float32 idref1 = 0;
float32 idcan1 = 0;
float32 iqcan1 = 0;
float32 Duty_YML=0.2;//YML_2022_10_25
float32 Phase_Shift_Angle=30;
float32 Udc_1_6_Half=0.2;
Uint16 PPnum=2; //比值：电机极对数/旋变极对数.哈飞电机：电机8对极，旋变4对极。
float32 Vd_Test_openloop=0; //开环用Ud、Uq标幺值
float32 Vq_Test_openloop=0.4;
float32 iq_Test=0.03;//电流闭环用iq标幺值
float32 signal1[Sample_points];
Uint32 Sector_out=0;
float32 KP_SPEED=1;
float32 KI_SPEED=0.0001;
float32 OUT_SPEED=0.3;

float32 KP_ID=1;
float32 KI_ID=0.0001;
float32 OUT_ID=0.8;
float32 KP_IQ=1;
float32 KI_IQ=0.0001;
float32 OUT_IQ=0.8;

float32 Udc_use=0;
float32 Udc_temp=0;

Uint16 AD_Sample_Counter=0;

float32 Iq_REF_X;
float32 Iq_FDB_X;
float32 Id_FDB_X;
float32 Ud_REF_X;
float32 Uq_REF_X;
float32 Ualpha_X;
float32 UBeta_X;
float32 SPEED_FDB_X;

#define CAN_MSG_ID              0x1111 // This example only supports standard ID  // 0111ID 标准 数据
#define CAN_TX_MSG_OBJ          1
#define CAN_RX_MSG_OBJ          2
#define CAN_MAX_BIT_DIVISOR     (13)   // The maximum CAN bit timing divisor
#define CAN_MIN_BIT_DIVISOR     (5)    // The minimum CAN bit timing divisor
#define CAN_MAX_PRE_DIVISOR     (1024) // The maximum CAN pre-divisor
#define CAN_MIN_PRE_DIVISOR     (1)    // The minimum CAN pre-divisor
#define CAN_BTR_BRP_M           (0x3F)
#define CAN_BTR_BRPE_M          (0xF0000)
#define CAN_MSG_ID_SHIFT        18U


//
// Globals
//
unsigned char ucTXMsgData[8] = {0,0,0,0,0,0,0,0}; // TX Data
unsigned char ucRXMsgData[8] = {0,0,0,0,0,0,0,0};         // RX Data
uint32_t messageSize = sizeof(ucRXMsgData);          // Message Size (DLC)
volatile unsigned long msgCount = 0; // A counter that keeps track of the
                                     // number of times the transmit was
                                     // successful.
volatile unsigned long errFlag = 0;  // A flag to indicate that some
                                     // transmission error occurred.

static const uint16_t canBitValues[] =
{
    0x1100, // TSEG2 2, TSEG1 2, SJW 1, Divide 5
    0x1200, // TSEG2 2, TSEG1 3, SJW 1, Divide 6
    0x2240, // TSEG2 3, TSEG1 3, SJW 2, Divide 7
    0x2340, // TSEG2 3, TSEG1 4, SJW 2, Divide 8
    0x3340, // TSEG2 4, TSEG1 4, SJW 2, Divide 9
    0x3440, // TSEG2 4, TSEG1 5, SJW 2, Divide 10
    0x3540, // TSEG2 4, TSEG1 6, SJW 2, Divide 11
    0x3640, // TSEG2 4, TSEG1 7, SJW 2, Divide 12
    0x3740  // TSEG2 4, TSEG1 8, SJW 2, Divide 13
};

typedef enum
{
        //! Transmit message object.
        MSG_OBJ_TYPE_TRANSMIT,

        //! Receive message object.
        MSG_OBJ_TYPE_RECEIVE
}
msgObjType;

//
// Function Prototypes
//
uint32_t setCANBitRate(uint32_t sourceClock, uint32_t bitRate);
void  setupMessageObject(uint32_t objID, uint32_t msgID, msgObjType msgType);
void sendCANMessage(uint32_t objID);
bool getCANMessage(uint32_t objID);
// Prototype statements for functions found within this file.
void scic_echoback_init(void);
void scic_fifo_init(void);
void scic_xmit(int a);
void scic_msg(char *msg);
void Initemif1();//先于GPIO和其他模块声明
extern void setup_emif1_pinmux_async_16bit(Uint16);
void Initad7606(void);
void Init_AD2S1210(void);  //defined by YML
void Initad5725(void);
void InitadIO(void);
void Initshuju(void);
void SAMPLEPROCESS(void);
void EXADREAD(void);
void InitEPwmExample(void);
void angleREAD(void);
void jieshou(void);
__interrupt void epwm4_isr(void);

// Instance a few transform objects
CLARKE clarke1 = CLARKE_DEFAULTS;
PARK park1 = PARK_DEFAULTS;
IPARK ipark1 = IPARK_DEFAULTS;
Angle angle1 = ANGLE_DEFAULTS;
// Instance PID regulators to regulate the d and q synchronous axis currents,
// and speed        实例PID调节器，用于调节D和Q同步轴电流，和速度
PIDREG3 pid1_id = PIDREG3_DEFAULTS;
PIDREG3 pid1_iq = PIDREG3_DEFAULTS;
PIDREG3 pid1_spd = PIDREG3_DEFAULTS;
// Instance a Space Vector PWM modulator. This modulator generates a, b and c  实例一个空间矢量pwm调制器。
// phases based on the d and q stationery reference frame inputs 该调制器基于d和q信纸参考帧输入产生a、b和c相位

SVGENDQ svgen_dq1 = SVGENDQ_DEFAULTS;
AZ1GENDQ az1gen_dq1 = AZ1GENDQ_DEFAULTS;
NSGENDQ nsgen_dq1 = NSGENDQ_DEFAULTS;
float m=0;


// Instance a ramp generator to simulate an Anglele  使用渐变生成器来模拟角度
RAMPGEN rg1 = RAMPGEN_DEFAULTS;

//
// Main
//
void main(void)
{
//
//	memcpy(&RamfuncsRunStart,&RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the F2837xD_SysCtrl.c file.
//
    InitSysCtrl(); //这里已经把所有外设时钟都打开了
//
// Step 2. Initialize GPIO:
// This example function is found in the F2837xD_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
//
     InitGpio();

//

//
   Initemif1();//EMIF初始化
    Initad7606();
    Init_AD2S1210();  //defined by YML
    Initad5725();
//    InitadIO();

    //AD7606B测试专用
        //  *(AD7606) = 0x0515;  //YML//写数据FF,写数据只需要一条语句。
         // *(AD7606) = 0x8500;  //YML//读数据需要两条语句
        //  EXADRESULTS.ADV1= *(AD7606);
        //  *(AD7606) = 0x0311;  //YML//写0000 0011 0001 0001  通道1和2设置为正负5V
        //  *(AD7606) = 0x8300;  //YML//读数据需要两条语句
        //  EXADRESULTS.ADV1= *(AD7606);
        // *(AD7606) = 0x0411;  //YML//写0000 0100 0001 0001  通道3和4设置为正负5V
        //  *(AD7606) = 0x8400;  //YML//读数据需要两条语句
       //  EXADRESULTS.ADV2= *(AD7606);
       //   *(AD7606) = 0x0521;  //YML//写0000 0100 0010 0001  通道5设置为正负5V，6设置为正负10V
       //   *(AD7606) = 0x8500;  //YML//读数据需要两条语句
       ////   EXADRESULTS.ADV3= *(AD7606);
        //  *(AD7606) = 0x0622;  //YML//写0000 0100 0010 0010  通道7设置为正负5V，8设置为正负10V
        //  *(AD7606) = 0x8600;  //YML//读数据需要两条语句
        //   EXADRESULTS.ADV4= *(AD7606);

      InitEPwm1Gpio();//板子设计的是非默认选择，要改.c文件的函数
      InitEPwm2Gpio();
      InitEPwm3Gpio();
      InitEPwm4Gpio();
//      Initshuju();
//
// For this case just init GPIO pins for ePWM1, ePWM2, ePWM3
// These functions are in the F2837xD_EPwm.c file
//      GPIO_SetupPinMux(18, GPIO_MUX_CPU1, 3);  //GPIO18 - CANRXA
//       GPIO_SetupPinMux(19, GPIO_MUX_CPU1, 3);  //GPIO19 - CANTXA
//       GPIO_SetupPinOptions(18, GPIO_INPUT, GPIO_ASYNC);
//       GPIO_SetupPinOptions(19, GPIO_OUTPUT, GPIO_PUSHPULL);
//      GPIO_SetupPinMux(107, GPIO_MUX_CPU1, 6);
//        GPIO_SetupPinOptions(107, GPIO_INPUT, GPIO_PUSHPULL);
//        GPIO_SetupPinMux(106, GPIO_MUX_CPU1, 6);
//       GPIO_SetupPinOptions(106, GPIO_OUTPUT, GPIO_PUSHPULL);

      scic_fifo_init();      // Initialize the SCI FIFO
      scic_echoback_init();  // Initialize SCI for echoback
        //
       // Initialize the CAN-A controller
       //
       InitCAN();

       //
       // Setup CAN to be clocked off the SYSCLKOUT
       //
       ClkCfgRegs.CLKSRCCTL2.bit.CANABCLKSEL = 0;

       //
       // Set up the bit rate for the CAN bus.  This function sets up the CAN
       // bus timing for a nominal configuration.
       // In this example, the CAN bus is set to 500 kHz.
       //
       // Consult the data sheet for more information about
       // CAN peripheral clocking.
       //
       uint32_t status = setCANBitRate(200000000, 500000);   //设置波特率



       //
       // Step 3. Clear all interrupts and initialize PIE vector table:
       // Disable CPU interrupts
       //
// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
//
    DINT;

//
// Initialize the PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the F2837xD_PieCtrl.c file.
//
    InitPieCtrl();

//
// Disable CPU interrupts and clear all CPU interrupt flags:
//
    IER = 0x0000;
    IFR = 0x0000;

//
// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in F2837xD_DefaultIsr.c.
// This function is found in F2837xD_PieVect.c.
//
    InitPieVectTable();

    // Initialize the message object that will be used for sending CAN
     // messages.
     //
     setupMessageObject(CAN_TX_MSG_OBJ, 0x0061, MSG_OBJ_TYPE_TRANSMIT); //发用OBJ1，中间是过滤id，后面是收发方向控制

     //
     // Initialize the message object that will be used for receiving CAN
     // messages.
     //
     setupMessageObject(CAN_RX_MSG_OBJ, 0x0041, MSG_OBJ_TYPE_RECEIVE); //收速度用 OBJ2 左移18位，0X1111相当于0X111  彩色485线对应0041，蓝色485线对应0031
   //  setupMessageObject(3, 0X0002, MSG_OBJ_TYPE_RECEIVE);//收id 0x0001
   //  setupMessageObject(4, 0X0003, MSG_OBJ_TYPE_RECEIVE);//收iq 0x0011

     //
     // Enable the CAN for operation.
     //
     CanaRegs.CAN_CTL.bit.Init = 0;
//
// Interrupts that are used in this example are re-mapped to
// ISR functions found within this file.
//
    EALLOW; // This is needed to write to EALLOW protected registers
    PieVectTable.EPWM4_INT = &epwm4_isr;

    EDIS;   // This is needed to disable write to EALLOW protected registers
    // Initialize the PID_REG3 module for speed
    pid1_spd.Kp = KP_SPEED;//0.8;
    pid1_spd.Ki = KI_SPEED;//0.0005;
    pid1_spd.Kd =0 ;
    pid1_spd.Kc =0.01 ;
      pid1_spd.OutMax =OUT_SPEED;
      pid1_spd.OutMin = -OUT_SPEED;

    // Initialize the PID_REG3 module for Id
    	pid1_id.Kp =KP_ID;//0.2
    	pid1_id.Ki = KI_ID;
    	pid1_id.Kd =0 ;
    	pid1_id.Kc =0.01 ;
        pid1_id.OutMax = OUT_ID;
        pid1_id.OutMin = -OUT_ID;


    // Initialize the PID_REG3 module for Iq
    	pid1_iq.Kp = KP_IQ;//0.2
    	pid1_iq.Ki = KI_IQ;
    	pid1_iq.Kd = 0;
    	pid1_iq.Kc = 0.01;
        pid1_iq.OutMax = OUT_IQ;
        pid1_iq.OutMin = -OUT_IQ;




//
// For this example, only initialize the ePWM
//
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;//停止TBCLK
    ClkCfgRegs.PERCLKDIVSEL.bit.EPWMCLKDIV=0;//2分频PLLSYSCLK 0不分频，1 2分频
    EDIS;

    InitEPwmExample();//最后设置EPWM模块，设置完直接开始等待了
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;//开启TBCLK
    EDIS;

//
// Step 4. User specific code, enable interrupts:
//
// Enable CPU INT3 which is connected to EPWM1-3 INT:
//
    IER |= M_INT3;

//
// Enable EPWM INTn in the PIE: Group 3 interrupt 1-3
//
    PieCtrlRegs.PIEIER3.bit.INTx4 = 1;
//
//    SeizeFlashPump();
//       FlashOff();
//       ReleaseFlashPump();
//  STANDBY();
// Enable global Interrupts and higher priority real-time debug events:
//
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

//
// Step 5. IDLE loop. Just sit and loop forever (optional):


    for(;;)
    {
//      jieshou();

        Udc_temp=(pid1_id.Out*pid1_id.Out+pid1_iq.Out*pid1_iq.Out);
        Udc_use=sqrt(Udc_temp);

    IA=SAMPLERUSELTS.IA;
    IB=SAMPLERUSELTS.IB;
    IC=SAMPLERUSELTS.IC;

    if(IA>0.3||IB>0.3||IC>0.3)//按照100欧姆采样电阻计算，I1=I实际/200; 0.2=40A
    {
        DELAY_US(1000);
        if(IA>0.3||IB>0.3||IC>0.3)
        Over_Current_Flag=1;
    }


    if(Over_Current_Flag==1)
        {

         SPEED_FDB_X= pid1_spd.Fdb;
         Iq_REF_X= pid1_iq.Ref;
         Iq_FDB_X=pid1_iq.Fdb;
         Id_FDB_X= pid1_id.Fdb;
         Ud_REF_X= ipark1.Ds;
         Uq_REF_X= ipark1.Qs;
         Ualpha_X=svgen_dq1.Ualpha;
         UBeta_X=svgen_dq1.Ubeta;

                  EALLOW;//必须要加
                  EPwm4Regs.TZCTL.bit.TZA=2;   //0Z 1H  2L  3CMPACMPB
                  EPwm4Regs.TZCTL.bit.TZB=2;
                  EPwm2Regs.TZCTL.bit.TZA=2;
                  EPwm2Regs.TZCTL.bit.TZB=2;
                  EPwm3Regs.TZCTL.bit.TZA=2;
                  EPwm3Regs.TZCTL.bit.TZB=2;

                  EPwm4Regs.TZFRC.bit.OST=1;
                  EPwm2Regs.TZFRC.bit.OST=1;
                  EPwm3Regs.TZFRC.bit.OST=1;
                  EDIS;
                  Over_Current_Flag=0;
                  Error_Flag=1;//故障标志为，只有重新烧程序才能清除，才能重新使能PWM
        }

    if(Clear_Error_Flag==1)
            {
                      EALLOW;//必须要加
                      EPwm4Regs.TZCTL.bit.TZA=3;   //0Z 1H  2L  3CMPACMPB
                      EPwm4Regs.TZCTL.bit.TZB=3;
                      EPwm2Regs.TZCTL.bit.TZA=3;
                      EPwm2Regs.TZCTL.bit.TZB=3;
                      EPwm3Regs.TZCTL.bit.TZA=3;
                      EPwm3Regs.TZCTL.bit.TZB=3;
                      EDIS;

                      Error_Flag=0;//故障标志为，只有重新烧程序才能清除，才能重新使能PWM
                      Clear_Error_Flag=0;//把自己清零
                      Over_Current_Flag=0;
            }

    if(Speedrpm>0.8)      //转速控防止飞车
        SpeedRef=0.6;//参考转速限定到2000r

             //            idref1 = 0;//作为负载电机防飞车
             //            iqref1 = 0;
             //            idref = 0;
             //            iqref = 0;



     }
}


//
// epwm1_isr - EPWM1 ISR to update compare values
//
__interrupt void epwm4_isr(void)
{
    //   GpioDataRegs.GPEDAT.bit.GPIO146=0;
        EPwm4Regs.ETCLR.bit.INT = 1;
        // Acknowledge this interrupt to receive more interrupts from group 3
        PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
       // asm (" ESTOP0");

//        if(iqref>0.08)  //扭矩控制专用的
//        {
//            iqref1=0.08;
//        }
//        else if(iqref<-0.08)
//        {
//            iqref1=-0.08;
//        }
//        else
//            iqref1=iqref;
//        if(idref>0.08)
//        {
//            idref1=0.08;
//        }
//        else if(idref<-0.08)
//        {
//            idref1=-0.08;
//        }
//        else
//            idref1=idref;

       // while (GpioDataRegs.GPCDAT.bit.GPIO68==1) //AD的BUSY引脚
        //asm (" ESTOP0");
       // asm (" NOP"); //重要

        for(AD_Sample_Counter=0;AD_Sample_Counter<9;AD_Sample_Counter++)
        {
        }

        EXADREAD();//AD采样信号读回来
        SAMPLEPROCESS();//折算成IA IB IC UDC数字量


        angleREAD();  //读初始位置  SIC伺服电机为5对极
        AngleStore[0] = AngleReal;//电角度(-PI,PI)
        temp1 = AngleStore[0]-AngleStore[1];//YML//当前的角度减去过去一个时刻的角度
           if(temp1<-4)//5对极，76000r/min=4   3000r/min=0.157 画个区间图就明显了 防止区间边界角度跳变
           {
              temp1 += 2*PI;
           }
            else if(temp1>4)
            {
              temp1 -= 2*PI;
            }
            //速度计算
           OmegaStore[0] =100000000/EPWM1_TIMER_TBPRD*temp1/4/2000;//w rad/s ：temp*f/p/nb p=4 nb=2000 f = 10000 ，，f=20k就需要乘一倍。。。    标幺值2000r/min 在这标幺和在后面标幺一样
            OmegaM = 0.01*OmegaStore[0] + 0.99*OmegaStore[1];  //滤波系数可再做修改
            OmegaStore[1] = OmegaM;
          // OmegaM=100000000/EPWM1_TIMER_TBPRD*temp1/4/2000;
            Speedrpm = OmegaM*30/PI;// n=60*w/2/PI (rpm)
            AngleStore[1] = AngleStore[0];

           //Speed loop function
            pid1_spd.Ref = SpeedRef; //标幺值2000RPM，SpeedRef=0.1是200RPM
            if(SpeedLoopCount<SpeedLoopPrescaler)
                SpeedLoopCount++;
              else
              {
                  SpeedLoopCount = 0;

               pid1_spd.Fdb =Speedrpm;
               pid1_spd.calc(&pid1_spd);
              }


            /****************拉零*/
//
//             EPwm4Regs.TZSEL.bit.OSHT1 = 1;
//
//             EPwm2Regs.CMPA.bit.CMPA = (int16)(0.5*EPWM1_TIMER_TBPRD);
//             EALLOW;
//             EPwm4Regs.TZFRC.bit.OST = 1;
//             EPwm3Regs.TZFRC.bit.OST = 1;
//             EDIS;



//            rampgen_calc(&rg1);//YML//开环专用！！    前面加上定义的HF=50，，，然后rg1.Fre=HF*0.001;//这是角度斜坡的变化斜率，最后只需要改变Ud,Uq=0即可。其余电流都不用动。
//            angle1.Angle = rg1.Angle;
              angle1.Angle =AngleReal;//闭环需要打开
//           for(i=0;i<Sample_points;i++)
//           {
//               signal1[i]=angle1.Angle;
//           }

            angle1.calc(&angle1);

            clarke1.As = SAMPLERUSELTS.IA;
            clarke1.Bs = SAMPLERUSELTS.IB;
            clarke1.Cs = SAMPLERUSELTS.IC;
            clarke1.calc(&clarke1);
            // ------------------------------------------------------------------------------
            //    Connect inputs of the PARK module and call the park transformation
            //    calculation function.
            // ------------------------------------------------------------------------------
            park1.Alpha = clarke1.Alpha;
            park1.Beta = clarke1.Beta;
            park1.Sine = angle1.Sine;
            park1.Cosine = angle1.Cosine;
            park1.calc(&park1);
            // ------------------------------------------------------------------------------
            //    Connect inputs of the PIDREG3 module and call the pid loopback
            //    calculation function.
            // ------------------------------------------------------------------------------
            pid1_id.Ref =0;
            pid1_id.Fdb =park1.Ds;
            pid1_id.calc(&pid1_id);//输出为Ud

            // ------------------------------------------------------------------------------
            //    Connect inputs of the PIDREG3 module and call the pid loopback
            //    calculation function.
            // ------------------------------------------------------------------------------
            //pid1_iq.Ref =0.01;  //去掉转速环
            pid1_iq.Ref = pid1_spd.Out;
            pid1_iq.Fdb = park1.Qs;
            pid1_iq.calc(&pid1_iq);


           ipark1.Ds =pid1_id.Out;//Vd_Test_openloop;//pid1_id.Out;
          ipark1.Qs =pid1_iq.Out;//Vq_Test_openloop;//pid1_iq.Out;

            //ipark1.Ds =Vd_Test_openloop;//pid1_id.Out;
            //ipark1.Qs =Vq_Test_openloop;//pid1_iq.Out;


            ipark1.Sine = angle1.Sine;
            ipark1.Cosine = angle1.Cosine;
            ipark1.calc(&ipark1);

            // ------------------------------------------------------------------------------
            //    Connect inputs of the SVGEN_DQ module and call the space-vector gen.
            //    calculation function.
            // ------------------------------------------------------------------------------
            svgen_dq1.Ualpha = ipark1.Alpha;
            svgen_dq1.Ubeta = ipark1.Beta;
            svgen_dq1.calc(&svgen_dq1);

//                            EPwm4Regs.CMPA.bit.CMPA  = (int16)(az1gen_dq1.Ta*EPWM1_TIMER_TBPRD);
//                            EPwm2Regs.CMPA.bit.CMPA = (int16)(az1gen_dq1.Tb*EPWM1_TIMER_TBPRD);
//                            EPwm3Regs.CMPA.bit.CMPA = (int16)(az1gen_dq1.Tc*EPWM1_TIMER_TBPRD);

            m = sqrt(ipark1.Alpha*ipark1.Alpha+ipark1.Beta*ipark1.Beta);
            switch(flag_mix)
            {
            case 0://从AZ1->NS 大调制度 //防止频繁调回 高调制度防调回
                if(m<Change_Point&&m>=0)//az1 先
                    {
                        if(AngleReal/60> Angle_pianyi && AngleReal/60<(60-Angle_pianyi))//角度制
                        {
                            az1gen_dq1.Ualpha = ipark1.Alpha;
                            az1gen_dq1.Ubeta = ipark1.Beta;
                            az1gen_dq1.calc(&az1gen_dq1);

                            EPwm4Regs.CMPA.bit.CMPA  = (int16)(az1gen_dq1.Ta*EPWM1_TIMER_TBPRD);
                            EPwm2Regs.CMPA.bit.CMPA = (int16)(az1gen_dq1.Tb*EPWM1_TIMER_TBPRD);
                            EPwm3Regs.CMPA.bit.CMPA = (int16)(az1gen_dq1.Tc*EPWM1_TIMER_TBPRD);
                            flagggg=0;
                        }
                        else
                        {
                            ;
                        }

                    }
                    else   //切换后 ns
                    {
                        nsgen_dq1.Ualpha = ipark1.Alpha;
                        nsgen_dq1.Ubeta = ipark1.Beta;
                        nsgen_dq1.calc(&nsgen_dq1);

                        EPwm4Regs.CMPA.bit.CMPA  = (int16)(nsgen_dq1.Ta*EPWM1_TIMER_TBPRD);
                        EPwm2Regs.CMPA.bit.CMPA = (int16)(nsgen_dq1.Tb*EPWM1_TIMER_TBPRD);
                        EPwm3Regs.CMPA.bit.CMPA = (int16)(nsgen_dq1.Tc*EPWM1_TIMER_TBPRD);
                        flagggg=1;
                        flag_mix=1;//达到设定调制度 需要稳定运行
                    }
                break;
            case 1://防止频繁调回 低调制度防调回
                if(m<Return_Point&&m>=0)
                    {
                        if(AngleReal/60> Angle_pianyi && AngleReal/60<(60-Angle_pianyi))//角度制
                        {
                            az1gen_dq1.Ualpha = ipark1.Alpha;
                            az1gen_dq1.Ubeta = ipark1.Beta;
                            az1gen_dq1.calc(&az1gen_dq1);

                            EPwm4Regs.CMPA.bit.CMPA  = (int16)(az1gen_dq1.Ta*EPWM1_TIMER_TBPRD);
                            EPwm2Regs.CMPA.bit.CMPA = (int16)(az1gen_dq1.Tb*EPWM1_TIMER_TBPRD);
                            EPwm3Regs.CMPA.bit.CMPA = (int16)(az1gen_dq1.Tc*EPWM1_TIMER_TBPRD);
                            flagggg=0;
                            flag_mix=0;//达到设定调制度 需要稳定运行
                        }
                        else
                        {
                            ;
                        }
                    }
                    else
                    {
                        nsgen_dq1.Ualpha = ipark1.Alpha;
                        nsgen_dq1.Ubeta = ipark1.Beta;
                        nsgen_dq1.calc(&nsgen_dq1);

                        EPwm4Regs.CMPA.bit.CMPA  = (int16)(nsgen_dq1.Ta*EPWM1_TIMER_TBPRD);
                        EPwm2Regs.CMPA.bit.CMPA = (int16)(nsgen_dq1.Tb*EPWM1_TIMER_TBPRD);
                        EPwm3Regs.CMPA.bit.CMPA = (int16)(nsgen_dq1.Tc*EPWM1_TIMER_TBPRD);
                        flagggg=1;
                    }
                break;

            }


//            EPwm4Regs.CMPA.bit.CMPA  = (int16)(svgen_dq1.Ta*EPWM1_TIMER_TBPRD);
//            EPwm2Regs.CMPA.bit.CMPA = (int16)(svgen_dq1.Tb*EPWM1_TIMER_TBPRD);
//            EPwm3Regs.CMPA.bit.CMPA = (int16)(svgen_dq1.Tc*EPWM1_TIMER_TBPRD);


//          EPwm4Regs.CMPA.bit.CMPA  = (int16)(0.5*EPWM1_TIMER_TBPRD);    //拉0用
//          EPwm2Regs.CMPA.bit.CMPA = (int16)(Duty_YML*EPWM1_TIMER_TBPRD);
//          EPwm3Regs.CMPA.bit.CMPA = (int16)(0.7*EPWM1_TIMER_TBPRD);
//
            *(DAA)    = (int16)(flagggg*DAHALF/10+DAHALF);
            //*(DAA)    = (int16)(Sector_out*DAHALF/6);
//            *(DAB)    = (int16)(svgen_dq1.Tb*EPWM1_TIMER_TBPRD);//(int16)(park1.Ds*DAHALF)+DAHALF;
//         // *(DAC)    = (int16)(svgen_dq1.Tc*EPWM1_TIMER_TBPRD);
//            *(DAC)    = (int16)(rg1.Angle*DAHALF/5)+DAHALF;
}

void angleREAD(void)
    {



/*
             while(ScicRegs.SCIFFRX.bit.RXFFST !=6 ) { } // wait for XRDY =1 for empty state

                         // Get character
                         msg6[0] =ScicRegs.SCIRXBUF.all;
                         msg6[1] =ScicRegs.SCIRXBUF.all;
                         msg6[2] =ScicRegs.SCIRXBUF.all;
                         msg6[3] =ScicRegs.SCIRXBUF.all;
                         msg6[4] =ScicRegs.SCIRXBUF.all;
                         msg6[5] =ScicRegs.SCIRXBUF.all;
                         i=msg6[2];
                                 j=          msg6[3];
                                         k=      msg6[4];

              bianma=i+256*j+65536*k;

            bianma1=bianma>>9;//右移9位，23位转14位
              AngleInit = (int32) bianma1;

  */
      AngleInit = *(AD2S1210)>>4;
      AngleInit = AngleInit-Anglezero;
      temp1 = ((float32)AngleInit/2048.0*PI)*PPnum+PI/2;//pi/2是定位的初始角度，随着定位方式不同改变
      while(temp1<-PI)
      {
         temp1+=2*PI;
      }
      while(temp1>PI)
      {
         temp1-=2*PI;
      }
      AngleReal = temp1;

    }

//
// InitEPwm1Example - Initialize EPWM1 values
//
void InitEPwmExample()
{
    GPIO_SetupPinMux(128,0,0);//配置成CPU1 GPIO
    GPIO_SetupPinOptions(128,1,1);//配置成输出上拉
    GpioDataRegs.GPEDAT.bit.GPIO128=0;   //低
    GPIO_SetupPinMux(53,0,0);//配置成CPU1 GPIO
	GPIO_SetupPinOptions(53,1,1);//配置成输出 上拉 复位引脚
	GpioDataRegs.GPBDAT.bit.GPIO53=1;

   // Setup TBCLK
   EPwm1Regs.TBCTL.bit.CTRMODE =TB_COUNT_UPDOWN; // Count up
   EPwm1Regs.TBPRD = EPWM1_TIMER_TBPRD;       // Set timer period
   EPwm1Regs.TBCTL.bit.PHSEN = TB_ENABLE;    // Disable phase loading
   EPwm1Regs.TBPHS.bit.TBPHS = 0x0000;        // Phase is 0
   EPwm1Regs.TBCTL.bit.PHSDIR=1;
   EPwm1Regs.TBCTR = 0x0000;                  // Clear counter
   EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   // Clock ratio to SYSCLKOUT
   EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

      EPwm2Regs.TBCTL.bit.CTRMODE =TB_COUNT_UPDOWN; // Count up
      EPwm2Regs.TBPRD = EPWM1_TIMER_TBPRD;       // Set timer period
      EPwm2Regs.TBCTL.bit.PHSEN = TB_ENABLE;    // enable phase loading  同步用
      EPwm2Regs.TBPHS.bit.TBPHS = 0x0000; // Phase is 0
      EPwm2Regs.TBCTL.bit.PHSDIR=1;
      EPwm2Regs.TBCTR = 0x0000;                  // Clear counter
      EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   // Clock ratio to SYSCLKOUT
      EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;

        EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
        EPwm3Regs.TBPRD = EPWM1_TIMER_TBPRD;       // Set timer period
        EPwm3Regs.TBCTL.bit.PHSEN = TB_ENABLE;    // enable phase loading 同步用
        EPwm3Regs.TBPHS.bit.TBPHS = 0x0000;        // Phase is 0
        EPwm3Regs.TBCTL.bit.PHSDIR=1;
        EPwm3Regs.TBCTR = 0x0000;                  // Clear counter
        EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   // Clock ratio to SYSCLKOUT
        EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;

        EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
        EPwm4Regs.TBPRD = EPWM1_TIMER_TBPRD;       // Set timer period
        EPwm4Regs.TBCTL.bit.PHSEN = TB_ENABLE;    // enable phase loading 同步用
        EPwm4Regs.TBPHS.bit.TBPHS = 0x0000;        // Phase is 0
        EPwm4Regs.TBCTL.bit.PHSDIR=1;
        EPwm4Regs.TBCTR = 0x0000;                  // Clear counter
        EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   // Clock ratio to SYSCLKOUT
        EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV1;


   // Setup Sync  The delay from internal master module to slave modules is given by:
   //– if ( TBCLK = EPWMCLK): 2 x EPWMCLK  – if ( TBCLK < EPWMCLK): 1 x TBCLK  同步有延迟，主从延迟可以看TBCTR寄存器看出来，加个同步主要为了保险
   //虽然是同一个TBCLK，但还是要每个周期都同步一下，万一中间跑飞了，没有同步就回不来了
   EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;      // Output EPWM1SYNCO signal when zero
   EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;  // Disable EPWM2SYNCO signal
   EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;  // Disable EPWM3SYNCO signal
   EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
   //
   // Setup shadow register load on ZERO
   //
   EPwm1Regs.TBCTL.bit.PRDLD = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  //加减计数再PRD时装载，一个周期前后不就可能不对称了吗

   EPwm2Regs.TBCTL.bit.PRDLD = CC_SHADOW;
   EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

   EPwm3Regs.TBCTL.bit.PRDLD = CC_SHADOW;
   EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

   EPwm4Regs.TBCTL.bit.PRDLD = CC_SHADOW;
   EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;


   // Set actions
   EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;//AQ_SET;            // Set PWM1A on Zero
   EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;//AQ_SET;            // Set PWM1A on Zero


   EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;
   EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;

   EPwm3Regs.AQCTLA.bit.CAU = AQ_SET;
   EPwm3Regs.AQCTLA.bit.CAD = AQ_CLEAR;

   EPwm4Regs.AQCTLA.bit.CAU = AQ_SET;
   EPwm4Regs.AQCTLA.bit.CAD = AQ_CLEAR;


  // Active Low PWMs - Setup Deadband
       EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
       EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; //低电平有效 互补输出，还是高电平有效互补输出
       EPwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;//A作为上下沿延时基础
       EPwm1Regs.DBRED.bit.DBRED =  80;
       EPwm1Regs.DBFED.bit.DBFED =  80;

       EPwm2Regs.DBCTL.bit.OUT_MODE =DB_FULL_ENABLE;// DB_FULL_ENABLE;
       EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;//DB_ACTV_HIC; //低电平有效 互补输出，还是高电平有效互补输出
       EPwm2Regs.DBCTL.bit.IN_MODE = DBA_ALL;//A作为上下沿延时基础
       EPwm2Regs.DBRED.bit.DBRED =  80;
       EPwm2Regs.DBFED.bit.DBFED =  80;

       EPwm3Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
       EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; //低电平有效 互补输出，还是高电平有效互补输出
       EPwm3Regs.DBCTL.bit.IN_MODE = DBA_ALL;//A
       EPwm3Regs.DBRED.bit.DBRED =  80;
       EPwm3Regs.DBFED.bit.DBFED =  80;

        EPwm4Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
        EPwm4Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; //低电平有效 互补输出，还是高电平有效互补输出
        EPwm4Regs.DBCTL.bit.IN_MODE = DBA_ALL;//A
        EPwm4Regs.DBRED.bit.DBRED =  80;
        EPwm4Regs.DBFED.bit.DBFED =  80;

       // Disable PWM chopping function
      	EPwm1Regs.PCCTL.bit.CHPEN= 0;
      	EPwm2Regs.PCCTL.bit.CHPEN= 0;
      	EPwm3Regs.PCCTL.bit.CHPEN= 0;
        EPwm4Regs.PCCTL.bit.CHPEN= 0;

      	 //ad信号
        EPwm4Regs.ETSEL.bit.SOCBEN = ET_CTR_ZERO;     //Enable EPWM1SOCB pulse
        EPwm4Regs.ETSEL.bit.SOCBSEL = ET_CTR_ZERO;     //Enable event time-base counter equal to period
   	    EPwm4Regs.ETPS.bit.SOCBPRD = ET_1ST;//ET_1ST;           // Generate EPWM1SOCB on 1st event                                                 //等进入中断后已经转换了半个周期

       // Interrupt where we will change the Deadband
       EPwm4Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;    // Select INT on Zero event  一般进入中断时间和采样信号同步，这样保证采回来数的实时性
       EPwm4Regs.ETSEL.bit.INTEN = 1;               // Enable INT
       EPwm4Regs.ETPS.bit.INTPRD = ET_1ST;          // Generate INT on 3rd event


       EALLOW;
       SyncSocRegs.ADCSOCOUTSELECT.bit.PWM4SOCBEN=1;




      EDIS;

      //拉0位用
//
//      EALLOW;
//      EPwm4Regs.TZCTL.bit.TZA=2;   //0Z 1H  2L  3CMPACMPB
//      EPwm4Regs.TZCTL.bit.TZB=2;
//      EPwm2Regs.TZCTL.bit.TZA=3;
//      EPwm2Regs.TZCTL.bit.TZB=3;
//      EPwm3Regs.TZCTL.bit.TZA=2;
//      EPwm3Regs.TZCTL.bit.TZB=1;
//
//      EPwm4Regs.TZFRC.bit.OST=1;
//      EPwm2Regs.TZFRC.bit.OST=1;
//      EPwm3Regs.TZFRC.bit.OST=1;
//      EDIS;

}



void Initemif1()
{
	 EALLOW;
	  ClkCfgRegs.PERCLKDIVSEL.bit.EMIF1CLKDIV = 0x0;   ////Configure to run EMIF1 on full Rate (EMIF1CLK = CPU1SYSCLK)
	  EDIS;

	  EALLOW;
	//
	// Grab EMIF1 For CPU1
	//
	  Emif1ConfigRegs.EMIF1MSEL.all = 0x93A5CE71;


	//
	//Disable Access Protection (CPU_FETCH/CPU_WR/DMA_WR)
	//
	  Emif1ConfigRegs.EMIF1ACCPROT0.all = 0x0;



	//
	// Commit the configuration related to protection. Till this bit remains set
	// content of EMIF1ACCPROT0 register can't be changed.
	//
	  Emif1ConfigRegs.EMIF1COMMIT.all = 0x1;


	//
	// Lock the configuration so that EMIF1COMMIT register can't be
	// changed any more.
	//
	  Emif1ConfigRegs.EMIF1LOCK.all = 0x1;

	  EDIS;

	//
	//Configure GPIO pins for EMIF1
	//
	  setup_emif1_pinmux_async_16bit(0); //先声明此函数在各模块GPIO前，这里还有很多引脚没用到，可能被别的模块占用，尽量不要改.c文件里的函数，所以先声明就好

	//
	//Configure the access timing for CS4 space
	//
      Emif1Regs.ASYNC_CS2_CR.all =  (EMIF_ASYNC_ASIZE_16    | // 16Bit Memory
                                                              // Interface
                                     EMIF_ASYNC_TA_4        | // Turn Around time
                                                              // of 2 Emif Clock
                                     EMIF_ASYNC_RHOLD_8     | // Read Hold time
                                                              // of 1 Emif Clock
                                     EMIF_ASYNC_RSTROBE_16   | // Read Strobe time
                                                              // of 4 Emif Clock
                                     EMIF_ASYNC_RSETUP_16    | // Read Setup time
                                                              // of 1 Emif Clock
                                     EMIF_ASYNC_WHOLD_8     | // Write Hold time
                                                              // of 1 Emif Clock
                                     EMIF_ASYNC_WSTROBE_16   | // Write Strobe time
                                                              // of 1 Emif Clock
                                     EMIF_ASYNC_WSETUP_16    | // Write Setup time
                                                              // of 1 Emif Clock
                                     EMIF_ASYNC_EW_DISABLE  | // Extended Wait
                                                              // Disable.
                                     EMIF_ASYNC_SS_DISABLE    // Strobe Select Mode
                                                              // Disable.
                                    );

      Emif1Regs.ASYNC_CS3_CR.all =  (EMIF_ASYNC_ASIZE_16    | // 16Bit Memory
                                                              // Interface
                                     EMIF_ASYNC_TA_4        | // Turn Around time
                                                              // of 2 Emif Clock
                                     EMIF_ASYNC_RHOLD_8     | // Read Hold time
                                                              // of 1 Emif Clock
                                     EMIF_ASYNC_RSTROBE_16   | // Read Strobe time
                                                              // of 4 Emif Clock
                                     EMIF_ASYNC_RSETUP_16    | // Read Setup time
                                                              // of 1 Emif Clock
                                     EMIF_ASYNC_WHOLD_8     | // Write Hold time
                                                              // of 1 Emif Clock
                                     EMIF_ASYNC_WSTROBE_16   | // Write Strobe time
                                                              // of 1 Emif Clock
                                     EMIF_ASYNC_WSETUP_16    | // Write Setup time
                                                              // of 1 Emif Clock
                                     EMIF_ASYNC_EW_DISABLE  | // Extended Wait
                                                              // Disable.
                                     EMIF_ASYNC_SS_DISABLE    // Strobe Select Mode
                                                              // Disable.
                                    );

	  Emif1Regs.ASYNC_CS4_CR.all =  (EMIF_ASYNC_ASIZE_16    | // 16Bit Memory
	                                                          // Interface
	                                 EMIF_ASYNC_TA_4        | // Turn Around time
	                                                          // of 2 Emif Clock
	                                 EMIF_ASYNC_RHOLD_8     | // Read Hold time
	                                                          // of 1 Emif Clock
	                                 EMIF_ASYNC_RSTROBE_16   | // Read Strobe time
	                                                          // of 4 Emif Clock
	                                 EMIF_ASYNC_RSETUP_16    | // Read Setup time
	                                                          // of 1 Emif Clock
	                                 EMIF_ASYNC_WHOLD_8     | // Write Hold time
	                                                          // of 1 Emif Clock
	                                 EMIF_ASYNC_WSTROBE_16   | // Write Strobe time
	                                                          // of 1 Emif Clock
	                                 EMIF_ASYNC_WSETUP_16    | // Write Setup time
	                                                          // of 1 Emif Clock
	                                 EMIF_ASYNC_EW_DISABLE  | // Extended Wait
	                                                          // Disable.
	                                 EMIF_ASYNC_SS_DISABLE    // Strobe Select Mode
	                                                          // Disable.
	                                );

}


//volatile int16 EXADReg;
//外部AD
 void Initshuju(void)
{
	EXADRESULTS.ADV1 = 0;
	EXADRESULTS.ADV2 = 0;
	EXADRESULTS.ADV3 = 0;
	EXADRESULTS.ADV4 = 0;
	EXADRESULTS.ADV5 = 0;
	EXADRESULTS.ADV6 = 0;
	EXADRESULTS.ADV7 = 0;
	EXADRESULTS.ADV8 = 0;


    SAMPLERUSELTS.IA = 0;
    SAMPLERUSELTS.IB = 0;
    SAMPLERUSELTS.IC = 0;
    SAMPLERUSELTS.IBUS = 0;
    SAMPLERUSELTS.UDC = 0;


}


//ad7616初始化
	void Initad7606(void)  //RANGE 低电平 硬件模式下，选择为正负5V
	{
	    GPIO_SetupPinMux(66,0,0);//配置成CPU1 GPIO
	    GPIO_SetupPinOptions(66,1,1);//配置成输出上拉                     STBY引脚 高电平弃用待机模式
	    GpioDataRegs.GPCDAT.bit.GPIO66=1;   //低电平是省电模式，软件操作下可忽略改引脚，最好外部接高。高电平时正常状态
	    GPIO_SetupPinMux(67,0,0);//配置成CPU1 GPIO
	    GPIO_SetupPinOptions(67,1,1);//配置成输出上拉                      复位引脚 高电平复位与7616不同
	    GPIO_SetupPinMux(68,0,0);//配置成CPU1 GPIO
	    GPIO_SetupPinOptions(68,0,GPIO_ASYNC);//配置成输入异步，  BUSY反馈
	    GPIO_SetupPinMux(10,0,3);//配置成CPU1 ADSOCBO   高电平有效正好，与7616不同
	                                                     //   GPIO_SetupPinOptions(10,1,1);//配置成输出 上拉 CONVST引脚  外接反相器正好一个高电平有效转为低电平有效
	    GpioDataRegs.GPCDAT.bit.GPIO67=0;
	    DELAY_US(100);
	    GpioDataRegs.GPCDAT.bit.GPIO67=1;
	    DELAY_US(5);//最少1.5us
	    GpioDataRegs.GPCDAT.bit.GPIO67=0;
	    DELAY_US(20000);//15ms后完成初始化

		GPIO_SetupPinMux(62,0,0);//配置成CPU1 GPIO
	    GPIO_SetupPinOptions(62,1,1);//配置成输出上拉                     REF引脚 低电平使用外部基准
	    GpioDataRegs.GPBDAT.bit.GPIO62=0;

	     GPIO_SetupPinMux(63,0,0);//配置成CPU1 GPIO
	      GPIO_SetupPinOptions(63,1,1);//配置成输出上拉       OS0引脚
	        GpioDataRegs.GPBDAT.bit.GPIO63=0;
	        GPIO_SetupPinMux(64,0,0);//配置成CPU1 GPIO
	         GPIO_SetupPinOptions(64,1,1);//配置成输出上拉       OS1引脚
	     GpioDataRegs.GPCDAT.bit.GPIO64=0;
	        GPIO_SetupPinMux(65,0,0);//配置成CPU1 GPIO
	       GPIO_SetupPinOptions(65,1,1);//配置成输出上拉       OS2引脚    000无过采样   111为软件模式
	        GpioDataRegs.GPCDAT.bit.GPIO65=0;
	                       //YML//正负5V软件配置程序
	                        *(AD7606) = 0x0515;  //YML//初始读写一次，没用
	                        *(AD7606) = 0x8500;  //YML//
	                        EXADRESULTS.ADV1= *(AD7606);
	                        *(AD7606) = 0x0311;  //YML//写0000 0011 0001 0001  通道1和2设置为正负5V
	                        //   *(AD7606) = 0x0000;
	                        *(AD7606) = 0x0411;  //YML//写0000 0100 0001 0001  通道3和4设置为正负5V
	                        // *(AD7606) = 0x0000;
	                       *(AD7606) = 0x0511;  //YML//写0000 0100 0010 0001  通道5设置为正负5V，6设置为正负5V
	                        // *(AD7606) = 0x0000;
	                        *(AD7606) = 0x0622;  //YML//写0000 0100 0010 0010  通道7设置为正负5V，8设置为正负10V
	                         *(AD7606) = 0x0000;




	}

	void Init_AD2S1210(void)  //Dfined by YML
	{
	    GPIO_SetupPinMux(162,0,0);              //RES1            //配置位数12位
	          GPIO_SetupPinOptions(162,1,1);          //配置成输出上拉
	          GpioDataRegs.GPFDAT.bit.GPIO162=1;
	          GPIO_SetupPinMux(163,0,0);              //RES0
	          GPIO_SetupPinOptions(163,1,1);          //配置成输出上拉
	          GpioDataRegs.GPFDAT.bit.GPIO163=0;

	          GPIO_SetupPinMux(164,0,0);              //XA0           配置程位置模式
	          GPIO_SetupPinOptions(164,1,1);          //配置成输出上拉
	          GPIO_SetupPinMux(167,0,0);              //XA1
	          GPIO_SetupPinOptions(167,1,1);          //配置成输出上拉

	          GpioDataRegs.GPFCLEAR.bit.GPIO164 = 1;  //不要用DAT
	          GpioDataRegs.GPFCLEAR.bit.GPIO167 = 1;  //A0A1=00是位置模式

	  //        GPIO_SetupPinMux(166,0,0);              //DOS
	   //       GPIO_SetupPinOptions(166,0,GPIO_ASYNC);

	      //    GPIO_SetupPinMux(167,0,0);              //LOT
	      //    GPIO_SetupPinOptions(167,0,GPIO_ASYNC);

	          *(AD2S1210) = 0x92;                     //把数据放到地址里  FRE
	          asm (" NOP");
	          *(AD2S1210) = 0x14;                     //把数据放到地址里


	  //读
	          *(AD2S1210) = 0x88;
	          AD2S.LOT0 = *(AD2S1210);
	          *(AD2S1210) = 0x89;
	          AD2S.DOS0 = *(AD2S1210);
	          *(AD2S1210) = 0x8A;
	          AD2S.DOS1 = *(AD2S1210);
	          *(AD2S1210) = 0x8B;
	          AD2S.DOS2 = *(AD2S1210);
	          *(AD2S1210) = 0x8C;
	          AD2S.DOS3 = *(AD2S1210);
	          *(AD2S1210) = 0x8D;
	          AD2S.LOT1 = *(AD2S1210);
	          *(AD2S1210) = 0x8E;
	          AD2S.LOT2 = *(AD2S1210);
	          *(AD2S1210) = 0x91;
	          AD2S.FREQ = *(AD2S1210);
	          *(AD2S1210) = 0x92;
	          AD2S.CONTROL = *(AD2S1210);
	          *(AD2S1210) = 0xFF;
	          AD2S.FAULT = *(AD2S1210);

	  //Output
	          GPIO_SetupPinMux(168,0,0);             //RESET
	          GPIO_SetupPinOptions(168,1,1);         //配置成输出上拉
	          GpioDataRegs.GPFDAT.bit.GPIO168=0;
	          DELAY_US(1);
	          GpioDataRegs.GPFDAT.bit.GPIO168=1;

	}

    void InitadIO(void)  //RANGE 低电平 硬件模式下，选择为正负5V
    {
//        GPIO_SetupPinMux(0,0,0);//配置成CPU1 GPIO
//        GPIO_SetupPinOptions(0,0,GPIO_ASYNC);//配置成输入异步，IO1
//
//        GPIO_SetupPinMux(42,0,0);//配置成CPU1 GPIO
//        GPIO_SetupPinOptions(42,0,GPIO_ASYNC);//配置成输入异步，IO2
//
//        GPIO_SetupPinMux(43,0,0);//配置成CPU1 GPIO
//        GPIO_SetupPinOptions(43,0,GPIO_ASYNC);//配置成输入异步，IO3
//
//        GPIO_SetupPinMux(120,0,0);//配置成CPU1 GPIO
//        GPIO_SetupPinOptions(120,0,GPIO_ASYNC);//配置成输入异步，IO4
//
//        GPIO_SetupPinMux(121,0,0);//配置成CPU1 GPIO
//        GPIO_SetupPinOptions(121,0,GPIO_ASYNC);//配置成输入异步，IO5
//
//        GPIO_SetupPinMux(126,0,0);//配置成CPU1 GPIO
//        GPIO_SetupPinOptions(126,0,GPIO_ASYNC);//配置成输入异步，Fault_A TOP 上桥臂
//
//        GPIO_SetupPinMux(129,0,0);//配置成CPU1 GPIO
//        GPIO_SetupPinOptions(129,0,GPIO_ASYNC);//配置成输入异步，Fault_A BOTTOM 下桥臂
//
//        GPIO_SetupPinMux(117,0,0);//配置成CPU1 GPIO
//        GPIO_SetupPinOptions(117,0,GPIO_ASYNC);//配置成输入异步，Fault_B TOP 上桥臂
//
//        GPIO_SetupPinMux(118,0,0);//配置成CPU1 GPIO
//        GPIO_SetupPinOptions(118,0,GPIO_ASYNC);//配置成输入异步，Fault_B BOTTOM 下桥臂
//
//        GPIO_SetupPinMux(119,0,0);//配置成CPU1 GPIO
//        GPIO_SetupPinOptions(119,0,GPIO_ASYNC);//配置成输入异步，Fault_C TOP 上桥臂
//
//        GPIO_SetupPinMux(138,0,0);//配置成CPU1 GPIO
//        GPIO_SetupPinOptions(138,0,GPIO_ASYNC);//配置成输入异步，Fault_C BOTTOM 下桥臂
//
//        GPIO_SetupPinMux(133,0,0);//配置成CPU1 GPIO
//        GPIO_SetupPinOptions(133,0,GPIO_ASYNC);//配置成输入异步，电压报警
//
//        GPIO_SetupPinMux(134,0,0);//配置成CPU1 GPIO
//        GPIO_SetupPinOptions(134,0,GPIO_ASYNC);//配置成输入异步，母线电流报警
//
//        GPIO_SetupPinMux(138,0,0);//配置成CPU1 GPIO
//        GPIO_SetupPinOptions(138,0,GPIO_ASYNC);//配置成输入异步，PWM使能状态/相电流过流
//
//        GPIO_SetupPinMux(137,0,0);//配置成CPU1 GPIO
//        GPIO_SetupPinOptions(137,1,1);//配置成输出上拉                  SD1引脚
//        GpioDataRegs.GPEDAT.bit.GPIO137= 1;
//
//        GPIO_SetupPinMux(135,0,0);//配置成CPU1 GPIO
//        GPIO_SetupPinOptions(135,1,1);//配置成输出上拉                   SD2引脚
//        GpioDataRegs.GPEDAT.bit.GPIO135= 1;
    }

    void Initad5725(void)  //RANGE 低电平 硬件模式下，选择为正负5V
    {
        GPIO_SetupPinMux(33,0,2);//配置成CPU1 GPIO RW管脚
    }



	//读AD结果

    void EXADREAD(void)
	{

		EXADRESULTS.ADV1 = *(AD7606);    //7606 8个通道
		EXADRESULTS.ADV2 = *(AD7606);
		EXADRESULTS.ADV3 = *(AD7606);
		EXADRESULTS.ADV4 = *(AD7606);
		EXADRESULTS.ADV5 = *(AD7606);
		EXADRESULTS.ADV6 = *(AD7606);
		EXADRESULTS.ADV7 = *(AD7606);
		EXADRESULTS.ADV8 = *(AD7606);

	}
//相电流与AD输出的数字量CODE关系：Vp=CODE*50/32768
		//规格化 （-1,1）

    void SAMPLEPROCESS(void)
	{

	    SAMPLERUSELTS.IA = ((float)EXADRESULTS.ADV1/32768);
	    SAMPLERUSELTS.IB = ((float)EXADRESULTS.ADV2/32768);
	    SAMPLERUSELTS.IC = ((float)EXADRESULTS.ADV4/32768);
	    SAMPLERUSELTS.IBUS =((float)EXADRESULTS.ADV5/32768);
	//	SAMPLERUSELTS.UDC =((float)EXADRESULTS.ADV3/32768*800);//母线电压显示的是实际值！！！三相电流都是标幺值

	}


//
// setCANBitRate - Set the CAN bit rate based on device clock (Hz)
//                 and desired bit rate (Hz)
//
uint32_t setCANBitRate(uint32_t sourceClock, uint32_t bitRate)
{
    uint32_t desiredRatio;
    uint32_t canBits;
    uint32_t preDivide;
    uint32_t regValue;
    uint16_t canControlValue;

    //
    // Calculate the desired clock rate.
    //
    desiredRatio = sourceClock / bitRate;

    //
    // Make sure that the Desired Ratio is not too large.  This enforces the
    // requirement that the bit rate is larger than requested.
    //
    if((sourceClock / desiredRatio) > bitRate)
    {
        desiredRatio += 1;
    }

    //
    // Check all possible values to find a matching value.
    //
    while(desiredRatio <= CAN_MAX_PRE_DIVISOR * CAN_MAX_BIT_DIVISOR)
    {
        //
        // Loop through all possible CAN bit divisors.
        //
        for(canBits = CAN_MAX_BIT_DIVISOR;
            canBits >= CAN_MIN_BIT_DIVISOR;
            canBits--)
        {
            //
            // For a given CAN bit divisor save the pre divisor.
            //
            preDivide = desiredRatio / canBits;

            //
            // If the calculated divisors match the desired clock ratio then
            // return these bit rate and set the CAN bit timing.
            //
            if((preDivide * canBits) == desiredRatio)
            {
                //
                // Start building the bit timing value by adding the bit timing
                // in time quanta.
                //
                regValue = canBitValues[canBits - CAN_MIN_BIT_DIVISOR];

                //
                // To set the bit timing register, the controller must be
                // placed
                // in init mode (if not already), and also configuration change
                // bit enabled.  The state of the register should be saved
                // so it can be restored.
                //
                canControlValue = CanaRegs.CAN_CTL.all;
                CanaRegs.CAN_CTL.bit.Init = 1;
                CanaRegs.CAN_CTL.bit.CCE = 1;

                //
                // Now add in the pre-scalar on the bit rate.
                //
                regValue |= ((preDivide - 1) & CAN_BTR_BRP_M) |
                            (((preDivide - 1) << 10) & CAN_BTR_BRPE_M);

                //
                // Set the clock bits in the and the bits of the
                // pre-scalar.
                //
                CanaRegs.CAN_BTR.all = regValue;

                //
                // Restore the saved CAN Control register.
                //
                CanaRegs.CAN_CTL.all = canControlValue;

                //
                // Return the computed bit rate.
                //
                return(sourceClock / ( preDivide * canBits));
            }
        }

        //
        // Move the divisor up one and look again.  Only in rare cases are
        // more than 2 loops required to find the value.
        //
        desiredRatio++;
    }
    return 0;
}

//
// setupMessageObject - Setup message object as Transmit or Receive
//
void setupMessageObject(uint32_t objID, uint32_t msgID, msgObjType msgType)
{
    //
    // Use Shadow variable for IF1CMD. IF1CMD should be written to in
    // single 32-bit write.
    //
    union CAN_IF1CMD_REG CAN_IF1CMD_SHADOW;

    //
    // Wait for busy bit to clear.
    //
    while(CanaRegs.CAN_IF1CMD.bit.Busy)
    {
    }

    //
    // Clear and
    //Write out the registers to program the message object.
    //
    CAN_IF1CMD_SHADOW.all = 0;
    CanaRegs.CAN_IF1MSK.all = 0;   //接受全都不过滤
    CanaRegs.CAN_IF1ARB.all = 0;
    CanaRegs.CAN_IF1MCTL.all = 0;

    //
    // Set the Control, Mask, and Arb bit so that they get transferred to the
    // Message object.
    //
    CAN_IF1CMD_SHADOW.bit.Control = 1;
    CAN_IF1CMD_SHADOW.bit.Arb = 1;
    CAN_IF1CMD_SHADOW.bit.Mask = 1;
    CAN_IF1CMD_SHADOW.bit.DIR = 1;

    //
    // Set direction to transmit
    //
    if(msgType == MSG_OBJ_TYPE_TRANSMIT)
    {
        CanaRegs.CAN_IF1ARB.bit.Dir = 1;
    }

    //
    // Set Message ID (this example assumes 11 bit ID mask)
    //
    CanaRegs.CAN_IF1ARB.bit.ID = (msgID << CAN_MSG_ID_SHIFT); //29位只用11位 ID0x0111 移位后1出去一个
    CanaRegs.CAN_IF1ARB.bit.MsgVal = 1;

    //
    // Set the data length since this is set for all transfers.  This is
    // also a single transfer and not a FIFO transfer so set EOB bit.
    //
    CanaRegs.CAN_IF1MCTL.bit.DLC = messageSize;
    CanaRegs.CAN_IF1MCTL.bit.EoB = 1;

    //
    // Transfer data to message object RAM
    //
    CAN_IF1CMD_SHADOW.bit.MSG_NUM = objID;
    CanaRegs.CAN_IF1CMD.all = CAN_IF1CMD_SHADOW.all;  //一写入就发送
}

//
// sendCANMessage - Transmit data from the specified message object
//
void sendCANMessage(uint32_t objID)
{
    //
    // Use Shadow variable for IF1CMD. IF1CMD should be written to in
    // single 32-bit write.
    //
    union CAN_IF1CMD_REG CAN_IF1CMD_SHADOW;

    //
    // Wait for busy bit to clear.
    //
    while(CanaRegs.CAN_IF1CMD.bit.Busy)
    {
    }

    //
    // Write data to transfer into DATA-A and DATA-B interface registers
    //
    uint16_t index;
    for(index = 0; index < messageSize; index++)
    {
        switch(index)
        {
            case 0:
                CanaRegs.CAN_IF1DATA.bit.Data_0 = ucTXMsgData[index];
                break;
            case 1:
                CanaRegs.CAN_IF1DATA.bit.Data_1 = ucTXMsgData[index];
                break;
            case 2:
                CanaRegs.CAN_IF1DATA.bit.Data_2 = ucTXMsgData[index];
                break;
            case 3:
                CanaRegs.CAN_IF1DATA.bit.Data_3 = ucTXMsgData[index];
                break;
            case 4:
                CanaRegs.CAN_IF1DATB.bit.Data_4 = ucTXMsgData[index];
                break;
            case 5:
                CanaRegs.CAN_IF1DATB.bit.Data_5 = ucTXMsgData[index];
                break;
            case 6:
                CanaRegs.CAN_IF1DATB.bit.Data_6 = ucTXMsgData[index];
                break;
            case 7:
                CanaRegs.CAN_IF1DATB.bit.Data_7 = ucTXMsgData[index];
                break;
        }
    }

    //
    // Set Direction to write and set DATA-A/DATA-B to be transfered to
    // message object
    //
    CAN_IF1CMD_SHADOW.all = 0;
    CAN_IF1CMD_SHADOW.bit.DIR = 1;
    CAN_IF1CMD_SHADOW.bit.DATA_A = 1;
    CAN_IF1CMD_SHADOW.bit.DATA_B = 1;

    //
    // Set Tx Request Bit
    //
    CAN_IF1CMD_SHADOW.bit.TXRQST = 1;

    //
    // Transfer the message object to the message object specified by
    // objID.
    //
    CAN_IF1CMD_SHADOW.bit.MSG_NUM = objID;
    CanaRegs.CAN_IF1CMD.all = CAN_IF1CMD_SHADOW.all;
}

//
// getCANMessage - Check the message object for new data.
//                 If new data, data written into array and return true.
//                 If no new data, return false.
//
bool getCANMessage(uint32_t objID)
{
    bool status;

    //
    // Use Shadow variable for IF2CMD. IF2CMD should be written to in
    // single 32-bit write.
    //
    union CAN_IF2CMD_REG CAN_IF2CMD_SHADOW;

    //
    // Set the Message Data A, Data B, and control values to be read
    // on request for data from the message object.
    //
    CAN_IF2CMD_SHADOW.all = 0;
    CAN_IF2CMD_SHADOW.bit.Control = 1;
    CAN_IF2CMD_SHADOW.bit.DATA_A = 1;
    CAN_IF2CMD_SHADOW.bit.DATA_B = 1;

    //
    // Transfer the message object to the message object IF register.
    //
    CAN_IF2CMD_SHADOW.bit.MSG_NUM = objID;
    CanaRegs.CAN_IF2CMD.all = CAN_IF2CMD_SHADOW.all;

    //
    // Wait for busy bit to clear.
    //
    while(CanaRegs.CAN_IF2CMD.bit.Busy)
    {
    }

    //
    // See if there is new data available.
    //
    if(CanaRegs.CAN_IF2MCTL.bit.NewDat == 1)
    {
        //
        // Read out the data from the CAN registers.
        //
        uint16_t index;
        for(index = 0; index < messageSize; index++)
        {
            switch(index)
            {
                case 0:
                    ucRXMsgData[index] = CanaRegs.CAN_IF2DATA.bit.Data_0;
                break;
                case 1:
                    ucRXMsgData[index] = CanaRegs.CAN_IF2DATA.bit.Data_1;
                break;
                case 2:
                    ucRXMsgData[index] = CanaRegs.CAN_IF2DATA.bit.Data_2;
                break;
                case 3:
                    ucRXMsgData[index] = CanaRegs.CAN_IF2DATA.bit.Data_3;
                break;
                case 4:
                    ucRXMsgData[index] = CanaRegs.CAN_IF2DATB.bit.Data_4;
                break;
                case 5:
                    ucRXMsgData[index] = CanaRegs.CAN_IF2DATB.bit.Data_5;
                break;
                case 6:
                    ucRXMsgData[index] = CanaRegs.CAN_IF2DATB.bit.Data_6;
                break;
                case 7:
                    ucRXMsgData[index] = CanaRegs.CAN_IF2DATB.bit.Data_7;
                break;
            }
        }

        //
        // Populate Shadow Variable
        //
        CAN_IF2CMD_SHADOW.all = CanaRegs.CAN_IF2CMD.all;

        //
        // Clear New Data Flag
        //
        CAN_IF2CMD_SHADOW.bit.TxRqst = 1;

        //
        // Transfer the message object to the message object IF register.
        //
        CAN_IF2CMD_SHADOW.bit.MSG_NUM = objID;
        CanaRegs.CAN_IF2CMD.all = CAN_IF2CMD_SHADOW.all;

        status = true;
    }
    else
    {
        status = false;
    }

    return(status);
}
void scic_echoback_init()
{
    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function

    ScicRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
                                   // No parity,8 char bits,
                                   // async mode, idle-line protocol
    ScicRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
                                   // Disable RX ERR, SLEEP, TXWAKE
    //SciaRegs.SCICTL2.all =0x0003;
    //SciaRegs.SCICTL2.bit.TXINTENA =1;
    //SciaRegs.SCICTL2.bit.RXBKINTENA =1;
  EALLOW;
ClkCfgRegs.LOSPCP.bit.LSPCLKDIV=0x0001;
 EDIS;
    ScicRegs.SCIHBAUD.all    =0x0000;  // 9600 baud @LSPCLK = 50MHz (200 MHz SYSCLK).
    ScicRegs.SCILBAUD.all    =0x0004;
    ScicRegs.SCICTL1.all =0x0023;  // Relinquish SCI from Reset
}

// Transmit a character from the SCI
void scic_xmit(int a)
{
    while (ScicRegs.SCIFFTX.bit.TXFFST != 0) {}
    ScicRegs.SCITXBUF.all =a;

}

void scic_msg(char * msg)
{
    int i;
    i = 0;
    while(msg[i] != '\0')
    {
        scic_xmit(msg[i]);
        i++;
    }
}


// Initialize the SCI FIFO
void scic_fifo_init()
{
    ScicRegs.SCIFFTX.all=0xE040;//使能发送FIFO,禁止发送FIFO中断
    ScicRegs.SCIFFRX.all=0x2044;//重置接受FIFO，禁止接收FIFO中断
    ScicRegs.SCIFFCT.all=0x0;//FIFO延迟为0，禁止波特率自动检测

}
void jieshou()
{
    if(CanaRegs.CAN_NDAT_21 & 0x00000002)

    {
       getCANMessage(CAN_RX_MSG_OBJ);//收速度
//         SpeedRef1 = ((float)((float)(ucRXMsgData[1]<<8)+ucRXMsgData[0]-2000)*0.0005);
   //    getCANMessage(3);//收id
       idcan1 = ((float)((float)(ucRXMsgData[0]<<8)+ucRXMsgData[1]-2000)*0.0004);
   //     getCANMessage(4);//收iq
        iqcan1 = ((float)((float)(ucRXMsgData[2]<<8)+ucRXMsgData[3]-2000)*0.0004);
  //     iss++;
    }
}
//
// End of file
//


//
// End of file
//
