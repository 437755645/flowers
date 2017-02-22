//===============================================
//
//    Serial sensor communicaton routines
//
//===============================================

#include 	"header.h"
#include    "h_macro.h"

#define		RX_BUF_SIZE_FOR_SER_SENSOR		64

extern volatile	UINT 	ms_click;

extern BYTE 	IPL_CPU;
extern BYTE 	soft_timer0,ActiveCom;
extern BYTE		cfg[];


BYTE Stream[64];

//=============================================
// 校验NIHAO响应完整性
//
// HAO@src#n, val1,val2…valN,~chksum <CR>
// chksum : "3AB8"
//
// pCR: 指向CR
//=============================================
BYTE CheckNIHAO(BYTE *pCR)
{
//BYTE c[RX_BUF_SIZE3];
//UINT uCRC;
UINT sum=0;
BYTE *p;

	p=RX_BUF0();
	if((pCR-p)<3){return FAILED;}

	while(p<(pCR-2)){
	  sum+=*p++;
	}
	sum=~sum;

	p=pCR-4;		//指向chksum
	if(LoOfInt(sum)!=ValueHex2(p))return 0;
	if(HiOfInt(sum)!=ValueHex2(p+2))return 0;
	return 1;
}

//==============================================
// Receive form sensor
// 按PROTOCOL_NIHAO接收Sensor响应
//
// 1、要求传感器具有Now-Ready特征;
//==============================================
BYTE RxFromSensor_NIHAO(void)
{
UINT EndMs;
BYTE r;
BYTE *p,*pEnd;

	
	EndMs=InitRx(2500,FILTER_NIHAO);	//2.5s, 最大延时
	p=RX_BUF0();

	while(1){
      ClrWdt();
	  pEnd=getRxPtr();

	  while(p<pEnd){	//Loop through all chars received.
	    if(*p==0x0D){
		  r=CheckNIHAO(p);
		  return r;
	    }
	    p++;
	  }

	  if(IsTimeUp(EndMs)==TRUE){
	    return RX_OVER_TIME_OVERFLOW;
	  }
	}
}

//==============================================
// 判断是否"^NNNN"
//
//==============================================
UINT IsException(BYTE *p)
{
UINT uEx;

	uEx=0;
	if(*p=='^'){
	  p++;
	  sscanf((const char *)p,(const char *)"%u",&uEx);
	  return uEx;
	}
	return uEx;
}

//==============================================
// NIHAO => BELT
// fmt:port format. "125",cx=1,2,5
//==============================================
BYTE NIHAO2Belt(BYTE iC0,BYTE *fmt,BYTE *pNH,BYTE *belt)
{
BYTE *p,*pNH0,*pfmt;
BYTE lenNH,type;
BYTE iCx;
UINT cx;
UINT uEx;
BYTE off,des_len;
UINT uB;
float fB;

	lenNH=strlen((char *)pNH);
	
	pfmt=fmt;
	off=0;
	pNH0=pNH;
	while(*pfmt){		//顺序处理fmt中的Cx
	  iCx=X(*pfmt);
	  cx=getCX(iC0,iCx);
	  type=cfg[cx+CX_BELT_TYPE];

	  p=InStr(pNH,",",lenNH-off);
	  off=p-pNH0;
	  if(p){
		p++;			//指向NIHAO中的一个数据（字符串）
		pNH=p;
		uEx=IsException(p);
		if(uEx){		//出现异常标识
//		  jlkhjfljlskdjflkds 		
		}

		switch(type){
	      case FMT_COMPACT_U:		//紧凑格式
			sscanf((const char *)p,(const char *)"%u",&uB);
			des_len=Val2BELT(belt,(BYTE *)&uB,cx);
	  		belt+=des_len;
	        break;

	      case FMT_FLOAT_F:			//浮点格式
		    sscanf((const char *)p,(const char *)"%f",&fB);
			des_len=Val2BELT(belt,(BYTE *)&fB,cx);
	  		belt+=des_len;
	        break;

		  default:
		    break;
		}
	  }
	  else{			//处理完全部','
		break;
	  }
	}
	return OK;
}

