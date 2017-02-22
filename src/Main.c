#include 	"header.h"
#include    "h_macro.h"

//���� see in "p24fj256ga106.h"
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

//Ĭ��cfg[]
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
//1��ϵͳ����ʱ���ڶ��������л���
//2���κ�ʱ��ֻ��һ����Ծ����
//=========================================
BYTE		Tasking[20];	//0:δ������ 1��������
BYTE		ActiveTask;		//��ǰCPU���еĻ�Ծ�����		

//�����ڽ׶κ�
//BYTE 		stepDISP;
BYTE 		stepINT_REPORT;
BYTE 		stepDO_REPORT;
BYTE 		Step[10];			//�������STEPS_


//��ǰ�˿ںţ�1-3, ֻ��LCD��ʾʱΪ4����ʼ�˿ڣ�1
BYTE		ActiveCom;		//1-4
BYTE		ComOpen[5];		//COM��ռ�ñ��:1-4; Ϊ���һ�£���ʼ�˿ڣ�1
BYTE		ModemReady[5];	//ModemReady���


//=========================================
//Normal Variables
//=========================================
volatile UINT 	ms_click,sec_click,sec_cmp;

//��ʱ
UINT	iTimeCmp;
UINT	iIdleKey;
UINT	iWaitSensorCmp;
UINT	iRefreshDly;	

BYTE   	filter_type;
BYTE 	ComStatus;			//ͨ��ģʽ
BYTE	minTrans0;
BYTE 	fCfgGood;	
BYTE 	*pCmfg;
BYTE 	ComSW;


//Status
BYTE	cntGModemFailed;	//����-GSM Modemʧ��
BYTE	cntSensorComFailed;	//����-������ͨ��ʧ��
BYTE	cntMemMirrorFailed;	//����-�洢�������
BYTE	cntRecFailed;
BYTE 	cntXmeFailed;
BYTE	cntLCDErr;
//BYTE 	cntRestart;			//Rst����



//for LCD
BYTE	fLcdReady;
BYTE	fBlinkEnabled;
BYTE	ScrollIndex;		//������ʾ���Զ�+1; {0 ��LINES_LCD_BUF-1}
BYTE	CurX,CurY;			//LCD cursor position, start from {0,0}
BYTE 	*pFormat,*pDefault;
BYTE 	ResetCode;
int		pb_count;

SCREEN 	Srn[4];
extern SCREEN *ActiveSrn;

//for KEY
BYTE 	KeyPollEnabled;		//����OK����ʱ��������ֵ��ѯ
BYTE 	KeyBlindCount;

//���ڱ����������ʱ�䣨����15�֣���5�֣�
//�����hYear�ȣ�rTime����1���ӣ�hYear��ʱ���ܱ��޸Ļ�ˢ��
hTIME	rTime;	

BYTE	*ptr_tx;	
BYTE	Tx_Count;

//
BYTE	*ptr1_rx,rx1[RX_BUF_SIZE1];		//�����ջ��壺32+(2+4+2048)+2=2088	
BYTE	*ptr2_rx,rx2[RX_BUF_SIZE2];
BYTE	*ptr3_rx,rx3[RX_BUF_SIZE3];		//�������ӿ�	
BYTE	*ptr4_rx;


//�˿�Belt
PORT	Port[8];			//֧�����8����Ԫ(C0)

//������
BYTE	CellRequest0;
BYTE	CellRequest1;
BYTE 	IntC0[62];			//�˿ڵ��жϱ�־�����жϴ������ˢ��

BYTE 	c0Table[62];		//c0�׵�ַ��
BYTE 	cxTable[256];		//cx_off��+cx�׵�ַ��

//for IPL
extern BYTE 	IPL_CPU;

#ifdef SIM_ON
hTIME simDT;
#endif

UINT KeyV[3]={0x02AA,0x0158,0x0020};	//KEY1-3 ��Ӧ�ĵ�ѹֵ


