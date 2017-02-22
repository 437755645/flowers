#include 	"header.h"
#include    "h_macro.h"

//手机指令类型
#define 	QUERY_RAIN_AND_LEVEL	1
#define		QUERY_SYSTEM_STATUS		9
#define		QUERY_TERMINAL_RESET 	0x0f
#define		QUERY_COMM_PARA 		0x0e
#define 	QUERY_CFG_UPLOAD		0x0d
#define 	QUERY_ERROR				0x0c
#define 	QUERY_COMM_TEST			0x0b
#define 	QUERY_COM_TRANS			0x0a	//建立透传模式

#define		SM_INST_RECEIVED		1
#define		SM_MOBILE_QUERYING		2
#define		SM_FAILED				3

#define		LEN_SMSC_00				2
#define		LEN_OF_PDU_DATE_CODING	7		//yy mm dd hh mm ss zz
#define		LEN_OF_MQUERY_ECHO_SM	136

#define		CX_DAY_RAIN_INDEX		4
#define		CX_CUR_LEVEL_INDEX		5
#define		CX_DAY_RAIN_OFFSET		6		//从第6个字节开始
#define		CX_CUR_LEVEL_OFFSET		8		//从第8个字节开始


#define		HZ_DATE					1
#define		HZ_TIME					2

extern BYTE 	IPL_CPU;
extern BYTE 	Step[];			//定义见：STEPS_

extern const char sVer[];


const char ROM_LF[]={0x0A,0x00};

//"8时起累积雨量(mm): "="003865F68D777D2F79EF96E891CF0028006D006D0029003A0020" 长度13*2
const char MQuery_txtRain[]=  {0x00,0x38,0x65,0xF6,0x8D,0x77,0x7D,0x2F,0x79,0xEF,0x96,0xE8,0x91,0xCF,0x00,0x28,0x00,0x6D,0x00,0x6D,0x00,0x29,0x00,0x3A,0x00,0x20};	

//"当前水位(m): "="5F53524D6C344F4D0028006D0029003A0020" 长度9*2
const char MQuery_txtLevel[]={0x5F,0x53,0x52,0x4D,0x6C,0x34,0x4F,0x4D,0x00,0x28,0x00,0x6D,0x00,0x29,0x00,0x3A,0x00,0x20};

//"Id:"长度8
const char MQuery_txtId[]={0x00,'I',0x00,'d',0x00,0x3A,0x00,0x20};

//"电压:"="5F53524D6C344F4D003A000A" 长度8
const char MQuery_txtVoltage[]={0x75,0x35,0x53,0x8B,0x00,0x3A,0x00,0x20};

//"信号强度:"="5F53524D6C344F4D003A000A" 长度12
const char MQuery_txtCSQ[]={0x4F,0xE1,0x53,0xF7,0x5F,0x3A,0x5E,0xA6,0x00,0x3A,0x00,0x20};


extern volatile	UINT 	ms_click;
extern BYTE 	c0Table[],cxTable[];

extern BYTE 	ActiveCom;
extern BYTE 	ComStatus;
extern BYTE 	cfg[];

extern BYTE 	ModemReady[];

//================================
//Bit usage variables
//================================
extern MSG0			Msg0;
extern MSG1			Msg1;
extern ERRORS		Error0;
extern TIMERS		Tmr;
extern TIMEUP 		TmrUp;



UINT CSQ;			

