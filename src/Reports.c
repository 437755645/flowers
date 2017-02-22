#include 	"header.h"
#include    "h_macro.h"


extern BYTE 	IPL_CPU;

extern BYTE 	Step[];			//�������STEPS_

extern BYTE		Tasking[];		//0:δ������ 1��������
extern BYTE		ActiveTask;
extern BYTE 	ActiveCom;
extern BYTE		ComOpen[];		//COM��ռ�ñ��

static BYTE 	stepFC;
extern BYTE 	stepDO_REPORT;
extern BYTE 	stepINT_REPORT;

extern BYTE		ModemReady[];

extern MSG2		Msg2;
extern BYTE 	IntC0[];	//�˿ڵ��жϱ�־�����жϴ������ˢ��


extern hTIME	rTime;			//���ڱ����������ʱ�䣨����15�֣���5�֣�
extern BYTE 	c0Table[],cxTable[];	
extern BYTE 	cfg[];
extern PORT		Port[];
extern BYTE 	CoEntry[];		//Co��ڵ�ַ������62����Ԫ(0-9,A-Z,a-z)��

#ifdef  STOP_WATCH_ON
extern UINT 	stopwatch[];			//stopwatch
extern UINT 	ms_click;
#endif

//================================
//Bit usage variables
//================================
extern MSG0			Msg0;
extern MSG1			Msg1;
extern ERRORS		Error0;
extern TIMERS		Tmr;
extern TIMEUP 		TmrUp;


TMISSION queue[8];	//���8��ͨ������,0��ʾ���н�����


//===============================================================================
// Sample:
//CMPJ��TMRP, %1@1;				INTC0.2, (%1@1) ? (%4@2);INTC1.6, (%3@1) ? ( %5@2);��
//CMPJ��TMRP, %1@1&%4@2;			INTC1.2, (%1@1) ? (%4@2);INTC1.6, (%3@1) ? ( %5@2);��
//CMPJ��TMRP, %1@1?%4@2;			INTC1.2, (%1@1) ? (%4@2);INTC1.6, (%3@1) ? ( %5@2);��
//CMPJ��TMRP, %1@1&%4@2&%1@3;	INTC3.2, (%1@1) ? (%4@2);INTC1.6, (%3@1) ? ( %5@2);��
//===============================================================================

BYTE *CMC(BYTE offset)
{
	return &cfg[0]+cfg[CM_COM0]*lenCELL+offset;
}

BYTE *CMX(BYTE offset)
{
	return &cfg[0]+cfg[FLD_LEN]*lenCELL+offset;
}


//===============================================================================
// ���жϣ�����||�������������Report
//
// * ����ͬ�жϷ���һ��˳����ĺô����ڣ�
//   ��ͬ�ж�ͬʱ����ʱ��������̽������������ڽ����ظ���
//===============================================================================
void IntReports(void)
{
static BYTE i,j;	
static BYTE INTCIJ[7];		

static BYTE *p;
static TMISSION *q;			//ָ��ͨ��������С�0��ʾ���������8����

static int count;

//�Ŷ�
if(Tasking[TASK_CALENDAR_INT_REP]){
  ActiveTask=TASK_CALENDAR_INT_REP;
}
else{
  ActiveTask=TASK_SENSOR_INT_REP;
}


//***
switch(stepINT_REPORT){
  case 0:
		q=&queue[0];
		p=FindParaStr(CMC(CMC_LINK),"CMPJ");	//��ȡCMPJ�ַ���
		//if(p==0){}					//��ʱ�����¼������CMPJ""

//==============
//��ʱ��"TMRP"
	  	if(Tasking[TASK_CALENDAR_INT_REP]){
	      p=InStr(p,"TMRP",20);
	      if(p){							//goto REPORT_SENSOR;}
		    p+=4;	//skip "TMRP"
	        stepINT_REPORT=2; stepFC=0;
  	case 2: p=FC_Proc(q,p);			
		    if(p==RECYCLE){
		      count++;		//���룡����
		      return;}		
	        if(*p=='"'){	//������
			  Tasking[TASK_CALENDAR_INT_REP]=0;
		      goto INT_REPORT_OVER;
		    }
		  }
		}
		
//==============
//��������
		if(Tasking[TASK_SENSOR_INT_REP]){
	  	  for(i=0;i<C0S;i++){
	        if(IntC0[i]==0){continue;}		//�������ж���
	        for(j=0;j<8;j++){				//����ÿһ�ж�λ,��ʱ����Ԫ��<=8
		      if(testbit(IntC0[i],j)){
		        GetINTC(i,j,&INTCIJ[0]);
	    	    p=InStr(p,(const char *)&INTCIJ[0],20);		//Ѱ�� "INTCij"
		        if(p){
			      stepINT_REPORT=3; stepFC=0;
  		  case 3: p=FC_Proc(q,p);		
			      if(p==RECYCLE){	
					count++;	//���룡����
					return;}
			      if(*p=='"'){	//������
					Tasking[TASK_SENSOR_INT_REP]=0;
      			    goto INT_REPORT_OVER;
			      }
		        }
		      }  
	        }
	      }
		}

INT_REPORT_OVER:
		EndCom();
		ActiveTask=TASK_IDLE;	//�ͷ�����Ȩ��
		ei_all(); 
		return;			//ȫ������

  default:
		goto INT_REPORT_OVER;
  }
}


