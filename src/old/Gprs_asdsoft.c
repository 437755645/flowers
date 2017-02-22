#include 	"header.h"
#include    "h_macro.h"


extern BYTE		rx[];
extern BYTE		*ptr_rx;

//extern BYTE modem_version;
extern UINT	iTimeCmp;
extern BYTE Stream[];


//================================
//Bit usage variables
//================================
extern TASKLINE 	TaskLine; 
extern MSG0			Msg0;
extern MSG1			Msg1;
extern ERRORS		Error0;
extern TIMERS		Tmr;
extern TIMEUP 		TmrUp;


//=================================================================
/*
BYTE SetGprsPara(void)
{
//BYTE modem_version;

	SendAT("at+wopen=1",0x0d);
	Delay_s(1);

//	modem_version=GetByte(M_CFG,MAP_REG0+((UINT)(MAIN_MODEM_VERSION)));

//modem_version=0;

	if(modem_version==1){
	  SendAT("at#apnserv=\"cmnet\"",0x0d); 	
	  if(IsStrInStream(1,"OK",WITHIN_1S)==FALSE){
			modem_version++;
	    goto SET_GPRS_ERR;
	  }

	  Send_tcptxdelay();
	  if(IsStrInStream(1,"OK",WITHIN_1S)==FALSE){
modem_version++;
	    goto SET_GPRS_ERR;
	  }
	}
	return OK;

SET_GPRS_ERR:
	return FAILED;
}
*/

/*
BYTE GPRS_connection(void)
{
BYTE errCode;

//Old Modem
	if(modem_version==1){
	  SendAT("at#gprsmode=1",0x0d); 	
	  if(IsStrInStream(1,"OK",WITHIN_1S)==FALSE){
	    errCode=GPRS_ERR_GPRSMODE; 
	    goto GPRS_ERR;
	  }

	  SendAT("at+cgreg=1",0x0d); 				
	  if(IsStrInStream(1,"OK",WITHIN_1S*2)==FALSE){
	    errCode=GPRS_ERR_CGREG; 
	    goto GPRS_ERR;
	  }
	  Delay_ms(200);
//
	  SendAT("at+cgatt=1",0x0d); 
	  if(IsStrInStream(1,"OK",WITHIN_1S*12)==FALSE){
	    errCode=GPRS_ERR_CGATT; 
	    goto GPRS_ERR;
	  }

//�����Ƿ�ɹ�,������.(35840): �����Ѵ���	
	  Delay_s(1);
	  SendAT("at#connectionstart",0x0d);
	  if(IsStrInStream(1,"35840",WITHIN_1S*8)==TRUE){
	  }		
	}

//asdsoft Modem
	else{}
	return GPRS_CONNECTION_OK;

GPRS_ERR:
	if(InStr(&Stream[0],"37123",0x20)){
	  ResetPeri();
	}
	return errCode;
}
*/

//============================================================
//  ����gprs���ݣ��ɷ����������顣
//
// 1����չ���鳤��=0ʱ�����ݿɼ���1��2����ֻ��������p��
// 2����չ���鳤�ȣ�=0ʱ������Ϊ�����ƣ���������p��exp�������ݸ����ɽϴ�
//
//============================================================
BYTE GPRS_transmit(BYTE *p,int len,BYTE *exp,int len_exp)
{
BYTE r;

    TX_IE_OFF();	//TX1_IE=0;

	//���ͳ���ʵʱ����
	ClrWdt();
	while(len!=0){
	  Serial_TransmitHL(*p);		//1=��2�����Ϳɼ�����
	  p++; len--;
	}

	while(len_exp!=0){
	  Serial_TransmitHL(*exp);		//��չ����
	  exp++; len_exp--;
	}


/*	//���Ϳ�����(������)
	else{
	  for(i=0;i<16;i++){
	    Serial_Transmit(*p);		//���Ϳɼ�����
	    p++; len--;
	  }
							//len_exp!=0
	  while(len_exp!=0){
	    Serial_Transmit(*exp);		//������byte
	    exp++; len_exp--;
	  }
	}
*/

	Delay_ms(200);

	r=GPRS_end();
	return r;				
}


