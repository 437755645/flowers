#include 	"header.h"
#include    "h_macro.h"

extern BYTE 	IPL_CPU;
extern BYTE 	*pCmfg;
extern BYTE		*ptr_tx;

extern BYTE	Tasking[];
extern BYTE	ModemReady[];

extern BYTE	*ptr1_rx,rx1[];		//�����ջ��壺32+(2+4+2048)+2=2088	
extern BYTE	*ptr2_rx,rx2[];
extern BYTE	*ptr3_rx,rx3[];		//�������ӿ�	
extern BYTE *ptr4_rx;

extern volatile	UINT 	ms_click;
extern UINT		iTimeCmp;
extern BYTE 	filter_type;

//��ǰ�˿ںţ�1-3, ֻ��LCD��ʾʱΪ4
extern BYTE 	ActiveCom;
extern BYTE		ComOpen[];		//COM��ռ�ñ��

extern MSG2		Msg2;
extern BYTE 	ComStatus;
extern BYTE		minTrans0; 	


extern BYTE 	Step[];			//�������STEPS_


//================================
//Bit usage variables
//================================
extern MSG0			Msg0;
extern MSG1			Msg1;
extern ERRORS		Error0;
extern TIMERS		Tmr;
extern TIMEUP 		TmrUp;


//===========================================================================
void Serial_Transmit(BYTE x)
{
BYTE fBF;

	ClrWdt();
	switch(ActiveCom){
	case 1:				
		while(U1STAbits.UTXBF)continue;	
		U1TXREG=x;
		break;
	case 2:				
		while(U2STAbits.UTXBF)continue;	
		U2TXREG=x;
		break;
	case 3:				
		while(U3STAbits.UTXBF)continue;	
		U3TXREG=x;
		break;
	case 4:				
//		while(U4STAbits.UTXBF)continue;
	    do{
		  fBF=U4STAbits.UTXBF;
		}while(fBF);	

		U4TXREG=x;
		break;		
	}                 
}

//===============================================
//TXSTA=[CSRC=*,TX9=0,TXEN=1,SYNC=0,-,BRGH=1,TRMT=f,TX9D=0]	
//RCSTA=[SPEN=1,RX9=0,SREN=*,CREN=1,ADDEN=0,FERR,OERR,RX9D]	
//void Serial_Setup(UINT Baudrate)
void Serial_Setup(ULNG Baudrate)
{
	switch(ActiveCom){
	case 1:				
     	UART1Init(Baudrate);
		break;
	case 2:				
     	UART2Init(Baudrate);
		break;
	case 3:				
     	UART3Init(Baudrate);
		break;
	case 4:				
     	UART4Init(Baudrate);
		break;		
	}                 
}

void ClrUErr(void)
{
	switch(ActiveCom){
	case 1:				
 	  	U1STAbits.OERR=0;
      	break;
	case 2:				
 	  	U2STAbits.OERR=0;
 		break;
	case 3:				
 	  	U3STAbits.OERR=0;
 		break;
	case 4:				
	  	U4STAbits.OERR=0;
 		break;		
	}                 
}
		
//===============================================		
//Start a new receiption		
//Max Delay: maxDelayMS
//===============================================
UINT InitRx(UINT maxDelayMS,BYTE Filter)
{
BYTE i;	
UINT ipl_save;
BYTE *rx;

	di(ipl_save,7);

	rx=RX_BUF0();
	ClrRxBuf();
	Set_RxPtr(rx);		

	filter_type=Filter;				//�趨����
	fLongInst=FALSE;

	RX_IE_OFF();
	while(URXDA()){
	  i=URXREG();		//clear FIFO
	}

	ClrUErr();
	TX_IE_OFF();
	RX_IE_ON();	

	ei(ipl_save);							
//
	return (ms_click+maxDelayMS/2);
}



void Serial_End_Tx(void)
{
	switch(ActiveCom){
	case 1:
	  TX1_IE=0;	
	  //TX1_EN=0;	
	  //COM1_EN=0;	
	  break;
	case 2:
	  TX2_IE=0;	
	  //TX2_EN=0;	
	  //COM2_EN=0;	
	  break;
	case 3:
	  TX3_IE=0;	
	  //TX3_EN=0;	
	  //COM3_EN=0;	
	  break;
	}
}

BYTE *RX_BUF0(void)
{
	switch(ActiveCom){
	case 1:
	  return &rx1[0];
	case 2:
	  return &rx2[0];
	case 3:
	  return &rx3[0];
	}
	return 0;
}

BYTE *RX_BUF_END(void)
{			
	switch(ActiveCom){
	case 1:
	  return &rx1[RX_BUF_SIZE1];
	case 2:
	  return &rx2[RX_BUF_SIZE2];
	case 3:
	  return &rx3[RX_BUF_SIZE3];
	}
	return 0;
}

