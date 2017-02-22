#include 	"header.h"
#include    "h_macro.h"

//配置 see in "p24fj256ga106.h"
#ifdef WATCHDOG_ON
  _CONFIG1(FWDTEN_ON & FWPSA_PR128 & WDTPS_PS256 
			& JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF  & BKBUG_OFF &ICS_PGx2)//
#else
  _CONFIG1(FWDTEN_OFF & FWPSA_PR128 & WDTPS_PS256 
			& JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF  & BKBUG_OFF &ICS_PGx2)//
#endif

_CONFIG2(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRI & IOL1WAY_OFF&FNOSC_PRIPLL )


#define	MAX_CFG			2048
#define MAX_CFGS		(MAX_CFG/lenCELL)

//==========================
//String
//==========================
const char sVer[]=sVERSION;

//默认cfg[]
#include "CFG_DEFAULT.h"		

BYTE cfg[MAX_CFG];

#ifdef  STOP_WATCH_ON
UINT stopwatch[16];			//stopwatch
#endif

//=========================================
//Bit usage variables
//=========================================
MSG0		Msg0;
MSG1		Msg1;
MSG2		Msg2;
SW			sw1;
ERRORS		Error0;
TIMERS		Tmr;
TIMEUP 		TmrUp;

volatile KEY	key;

//=========================================
//1、系统运行时，在多个任务间切换；
//2、任何时候只有一个活跃任务；
//=========================================
BYTE		Tasking[20];	//0:未启动； 1：运行中
BYTE		ActiveTask;		//当前CPU运行的活跃任务号		

//任务内阶段号
//BYTE 		stepDISP;
BYTE 		stepINT_REPORT;
BYTE 		stepDO_REPORT;
BYTE 		Step[10];			//定义见：STEPS_


//当前端口号：1-3, 只在LCD显示时为4。起始端口：1
BYTE		ActiveCom;		//1-4
BYTE		ComOpen[5];		//COM口占用标记:1-4; 为标记一致，起始端口：1
BYTE		ModemReady[5];	//ModemReady标记


//=========================================
//Normal Variables
//=========================================
volatile UINT 	ms_click,sec_click,sec_cmp;

//延时
UINT	iTimeCmp;
UINT	iIdleKey;
UINT	iWaitSensorCmp;
UINT	iRefreshDly;	

BYTE   	filter_type;
BYTE 	ComStatus;			//通信模式
BYTE	minTrans0;
BYTE 	fCfgGood;	
BYTE 	*pCmfg;
BYTE 	ComSW;


//Status
BYTE	cntGModemFailed;	//次数-GSM Modem失败
BYTE	cntSensorComFailed;	//次数-传感器通信失败
BYTE	cntMemMirrorFailed;	//次数-存储镜像错误
BYTE	cntRecFailed;
BYTE 	cntXmeFailed;
BYTE	cntLCDErr;
//BYTE 	cntRestart;			//Rst次数



//for LCD
BYTE	fLcdReady;
BYTE	fBlinkEnabled;
BYTE	ScrollIndex;		//滚动显示，自动+1; {0 ：LINES_LCD_BUF-1}
BYTE	CurX,CurY;			//LCD cursor position, start from {0,0}
BYTE 	*pFormat,*pDefault;
BYTE 	ResetCode;
int		pb_count;

SCREEN 	Srn[4];
extern SCREEN *ActiveSrn;

//for KEY
BYTE 	KeyPollEnabled;		//仅在OK按下时，启动键值轮询
BYTE 	KeyBlindCount;

//用于保存整点或整时间（如整15分，整5分）
//相对于hYear等，rTime凝固1分钟，hYear随时可能被修改或刷新
hTIME	rTime;	

BYTE	*ptr_tx;	
BYTE	Tx_Count;

//
BYTE	*ptr1_rx,rx1[RX_BUF_SIZE1];		//最大接收缓冲：32+(2+4+2048)+2=2088	
BYTE	*ptr2_rx,rx2[RX_BUF_SIZE2];
BYTE	*ptr3_rx,rx3[RX_BUF_SIZE3];		//传感器接口	
BYTE	*ptr4_rx;


//端口Belt
PORT	Port[8];			//支持最多8个基元(C0)

//传感器
BYTE	CellRequest0;
BYTE	CellRequest1;
BYTE 	IntC0[62];			//端口的中断标志，由中断处理进程刷新

BYTE 	c0Table[62];		//c0首地址表
BYTE 	cxTable[256];		//cx_off表+cx首地址表

//for IPL
extern BYTE 	IPL_CPU;

#ifdef SIM_ON
hTIME simDT;
#endif

UINT KeyV[3]={0x02AA,0x0158,0x0020};	//KEY1-3 对应的电压值


//==============================================
//定时器2
//==============================================
void __attribute__((__interrupt__,auto_psv)) _T2Interrupt(void)  
{  
	T2_IE=0;
	T2_IF=0;
	ms_click++;				//2ms/click

//timers
	if(fWaitSensorMeasure){
		if(ms_click==sec_cmp){
		  sec_cmp=ms_click+1000;				//delay 1s=1000ms
		  sec_click++;
		  if(sec_click==iWaitSensorCmp){
			fWaitSensorUp=TRUE;
			fWaitSensorMeasure=DISABLED;
		  }
		}
	}

//key AD	
	if(KeyPollEnabled==TRUE){
		if((ms_click&0x0f)==0){
		  StartKeyAD();				//定时开启KeyAD
		}

		KeyDisplay();
	}							

//cursor blink
	if(fBlinkEnabled==TRUE){
	  if((ms_click % BLINK_GAP)==0){
		fCursorToggle=1;
	  }
	}




//key blind
	/*if(fKeyBlind==TRUE){
		if((ms_click&0x0f)==0){
		  KeyBlindCount--;
		  if(KeyBlindCount==0){fKeyBlind=FALSE;}
		}
	}*/							

	T2_IE=1;
}

//====================================
// GPRS MSG数据通信模式
//====================================
void U_Rx(void)
{
BYTE *p,c; 

	CLR_RX_IF();				//RX1_IF=0;

//1、当需要接收第一个字符时（如等待响应），应首先初始化端口。
//2、外部主动发送时(如A>R，RING)，可能会丢失第一个字节
	if(IsComOpen()==FALSE){			//端口未被占用				//如果没有启动，启动通信任务
	  OpenComPort();   
	  RS232_on();
	  SetComTask();   	//Tasking[TASK_COM2_IN]=1;			
/*	  if(ActiveTask>TASK_IDLE){
		Serial_Setup(BAUD_19200);
		Serial_Transmit('B');	//ActiveTask+'0');	//Busy!
		EndCom();
	  }		*/
	}
	else{
	  while(URXDA()){
		p=getRxPtr();
		c=URXREG();
        *p=c;		//URXREG();
	    if(p>=RX_BUF_END())goto SER_RX_END;	

        //Filter for DIRECT
        if(filter_type==FILTER_DIRECT){					
	      if(p==RX_BUF0()){			
	        if(*p!=INST_HEADER)goto SER_RX_END;	
	      }
        }
		
		else if(filter_type==FILTER_NIHAO){
		  if(p==RX_BUF0()){			
	        if(*p!='H')goto SER_RX_END;		//"NI/HAO"	
	      }
		}

	    //Filter for USB-disk
	    else if(filter_type==FILTER_SLIP){
	      if(p==RX_BUF0()){			
	        if(*p!=0xC0)goto SER_RX_END;	
	      }
	      if(*p==0xE0){
		    RX_IE_OFF();		//RX2_IE=0;	
		    goto SER_RX_END;
	      }
	    }

		//p++
	    IncPtrRx();			
	  }
	}

SER_RX_END:
	Nop();
}