//DoReport()  
//	1)����ʱ����Ӧ��Ӧ�������ͣ�
//	2)��ʱ�����ϱ����ͣ��������ʱ��


//====================================
// FmtCom��ʽ����:
//
//  :DDGG #X@Y
//	:DDGG #X@Y & #X@Y & #X@Y & #X@Y 
//	:DDGG #X@Y ? #X@Y
//
// ���أ�
//    x: 	(����)ָ�룬ָ��մ����FC:#X@Y�󡣲�����
//    0��	δ������ϣ�����ȴ���
//====================================
BYTE *FC_Proc(TMISSION *q,BYTE *p)		//p:ָ����ʼ��
{
static BYTE fc;			//[#### @@@@]=[��ʽ/ͨ��]		
static TMISSION *fit;	//ָ��queue[]��ִ�й���fc��λ��
BYTE r,timefit;
PROPERTY property;

	property.ALLbits=0;
	property.bitINST_DIR=1;				//�Ա�ָ��
	property.bitANSWER_NEEDED=1;		//�ڴ��ظ���ͨ�ŷ�ʽ�ɸ��Ǳ����ԣ�

	switch(stepFC){
  	  case 0:
		while((*p!=0)&&(*p!=';')&&(*p!='"')){	//';'��ʾ������
		  if(*p==':'){
		    p=TimeFit(p,&timefit);
		    if(!timefit){p+=8; continue;}			//�����϶�ʱ����,skip
		  }
		  else if(*p==','){
			p++; continue;
		  }
		  else if(*p=='?'){
		    if(fit->fSuccess==TRUE){
			  p+=5; continue;		//ǰһ��ͨ�ųɹ���skip�����ŵ�#f@c
		    }
		    else{p++; continue;}	//ǰһ��ͨ�Ų��ɹ���ִ�б����ŵ�
	      }

	  	  p=GetFC(p,&fc);			//fc=[#X@Y],pָ��#X@Y�����һ���ַ���
	  	  if(IffcRepeated(fit,q,fc)==TRUE){		//����ִ�й���fc
		    continue;
	  	  }
	
	      if(!p){while(1){ClrWdt();}}
	      q->fc=fc;			//��fc����queue[]
	 	  q++;

		  stepFC=1; stepDO_REPORT=0;
  	  case 1: 
		  r=DoReport(fc,property.ALLbits);
		  if(r==0xff){return RECYCLE;}		//���룡����
	  }
	  return p;		//ָ��δ�������һ���ַ���

  	  default: while(1){ClrWdt();}		
	}
}

//==================================
//		ͨ��"@ncp..."��ȡͨ����c
//
// ���أ�channel��
//      0:    ָ����@n������
//      ������ͨ����
//==================================
BYTE getChannel(BYTE tsk_id)
{
BYTE tsk_str[10];
BYTE cmfg[128],*p;

	tsk_str[0]='@';
	tsk_str[1]='0'+tsk_id;
	tsk_str[2]=0;

	p=GetComStr(tsk_id,&cmfg[0]);		//��ȡ@nͨ������
	if(p==0){return 0;}

	return *(p+2)-'0';	//ͨ����c
}