//===================================
void RS232_on(void)
{
	switch(ActiveCom){
	case 1:
	  SLEEP1_DIR=OUT; SLEEP1=1;	//RS232 wake up
	  break;
	case 2:
	  SLEEP2_DIR=OUT; SLEEP2=1;	//RS232 wake up
	  break;
	case 3:
	  SLEEP3_DIR=OUT; SLEEP3=1;	//RS232 wake up
	  break;
	}
}

void RS232_off(void)
{
	switch(ActiveCom){
	case 1:
	  SLEEP1_DIR=OUT; SLEEP1=0;	//go sleep
	  break;
	case 2:
	  SLEEP2_DIR=OUT; SLEEP2=0;	//go sleep
	  break;
	case 3:
	  SLEEP3_DIR=OUT; SLEEP3=0;	//go sleep
	  break;
	}
}

//===================================
// ����ActiveCom��Ӧ�˿ڵ�RX_IE״̬
//===================================
BYTE RX_IE(void)
{
	switch(ActiveCom){
	case 1:
	  return RX1_IE;
//	  break;
	case 2:
	  return RX2_IE;
//	  break;
	case 3:
	  return RX3_IE;
//	  break;
	case 4:
	  return RX4_IE;
//	  break;
	}
	return 0;
}

//===================================
// ����ActiveCom��Ӧ�˿ڵ�TRMT״̬
//===================================
BYTE TRMT(void)
{
	switch(ActiveCom){
	case 1:
	  return U1STAbits.TRMT;
//	  break;
	case 2:
	  return U2STAbits.TRMT;
//	  break;
	case 3:
	  return U3STAbits.TRMT;
//	  break;
	case 4:
	  return U4STAbits.TRMT;
//	  break;
	}
	return 0;
}

//===================================
// ����ActiveCom��Ӧ�˿ڵ�URXDA״̬
//===================================
BYTE URXDA(void)
{
BYTE r;
	r=0;
	switch(ActiveCom){
	case 1:
	  r=U1STAbits.URXDA;
	  break;
	case 2:
	  r=U2STAbits.URXDA;
	  break;
	case 3:
	  r=U3STAbits.URXDA;
	  break;
	case 4:
	  r=U4STAbits.URXDA;
	  break;
	}
	return r;
}


//===================================
// ����ActiveCom��Ӧ�˿ڵ�URXREG
//===================================
BYTE URXREG(void)
{
	switch(ActiveCom){
	case 1:
	  return U1RXREG;
//	  break;
	case 2:
	  return U2RXREG;
//	  break;
	case 3:
	  return U3RXREG;
//	  break;
	case 4:
	  return U4RXREG;
//	  break;
	}
	return 0;
}

BYTE *getRxPtr(void)
{
	switch(ActiveCom){
	  case 1:
	    return ptr1_rx;
	  case 2:
	    return ptr2_rx;
	  case 3:
	    return ptr3_rx;
	  case 4:
	    return ptr4_rx;
	  default:
	    break;
	}
	return 0;
}

void Set_RxPtr(BYTE *rx)
{
	switch(ActiveCom){
	  case 1:
	    ptr1_rx=rx;
		break;
	  case 2:
	    ptr2_rx=rx;
		break;
	  case 3:
	    ptr3_rx=rx;
		break;
	  case 4:
	    ptr4_rx=rx;
		break;
	  default:
	    break;
	}
}


void IncPtrRx(void)
{
	switch(ActiveCom){
	  case 1:
	    ptr1_rx++;
		break;
	  case 2:
	    ptr2_rx++;
		break;
	  case 3:
	    ptr3_rx++;
		break;
	  case 4:
	    ptr4_rx++;
		break;
	  default:
	    break;
	}
}


//=====================
void RX_IE_ON(void)
{
	switch(ActiveCom){
	case 1:
	  RX1_IE=1; return;
	case 2:
	  RX2_IE=1; return;
	case 3:
	  RX3_IE=1; return;
	case 4:
	  //RX4_IE=1; 
	  return;
	}
}

//=====================
void RX_IE_OFF(void)
{
	switch(ActiveCom){
	case 1:
	  RX1_IE=0; return;
	case 2:
	  RX2_IE=0; return;
	case 3:
	  RX3_IE=0; return;
	case 4:
	  RX4_IE=0; return;
	}
}

//===================================
// ����ActiveCom��Ӧ�˿ڵ�TX_IE״̬
//===================================
BYTE TX_IE(void)
{
	switch(ActiveCom){
	case 1:
	  return TX1_IE;
//	  break;
	case 2:
	  return TX2_IE;
//	  break;
	case 3:
	  return TX3_IE;
//	  break;
	case 4:
	  return TX4_IE;
//	  break;
	}
	return 0;
}