//==============================================
//COM1: GPRS模块通信接收/透传
//==============================================
void __attribute__((__interrupt__,auto_psv)) _U1RXInterrupt(void)  
{
BYTE save;

	save=ActiveCom;
  	ActiveCom=COM1;

	U_Rx();

	ActiveCom=save;
}


//==============================================
//Com2 for DIRECT/USB-disk
//==============================================
void __attribute__((__interrupt__,auto_psv)) _U2RXInterrupt(void)  
{  
BYTE save;

	save=ActiveCom;

//DEBUG
//  	ActiveCom=COM1;
//	RS232_on();
//	Serial_Setup(BAUD_19200);
//	Serial_Transmit('B');	//ActiveTask+'0');	//Busy!
//	EndCom();


  	ActiveCom=COM2;

	U_Rx();

	ActiveCom=save;
}

//==============================================
//Com3 for Sensor,Beidou
//==============================================
void __attribute__((__interrupt__,auto_psv)) _U3RXInterrupt(void)
{
BYTE save;

	save=ActiveCom;
  	ActiveCom=COM3;

	U_Rx();

	ActiveCom=save;
}

//==============================================
//Com1发送
//==============================================
void __attribute__((__interrupt__,auto_psv)) _U1TXInterrupt(void)  
{  
	if(TX1_IF){		//interrupt		
	  if(--Tx_Count==0){			
	    TX1_IE=FALSE; 		//发送完毕标志			
	  }								
	  S1_Transmit(*ptr_tx++);		
	}								
}

//==============================================
//Com2发送
//==============================================
void __attribute__((__interrupt__,auto_psv)) _U2TXInterrupt(void)  
{  
	if(TX2_IF){		//interrupt		
	  if(--Tx_Count==0){			
	    TX2_IE=FALSE; 		//发送完毕标志;			
	  }								
	  S2_Transmit(*ptr_tx++);		
	}								
}

//==============================================
// LCD通信接收
//==============================================
void __attribute__((__interrupt__,auto_psv)) _U4RXInterrupt(void)  
{  
//	RX4_IF=0;	//interrupt
//    *ptr4=U4RXREG;
//	if(ptr4>=BUF4_END)goto SER_RX_END;	
//	ptr4++;	
//
//SER_RX_END:
//	Nop();
}

//==============================================
//Calendar 1Min int.
//==============================================
void __attribute__((__interrupt__,auto_psv)) _INT1Interrupt(void)     //边沿触发           
{
//BYTE save_T2IE,save_T2ON;


#ifdef SIM_ON
	AddMinute(&simDT,1);		//当前时间加1分钟
#endif

	CALENDAR_IF=0;
	CALENDAR_IE=FALSE;

//	saveT2IE=T2_IE;
//	saveT2ON=T2_ON;

//	Timer2_on();	//turn on TMR2

//?????
//	KillC0s();

	if(swSD_CARD){
	  PW_SD_on();
	}

	if(testbit(ComSW,bitSW_REPORT)){
	  Tasking[TASK_CALENDAR_INT_REP]=1;	//预设自报
	  stepINT_REPORT=0;
	}

	CalendarProc();				//实测耗时：(70ms/Cx) X 记录cx个数

//	T2_IE=saveT2IE;
//	T2_ON=saveT2ON;
}


//==============================================
// Key wakeup
// 这里采用的是电平变化中断。也可以采用外部中断口的形式。
//（使用外部中断口，需要对 外设引脚赋予外部中断口功能即可。此功能稍后更近）
// 雨量计数中断AND  key,外部定时器中断
//==============================================
void __attribute__((__interrupt__,auto_psv)) _CNInterrupt(void)     //边沿触发           
{
    _CNIF=0;              	//清零中断标志位,
	if(btnKey==0){			//按下OK：button-press is  a valid wakeup, while depress not
	  swBUZZ=OFF;			

	  //键盘
	  fBtnNow=1;			//save btn status

	  key.val=KEY4;
	  key.count=KEY_CONFIRM;

	  //显示
	  PW_LCD_on();			//turn 33V_NOFF for LCD
	  Tasking[TASK_LCD_DISP]=1;
	  iIdleKey=ms_click+WAIT_FOR_DSP;		//sleep after time is up
//	  stepDISP=0;
	  if(swLCD_LIGHT==OFF){fLightOnRequest=TRUE;}		

	  swBUZZ=OFF;
	  KeyPollEnabled=TRUE;	  	//启动
	}
}

/*====================================================================
_ADC1Interrupt(): ISR name is chosen from the device linker script.
====================================================================*/
void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)
{ 
UINT uk;
BYTE i;

	AD_IF=0;				//Clear the ADC1 Interrupt Flag

	if(key.val==KEY4){		//OK键，优先处理
	  return;
	}		

//	while(!_DONE)continue;
	uk=ADC1BUF0;
	_ADON=0;		//shut off A/D	
	T3_ON=0;

	if(key.count==0){
	  for(i=0;i<3;i++){			//轮询KEY1-3
	    if(Abs(uk,KeyV[i])<=KEY_WIDTH){			//键值比较
		  key.val=i+1; 
		  key.count++; 
		  return;
		}
	  }
	  key.val=KEY_NULL; 
	  key.count++;
	}
	else{
	  if(key.val){
		//KEY_NULL
	    if(key.val==KEY_NULL){
		  key.count++;
		  return;
	    }
 
		//KEY1-3
	    if(Abs(uk,KeyV[key.val-1])<=KEY_WIDTH){		//键值比较
		  if(key.count<KEY_CONFIRM){key.count++;}
		  return;
	    }
	    else{
		  key.val=KEY_UNKNOW;
		  key.count=0;
		  return;
	    }
	  }
	}
}


//==============================================
// Rain 
//==============================================
void __attribute__((__interrupt__,auto_psv)) _INT2Interrupt(void)     //边沿触发           
{
	if(RAIN_IF){
	  RAIN_IF=0;
	  IncRain();
	  fSrnRefresh=TRUE;		//屏幕刷新

	  if(EventRain()){
	    Tasking[TASK_SENSOR_INT_REP]=1;
	  }
	}
}

//==============================================
// GPRS RING
//==============================================
void __attribute__((__interrupt__,auto_psv)) _INT3Interrupt(void)     //边沿触发           
{
	if(GPRS_Ring_IF){
	  GPRS_Ring_IF=0;
//	  fRingIn=TRUE;	
	}
}


//=================================================
//                      LVD
//
// 1、如果在记录时，出现LVD中断。由于记录与本例程的
// IPL一致，所以，将等待记录完成后复位。这样可以
// 避免立即退出记录过程，对记录可能的损坏。
// 2、这里假定欠压发生到系统无法工作有短暂延时。
//=================================================
void __attribute__((__interrupt__,auto_psv)) _LVDInterrupt(void)                
{
	_LVDIF=0;

//
/*	PW_NOFF_off();
	PW_NON_off(); 
	blight_off();					//关闭LCD_LED
	LED_STATUS_off();
//
	while(1);
*/
}


#define _trapISR __attribute__((interrupt,no_auto_psv))

/* ****************************************************************
* Standard Exception Vector handlers if ALTIVT (INTCON2<15>) = 0  *
*                                                                 *
* Not required for labs but good to always include                *
******************************************************************/
void _trapISR _OscillatorFail(void)
{

        INTCON1bits.OSCFAIL = 0;
        while(1);
}

