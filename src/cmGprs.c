#include 	"header.h"
#include    "h_macro.h"

extern BYTE		minTrans0;

extern BYTE	ActiveCom;
extern BYTE	ModemReady[];

extern BYTE ComStatus;

//extern BYTE Step[];			//�������STEPS_

//================================
//Bit usage variables
//================================
extern MSG0			Msg0;
extern MSG1			Msg1;
extern ERRORS		Error0;
extern TIMERS		Tmr;
extern TIMEUP 		TmrUp;



//============================================================
//  ����gprs���ݣ��ɷ����������顣
//
// 1����չ���鳤��=0ʱ�����ݿɼ���1��2����ֻ��������p��
// 2����չ���鳤�ȣ�=0ʱ������Ϊ�����ƣ���������p��exp�������ݸ����ɽϴ�
//
//============================================================
BYTE GPRS_transmit(BYTE *p,UINT len)			//,BYTE *exp,int len_exp)
{
//BYTE r;

    TX_IE_OFF();	//TX1_IE=0;

	//���ͳ���ʵʱ����
	ClrWdt();
	TransmitBytes(p,len);
	Delay_ms(200);
	return OK;
}


BYTE GPRS_end(void)
{
BYTE r;
BYTE fReset;
BYTE *rx;
	rx=RX_BUF0();
	
	Delay_ms(200);

	r=GetGprsResp();				//use rx[]

	if(r==OK){
	  lcd_puts(0,L3,"..........�ɹ�          ");
	  fReset=CalendarAutoAdjust(rx);
//	  Delay_s(1);
	}

	Delay_s(1);
	
//	Delay_s(2);

	lcd_puts(0,L1,"���ڹر�........        ");
	SendAT("+++",0);
	IsStrInStream(0,"OK",WITHIN_1S*3);
	Delay_s(1);
	SendAT("at+tcpclos",0x0d); IsStrInStream(0,"OK",WITHIN_1S*30);		//51);
	  
    Delay_s(1);                 	//delay for transmit last byte>500ms
    Serial_End_Tx();
	if(fReset==TRUE){Restart(RST_FOR_MAIN_94);}
	return r;
}

BYTE GetGprsResp(void)
{
BYTE r;
BYTE *rx,*p;
UINT EndMs;

	rx=RX_BUF0();

	EndMs=InitRx(30000,FILTER_GPRS_RESP);
	r=FAILED;

	while(1){
	  //Delay_ms(1);
	  p=getRxPtr();
	  if((p-rx)>=32){		//Enough bytes received(16�ֽ�,hex)
		r=CheckGprsResp(rx);
		break;
	  }
	  if(IsTimeUp(EndMs)==TRUE){r=FAILED; break;}
	}
	RX_IE_OFF();	
	return r;	  
}


BYTE CheckGprsResp(BYTE *p)
{
//char i;
BYTE r;			//*ptmp,r;
BYTE *rx;
UINT uLen;

	rx=RX_BUF0();

/*	ptmp=p;				//����

//'1'->0x01
	for(i=0;i<32;i++){
	  *p=ValueHex(*p);
	  p++; 
	}

//���������rx[0-15]
	p=ptmp;				//�ָ�
	for(i=0;i<32;i++){
	  *ptmp=(*p<<4)+*(p+1);
	  ptmp++;
	  p++;p++;
	}
	*/
	r=CheckCRC(rx,lenHEAD-2);
	if(r){
	  uLen=getUINT(rx+FRM_LEN)+1;
	  if(uLen>lenHEAD){
		r=CheckCRC(rx+DATA0,uLen-lenHEAD-2);
	  }
	}

	return r;
}


//=============================================
// ����Tcp����,��ִ��IdeaЭ��(����id)
// ����:
//		GPRS_CONNECTION_OK
//		GPRS_ERR_TCPPORT
//		GPRS_ERR_ATTACH
//		GPRS_ERR_OTCP
//=============================================
BYTE ConnectTcpServer(BYTE *ip,BYTE *port)
{
BYTE r;

	r=GPRS_CONNECTION_OK;
	Delay_s(1);

  	SendAT("at+tcpopen=\"",0x0);
  	TransmitStr(ip);
  	SendAT("\",",0x0);
  	TransmitStr(port);
	Serial_Transmit(0x0d);

	r=IsStrInStream(0,"CONNECT|ERR",WITHIN_1S*51);

	Delay_s(1);		//���Ա�����ȡ����һ�����޷��յ���ִ

//END_GPRS_CONNECTION:
	return r;
}

/*
{
BYTE r;

static UINT DelayMs;
BYTE *step;
	step=&Step[STEPS_CONNECT_TCP_SERVER];

	switch(*step){
	case 0:
	  r=GPRS_CONNECTION_OK;
	  DelayMs=ms_click+WITHIN_1S;		//Delay_s(1);
	  (*step)++;

	case 1:
	  if(!IsTimeUp(DelayMs)){return;}
	  (*step)++;

	case 2:	  
  	  SendAT("at+tcpopen=\"",0x0);
  	  TransmitStr(ip);
  	  SendAT("\",",0x0);
  	  TransmitStr(port);
	  Serial_Transmit(0x0d);
	  (*step)++;

	case 3:
	  r=IsStrInStream(0,"CONNECT|ERR",WITHIN_1S*51);

	Delay_s(1);		//���Ա�����ȡ����һ�����޷��յ���ִ

	}

//END_GPRS_CONNECTION:
	return r;
}*/

/*
BYTE ComTransparant(void)
{
BYTE errCode;
char pNodeAddress[10];
char ip[]="111.1.38.52";

	if(Connect(CONNECT_GPRM)==FAILED){
	  return GPRS_ERR_CONNECTIONSTOP;
	}

	Delay_s(1);
//
  	SendAT("at+tcpopen=\"",0x0);
	TransmitStr((BYTE *)&ip[0]);
  	SendAT("\",10009",0);
	Serial_Transmit(0x0d);

//
	if(IsStrInStream(0,"CONNECT",WITHIN_1S*51)==FALSE){
	  errCode=GPRS_ERR_TRANS; 
	  goto GPRS_TRANS_ERR;
	}

	Delay_s(1);

//���Ͳ�վid��
  	GetNodeAddress((char *)&pNodeAddress[0]);
	TransmitStr((BYTE *)&pNodeAddress[0]);

//��־
	ComStatus=STATUS_GPRS_TRANS;		//���ڡ�͸����ģʽ
	ModemReady[ActiveCom]=FALSE;

//��"͸��"��ʼSecond
	minTrans0=GetMinute();

	Delay_s(2);
	EndCom(); 
	return GPRS_CONNECTION_OK;

GPRS_TRANS_ERR:
	return errCode;
}
*/

BYTE GPRM_end(void)
{
BYTE Stat[12];
	while(1){
	  SendAT("AT+STATUS?",0x0d);
	  if(!IsStrInStream(0,"OK",WITHIN_1S*3)){		//״̬�仯����
	    return FAILED;
	  }
	  if(XModemRet(&Stat[0])){
		;			//�ж�XModem����ֵ
	  }
	}
	return OK;
}

//===================================
// У��XModem����ֵ
//===================================
BYTE XModemRet(BYTE *stat)
{
BYTE len,r;
BYTE *buf;
 
	r=OK;
	buf=RX_BUF0();
	if(!Txt2Bin(stat,buf,6)){	//6���ֽ�
	  return FAILED;
	}
	
	len=*(stat+1)+1;
	if(!(CheckCRC(stat,len-2))){
	  r=FAILED;
	}
	return r;
}