//=====================
void TX_IE_ON(void)
{
	switch(ActiveCom){
	case 1:
	  TX1_IE=1; return;
	case 2:
	  TX2_IE=1; return;
	case 3:
	  TX3_IE=1; return;
	case 4:
	  TX4_IE=1; return;
	}
}

//=====================
void TX_IE_OFF(void)
{
	switch(ActiveCom){
	case 1:
	  TX1_IE=0; return;
	case 2:
	  TX2_IE=0; return;
	case 3:
	  TX3_IE=0; return;
	case 4:
	  TX4_IE=0; return;
	}
}


BYTE IsComOpen(void)
{
	if(ComOpen[ActiveCom]){
	  return 1;
	}
	else{
	  return 0;
	}
}

void OpenComPort(void)
{
	ComOpen[ActiveCom]=1;
}


void CloseComPort(void)
{
	ComOpen[ActiveCom]=0;
}

BYTE IPL_URX_INT(void)
{
BYTE r;
	r=0;
	switch(ActiveCom){
	case 1:
	  r=IPL_U1RX_INT;
	  break;
	case 2:
	  r=IPL_U2RX_INT;
	  break;
	case 3:
	  r=IPL_U3RX_INT;
	  break;
	case 4:
	  r=IPL_U4RX_INT;
	  break;
	}
	return r;
}





/*
BYTE TaskCommXInt(void)
{
	switch(ActiveCom){
	case 1:
	  return task_Comm1Int;
	case 2:
	  return task_Comm2Int;
	case 3:
	  return task_Comm3Int;
	case 4:
	  return task_Comm4Int;
	}
}

void MarkTaskCommXInt(BYTE value)
{
	switch(ActiveCom){
	case 1:
	  task_Comm1Int=value;
	  return;
	case 2:
	  task_Comm2Int=value;
	  return;
	case 3:
	  task_Comm3Int=value;
	  return;
	case 4:
	  task_Comm4Int=value;
	  return;
	}
}
*/

//===================================
// ����ActiveCom��Ӧ�˿ڵ�RX_IF״̬
//===================================
/*
BYTE RX_IF(void)
{
	switch(ActiveCom){
	case 1:
	  return RX1_IF;
//	  break;
	case 2:
	  return RX2_IF;
//	  break;
	case 3:
	  return RX3_IF;
//	  break;
	case 4:
	  return RX4_IF;
//	  break;
	}
	return 0;
}
*/


void SetComTask(void)
{
	switch(ActiveCom){
	case 1:
	  Tasking[TASK_COM1_IN]=1;
	  break;
	case 2:
	  Tasking[TASK_COM2_IN]=1;
	  break;
//	case 3:
//	  Tasking[TASK_COM3_IN]=1;
//	  break;
//	case 4:
//	  Tasking[TASK_COM4_IN]=1;
//	  break;
	}
}

//=====================
void CLR_RX_IF(void)
{
	switch(ActiveCom){
	case 1:
	  RX1_IF=0; return;
	case 2:
	  RX2_IF=0; return;
	case 3:
	  RX3_IF=0; return;
	case 4:
	  RX4_IF=0; return;
	}
}

//=====================
void CLR_TX_IF(void)
{
	switch(ActiveCom){
	case 1:
	  TX1_IF=0; return;
	case 2:
	  TX2_IF=0; return;
	case 3:
	  TX3_IF=0; return;
	case 4:
	  TX4_IF=0; return;
	}
}




//===============================================
/*void SerialSend(char r)
{
//signed char i;

//Clr buffer
    FillBufWith(&p2[0],0x80,0x88);
	Serial_Transmit(r);		//Send "Ready" to Monitor
}*/

void ClrRxBuf(void)
{
UINT uLen;
BYTE *start,*end;

	start=RX_BUF0();
	end=RX_BUF_END();
	uLen=end-start+1;
    FillBufWith(start,uLen,0x88);
}

BYTE DoReceive(UINT EndMs)
{
BYTE t;

	if(IsTimeUp(EndMs)==TRUE){
	  return RX_OVER_TIME_OVERFLOW;
	}

	t=IsRxOver();
	return t;
}

		