void _trapISR _AddressError(void)
{

        INTCON1bits.ADDRERR = 0;
        while(1);
}

void _trapISR _StackError(void)
{

        INTCON1bits.STKERR = 0;
        while(1);
}

void _trapISR _MathError(void)
{

        INTCON1bits.MATHERR = 0;
        while(1);
}



//==============================================
//Com2 for DIRECT/USB-disk
//==============================================
/*
void __attribute__((__interrupt__,auto_psv)) _U2RXInterrupt(void)  
{  
	RX2_IF=0;		
	if(task_Comm2Int==0){	//如果没有启动，启动通信任务
	  if(ActiveTask==TASK_IDLE){
	    task_Comm2Int=1;
	    RS232_on();			//SLEEP232_DIR=OUT; SLEEP232=1;	//RS232 wake up
	  }
	  else{
		Serial_Transmit(ActiveTask);	//Busy!
	  }
	}

	else{
	  while(U2STAbits.URXDA){
        *ptr_rx=U2RXREG;
	    if(ptr_rx>=BUF_RX_END)goto SER_RX_END;	

        //Filter for DIRECT
        if(filter_type==FILTER_DIRECT){					
	      if(ptr_rx==BUF_RX_BEG){			
	        if(*ptr_rx!=INST_HEADER)goto SER_RX_END;	
	      }
        }
	    //Filter for USB-disk
	    else if(filter_type==FILTER_SLIP){
	      if(ptr_rx==BUF_RX_BEG){			
	        if(*ptr_rx!=0xC0)goto SER_RX_END;	
	      }
	      if(*ptr_rx==0xE0){
		    RX_IE_OFF();		//RX2_IE=0;	
		    goto SER_RX_END;
	      }
	    }
	    ptr_rx++;	
	  }
	}

SER_RX_END:
	Nop();		
}
*/

//==============================================
// Sensor通信接收
//==============================================
/*
void __attribute__((__interrupt__,auto_psv)) _U3RXInterrupt(void)  
{  
	RX3_IF=0;	
	if(task_SensorInt==0){	//如果没有启动，启动通信任务
	  task_SensorInt=1;
	  fWaitSensorUp=TRUE;
	  fWaitSensorMeasure=DISABLED;
	  fSensorChanged=1;

	  ptr_rx3=&rx3[0];
	}
							
	else{
      *ptr_rx3=U3RXREG;
	  if(ptr_rx3>=BUF3_END)goto SER_RX_END;	
	  ptr_rx3++;	
	}
SER_RX_END:
	Nop();
}*/


//=============================================
// GPRS透传模式
//=============================================
/*
void Urx_GPRS_TRANS(void)
{
BYTE *ptr_rx; 

	CLR_RX_IF();		//RX1_IF=0;
	if(task_Comm1Int==0){	//如果没有启动，启动通信任务
	  if(ActiveTask==TASK_IDLE){
	    task_Comm1Int=1;
	    RS232_on();			//SLEEP232_DIR=OUT; SLEEP232=1;	//RS232 wake up
	  }
	  else{
		Serial_Transmit(ActiveTask);	//Busy!
	  }
	}

	else{
	  ptr_rx=getRxPtr();
	  while(URXDA()){
   	    *ptr_rx=URXREG();
	    if(ptr_rx>=BUF_RX_END)goto SER_RX_END2;

        //Filter for DIRECT
        if(filter_type==FILTER_DIRECT){					
	      if(ptr_rx==BUF_RX_BEG){			
	        if(*ptr_rx!=INST_HEADER)goto SER_RX_END2;	
	      }
        }
	    //Filter for USB-disk
	    else if(filter_type==FILTER_SLIP){
	      if(ptr_rx==BUF_RX_BEG){			
	        if(*ptr_rx!=0xC0)goto SER_RX_END2;	
	      }
	      if(*ptr_rx==0xE0){
		    RX_IE_OFF();		//RX2_IE=0;	
		    goto SER_RX_END2;
	      }
	    }
	    ptr_rx++;	
	  }
	}

SER_RX_END2:
	Nop();		
}
*/


//===========================================================================================================



//==============================================
//end com. task;
//==============================================
void EndCom(void)
{
	while(!TRMT()){continue;}

	CloseComPort();			//关闭端口	
	RS232_off();

    Serial_End_Tx();				
	CLR_RX_IF();		//清除通信期间留下的中断标志
	RX_IE_ON();

//	SwitchToComport(PORT_SLEEP);	//切换可能导致Rx跳变,误触发新的通信.
	Delay_ms(200);

//	ActiveCom=COM1;
//    ei_all();       
}


void SwitchToComport(BYTE port)
{
	switch(port){
	case PORT_SLEEP:
	  TX1_DIR=OUT; 

//	  TX1=0;	//虽然省电,但可能会导致输出一个错误TX,Modem可能会返回错误.
	  TX1=1;	//会增加0.2mA电流,但无错误输出.
//
	  RX1_DIR=IN;  RX1=1;	//RX1_DIR=OUT; RX1=0; for old board
	  break;
    case 1:
	  PW_NOFF_on();
  	  break;
    case 2:
	  PW_NOFF_on();
  	  break;
	}  
}

//=====================================
// 如果有传感器请求，作标记
//=====================================
/*
void MarkSensorRequest(BYTE *des)
{
	if(testbit(*(des+PARA2),7)){					//检测到传感器请求
	  SetCfgBit(REG0+(UINT)MAIN_PROPERTY2,bitSENSOR_REPORT_REQUEST);
	  CellRequest0=0;
	  CellRequest1=0;
	}
}
*/


//===============================
//  belt中填入“错误标志”
//===============================
void ClrBelt(BYTE *des,BYTE belt_len)
{
BYTE i;
	for(i=0;i<belt_len;i++){
	  if(i==0){*des++=ERR_PORT_ACCESS;}
	  else{*des++=(ERR_PORT_ACCESS&0x40);}		//高位清零
	}
}	

//========================================
// Instruct sensor to start measurement
//========================================
//return: delay time needed
//0: failed to comm. of course no delay needed
//non-zero: delay time

BYTE DoMeasurement(BYTE iPort,BYTE *des,BYTE des_limit,BYTE speed_mode)
{
BYTE i,inst[16];
UINT sum;

	FillBufWith(&inst[0],16,0x0);

	ConnectCell(iPort);
	Delay_ms(10);
	
	inst[FRM_VER]=INST_HEADER;
	inst[FRM_LEN]=lenHEAD-1;
//	inst[FRM_DES_ADDR]=iPort; 
//	inst[FRM_DES_ADDR+1]=0;
	//inst[FRM_STA_ID]=0;		默认值
	inst[FRM_OP]=OP_DO_MEASUREMENT;
	//inst[FRM_PROPERTY]=0;
	//inst[FRM_INST_INDEX]=0;
	sum=GetCRC(&inst[0],lenHEAD-2);
	inst[HCRC_L]=LoOfInt(sum);
	inst[HCRC_H]=HiOfInt(sum);
//
	i=RWSerialSensor(&inst[0],inst[FRM_LEN]+1,des,des_limit,speed_mode);
	if(i==OK){
	  return inst[FRM_DELAY_NEEDED];		//返回：delay time needed
	}
	else{ 
	  return 0xff;
	}
}