//===========================
//Received SM and delete SM
//===========================
// An example of SM:
//  +CMGL: 1,"REC UNREAD","07602",,"05/06/27,19:18:48+00" !Qrfuuru?0XE00000000tr3
//
//Return: 
//  ReturnCode denote if SM-instruction is ever received
//
// 	1、处理完全部短信再复位，
//	2、多个设置指令有效，后者可能覆盖前者
//============================
void SM_process(void)
{
BYTE count,r,t,fReset;
BYTE *oldptr;
BYTE sm_index[4],tel_num[16];
BYTE num515;	//515错误计数
BYTE *rx;
BYTE *p;
UINT EndMs;
BYTE com_save;

	if(ComStatus==STATUS_GPRS_TRANS){return;}	//透传模式下，不发短信

	com_save=ActiveCom;
	ActiveCom=COM1;

	rx=RX_BUF0();

//检查Modem状态
	if(IsModemReady()==FALSE){
	  AT_creg(ActiveCom);		//马上刷新，保证时钟停振时，也能响应	
	  if(IsModemReady()==FALSE){
	    goto SM_END;
	  }
	}


	if(!Connect(CONNECT_SM)){goto SM_END;}
	MsgBox("正在处理短信..        ");
//	Delay_s(1);

	fReset=FALSE;			//如果需要复位，复位在短信全部处理完毕后进行。
	count=0;
	num515=0;
    while(count++<20){		//maximun number of sm which will be processed.
      SendAT("at+cmgf=1",0x0d);
	  IsStrInStream(0,"OK",WITHIN_1S);		  //Delay_ms(100);

	  EndMs=InitRx(2500,FILTER_NULL);
	  oldptr=rx;

      SendAT("at+cmgl=\"ALL\"",0x0d);		//read sm-list
	  Delay_s(1);      						//wait 1's until no byte in within 10ms      
	  while(1){
		ClrWdt();
		pureDelay_ms(250);
		p=getRxPtr();
		if(p==oldptr){break;}				//no byte received in 0.75s
		if(p>=RX_BUF_END()){break;}		
		oldptr=p;
	  }
	  RX_IE_OFF();		

      //delete 1st sm in sm-list
	  p=getRxPtr();	  
	  if(p-rx<0x20){						//长度太短,忽略,可能是"OK"等
		if(InStr(rx,"ERROR: 515",0x20)){		//Wait
		  lcd_puts(0,L1,"错误515.........        ");
		  if(num515++>3){
			SendAT("atz",0x0d); 
			ResetPeri(1); 
			goto SM_END;
		  }
		  Delay_s(1); continue;
		}
	    else if(InStr(&rx[0],"ERROR",0x20)){		//Wait
		  break;
	    }				
		else{
		  Delay_s(1); 
		  break;
		}				
	  }
//
//
	  if(Get_sm_index(&sm_index[0])==FAILED){
		continue;
	  }			//杂乱数据，忽略
	  Get_tel_number(&tel_num[0]);
//
	  t=CheckSM(&sm_index[0]);
	  switch(t>>4){
        case SM_FAILED:					//非指令和垃圾短信，向中心站返回SM
		  lcd_clear();
		  /*if((GetComByte(4)>>7)){		//监视电话Off时，垃圾短信不处理
		    lcd_cursor(0,L0); lcd_puts("报告垃圾短信..          ");
		    lcd_cursor(6,L1); lcd_puts_ram(&sm_index[0]);
		    SendBackSM(&sm_index[0]);
		  }*/
		  DeleteSM(&sm_index[0]);
		  continue;						//until no sm left

	    case SM_INST_RECEIVED:			//bin短信指令，执行指令
		  MsgBox("处理短信指令..        ");
		  DeleteSM(&sm_index[0]);
          r=Inst_Analysis(CONNECT_SM);
		  if(r==STEP_RESET){fReset=TRUE;}	//1、处理完全部短信再复位，2、多个设置指令有效，后者可能覆盖前者
		  continue;						//until no sm left

	    case SM_MOBILE_QUERYING:		//text短信指令(移动查询)
		  MsgBox("回复手机查询..        ");
		  DeleteSM(&sm_index[0]);
		  r=MobileQueryResponse(t&0x0f,&tel_num[0]);
		  if(r==OK_QUIT){goto SM_END;}		//进入"透传模式"
	      break;
	  }
    }

	if(fReset==TRUE){
	  Restart(RST_FOR_MAIN_90);	//正常重起
	}

//太慢！
//	SendAT("AT+CMGD=1,4",0x0d);		//delate all sm, enabled this line will cause "AT+CMGD=1,4" run repeatly.
	Delay_ms(50);

SM_END:
	ActiveCom=com_save;
	return;		//BackCode;
}

//================================
// 本函数在收到垃圾短信时
// 自动向中心站返回收到的短信.
//
//================================
void SendBackSM(BYTE *sm_index)
{
BYTE iTel,i;
BYTE tel[16];
BYTE *p;
BYTE *pTailStart,*pTailEnd,*pPduHeaderEnd;
BYTE lenSMSC,lenMsg;
BYTE pdu_len[5];
BYTE pdu_head[0x20];
BYTE *rx;

	rx=RX_BUF0();
	
//Read in pdu mode.
	Serial_Transmit(0x0d);
	Delay_ms(100);
	SendAT("at+cmgf=0",0x0d); Delay_ms(100);
	SendAT("at+cmgr=",0);
	TransmitStr(sm_index);
	Serial_Transmit(0x0d);

//Read response
//   	FillBufWith(&rx[0],255,0x0d);	//Clr buffer

//Skip'+'(+CMGR)
	if(IsStrInStream(0,"+",WITHIN_1S*2)==FALSE){
	  return;}
//Skip LF(last byte before PDU)
  	if(IsStrInStream(0,&ROM_LF[0],WITHIN_1S*2)==FALSE){
	  return;}

	p=Merge2Bytes(WITHIN_1S*2);
	//原为：if(*p!=CR){
	if(p==0){return;}
	pTailEnd=p;
	//Now, sm in pdu is ready in rx[].

	lenSMSC=*rx;
	p=rx+lenSMSC+2;			//
	if(testbit0(*p)){(*p)++;}	//补偿电话长度
	pTailStart=p+((*p)>>1)+2;	//point to 1st byte after tel-num

//
	iTel=4;					//监视电话
//	LoadTelNum(iTel,&tel[0]);
	pPduHeaderEnd=Make_SM_Head(&tel[0],&pdu_head[0],pTailStart);
	pTailStart++;
	pTailStart++;

//lenMsg ?
	lenMsg=(pPduHeaderEnd-&pdu_head[0]-2)>>1;
	pTailStart+=LEN_OF_PDU_DATE_CODING;			//skip PDU_DATE_CODING
	lenMsg+=pTailEnd-pTailStart;

//	itoa((int)(lenMsg),((char *)(&pdu_len[0])));
	sprintf(((char *)(&pdu_len[0])),"%d",lenMsg);
	p=InStrChar(&pdu_len[0],0x0,4);	//指向结尾
	for(i=4;i>=1;i--){
	  if(p>=&pdu_len[0]){
		pdu_len[i-1]=*p--;
	  }
	  else{pdu_len[i-1]='0';}
	}
//
	SendAT("at+cmgf=0",0x0d); Delay_ms(100);
	SendAT("at+cmgs=",0x0);

	TransmitStr(&pdu_len[0]);

	Serial_Transmit(0x0d); Delay_ms(100);	
//
	*pPduHeaderEnd=0x0;			//字符串结尾
	TransmitStr(&pdu_head[0]);
//
	if(pTailStart>=pTailEnd){return;}		//陷阱
	p=pTailStart;
	while(p!=pTailEnd){
	  Serial_Transmit(HexHiByte(*p));
	  Serial_Transmit(HexLoByte(*p));
	  p++;
	}

	EndSM();
}