//=================================
//         Transmit bytes
//------------------------------
//  Tx_Count: 
//	num of bytes to transmit
//=================================
void InitTx(BYTE *BufTx)
{
UINT ipl_save;

	di(ipl_save,7);	
	ptr_tx=BufTx;

	switch(ActiveCom){
	case 1:
  	  COM1_EN=1;		
	  TX1_EN=0;	TX1_EN=1;	//reset transmit
	  TX1_IE=1;				//transmit start
	  break;
	case 2:
  	  COM2_EN=1;		
	  TX2_EN=0;	TX2_EN=1;	//reset transmit
	  TX2_IE=1;				//transmit start
	  break;
	case 3:
  	  COM3_EN=1;	
	  TX3_EN=0;	TX3_EN=1;	//reset transmit
	  TX3_IE=1;				//transmit start
	  break;
	case 4:
  	  COM4_EN=1;		
	  TX4_EN=0;	TX4_EN=1;	//reset transmit
	  TX4_IE=1;				//transmit start
	  break;
	}		
	ei(ipl_save);
}				


//=============================
// CMFG"
// @1101-45[GPIP:7ae0ac2f1394]
// @2111-45[SMPN:13558108677]
// @3111-45[BDID:327680]
// @4111-45[DOMA:www.baidu.com]
// @5111-45[GPIP:da7f031004d2]
// @6111-45[GPIP:7f0000010000,c0a80101ffff]
// @7111-41[GPIP:7ae0ac2f1394]
// @8100-45[GPIP:7ae0ac2f1394]
// "
//=============================
BYTE InitAllComTasks(void)
{
BYTE ComPara[256];
BYTE lenStr;
BYTE *p;
BYTE lenCMFG;

	lenCMFG=*(CMC(CMC_CMFG_LEN));			//��������

	p=CMC(CMC_LINK);
	p=FindParaStr(p,"CMFG");
	if(!p){return FAILED;}

	p++;		//skip '"'
	while(1){
	  p=InStr(p,"@",lenCMFG);		//Ѱ�ҡ�@��
	  if(p){
	    lenStr=CopyParaStr(p,&ComPara[0],'@');
	    InitComTask(&ComPara[0]);
	
		p+=lenStr;
		if(lenCMFG>lenStr){
		  lenCMFG-=lenStr;
		}
		else{ break;}
	  }
	  else{break;}	
	}
	return OK;
}

/*
BYTE *PointToBackOf(const BYTE *caption)
{
	p=areaCOM(CMC_LINK);
	p=FindParaStr(p,"CMFG");
	if(!p){return FAILED;}
}*/

//======================================================
// ����@nXXXX-45[......]������ʼ��
// @n sw channel  protocol - min_gap [comtype:paras...]
//======================================================
BYTE InitComTask(BYTE *cmfg)
{
BYTE sw,channel;
BYTE *p,lenStr;

	lenStr=strlen((char *)cmfg);

	sw=X(*(cmfg+1));
	if(sw==OFF){return OK;}

	channel=X(*(cmfg+2));

	p=cmfg;
  	p=InStr(p,"[GPRS:",lenStr);		//Ѱ�ҡ�[��
	if(p){
	  p+=6;
	  return InitModem_GPRS(channel);
	}

	p=cmfg;
  	p=InStr(p,"[GPRM:",lenStr);		//Ѱ�ҡ�[��
	if(p){
	  p+=6;
	  return InitModem_GPRM(channel,p);
	}

	p=cmfg;
  	p=InStr(p,"[WIFI:",lenStr);		//Ѱ�ҡ�[��
	if(p){
	  p+=6;
	  return InitModem_WIFI(channel,p);
	}
	return OK;
}

//=====================================
// ��ȡһ��"@nXXXX-45[......]"
//         "#nT06,C[001234,101234567]"
// ���أ�
//      �ַ�������
// cSign: '@','#'
//=====================================
BYTE CopyParaStr(BYTE *p,BYTE *des,BYTE cSign)
{
BYTE *p0;

	p0=p;
	do{					//'@'��һ��������'"'����
	    *des++=*p++;
	}while((*p!=cSign)&&(*p!='"'));		
	*des=0;
	return p-p0;
}

//==================================
// Ⱥ��
//==================================
BYTE InitModem_GPRM(BYTE channel,BYTE *para)
{
BYTE *p;

	p=para;
	ActiveCom=channel;
	RX_IE_OFF();	//RX1_IE=FALSE;		//��ֹ�����źŴ���RING_ECHO
	SLEEP1_DIR=OUT; SLEEP1=1;	//RS232 wake up
	if(DetectSetBaud()==FAILED){return FAILED;}

	SendAT("at+gprsgrou=\"",0x0);
	while(*p!=']'){Serial_Transmit(*p++);}
	SendAT("\"",0xd);

	if(!IsStrInStream(0,"OK",WITHIN_1S*1)){
	  return FAILED;
	}
	return OK;
}


//==================================
// WIFI
//==================================
BYTE InitModem_WIFI(BYTE channel,BYTE *para)
{
	return 1;
}