//=================================
// Start all sensor's measurement 
//
// return: 
//	max_delay={0-254}
//  0xff: failed
//=================================
BYTE StartSensors(void)
{
BYTE iC0,port_delay,max_delay;	
UINT c0;	//c0_start;
BYTE resp[16];

//	ptr3=&p3[0];
//	ReadBelt(0);		//mainboard-self	

//quary if port delay is necessary ?
    iC0=0;
    max_delay=0;
	//addrC0=REG0+((UINT)(CM_C0_START)*lenCELL);				//指向C0首地址
    while(iC0<C0S){
	  //c0=((UINT)c0Table[iC0])*lenCELL;
	  c0=getC0(iC0);
	  port_delay=cfg[c0+C0_MAX_DELAY];
	  port_delay&=0x3f;
	  if(port_delay!=0){
		if(IsSensorReady()){
		  //ReadBelt(i,&resp[0],16);		//resp_limit=16
		  DoMeasurement(iC0,&resp[0],16,MODE_NORMAL);		//返回数组中含有"延时"参数,可以利用也可不用.
		}
	  }	//to start the port sensor
	  if(port_delay>max_delay){max_delay=port_delay;}
	  
	  iC0++;
    }
//    
	return max_delay;
}

//====================================================================
// Read all belts and join together in *desBelts
//
// speed_mode: 
//	0: 快速模式，不等待慢速传感器完成当前测量；
//  1: 慢速模式，等待传感器完成当前测量；
// return:
//       belts的总长度
//	     0: 失败
//====================================================================
void PrePortReading(void)
{
//BYTE max_delay;

	PW_NOFF_on();	//开启PW_NOFF（可能为传感器供电），直到全部Belt读完。

//  max_delay=StartSensors();

//取消等待，要求: 
//			1)针对快速传感器，能快速响应。
//			2)慢速的传感器要求自供电,并自主决定采样间隔。
//  后续可考虑：采样由时间条件控制（如TCTL GGDD #1。定时控制 间隔 #1控制方案）
//
//
//  WaitSensorEcho(max_delay);		
}

void PostPortReading(void)
{
	if(swLCD_LIGHT==OFF){	//当背光处于开启状态时，认为人工介入状态，PW_NOFF维持开启。
	  PW_NOFF_off();		//全部Belt读完后，关闭PW_NOFF
	}
}



BYTE SensorIntReports(void)
{
//  IsSensor

  return 1;
}



BYTE IsRxOver(void)
{
BYTE *p,*inst,*pEnd;

	p=getRxPtr();
	inst=RX_BUF0();

//Long instruction
	if(fLongInst){
	  pEnd=inst+getUINT(inst+FRM_LEN)+1;
	  if(p<pEnd)return RX_WAIT;		//	  if(p<(&rx[0]+getUINT(&rx[FRM_LEN])+1))return RX_WAIT;	
	  else{				//Task over
	    RX_IE_OFF();	

	    if(CheckCRC(inst+DATA0,getUINT(inst+FRM_LEN)-lenHEAD-1)){		//
	      return RX_OVER_OK;
	    }
	    else{
		  RX_IE_OFF(); 
		  return RX_OVER_WITH_ERR;
		}
	  }
	}

//Short instruction
	else{
	  if(p>=(inst+lenSHORT)){	
	    if(CheckCRC(inst,lenSHORT-2)){
	      if(getUINT(inst+FRM_LEN)==lenSHORT-1){
		//短指令
			RX_IE_OFF();	
	        return RX_OVER_OK;
	      }
	      else{
	        fLongInst=1;
		    return RX_WAIT;
	      }
	    }
  	    
	    else{
	      RX_IE_OFF();		
      	  return RX_OVER_WITH_ERR;
	    } 
	  }
	  else{ 
		return RX_WAIT;
	  }
	}


//	return RX_WAIT;

}


void TriggerPort(BYTE PortIndex)
{
/*	switch(PortIndex)
	{
	case 0:
	  break;		//used when basic cell in addressed.

//Port selection.
	case SPI_ID_1:
//	  M_SPI_PORT_CS1=0; NOP; NOP; NOP; NOP; NOP;
//	  M_SPI_PORT_CS1=1;Delay_ms(1);// Delay100TCYx(1);	//35us	//Delay_ms(1);	  
//	  M_SPI_PORT_CS1=0;
	  break;
	case SPI_ID_2:
//	  CS_PORT2=0；
//	  CS_PORT2=1;
	  NOP;
	  break;
	case SPI_ID_3:
//	  CS_PORT3=0；
//	  CS_PORT3=1;
	  NOP;
	  break;
	case SPI_ID_4:
//	  CS_PORT4=0；
//	  CS_PORT4=1;
	  NOP;
	  break;

	default: Restart(RST_FOR_ERR_MAIN_1);
	}*/
}


void ConnectCell(BYTE PortID)
{
	TriggerPort(PortID);
	if(PortID==0){
	  return;				//mainboard-self
	}	
	else if((PortID==1)||(PortID==2)||(PortID==3)||(PortID==4))
	{
	  PW_NOFF_on();
	  //Delay_ms(200);
	}
	else{
	  Restart(RST_FOR_ERR_MAIN_2);
	}
}


BYTE Inst_Analysis(BYTE connection_type)
{
BYTE i,saveCALENDAR_IE;
ULNG InstAddr;
ULNG StaID;	//取StaID的低Word，用于现场485联网。其余通信方式采用ADDR_CARELESS。
BYTE f,c,fc;
BYTE r;
PROPERTY property;
BYTE *inst;

//sw[0]=ms_click;

	property.ALLbits=0;
	property.bitINST_DIR=0;				//响应
	property.bitANSWER_NEEDED=0;		//不需要回复。通信方式可覆盖本属性；

	inst=RX_BUF0();
	if(*inst!=INST_HEADER){return FAILED;}

//Address
	StaID=getULNG(&cfg[CM_STA_ID]);

//	if(!testbit(rx[FRM_PROPERTY+1],bitDES_CHECK)

	InstAddr=getULNG(inst+FRM_SRC_ADDR);
InstAddr=ADDR_CARELESS;	//!!!

	if((InstAddr==ADDR_CARELESS)		//万能地址，用于广播或检测工具。
	  ||(InstAddr==StaID)){			//指定地址，[0:0xffff],用于485等需寻址应用。
	  switch(*(inst+FRM_OP)){
	    case OP_READ_BELT:
//		case OP_READ_RANDOM:
		  if(*(inst+FRM_OP)==OP_READ_BELT){		//最新Belt数据
			f=*(inst+TSEN_BELT_FMT_ID)&0x0f;
			c=6;							//DIRECT_2
			fc=(f<<4)|c;
//			if(stepDO_REPORT){return BUSY;}  可能此时并通道等资源未被占用
			stepDO_REPORT=0;
		    do{
			  r=DoReport(fc,property.ALLbits);
			}while(r==0xff);
//sw[1]=ms_click;
		  }
		  else{									//历史数据
			//i=InstQuery(&rx[0]);
			i=1;	//暂时取消“随机查询”

		    if(i==FAILED){
			//  MakeTextMsg("No Result Matched");
		    }
		    else{
//			  MakeQueryMsg(rx[PARA1],i);			//虚元号,REC_OR_MEM
		    }
		  }
		  break; 
//下载
        case OP_DOWNLOAD_MAP:
		  saveCALENDAR_IE=CALENDAR_IE;				
		  CALENDAR_IE=FALSE;			//关闭日历时钟中断
          i=DownLoadMap(CONNECT_DIRECT,inst);
		  CALENDAR_IE=saveCALENDAR_IE;	//恢复原状态
		  return ShowDownResult(i);		//“认证失败”或“写注册后Reset” 
		  
		  //return STEP_RESET;
		  //break;
//上载
        case OP_UPLOAD_MAP:				
          UpLoadMap(CONNECT_DIRECT);
		  break;

//读历史记录
//		case OP_READ_RECS:
//		  UpLoadSDRecs(inst);
//		  break;


/*        case OP_READ_BLOCK:					//GPRS方式通信
		  xmPage=getUINT(&rx[PARA2]);
		  MapType=rx[PARA1];
		  MakeMapMsg(MapType,xmPage);	
		  Reports(NO_TIME_FIT,256);
		  break;
*/
        case OP_REPEAT:
		  RepeatSM(inst+DATA0);
		  break;

	    default: Restart(RST_FOR_ERR_MAIN_3);
	  }
	}

//Real-address
	else{
/*	  i=rx[3]& MASK_PORT;		//0b00011111;		//i: port index;
	  switch(i){
	    case 0:
	      fBasicBeRead=1;
	      return STEP_OVER_ONE;
	    case 1:		//-15: from 1 to 15
	      //bit
	      ConnectCell(i);
	      return STEP_OVER_ONE;
	    case 16:
//	      BasicBeWritten();
	      return STEP_ONLY_ONE;

    	default: Restart(RST_FOR_ERR_MAIN_4);
	  } */
	}
	return 1;
}