BYTE *Make_SM_Head(BYTE *tel,BYTE *pdu_head,BYTE *pTailStart)
{
BYTE lenTel,xlenTel;
BYTE i,cTel[16];

	strcpy((char *)&cTel[0],(const char *)tel);
	lenTel=strlen((const char *)&cTel[0]);
	xlenTel=lenTel;
	if(testbit0(lenTel)){
	  cTel[xlenTel]='F';
	  xlenTel++;		//xlenTel=++xlenTel; 等效吗？
	}

	//p0=&pdu_head[0];
	*pdu_head++='0';			//忽略SMSC
	*pdu_head++='0';
	*pdu_head++='1';			//FO
	*pdu_head++='1';
	*pdu_head++='0';			//TP-MR
	*pdu_head++='0';
//
	if(lenTel<=8){				//特服号
	  *pdu_head++='0';			//length
	  *pdu_head++=HexLoByte(lenTel);
	  *pdu_head++='A';
	  *pdu_head++='1';
	}
	else{
	  *pdu_head++='0';		//length
	  *pdu_head++=HexLoByte(lenTel+2);	//'+2'?: "86"
	  *pdu_head++='9';
	  *pdu_head++='1';
	  *pdu_head++='6';
	  *pdu_head++='8';
	}

//Add with tel-number
	for(i=0;i<xlenTel;){
	  *pdu_head++=cTel[i+1];
	  *pdu_head++=cTel[i];
	  i++;i++;
	}
	*pdu_head++=HexHiByte(*pTailStart);	//2 bytes
	*pdu_head++=HexLoByte(*pTailStart++);
	*pdu_head++=HexHiByte(*pTailStart);
	*pdu_head++=HexLoByte(*pTailStart++);
	*pdu_head++='A';
	*pdu_head++='7';
	*pdu_head='\0';		//以'\0'结束字符串
	return pdu_head;
}

//==============================================
// 接受到的数据在rx[]
// Receiving chars and merge 2 bytes into 1-byte
//
// limitation:
//	1. Value of receiving bytes should within 
//	  [0-15]
//  2. p=0 if data flow is not as expected.
//     too long or receiving-time too long.
//==============================================
BYTE *Merge2Bytes(UINT LimitMS)
{
BYTE len,c;
UINT iCmp;
BYTE *p;
BYTE *pbuf,buf[10];
BYTE *rx;

	rx=RX_BUF0();

	ClrUErr();
	while(URXDA()){
	  c=URXREG();		//clear FIFO
	}
	
	TX_IE_OFF();	//TX1_IE=0;
	RX_IE_OFF();	//*	RX_IE_OFF();	//RX1_IE=0; 		//note the order!!!

	iCmp=ms_click+LimitMS/2;
	len=0;
	p=&rx[0];
	pbuf=&buf[0];
	while(1){
	  ClrWdt();

	  if(URXDA()){						
	    *pbuf=URXREG();	//U1RXREG;
		len++;
		if(*pbuf==CR){
		  return p;		//Normal exit
		}		
		if(p>=RX_BUF_END()){p=0; break;}
	    if(!testbit0(len)){
		  *p=(ValueHex(buf[0])<<4)+(ValueHex(buf[1])&0x0f);
		  p++; 
		  pbuf=&buf[0];
		  continue;
		}
//		if(p>=mBUF2_END){p=0; break;}
	    pbuf++;
	  }

	  if(IsTimeUp(iCmp)==TRUE){p=0; break;}
	}
	return p;
}




/*
char IsStrInStream(BYTE cmp_start,const char *sCmp,UINT LimitMS)		
{
UINT iCmp;
const char *pSave;
BYTE r,c,i_stream;
BYTE ipl_save;

	di(ipl_save,IPL_T2_INT-1);		//允许T2_INT中断	

	ClrUErr();
	while(URXDA()){
	  c=URXREG();		//clear FIFO
	}
	
	TX_IE_OFF();	//TX1_IE=0;
	RX_IE_OFF();	//	RX_IE_OFF();	//RX1_IE=0; 		//note the order!!!

	Timer2_on();

	pSave=sCmp;
	iCmp=ms_click+LimitMS/2;

	FillBufWith(&Stream[0],64,0x0);
	i_stream=0;
	while(1){
      ClrWdt();

	  if(URXDA()){		
    	c=URXREG();		
		if(i_stream<64){
		  Stream[i_stream++]=c;
		}

    	iCmp=ms_click+(WITHIN_1S*3)/2;	//收到1个字符后，等待时间缩短为最多1s.（北斗卫星的成功回复分2段，前段较快，后段较慢，优化后调整为3s)

		if(i_stream>=cmp_start){	//从第cmp_start个byte开始比较
	      if(c==*sCmp){
		    sCmp++;
		    if(*sCmp!=0){continue;}	
		    else{
			  r=TRUE;
			  break;
		    }
		  }
		  else{sCmp=pSave;}		//从头开始比较
		}
	  }
	  if(IsTimeUp(iCmp)==TRUE){
		r=FALSE;
		break;
	  }
	}

	ei(ipl_save);	//restore interrupts
	return r;
}
*/