BYTE GPRS_end(void)
{
BYTE r;
BYTE fReset;

	Delay_ms(200);

	r=GetGprsResp();				//use rx[]

	if(r==OK){
	  lcd_cursor(0,L3); lcd_puts("..........�ɹ�          ");
	  Delay_s(1);
	}

	if(r==OK){
	  fReset=CalendarAutoAdjust(&rx[0]);
	}

	Delay_s(2);
//	if(modem_version==1){
//  	  Serial_Transmit(0x03);     		//�ر�����,important for server detach.	
//	}
//	else{
	  lcd_cursor(0,L1); lcd_puts("���ڹر�........        ");
	  SendAT("+++",0);
	  IsStrInStream(1,"OK",WITHIN_1S*3);
	  Delay_s(1);
	  SendAT("at+tcpclos",0x0d); IsStrInStream(1,"OK",WITHIN_1S*30);		//51);
	  
/*	  if(IsStrInStream("OK",WITHIN_1S*3)==TRUE){
	    Delay_s(1);
	    SendAT("at+tcpclos",0x0d); IsStrInStream("OK",WITHIN_1S*51);
	  }*/
//	}
    Delay_s(1);                 	//delay for transmit last byte>500ms
    Serial_End_Tx();
	if(fReset==TRUE){Restart(RST_FOR_MAIN_94);}
	return r;
}

BYTE GetGprsResp(void)
{
BYTE r;

//	FillBufWith(&rx[0],0x20,0x88);
//  	Reset_Rx(60, FILTER_GPRS_RESP);	//maxDelay=60*0.5s=30s
	InitRx(30000,FILTER_GPRS_RESP);

	while(1){
	  Delay_ms(1);
	  if((ptr_rx-&rx[0])>=32){		//Enough bytes received(16�ֽ�,hex)
		r=CheckGprsResp(&rx[0]);
		break;
	  }
	  if(IsTimeUp(iTimeCmp)==TRUE){r=FAILED; break;}
	}
	RX_IE_OFF();	
	return r;	  
}

//
//
BYTE CheckGprsResp(BYTE *p)
{
char i;
BYTE *ptmp,r;

	ptmp=p;				//����

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
	
	r=CheckCRC(&rx[0],lenHEAD-2);
	return r;
}

//=================================
// ����Modem version�Ĳ�ͬ
// ����:
//		at#tcpserv="xxx.xxx.xxx.xxx"
//		��at#tcpserv=1,"xxx.xxx.xxx.xxx"
//=================================
void Send_tcpserv(BYTE com_index)
{

	Delay_s(1);

//����{at#tcpserv=}
/*	if(modem_version==1){
	  SendAT("at#tcpserv=1,\"",0x0);
	  TransmitIpStr(com_index);
	  Serial_Transmit(DOUBLE_QUOTE);
	}
	else{
	  SendAT("at+tcpopen=\"",0x0);
	  TransmitIpStr(com_index);
  	  SendAT("\",5002",0);
	}
*/
  	SendAT("at+tcpopen=\"",0x0);
  	TransmitIpStr(com_index);
  	SendAT("\",5002",0);

	Serial_Transmit(0x0d);
}

//=======================
// ����Ip��ַ
//=======================
void TransmitIpStr(BYTE com_index)
{
BYTE i,ip[4],sIp[5];
UINT uA;

//��Ip
	uA=MAP_REG0+(UINT)MAIN_COM1;
	uA+=com_index*8;				//ͨ�Ų�����ʼ��ַ
	mem_ReadData(M_CFG,uA+2,&ip[0],4);	    

//����Ip��ַ	
	for(i=0;i<4;i++){				
	  //itoa(((int)ip[i]),(char *)&sIp[0]);
	  sprintf((char *)&sIp[0],"%d",ip[i]);
	  TransmitStr(&sIp[0]);
	  if(i!=3){Serial_Transmit('.');}
	}
}


