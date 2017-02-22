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
//  发送gprs数据，可发送两个数组。
//
// 1、扩展数组长度=0时，数据可见（1拆2），只发送数组p。
// 2、扩展数组长度！=0时，数据为二进制，发送数组p和exp。（数据个数可较大）
//
//============================================================
BYTE USWAVE_transmit(BYTE *p,UINT len)			//,BYTE *exp,int len_exp)
{

    TX_IE_OFF();

	//发送常规实时数据
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
	  lcd_cursor(0,L3); lcd_puts("..........成功          ");
	  fReset=CalendarAutoAdjust(rx);
//	  Delay_s(1);
	}

	Delay_s(1);

	lcd_cursor(0,L1); lcd_puts("正在关闭........        ");
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
	  if((p-rx)>=32){		//Enough bytes received(16字节,hex)
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
// 建立Tcp连接,并执行Idea协议(发送id)
// 返回:
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

	Delay_s(1);		//测试表明，取消这一步会无法收到回执

//END_USWAVE_CONNECTION:
	return r;
}

