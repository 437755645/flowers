#include 	"header.h"
#include    "h_macro.h"

/*
extern BYTE		minTrans0;

extern BYTE		rx[];
extern BYTE		*p;

//extern UINT	iTimeCmp;
extern BYTE ComStatus;

//================================
//Bit usage variables
//================================
extern TASKLINE 	TaskLine; 
extern MSG0			Msg0;
extern MSG1			Msg1;
extern ERRORS		Error0;
extern TIMERS		Tmr;
extern TIMEUP 		TmrUp;
*/

/*
//============================================================
//  发送gprs数据，可发送两个数组。
//
// 1、扩展数组长度=0时，数据可见（1拆2），只发送数组p。
// 2、扩展数组长度！=0时，数据为二进制，发送数组p和exp。（数据个数可较大）
//
//============================================================
BYTE GPRS_transmit(BYTE *p,UINT len)			//,BYTE *exp,int len_exp)
{
//BYTE r;

    TX_IE_OFF();	//TX1_IE=0;

	//发送常规实时数据
	ClrWdt();
	TransmitBytes(p,len);
	Delay_ms(200);
	return OK;
}


BYTE GPRS_end(void)
{
BYTE r;
BYTE fReset;

	Delay_ms(200);

	r=GetGprsResp();				//use rx[]

	if(r==OK){
	  lcd_cursor(0,L3); lcd_puts("..........成功          ");
	  Delay_s(1);
	}

	if(r==OK){
	  fReset=CalendarAutoAdjust(&rx[0]);
	}

	Delay_s(2);

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

BYTE GetGprsResp(void)
{
BYTE r;

	InitRx(30000,FILTER_GPRS_RESP);
	r=FAILED;

	while(1){
	  //Delay_ms(1);
	  if((ptr_rx-&rx[0])>=32){		//Enough bytes received(16字节,hex)
		r=CheckGprsResp(&rx[0]);
		break;
	  }
	  if(IsTimeUp(iTimeCmp)==TRUE){r=FAILED; break;}
	}
	RX_IE_OFF();	
	return r;	  
}


BYTE CheckGprsResp(BYTE *p)
{
char i;
BYTE *ptmp,r;

	ptmp=p;				//保存

//'1'->0x01
	for(i=0;i<32;i++){
	  *p=ValueHex(*p);
	  p++; 
	}

//结果保存在rx[0-15]
	p=ptmp;				//恢复
	for(i=0;i<32;i++){
	  *ptmp=(*p<<4)+*(p+1);
	  ptmp++;
	  p++;p++;
	}
	
	r=CheckCRC(&rx[0],lenHEAD-2);
	return r;
}
*/