//==============================================
// Receive form sensor
// 按PROTOCOL_YR_STANDARD接收Sensor响应
//
// Chars received is in Stream[].最长不超过64个字节
//
//==============================================
BYTE RxFromSensor(UINT LimitMS)		
{
BYTE c,iStream,ssRET;
UINT iCmp;

	ClrUErr();
	while(URXDA()){
	  c=URXREG();	//clear FIFO
	}
	
	TX_IE_OFF();	
	RX_IE_OFF();	

	Timer2_on();

	iCmp=ms_click+LimitMS/2;

	FillBufWith(&Stream[0],64,0x0);
	iStream=0;
	while(1){
      ClrWdt();

	  if(URXDA()){		
    	c=URXREG();		
		if(iStream<64){Stream[iStream++]=c;}

		if(iStream==1){	//接收到的第一个字符为‘！’		
		  if(Stream[0]!='!'){
			ssRET=SS_ERR; break;}
		}

		if(iStream==2){	//长度超限		
		  if((Stream[FRM_LEN]<(lenSHORT-1))||(Stream[FRM_LEN]>(lenCELL-1))){
			ssRET=SS_ERR; break;}
		}

		if(iStream<lenHEAD-1)continue;
		if(iStream==lenHEAD-1){
		  if(CheckCRC(&Stream[0],lenSHORT-2)){
	  		if(Stream[FRM_LEN]==(lenSHORT-1)){
	    	  ssRET=SS_OK; 
			  break;				//OK！短指令
	  		}
	  		else{continue;}			//继续等待长指令结束
		  }
		}

		//iStream > 15	
		if(iStream>Stream[FRM_LEN]){
	      if(CheckCRC(&Stream[lenSHORT],Stream[FRM_LEN]-lenSHORT-1)){
	        ssRET=SS_OK; 			//OK！长指令
			break;					//bytes ready in buff0;
	      }
	      else{ssRET=SS_ERR; break;}		//长指令校验错误
	    }
	  }

	  //超时退出
	  if(IsTimeUp(iCmp)==TRUE){
		ssRET=SS_ERR;
		break;
	  }
	}
	return ssRET;
}


//========================================================
//	             Read/write Slave serial sensor
//buff0: 1st byte of string(instruction/return) buffer
//len0:	length of instruction string byte 
//return: com procedure status
// 在无Sensor时，最大延时：3s
//========================================================
BYTE RWSerialSensor(BYTE *inst,BYTE len0,BYTE *resp,UINT resp_limit,BYTE speed_mode)
{
BYTE cRET,iNO_ECHO;
UINT ipl_save;
BYTE com_save;

	di(ipl_save,IPL_CALENDAR_INT);		//prevent from calendar int。

	com_save=ActiveCom;
	ActiveCom=COM3;

	RS232_on();
  	Serial_Setup(BAUD_19200);
	cRET=0;

//快速模式时，至多等待300ms;
//慢速模式时，至多等待6S
	if(speed_mode==MODE_QUICK){iNO_ECHO=3;}
	else{iNO_ECHO=60;}

	while(iNO_ECHO){
      Serial_Transmit('A');			//Wakeup sensor before communication，接收'R'(中断的响应)	
	  if(IsStrInStream(0,"R",MS100)==TRUE){
		break;
	  }
	  else{
		if(--iNO_ECHO==0){
	      cRET=SS_ERR1;
	      goto RWSS_END;
		}
	  }
	}

//发送指令
	TransmitBytes(inst,len0);		//Send bytes in buff0

//接收响应	
	cRET=RxFromSensor(WITHIN_1S*6);

	if(cRET==SS_OK){
	  DataCpy(resp,&Stream[0],Stream[FRM_LEN]+1);
	}

RWSS_END:
	RS232_off();

	ActiveCom=com_save;
	ei(ipl_save);	
	return cRET;
}