//==================================
// ��׼GPRS
//==================================
BYTE InitModem_GPRS(BYTE channel)
{
	ActiveCom=channel;
	RX_IE_OFF();	//RX1_IE=FALSE;		//��ֹ�����źŴ���RING_ECHO
	SLEEP1_DIR=OUT; SLEEP1=1;	//RS232 wake up
	if(DetectSetBaud()==FAILED){return FAILED;}
//
#ifdef DEBUG_MODEM_ON
	SendAT("at#vall",0x0d);
	Delay_ms(100);
	Nop();

/*
	SendAT("at#apnserv=\"swj.zj\"",0x0d);
	Delay_ms(100);
	Nop();
*/

	SendAT("at&v0",0x0d);
	Delay_ms(100);
	Nop();

	SendAT("at&v1",0x0d);
	Delay_ms(100);
	Nop();

	SendAT("at+cgclass=\"B\"",0x0d);
	Delay_ms(100);
#endif
//
	SendAT("at+cmee=1",0x0d);
	Delay_ms(100);
	SendAT("at+wind=0",0x0d);	//ȡ��wind��ʾ
	Delay_ms(100);
	SendAT("at+ifc=0,0",0x0d);
	Delay_ms(100);
	SendAT("ate0v1",0x0d);
	Delay_ms(100);
	SendAT("at+cmgf=1",0x0d);
	Delay_ms(100);

//	SaveModemVersion();

//����
	SendAT("at&w",0x0d);
	Delay_s(1);

//mirror
//	SendAT("at+wmir",0x0d);		//��������
//	Delay_s(1);

//�˳�
//	TaskOver();	//modem in channel 1
	return OK;
}

//===================================
// Detect and set Baudrate
//===================================
BYTE DetectSetBaud(void)
{
	Serial_Setup(BAUD_19200);	//����19200
	SendAT("at",0x0d);
	if(!IsStrInStream(0,"OK",WITHIN_1S*1)){
	  //����9600
	  Serial_Setup(BAUD_9600);	
	  SendAT("at",0x0d);
	  if(IsStrInStream(0,"OK",WITHIN_1S*1)){
	    SendAT("at+ipr=19200",0x0d);	//9600->19200
	    Delay_ms(100);					//delay for transmit finished
	    Serial_Setup(BAUD_19200);		//  before switching baudrate
	  }

	  //����115200
	  else{
		Serial_Setup(BAUD_115200);	
		SendAT("at",0x0d);
		if(IsStrInStream(0,"OK",WITHIN_1S*1)){
	  	  SendAT("at+ipr=19200",0x0d);	//115200->19200
	  	  Delay_ms(100);				//delay for transmit finished
	  	  Serial_Setup(BAUD_19200);		//  before switching baudrate
		}
		else{
		  return FAILED;		//���г���ʧ��
		}
	  }
	}
	Delay_ms(100);
	return OK;
}

//================================
// �ж��Ƿ���ҪReset Modem
// ������
//		1.fModemRstNeeded==TRUE��ÿ����CSQ���515����
//		2.����ʧ�ܴ��������趨ֵ
//		(��)
//================================
void ModemRstIfNeeded(void)
{
UINT modem_fail;

	if(fModemRstNeeded==FALSE){
	  return;
	} 

	ReadCount(mapCNT_MODEM_FAIL,&modem_fail);
	if((modem_fail % 4)==FALSE){return;}

//ʧ�ܷ�4��λ
	WriteCount(mapCNT_MODEM_FAIL,modem_fail);
	MsgBox("Reset Modem...  ");
	Delay_s(1);       
	ResetPeri(10);		//��ָ�λModem
}


/*
//=== Reset Modem 4 seconds ===
//This will be useful when modem is not stable.
char ResetModem(void)
{
char wait,modem_status,i;

	RS232_off();			//cut-off influnence between terminal and modem.
//
	SendAT("at+cfun=0",0x0d); Delay_s(1);
	PW_NON_off(); 			//reset all peripheral
//
	Delay_s(4);
	PW_NON_on(); 			//reset all peripheral
	Delay_s(1);
//
	RS232_on();
	SendAT("at+cfun=1",0x0d); Delay_s(1);

	wait=100;
	while(wait--){
	  Delay_ms(200);		//delay for stablizing
	  SendAT("at",0x0d);
	  if(Get_AT_Response(1)==OK){
		modem_status=OK;
		goto MDM_RESET_RET;
	  }
	}

	modem_status=FAILED;
MDM_RESET_RET:
	TaskOver(1); 		//����ضϹ��̿��ܲ������ж�
	TaskOver(2);		//����ضϹ��̿��ܲ������ж�
	return modem_status;
}
*/