//==================================
// Download后，显示执行结果
// 返回：是否重启，Reset
//==================================
BYTE ShowDownResult(BYTE result)
{
//开启背光
	blight_on();

//
	if(result==STEP_AUTHEN_FAILURE){
	  MsgBox("身份验证失败......        ");
	  Delay_s(1);
      return FAILED;
	}

	if(result==OK){
	  MsgBox("写注册..............成功");
	}
	else{
	  MsgBox("写注册..............失败");
	  //return FAILED;
	}

//???视情况决定是否...
	if(1){
      lcd_puts(0,L1,"初始化Modem...          ");       ///
	  if(InitAllComTasks()==OK){
        lcd_puts(0,L1,"初始化Modem.........完成");       ///
	  }
	  else{
        lcd_puts(0,L1,"初始化Modem.........失败");       ///
	  }
	}
//
	Delay_s(5);
	return STEP_RESET;		//是否重启？
}

//=============================================
//Output buffer string to comm. port
//return: len of buffer
//=============================================
/*
BYTE OutputBuf(BYTE response_com_type,BYTE *buff0,BYTE len)	//des. is define in channel object
{

	switch(response_com_type){

	  case CONNECT_DIRECT:			//透明
	    return len;

	  case CONNECT_USWAVE:		//超短波
	    return FormatUltrashort(buff0,len);

	  case CONNECT_PSTN:			//电话网
	    return FormatPSTN(buff0,len);

//	  case CONNECT_GSM_DT:			//GSM数传
//	    return FormatPSTN(buff0,len);

	  case CONNECT_SM:			//GSM短消息
	    return FormatSM(buff0,len);

	  default: Restart(RST_FOR_ERR_MAIN_5);
	}
	return 0;
}*/

//Add respective ultrashort comm. header
UINT FormatUltrashort(BYTE *buff0,BYTE len)
{return 0;}


//Add respective short message comm. header
UINT FormatSM(BYTE *buff0,BYTE len)
{return 0;}

UINT FormatPSTN(BYTE *buff0,BYTE len)
{return 0;}


BYTE StripOffComHeader(BYTE connection_type)
{
	switch(connection_type){

	  case CONNECT_PSTN:
	  	return 1;

//	  case CONNECT_GSM_DT:
//	    	return 1;

	  case CONNECT_DIRECT:
	  	return 1;

	  case CONNECT_SM:
//		do strip
	  	return 1;

	  case CONNECT_USWAVE:
	  	return 1;

	  case CONNECT_WIFI:
	  	return 1;

	  default: Restart(RST_FOR_ERR_MAIN_6);
	}
	return 1;
}


//======================================
//
//用于被查询方式，不包括自报
//
//======================================
BYTE Connect(BYTE connection_type)
{
//BYTE r;

	RS232_on();			//SLEEP232_DIR=OUT; SLEEP232=1;				//RS232 wake up serial

	switch(connection_type){
	  case CONNECT_NULL:
	    Serial_Setup(BAUD_19200);
		break;

	  case CONNECT_DIRECT:
	    Serial_Setup(BAUD_19200);
	    Serial_Transmit('R');
	    break;

	  case CONNECT_USWAVE:
	    Serial_Setup(BAUD_300);
	    Serial_Transmit('R');
//		exchange some leading chars.
	  	break;

	  case CONNECT_SM:
	  case CONNECT_GPRS:
	  case CONNECT_GPRM:
	    if(IsModemReady()==FALSE){			//检查Modem状态
	      AT_creg(ActiveCom);	//马上刷新，保证时钟停振时，也能响应	
	      if(IsModemReady()==FALSE){
		    return FAILED;			
		  }
	    }
		break;

	  case CONNECT_BEIDOU:
//	    Serial_Setup(BAUD_19200);
	    Serial_Setup(BAUD_9600);
		break;

	  case CONNECT_PSTN:
	    Serial_Setup(BAUD_19200);
	    Serial_Transmit('R');
//		OffHook;
	  	break;

	  case CONNECT_WIFI:
		PW_NOFF_on();			//WIFI模块上电
		Delay_s(2);
		//Serial_Setup(BAUD_115200);
		break;

	  default: Restart(RST_FOR_ERR_MAIN_7);
	}
	return OK;
}

//======================================
//
// Com1 被动通信
// 用于被查询方式，不包括自报
//
//======================================
void Com1Proc(void)
{
BYTE com_type;

//	ActiveTask=TASK_COM1_IN;
//	ActiveCom=COM1;

//透传模式
	if(ComStatus==STATUS_GPRS_TRANS){
	  DirectAccess();
	  return;
	}

//常规模式
//	com_type=GetComType(ActiveCom);			//读取队列

//临时
com_type=CONNECT_SM;
	if(com_type==CONNECT_ERROR){goto COM_OVER;}		//对应通道没有任务

//NEXT_COMM:	
	switch(com_type){
//	case CONNECT_DIRECT:
//      Com_ConnectionMode(channel);
//	  	break;
//    case CONNECT_GSM_DT:
//      GSM_DT_ConnectionMode();
//	  	break;
    case CONNECT_SM:  	//SM mode      
	case CONNECT_GPRM:     	//GPRS ring-echo mode      
		if(IsRingIn()==TRUE){
		  RingReport(ActiveCom);
			//di();len=ReadBelts(&p3[0],&p3[255],MODE_NORMAL);ei();		//belts in p3[]
			//MakeMsg(0,1,&p3[0]);
			//Reports(NO_TIME_FIT,0);		//lenExp
      	}
		else{
          SM_process();           //read sm,analyse,do
		  Delay_s(1);				//防止返回信息触发重入
		}
		break;
	default:
		break;
	}

COM_OVER:
	Tasking[ActiveTask]=0;	//关闭当前任务
	ActiveTask=TASK_IDLE;	//释放任务权柄
	EndCom();
	ei_all();
}	    

void Com2Proc(void)
{
   	DirectAccess();
//	ActiveTask=0;	//释放任务权柄
}

BYTE RingReport(BYTE COMi)
{
	return 1;
}