//============================================
// 获取电话号码,以'/0'结尾
//============================================
BYTE Get_tel_number(BYTE *tel_num)
{
BYTE *p;
char i;
BYTE *rx;

	rx=RX_BUF0();
//
	p=InStr(rx,"+86",50);
	if(!p){goto GET_TEL_END;}
//
    i=0;
	p+=3;
    while((*p!='"')&&(i++<15)){*tel_num++=*p++;}
    if(i==15){goto GET_TEL_END;}      	//no "" pair found
	*tel_num=0x0;
	return OK;

GET_TEL_END:
    *tel_num=0x0;                       //add 0x0 and tel-number end
	return FAILED;
}

//=======================
// SM moved into &rx[0]
// return: len of SM
//=======================
BYTE GetSM(void)
{    
BYTE i;
BYTE *p,*sm;
BYTE *rx;

	rx=RX_BUF0();

//Looking for 0x0d after rx[50]
	p=InStrChar(rx+50,CR,30);
	if(!p){return FAILED;}
	p++; p++;							//skip 0x0a

//SM received saved in rx[].
	sm=rx;						
	for(i=0;i<160;i++){
	  *sm++=*p++;
	  if(*p==0x0d){break;}
	}
	*sm=0;

	return i+1;		//len of SM
}


//============================================
// Resource:	rx[]
//
// Return:
// 1. tel-number in header[],if header[0]!=0x0.
// 2. decoded SM received saved in rx[]		//,ending with 0x0
//============================================
char Decode_SM_To_src(BYTE sm_len)
{
BYTE i,j;
BYTE sms[160];
BYTE *rx;

	rx=RX_BUF0();

	if(sm_len!=160){return FAILED;}		//长度不足

	DataCpy(&sms[0],rx,sm_len);	//save in sms[]

//合并，恢复
	j=0;
	for(i=0;i<80;i++){
	  sms[i]=(ValueHex(sms[j])<<4)+ValueHex(sms[j+1]);
	  j+=2;
	}


	if(sms[0]!=INST_HEADER){return FAILED;}  
	

  
/*      
//move 1-byte ahead, discard header add-on,-0x21,-6
    p=&rx[0];
	sm=p+1;
    for(i=0;i<108;i++){
      if(*sm<'['){*p=(*sm)-0x30;}   //-'0'
      else{*p=(*sm)-0x36;}            //-'0'-0x6
      sm++;
      p++;
    }
    
//4-byte -> 3-byte     
    i=0;
    while(1){
      rx[i]=((rx[i+1]& 0x3)<<6)+(rx[i]);  i++; if(i>=106){break;}
      rx[i]=((rx[i+1]& 0xf)<<4)+(rx[i]>>2); i++; if(i>=106){break;}
      rx[i]=((rx[i+1]& 0x3f)<<2)+(rx[i]>>4); i++; if(i>=106){break;}
      i++;
    }
    
//skip 4th byte
    sm=&rx[0];
    p=sm;
    for(i=0;i<106;i++){
      *p=*sm;
      if((((int)(sm)+1)& 0x3)==0){sm++;}
      else{p++;sm++;}
    }
*/
    DataCpy(rx,&sms[0],80);
    if(CheckCRC(rx,lenSHORT-2)){
	  if(*(rx+FRM_LEN)==lenSHORT-1){return OK;}
      else{             //long instruction
        if(CheckCRC(rx+DATA0,*(rx+FRM_LEN)-lenHEAD-1)){return OK;}
      }
    }

//	DataCpy(&rx[0],&p3[0],160);		//save in p3[],which will be used to
									// restore rx[] when sm is not an INST.
    return FAILED;    
}    


BYTE Get_sm_index(BYTE *sm_index)
{
char i,j;
BYTE *p;
BYTE *rx;

	rx=RX_BUF0();
//
	p=rx;	//looking for digits after "(CM)GL"
    for(i=0;i<0x10;i++){
      if(*p=='G'){
		p++;
        if(*p=='L'){
		  p++;
          j=6;
          while((j--)>0){
            if(isdigit(*p)){goto SEND_DIGITS;}
            p++;
          }
        }
      }
	  p++;
    }
    return FAILED;              //no "CMGL" found as expected, come to end
    
SEND_DIGITS:
	for(i=0;i<4;i++){			//短信序号保存在sm_index[],0x0结尾
	  if(isdigit(*p)){*sm_index=*p;}
	  else{*sm_index=0; break;}
	  sm_index++; p++;
	  if(i==3){return FAILED;}	//数字超过999，不正常短信
	}
	return OK;
}

//===========================
//Check SM format:
// return: 	前4位：类型
//			后4位：参数
//			SM_INST_RECEIVED<<4
//			SM_MOBILE_QUERYING<<4+m_code
//			SM_FAILED<<4
//===========================
BYTE CheckSM(BYTE *sm_index)
{  
BYTE m_code,sm_len;

	sm_len=GetSM();			//sm in &rx[0],end with 0x0;
	
	if(Decode_SM_To_src(sm_len)==OK){
	  return SM_INST_RECEIVED<<4;
	}
	else{
	  m_code=IsMobileQuerying();
	  if(m_code==0){
		return SM_FAILED<<4;
	  }
	  else{
		return (SM_MOBILE_QUERYING<<4)+m_code;
	  }
	}
}

