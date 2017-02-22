m1

#ifndef _BASE_H_
#define _BASE_H_

//#define		BYTE				unsigned char
typedef unsigned char           BYTE;

//#define		UINT				unsigned int
typedef unsigned int            UINT;

#define		ULNG				unsigned long
#define		ULLG				unsigned long long

typedef struct {
	BYTE year;
	BYTE month;
	BYTE day;
	BYTE hour;
	BYTE minute;
	BYTE second;
}hTIME;



//发送任务
typedef struct{
	BYTE fc;				//[%%%% @@@@]=[格式/通信]
	UINT sec_stamp;			//=min*60+sec
	BYTE fSuccess;			//发送成功标志
}TMISSION;


//端口
typedef struct{
	BYTE buf[68];	//缓冲
//	BYTE *belt;		//max:header[32]+[len:2]+[belt:32]+[crc:2]
	UINT time_tick;	//单位：ms
	struct{
		unsigned 	bitSensorRequest:1;		//Sensor：设置的请求标志，由Sensor提出，Host清除。
		unsigned 	bitPortSubscribe:1;		//Host：预订Sensor消息(INT)，由Host在需要时设定（如显示）。无预订时，传感器可保持IDLE。
		unsigned 	bitSensorChanged:1;		//至少一个Cx值已经改变。
		unsigned	:5;						//暂时不用
	};
}PORT;
 

typedef struct{
	BYTE val;		//0,1,2,3: 分别代表button 1-4
	BYTE count;		//检测到的键盘重复次数
}KEY;


typedef union{
	struct{
		unsigned	bitINST_DIR:1;
		unsigned	bitANSWER_NEEDED:1;
		unsigned	bitIGNORE_TIME:1;
		unsigned	bitCONCATENATED:1;
		unsigned	bitIfSINGLE_FRAME:1;
		unsigned	bitIfWAIT:1;
		unsigned	bitBROADCAST:1;
		unsigned	:1;
		unsigned	bitSPACK:1;
		unsigned	bitDES_CHECK:1;
		unsigned	bitXPARA:1;
		unsigned	bitLOGIN:1;
		unsigned	bitDO_CFG_CRC:1;
		unsigned	:1;
		unsigned	:1;
		unsigned	bitRET:1;
	};
  	UINT ALLbits;
}PROPERTY;

/*
typedef union{
	struct{
		unsigned 	bitScheduleInt:1;
		unsigned	bitSensorInt:1;		//暂时不用
		unsigned	bitComm1Int:1;
		unsigned	bitComm2Int:1;		
		unsigned	bitKeyInt:1;		//暂时不用
		unsigned	bitUserInt:1;		//暂时不用
		unsigned	bitSerSensorInt:1;	//串口传感器
		unsigned 	:1;
	};
  	BYTE ALLbits;
}TASKLINE;
*/

typedef union{
	struct{
		unsigned 	bitSrnRefresh:1;
		unsigned	bitKeyReleased:1;		//按键释放标记
		unsigned	bitBasicBeWritten:1;
		unsigned	bitHomePageChoice:1;	//首页选择:0:当前参数 1:设备名称
		unsigned	bitError:1;
		unsigned	bitLongInst:1;
		unsigned	bitTimeToBroadcast:1;
		unsigned	bitCursorToggle:1;		//光标：c,'_'		
	};
	BYTE ALLbits;	
}MSG0;	


typedef union{
	struct{
		unsigned	bitSensorChanged:1;
		unsigned	bitBtnNow:1;
//		unsigned	bitModemReady:1;			//GSM Modem ready(可发短信)
		unsigned	bitModemRstNeeded:1;
//		unsigned    bitKeyBlind:1;
//		unsigned	bitDspWaiting:1;			//flag of wait-for-keypress
		unsigned	bitPowerOnReset:1;			
		unsigned	bitCursorOn:1;
		unsigned	bitTabDigit:1;
	};
	BYTE ALLbits;	
}MSG1;


typedef union{
	struct{
		unsigned	bitLCD_PW_READY:1;
		unsigned	bitLightOnRequest:1;
		unsigned	bitRingIn:1;				//Ring in
		unsigned	bitSensorRepRequest:1;		//
		unsigned 	:1;
		unsigned 	:1;
		unsigned 	:1;
		unsigned 	:1;
		//unsigned	bitModemRstNeeded:1;
		//unsigned    bitKeyBlind:1;
		//unsigned	bitDspWaiting:1;			//flag of wait-for-keypress
		//unsigned	bitCursorOn:1;
		//unsigned	bitTabDigit:1;
	};
	BYTE ALLbits;	
}MSG2;



typedef union{
	struct{
		unsigned	LCD_SW:1;
		unsigned	LCD_LIGHT_SW:1;
		unsigned	SD_CARD_SW:1;		
		unsigned	:1;		
		unsigned 	:1;
		unsigned 	:1;
		unsigned 	:1;
		unsigned 	:1; 
	};
	BYTE ALLbits;	
}SW;

//Fatal Error
typedef union{
	struct{
		unsigned	bitERR_IIC:1;				//IIC读写错误
		unsigned	bitERR_FRAM_SAVE:1;			//FRAM读写失败
		unsigned	bitIIC_ERR2:1;
		unsigned	bitERR_ERR3:1;
		unsigned	bitERR_ERR4:1;
		unsigned	bitERR_XMEM_PROGRAM:1;		//XMEM操作错误
		unsigned	bitERR_XMEM_CLR_CX_SECTOR:1;
		unsigned	bitERR_XMEM_INIT:1;
	};
	BYTE ALLbits;	
}ERRORS;


//8 Timers
typedef union{
	struct{
		unsigned	bitWaitSensorMeasurement:1;
		unsigned 	:1;
		unsigned 	:1;
		unsigned 	:1;
		unsigned 	:1;
		unsigned 	:1;
		unsigned 	:1;
		unsigned 	:1;
	};
	BYTE ALLbits;	
}TIMERS;


//8 Timer is up
typedef union{
	struct{
		unsigned	bitWaitSensorUp:1;
		unsigned 	:1;
		unsigned 	:1;
		unsigned 	:1;
		unsigned 	:1;
		unsigned 	:1;
		unsigned 	:1;
		unsigned 	:1;
	};
	BYTE ALLbits;	
}TIMEUP;

#endif