/*
BYTE *FC_Proc(TMISSION *q,BYTE *p,BYTE fTFIT)	//p:ָ����ʼ��
{
BYTE fc,channel;
BYTE r;
TMISSION *fit;	//ָ��queue[]��ִ�й���fc��λ��

	switch(stepFC){
	  case 0:
	    while((*p!=0)&&(*p!=';')&&(*p!='"')){	//';'��ʾ������
	      p=GetFC(p,&fc);						//fc=[%X@Y],pָ��%X@Y�����һ���ַ���
	      if(IffcRepeated(fit,q,fc)==TRUE){			//����ִ�й���fc
	  	    if(*p=='&'){				//��������&��
	          p++; continue;			//pָ����һ����%X@Y����
	        }

	      	if(*p=='?'){
		      if(fit->fSuccess==TRUE){return 0;}	//ǰһ��ͨ�ųɹ�����ִ�б����ŵ�
			  else{p++; continue;}
	        }
		  }
	    
		  channel=fc&0x0f;
		  if(sw_TimeFit(channel,fTFIT)==TRUE){	//�ж�channel_sw��ʱ������
		    r=DoReport(fc);
			goto FC0_END;
	      }
		}
		return 0;		//����	

FC0_END:
	    stepFC=1;
	    break;

	  case 1:
//		r=ComResp();
  	    if(r==SUCCESS){
	      q->fc=fc;			//��fc����queue[]
	 	  //q->sec_stamp=GetSecStamp();
		  q->fSuccess=TRUE;
		  q++;
	  	  if(*p=='&'){		//��������&��, pָ����һ����%X@Y����
		    p++; stepFC=0;
	        break;
		  }
		  else{			//=='?'�򡮡���
			stepREP=2; 	//�����������������ϲ����
			break;
		  }	
		}
	    else{	//ʧ��
	  	  if(*p=='?'){		
		    p++; stepFC=0; //pָ����һ����%X@Y��,��������ͨ����
	        break;
		  }
		  else{	//�ޱ���ͨ��
			stepREP=2; 	//�����������������ϲ����
			break;
		  }
	    }
	  default:
		break;
	}
	return p;		//ָ��δ�������һ���ַ���
}
*/

//======================================
// �ж�fc�Ƿ��Ѿ���ִ�й��Ķ�����
// fit: ָ��ƥ���λ��
// q:   ��ǰ��λ��
//======================================
BYTE IffcRepeated(TMISSION *fit,TMISSION *q,BYTE fc)
{
//TMISSION *p;

	fit=&queue[0];
	while(fit!=q){
	  if(fc==fit->fc){
		return TRUE;
	  }
	  fit++;
	}
	return FALSE;
}


//==================================
// ��ȡ��INTCij��
//==================================
void GetINTC(BYTE i,BYTE j,BYTE *str)
{
	*str++='I';
	*str++='N';
	*str++='T';
	*str++='C';
	*str++=i+'0';		//��Ԫ
	//*str++='.';
	*str++=j+'0';		//��Ԫ
	*str++=0;
}

//===================================
//
// fc=[%X@Y],pָ��%X@Y�����һ���ַ���
//===================================
BYTE *GetFC(BYTE *p,BYTE *fc)
{
	if((*p==';')||(*p=='"')){return 0;}

	if(*p=='#'){
	  *fc=(X(*(p+1))<<4)+(X(*(p+3))&0x0f);		//����16�ָ�ʽ��16��ͨ������
	  p+=4;
	}
	return p;
}

/*
//===================================
//
// ��չ�ַ���
// "0-9","A-Z","a-z" ==> 0-61
//===================================
BYTE XCode(BYTE x)
{
	if(x<='9'){x=x-'9';}						//0-9
	else if((x>='A')&&(x<='Z')){x=x-'A'+10;}	//10-35
	else if((x>='a')&&(x<='z')){x=x-'a'+36;}		//36-61
	return x;
}
*/




//=============================================
// �ҵ���chan_id��Ӧ��ͨ�Žṹ������ͨ������
//
// channel: 0-3
// [S LLLL TTT]
//=============================================
BYTE GetComType(BYTE *sComTask)
{
BYTE *p,*r;
BYTE lenStr;

	p=sComTask+9;		//ָ��'['���һ���ַ�
	lenStr=6;			//		//strlen((char *)sComTask);

	r=InStr(p,"DIRECT",lenStr);
	if(r){return CONNECT_DIRECT;}

	r=InStr(p,"GPRM",lenStr);
	if(r){return CONNECT_GPRM;}

	r=InStr(p,"GPRS",lenStr);
	if(r){return CONNECT_GPRS;}

	r=InStr(p,"SMPN",lenStr);
	if(r){return CONNECT_SM;}

	r=InStr(p,"BDID",lenStr);
	if(r){return CONNECT_BEIDOU;}

	r=InStr(p,"WIFI",lenStr);
	if(r){return CONNECT_WIFI;}

	return 0;
}