BYTE IsMobileQuerying(void)
{
BYTE *rx;
	rx=RX_BUF0();
//
	if((InStr(rx,"1",2)!=0)||(InStr(&rx[0],"0031",2)!=0)){return QUERY_RAIN_AND_LEVEL;}
	else if((InStr(rx,"9",2)!=0)||(InStr(&rx[0],"0039",2)!=0)){return QUERY_SYSTEM_STATUS;}
	else if(InStr(rx,"r",2)!=0){return QUERY_TERMINAL_RESET;}	//请求复位
	else if(InStr(rx,"R",2)!=0){return QUERY_TERMINAL_RESET;}	//请求复位
	else if(InStr(rx,"g",2)!=0){return QUERY_COMM_PARA;}		//请求通信参数（GPRS等） 
	else if(InStr(rx,"c",2)!=0){return QUERY_CFG_UPLOAD;}		//请求上传cfgs 
	else if(InStr(rx,"e",2)!=0){return QUERY_ERROR;}			//请求回传错误报告 
	else if(InStr(rx,"s",2)!=0){return QUERY_COMM_TEST;}		//请求平安报 
	else if(InStr(rx,"t",2)!=0){return QUERY_COM_TRANS;}		//请求建立透传模式 
	return 0;
}



//=============================
// 删除指定序号的短信
// sm_index指向短信序号
//=============================
void DeleteSM(BYTE *sm_index)
{
	SendAT("AT+CMGD=",0);		//
	TransmitStr(sm_index);
	Serial_Transmit(0x0d);
//	Delay_s(1);
	IsStrInStream(0,"OK",WITHIN_1S);		  
}


//===========================  
//Prepare "AT+CMGS="xxxxx"",[CR]  
//tel-num: 'xxxxx'
//index: index of tele-number
//===========================
char Send_AT_CMGS(BYTE *tel)
{
//char i;

	SendAT("at+csmp=1,167,0,0",0x0d); Delay_ms(100);
	SendAT("at+cmgf=1",0x0d); Delay_ms(100);
	SendAT("at+cmgs=",0); Delay_ms(10);

	Serial_Transmit(DOUBLE_QUOTE);

	TransmitStr(tel);
/*	for(i=0;i<12;i++){
	  if(isdigit(*tel)){
		Serial_Transmit(*tel);
		tel++;
	  }
	  else{break;}							//非数字退出
	}
*/
	Serial_Transmit(DOUBLE_QUOTE);

	Serial_Transmit(0x0d);
    Delay_ms(50);                         	//delay for transmit last byte
	return 1;
}      

//================================
// Data are ready in p3[] 
//================================
/*
BYTE Send_Data(BYTE maxLen)
{
BYTE i,r;		

    TX_IE_OFF();	//TX1_IE=0;
    i=0;

//Limit
	if(maxLen>160){
	  maxLen=160;		////短信长度限制，如果超长，截短，可能会损失水位长度
	}

    while(i<maxLen){
	  if((p3[i]<'0')||(p3[i]>'u')){break;}	//遇到非法字符...
	  if((p3[i]>'Z')&&(p3[i]<'a')){break;}	//遇到非法字符...
	  Serial_Transmit(p3[i]);
	  //Serial_Transmit('8');
      i++;
    }
	r=EndSM();
    Serial_End_Tx();				
	return r;
}
*/

//=======================================
// 
//	Reserve 6-bit every byte, byte%64, 
//	3-byte -> 4-byte
//
// Return:
//	len of result.
//=======================================
BYTE Reserve_6_bits(BYTE *p,BYTE len)
{
BYTE i,maxLen,a,b,c;	//,d;
BYTE *src,*des;

//move datas to the end area, make room for inserting	
	i=len/3;
	if(len==i*3){maxLen=i*4;}
	else{
	  maxLen=i*4+(len-i*3)+1;
	}
//move to bottom area.
	src=p+len-1;
	des=p+maxLen-1;
	i=len;
	while(i--){*des--=*src--;}
	des++; src++;

//insert one space every 3 bytes.
	i=0;
	while(i<len){
	  *src++=*des++;
	  i++;
	  if(i%3==0){			//4的倍数
		*src=0x88;			//for debug. discard for normal
		src++;
	  }
	}
	*src=0;			//add 0x0 behind bottom, it's useful when split by 6-bit
//
	i=0;
	while(1){
		a=*p; 
		*p++=a&0x3f; 
		if(++i>=maxLen){break;}
	
		b=*p;
		*p++=((b<<2)+(a>>6))&0x3f;
		if(++i>=maxLen){break;}

		c=*p;
		*p++=((c<<4)+(b>>4))&0x3f;
		if(++i>=maxLen){break;}

	  	*p++=c>>2; 
		if(++i>=maxLen){break;}
	}
	return maxLen;
}

//=======================================================
// Make array visible,
//   1.Add a 0x30 to make byte visible in text mode
//
//    [valid chars]:
//		0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ
//
//		[\]^_'    <-----not visible in some system
//
//		abcdefghijklmnopqrstu
//=======================================================
void BinaryVisible(BYTE *des,BYTE *src,BYTE len)
{
BYTE i;
	for(i=0;i<len;i++){
	  *des=*src+'0';		//'0'=0x30
	  //skip " [\]^_' " which is not visible in some system
	  if(*des>='['){*des+=0x06;}	//'['=0x5b
	  src++; des++;		
	}
}


