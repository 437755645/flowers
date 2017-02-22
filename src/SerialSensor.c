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
// У��NIHAO��Ӧ������
//
// HAO@src#n, val1,val2��valN,~chksum <CR>
// chksum : "3AB8"
//
// pCR: ָ��CR
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

	p=pCR-4;		//ָ��chksum
	if(LoOfInt(sum)!=ValueHex2(p))return 0;
	if(HiOfInt(sum)!=ValueHex2(p+2))return 0;
	return 1;
}

//==============================================
// Receive form sensor
// ��PROTOCOL_NIHAO����Sensor��Ӧ
//
// 1��Ҫ�󴫸�������Now-Ready����;
//==============================================
BYTE RxFromSensor_NIHAO(void)
{
UINT EndMs;
BYTE r;
BYTE *p,*pEnd;

	
	EndMs=InitRx(2500,FILTER_NIHAO);	//2.5s, �����ʱ
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
// �ж��Ƿ�"^NNNN"
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
	while(*pfmt){		//˳����fmt�е�Cx
	  iCx=X(*pfmt);
	  cx=getCX(iC0,iCx);
	  type=cfg[cx+CX_BELT_TYPE];

	  p=InStr(pNH,",",lenNH-off);
	  off=p-pNH0;
	  if(p){
		p++;			//ָ��NIHAO�е�һ�����ݣ��ַ�����
		pNH=p;
		uEx=IsException(p);
		if(uEx){		//�����쳣��ʶ
//		  jlkhjfljlskdjflkds 		
		}

		switch(type){
	      case FMT_COMPACT_U:		//���ո�ʽ
			sscanf((const char *)p,(const char *)"%u",&uB);
			des_len=Val2BELT(belt,(BYTE *)&uB,cx);
	  		belt+=des_len;
	        break;

	      case FMT_FLOAT_F:			//�����ʽ
		    sscanf((const char *)p,(const char *)"%f",&fB);
			des_len=Val2BELT(belt,(BYTE *)&fB,cx);
	  		belt+=des_len;
	        break;

		  default:
		    break;
		}
	  }
	  else{			//������ȫ��','
		break;
	  }
	}
	return OK;
}

//==============================================
// Receive form sensor
// ��PROTOCOL_YR_STANDARD����Sensor��Ӧ
//
// Chars received is in Stream[].�������64���ֽ�
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

		if(iStream==1){	//���յ��ĵ�һ���ַ�Ϊ������		
		  if(Stream[0]!='!'){
			ssRET=SS_ERR; break;}
		}

		if(iStream==2){	//���ȳ���		
		  if((Stream[FRM_LEN]<(lenSHORT-1))||(Stream[FRM_LEN]>(lenCELL-1))){
			ssRET=SS_ERR; break;}
		}

		if(iStream<lenHEAD-1)continue;
		if(iStream==lenHEAD-1){
		  if(CheckCRC(&Stream[0],lenSHORT-2)){
	  		if(Stream[FRM_LEN]==(lenSHORT-1)){
	    	  ssRET=SS_OK; 
			  break;				//OK����ָ��
	  		}
	  		else{continue;}			//�����ȴ���ָ�����
		  }
		}

		//iStream > 15	
		if(iStream>Stream[FRM_LEN]){
	      if(CheckCRC(&Stream[lenSHORT],Stream[FRM_LEN]-lenSHORT-1)){
	        ssRET=SS_OK; 			//OK����ָ��
			break;					//bytes ready in buff0;
	      }
	      else{ssRET=SS_ERR; break;}		//��ָ��У�����
	    }
	  }

	  //��ʱ�˳�
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
// ����Sensorʱ�������ʱ��3s
//========================================================
BYTE RWSerialSensor(BYTE *inst,BYTE len0,BYTE *resp,UINT resp_limit,BYTE speed_mode)
{
BYTE cRET,iNO_ECHO;
UINT ipl_save;
BYTE com_save;

	di(ipl_save,IPL_CALENDAR_INT);		//prevent from calendar int��

	com_save=ActiveCom;
	ActiveCom=COM3;

	RS232_on();
  	Serial_Setup(BAUD_19200);
	cRET=0;

//����ģʽʱ������ȴ�300ms;
//����ģʽʱ������ȴ�6S
	if(speed_mode==MODE_QUICK){iNO_ECHO=3;}
	else{iNO_ECHO=60;}

	while(iNO_ECHO){
      Serial_Transmit('A');			//Wakeup sensor before communication������'R'(�жϵ���Ӧ)	
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

//����ָ��
	TransmitBytes(inst,len0);		//Send bytes in buff0

//������Ӧ	
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