void TransmitComStr(BYTE com_index)
{
BYTE com_para,sComPara[5],com_type;
UINT uA;
BYTE sTel[16];

//��Ip
	uA=MAP_REG0+(UINT)MAIN_COM1;
	uA+=com_index*8;				//ͨ�Ų�����ʼ��ַ
	mem_ReadData(M_CFG,uA,&com_para,1);	    

	if(com_para>>7){sComPara[0]='O'; sComPara[1]='n'; sComPara[2]=0x0;}
	else{sComPara[0]='O'; sComPara[1]='f'; sComPara[2]='f'; sComPara[3]=0x0;}

	TransmitStr(&sComPara[0]);		//����״̬
	Serial_Transmit('/');
	
	com_type=(com_para&0x7f)>>3;		//ͨ������
	switch(com_type){		
	  case CONNECT_GPRS:
		TransmitIpStr(com_index);
		break;
  	  case CONNECT_GSM_SM:
	  case CONNECT_BEIDOU:
		LoadTelNum(com_index,&sTel[0]);
		TransmitStr(&sTel[0]);
		break;
	}
	Serial_Transmit(0x0d);

}








/*
void Send_tcptxdelay(void)
{
	Delay_s(1);

//����
    if(modem_version==1){
	  SendAT("at#tcptxdelay=1,500",0x0d);
	}
}
*/

/*
void Send_tcpport(BYTE modem_version)
{
	Delay_s(1);

//����
	if(modem_version==1){
	  SendAT("at#tcpport=1,5002",0x0d);	//{at#tcpport=1,5002}
	}
}
*/

//====================================
// ����Tcp����,��ִ��IdeaЭ��(����id)
// ����:
//		GPRS_CONNECTION_OK
//		GPRS_ERR_TCPPORT
//		GPRS_ERR_ATTACH
//		GPRS_ERR_OTCP
//====================================
BYTE OpenIdeaOverTcp(BYTE com_index)
{
BYTE errCode;
char pNodeAddress[10];

//	modem_version=GetByte(M_CFG,MAP_REG0+((UINT)(MAIN_MODEM_VERSION)));
	
//Tcp
/*	if(modem_version==1){
	  SendAT("at#tcpport=1,\"5002\"",0x0d);	//{at#tcpport=1,"5002"}
	  if(IsStrInStream(1,"OK",WITHIN_1S)==FALSE){
	    errCode=GPRS_ERR_TCPPORT; 
	    goto GPRS_ERR;
	  }

	  Send_tcpserv(com_index);	//	SendAT("at#tcpserv=\"218.108.16.149\"",0x0d);

	  if(IsStrInStream(1,"OK",WITHIN_1S*4)==FALSE){
	    errCode=GPRS_ERR_ATTACH; 
	    goto GPRS_ERR;
	  }
	  Delay_s(1);
	  SendAT("at#otcp=1",0x0d);
	  if(IsStrInStream(1,"Waiting",WITHIN_1S*20)==FALSE){
	    errCode=GPRS_ERR_OTCP; 
	    goto GPRS_ERR;
	  }
	}


//asdsoft
	else{
	  Send_tcpserv(com_index);	
	  if(IsStrInStream(1,"CONNECT",WITHIN_1S*51)==FALSE){
	    errCode=GPRS_ERR_ATTACH; 
	    goto GPRS_ERR;
	  }
	}
*/

	Send_tcpserv(com_index);	
	if(IsStrInStream(1,"CONNECT",WITHIN_1S*51)==FALSE){
	  errCode=GPRS_ERR_ATTACH; 
	  goto GPRS_ERR;
	}

	Delay_s(1);

//���Ͳ�վid��
  	GetNodeAddress((char *)&pNodeAddress[0]);
	TransmitStr((BYTE *)&pNodeAddress[0]);
//
	Delay_s(2); 
	return GPRS_CONNECTION_OK;

GPRS_ERR:
	return errCode;
}