void WaitSensorEcho(BYTE max_delay)
{
	if(max_delay>10){max_delay=10;}		//limit to max_delay

	if(max_delay==0){
	  //fWaitSensorUp=TRUE;
	  fWaitSensorMeasure=DISABLED;	
	  return;
	}	

	Delay_s(max_delay);
}



//====================================
// For DIRECT/TRANSPARANT mode:
//
// 被动通信
// 用于被查询方式，不包括自报
//
// 调用前明确：ActiveCom
//======================================
void DirectAccess(void)
{
BYTE t,com_type;
UINT EndMs;

//重设"透传"起始Second
//	if(ComStatus==STATUS_GPRS_TRANS){minTrans0=GetMinute();}

	PW_NOFF_on();			
	if(RxPinLocked()==TRUE){
	  goto DIRECT_ACCESS_OVER;	//后续处理只针对下降脉冲触发。对长时间拉低不响应。
	}

//debug mode
	com_type=CONNECT_DIRECT;		 	//debug mode

//
	Serial_Transmit('R');
	EndMs=InitRx(2500,FILTER_DIRECT);	//2.5s, 最大延时
//
	do{
	  ClrWdt();
	  t=DoReceive(EndMs);
	}while(t==RX_WAIT);

	RX_IE_OFF();

	if((t==RX_OVER_TIME_OVERFLOW)
	||(t==RX_OVER_WITH_ERR)
	||(t!=RX_OVER_OK)){
	  goto DIRECT_ACCESS_OVER;
	}			//Rx OK!

//	if(!StripOffComHeader(com_type)){return;}		
    t=Inst_Analysis(com_type);

	if(t==STEP_RESET){Restart(RST_FOR_MAIN_90);}	//正常重起
	    
DIRECT_ACCESS_OVER:
	Tasking[ActiveTask]=0;	//关闭当前任务
	ActiveTask=TASK_IDLE;	//释放任务权柄
	EndCom();
	ei_all();
}

//============================
// 判断Rx是否被强制拉低
//============================
BYTE RxPinLocked(void)
{
	Delay_ms(2);		//if the specific pin is not in low state long enough, 

	if(ActiveCom==COM1){
	  if(RX1==0){return TRUE;}
	}
	else if(ActiveCom==COM2){
	  if(RX2==0){return TRUE;}
	}

//	if(!RX2){			//get into debug mode
//	  goto COM2_OVER;	//后续处理只针对下降脉冲触发。对长时间拉低不响应。
//	}	
	return FALSE;
}


void ChargeManage(void)
{
//	CTL_CHARGE=1;
}

void CheckCfgValidation(void)
{
//1. Check if station is empty.
}

//===================================
// Sleep前电源准备
//
// 防止启闭电源带来的冲击
// 外设电源应在1s内降到安全电压下。
//===================================
void PowerBeforeSleep(void)
{
//置输入,防止暂态冲击
	LCD_TX_DIR=IN;			
	LCD_RX_DIR=IN;
	LCD_TX=1;
	LCD_RX=1;

	di_all();				

	PW_NON_on(); 
	PW_NOFF_off();

	PW_LCD_off();	//turn off 33V_NOFF for LCD
	PW_SD_off();

	Delay_s(1);		
	//等待电源稳定关断。
	//外设电源关断时，状态不确定，可能成为触发源)

	ei_all();

//置输出低，低功耗	
	COM4_EN=FALSE;			//关闭COM4
	LCD_TX_DIR=OUT;			//省电
	LCD_RX_DIR=OUT;
	//Delay_ms(100);
	while(LCD_TX==1){LCD_TX=0;}
	LCD_RX=0;
}


void StartScreen(void)
{
BYTE msg[10];
SCREEN *SrnSave;


#ifdef SIM_ON
	return;
#endif

	SrnSave=ActiveSrn;
	ActiveSrn=&Srn[SCREEN_STATUS];

	if(fPowerOnReset){
	  Scroll(WAY_CRLF," >加电启动...");
	}
	else{
	  Scroll(WAY_CRLF," >软复位...");
	  btoa(ResetCode,(char *)&msg[0]);
	  Scroll(WAY_CATCH,(const char *)&msg[0]);
	}

	LCDShow();

	ActiveSrn=SrnSave;
	Delay_s(1);
}

/*
//
	mark=0;
//非上电复位 
	if(!_POR){
	  lcd_puts(0,L0,"start(1)...             ");
	  //lcd_cursor(3,L0); lcd_puts("1");		//第一行：一般复位导致的重启
	  if(Error0.ALLbits){
		btoa(Error0.ALLbits,(char *)&msg[0]);
		lcd_puts(0,L1,(const char *)&msg[0]);		//第二行：显示错误类型
	  }
	  btoa(ResetCode,(char *)&msg[0]);
	  lcd_puts(0,L2,(const char *)&msg[0]);		//第三行：显示复位原因
	  Delay_s(2);
	}		

//上电复位
	else{
	  lcd_puts(0,L0,"start(0)...             ");			//第一行：上电复位导致的重启
//	  setbit(mark,bitPWR_ON);				//作“上电”标记       
	  setbit0(mark);						//作“上电”标记       
	}
	Error0.ALLbits=0;
*/

//Id是否被重新设置？
/*	mem_ReadData(M_CFG,REG0+(UINT)CM_STA_ID,(BYTE *)&iNodeAddress,2);	
	mem_ReadData(M_CFG,(UINT)MAP_ID_MIRROR,(BYTE *)&iID_mirror,2);	
	if(iNodeAddress!=iID_mirror){
	  setbit(mark,bitID_CHANGED);
	}

	mem_WriteData(M_CFG,&mark,(UINT)MAP_MARK,1);	
	WriteCfg(mark,MAP_MARK);		*/
//}


void SystemGuard(void)
{
  	RX1_IE=TRUE; 				
    RX2_IE=TRUE; 				

	if(fCfgGood){
 	  CALENDAR_IE=TRUE; 	
	}			

	_LVDIE=ENABLED;

//	RB_IE=TRUE; 注释				
//	RB_IE=FALSE; 				

	ei_all();       			
}

//=====================================
// 形成c0Table表：
//         		C1 addr		//首地址
//         		C2 addr
//
// 形成cxTable表：
//         		C1 offset		
//         		C2 offset	//offset均从表头0算起
//         		...
//         		C1.1 addr
//         		C1.2 addr
//         		...
//         		C2.1 addr
//         		C2.2 addr
//         		...
//
// 1、c0Table[]、cxTable[]以CELL为单位
// 2、cfg[0]对应的起始地址为：0
//=====================================
void GetCTable(void)
{
BYTE *t,offset;	
BYTE c0_num,len;
BYTE iC0,iCx;
BYTE CXS;
BYTE cCX;
BYTE *pC0;

//c0Table[], 排列C0首地址
	t=&c0Table[0];
	pC0=&cfg[CM_C0_START]; 
	*t++=cfg[CM_C0_START];
	for(iC0=1;iC0<C0S;iC0++){
	  *t=cfg[(*pC0)*lenCELL+C0_NEXT_START];		//C0 start
	  pC0=t;
	  t++;
	}


//排列Cx的offset
	c0_num=C0S;
	t=&cxTable[c0_num];
	offset=c0_num;

//排列Cx首地址
	for(iC0=0;iC0<c0_num;iC0++){
	  cxTable[iC0]=offset;
	  CXS=cfg[c0Table[iC0]*lenCELL+C0_CXS];		//虚元数
	  //
	  len=cfg[c0Table[iC0]*lenCELL+FLD_LEN];	//基元长度
	  cCX=c0Table[iC0]+len;						//第1个虚元的起始地址
	  for(iCx=0;iCx<CXS;iCx++){
	    *t++=cCX;				//CX start
	    cCX+=cfg[cCX*lenCELL+FLD_LEN];
		offset++;
	  }
	}
}