//======================================================
// �ж��Ƿ����㶨ʱ����
//
// ���أ�
// fit:	TRUE:  ����
//		FALSE��������
// p: ָ��":GGDD"��
//======================================================
BYTE *TimeFit(BYTE *p,BYTE *fit)		//p ָ��Gap&Delay��":GGDD"
{
BYTE TimeUnit,UnsoInterval;
BYTE biaDelay,c;

//TMRP:4F00#1@1&:8C08#4@3;
	if(*p!=':'){
	  *fit=FALSE; 
	  return p;
	}

	p++;
	c=ValueHex2(p);
	UnsoInterval=c&0x3f;	//���������UU(��λ)
	if(UnsoInterval==0){	//0����ֹ������
	  *fit=FALSE; 
	  return p;
	}

	TimeUnit=c>>6;			//UU(��λ)

	p+=2;					//ָ��DD��
	c=ValueHex2(p);
	biaDelay=c&0x3f;
	p+=2;					//ָ��DD����

	*fit=FALSE;
	switch(TimeUnit){
	  case UNIT_HOUR:
        if(rTime.minute==0){					//���㱨
		  if(((rTime.hour+UnsoInterval-biaDelay)%UnsoInterval)==0){
			*fit=TRUE;		//ˮ�ģ�ȷ��8:00����
		  }
	    }
	    break;

	  case UNIT_MINUTE:
        //if(rTime.second==0){					//�����ӱ�
	    if((rTime.minute+UnsoInterval-biaDelay)%UnsoInterval==0){			//������
		  *fit=TRUE;							
	    }
	    break;

	  case UNIT_SECOND: 
	    if((rTime.second+UnsoInterval-biaDelay)%UnsoInterval==0){			//������
		  *fit=TRUE;							
	    }
	    break;


	  default: break;
	}
//
//	else if(fTFIT==fTFIT_MIN_GAP){
//	}
//
	return p;
}



//===============================================
//��ʾ��ʽ�ţ�ͨ�ź�F:XX C:XX
//===============================================
void DspFC(BYTE fc)		
{
BYTE sFC[9];			//"F:XX C:XX"

	sFC[0]='F'; sFC[1]=':';
	sprintf((char *)&sFC[2],"%02d",fc>>4); 
	sFC[4]=' '; 
	sFC[5]='C'; sFC[6]=':';
	sprintf((char *)&sFC[7],"%02d",fc&0x0f);

	ActiveCom=LCD_PORT;
	lcd_clear();
	lcd_puts(0,0,(const char *)&sFC[0]);		
}

//==========================================================
// "@n sw channel protocol - min_gap [comtype paras]"
// "@1111-45[GPIP:ffffffffffff,ffffffff0000,ffffffffffff]"
//
// ���أ�
//     ָ��ָ���ҵ�str���׵�ַ'@'
//==========================================================
BYTE *GetComStr(BYTE comtsk_id,BYTE *cmfg)
{
BYTE *p;		//,*p0;
BYTE sn[5];
UINT lenCMFG;

	sn[0]='@';
	btoa(comtsk_id&0x0f,(char *)&sn[1]);			//���16��com����

	p=CMC(CMC_LINK);
	p=FindParaStr(p,"CMFG");
	p++;

	lenCMFG=*(CMC(CMC_CMFG_LEN));			//��������
	p=InStr(p,(const char *)&sn[0],lenCMFG);		//Ѱ�ҡ�@n��	
	if(p){
	  CopyParaStr(p,cmfg,'@');
	}
	return p;
}


//=======================================================
// GPRSͨ��
// cmfg:
// @1111-45[GPRS:ffffffffffff,ffffffff0000,ffffffffffff]
//=======================================================
BYTE DoGPRS(BYTE *buf,UINT lenMsg,BYTE *cmfg)
{
BYTE *p,lenStr;
BYTE sIp[16],sPort[6];		//"122.224.172.153:25005"
BYTE r;

	if(!Connect(CONNECT_GPRS)){
	  Scroll(WAY_CATCH,"...����Ӧ");
	  return FAILED;
	}

//TCOM������ip
	lenStr=strlen((char *)cmfg)-8;
	p=cmfg+8;						//ָ��[GPRS
  	p=InStr(p,"[GPRS:",lenStr);		//Ѱ�ҡ�[��
	if(!p){return FAILED;}
	p+=6;

//����TCOM������
	if(getIp(&sIp[0],p)==FAILED){return FAILED;}
	if(getPort(&sPort[0],p+8)==FAILED){return FAILED;}
	r=ConnectTcpServer((BYTE *)&sIp[0],(BYTE *)&sPort[0]);	//"122.224.172.53",50051	

//����
	if(r==GPRS_CONNECTION_OK){
	  GPRS_transmit(buf,lenMsg);
	  return OK;
	}
	else{
	  return FAILED;
	}
}


