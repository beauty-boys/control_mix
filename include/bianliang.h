
#ifndef bianliang_H
#define bianliang_H

////外扩EMIF地址声明
	Uint16    *AD7606 = (Uint16 *)0x380000;
    Uint16    *AD2S1210 = (Uint16 *)0x380008;
	Uint16    *DAA 	  = (Uint16 *)0x380010;
	Uint16    *DAB    = (Uint16 *)0x380011;
	Uint16    *DAC    = (Uint16 *)0x380012;
	Uint16    *DAD    = (Uint16 *)0x380013;
	//外扩AD结构体
		struct EXAD
		{
			int16 ADV1; // V1  数据V0A
			int16 ADV2; // V2  测试
			int16 ADV3; // V3  数据V1A
			int16 ADV4; // V4  测试
			int16 ADV5; // V5  数据V2A
			int16 ADV6; // V6  测试
			int16 ADV7; // V7  数据V3A
			int16 ADV8; // V8  测试



		};

		//定义采样处理结构体
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