//=====================================
// 如uSrc指向的CFG校验正确，加载至cfg[];
// 如校验失败，cfg[]不变
//
// 返回：0：失败；1 成功
//=====================================
BYTE LoadCfgIfOk(UINT uSrc)
{
BYTE cfgs;
BYTE i,*p;
UINT sum,def_len;		
BYTE cfg_bak[64*3];		//暂时保存默认cfg[]

#ifdef SIM_ON
	return OK;
#endif

	def_len=64*3;
	memcpy(&cfg_bak[0],&cfg[0],def_len);	//cfg[]  ->  cfg_bak[]

//从FRAM加载至cfg[]
	cfgs=GetByte(M_CFG,(UINT)(CM_LEN_CFG));
	if(cfgs>MAX_CFGS){cfgs=MAX_CFGS;}					//限制CFG总长度 <(32*64)= 2048
	if(cfgs<=3){goto USE_DEFAULT_CFG;}

	p=&cfg[0];
	sum=0;
	for(i=0;i<cfgs;i++){
	  ClrWdt();
	  mem_ReadData(M_CFG,uSrc,p,lenCELL);			//=>cfg[]
	  uSrc+=lenCELL;
	  p+=lenCELL;	  
	}

//校验
	if(CheckCfg(&cfg[0])){			//校验成功
	  return OK;
	}

USE_DEFAULT_CFG:					//校验失败,使用默认配置
	memcpy(&cfg[0],&cfg_bak[0],def_len);	//cfg_bak[]  ->  cfg[]
	return FAILED;
}

//================================
// p指向一个CFG结构
//================================
BYTE CheckCfg(BYTE *p)
{
UINT i,sum;
BYTE cfgs;
BYTE crc_l,crc_h;

	crc_l=*(p+CM_CFG_CRC);
	crc_h=*(p+CM_CFG_CRC+1);
	cfgs=*(p+CM_LEN_CFG);
	if(cfgs>MAX_CFGS){cfgs=MAX_CFGS;}		//限制CFG总长度 <(32*64)= 2048
	if(cfgs<3){return 0;}
	sum=0;
	for(i=0;i<(UINT)cfgs*lenCELL;i++){
	  if((i!=CM_CFG_CRC)&&(i!=(CM_CFG_CRC+1))){
	    sum+=*p;
	  }
	  p++;
	}
	sum=~sum;
	if(LoOfInt(sum)!=crc_l)return 0;
	if(HiOfInt(sum)!=crc_h)return 0;
	return 1;
}

void InitPower(void)
{
/*
//欠压检测，VDD>2.5v(标称)
	if(_LVDIF){						//检测到低电压，复位
	  PW_NOFF_off();
	  PW_NON_off(); 
	  blight_off();					//关闭LCD_LED
	  LED_STATUS_off();
	  while(1);		
	}
	_LVDIE=ENABLED;
*/

//=== 电压检测
	PowerTrap();

//=== 确保电压状态
	PW_NON_on();
	PW_NOFF_off();
}

//==============================
// 是否有任务在运行？
//==============================
BYTE TaskLine(void)
{
BYTE r=0;

	r|=Tasking[TASK_COM1_IN];
	r|=Tasking[TASK_COM2_IN];
	r|=Tasking[TASK_CALENDAR_INT_REP];
	r|=Tasking[TASK_SENSOR_INT_REP];
	r|=Tasking[TASK_PSW_EDIT];
	r|=Tasking[TASK_LCD_DISP];
	r|=Tasking[TASK_SENSOR_QUERY];
	if(r){return 1;}
	return 0;
}

//===================================
//            Sleep
//
//===================================
void GoSleep(void)
{
BYTE sleep_count;

	ActiveTask=TASK_SLEEP;

	LED_STATUS_off();
	Timer2_off();
	SwitchToComport(PORT_SLEEP);

//关闭Keypad
	EndKeyPoll();

//电源准备。防止启闭电源带来的冲击
	PowerBeforeSleep();

//Safeguard
	SystemGuard();

	U1MODEbits.WAKE=1;
	U2MODEbits.WAKE=1;
	RCON=0b0000000000000000;	
	_WDTO=0;
	sleep_count=70;
	//========================================================================
	// LPRC fmax=31k*1.2=37.2k, 最小周期*128=3.4ms，FWPSA=1，WDT prescaler=128
	// 3.4ms*256=0.87s, WDT postscaler=256,
	// 0.87s*70=60.9s,  60.9<WDT<91, WDTO发生在60.9s以上，91s以下
	// 1min内，至少被非dog(e.g:如Calendar)唤醒一次。
	//========================================================================
	while(1){
	  ClrWdt(); 
	  LED_STATUS_off();
	  if(TaskLine()){break;}
 
	  Sleep();	//typically wakeup every 2's

	  if(_WDTO){
		_WDTO=0;
	    ClrWdt();
		sleep_count--;		
		if(!sleep_count){
		  Restart(RST_FOR_ERR_MAIN_12);
		}
	    LED_STATUS_on();
	 	delay_us(100);
	
		//Safeguard
		SystemGuard();
	  }
	  else{
		ClrWdt(); 
		break;		//wake-up
	  }		
	}

//After wakeup...
	Timer2_on();	//turn on TMR2
	KillC0s();
	PowerRestore();			//Power
    LED_STATUS_on();		//turn STATUS led

//OK唤醒？
	if(fBtnNow==1){			//有ok键按下
	  fBtnNow=0;

	  PW_LCD_on();			//turn 33V_NOFF for LCD
	  //lcd_init(); 
	  fLcdReady=lcd_stable();
	  blight_on();			//开启背光
	  cursor_off();

	  ActiveSrn=&Srn[0];
	  KeyPollEnabled=TRUE;
	  fSrnRefresh=TRUE;
	  fBlinkEnabled=TRUE;
	}
	ActiveTask=TASK_IDLE;	//释放任务权柄
}


void KeyDisplay(void)
{
	//*** 根据Key，修改ActiveSrn ***
	KeyProc();

	//****** 显示 ******
	if(fCfgGood){
	  if(swLCD){		//开关LCD
	    if(fSrnRefresh){
	      LCDShow();
		  fSrnRefresh=FALSE;
		}
	  	CursorBlink();
	  }
	  else{		//关闭显示
		lcd_clear();
		blight_off();

  		fLcdReady=FALSE; 
		fBlinkEnabled=FALSE;
		Tasking[TASK_LCD_DISP]=0;
	  }
	}
}