//==============================================
//��ʱ��2
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
		  StartKeyAD();				//��ʱ����KeyAD
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
// GPRS MSG����ͨ��ģʽ
//====================================
void U_Rx(void)
{
BYTE *p,c; 

	CLR_RX_IF();				//RX1_IF=0;

//1������Ҫ���յ�һ���ַ�ʱ����ȴ���Ӧ����Ӧ���ȳ�ʼ���˿ڡ�
//2���ⲿ��������ʱ(��A>R��RING)�����ܻᶪʧ��һ���ֽ�
	if(IsComOpen()==FALSE){			//�˿�δ��ռ��				//���û������������ͨ������
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
//COM1: GPRSģ��ͨ�Ž���/͸��
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
//Com1����
//==============================================
void __attribute__((__interrupt__,auto_psv)) _U1TXInterrupt(void)  
{  
	if(TX1_IF){		//interrupt		
	  if(--Tx_Count==0){			
	    TX1_IE=FALSE; 		//������ϱ�־			
	  }								
	  S1_Transmit(*ptr_tx++);		
	}								
}

//==============================================
//Com2����
//==============================================
void __attribute__((__interrupt__,auto_psv)) _U2TXInterrupt(void)  
{  
	if(TX2_IF){		//interrupt		
	  if(--Tx_Count==0){			
	    TX2_IE=FALSE; 		//������ϱ�־;			
	  }								
	  S2_Transmit(*ptr_tx++);		
	}								
}

//==============================================
// LCDͨ�Ž���
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
void __attribute__((__interrupt__,auto_psv)) _INT1Interrupt(void)     //���ش���           
{
//BYTE save_T2IE,save_T2ON;


#ifdef SIM_ON
	AddMinute(&simDT,1);		//��ǰʱ���1����
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
	  Tasking[TASK_CALENDAR_INT_REP]=1;	//Ԥ���Ա�
	  stepINT_REPORT=0;
	}

	CalendarProc();				//ʵ���ʱ��(70ms/Cx) X ��¼cx����

//	T2_IE=saveT2IE;
//	T2_ON=saveT2ON;
}


//==============================================
// Key wakeup
// ������õ��ǵ�ƽ�仯�жϡ�Ҳ���Բ����ⲿ�жϿڵ���ʽ��
//��ʹ���ⲿ�жϿڣ���Ҫ�� �������Ÿ����ⲿ�жϿڹ��ܼ��ɡ��˹����Ժ������
// ���������ж�AND  key,�ⲿ��ʱ���ж�
//==============================================
void __attribute__((__interrupt__,auto_psv)) _CNInterrupt(void)     //���ش���           
{
    _CNIF=0;              	//�����жϱ�־λ,
	if(btnKey==0){			//����OK��button-press is  a valid wakeup, while depress not
	  swBUZZ=OFF;			

	  //����
	  fBtnNow=1;			//save btn status

	  key.val=KEY4;
	  key.count=KEY_CONFIRM;

	  //��ʾ
	  PW_LCD_on();			//turn 33V_NOFF for LCD
	  Tasking[TASK_LCD_DISP]=1;
	  iIdleKey=ms_click+WAIT_FOR_DSP;		//sleep after time is up
//	  stepDISP=0;
	  if(swLCD_LIGHT==OFF){fLightOnRequest=TRUE;}		

	  swBUZZ=OFF;
	  KeyPollEnabled=TRUE;	  	//����
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

	if(key.val==KEY4){		//OK�������ȴ���
	  return;
	}		

//	while(!_DONE)continue;
	uk=ADC1BUF0;
	_ADON=0;		//shut off A/D	
	T3_ON=0;

	if(key.count==0){
	  for(i=0;i<3;i++){			//��ѯKEY1-3
	    if(Abs(uk,KeyV[i])<=KEY_WIDTH){			//��ֵ�Ƚ�
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
	    if(Abs(uk,KeyV[key.val-1])<=KEY_WIDTH){		//��ֵ�Ƚ�
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
void __attribute__((__interrupt__,auto_psv)) _INT2Interrupt(void)     //���ش���           
{
	if(RAIN_IF){
	  RAIN_IF=0;
	  IncRain();
	  fSrnRefresh=TRUE;		//��Ļˢ��

	  if(EventRain()){
	    Tasking[TASK_SENSOR_INT_REP]=1;
	  }
	}
}

//==============================================
// GPRS RING
//==============================================
void __attribute__((__interrupt__,auto_psv)) _INT3Interrupt(void)     //���ش���           
{
	if(GPRS_Ring_IF){
	  GPRS_Ring_IF=0;
//	  fRingIn=TRUE;	
	}
}


//=================================================
//                      LVD
//
// 1������ڼ�¼ʱ������LVD�жϡ����ڼ�¼�뱾���̵�
// IPLһ�£����ԣ����ȴ���¼��ɺ�λ����������
// ���������˳���¼���̣��Լ�¼���ܵ��𻵡�
// 2������ٶ�Ƿѹ������ϵͳ�޷������ж�����ʱ��
//=================================================
void __attribute__((__interrupt__,auto_psv)) _LVDInterrupt(void)                
{
	_LVDIF=0;

//
/*	PW_NOFF_off();
	PW_NON_off(); 
	blight_off();					//�ر�LCD_LED
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
	if(task_Comm2Int==0){	//���û������������ͨ������
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
// Sensorͨ�Ž���
//==============================================
/*
void __attribute__((__interrupt__,auto_psv)) _U3RXInterrupt(void)  
{  
	RX3_IF=0;	
	if(task_SensorInt==0){	//���û������������ͨ������
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
// GPRS͸��ģʽ
//=============================================
/*
void Urx_GPRS_TRANS(void)
{
BYTE *ptr_rx; 

	CLR_RX_IF();		//RX1_IF=0;
	if(task_Comm1Int==0){	//���û������������ͨ������
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

	CloseComPort();			//�رն˿�	
	RS232_off();

    Serial_End_Tx();				
	CLR_RX_IF();		//���ͨ���ڼ����µ��жϱ�־
	RX_IE_ON();

//	SwitchToComport(PORT_SLEEP);	//�л����ܵ���Rx����,�󴥷��µ�ͨ��.
	Delay_ms(200);

//	ActiveCom=COM1;
//    ei_all();       
}


void SwitchToComport(BYTE port)
{
	switch(port){
	case PORT_SLEEP:
	  TX1_DIR=OUT; 

//	  TX1=0;	//��Ȼʡ��,�����ܻᵼ�����һ������TX,Modem���ܻ᷵�ش���.
	  TX1=1;	//������0.2mA����,���޴������.
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
// ����д��������������
//=====================================
/*
void MarkSensorRequest(BYTE *des)
{
	if(testbit(*(des+PARA2),7)){					//��⵽����������
	  SetCfgBit(REG0+(UINT)MAIN_PROPERTY2,bitSENSOR_REPORT_REQUEST);
	  CellRequest0=0;
	  CellRequest1=0;
	}
}
*/


//===============================
//  belt�����롰�����־��
//===============================
void ClrBelt(BYTE *des,BYTE belt_len)
{
BYTE i;
	for(i=0;i<belt_len;i++){
	  if(i==0){*des++=ERR_PORT_ACCESS;}
	  else{*des++=(ERR_PORT_ACCESS&0x40);}		//��λ����
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
	//inst[FRM_STA_ID]=0;		Ĭ��ֵ
	inst[FRM_OP]=OP_DO_MEASUREMENT;
	//inst[FRM_PROPERTY]=0;
	//inst[FRM_INST_INDEX]=0;
	sum=GetCRC(&inst[0],lenHEAD-2);
	inst[HCRC_L]=LoOfInt(sum);
	inst[HCRC_H]=HiOfInt(sum);
//
	i=RWSerialSensor(&inst[0],inst[FRM_LEN]+1,des,des_limit,speed_mode);
	if(i==OK){
	  return inst[FRM_DELAY_NEEDED];		//���أ�delay time needed
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
	//addrC0=REG0+((UINT)(CM_C0_START)*lenCELL);				//ָ��C0�׵�ַ
    while(iC0<C0S){
	  //c0=((UINT)c0Table[iC0])*lenCELL;
	  c0=getC0(iC0);
	  port_delay=cfg[c0+C0_MAX_DELAY];
	  port_delay&=0x3f;
	  if(port_delay!=0){
		if(IsSensorReady()){
		  //ReadBelt(i,&resp[0],16);		//resp_limit=16
		  DoMeasurement(iC0,&resp[0],16,MODE_NORMAL);		//���������к���"��ʱ"����,��������Ҳ�ɲ���.
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
//	0: ����ģʽ�����ȴ����ٴ�������ɵ�ǰ������
//  1: ����ģʽ���ȴ���������ɵ�ǰ������
// return:
//       belts���ܳ���
//	     0: ʧ��
//====================================================================
void PrePortReading(void)
{
//BYTE max_delay;

	PW_NOFF_on();	//����PW_NOFF������Ϊ���������磩��ֱ��ȫ��Belt���ꡣ

//  max_delay=StartSensors();

//ȡ���ȴ���Ҫ��: 
//			1)��Կ��ٴ��������ܿ�����Ӧ��
//			2)���ٵĴ�����Ҫ���Թ���,�������������������
//  �����ɿ��ǣ�������ʱ���������ƣ���TCTL GGDD #1����ʱ���� ��� #1���Ʒ�����
//
//
//  WaitSensorEcho(max_delay);		
}

void PostPortReading(void)
{
	if(swLCD_LIGHT==OFF){	//�����⴦�ڿ���״̬ʱ����Ϊ�˹�����״̬��PW_NOFFά�ֿ�����
	  PW_NOFF_off();		//ȫ��Belt����󣬹ر�PW_NOFF
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
		//��ָ��
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
//	  CS_PORT2=0��
//	  CS_PORT2=1;
	  NOP;
	  break;
	case SPI_ID_3:
//	  CS_PORT3=0��
//	  CS_PORT3=1;
	  NOP;
	  break;
	case SPI_ID_4:
//	  CS_PORT4=0��
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
ULNG StaID;	//ȡStaID�ĵ�Word�������ֳ�485����������ͨ�ŷ�ʽ����ADDR_CARELESS��
BYTE f,c,fc;
BYTE r;
PROPERTY property;
BYTE *inst;

//sw[0]=ms_click;

	property.ALLbits=0;
	property.bitINST_DIR=0;				//��Ӧ
	property.bitANSWER_NEEDED=0;		//����Ҫ�ظ���ͨ�ŷ�ʽ�ɸ��Ǳ����ԣ�

	inst=RX_BUF0();
	if(*inst!=INST_HEADER){return FAILED;}

//Address
	StaID=getULNG(&cfg[CM_STA_ID]);

//	if(!testbit(rx[FRM_PROPERTY+1],bitDES_CHECK)

	InstAddr=getULNG(inst+FRM_SRC_ADDR);
InstAddr=ADDR_CARELESS;	//!!!

	if((InstAddr==ADDR_CARELESS)		//���ܵ�ַ�����ڹ㲥���⹤�ߡ�
	  ||(InstAddr==StaID)){			//ָ����ַ��[0:0xffff],����485����ѰַӦ�á�
	  switch(*(inst+FRM_OP)){
	    case OP_READ_BELT:
//		case OP_READ_RANDOM:
		  if(*(inst+FRM_OP)==OP_READ_BELT){		//����Belt����
			f=*(inst+TSEN_BELT_FMT_ID)&0x0f;
			c=6;							//DIRECT_2
			fc=(f<<4)|c;
//			if(stepDO_REPORT){return BUSY;}  ���ܴ�ʱ��ͨ������Դδ��ռ��
			stepDO_REPORT=0;
		    do{
			  r=DoReport(fc,property.ALLbits);
			}while(r==0xff);
//sw[1]=ms_click;
		  }
		  else{									//��ʷ����
			//i=InstQuery(&rx[0]);
			i=1;	//��ʱȡ���������ѯ��

		    if(i==FAILED){
			//  MakeTextMsg("No Result Matched");
		    }
		    else{
//			  MakeQueryMsg(rx[PARA1],i);			//��Ԫ��,REC_OR_MEM
		    }
		  }
		  break; 
//����
        case OP_DOWNLOAD_MAP:
		  saveCALENDAR_IE=CALENDAR_IE;				
		  CALENDAR_IE=FALSE;			//�ر�����ʱ���ж�
          i=DownLoadMap(CONNECT_DIRECT,inst);
		  CALENDAR_IE=saveCALENDAR_IE;	//�ָ�ԭ״̬
		  return ShowDownResult(i);		//����֤ʧ�ܡ���дע���Reset�� 
		  
		  //return STEP_RESET;
		  //break;
//����
        case OP_UPLOAD_MAP:				
          UpLoadMap(CONNECT_DIRECT);
		  break;

//����ʷ��¼
//		case OP_READ_RECS:
//		  UpLoadSDRecs(inst);
//		  break;


/*        case OP_READ_BLOCK:					//GPRS��ʽͨ��
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
// Download����ʾִ�н��
// ���أ��Ƿ�������Reset
//==================================
BYTE ShowDownResult(BYTE result)
{
//��������
	blight_on();

//
	if(result==STEP_AUTHEN_FAILURE){
	  MsgBox("�����֤ʧ��......        ");
	  Delay_s(1);
      return FAILED;
	}

	if(result==OK){
	  MsgBox("дע��..............�ɹ�");
	}
	else{
	  MsgBox("дע��..............ʧ��");
	  //return FAILED;
	}

//???����������Ƿ�...
	if(1){
      lcd_puts(0,L1,"��ʼ��Modem...          ");       ///
	  if(InitAllComTasks()==OK){
        lcd_puts(0,L1,"��ʼ��Modem.........���");       ///
	  }
	  else{
        lcd_puts(0,L1,"��ʼ��Modem.........ʧ��");       ///
	  }
	}
//
	Delay_s(5);
	return STEP_RESET;		//�Ƿ�������
}

//=============================================
//Output buffer string to comm. port
//return: len of buffer
//=============================================
/*
BYTE OutputBuf(BYTE response_com_type,BYTE *buff0,BYTE len)	//des. is define in channel object
{

	switch(response_com_type){

	  case CONNECT_DIRECT:			//͸��
	    return len;

	  case CONNECT_USWAVE:		//���̲�
	    return FormatUltrashort(buff0,len);

	  case CONNECT_PSTN:			//�绰��
	    return FormatPSTN(buff0,len);

//	  case CONNECT_GSM_DT:			//GSM����
//	    return FormatPSTN(buff0,len);

	  case CONNECT_SM:			//GSM����Ϣ
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
//���ڱ���ѯ��ʽ���������Ա�
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
	    if(IsModemReady()==FALSE){			//���Modem״̬
	      AT_creg(ActiveCom);	//����ˢ�£���֤ʱ��ͣ��ʱ��Ҳ����Ӧ	
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
		PW_NOFF_on();			//WIFIģ���ϵ�
		Delay_s(2);
		//Serial_Setup(BAUD_115200);
		break;

	  default: Restart(RST_FOR_ERR_MAIN_7);
	}
	return OK;
}

//======================================
//
// Com1 ����ͨ��
// ���ڱ���ѯ��ʽ���������Ա�
//
//======================================
void Com1Proc(void)
{
BYTE com_type;

//	ActiveTask=TASK_COM1_IN;
//	ActiveCom=COM1;

//͸��ģʽ
	if(ComStatus==STATUS_GPRS_TRANS){
	  DirectAccess();
	  return;
	}

//����ģʽ
//	com_type=GetComType(ActiveCom);			//��ȡ����

//��ʱ
com_type=CONNECT_SM;
	if(com_type==CONNECT_ERROR){goto COM_OVER;}		//��Ӧͨ��û������

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
		  Delay_s(1);				//��ֹ������Ϣ��������
		}
		break;
	default:
		break;
	}

COM_OVER:
	Tasking[ActiveTask]=0;	//�رյ�ǰ����
	ActiveTask=TASK_IDLE;	//�ͷ�����Ȩ��
	EndCom();
	ei_all();
}	    

void Com2Proc(void)
{
   	DirectAccess();
//	ActiveTask=0;	//�ͷ�����Ȩ��
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
// ����ͨ��
// ���ڱ���ѯ��ʽ���������Ա�
//
// ����ǰ��ȷ��ActiveCom
//======================================
void DirectAccess(void)
{
BYTE t,com_type;
UINT EndMs;

//����"͸��"��ʼSecond
//	if(ComStatus==STATUS_GPRS_TRANS){minTrans0=GetMinute();}

	PW_NOFF_on();			
	if(RxPinLocked()==TRUE){
	  goto DIRECT_ACCESS_OVER;	//��������ֻ����½����崥�����Գ�ʱ�����Ͳ���Ӧ��
	}

//debug mode
	com_type=CONNECT_DIRECT;		 	//debug mode

//
	Serial_Transmit('R');
	EndMs=InitRx(2500,FILTER_DIRECT);	//2.5s, �����ʱ
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

	if(t==STEP_RESET){Restart(RST_FOR_MAIN_90);}	//��������
	    
DIRECT_ACCESS_OVER:
	Tasking[ActiveTask]=0;	//�رյ�ǰ����
	ActiveTask=TASK_IDLE;	//�ͷ�����Ȩ��
	EndCom();
	ei_all();
}

//============================
// �ж�Rx�Ƿ�ǿ������
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
//	  goto COM2_OVER;	//��������ֻ����½����崥�����Գ�ʱ�����Ͳ���Ӧ��
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
// Sleepǰ��Դ׼��
//
// ��ֹ���յ�Դ�����ĳ��
// �����ԴӦ��1s�ڽ�����ȫ��ѹ�¡�
//===================================
void PowerBeforeSleep(void)
{
//������,��ֹ��̬���
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
	//�ȴ���Դ�ȶ��ضϡ�
	//�����Դ�ض�ʱ��״̬��ȷ�������ܳ�Ϊ����Դ)

	ei_all();

//������ͣ��͹���	
	COM4_EN=FALSE;			//�ر�COM4
	LCD_TX_DIR=OUT;			//ʡ��
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
	  Scroll(WAY_CRLF," >�ӵ�����...");
	}
	else{
	  Scroll(WAY_CRLF," >��λ...");
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
//���ϵ縴λ 
	if(!_POR){
	  lcd_puts(0,L0,"start(1)...             ");
	  //lcd_cursor(3,L0); lcd_puts("1");		//��һ�У�һ�㸴λ���µ�����
	  if(Error0.ALLbits){
		btoa(Error0.ALLbits,(char *)&msg[0]);
		lcd_puts(0,L1,(const char *)&msg[0]);		//�ڶ��У���ʾ��������
	  }
	  btoa(ResetCode,(char *)&msg[0]);
	  lcd_puts(0,L2,(const char *)&msg[0]);		//�����У���ʾ��λԭ��
	  Delay_s(2);
	}		

//�ϵ縴λ
	else{
	  lcd_puts(0,L0,"start(0)...             ");			//��һ�У��ϵ縴λ���µ�����
//	  setbit(mark,bitPWR_ON);				//�����ϵ硱���       
	  setbit0(mark);						//�����ϵ硱���       
	}
	Error0.ALLbits=0;
*/

//Id�Ƿ��������ã�
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

//	RB_IE=TRUE; ע��				
//	RB_IE=FALSE; 				

	ei_all();       			
}

//=====================================
// �γ�c0Table��
//         		C1 addr		//�׵�ַ
//         		C2 addr
//
// �γ�cxTable��
//         		C1 offset		
//         		C2 offset	//offset���ӱ�ͷ0����
//         		...
//         		C1.1 addr
//         		C1.2 addr
//         		...
//         		C2.1 addr
//         		C2.2 addr
//         		...
//
// 1��c0Table[]��cxTable[]��CELLΪ��λ
// 2��cfg[0]��Ӧ����ʼ��ַΪ��0
//=====================================
void GetCTable(void)
{
BYTE *t,offset;	
BYTE c0_num,len;
BYTE iC0,iCx;
BYTE CXS;
BYTE cCX;
BYTE *pC0;

//c0Table[], ����C0�׵�ַ
	t=&c0Table[0];
	pC0=&cfg[CM_C0_START]; 
	*t++=cfg[CM_C0_START];
	for(iC0=1;iC0<C0S;iC0++){
	  *t=cfg[(*pC0)*lenCELL+C0_NEXT_START];		//C0 start
	  pC0=t;
	  t++;
	}


//����Cx��offset
	c0_num=C0S;
	t=&cxTable[c0_num];
	offset=c0_num;

//����Cx�׵�ַ
	for(iC0=0;iC0<c0_num;iC0++){
	  cxTable[iC0]=offset;
	  CXS=cfg[c0Table[iC0]*lenCELL+C0_CXS];		//��Ԫ��
	  //
	  len=cfg[c0Table[iC0]*lenCELL+FLD_LEN];	//��Ԫ����
	  cCX=c0Table[iC0]+len;						//��1����Ԫ����ʼ��ַ
	  for(iCx=0;iCx<CXS;iCx++){
	    *t++=cCX;				//CX start
	    cCX+=cfg[cCX*lenCELL+FLD_LEN];
		offset++;
	  }
	}
}


//=====================================
// ��uSrcָ���CFGУ����ȷ��������cfg[];
// ��У��ʧ�ܣ�cfg[]����
//
// ���أ�0��ʧ�ܣ�1 �ɹ�
//=====================================
BYTE LoadCfgIfOk(UINT uSrc)
{
BYTE cfgs;
BYTE i,*p;
UINT sum,def_len;		
BYTE cfg_bak[64*3];		//��ʱ����Ĭ��cfg[]

#ifdef SIM_ON
	return OK;
#endif

	def_len=64*3;
	memcpy(&cfg_bak[0],&cfg[0],def_len);	//cfg[]  ->  cfg_bak[]

//��FRAM������cfg[]
	cfgs=GetByte(M_CFG,(UINT)(CM_LEN_CFG));
	if(cfgs>MAX_CFGS){cfgs=MAX_CFGS;}					//����CFG�ܳ��� <(32*64)= 2048
	if(cfgs<=3){goto USE_DEFAULT_CFG;}

	p=&cfg[0];
	sum=0;
	for(i=0;i<cfgs;i++){
	  ClrWdt();
	  mem_ReadData(M_CFG,uSrc,p,lenCELL);			//=>cfg[]
	  uSrc+=lenCELL;
	  p+=lenCELL;	  
	}

//У��
	if(CheckCfg(&cfg[0])){			//У��ɹ�
	  return OK;
	}

USE_DEFAULT_CFG:					//У��ʧ��,ʹ��Ĭ������
	memcpy(&cfg[0],&cfg_bak[0],def_len);	//cfg_bak[]  ->  cfg[]
	return FAILED;
}

//================================
// pָ��һ��CFG�ṹ
//================================
BYTE CheckCfg(BYTE *p)
{
UINT i,sum;
BYTE cfgs;
BYTE crc_l,crc_h;

	crc_l=*(p+CM_CFG_CRC);
	crc_h=*(p+CM_CFG_CRC+1);
	cfgs=*(p+CM_LEN_CFG);
	if(cfgs>MAX_CFGS){cfgs=MAX_CFGS;}		//����CFG�ܳ��� <(32*64)= 2048
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
//Ƿѹ��⣬VDD>2.5v(���)
	if(_LVDIF){						//��⵽�͵�ѹ����λ
	  PW_NOFF_off();
	  PW_NON_off(); 
	  blight_off();					//�ر�LCD_LED
	  LED_STATUS_off();
	  while(1);		
	}
	_LVDIE=ENABLED;
*/

//=== ��ѹ���
	PowerTrap();

//=== ȷ����ѹ״̬
	PW_NON_on();
	PW_NOFF_off();
}

//==============================
// �Ƿ������������У�
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

//�ر�Keypad
	EndKeyPoll();

//��Դ׼������ֹ���յ�Դ�����ĳ��
	PowerBeforeSleep();

//Safeguard
	SystemGuard();

	U1MODEbits.WAKE=1;
	U2MODEbits.WAKE=1;
	RCON=0b0000000000000000;	
	_WDTO=0;
	sleep_count=70;
	//========================================================================
	// LPRC fmax=31k*1.2=37.2k, ��С����*128=3.4ms��FWPSA=1��WDT prescaler=128
	// 3.4ms*256=0.87s, WDT postscaler=256,
	// 0.87s*70=60.9s,  60.9<WDT<91, WDTO������60.9s���ϣ�91s����
	// 1min�ڣ����ٱ���dog(e.g:��Calendar)����һ�Ρ�
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

//OK���ѣ�
	if(fBtnNow==1){			//��ok������
	  fBtnNow=0;

	  PW_LCD_on();			//turn 33V_NOFF for LCD
	  //lcd_init(); 
	  fLcdReady=lcd_stable();
	  blight_on();			//��������
	  cursor_off();

	  ActiveSrn=&Srn[0];
	  KeyPollEnabled=TRUE;
	  fSrnRefresh=TRUE;
	  fBlinkEnabled=TRUE;
	}
	ActiveTask=TASK_IDLE;	//�ͷ�����Ȩ��
}


void KeyDisplay(void)
{
	//*** ����Key���޸�ActiveSrn ***
	KeyProc();

	//****** ��ʾ ******
	if(fCfgGood){
	  if(swLCD){		//����LCD
	    if(fSrnRefresh){
	      LCDShow();
		  fSrnRefresh=FALSE;
		}
	  	CursorBlink();
	  }
	  else{		//�ر���ʾ
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

//�ر�ȫ���ж�
	di_all();

//��֤�жϿ���ʱ���������벻����
	ptr1_rx=&rx1[0];			
	ptr2_rx=&rx2[0];
	ptr3_rx=&rx3[0];				
	//ptr4_rx;


//***************** ��ʼ�� ******************
	InitCPU();
	InitCN();		//�жϳ�ʼ��
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


//****************** ��Դ *******************
//��λ��Χ�豸
	ResetPeri(1);
	Delay_s(1);		//���⡰��ѹ���ȶ�ʱ�����ٽ���ؼ����衱��
					//��CPU��ʱ���ڹؼ�����ǰReset��
//��Դ
	InitPower();

//��λ��� 
	if(_POR){
	  fPowerOnReset=0;
	}		//��λ����
	else{
	  fPowerOnReset=1;
	}			//�ӵ�����
	StatusReset();				//�����ʶ����isPOR()����
//	ResetCode=0;				//��λԭ���������


//***************** CFG[] ******************
//����CFG��FRAM��
	Delay_ms(100);		//Delay for 1st access(mini.10ms required by FM25L256B,mini.250us for FM25V02)
	fCfgGood=LoadCfgIfOk(CFG0);	
	if(fCfgGood==FALSE){			//���У��ʧ�ܣ�cfg[]ʹ��Ĭ��ֵ��		
	  SaveCfg(&cfg[0],CFG0);		//Save to CFG, FRAM
	  SaveCfg(&cfg[0],CFG_BUF0);	//����CFG��CFG_BUF��ΪԶ�����ã������ֽڣ���У����׼����
	}
	GetCTable();						//����CTable

//=== ����
	FramLock(M_CFG);
	FramLock(M_MEM);

//C0S�ݴ�
	if((C0S==0)||(C0S>10)){
	  C0S=0;					//�ȴ�д����ȷ��cfg��ӦWarning
	}


//***************** ͨ�ſ� ******************
//ͨ�ſ�(ȷ�������¼��ܵõ���Ӧ)
	InitCom();
//ͨ���ܿ���
	ComSW=*(CMC(CMC_YN));

//��ʱ
//	ComSW=0;

	FillBufWith(&ComOpen[0],4,0x0);		//���COM��ռ��

//=== Modem ===
	fModemRstNeeded=FALSE;		//Modem���踴λ
	ModemReady[0]=FALSE;		//����ʱ,�ٶ�Ϊfalse
	ModemReady[1]=FALSE;		//����ʱ,�ٶ�Ϊfalse
	ModemReady[2]=FALSE;		//����ʱ,�ٶ�Ϊfalse
	cntGModemFailed=0;			//��ֹ����һ�����͸�λModem�������

//pCmfg
	pCmfg=CMC(CMC_LINK);
	pCmfg=FindParaStr(pCmfg,"CMFG");

	ComStatus=STATUS_GPRS_MSG;


//****************** ��ʾ ******************
//��ʾ������
	swLCD=0;
	if(fLCD_SW){swLCD=1;}


//PW_LCD_on();

//��ʱ
//	swLCD=0;

	cntLCDErr=0;
	if(swLCD){
	  //��ʾ������Ļ
	  InitDsp();		

//Delay_s(1);

//	  stepDISP=0;
	  cursor_off();

	  //��ʾ���Զ��رտ���
	  old_click=HiOfInt(ms_click);
	  iIdleKey=ms_click+WAIT_FOR_DSP;		//sleep after time is up

	  //����LCD��ʾ����
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

//����ر�
//	blight_off();				


//***************** ����Key *****************
	key.count=0;
	key.val=KEY_UNKNOW;		//NULL;


//****************** ������ ******************
	CellRequest0=0;
	CellRequest1=0;

//�ڲ�������
	InitInnerSensor();

//Sensor INT ON
	RAIN_IF=0;
    RAIN_IE=TRUE;

//�������ж���
	FillBufWith(&IntC0[0],62,0x0);		
	cntSensorComFailed=0;


//****************** SD-CARD *****************
	swSD_CARD=0;
	if(fSD_CARD){swSD_CARD=1;}

//��ʱ
swSD_CARD=0;

	InitCardRecord();

//******************* ��λ ******************
	ResetCode=0;			//��λԭ���������
	IncCount(mapCNT_RST);	//��λ����


//=== MacID
	ReadID();		//MacID[]��ֵ


//==== Timer2
	Timer2_on();	//turn on TMR2

//==== ����INT on
	CALENDAR_IE=FALSE;
	InitCalendar();
	CALENDAR_IE=ENABLED;


	FillBufWith(&Tasking[0],20,0x0);	//���ȫ������
	RCON=0b0000000000000000;

//�����ж�
	ei_all();

	DEBUG();


//=================��ѭ��=================
	while(1){

	  //��·ά��
//	  ComMaintenance(ComSW);

//��ʱ�ر�Calendar�ж�
//	  SystemGuard();

	  //�û����ƹ���
	  usrProc();


	  //�������ж�
	  if(Tasking[TASK_SENSOR_INT_REP]){
		Tasking[TASK_SENSOR_INT_REP]=0;
//	    SensorIntReports();
	  }

	  //*** �ж��¼�(Com1������ͨ�ţ���ָ���ʽ��Ӧ)
	  if(Tasking[TASK_COM1_IN]){
		if(Tasking[TASK_CALENDAR_INT_REP]==0){	//�С���������������ʱ����ʱ��ӦCom1�жϡ�����A<>R���֣���Զ�̶�д��Ӧ���㹻��ʱ��
		  if(Tasking[TASK_SENSOR_INT_REP]==0){	//�д������ж�ʱ���ȴ��������жϡ�
			ActiveTask=TASK_COM1_IN;
		    ActiveCom=COM1;
	        Com1Proc();
		  }
		}
	  }

	  //*** �ж��¼�(Com2������ͨ�ţ���ָ���ʽ��Ӧ)
	  if(Tasking[TASK_COM2_IN]){
		if(Tasking[TASK_CALENDAR_INT_REP]==0){	//�С���������������ʱ����ʱ��ӦCom2�жϡ�����A<>R����(��д����)Ӧ���㹻��ʱ��
		  if(Tasking[TASK_SENSOR_INT_REP]==0){	//�д������ж�ʱ���ȴ��������жϡ�
			ActiveTask=TASK_COM2_IN;
		    ActiveCom=COM2;
	        Com2Proc();
		  }
		}
	  }

	  //*** Editor���� 
	  if(Tasking[TASK_PSW_EDIT]){
		Editor(SCREEN_PSW);
	  }


	  //*** ˯�߿���(Sleep control)
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
	  //��¼����ʱ�̡��ϱ�ʱ�̽�����һ�𣬿ɼ�������ReadBelts(1s֮��).


//����ͨ��,��Ҫ��ʽ#֧��
	  if(testbit(ComSW,bitSW_REPORT)){
	    if(fCfgGood){
		  if((Tasking[TASK_CALENDAR_INT_REP])||(Tasking[TASK_SENSOR_INT_REP])){
		    IntReports();				//�жϣ�����||��������
		
		  //  ModemRstIfNeeded();			//��ʧ���������㣬��λModem
	      }
	    }
	  }

	  //����(trap)
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