//===============================================
// ip_port: ffffffff-ffff
//===============================================
char WifiConnection(BYTE *ip_port,BYTE *psw)
{
BYTE err_wifi=1;
BYTE sPort[10],sIp[20];

	while(1){
	  //A  =>	+ok
	  SendAT("+++",0x0);
	  if(IsStrInStream(1,"+ok",WITHIN_1S*5)==FALSE){
	    err_wifi=0;
	    break;
	  }

	  //at+wmode=apsta =>	+ok
	  SendAT("at+wmode=apsta",0x0d);
	  if(IsStrInStream(1,"+ok",WITHIN_1S*2)==FALSE){
	    err_wifi=-1;
	    break;
	  }

/* Skip
at+wscan	+ok=
Ch,SSID,BSSID,Security,Indicator
1,ZJSW_GUEST,3C:E5:A6:61:B4:40,WPA2PSK/AES,76
1,ZJSW,3C:E5:A6:61:B4:41,OPEN/NONE,74
1,ZJSW_GUEST,C4:CA:D9:27:ED:C0,WPA2PSK/AES,44
1,ZJSW,C4:CA:D9:27:ED:C1,OPEN/NONE,46
1,TP-LINK_qiandan,D0:C7:C0:CC:A0:10,WPAPSKWPA2PSK/AES,0
……
*/

	  //AT+WSSSID=ZJSW  =>	+ok
	  SendAT("at+wmode=apsta",0x0d);
	  if(IsStrInStream(1,"+ok",WITHIN_1S*3)==FALSE){
	    err_wifi=-2;
	    break;
	  }

	  //AT+WSKEY=wpa2psk,aes,psw  =>  +ok
	  SendAT("AT+WSKEY=wpa2psk,aes,",0x0);
	  SendAT((const char *)psw,0x0d);
	  if(IsStrInStream(1,"+ok",WITHIN_1S*5)==FALSE){
	    err_wifi=-3;
	    break;
	  }
	  
	  //AT+NETP=TCP,Client,40071, 115.236.28.50	=> +ok
	  getPort(&sPort[0],ip_port+8);
	  getIp(&sIp[0],ip_port);

	  SendAT("AT+NETP=TCP,Client,",0x0);
	  TransmitStr(&sPort[0]);
	  Serial_Transmit(',');
	  SendAT((const char *)&sIp[0],0x0d);
	  if(IsStrInStream(1,"+ok",WITHIN_1S*10)==FALSE){
	    err_wifi=-4;
	    break;
	  }

	  //at+z =>	+ok
	  SendAT("at+z",0x0d);
	  if(IsStrInStream(1,"+ok",WITHIN_1S*3)==FALSE){
	    err_wifi=-5;
	    break;
	  }

	  //+++ =>	A
	  SendAT("+++",0x0);
	  if(IsStrInStream(1,"+ok",WITHIN_1S*3)==FALSE){
	    err_wifi=-6;
	    break;
	  }

	  //A => +ok
	  SendAT("A",0x0d);
	  if(IsStrInStream(1,"+ok",WITHIN_1S*3)==FALSE){
	    err_wifi=-7;
	    break;
	  }

	  //at+tcplk =>	+ok=on
	  SendAT("at+tcplk",0x0d);
	  if(IsStrInStream(1,"+ok=on",WITHIN_1S*3)==FALSE){
	    err_wifi=-8;
	    break;
	  }
	  break; 
	}
	return err_wifi;
}

/*
//====================================
// 建立Tcp连接,并执行Idea协议(发送id)
// 返回:
//		GPRS_CONNECTION_OK
//		GPRS_ERR_TCPPORT
//		GPRS_ERR_ATTACH
//		GPRS_ERR_OTCP
//====================================
BYTE ConnectTcpServer(BYTE *ip_port)
{
BYTE errCode;

	Delay_s(1);

  	SendAT("at+tcpopen=",0x0);
  	TransmitStr(ip_port);		//,0);
	Serial_Transmit(0x0d);

//	SendIp(com_index);	
	if(IsStrInStream(1,"CONNECT",WITHIN_1S*51)==FALSE){
	  errCode=GPRS_ERR_ATTACH; 
	  goto GPRS_ERR;
	}

	Delay_s(1);		//测试表明，取消这一步会无法收到回执

	return GPRS_CONNECTION_OK;

GPRS_ERR:
	return errCode;
}


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
	if(IsStrInStream(1,"CONNECT",WITHIN_1S*51)==FALSE){
	  errCode=GPRS_ERR_TRANS; 
	  goto GPRS_TRANS_ERR;
	}

	Delay_s(1);

//发送测站id号
  	GetNodeAddress((char *)&pNodeAddress[0]);
	TransmitStr((BYTE *)&pNodeAddress[0]);

//标志
	ComStatus=STATUS_GPRS_TRANS;		//处于“透传”模式
	fModemReady=FALSE;

//设"透传"起始Second
	minTrans0=GetMinute();

	Delay_s(2);
	TaskOver(); 
	return GPRS_CONNECTION_OK;

GPRS_TRANS_ERR:
	return errCode;
}

BYTE GPRM_end(void)
{
BYTE Stat[12];
	while(1){
	  SendAT("AT+STATUS?",0x0d);
	  if(!IsStrInStream(1,"OK",WITHIN_1S*3)){		//状态变化提醒
	    return FAILED;
	  }
	  if(XModemRet(&Stat[0])){
		;			//判断XModem返回值
	  }
	}
	return OK;
}

//===================================
// 校验XModem返回值
//===================================
BYTE XModemRet(BYTE *stat)
{
BYTE len,r;
 
	r=OK;
	if(!Txt2Bin(stat,&Stream[0],6)){	//6个字节
	  return FAILED;
	}
	
	len=*(stat+1)+1;
	if(!(CheckCRC(stat,len-2))){
	  r=FAILED;
	}
	return r;
}
*/