//==========================================================
//
//                         MIX MAIN
//
//==========================================================
int main()
{
BYTE old_click;

//关闭全部中断
	di_all();

//保证中断开启时，大量输入不崩溃
	ptr1_rx=&rx1[0];			
	ptr2_rx=&rx2[0];
	ptr3_rx=&rx3[0];				
	//ptr4_rx;


//***************** 初始化 ******************
	InitCPU();
	InitCN();		//中断初始化
	InitINTx();
	InitIPL();
    LED_STATUS_on();		//Status led
//
//
	Msg0.ALLbits=0;
	Msg1.ALLbits=0;
	Msg2.ALLbits=0;
	Tmr.ALLbits=0;						//disable all timer
	TmrUp.ALLbits=0;


//****************** 电源 *******************
//复位外围设备
	ResetPeri(1);
	Delay_s(1);		//避免“电压不稳定时，快速进入关键步骤”，
					//让CPU有时间在关键步骤前Reset。
//电源
	InitPower();

//复位标记 
	if(_POR){
	  fPowerOnReset=0;
	}		//复位重启
	else{
	  fPowerOnReset=1;
	}			//加电启动
	StatusReset();				//清除标识，与isPOR()配套
//	ResetCode=0;				//复位原因序号清零


//***************** CFG[] ******************
//加载CFG（FRAM）
	Delay_ms(100);		//Delay for 1st access(mini.10ms required by FM25L256B,mini.250us for FM25V02)
	fCfgGood=LoadCfgIfOk(CFG0);	
	if(fCfgGood==FALSE){			//如果校验失败，cfg[]使用默认值。		
	  SaveCfg(&cfg[0],CFG0);		//Save to CFG, FRAM
	  SaveCfg(&cfg[0],CFG_BUF0);	//加载CFG到CFG_BUF，为远程设置（数个字节）的校验做准备。
	}
	GetCTable();						//加载CTable

//=== 锁定
	FramLock(M_CFG);
	FramLock(M_MEM);

//C0S容错
	if((C0S==0)||(C0S>10)){
	  C0S=0;					//等待写入正确的cfg，应Warning
	}


//***************** 通信口 ******************
//通信口(确保串口事件能得到响应)
	InitCom();
//通信总开关
	ComSW=*(CMC(CMC_YN));

//临时
//	ComSW=0;

	FillBufWith(&ComOpen[0],4,0x0);		//清除COM口占用

//=== Modem ===
	fModemRstNeeded=FALSE;		//Modem不需复位
	ModemReady[0]=FALSE;		//启动时,假定为false
	ModemReady[1]=FALSE;		//启动时,假定为false
	ModemReady[2]=FALSE;		//启动时,假定为false
	cntGModemFailed=0;			//防止出现一启动就复位Modem的情况。

//pCmfg
	pCmfg=CMC(CMC_LINK);
	pCmfg=FindParaStr(pCmfg,"CMFG");

	ComStatus=STATUS_GPRS_MSG;


//****************** 显示 ******************
//显示屏开关
	swLCD=0;
	if(fLCD_SW){swLCD=1;}


//PW_LCD_on();

//临时
//	swLCD=0;

	cntLCDErr=0;
	if(swLCD){
	  //显示开机屏幕
	  InitDsp();		

//Delay_s(1);

//	  stepDISP=0;
	  cursor_off();

	  //显示器自动关闭控制
	  old_click=HiOfInt(ms_click);
	  iIdleKey=ms_click+WAIT_FOR_DSP;		//sleep after time is up

	  //加载LCD显示缓冲
	  Write_DspBuf(SCREEN_CX,&Srn[SCREEN_CX]);	
	  Write_DspBuf(SCREEN_SYS,&Srn[SCREEN_SYS]);	
	  Write_DspBuf(SCREEN_STATUS,&Srn[SCREEN_STATUS]);
	  Write_DspBuf(SCREEN_MENU,&Srn[SCREEN_MENU]);

	  ActiveSrn=&Srn[SCREEN_CX];
	  KeyPollEnabled=FALSE;
	  fSrnRefresh=TRUE;
	  fBlinkEnabled=FALSE;
	  StartScreen();			
//	  LCDShow();
	}

//背光关闭
//	blight_off();				


//***************** 键盘Key *****************
	key.count=0;
	key.val=KEY_UNKNOW;		//NULL;


//****************** 传感器 ******************
	CellRequest0=0;
	CellRequest1=0;

//内部传感器
	InitInnerSensor();

//Sensor INT ON
	RAIN_IF=0;
    RAIN_IE=TRUE;

//传感器中断字
	FillBufWith(&IntC0[0],62,0x0);		
	cntSensorComFailed=0;


//****************** SD-CARD *****************
	swSD_CARD=0;
	if(fSD_CARD){swSD_CARD=1;}

//临时
swSD_CARD=0;

	InitCardRecord();

//******************* 复位 ******************
	ResetCode=0;			//复位原因序号清零
	IncCount(mapCNT_RST);	//复位计数


//=== MacID
	ReadID();		//MacID[]赋值


//==== Timer2
	Timer2_on();	//turn on TMR2

//==== 日历INT on
	CALENDAR_IE=FALSE;
	InitCalendar();
	CALENDAR_IE=ENABLED;


	FillBufWith(&Tasking[0],20,0x0);	//清除全部任务
	RCON=0b0000000000000000;

//开启中断
	ei_all();

	DEBUG();


//=================主循环=================
	while(1){

	  //链路维持
//	  ComMaintenance(ComSW);

//暂时关闭Calendar中断
//	  SystemGuard();

	  //用户定制过程
	  usrProc();


	  //传感器中断
	  if(Tasking[TASK_SENSOR_INT_REP]){
		Tasking[TASK_SENSOR_INT_REP]=0;
//	    SensorIntReports();
	  }

	  //*** 中断事件(Com1，被动通信，按指令格式响应)
	  if(Tasking[TASK_COM1_IN]){
		if(Tasking[TASK_CALENDAR_INT_REP]==0){	//有“主动报”在运行时，延时响应Com1中断。主机A<>R握手（如远程读写）应有足够延时。
		  if(Tasking[TASK_SENSOR_INT_REP]==0){	//有传感器中断时，先处理传感器中断。
			ActiveTask=TASK_COM1_IN;
		    ActiveCom=COM1;
	        Com1Proc();
		  }
		}
	  }

	  //*** 中断事件(Com2，被动通信，按指令格式响应)
	  if(Tasking[TASK_COM2_IN]){
		if(Tasking[TASK_CALENDAR_INT_REP]==0){	//有“主动报”在运行时，延时响应Com2中断。主机A<>R握手(如写配置)应有足够延时。
		  if(Tasking[TASK_SENSOR_INT_REP]==0){	//有传感器中断时，先处理传感器中断。
			ActiveTask=TASK_COM2_IN;
		    ActiveCom=COM2;
	        Com2Proc();
		  }
		}
	  }

	  //*** Editor界面 
	  if(Tasking[TASK_PSW_EDIT]){
		Editor(SCREEN_PSW);
	  }


	  //*** 睡眠控制(Sleep control)
	  if(IsTimeUp(iIdleKey)==TRUE){
		Tasking[TASK_LCD_DISP]=0;
		Tasking[TASK_PSW_EDIT]=0;
	  }

      if(TaskLine()==0){ 
		GoSleep();
	    //if(fDspWaiting==TRUE){
	   	//  if(IsTimeUp(iIdleKey)==TRUE){GoSleep();}
		//}
		//else{GoSleep();}
      }
	  //记录发生时刻、上报时刻紧接在一起，可减少连续ReadBelts(1s之内).


//主动通信,需要格式#支持
	  if(testbit(ComSW,bitSW_REPORT)){
	    if(fCfgGood){
		  if((Tasking[TASK_CALENDAR_INT_REP])||(Tasking[TASK_SENSOR_INT_REP])){
		    IntReports();				//中断：日历||传感器。
		
		  //  ModemRstIfNeeded();			//如失败条件满足，复位Modem
	      }
	    }
	  }

	  //陷阱(trap)
	  #ifdef WATCHDOG_ON
	  ClrWdt();
	  if((PR2!=PR_TMR2)
	  ||((T2CON&0x00FF)!=0x0000)){
	    Nop();
	    Restart(RST_FOR_ERR_MAIN_11);
	  }
	  #endif
	}
}