//=======================================================
// Wifiͨ��
// cmfg:
// @1111-45[WIFI:ffffffffffff,ffffffff0000,ffffffffffff]
//=======================================================
BYTE DoWifi(BYTE *buf,UINT lenMsg,BYTE *cmfg)
{
BYTE *p,lenStr;
//BYTE sIp[16],sPort[6];		//"122.224.172.153:25005"
BYTE psw[20];
BYTE *ip_port;
BYTE *p_start,*p_end;
char r;

	if(!Connect(CONNECT_WIFI)){return FAILED;}

//TCOM������ip
	lenStr=strlen((char *)cmfg)-8;	//8:'['֮ǰ�ַ���
	p=cmfg+8;						//ָ��[GPRS
  	p_start=InStr(p,"[WIFI:",lenStr);		//Ѱ�ҡ�[��
	p_end=InStr(p,"]",lenStr);
	if(!p_start){return FAILED;}

  	p=InStr(p_start,"[PSW=",p_end-p_start);		//Ѱ�ҡ�[��

//��ʱ
psw[0]='t';
psw[1]='o';
psw[2]='n';
psw[3]='g';
psw[4]='x';
psw[5]='i';
psw[6]='n';
psw[7]='k';
psw[8]='e';
psw[9]='@';
psw[10]='4';
psw[11]='0';
psw[12]='1';
psw[13]='\0';


//����TCOM������
	ip_port=p_start+6;
	r=WifiConnection(ip_port,&psw[0]);		
	if(r==1){
//����
	  GPRS_transmit(buf,lenMsg);
	  return OK;
	}
	else{return FAILED;}
}



//====================================================
// GPRMͨ��
// cmfg:
// @1111-45[GPRM:ffffffffffff,ffffffff0000,ffffffffffff]
//====================================================
BYTE DoGPRM(BYTE *buf,UINT lenMsg,BYTE *cmfg)
{
BYTE *p,lenStr;

	if(!Connect(CONNECT_GPRM)){return FAILED;}

//ز��ǰȺ��
	SendAT("AT+GPRSMASS=0",0x0d);	
	if(!IsStrInStream(0,"OK",WITHIN_1S*3)){		
	  return FAILED;
	}

//Ⱥ��Ŀ��
	lenStr=strlen((char *)cmfg)-8;
	p=cmfg+8;						//ָ��[GPRM
  	p=InStr(p,"[GPRM:",lenStr);		//Ѱ�ҡ�[��
	if(!p){return FAILED;}
	p+=6;
	
	//at+gprsgrou="ffffffffffff?ffffffff0000,ffffffffffff"��0x0d
	SendAT("at+gprsgrou=\"",0x0);
	while(*p!=']'){Serial_Transmit(*p++);}
	SendAT("\"",0xd);
	
	if(!IsStrInStream(0,"OK",WITHIN_1S*3)){		
	  return FAILED;
	}

//MSG����
	SendAT("AT+GPRSMSG",0x0d);	
	if(!IsStrInStream(0,">",WITHIN_1S*3)){		//�ȴ�">"
	  return FAILED;
	}

//Msg����
	while(lenMsg){
	  Serial_TransmitHL(*buf);
	  buf++; lenMsg--;
	}
	Serial_Transmit(ASC_CTRL_Z);

	if(!IsStrInStream(0,"OK",WITHIN_1S*3)){		
	  return FAILED;
	}

//����Ⱥ��
	SendAT("AT+GPRSMASS=1",0x0d);	
	if(!IsStrInStream(0,"OK",WITHIN_1S*3)){		
	  return FAILED;
	}

/*
	if(!IsStrInStream(0,"A",WITHIN_1S*30)){		//״̬�仯����
	  return FAILED;
	}
*/
//	if(IsStrInStream(0,"OK",WITHIN_1S)){
//	  r=XModemRet();
//	  return OK;
//	}

	return OK;
}