void SendAT(const char *cmd,BYTE end)
{
	TransmitStr((BYTE *)cmd);
	if(end!=0x0){Serial_Transmit(end);}
	Delay_ms(10);
}

//===========================
// 短信中继
// [目标]/[内容]
// e.g: "1861/1"
//===========================
void RepeatSM(BYTE *sm)
{
BYTE *tel;

	tel=sm;
	while(*sm!='/'){sm++;}
	*sm++=0;		//tel形成,sm指向短信内容
	
	Send_AT_CMGS(tel);

/*	SendAT("at+csmp=1,167,0,0",0x0d); Delay_ms(100);
	SendAT("at+cmgf=1",0x0d); Delay_ms(100);
	SendAT("at+cmgs=",0x0); Delay_ms(100);

	Serial_Transmit(DOUBLE_QUOTE);
	while(*sm!='/'){Serial_Transmit(*sm++);}
	Serial_Transmit(DOUBLE_QUOTE);
	Serial_Transmit(0x0d);
	Delay_ms(100);

	sm++;			//skip '/'
*/
	TransmitStr(sm);
	EndSM();
}

//===========================
// 返回:
//	Version
//	Modem
//	Ip
//===========================
BYTE TransmitPARA(BYTE *tel)
{
char vs[20],i;
BYTE mv;	//modem version
//BYTE xm_fail;
BYTE Error[10]; //,cnt;


/*	SendAT("at+csmp=1,167,0,0",0x0d); Delay_ms(100);
	SendAT("at+cmgf=1",0x0d); Delay_ms(100);
	SendAT("at+cmgs=",0x0); Delay_ms(100);

	Serial_Transmit(DOUBLE_QUOTE);
	while(*tel!=0){Serial_Transmit(*tel++);}
	Serial_Transmit(DOUBLE_QUOTE);
	Serial_Transmit(0x0d);
	Delay_ms(100);
*/
	Send_AT_CMGS(tel);

//Version
	ClrWdt();
	strcpy(&vs[0],&sVer[0]);
	TransmitStr((BYTE *)&vs[0]);
	Serial_Transmit(0x0d);

//modem version
	ClrWdt();
	Serial_Transmit('M');
	Serial_Transmit(':');
//	mv=cfg[CMC_MODEM_VERSION];	//刷新modem_version
    mv=1;
	Serial_Transmit(HexLoByte(mv));
	Serial_Transmit(0x0d);

//Com para
	ClrWdt();
	for(i=0;i<5;i++){
//	  TransmitComStr(i);
	}

//Error：[E:xxxxxxxx]
	ClrWdt();
	Serial_Transmit('E');
	Serial_Transmit(':');
//	xm_fail=GetByte(M_CFG,REG0+((UINT)(MAIN_XMEM_FAIL)));
//	BinOfByte(xm_fail,&Error[0]);			//发送XME错误代号
	TransmitStr(&Error[0]);
	Serial_Transmit('/');

	//停振次数
//	cnt=GetByte(M_CFG,MAP_XSTP_CNT);
	//btoa(cnt,(char *)&Error[0]);
	//BinOfByte(iRecFailed,&Error[0]);		//发送记录过程错误代号
	TransmitStr(&Error[0]);
	Serial_Transmit(0x0d);
	Delay_ms(100);

	return EndSM();
}


//===============================
// 移动查询响应
// Response to Mobile Querying
//===============================
BYTE MobileQueryResponse(BYTE inst_type,BYTE *tel)
{
BYTE r,len;
BYTE i,date_time[21];
UINT cx;
BYTE buf[256];
BYTE iC0,iCx;

	r=FAILED;

	if(inst_type==QUERY_TERMINAL_RESET){			//手机复位请求
//	  ResetPeri();
	  Restart(RST_FOR_MAIN_93);
	}

	if(inst_type==QUERY_COMM_PARA){					//手机查询GPRS参数设置
	  r=TransmitPARA(tel);							//西文模式发送
	  return r;
	}


	if(inst_type==QUERY_CFG_UPLOAD){				//手机请求Cfgs数据
//	  ReportCfgs();			
//	  MapUpload();
	  return OK;
	}

	if(inst_type==QUERY_COM_TRANS){				//手机请求建立透传模式
//	  ComTransparant();			
	  return OK_QUIT;		//马上退出“短信查询”
	}

	if(inst_type==QUERY_ERROR){						//手机查询错误信息
//	  len=TransmitError(len);
	  return OK;
	}

	if(inst_type==QUERY_SYSTEM_STATUS){				//手机查询系统状态
//	  ShowCSQ((BYTE *)&sm_buf[0]);
	}

	if(inst_type==QUERY_COMM_TEST){					//手机平安报
//	  CommTest(0);			
	  return OK;
	}


	ActiveCom=COM1;

	CSQ=getCSQ(ActiveCom);

	if(!Connect(CONNECT_SM)){return 0;}
//	Delay_s(1);

	i=0;
	while(1){
	  SendAT("at+cmgf=0",0x0d); 
	  if(IsStrInStream(0,"OK",WITHIN_1S*2)==TRUE){break;}
	  else{
		SendAT("at+cfun=0",0x0d); Delay_s(1);
		SendAT("at+cfun=1",0x0d); Delay_s(15);
	  }
	  if(i++<3){return r;}					//Modem操作失败
	}

//
	SendAT("at+cmgs=136",0x0d); Delay_ms(100);

//head
	len=0;
	len=TransmitMQuery_head(len,tel);

  	GetCurDateTime((char *)&date_time[0]);

//date & time
	len=Transmit(HZ_DATE,len,&date_time[0]);
	len=Transmit(HZ_TIME,len,&date_time[11]);

	switch(inst_type){
	case QUERY_RAIN_AND_LEVEL:
//content
	  iC0=0;iCx=3;	//日雨量
	  cx=getCX(iC0,iCx);
//rain
	  len=TransmitTitle(len,&MQuery_txtRain[0],26);
	  len=TransmitValue(&buf[0],CX_DAY_RAIN_INDEX,cx,len,CX_DAY_RAIN_OFFSET);

//level
	  len=TransmitTitle(len,&MQuery_txtLevel[0],18);
	  len=TransmitValue(&buf[0],CX_CUR_LEVEL_INDEX,cx,len,CX_CUR_LEVEL_OFFSET);
	  break;

	case QUERY_SYSTEM_STATUS:
//id
	  len=TransmitTitle(len,&MQuery_txtId[0],8);
	  len=TransmitId(len);

//voltage
	  len=TransmitTitle(len,&MQuery_txtVoltage[0],8);
	  len=TransmitVoltage(len);

//csq
	  len=TransmitTitle(len,&MQuery_txtCSQ[0],12);
	  len=TransmitCSQ(len);

	  break;

	default: return r;
	}


//已发送字节数
	len--;							//长度不包括起始00

	while(len<LEN_OF_MQUERY_ECHO_SM){
	  Serial_Transmit('0'); Serial_Transmit('0'); 
	  len++;
	}
//
	r=EndSM();
	return r;
}


