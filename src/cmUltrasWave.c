#include 	"header.h"
#include    "h_macro.h"

extern BYTE		minTrans0;

extern BYTE	ActiveCom;
extern BYTE	ModemReady[];

extern BYTE ComStatus;

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
BYTE USWAVE_transmit(BYTE *p,UINT len)			//,BYTE *exp,int len_exp)
{

    TX_IE_OFF();

	//���ͳ���ʵʱ����
	ClrWdt();
	TransmitBytes(p,len);
	Delay_ms(200);
	return OK;
}


BYTE USWAVE_end(void)
{
BYTE r;
BYTE fReset;
BYTE *rx;
	rx=RX_BUF0();
	
	Delay_ms(200);

	r=GetUSWaveResp();				//use rx[]

	if(r==OK){
	  lcd_cursor(0,L3); lcd_puts("..........�ɹ�          ");
	  fReset=CalendarAutoAdjust(rx);
//	  Delay_s(1);
	}

	Delay_s(1);

	lcd_cursor(0,L1); lcd_puts("���ڹر�........        ");
	SendAT("+++",0);
	IsStrInStream(1,"OK",WITHIN_1S*3);
	Delay_s(1);
	SendAT("at+tcpclos",0x0d); IsStrInStream(1,"OK",WITHIN_1S*30);		//51);
	  
    Delay_s(1);                 	//delay for transmit last byte>500ms
    Serial_End_Tx();
	if(fReset==TRUE){Restart(RST_FOR_MAIN_94);}
	return r;
}

BYTE GetUSWaveResp(void)
{
BYTE r;
BYTE *rx,*p;
UINT EndMs;

	rx=RX_BUF0();

	EndMs=InitRx(30000,FILTER_USWAVE_RESP);
	r=FAILED;

	while(1){
	  p=getRxPtr();
	  if((p-rx)>=32){		//Enough bytes received(16�ֽ�,hex)
		r=CheckUSWaveResp(rx);
		break;
	  }
	  if(IsTimeUp(EndMs)==TRUE){r=FAILED; break;}
	}
	RX_IE_OFF();	
	return r;	  
}


BYTE CheckUSWaveResp(BYTE *p)
{
BYTE r;
BYTE *rx;
UINT uLen;

	rx=RX_BUF0();
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
//		USWAVE_CONNECTION_OK
//		USWAVE_ERR_TCPPORT
//		USWAVE_ERR_ATTACH
//		USWAVE_ERR_OTCP
//=============================================
BYTE ConnectTcpServer(BYTE *ip,BYTE *port)
{
BYTE r;

	r=USWAVE_CONNECTION_OK;
	Delay_s(1);

  	SendAT("at+tcpopen=\"",0x0);
  	TransmitStr(ip);
  	SendAT("\",",0x0);
  	TransmitStr(port);
	Serial_Transmit(0x0d);

	r=IsStrInStream(1,"CONNECT|ERR",WITHIN_1S*51);

	Delay_s(1);		//���Ա�����ȡ����һ�����޷��յ���ִ

//END_USWAVE_CONNECTION:
	return r;
}