//=========================================================================
//	DA6C1095138A,DA6C1095138A,FFFFFFFFFFFF,FFFFFFFFFFFF]
//	---> 218.108.16.149:5002,218.108.16.149:5002 ? 218.103.12.144:1005
//=========================================================================
/*
BYTE *ConvertIpStr(char *des,BYTE *src)
{
BYTE i;
char s[6]; //���ip�е�1�����Ͷ˿ڡ��˿����6 byte.
UINT u1,u2;

	*des=0;
//ip "218.108.16.149"
	for(i=0;i<4;i++){
	  sprintf((char *)&s[0],"%d",(ValueHex(*src)<<4)+ValueHex(*(src+1)));
	  src+=2;
	  strcat(des,&s[0]);
	  if(i==3){break;}
	  strcat(des,".");
 	}
 	strcat(des,":");

//port "5002"
	u1=(((UINT)ValueHex(*src))<<4)+ValueHex(*(src+1)); 
	src+=2;
	u2=(((UINT)ValueHex(*src))<<4)+ValueHex(*(src+1));
	u1=(u1<<8)+u2;
	src+=2;				//src:ָ��ip�ַ�����--> ',','?',']'
	sprintf(&s[0],"%u",u1);

	strcat(des,&s[0]);  
	return src;
}
*/

/*
void GetIpGroup(char *des,BYTE *src)
{
char ip_port[22];	//����ip��aaa.bbb.ccc.ddd:ppppp+�ָ���,��󳤶ȣ�22
char sep[2];

	*des=0;
	while(1){
	  src=ConvertIpStr(&ip_port[0],src);
	  strcat(des,(const char *)&ip_port[0]);
	  if(*src==']'){break;}

	  sep[0]=*src;
	  sep[1]=0;
	  strcat(des,&sep[0]);				//�ָ���
	  src++;
	}
}
*/


//====================================================
// BEIDOUͨ��
// cmfg:
//	@321BDID 327680
//====================================================
BYTE DoBEIDOU(BYTE *buf,BYTE lenMsg,BYTE *cmfg)
{
BYTE r;
//BYTE len;
//BYTE tel[13];

	cmfg+=9;
//
	PW_BEIDOU_on();	
	Connect(CONNECT_BEIDOU);
	r=BEIDOU_Transmit(buf,lenMsg,cmfg);
	PW_BEIDOU_off();	

	//������������־
	//ClrCfgBit(REG0+((UINT)(MAIN_PROPERTY2)),bitSENSOR_REPORT_REQUEST);
	fSensorRepRequest=0;

	if(r==OK){
	  Scroll(WAY_CATCH,"...�ɹ�");
	}
	else{
	  Scroll(WAY_CATCH,"...ʧ��");
	}
	Delay_s(3);
//	  goto END_REPORT;

	  //�Է����ղ��ԣ�Ϊ�˱�����ϣ��ȴ�10s��
//	  Delay_s(10);
//	  Delay_s(60);
//	  Delay_s(30);
//	  BEIDOU_TimeAutoAdjust();		//��ʱ��У׼���豸����

	return r;
}


//====================================================
// DIRECTͨ��
// 
// cmfg: @211DIRE[]
//====================================================
BYTE DoDIRECT(BYTE *buf,UINT lenMsg,BYTE *cmfg)
{
BYTE r;
//BYTE i=0;

	RS232_on();
 	Serial_Setup(BAUD_19200);

//�жϷ�ʽ
//	Tx_Count=lenMsg;
//	InitTx(buf);
//  while(Tx_Count)continue;

//di_all();
	while(lenMsg--){
	  Serial_Transmit(*buf++);
	}

    Delay_ms(10);
    Serial_End_Tx();				
	Delay_ms(10);
	r=OK;
	return r;
}

//====================================================
// Ultrashort waveͨ��
// 
// cmfg: @211DIRE[]
//====================================================
BYTE DoUsWave(BYTE *buf,UINT lenMsg,BYTE *cmfg)
{
BYTE r;

	RS232_on();
 	Serial_Setup(BAUD_300);

//�жϷ�ʽ
//	Tx_Count=lenMsg;
//	InitTx(buf);
//  while(Tx_Count)continue;

//di_all();
	while(lenMsg--){
	  Serial_Transmit(*buf++);
	}

    Delay_ms(10);
    Serial_End_Tx();				
	Delay_ms(10);
	r=OK;
	return r;
}