//===========================================
BYTE TransmitTitle(BYTE len,const char *hz_title,BYTE title_len)
{
BYTE c;

	while(title_len--){
	  c=(BYTE)(*hz_title++);
//	  Serial_Transmit(HexHiByte(c));
//	  Serial_Transmit(HexLoByte(c)); len++;
	  Serial_TransmitHL(c); len++;
	}
	return len;	
}




//===========================================
BYTE TransmitValue(BYTE *buf,BYTE cx_index,UINT addr,BYTE len,BYTE cx_offset)
{
BYTE cfg_cx[64];
BYTE CxValue[20];		//,*p;
	
	addr+=((UINT)(cx_index-1))*lenCELL;
	mem_ReadData(M_CFG,addr,&cfg_cx[0],lenCELL);		//读虚元CFG -> cfg_cx[]

//定制(日雨量)
//	if(cx_index==CX_DAY_RAIN_INDEX){pRec_Cx=map_PTR_REC0+(3*0x10);}
//	else if(cx_index==CX_CUR_LEVEL_INDEX){pRecCx=ADDR_PTR_REC0+2*0x10;}
//定制 end

	Seg2Str(buf,&CxValue[0],&cfg_cx[0]);
//
	len=TransmitHZdigits(len,&CxValue[0]);
	return len;
}


BYTE TransmitHZdigits(BYTE len,BYTE *pCxValue)
{
//	p=&CxValue[0];
	while(*pCxValue){
	  if(*pCxValue=='-'){break;}			//S板不响应时," --- "
	  Serial_Transmit('0'); Serial_Transmit('0'); len++;
//	  Serial_Transmit(HexHiByte(*pCxValue)); Serial_Transmit(HexLoByte(*pCxValue)); len++;
	  Serial_TransmitHL(*pCxValue); len++;
	  pCxValue++; 
	}
	Serial_Transmit('0'); Serial_Transmit('0'); len++; 
	Serial_Transmit('0'); Serial_Transmit('A'); len++;	//汉字回车
	return len;	
}

BYTE TransmitId(BYTE len)
{
BYTE pID[16];

//	GetNodeAddress((char *)&pID[0]);
    sprintf((char *)&pID[0],"%ld",getULNG(&cfg[CM_STA_ID]));

	len=TransmitHZdigits(len,&pID[0]);
	return len;
}

/*
BYTE TransmitError(BYTE len)
{
BYTE Error[12];		//[E:xxxxxxxx,0x0]
BYTE xm_fail;

	Error[0]='E'; Error[1]=':';
	xm_fail=GetByte(M_CFG,REG0+((UINT)(MAIN_XMEM_FAIL)));
	BinOfByte(xm_fail,&Error[2]);
	len=TransmitHZdigits(len,&Error[0]);
	return len;
}
*/

BYTE TransmitVoltage(BYTE len)
{
BYTE v[10],i;

//Voltage
	i=VoltageNow();
//	itoa(((int)(i)),((char *)&v[0]));
	sprintf(((char *)&v[0]),"%3.1f",((float)(i)*0.1));
//	for(i=0;i<10;i++){if(v[i]==0){break;}}		//寻找结束符'/0'
//	v[i+1]=0; v[i]=v[i-1]; v[i-1]='.';	//插入'.',相当于"x/10"
//
	len=TransmitHZdigits(len,&v[0]);
	return len;
}

//========================
// +CSQ: aaaaa,bbb
//=========================
BYTE TransmitCSQ(BYTE len)
{
BYTE sCSQ[10];

/*BYTE csq0[10],comma[2],csq1[10];

//CSQ
	sprintf(((char *)&csq0[0]),"%d",LoOfInt(CSQ));

	comma[0]=',';comma[1]=0x0;

	sprintf(((char *)&csq1[0]),"%d",HiOfInt(CSQ));

//连接
	strcat(((char *)&csq0[0]),(const char *)&comma[0]);
	strcat(((char *)&csq0[0]),(const char *)&csq1[0]);
*/
//

	sprintf((char *)&sCSQ[0],"%d,%d",HiOfInt(CSQ),HiOfInt(CSQ));

	len=TransmitHZdigits(len,&sCSQ[0]);
	return len;
}