/*
//====================================================
//Detect ring-in pin
//A ring-in signal should keep low for at least 100ms
//====================================================
BYTE IsRingIn(void)
{
BYTE x,r;
//Delay 100ms

	x=100;
	while(x!=0){
	  if(PORTBbits.INT0){
		Nop();
		r=0; goto RRR;
//		return FALSE;
	  }
	  Delay100TCYx(28);
	  x--;
	}
	r=1;
RRR:
	return r;
//	return TRUE;
}
*/


//===========================
// When pin RING-IN valid
//===========================
/*BYTE IsRingIn(void)
{
	ActiveCom=COM1;
	Connect(CONNECT_NULL);
//	i=IsStrInStream(0,"RING",WITHIN_1S*8);

	if(fRingIn==TRUE){
	  Serial_Transmit(0x0d); Delay_ms(100);
	  SendAT("ath",0x0d);		
	  fRingIn=FALSE;
	  return TRUE;
	}	
	return FALSE;
}
*/

//===========================================
// ffffffffffff => "122.224.172.153",25005
// r=0,��ʽ����ʱ;
// r=1,OK
//===========================================
BYTE getIp(BYTE *des,BYTE *src)
{
BYTE ip[8];
//UINT port;

	if(!Txt2Bin(&ip[0],src,4)){return FAILED;}
//
	sprintf((char *)des,"%u.%u.%u.%u",ip[0],ip[1],ip[2],ip[3]);
	return OK;
}

BYTE getPort(BYTE *des,BYTE *src)
{
BYTE port[8];
UINT uPort;

	if(!Txt2Bin(&port[0],src,2)){return FAILED;}
//
	uPort=getUINT(&port[0]);
	sprintf((char *)des,"%u",uPort);
	return OK;
}



//===========================
// When pin RING-IN invalid 
//===========================
BYTE IsRingIn(void)
{
BYTE i;
//	ActiveCom=COM1;
	Connect(CONNECT_NULL);
	i=IsStrInStream(0,"RING",WITHIN_1S*8);

	if(i==TRUE){
	  Serial_Transmit(0x0d); Delay_ms(100);
	  SendAT("ath",0x0d);		
	}	
	return i;
}


//=============================================
// �ҵ���һ����������ͨ��channel��Ӧ��ͨ�Žṹ
//
// ����: "ͨ������"
// channel: 1-4
//=============================================
/*
BYTE GetComType(BYTE channel)
{
char index;
BYTE bCom,sw,TT;
UINT uAddr;

	uAddr=REG0+(UINT)MAIN_COM1;
	for(index=0;index<4;index++){
	  bCom=GetByte(M_CFG,uAddr);		//[S LLLL TT B]
	  sw=bCom>>7;
	  if(sw){
	  	TT=((bCom>>1)&0x03)+1;
		if(TT==channel){
		  return (bCom>>3)&0x0f;		//����"ͨ������"
		}
	  }
	  uAddr+=8;
	}
	return CONNECT_ERROR;
}
*/

//=============================
// ��com1���ͽ��յ���atָ��
//=============================
/*
void RepeatAT(BYTE *at)
{
//    while(*at!=0){Serial_Transmit(*at); at++;}
}
*/



/*
//==============================
// ����at+cmgr?���صİ汾��,�趨
// MODEMʶ���־,�Լ��ݲ�ͬ�汾
// ��MODEM
//==========================
void SaveModemVersion(void)
{
BYTE modem_version;

	modem_version=1;							//�°汾��ȱʡ
	SendAT("at+cgmr",0x0d);			
	if(IsStrInStream("64",WITHIN_1S)==TRUE){
	  if((Stream[2]=='6')&&(Stream[3]=='4')){
	    modem_version=0; 						//�ɰ汾
	  }
	}

//����
	i2c_WriteBytes16(M_CFG,&modem_version,MAP_REG0+(UINT)MAIN_MODEM_VERSION,1);
}

*/


//==============================
// ����at+cmgr?���صİ汾��,�趨
// MODEMʶ���־,�Լ��ݲ�ͬ�汾
// ��MODEM
//==========================
/*
void ShowModemVersion(void)
{
BYTE *str;

	SendAT("at+cgmr",0x0d);			
	IsStrInStream(0,"OK",WITHIN_1S);
	Stream[63]='\0';	//ǿ�ƽ�β
	lcd_clear();
	str=&Stream[0];
	for(;*str!=0; str++){        //����ֹͣ��0����
	  if((*str==0x0d)||(*str==0x0a))continue;
//??	  lcd_write(LCD_DATA,*str);
	}
//	Delay_s(1);
//	TaskOver(); 
}
*/

