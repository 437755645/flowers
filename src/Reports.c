#include 	"header.h"
#include    "h_macro.h"


extern BYTE 	IPL_CPU;

extern BYTE 	Step[];			//定义见：STEPS_

extern BYTE		Tasking[];		//0:未启动； 1：运行中
extern BYTE		ActiveTask;
extern BYTE 	ActiveCom;
extern BYTE		ComOpen[];		//COM口占用标记

static BYTE 	stepFC;
extern BYTE 	stepDO_REPORT;
extern BYTE 	stepINT_REPORT;

extern BYTE		ModemReady[];

extern MSG2		Msg2;
extern BYTE 	IntC0[];	//端口的中断标志，由中断处理进程刷新


extern hTIME	rTime;			//用于保存整点或整时间（如整15分，整5分）
extern BYTE 	c0Table[],cxTable[];	
extern BYTE 	cfg[];
extern PORT		Port[];
extern BYTE 	CoEntry[];		//Co入口地址，至多62个基元(0-9,A-Z,a-z)；

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


TMISSION queue[8];	//最多8个通信任务,0表示队列结束。


//===============================================================================
// Sample:
//CMPJ”TMRP, %1@1;				INTC0.2, (%1@1) ? (%4@2);INTC1.6, (%3@1) ? ( %5@2);“
//CMPJ”TMRP, %1@1&%4@2;			INTC1.2, (%1@1) ? (%4@2);INTC1.6, (%3@1) ? ( %5@2);“
//CMPJ”TMRP, %1@1?%4@2;			INTC1.2, (%1@1) ? (%4@2);INTC1.6, (%3@1) ? ( %5@2);“
//CMPJ”TMRP, %1@1&%4@2&%1@3;	INTC3.2, (%1@1) ? (%4@2);INTC1.6, (%3@1) ? ( %5@2);“
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
// 由中断（日历||传感器）引起的Report
//
// * 将不同中断放在一起，顺序处理的好处在于：
//   不同中断同时发生时，处理过程较清晰。不至于交叉重复。
//===============================================================================
void IntReports(void)
{
static BYTE i,j;	
static BYTE INTCIJ[7];		

static BYTE *p;
static TMISSION *q;			//指向通信任务队列。0表示结束，最多8个。

static int count;

//排队
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
		p=FindParaStr(CMC(CMC_LINK),"CMPJ");	//获取CMPJ字符串
		//if(p==0){}					//空时（如记录器），CMPJ""

//==============
//定时报"TMRP"
	  	if(Tasking[TASK_CALENDAR_INT_REP]){
	      p=InStr(p,"TMRP",20);
	      if(p){							//goto REPORT_SENSOR;}
		    p+=4;	//skip "TMRP"
	        stepINT_REPORT=2; stepFC=0;
  	case 2: p=FC_Proc(q,p);			
		    if(p==RECYCLE){
		      count++;		//重入！！！
		      return;}		
	        if(*p=='"'){	//结束符
			  Tasking[TASK_CALENDAR_INT_REP]=0;
		      goto INT_REPORT_OVER;
		    }
		  }
		}
		
//==============
//传感器报
		if(Tasking[TASK_SENSOR_INT_REP]){
	  	  for(i=0;i<C0S;i++){
	        if(IntC0[i]==0){continue;}		//传感器中断字
	        for(j=0;j<8;j++){				//测试每一中断位,暂时：虚元数<=8
		      if(testbit(IntC0[i],j)){
		        GetINTC(i,j,&INTCIJ[0]);
	    	    p=InStr(p,(const char *)&INTCIJ[0],20);		//寻找 "INTCij"
		        if(p){
			      stepINT_REPORT=3; stepFC=0;
  		  case 3: p=FC_Proc(q,p);		
			      if(p==RECYCLE){	
					count++;	//重入！！！
					return;}
			      if(*p=='"'){	//结束符
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
		ActiveTask=TASK_IDLE;	//释放任务权柄
		ei_all(); 
		return;			//全部结束

  default:
		goto INT_REPORT_OVER;
  }
}


//DoReport()  
//	1)交互时的响应，应立即发送；
//	2)定时主动上报发送，可随机延时；


//====================================
// FmtCom格式处理:
//
//  :DDGG #X@Y
//	:DDGG #X@Y & #X@Y & #X@Y & #X@Y 
//	:DDGG #X@Y ? #X@Y
//
// 返回：
//    x: 	(非零)指针，指向刚处理的FC:#X@Y后。不重入
//    0：	未处理完毕，重入等待。
//====================================
BYTE *FC_Proc(TMISSION *q,BYTE *p)		//p:指向起始点
{
static BYTE fc;			//[#### @@@@]=[格式/通信]		
static TMISSION *fit;	//指向queue[]中执行过的fc的位置
BYTE r,timefit;
PROPERTY property;

	property.ALLbits=0;
	property.bitINST_DIR=1;				//自报指令
	property.bitANSWER_NEEDED=1;		//期待回复。通信方式可覆盖本属性；

	switch(stepFC){
  	  case 0:
		while((*p!=0)&&(*p!=';')&&(*p!='"')){	//';'表示结束；
		  if(*p==':'){
		    p=TimeFit(p,&timefit);
		    if(!timefit){p+=8; continue;}			//不符合定时条件,skip
		  }
		  else if(*p==','){
			p++; continue;
		  }
		  else if(*p=='?'){
		    if(fit->fSuccess==TRUE){
			  p+=5; continue;		//前一个通信成功，skip备用信道#f@c
		    }
		    else{p++; continue;}	//前一个通信不成功，执行备用信道
	      }

	  	  p=GetFC(p,&fc);			//fc=[#X@Y],p指向“#X@Y”后第一个字符；
	  	  if(IffcRepeated(fit,q,fc)==TRUE){		//跳过执行过的fc
		    continue;
	  	  }
	
	      if(!p){while(1){ClrWdt();}}
	      q->fc=fc;			//新fc加入queue[]
	 	  q++;

		  stepFC=1; stepDO_REPORT=0;
  	  case 1: 
		  r=DoReport(fc,property.ALLbits);
		  if(r==0xff){return RECYCLE;}		//重入！！！
	  }
	  return p;		//指向未处理的下一个字符。

  	  default: while(1){ClrWdt();}		
	}
}

//==================================
//		通过"@ncp..."获取通道号c
//
// 返回：channel。
//      0:    指定的@n不存在
//      其他：通道号
//==================================
BYTE getChannel(BYTE tsk_id)
{
BYTE tsk_str[10];
BYTE cmfg[128],*p;

	tsk_str[0]='@';
	tsk_str[1]='0'+tsk_id;
	tsk_str[2]=0;

	p=GetComStr(tsk_id,&cmfg[0]);		//获取@n通信任务
	if(p==0){return 0;}

	return *(p+2)-'0';	//通道号c
}

/*
BYTE *FC_Proc(TMISSION *q,BYTE *p,BYTE fTFIT)	//p:指向起始点
{
BYTE fc,channel;
BYTE r;
TMISSION *fit;	//指向queue[]中执行过的fc的位置

	switch(stepFC){
	  case 0:
	    while((*p!=0)&&(*p!=';')&&(*p!='"')){	//';'表示结束；
	      p=GetFC(p,&fc);						//fc=[%X@Y],p指向“%X@Y”后第一个字符；
	      if(IffcRepeated(fit,q,fc)==TRUE){			//跳过执行过的fc
	  	    if(*p=='&'){				//允许多个‘&’
	          p++; continue;			//p指向下一个“%X@Y”；
	        }

	      	if(*p=='?'){
		      if(fit->fSuccess==TRUE){return 0;}	//前一个通信成功，不执行备用信道
			  else{p++; continue;}
	        }
		  }
	    
		  channel=fc&0x0f;
		  if(sw_TimeFit(channel,fTFIT)==TRUE){	//判断channel_sw、时间条件
		    r=DoReport(fc);
			goto FC0_END;
	      }
		}
		return 0;		//结束	

FC0_END:
	    stepFC=1;
	    break;

	  case 1:
//		r=ComResp();
  	    if(r==SUCCESS){
	      q->fc=fc;			//新fc加入queue[]
	 	  //q->sec_stamp=GetSecStamp();
		  q->fSuccess=TRUE;
		  q++;
	  	  if(*p=='&'){		//允许多个‘&’, p指向下一个“%X@Y”；
		    p++; stepFC=0;
	        break;
		  }
		  else{			//=='?'或‘“’
			stepREP=2; 	//本函数结束，返回上层继续
			break;
		  }	
		}
	    else{	//失败
	  	  if(*p=='?'){		
		    p++; stepFC=0; //p指向下一个“%X@Y”,启动备用通道；
	        break;
		  }
		  else{	//无备用通道
			stepREP=2; 	//本函数结束，返回上层继续
			break;
		  }
	    }
	  default:
		break;
	}
	return p;		//指向未处理的下一个字符。
}
*/

//======================================
// 判断fc是否已经在执行过的队列中
// fit: 指向匹配的位置
// q:   当前的位置
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
// 获取“INTCij”
//==================================
void GetINTC(BYTE i,BYTE j,BYTE *str)
{
	*str++='I';
	*str++='N';
	*str++='T';
	*str++='C';
	*str++=i+'0';		//基元
	//*str++='.';
	*str++=j+'0';		//虚元
	*str++=0;
}

//===================================
//
// fc=[%X@Y],p指向“%X@Y”后第一个字符；
//===================================
BYTE *GetFC(BYTE *p,BYTE *fc)
{
	if((*p==';')||(*p=='"')){return 0;}

	if(*p=='#'){
	  *fc=(X(*(p+1))<<4)+(X(*(p+3))&0x0f);		//至多16种格式，16个通信任务
	  p+=4;
	}
	return p;
}

/*
//===================================
//
// 扩展字符集
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
// 找到与chan_id对应的通信结构，返回通信类型
//
// channel: 0-3
// [S LLLL TTT]
//=============================================
BYTE GetComType(BYTE *sComTask)
{
BYTE *p,*r;
BYTE lenStr;

	p=sComTask+9;		//指向'['后第一个字符
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
// 判断是否满足定时条件
//
// 返回：
// fit:	TRUE:  符合
//		FALSE：不符合
// p: 指向":GGDD"后
//======================================================
BYTE *TimeFit(BYTE *p,BYTE *fit)		//p 指向Gap&Delay，":GGDD"
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
	UnsoInterval=c&0x3f;	//间隔，屏蔽UU(单位)
	if(UnsoInterval==0){	//0：禁止主动报
	  *fit=FALSE; 
	  return p;
	}

	TimeUnit=c>>6;			//UU(单位)

	p+=2;					//指向“DD“
	c=ValueHex2(p);
	biaDelay=c&0x3f;
	p+=2;					//指向“DD“后

	*fit=FALSE;
	switch(TimeUnit){
	  case UNIT_HOUR:
        if(rTime.minute==0){					//整点报
		  if(((rTime.hour+UnsoInterval-biaDelay)%UnsoInterval)==0){
			*fit=TRUE;		//水文：确保8:00发送
		  }
	    }
	    break;

	  case UNIT_MINUTE:
        //if(rTime.second==0){					//整分钟报
	    if((rTime.minute+UnsoInterval-biaDelay)%UnsoInterval==0){			//整数倍
		  *fit=TRUE;							
	    }
	    break;

	  case UNIT_SECOND: 
	    if((rTime.second+UnsoInterval-biaDelay)%UnsoInterval==0){			//整数倍
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
//显示格式号：通信号F:XX C:XX
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
// 返回：
//     指针指向找到str的首地址'@'
//==========================================================
BYTE *GetComStr(BYTE comtsk_id,BYTE *cmfg)
{
BYTE *p;		//,*p0;
BYTE sn[5];
UINT lenCMFG;

	sn[0]='@';
	btoa(comtsk_id&0x0f,(char *)&sn[1]);			//最多16个com任务。

	p=CMC(CMC_LINK);
	p=FindParaStr(p,"CMFG");
	p++;

	lenCMFG=*(CMC(CMC_CMFG_LEN));			//长度限制
	p=InStr(p,(const char *)&sn[0],lenCMFG);		//寻找“@n”	
	if(p){
	  CopyParaStr(p,cmfg,'@');
	}
	return p;
}


//=======================================================
// GPRS通信
// cmfg:
// @1111-45[GPRS:ffffffffffff,ffffffff0000,ffffffffffff]
//=======================================================
BYTE DoGPRS(BYTE *buf,UINT lenMsg,BYTE *cmfg)
{
BYTE *p,lenStr;
BYTE sIp[16],sPort[6];		//"122.224.172.153:25005"
BYTE r;

	if(!Connect(CONNECT_GPRS)){
	  Scroll(WAY_CATCH,"...无响应");
	  return FAILED;
	}

//TCOM服务器ip
	lenStr=strlen((char *)cmfg)-8;
	p=cmfg+8;						//指向[GPRS
  	p=InStr(p,"[GPRS:",lenStr);		//寻找“[”
	if(!p){return FAILED;}
	p+=6;

//连接TCOM服务器
	if(getIp(&sIp[0],p)==FAILED){return FAILED;}
	if(getPort(&sPort[0],p+8)==FAILED){return FAILED;}
	r=ConnectTcpServer((BYTE *)&sIp[0],(BYTE *)&sPort[0]);	//"122.224.172.53",50051	

//发送
	if(r==GPRS_CONNECTION_OK){
	  GPRS_transmit(buf,lenMsg);
	  return OK;
	}
	else{
	  return FAILED;
	}
}


//=======================================================
// Wifi通信
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

//TCOM服务器ip
	lenStr=strlen((char *)cmfg)-8;	//8:'['之前字符数
	p=cmfg+8;						//指向[GPRS
  	p_start=InStr(p,"[WIFI:",lenStr);		//寻找“[”
	p_end=InStr(p,"]",lenStr);
	if(!p_start){return FAILED;}

  	p=InStr(p_start,"[PSW=",p_end-p_start);		//寻找“[”

//临时
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


//连接TCOM服务器
	ip_port=p_start+6;
	r=WifiConnection(ip_port,&psw[0]);		
	if(r==1){
//发送
	  GPRS_transmit(buf,lenMsg);
	  return OK;
	}
	else{return FAILED;}
}



//====================================================
// GPRM通信
// cmfg:
// @1111-45[GPRM:ffffffffffff,ffffffff0000,ffffffffffff]
//====================================================
BYTE DoGPRM(BYTE *buf,UINT lenMsg,BYTE *cmfg)
{
BYTE *p,lenStr;

	if(!Connect(CONNECT_GPRM)){return FAILED;}

//夭折前群发
	SendAT("AT+GPRSMASS=0",0x0d);	
	if(!IsStrInStream(0,"OK",WITHIN_1S*3)){		
	  return FAILED;
	}

//群发目标
	lenStr=strlen((char *)cmfg)-8;
	p=cmfg+8;						//指向[GPRM
  	p=InStr(p,"[GPRM:",lenStr);		//寻找“[”
	if(!p){return FAILED;}
	p+=6;
	
	//at+gprsgrou="ffffffffffff?ffffffff0000,ffffffffffff"，0x0d
	SendAT("at+gprsgrou=\"",0x0);
	while(*p!=']'){Serial_Transmit(*p++);}
	SendAT("\"",0xd);
	
	if(!IsStrInStream(0,"OK",WITHIN_1S*3)){		
	  return FAILED;
	}

//MSG设置
	SendAT("AT+GPRSMSG",0x0d);	
	if(!IsStrInStream(0,">",WITHIN_1S*3)){		//等待">"
	  return FAILED;
	}

//Msg内容
	while(lenMsg){
	  Serial_TransmitHL(*buf);
	  buf++; lenMsg--;
	}
	Serial_Transmit(ASC_CTRL_Z);

	if(!IsStrInStream(0,"OK",WITHIN_1S*3)){		
	  return FAILED;
	}

//启动群发
	SendAT("AT+GPRSMASS=1",0x0d);	
	if(!IsStrInStream(0,"OK",WITHIN_1S*3)){		
	  return FAILED;
	}

/*
	if(!IsStrInStream(0,"A",WITHIN_1S*30)){		//状态变化提醒
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
char s[6]; //存放ip中的1个，和端口。端口最大6 byte.
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
	src+=2;				//src:指向ip字符串后--> ',','?',']'
	sprintf(&s[0],"%u",u1);

	strcat(des,&s[0]);  
	return src;
}
*/

/*
void GetIpGroup(char *des,BYTE *src)
{
char ip_port[22];	//单个ip：aaa.bbb.ccc.ddd:ppppp+分隔符,最大长度：22
char sep[2];

	*des=0;
	while(1){
	  src=ConvertIpStr(&ip_port[0],src);
	  strcat(des,(const char *)&ip_port[0]);
	  if(*src==']'){break;}

	  sep[0]=*src;
	  sep[1]=0;
	  strcat(des,&sep[0]);				//分隔符
	  src++;
	}
}
*/


//====================================================
// BEIDOU通信
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

	//清除发送请求标志
	//ClrCfgBit(REG0+((UINT)(MAIN_PROPERTY2)),bitSENSOR_REPORT_REQUEST);
	fSensorRepRequest=0;

	if(r==OK){
	  Scroll(WAY_CATCH,"...成功");
	}
	else{
	  Scroll(WAY_CATCH,"...失败");
	}
	Delay_s(3);
//	  goto END_REPORT;

	  //自发自收测试，为了便于诊断，等待10s。
//	  Delay_s(10);
//	  Delay_s(60);
//	  Delay_s(30);
//	  BEIDOU_TimeAutoAdjust();		//如时间校准，设备重启

	return r;
}


//====================================================
// DIRECT通信
// 
// cmfg: @211DIRE[]
//====================================================
BYTE DoDIRECT(BYTE *buf,UINT lenMsg,BYTE *cmfg)
{
BYTE r;
//BYTE i=0;

	RS232_on();
 	Serial_Setup(BAUD_19200);

//中断方式
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
// Ultrashort wave通信
// 
// cmfg: @211DIRE[]
//====================================================
BYTE DoUsWave(BYTE *buf,UINT lenMsg,BYTE *cmfg)
{
BYTE r;

	RS232_on();
 	Serial_Setup(BAUD_300);

//中断方式
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

//检查Modem状态
	if((IsModemReady()==FALSE)){
	  AT_creg(ActiveCom);		//马上刷新，保证时钟停振时，也能响应	
	  if(IsModemReady()==FALSE){
	    Scroll(WAY_CATCH,"...失败 Modem not ready");
		return 0;
	  }
	}	


//SM群发
	cmfg+=9;	//指向通讯录tels

	SendAT("AT+SMGROU=",0);	//SM MASS设置
	TransmitStr(cmfg);
	Serial_Transmit('"');
	Serial_Transmit(0x0d);

	SendAT("AT+SMSG=",0);	//SM设置
	while(lenMsg){
	  Serial_TransmitHL(*buf);		
	  buf++; lenMsg--;
	}
	Serial_Transmit('"');

	SendAT("AT+SMMASS=1",0x0d);	//启动群发
	return 1;
}

//===============================================
//        			Reporting
//
// 1、按%X@Y通信；
// 2、仅针对1个信道，可复合通信（条件发送、群发等）
//
// 返回：
//    0: 失败；
//    1: 成功
//    0xff: 重入（未完成）
//===============================================    
BYTE DoReport(BYTE fc,UINT property)		
{    
BYTE r,sw;
BYTE typeComm,COMi;
BYTE fmt_id,comtsk_id,prot_id;
BYTE msg[500];
UINT lenMsg;
BYTE cmfg[128],*p;		//单个通信任务CMFG最长 <= 128
PROPERTY pp;


	FillBufWith(&msg[0],500,0x3f);	//空缺数据：0x3F[01 111111]

	pp.ALLbits=property;

	switch(stepDO_REPORT){
  	  case 0:
	  	fmt_id=fc>>4;
		comtsk_id=fc&0x0f;

		p=GetComStr(comtsk_id,&cmfg[0]);	//获取@n通信任务
		if(p==0){return 0;}

		sw=X(cmfg[2]);			//2：任务开关sw		
		if(sw==0){return 1;}	//开关

		prot_id=X(cmfg[4]);		//4:协议编号

		lenMsg=MakeMsg(prot_id,fmt_id,&msg[0],property);		//需要时才形成

//======================================
		r=FAILED;
		typeComm=GetComType(&cmfg[0]);

//显示格式号：通信号  ==> F:XX C:XX
//		DspFC(fc);		

		COMi=X(cmfg[3])+1;		//3:指向通道channel

/*	case 1:
		if(ComOpen[COMi-1]){
		  return 0xff;				//等待COM口释放
		}							//重入！！！			*/
		
//临时
//COMi=2;
//typeComm=CONNECT_WIFI;

		ActiveCom=COMi;
    	switch(typeComm){

//==============DIRECT通信==============	
		  case CONNECT_DIRECT:
	  	    Scroll(WAY_CRLF," >发送-DIRECT");
	  	    r=DoDIRECT(&msg[0],lenMsg,&cmfg[0]);
	  	    break;

//==============SM通信==================
    	  case CONNECT_SM:
	  	    Scroll(WAY_CRLF," >发送-SM");
	  	    r=DoSM(&msg[0],lenMsg,&cmfg[0]);
	  	    break;

//==============GPRM通信(群发)==========
    	  case CONNECT_GPRM:
	  	    Scroll(WAY_CRLF," >发送(GPRM)");
	  	    r=DoGPRM(&msg[0],lenMsg,&cmfg[0]);
	  	    break;

//==============GPRS通信==========
    	  case CONNECT_GPRS:
	  	    Scroll(WAY_CRLF," >发送(GPRS)");
	  	    r=DoGPRS(&msg[0],lenMsg,&cmfg[0]);
//临时
//r=OK;
	  	    break;

//==============BEIDOU通信==============
    	  case CONNECT_BEIDOU:
	  	    Scroll(WAY_CRLF," >发送-BEIDOU");
	  	    r=DoBEIDOU(&msg[0],(BYTE)lenMsg,&cmfg[0]);
	  	    break;

//==============UltrashortWave通信==============
    	  case CONNECT_USWAVE:
	  	    Scroll(WAY_CRLF," >发送-USWAVE");
	  	    r=DoUsWave(&msg[0],(BYTE)lenMsg,&cmfg[0]);
	  	    break;

//==============Wifi通信==============
    	  case CONNECT_WIFI:
	  	    Scroll(WAY_CRLF," >发送-WIFI");
	  	    r=DoWifi(&msg[0],(BYTE)lenMsg,&cmfg[0]);
	  	    break;

		  default:break;
		}

		if(r==FAILED){		//发送过程中夭折
		  if(!r){Scroll(WAY_CATCH,"...失败");}
		  goto END_DO_REPORT;
		}
	
		if(!pp.bitANSWER_NEEDED){
		  r=OK;						//不需要Answer
		  goto END_DO_REPORT;
		} 

		stepDO_REPORT=2;
	  case 2:	
//		r=GetComResp(typeComm,COMi);
//临时
r=1;
		if(r==0xff){return 0xff;}			//重入！！！

END_DO_REPORT:
		DelayAfterComm(typeComm);
//		Delay_s(1);		//设定盲区，1s内的其他从Modem返回的信息不处理

		Tasking[ActiveTask]=0;	//关闭当前任务
		EndCom();
		return r;
	}
	
return 9;	//实际不可及
}

//===================================================================
//	为避免响应某些Modem在通信结束（如下电）时，
//  返回信号，设定一些盲区
//设定盲区，1s内的其他从Modem返回的信息不处理
//===================================================================
void DelayAfterComm(BYTE typeComm)
{
	switch(typeComm){
	case CONNECT_DIRECT:
	  break;				//直连，无盲区
	default:
	  Delay_s(1);
	  break;
	}
}

//==============================
// 获取DoReport的结果(含群发)
//
// 返回：
//		1：		成功
//		0：		失败
//		0xff:	等待
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
			//Stream[]可用
			while(group_id<Stream[]){
			  if(OUTPUT_ON){
				ShowComProgress(&Stream[0]);
				//显示: "GPRS发送x:x"
				strcpy(out,"GPRS发送");
				strcat(out,Stream[5]);
				strcat(out,":");
				strcat(out,Stream[6]);
				Scroll(out);
				
				//查询结果				
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
	  //成功
	}
	else{
	  //失败
	}

	r=OK;
	return r;
}

//===========================================
// 最多等待1s
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
	  if((p-rx)>=32){		//Enough bytes received(16字节,hex)
		r=CheckGprsResp(rx);
		break;
	  }
	  if(IsTimeUp(EndMs)==TRUE){r=FAILED; break;}
	}
	RX_IE_OFF();	
	return r;	  
}


//======================
//显示msg: 
//	"GPRS发送x:x"
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


