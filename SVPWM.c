//###########################################################################
//
// ����˵����
// ��������е������Ե��SVPWM���Ƴ���
//��д�ˣ�������
//���������ڣ�2023����2����7
//���ʹ�����ڣ�2023_3_18      ���ݣ�������������
//�������ݣ���A�����¹ܸ�ΪPWM4AB���ƣ���XA1��ΪGPIO167���ơ�
/********************************************
 //*************** �޸ļ�¼*****************
//���һ��ʹ�ã�2_7_����������δ������������������֮ǰ���������������cos+-���ˣ�Ŀǰ��������λ�������˳ʱ����ת�ģ�
// 2_16_�ӹ�������������50kHz��ĸ�߿ɴ�210V

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



//YML_2022_11_5   �����������λ��1035��û��������

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



//#define FREQUENCY 200000000/EPWM1_TIMER_TBPRD/2  ��������ã�������ݿ�������ˣ����ʱ���㲻��
//extern Uint16 RamfuncsLoadStart;
//extern Uint16 RamfuncsLoadSize;
//extern Uint16 RamfuncsRunStart;


int16 DAHALF=0x0800;
int flagggg=0;

int flag_mix=0;//��ϵ��Ʊ�־λ

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
int16 XXX=0;//YML//���ԣ��ú��ɾ��
int16 AngleInit=0;//������������2^23
int32 Anglezero=3078;//������û��������3063������2�Լ������4�ԣ�__YML_2023_2_7����;
//��������е������Ե������λ��655������2�Լ������4�ԣ���
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
Uint16 PPnum=2; //��ֵ�����������/���伫����.���ɵ�������8�Լ�������4�Լ���
float32 Vd_Test_openloop=0; //������Ud��Uq����ֵ
float32 Vq_Test_openloop=0.4;
float32 iq_Test=0.03;//�����ջ���iq����ֵ
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

#define CAN_MSG_ID              0x1111 // This example only supports standard ID  // 0111ID ��׼ ����
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
void Initemif1();//����GPIO������ģ������
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
// and speed        ʵ��PID�����������ڵ���D��Qͬ������������ٶ�
PIDREG3 pid1_id = PIDREG3_DEFAULTS;
PIDREG3 pid1_iq = PIDREG3_DEFAULTS;
PIDREG3 pid1_spd = PIDREG3_DEFAULTS;
// Instance a Space Vector PWM modulator. This modulator generates a, b and c  ʵ��һ���ռ�ʸ��pwm��������
// phases based on the d and q stationery reference frame inputs �õ���������d��q��ֽ�ο�֡�������a��b��c��λ

SVGENDQ svgen_dq1 = SVGENDQ_DEFAULTS;
AZ1GENDQ az1gen_dq1 = AZ1GENDQ_DEFAULTS;
NSGENDQ nsgen_dq1 = NSGENDQ_DEFAULTS;
float m=0;


// Instance a ramp generator to simulate an Anglele  ʹ�ý�����������ģ��Ƕ�
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
    InitSysCtrl(); //�����Ѿ�����������ʱ�Ӷ�����
//
// Step 2. Initialize GPIO:
// This example function is found in the F2837xD_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
//
     InitGpio();

//

//
   Initemif1();//EMIF��ʼ��
    Initad7606();
    Init_AD2S1210();  //defined by YML
    Initad5725();
//    InitadIO();

    //AD7606B����ר��
        //  *(AD7606) = 0x0515;  //YML//д����FF,д����ֻ��Ҫһ����䡣
         // *(AD7606) = 0x8500;  //YML//��������Ҫ�������
        //  EXADRESULTS.ADV1= *(AD7606);
        //  *(AD7606) = 0x0311;  //YML//д0000 0011 0001 0001  ͨ��1��2����Ϊ����5V
        //  *(AD7606) = 0x8300;  //YML//��������Ҫ�������
        //  EXADRESULTS.ADV1= *(AD7606);
        // *(AD7606) = 0x0411;  //YML//д0000 0100 0001 0001  ͨ��3��4����Ϊ����5V
        //  *(AD7606) = 0x8400;  //YML//��������Ҫ�������
       //  EXADRESULTS.ADV2= *(AD7606);
       //   *(AD7606) = 0x0521;  //YML//д0000 0100 0010 0001  ͨ��5����Ϊ����5V��6����Ϊ����10V
       //   *(AD7606) = 0x8500;  //YML//��������Ҫ�������
       ////   EXADRESULTS.ADV3= *(AD7606);
        //  *(AD7606) = 0x0622;  //YML//д0000 0100 0010 0010  ͨ��7����Ϊ����5V��8����Ϊ����10V
        //  *(AD7606) = 0x8600;  //YML//��������Ҫ�������
        //   EXADRESULTS.ADV4= *(AD7606);

      InitEPwm1Gpio();//������Ƶ��Ƿ�Ĭ��ѡ��Ҫ��.c�ļ��ĺ���
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
       uint32_t status = setCANBitRate(200000000, 500000);   //���ò�����



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
     setupMessageObject(CAN_TX_MSG_OBJ, 0x0061, MSG_OBJ_TYPE_TRANSMIT); //����OBJ1���м��ǹ���id���������շ��������

     //
     // Initialize the message object that will be used for receiving CAN
     // messages.
     //
     setupMessageObject(CAN_RX_MSG_OBJ, 0x0041, MSG_OBJ_TYPE_RECEIVE); //���ٶ��� OBJ2 ����18λ��0X1111�൱��0X111  ��ɫ485�߶�Ӧ0041����ɫ485�߶�Ӧ0031
   //  setupMessageObject(3, 0X0002, MSG_OBJ_TYPE_RECEIVE);//��id 0x0001
   //  setupMessageObject(4, 0X0003, MSG_OBJ_TYPE_RECEIVE);//��iq 0x0011

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
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;//ֹͣTBCLK
    ClkCfgRegs.PERCLKDIVSEL.bit.EPWMCLKDIV=0;//2��ƵPLLSYSCLK 0����Ƶ��1 2��Ƶ
    EDIS;

    InitEPwmExample();//�������EPWMģ�飬������ֱ�ӿ�ʼ�ȴ���
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;//����TBCLK
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

    if(IA>0.3||IB>0.3||IC>0.3)//����100ŷķ����������㣬I1=Iʵ��/200; 0.2=40A
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

                  EALLOW;//����Ҫ��
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
                  Error_Flag=1;//���ϱ�־Ϊ��ֻ�������ճ�������������������ʹ��PWM
        }

    if(Clear_Error_Flag==1)
            {
                      EALLOW;//����Ҫ��
                      EPwm4Regs.TZCTL.bit.TZA=3;   //0Z 1H  2L  3CMPACMPB
                      EPwm4Regs.TZCTL.bit.TZB=3;
                      EPwm2Regs.TZCTL.bit.TZA=3;
                      EPwm2Regs.TZCTL.bit.TZB=3;
                      EPwm3Regs.TZCTL.bit.TZA=3;
                      EPwm3Regs.TZCTL.bit.TZB=3;
                      EDIS;

                      Error_Flag=0;//���ϱ�־Ϊ��ֻ�������ճ�������������������ʹ��PWM
                      Clear_Error_Flag=0;//���Լ�����
                      Over_Current_Flag=0;
            }

    if(Speedrpm>0.8)      //ת�ٿط�ֹ�ɳ�
        SpeedRef=0.6;//�ο�ת���޶���2000r

             //            idref1 = 0;//��Ϊ���ص�����ɳ�
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

//        if(iqref>0.08)  //Ť�ؿ���ר�õ�
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

       // while (GpioDataRegs.GPCDAT.bit.GPIO68==1) //AD��BUSY����
        //asm (" ESTOP0");
       // asm (" NOP"); //��Ҫ

        for(AD_Sample_Counter=0;AD_Sample_Counter<9;AD_Sample_Counter++)
        {
        }

        EXADREAD();//AD�����źŶ�����
        SAMPLEPROCESS();//�����IA IB IC UDC������


        angleREAD();  //����ʼλ��  SIC�ŷ����Ϊ5�Լ�
        AngleStore[0] = AngleReal;//��Ƕ�(-PI,PI)
        temp1 = AngleStore[0]-AngleStore[1];//YML//��ǰ�ĽǶȼ�ȥ��ȥһ��ʱ�̵ĽǶ�
           if(temp1<-4)//5�Լ���76000r/min=4   3000r/min=0.157 ��������ͼ�������� ��ֹ����߽�Ƕ�����
           {
              temp1 += 2*PI;
           }
            else if(temp1>4)
            {
              temp1 -= 2*PI;
            }
            //�ٶȼ���
           OmegaStore[0] =100000000/EPWM1_TIMER_TBPRD*temp1/4/2000;//w rad/s ��temp*f/p/nb p=4 nb=2000 f = 10000 ����f=20k����Ҫ��һ��������    ����ֵ2000r/min ������ۺ��ں������һ��
            OmegaM = 0.01*OmegaStore[0] + 0.99*OmegaStore[1];  //�˲�ϵ���������޸�
            OmegaStore[1] = OmegaM;
          // OmegaM=100000000/EPWM1_TIMER_TBPRD*temp1/4/2000;
            Speedrpm = OmegaM*30/PI;// n=60*w/2/PI (rpm)
            AngleStore[1] = AngleStore[0];

           //Speed loop function
            pid1_spd.Ref = SpeedRef; //����ֵ2000RPM��SpeedRef=0.1��200RPM
            if(SpeedLoopCount<SpeedLoopPrescaler)
                SpeedLoopCount++;
              else
              {
                  SpeedLoopCount = 0;

               pid1_spd.Fdb =Speedrpm;
               pid1_spd.calc(&pid1_spd);
              }


            /****************����*/