//====================================================
// cmfg:
//	@211SMPN 13558108677,13958108766
//====================================================
BYTE DoSM(BYTE *buf,UINT lenMsg,BYTE *cmfg)
{
BYTE maxLen;
BYTE sm[160];

	memcpy(&sm[0],buf,lenMsg);
	maxLen=Reserve_6_bits(&sm[0],lenMsg);
	BinaryVisible(&sm[0],&sm[0],maxLen);		//data is ready in sm[]
//

//���Modem״̬
	if((IsModemReady()==FALSE)){
	  AT_creg(ActiveCom);		//����ˢ�£���֤ʱ��ͣ��ʱ��Ҳ����Ӧ	
	  if(IsModemReady()==FALSE){
	    Scroll(WAY_CATCH,"...ʧ�� Modem not ready");
		return 0;
	  }
	}	


//SMȺ��
	cmfg+=9;	//ָ��ͨѶ¼tels

	SendAT("AT+SMGROU=",0);	//SM MASS����
	TransmitStr(cmfg);
	Serial_Transmit('"');
	Serial_Transmit(0x0d);

	SendAT("AT+SMSG=",0);	//SM����
	while(lenMsg){
	  Serial_TransmitHL(*buf);		
	  buf++; lenMsg--;
	}
	Serial_Transmit('"');

	SendAT("AT+SMMASS=1",0x0d);	//����Ⱥ��
	return 1;
}

//===============================================
//        			Reporting
//
// 1����%X@Yͨ�ţ�
// 2�������1���ŵ����ɸ���ͨ�ţ��������͡�Ⱥ���ȣ�
//
// ���أ�
//    0: ʧ�ܣ�
//    1: �ɹ�
//    0xff: ���루δ��ɣ�
//===============================================    
BYTE DoReport(BYTE fc,UINT property)		
{    
BYTE r,sw;
BYTE typeComm,COMi;
BYTE fmt_id,comtsk_id,prot_id;
BYTE msg[500];
UINT lenMsg;
BYTE cmfg[128],*p;		//����ͨ������CMFG� <= 128
PROPERTY pp;


	FillBufWith(&msg[0],500,0x3f);	//��ȱ���ݣ�0x3F[01 111111]

	pp.ALLbits=property;

	switch(stepDO_REPORT){
  	  case 0:
	  	fmt_id=fc>>4;
		comtsk_id=fc&0x0f;

		p=GetComStr(comtsk_id,&cmfg[0]);	//��ȡ@nͨ������
		if(p==0){return 0;}

		sw=X(cmfg[2]);			//2�����񿪹�sw		
		if(sw==0){return 1;}	//����

		prot_id=X(cmfg[4]);		//4:Э����

		lenMsg=MakeMsg(prot_id,fmt_id,&msg[0],property);		//��Ҫʱ���γ�

//======================================
		r=FAILED;
		typeComm=GetComType(&cmfg[0]);

//��ʾ��ʽ�ţ�ͨ�ź�  ==> F:XX C:XX
//		DspFC(fc);		

		COMi=X(cmfg[3])+1;		//3:ָ��ͨ��channel

/*	case 1:
		if(ComOpen[COMi-1]){
		  return 0xff;				//�ȴ�COM���ͷ�
		}							//���룡����			*/
		
//��ʱ
//COMi=2;
//typeComm=CONNECT_WIFI;

		ActiveCom=COMi;
    	switch(typeComm){

//==============DIRECTͨ��==============	
		  case CONNECT_DIRECT:
	  	    Scroll(WAY_CRLF," >����-DIRECT");
	  	    r=DoDIRECT(&msg[0],lenMsg,&cmfg[0]);
	  	    break;

//==============SMͨ��==================
    	  case CONNECT_SM:
	  	    Scroll(WAY_CRLF," >����-SM");
	  	    r=DoSM(&msg[0],lenMsg,&cmfg[0]);
	  	    break;

//==============GPRMͨ��(Ⱥ��)==========
    	  case CONNECT_GPRM:
	  	    Scroll(WAY_CRLF," >����(GPRM)");
	  	    r=DoGPRM(&msg[0],lenMsg,&cmfg[0]);
	  	    break;

//==============GPRSͨ��==========
    	  case CONNECT_GPRS:
	  	    Scroll(WAY_CRLF," >����(GPRS)");
	  	    r=DoGPRS(&msg[0],lenMsg,&cmfg[0]);
//��ʱ
//r=OK;
	  	    break;

//==============BEIDOUͨ��==============
    	  case CONNECT_BEIDOU:
	  	    Scroll(WAY_CRLF," >����-BEIDOU");
	  	    r=DoBEIDOU(&msg[0],(BYTE)lenMsg,&cmfg[0]);
	  	    break;

//==============UltrashortWaveͨ��==============
    	  case CONNECT_USWAVE:
	  	    Scroll(WAY_CRLF," >����-USWAVE");
	  	    r=DoUsWave(&msg[0],(BYTE)lenMsg,&cmfg[0]);
	  	    break;

//==============Wifiͨ��==============
    	  case CONNECT_WIFI:
	  	    Scroll(WAY_CRLF," >����-WIFI");
	  	    r=DoWifi(&msg[0],(BYTE)lenMsg,&cmfg[0]);
	  	    break;

		  default:break;
		}

		if(r==FAILED){		//���͹�����ز��
		  if(!r){Scroll(WAY_CATCH,"...ʧ��");}
		  goto END_DO_REPORT;
		}
	
		if(!pp.bitANSWER_NEEDED){
		  r=OK;						//����ҪAnswer
		  goto END_DO_REPORT;
		} 

		stepDO_REPORT=2;
	  case 2:	
//		r=GetComResp(typeComm,COMi);
//��ʱ
r=1;
		if(r==0xff){return 0xff;}			//���룡����

END_DO_REPORT:
		DelayAfterComm(typeComm);
//		Delay_s(1);		//�趨ä����1s�ڵ�������Modem���ص���Ϣ������

		Tasking[ActiveTask]=0;	//�رյ�ǰ����
		EndCom();
		return r;
	}
	
return 9;	//ʵ�ʲ��ɼ�
}

