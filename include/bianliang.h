
#ifndef bianliang_H
#define bianliang_H

////����EMIF��ַ����
	Uint16    *AD7606 = (Uint16 *)0x380000;
    Uint16    *AD2S1210 = (Uint16 *)0x380008;
	Uint16    *DAA 	  = (Uint16 *)0x380010;
	Uint16    *DAB    = (Uint16 *)0x380011;
	Uint16    *DAC    = (Uint16 *)0x380012;
	Uint16    *DAD    = (Uint16 *)0x380013;
	//����AD�ṹ��
		struct EXAD
		{
			int16 ADV1; // V1  ����V0A
			int16 ADV2; // V2  ����
			int16 ADV3; // V3  ����V1A
			int16 ADV4; // V4  ����
			int16 ADV5; // V5  ����V2A
			int16 ADV6; // V6  ����
			int16 ADV7; // V7  ����V3A
			int16 ADV8; // V8  ����



		};

		//�����������ṹ��
			struct SAMPLE
			{
				float32 IA; // IA
				float32 IB; // IB
				float32 IC; // IC
				float32 IBUS; // IBUS
				float32 UDC; // UDC

			};

#endif  //

//
// End of file
//