//====================================================
// Mark modem ready status
// 1������ʱ��fModemReady=FALSE;
// 2�����fModemReady=FALSE��ʱ��ÿ���Ӳ�ѯһ��״̬��
// ֱ��fModemReady=TRUE
//====================================================
/*
void RefreshModemStatus(BYTE COMi)
{
    if(ComStatus==STATUS_GPRS_MSG){
	  ActiveCom=COMi;
	  RX_IE_OFF();		//��ֹ�����źŴ���RING_ECHO

	  Connect(CONNECT_NULL);	
	  SendAT("at+creg?",0x0d);			
	  if(IsStrInStream(0,"OK",WITHIN_1S)){
	    fModemReady=FALSE;
	    if((Stream[11]=='1')||(Stream[11]=='5')){
	      fModemReady=TRUE;
	    }
	  }
	  else{	//����Ӧ
	    ComStatus=STATUS_GPRS_TRANS;		//�����ڡ�͸����ģʽ
	  }

//for debug
//	SendAT("at+csmp?",0x0d);			
//	Delay_s(1);

	  Delay_ms(200);
   	  CLR_RX_IF();		//RX1_IF=0;	//���ͨ���ڼ����µ��жϱ�־
      RX_IE_ON();			//RX1_IE=TRUE;		//����RING_ECHO
	}
}
*/

void AT_creg(BYTE COMi)
{
BYTE save;
BYTE *buf;
	
	save=ActiveCom;

	ActiveCom=COMi;
	RX_IE_OFF();		//��ֹ�����źŴ���RING_ECHO
	Connect(CONNECT_NULL);	

	ModemReady[COMi]=FALSE;
	buf=RX_BUF0();
	SendAT("at+creg?",0x0d);			
	if(IsStrInStream(0,"OK",WITHIN_1S*2)){
	  if((*(buf+11)=='1')||(*(buf+11)=='5')){
//	  if((Stream[11]=='1')||(Stream[11]=='5')){
	    ModemReady[COMi]=TRUE;
	  }
	}

//for debug
//	SendAT("at+csmp?",0x0d);			
//	Delay_s(1);

	Delay_ms(200);
   	CLR_RX_IF();		//RX1_IF=0;	//���ͨ���ڼ����µ��жϱ�־
    RX_IE_ON();			//RX1_IE=TRUE;		//����RING_ECHO

	ActiveCom=save;
}




void IsTransparantOver(void)
{
BYTE min;

	if(ComStatus==STATUS_GPRS_TRANS){
	  min=GetMinute();
	  if(min<minTrans0){min+=60;}
	  if((min-minTrans0)>5){		//5-6 min�޽�������
		ComStatus=STATUS_GPRS_MSG;

//		MsgBox("Close transparant mode..");
		ResetPeri(1);
	  }
	}
}

//===============================
//        ��ʼ��ͨ�ſ�:
// ��ʹ�����¼��ܵõ���Ӧ
//===============================
void InitCom(void)
{
	ActiveCom=COM1;
	UART1Init(BAUD_19200);
	RS232_off();			//cut-off influnence between terminal and modem.
	EndCom();

	ActiveCom=COM2;
	UART2Init(BAUD_19200);
	RS232_off();			//cut-off influnence between terminal and modem.
	EndCom();

	ActiveCom=COM3;
	UART3Init(BAUD_9600);
	RS232_off();			//cut-off influnence between terminal and modem.
	EndCom();
}

//====================================
//            ͨ����·ά��
//
// CMFG"
// @1101-45[GPIP:7ae0ac2f1394]
// @2111-45[SMPN:13558108677]
// @3111-45[BDID:327680]
// @4111-45[DOMA:www.baidu.com]
// @5111-45[GPIP:da7f031004d2]
// @6111-45[GPIP:7f0000010000,c0a80101ffff]
// @7111-41[GPIP:7ae0ac2f1394]
// @8100-45[GPIP:7ae0ac2f1394]
// "
//====================================
void ComMaintenance(BYTE ComSW)
{
BYTE comtsk_id,COMi;
BYTE sw,typeComm;
BYTE *p;

//com�ܿ���
	if(!testbit(ComSW,bitSW_REPORT)){return;}

//�ҵ�"CMFG"
	p=CMC(CMC_LINK);
	p=FindParaStr(p,"CMFG");
	if(!p){return;}
	p++;					//��ʼ����'"',skip 
	if(*p=='"'){return;}	//��������

	//��ѯȫ��ͨ������pָ�� @nscp...
	while(p){
	  sw=X(*(p+2));			//2�����񿪹�sw		
	  if(sw){		
	    comtsk_id=X(*(p+1));
	    COMi=X(*(p+3))+1;	//3:ָ��ͨ��channel
	    typeComm=GetComType(p);
	    switch(typeComm){
	      case CONNECT_SM:
	      case CONNECT_GPRS:
	      case CONNECT_GPRM:
			if(ComOpen[COMi]==FALSE){
	          AT_creg(COMi);
			}
		    break;

		  case CONNECT_GPRS_TRANS:
			if(ComOpen[COMi]==FALSE){
			  HeartBeat(COMi);
			}
			break;

	      default: break;
	    }
	  }
	  p=NextTask(p);
	}
}