//===================================================================
//	Ϊ������ӦĳЩModem��ͨ�Ž��������µ磩ʱ��
//  �����źţ��趨һЩä��
//�趨ä����1s�ڵ�������Modem���ص���Ϣ������
//===================================================================
void DelayAfterComm(BYTE typeComm)
{
	switch(typeComm){
	case CONNECT_DIRECT:
	  break;				//ֱ������ä��
	default:
	  Delay_s(1);
	  break;
	}
}

//==============================
// ��ȡDoReport�Ľ��(��Ⱥ��)
//
// ���أ�
//		1��		�ɹ�
//		0��		ʧ��
//		0xff:	�ȴ�
//==============================
BYTE GetComResp(BYTE com_type,BYTE com_port)
{
BYTE r=0;
char group_id,id[2];
BYTE *out;

	switch(com_type){
	  case CONNECT_GPRM:
	    r=GPRM_end();
	    break;

	  case CONNECT_GPRS:
		r=GPRS_end();
	    break;

	  case CONNECT_DIRECT:
		r=DIRECT_end();
		break;
	}
/*
	group_id=0
	switch(comtype){
	  case CONNECT_GPRS:
		SendAT("AT+STATUS?",0x0d);
		if(IsStrInStream(0,"OK",WITHIN_1S)){
		  if(CheckCRC(&Stream[0],Stream[1]){
			//Stream[]����
			while(group_id<Stream[]){
			  if(OUTPUT_ON){
				ShowComProgress(&Stream[0]);
				//��ʾ: "GPRS����x:x"
				strcpy(out,"GPRS����");
				strcat(out,Stream[5]);
				strcat(out,":");
				strcat(out,Stream[6]);
				Scroll(out);
				
				//��ѯ���				
				strcpy(out,"AT+GPRSSTAT=");
				sprintf(&id[0],"%u",group_id);
				strcat(out,&id[0]);
				strcat(out,"?");
				SendAT(out,0x0d);
			  }
			}
			if(IsMassFinished()){
			}
		  } 
		}
	    break;
	  case CONNECT_GPRS:
	    break;
	}
*/
	if(r){
	  //�ɹ�
	}
	else{
	  //ʧ��
	}

	r=OK;
	return r;
}

//===========================================
// ���ȴ�1s
//
//===========================================
BYTE DIRECT_end(void)
{
BYTE r;
BYTE *rx,*p;
UINT EndMs;

	rx=RX_BUF0();

	EndMs=InitRx(1000,FILTER_DIRECT);
	r=FAILED;

	while(1){
	  Delay_ms(1);
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


//======================
//��ʾmsg: 
//	"GPRS����x:x"
//======================
/*
ShowComProgress(BYTE *msg,BYTE group_id,BYTE *stat_stream)
{
	strcpy(out,msg);
	strcat(out,Stream[5]);
				strcat(out,":");
				strcat(out,Stream[6]);
				Scroll(out);
}				
*/




//=================================
// TimeIntoBelt
//=================================
/*
void TimeIntoBelt(BYTE *belt,BYTE s,BYTE len)
{
	memcpy(belt,&rTime[s],len);
//	p+=len;

//	return p;
}
*/