//===========================================
BYTE TransmitMQuery_head(BYTE len,BYTE *tel)
{
BYTE i;
//BYTE *p
BYTE sm_buf[64],*pPduHead;
BYTE sm_seg[2];

	sm_seg[0]=0x00;		//PID
	sm_seg[1]=0x08;		//编码模式：Unicode编码
	pPduHead=Make_SM_Head(tel,&sm_buf[0],&sm_seg[0]);
	len=pPduHead-(&sm_buf[0]);

	i=LEN_OF_MQUERY_ECHO_SM-(len>>1)+1;		//14 is length of pdu-head,00 not included
	sm_buf[len]=HexHiByte(i);
	sm_buf[len+1]=HexLoByte(i);
	sm_buf[len+2]='\0';

	TransmitStr(&sm_buf[0]);

	len=(len+2)>>1;
	return len;
}


//===========================================
BYTE Transmit(BYTE date_or_time,BYTE len,BYTE *src)
{
BYTE i,count;		//,*p;

	switch(date_or_time){
	case HZ_DATE:
	  *(src+10)=0x0a;		//use CR to replace original ' '
	  count=11;
	  break;

	case HZ_TIME:
	  *(src+8)=0x0a;		//use CR to replace original ' '
	  count=9;
	  break;
	}

  	for(i=0;i<count;i++){
	  Serial_Transmit('0'); 
	  Serial_Transmit('0');
	  len++;
//	  Serial_Transmit(HexHiByte(*src));
//	  Serial_Transmit(HexLoByte(*src));
	  Serial_TransmitHL(*src);
	  len++;
	  src++;
	}
	return len;
}

BYTE EndSM(void)
{
BYTE r;
	Serial_Transmit(ASC_CTRL_Z);	
//	Serial_Transmit(0x0d);
//
	while(!IFS0bits.U1TXIF)continue;
//
	r=IsStrInStream(0,"OK",WITHIN_1S*12);
	return 1;
}


void SM_ReportError(BYTE *err_msg,UINT max_len)
{
BYTE iTel,tel[20];
BYTE com_save;
UINT i,j,k;
const char mobile_tel[]={"13958108670"};
//const char mobile_tel[]={"8610639702"};

	com_save=ActiveCom;
	ActiveCom=1;

	ModemReady[ActiveCom]=TRUE;
    if(!Connect(CONNECT_SM)){return;}

	iTel=4;						//监视电话
//	LoadTelNum(iTel,&tel[0]);

	strcpy((char *)&tel[0],&mobile_tel[0]);

	i=0; k=0;
	while(1){
	  Send_AT_CMGS(&tel[0]);
	  Serial_Transmit('E');
	  Serial_Transmit('R');		
	  Serial_Transmit('R');
	  Serial_Transmit(k+0x30); k++;
	  Serial_Transmit(':');
	  for(j=0;j<64;j++){
		Serial_TransmitHL(*err_msg++); i++;
		if(i>=max_len){goto SM_RE_END;}
		if(i%64==0){
		  EndSM();
		  break;
		}
	  }
	}	

SM_RE_END:
	EndSM();
	ActiveCom=com_save;
}


//===============================	
//           获取CSQ 
//
// 条件：对应通道具GSM/GPRS模块
//===============================
UINT getCSQ(BYTE channel)
{
UINT uCSQ;
BYTE *rx;

	ActiveCom=channel;

	RX_IE_OFF();				//阻止返回信号触发RING_ECHO
	SLEEP1_DIR=OUT; SLEEP1=1;	//RS232 wake up
	Serial_Setup(BAUD_19200);

	SendAT("AT+CSQ",0x0d);

//	Waiting_CSQ();

	uCSQ=0;
	rx=RX_BUF0();
	if(IsStrInStream(0,"OK",WITHIN_1S)==TRUE){
      if((*(rx+4)=='S')&&(*(rx+5)=='Q')&&(*(rx+6)==':')){
		uCSQ=CSQ_to_UINT(rx+8);
	  }
	}
	Delay_ms(100);

   	CLR_RX_IF();	//清除通信期间留下的中断标志
	RX_IE_ON();		//开放RING_ECHO

	return uCSQ;
}

//==========================
// CSQ 存入UINT
//
// "+CSQ: a,b"
// 
// a: 0-31,99  => L
// b: 0-7,99   => H
//
// 特殊情况：99,99 => 0,0
//==========================
UINT CSQ_to_UINT(BYTE *csq)
{
BYTE i,j;
BYTE bCSQ[2];
BYTE *p;
UINT u;

	p=csq;

	for(j=0;j<2;j++){ 				//第j+1数字
	  for(i=0;i<10;i++){
	    if(!isdigit(*p++)){break;}
	  }
	  *p=0;
	  bCSQ[j]=(BYTE)(atoi((const char *)csq));
	  if(bCSQ[j]==99){
		bCSQ[j]=0;			//99表示没信号，替换成0
	  }	
	  if(j==0){csq=++p;}
	}
	u=bCSQ[0]+((UINT)bCSQ[1]<<8);	//	uCSQ=(bCSQ[1]<<5)+(bCSQ[0]&0x1f);
	return u;
}