//
//             EPwm4Regs.TZSEL.bit.OSHT1 = 1;
//
//             EPwm2Regs.CMPA.bit.CMPA = (int16)(0.5*EPWM1_TIMER_TBPRD);
//             EALLOW;
//             EPwm4Regs.TZFRC.bit.OST = 1;
//             EPwm3Regs.TZFRC.bit.OST = 1;
//             EDIS;



//            rampgen_calc(&rg1);//YML//����ר�ã���    ǰ����϶����HF=50������Ȼ��rg1.Fre=HF*0.001;//���ǽǶ�б�µı仯б�ʣ����ֻ��Ҫ�ı�Ud,Uq=0���ɡ�������������ö���
//            angle1.Angle = rg1.Angle;
              angle1.Angle =AngleReal;//�ջ���Ҫ��
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
            pid1_id.calc(&pid1_id);//���ΪUd

            // ------------------------------------------------------------------------------
            //    Connect inputs of the PIDREG3 module and call the pid loopback
            //    calculation function.
            // ------------------------------------------------------------------------------
            //pid1_iq.Ref =0.01;  //ȥ��ת�ٻ�
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
            case 0://��AZ1->NS ����ƶ� //��ֹƵ������ �ߵ��ƶȷ�����
                if(m<Change_Point&&m>=0)//az1 ��
                    {
                        if(AngleReal/60> Angle_pianyi && AngleReal/60<(60-Angle_pianyi))//�Ƕ���
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
                    else   //�л��� ns
                    {
                        nsgen_dq1.Ualpha = ipark1.Alpha;
                        nsgen_dq1.Ubeta = ipark1.Beta;
                        nsgen_dq1.calc(&nsgen_dq1);

                        EPwm4Regs.CMPA.bit.CMPA  = (int16)(nsgen_dq1.Ta*EPWM1_TIMER_TBPRD);
                        EPwm2Regs.CMPA.bit.CMPA = (int16)(nsgen_dq1.Tb*EPWM1_TIMER_TBPRD);
                        EPwm3Regs.CMPA.bit.CMPA = (int16)(nsgen_dq1.Tc*EPWM1_TIMER_TBPRD);
                        flagggg=1;
                        flag_mix=1;//�ﵽ�趨���ƶ� ��Ҫ�ȶ�����
                    }
                break;
            case 1://��ֹƵ������ �͵��ƶȷ�����
                if(m<Return_Point&&m>=0)
                    {
                        if(AngleReal/60> Angle_pianyi && AngleReal/60<(60-Angle_pianyi))//�Ƕ���
                        {
                            az1gen_dq1.Ualpha = ipark1.Alpha;
                            az1gen_dq1.Ubeta = ipark1.Beta;
                            az1gen_dq1.calc(&az1gen_dq1);

                            EPwm4Regs.CMPA.bit.CMPA  = (int16)(az1gen_dq1.Ta*EPWM1_TIMER_TBPRD);
                            EPwm2Regs.CMPA.bit.CMPA = (int16)(az1gen_dq1.Tb*EPWM1_TIMER_TBPRD);
                            EPwm3Regs.CMPA.bit.CMPA = (int16)(az1gen_dq1.Tc*EPWM1_TIMER_TBPRD);
                            flagggg=0;
                            flag_mix=0;//�ﵽ�趨���ƶ� ��Ҫ�ȶ�����
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


//          EPwm4Regs.CMPA.bit.CMPA  = (int16)(0.5*EPWM1_TIMER_TBPRD);    //��0��
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

            bianma1=bianma>>9;//����9λ��23λת14λ
              AngleInit = (int32) bianma1;

  */
      AngleInit = *(AD2S1210)>>4;
      AngleInit = AngleInit-Anglezero;
      temp1 = ((float32)AngleInit/2048.0*PI)*PPnum+PI/2;//pi/2�Ƕ�λ�ĳ�ʼ�Ƕȣ����Ŷ�λ��ʽ��ͬ�ı�
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
    GPIO_SetupPinMux(128,0,0);//���ó�CPU1 GPIO
    GPIO_SetupPinOptions(128,1,1);//���ó��������
    GpioDataRegs.GPEDAT.bit.GPIO128=0;   //��
    GPIO_SetupPinMux(53,0,0);//���ó�CPU1 GPIO
	GPIO_SetupPinOptions(53,1,1);//���ó���� ���� ��λ����
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
      EPwm2Regs.TBCTL.bit.PHSEN = TB_ENABLE;    // enable phase loading  ͬ����
      EPwm2Regs.TBPHS.bit.TBPHS = 0x0000; // Phase is 0
      EPwm2Regs.TBCTL.bit.PHSDIR=1;
      EPwm2Regs.TBCTR = 0x0000;                  // Clear counter
      EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   // Clock ratio to SYSCLKOUT
      EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;

        EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
        EPwm3Regs.TBPRD = EPWM1_TIMER_TBPRD;       // Set timer period
        EPwm3Regs.TBCTL.bit.PHSEN = TB_ENABLE;    // enable phase loading ͬ����
        EPwm3Regs.TBPHS.bit.TBPHS = 0x0000;        // Phase is 0
        EPwm3Regs.TBCTL.bit.PHSDIR=1;
        EPwm3Regs.TBCTR = 0x0000;                  // Clear counter
        EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   // Clock ratio to SYSCLKOUT
        EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;

        EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
        EPwm4Regs.TBPRD = EPWM1_TIMER_TBPRD;       // Set timer period
        EPwm4Regs.TBCTL.bit.PHSEN = TB_ENABLE;    // enable phase loading ͬ����
        EPwm4Regs.TBPHS.bit.TBPHS = 0x0000;        // Phase is 0
        EPwm4Regs.TBCTL.bit.PHSDIR=1;
        EPwm4Regs.TBCTR = 0x0000;                  // Clear counter
        EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   // Clock ratio to SYSCLKOUT
        EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV1;


   // Setup Sync  The delay from internal master module to slave modules is given by:
   //�C if ( TBCLK = EPWMCLK): 2 x EPWMCLK  �C if ( TBCLK < EPWMCLK): 1 x TBCLK  ͬ�����ӳ٣������ӳٿ��Կ�TBCTR�Ĵ������������Ӹ�ͬ����ҪΪ�˱���
   //��Ȼ��ͬһ��TBCLK��������Ҫÿ�����ڶ�ͬ��һ�£���һ�м��ܷ��ˣ�û��ͬ���ͻز�����
   EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;      // Output EPWM1SYNCO signal when zero
   EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;  // Disable EPWM2SYNCO signal
   EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;  // Disable EPWM3SYNCO signal
   EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
   //
   // Setup shadow register load on ZERO
   //
   EPwm1Regs.TBCTL.bit.PRDLD = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  //�Ӽ�������PRDʱװ�أ�һ������ǰ�󲻾Ϳ��ܲ��Գ�����

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
       EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; //�͵�ƽ��Ч ������������Ǹߵ�ƽ��Ч�������
       EPwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;//A��Ϊ��������ʱ����
       EPwm1Regs.DBRED.bit.DBRED =  80;
       EPwm1Regs.DBFED.bit.DBFED =  80;

       EPwm2Regs.DBCTL.bit.OUT_MODE =DB_FULL_ENABLE;// DB_FULL_ENABLE;
       EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;//DB_ACTV_HIC; //�͵�ƽ��Ч ������������Ǹߵ�ƽ��Ч�������
       EPwm2Regs.DBCTL.bit.IN_MODE = DBA_ALL;//A��Ϊ��������ʱ����
       EPwm2Regs.DBRED.bit.DBRED =  80;
       EPwm2Regs.DBFED.bit.DBFED =  80;

       EPwm3Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
       EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; //�͵�ƽ��Ч ������������Ǹߵ�ƽ��Ч�������
       EPwm3Regs.DBCTL.bit.IN_MODE = DBA_ALL;//A
       EPwm3Regs.DBRED.bit.DBRED =  80;
       EPwm3Regs.DBFED.bit.DBFED =  80;

        EPwm4Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
        EPwm4Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; //�͵�ƽ��Ч ������������Ǹߵ�ƽ��Ч�������
        EPwm4Regs.DBCTL.bit.IN_MODE = DBA_ALL;//A
        EPwm4Regs.DBRED.bit.DBRED =  80;
        EPwm4Regs.DBFED.bit.DBFED =  80;

       // Disable PWM chopping function
      	EPwm1Regs.PCCTL.bit.CHPEN= 0;
      	EPwm2Regs.PCCTL.bit.CHPEN= 0;
      	EPwm3Regs.PCCTL.bit.CHPEN= 0;
        EPwm4Regs.PCCTL.bit.CHPEN= 0;

      	 //ad�ź�
        EPwm4Regs.ETSEL.bit.SOCBEN = ET_CTR_ZERO;     //Enable EPWM1SOCB pulse
        EPwm4Regs.ETSEL.bit.SOCBSEL = ET_CTR_ZERO;     //Enable event time-base counter equal to period
   	    EPwm4Regs.ETPS.bit.SOCBPRD = ET_1ST;//ET_1ST;           // Generate EPWM1SOCB on 1st event                                                 //�Ƚ����жϺ��Ѿ�ת���˰������

       // Interrupt where we will change the Deadband
       EPwm4Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;    // Select INT on Zero event  һ������ж�ʱ��Ͳ����ź�ͬ����������֤�ɻ�������ʵʱ��
       EPwm4Regs.ETSEL.bit.INTEN = 1;               // Enable INT
       EPwm4Regs.ETPS.bit.INTPRD = ET_1ST;          // Generate INT on 3rd event


       EALLOW;
       SyncSocRegs.ADCSOCOUTSELECT.bit.PWM4SOCBEN=1;




      EDIS;

      //��0λ��
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
	  setup_emif1_pinmux_async_16bit(0); //�������˺����ڸ�ģ��GPIOǰ�����ﻹ�кܶ�����û�õ������ܱ����ģ��ռ�ã�������Ҫ��.c�ļ���ĺ����������������ͺ�

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
//�ⲿAD
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


//ad7616��ʼ��
	void Initad7606(void)  //RANGE �͵�ƽ Ӳ��ģʽ�£�ѡ��Ϊ����5V
	{
	    GPIO_SetupPinMux(66,0,0);//���ó�CPU1 GPIO
	    GPIO_SetupPinOptions(66,1,1);//���ó��������                     STBY���� �ߵ�ƽ���ô���ģʽ
	    GpioDataRegs.GPCDAT.bit.GPIO66=1;   //�͵�ƽ��ʡ��ģʽ����������¿ɺ��Ը����ţ�����ⲿ�Ӹߡ��ߵ�ƽʱ����״̬
	    GPIO_SetupPinMux(67,0,0);//���ó�CPU1 GPIO
	    GPIO_SetupPinOptions(67,1,1);//���ó��������                      ��λ���� �ߵ�ƽ��λ��7616��ͬ
	    GPIO_SetupPinMux(68,0,0);//���ó�CPU1 GPIO
	    GPIO_SetupPinOptions(68,0,GPIO_ASYNC);//���ó������첽��  BUSY����
	    GPIO_SetupPinMux(10,0,3);//���ó�CPU1 ADSOCBO   �ߵ�ƽ��Ч���ã���7616��ͬ
	                                                     //   GPIO_SetupPinOptions(10,1,1);//���ó���� ���� CONVST����  ��ӷ���������һ���ߵ�ƽ��ЧתΪ�͵�ƽ��Ч
	    GpioDataRegs.GPCDAT.bit.GPIO67=0;
	    DELAY_US(100);
	    GpioDataRegs.GPCDAT.bit.GPIO67=1;
	    DELAY_US(5);//����1.5us
	    GpioDataRegs.GPCDAT.bit.GPIO67=0;
	    DELAY_US(20000);//15ms����ɳ�ʼ��

		GPIO_SetupPinMux(62,0,0);//���ó�CPU1 GPIO
	    GPIO_SetupPinOptions(62,1,1);//���ó��������                     REF���� �͵�ƽʹ���ⲿ��׼
	    GpioDataRegs.GPBDAT.bit.GPIO62=0;

	     GPIO_SetupPinMux(63,0,0);//���ó�CPU1 GPIO
	      GPIO_SetupPinOptions(63,1,1);//���ó��������       OS0����
	        GpioDataRegs.GPBDAT.bit.GPIO63=0;
	        GPIO_SetupPinMux(64,0,0);//���ó�CPU1 GPIO
	         GPIO_SetupPinOptions(64,1,1);//���ó��������       OS1����
	     GpioDataRegs.GPCDAT.bit.GPIO64=0;
	        GPIO_SetupPinMux(65,0,0);//���ó�CPU1 GPIO
	       GPIO_SetupPinOptions(65,1,1);//���ó��������       OS2����    000�޹�����   111Ϊ���ģʽ
	        GpioDataRegs.GPCDAT.bit.GPIO65=0;
	                       //YML//����5V������ó���
	                        *(AD7606) = 0x0515;  //YML//��ʼ��дһ�Σ�û��
	                        *(AD7606) = 0x8500;  //YML//
	                        EXADRESULTS.ADV1= *(AD7606);
	                        *(AD7606) = 0x0311;  //YML//д0000 0011 0001 0001  ͨ��1��2����Ϊ����5V
	                        //   *(AD7606) = 0x0000;
	                        *(AD7606) = 0x0411;  //YML//д0000 0100 0001 0001  ͨ��3��4����Ϊ����5V
	                        // *(AD7606) = 0x0000;
	                       *(AD7606) = 0x0511;  //YML//д0000 0100 0010 0001  ͨ��5����Ϊ����5V��6����Ϊ����5V
	                        // *(AD7606) = 0x0000;
	                        *(AD7606) = 0x0622;  //YML//д0000 0100 0010 0010  ͨ��7����Ϊ����5V��8����Ϊ����10V
	                         *(AD7606) = 0x0000;




	}

	void Init_AD2S1210(void)  //Dfined by YML
	{
	    GPIO_SetupPinMux(162,0,0);              //RES1            //����λ��12λ
	          GPIO_SetupPinOptions(162,1,1);          //���ó��������
	          GpioDataRegs.GPFDAT.bit.GPIO162=1;
	          GPIO_SetupPinMux(163,0,0);              //RES0
	          GPIO_SetupPinOptions(163,1,1);          //���ó��������
	          GpioDataRegs.GPFDAT.bit.GPIO163=0;

	          GPIO_SetupPinMux(164,0,0);              //XA0           ���ó�λ��ģʽ
	          GPIO_SetupPinOptions(164,1,1);          //���ó��������
	          GPIO_SetupPinMux(167,0,0);              //XA1
	          GPIO_SetupPinOptions(167,1,1);          //���ó��������

	          GpioDataRegs.GPFCLEAR.bit.GPIO164 = 1;  //��Ҫ��DAT
	          GpioDataRegs.GPFCLEAR.bit.GPIO167 = 1;  //A0A1=00��λ��ģʽ

	  //        GPIO_SetupPinMux(166,0,0);              //DOS
	   //       GPIO_SetupPinOptions(166,0,GPIO_ASYNC);

	      //    GPIO_SetupPinMux(167,0,0);              //LOT
	      //    GPIO_SetupPinOptions(167,0,GPIO_ASYNC);

	          *(AD2S1210) = 0x92;                     //�����ݷŵ���ַ��  FRE
	          asm (" NOP");
	          *(AD2S1210) = 0x14;                     //�����ݷŵ���ַ��


	  //��
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
	          GPIO_SetupPinOptions(168,1,1);         //���ó��������
	          GpioDataRegs.GPFDAT.bit.GPIO168=0;
	          DELAY_US(1);
	          GpioDataRegs.GPFDAT.bit.GPIO168=1;

	}

    void InitadIO(void)  //RANGE �͵�ƽ Ӳ��ģʽ�£�ѡ��Ϊ����5V
    {
//        GPIO_SetupPinMux(0,0,0);//���ó�CPU1 GPIO
//        GPIO_SetupPinOptions(0,0,GPIO_ASYNC);//���ó������첽��IO1
//
//        GPIO_SetupPinMux(42,0,0);//���ó�CPU1 GPIO
//        GPIO_SetupPinOptions(42,0,GPIO_ASYNC);//���ó������첽��IO2
//
//        GPIO_SetupPinMux(43,0,0);//���ó�CPU1 GPIO
//        GPIO_SetupPinOptions(43,0,GPIO_ASYNC);//���ó������첽��IO3
//
//        GPIO_SetupPinMux(120,0,0);//���ó�CPU1 GPIO
//        GPIO_SetupPinOptions(120,0,GPIO_ASYNC);//���ó������첽��IO4
//
//        GPIO_SetupPinMux(121,0,0);//���ó�CPU1 GPIO
//        GPIO_SetupPinOptions(121,0,GPIO_ASYNC);//���ó������첽��IO5
//
//        GPIO_SetupPinMux(126,0,0);//���ó�CPU1 GPIO
//        GPIO_SetupPinOptions(126,0,GPIO_ASYNC);//���ó������첽��Fault_A TOP ���ű�
//
//        GPIO_SetupPinMux(129,0,0);//���ó�CPU1 GPIO
//        GPIO_SetupPinOptions(129,0,GPIO_ASYNC);//���ó������첽��Fault_A BOTTOM ���ű�
//
//        GPIO_SetupPinMux(117,0,0);//���ó�CPU1 GPIO
//        GPIO_SetupPinOptions(117,0,GPIO_ASYNC);//���ó������첽��Fault_B TOP ���ű�
//
//        GPIO_SetupPinMux(118,0,0);//���ó�CPU1 GPIO
//        GPIO_SetupPinOptions(118,0,GPIO_ASYNC);//���ó������첽��Fault_B BOTTOM ���ű�
//
//        GPIO_SetupPinMux(119,0,0);//���ó�CPU1 GPIO
//        GPIO_SetupPinOptions(119,0,GPIO_ASYNC);//���ó������첽��Fault_C TOP ���ű�
//
//        GPIO_SetupPinMux(138,0,0);//���ó�CPU1 GPIO
//        GPIO_SetupPinOptions(138,0,GPIO_ASYNC);//���ó������첽��Fault_C BOTTOM ���ű�
//
//        GPIO_SetupPinMux(133,0,0);//���ó�CPU1 GPIO
//        GPIO_SetupPinOptions(133,0,GPIO_ASYNC);//���ó������첽����ѹ����
//
//        GPIO_SetupPinMux(134,0,0);//���ó�CPU1 GPIO
//        GPIO_SetupPinOptions(134,0,GPIO_ASYNC);//���ó������첽��ĸ�ߵ�������
//
//        GPIO_SetupPinMux(138,0,0);//���ó�CPU1 GPIO
//        GPIO_SetupPinOptions(138,0,GPIO_ASYNC);//���ó������첽��PWMʹ��״̬/���������
//
//        GPIO_SetupPinMux(137,0,0);//���ó�CPU1 GPIO
//        GPIO_SetupPinOptions(137,1,1);//���ó��������                  SD1����
//        GpioDataRegs.GPEDAT.bit.GPIO137= 1;
//
//        GPIO_SetupPinMux(135,0,0);//���ó�CPU1 GPIO
//        GPIO_SetupPinOptions(135,1,1);//���ó��������                   SD2����
//        GpioDataRegs.GPEDAT.bit.GPIO135= 1;
    }

    void Initad5725(void)  //RANGE �͵�ƽ Ӳ��ģʽ�£�ѡ��Ϊ����5V
    {
        GPIO_SetupPinMux(33,0,2);//���ó�CPU1 GPIO RW�ܽ�
    }



	//��AD���

    void EXADREAD(void)
	{

		EXADRESULTS.ADV1 = *(AD7606);    //7606 8��ͨ��
		EXADRESULTS.ADV2 = *(AD7606);
		EXADRESULTS.ADV3 = *(AD7606);
		EXADRESULTS.ADV4 = *(AD7606);
		EXADRESULTS.ADV5 = *(AD7606);
		EXADRESULTS.ADV6 = *(AD7606);
		EXADRESULTS.ADV7 = *(AD7606);
		EXADRESULTS.ADV8 = *(AD7606);

	}
//�������AD�����������CODE��ϵ��Vp=CODE*50/32768
		//��� ��-1,1��

    void SAMPLEPROCESS(void)
	{

	    SAMPLERUSELTS.IA = ((float)EXADRESULTS.ADV1/32768);
	    SAMPLERUSELTS.IB = ((float)EXADRESULTS.ADV2/32768);
	    SAMPLERUSELTS.IC = ((float)EXADRESULTS.ADV4/32768);
	    SAMPLERUSELTS.IBUS =((float)EXADRESULTS.ADV5/32768);
	//	SAMPLERUSELTS.UDC =((float)EXADRESULTS.ADV3/32768*800);//ĸ�ߵ�ѹ��ʾ����ʵ��ֵ����������������Ǳ���ֵ

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
    CanaRegs.CAN_IF1MSK.all = 0;   //����ȫ��������
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
    CanaRegs.CAN_IF1ARB.bit.ID = (msgID << CAN_MSG_ID_SHIFT); //29λֻ��11λ ID0x0111 ��λ��1��ȥһ��
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
    CanaRegs.CAN_IF1CMD.all = CAN_IF1CMD_SHADOW.all;  //һд��ͷ���
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
    ScicRegs.SCIFFTX.all=0xE040;//ʹ�ܷ���FIFO,��ֹ����FIFO�ж�
    ScicRegs.SCIFFRX.all=0x2044;//���ý���FIFO����ֹ����FIFO�ж�
    ScicRegs.SCIFFCT.all=0x0;//FIFO�ӳ�Ϊ0����ֹ�������Զ����

}
void jieshou()
{
    if(CanaRegs.CAN_NDAT_21 & 0x00000002)

    {
       getCANMessage(CAN_RX_MSG_OBJ);//���ٶ�
//         SpeedRef1 = ((float)((float)(ucRXMsgData[1]<<8)+ucRXMsgData[0]-2000)*0.0005);
   //    getCANMessage(3);//��id
       idcan1 = ((float)((float)(ucRXMsgData[0]<<8)+ucRXMsgData[1]-2000)*0.0004);
   //     getCANMessage(4);//��iq
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