void HeartBeat(BYTE COMi)
{
	ActiveCom=COMi;
}

//===========================
// ָ����һ��CMFGָ��
// ��β��ʱ������0
//===========================
BYTE *NextTask(BYTE *pCmfg)
{
BYTE *p;

	p=pCmfg;
	while(1){
	  if(*p=='"'){		//CMFG��������'"'
		return 0;		//CMFG ����
	  }
	  else{
		p++;
		if((*p)=='@'){
		  return p;
		}
	  }
	}
}

//===========================================================
// Find specific char in stream within limited time,
//
// start: ��ʼ�㣬��0��ʼ�������յ��ĵ�һ��byte
//
// sCmp: �ַ������ɸ��ϣ��ɡ�|���ָ��������OK������OK|ERR��
//
//
// Return: ���ϵ��ַ�����ţ���1��ʼ
//
//		0:��ʱ��
//		1���յ���һ���ַ���
//		2:�յ��ڶ����ַ�
//
//		TRUE:  exist;
//		FALSE: not exist;
//==============================================
char IsStrInStream(BYTE start,const char *sCmp,UINT LimitMS)		
{
UINT end_ms;
BYTE r,i;		//,L;			
BYTE *rx,*rx0,*old,*p;			
BYTE sFind[2][MINI_STR_LIMIT];		//MINI_STR_LIMIT: ÿ���ַ���������
BYTE numCmp;	//�ַ����ĸ���
BYTE ipl_save;

//�ָ��ַ���
	numCmp=Split(sCmp,&sFind[0][0]);

//	di(ipl_save,IPL_URX_INT()-1);		//����UxRX_INT�ж�	
	di(ipl_save,0);						//����UxRX_INT�ж�	
	Timer2_on();

	OpenComPort();
	end_ms=InitRx(LimitMS,FILTER_NULL);

	rx=RX_BUF0();
	rx0=rx+start;
	old=rx;
	r=FALSE;
	while(1){
      ClrWdt();

	  rx=getRxPtr();
	  if(rx>old){
		old=rx;
		for(i=0;i<numCmp;i++){				//�Ƚ�ÿ���ַ���
		  p=InStr(rx0,(const char *)&sFind[i],(UINT)(rx-rx0));
	  	  if(p){
		    r=i+1;			//���ϵ��ַ�����ţ���1��ʼ
		    goto END_IN_STREAM;
		  }
		}
	  }
	
	  if(IsTimeUp(end_ms)==TRUE){
		r=0;		//FALSE
		break;
	  }
	}

END_IN_STREAM:
	ei(ipl_save);	//restore interrupts
	return r;
}

/*
char IsStrInStream2(BYTE start,const char *sCmp,UINT LimitMS)		
{
UINT end_ms;
BYTE r,i;		//,L;			
BYTE *rx,*rx0,*old,*p;			
BYTE sFind[2][MINI_STR_LIMIT];		//MINI_STR_LIMIT: ÿ���ַ���������
BYTE numCmp;	//�ַ����ĸ���
BYTE ipl_save;

BYTE *step;
	step=&Step[STEPS_IS_STR_IN_STREAM];

	switch(*step){
	case 0:
	  //�ָ��ַ���
	  numCmp=Split(sCmp,&sFind[0][0]);

	  //	di(ipl_save,IPL_URX_INT()-1);		//����UxRX_INT�ж�	
	  di(ipl_save,0);						//����UxRX_INT�ж�	
	  Timer2_on();

	  OpenComPort();
	  end_ms=InitRx(LimitMS,FILTER_NULL);

	  rx=RX_BUF0();
	  rx0=rx+start;
	  old=rx;
	  r=FALSE;
	  (*step)++;

	case 1:
//      ClrWdt();
	  rx=getRxPtr();
	  if(rx>old){
		old=rx;
		for(i=0;i<numCmp;i++){				//�Ƚ�ÿ���ַ���
		  p=InStr(rx0,(const char *)&sFind[i],(UINT)(rx-rx0));
	  	  if(p){
		    r=i+1;			//���ϵ��ַ�����ţ���1��ʼ
		    goto END_IN_STREAM;
		  }
		}
	  }
	
	  if(IsTimeUp(end_ms)){
		r=FAILED;		//FALSE
		break;
	  }
	  r=RELEASE;
	  return r;
	}

END_IN_STREAM:
	ei(ipl_save);	//restore interrupts
	return r;
}
*/
