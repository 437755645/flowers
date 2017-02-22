#include 	"header.h"
#include    "h_macro.h"

extern UINT ms_click;
extern BYTE ActiveCom;

extern const char ROM_0D[];
extern const char ROM_LF[];

extern BYTE 	Step[];			//定义见：STEPS_


//==========================================
// BEIDOU通信
//
// 指令:
//		$TTCA,1,213758,1,0,10,1234567890,0
// cmfg:
//	@321BDID 327680
//
//==========================================
BYTE BEIDOU_Transmit(BYTE *buf,BYTE lenMsg,BYTE *cmfg)
{
BYTE j,r;
BYTE sC[5];
char BeidouID[7];
BYTE *p,*buf0;
BYTE msg[123];
BYTE *rx;

	RX_IE_OFF();
	buf0=buf;

//获取BeidouID
	p=InStr(cmfg,"BDID",20); 						
	if(p){
	  DataCpy((BYTE *)&BeidouID[0],p+4,6);
	  BeidouID[6]=0;
	}

//填充"头"[213759 中心目标]【青川测试：216989】
	strcpy((char *)&msg[0],"$TTCA,1,tttttt,1,0,98,");	//copy 22 chars.电文长度=98 
	DataCpy(&msg[8],(BYTE *)&BeidouID[0],6);		//号码移入目标

//加载buf
	DataCpy(&msg[BEIDOU_HEAD_LEN],buf,lenMsg);		
//
	p=buf0+120;
	*p++=',';					//120
	*p++=BEIDOU_Chk(buf0,121);	//121; 对p[0]-p[120]作北斗异或计算  
	*p++=0x0d;					//122
	*p=0x0a;					//123

//循环2次。
	for(j=0;j<2;j++){
	  lcd_puts(0,L2,"等待 [  ]               ");
	  sprintf((char *)&sC[0],"%d",j);
	  lcd_puts(6,L2,(const char *)&sC[0]);	

//	  BEIDOU_ok();
	  if(Is_BEIDOU_Ready()==FALSE)continue;

//
	  p=buf0;
	  TransmitBytes(p,124);

	  r=IsStrInStream(1,&BeidouID[0],WITHIN_1S*3);
	  if(r==TRUE){
		Delay_s(3);
		RX_IE_OFF(); 
		return TRUE;
	  }

//校验错误=》SM
//	  strcpy((char *)&Stream[0],"CASS,0");

	  rx=RX_BUF0();
	  if(InStr(rx,"CASS,0",20)){
//	  if(InStr(&Stream[0],"CASS,0",20)){
		SM_ReportError(buf0,124);		//SM发送错误信息到手机，已拆分发送。
		RX_IE_OFF();
		return FALSE;				
	  }
	  Delay_s(50);
	}
	RX_IE_OFF();
	return FALSE;				
}



//======================================
// 北斗异或计算：
//     对p开始，长度len的数组作异或计算
//======================================
BYTE BEIDOU_Chk(BYTE *p,BYTE len)
{
BYTE chk,i;

	chk=*p++;
    for(i=1;i<len;i++){
	  chk^=*p++;
	}
	return chk;
}

//=====================================
// 北斗自动校时
//=====================================
void BEIDOU_TimeAutoAdjust(void)
{
BYTE cal[16],fReset,r;

	MsgBox("等待10秒....");

	Delay_s(10);		//上电后，等待北斗猫稳定

	r=BEIDOU_GetTime(&cal[0]);
	if(r==FALSE){return;}		//校时失败

  	fReset=CalendarAutoAdjust(&cal[0]);
	if(fReset==TRUE){Restart(RST_FOR_MAIN_94);}		//校时完成,重启
}	//不需校时

//=============================
// YMDHM分别存入*(cal+8)
//=============================
BYTE BEIDOU_GetTime(BYTE *cal)
{
BYTE *p;
hTIME dt;
BYTE inst[10];
BYTE *buf;

//填充"头"
	strcpy((char *)&inst[0],"$TAPP,");	//copy 23 chars.电文长度=98

//	inst[i++]=BEIDOU_Chk(&inst[0],6);		//121; 对p[0]-p[6]作北斗异或计算  

	inst[6]=0x1d;		//固定值
	inst[7]=0x0d;		//122
	inst[8]=0x0a;		//123
//
	p=&inst[0];
	TransmitBytes(p,9);

//时间返回特征:"$CASS,1,0x17,0x0D,0x0A,$TINF,18:51:17.43,0x13,0x0D,0x0A"
//                                                   |->秒百分位可作为结束判断
	if(IsStrInStream(1,".",WITHIN_1S*1)==FALSE){return FALSE;}	
// 	Delay_ms(100);	//等待所有数据

	*(cal+7)=OP_TIMESTAMP;		//校时指令
//
	Now(&dt);
//
	cal+=8;
	*cal++=dt.year;
	*cal++=dt.month;
	*cal++=dt.day;
//
	buf=RX_BUF0();
	p=InStr(buf,"TINF",0x20);
//	p=InStr(&Stream[0],"TINF",0x20);
	if(p){		//Wait
	  *(p+7)=0; 
	  *cal++=(BYTE)atoi((const char *)(p+5));		//hour,Stream[17:18]
	  *(p+10)=0;
	  *cal++=(BYTE)atoi((const char *)(p+8));		//hour,Stream[17:18]
	  *(p+13)=0;
	  *cal++=(BYTE)atoi((const char *)(p+11));		//hour,Stream[17:18]

	  return TRUE;
	}
	return FALSE;
}


BYTE Is_BEIDOU_Ready(void)
{
BYTE i;
UINT BEIDOU_Limit;

	BEIDOU_Limit=ms_click+30000;		//30's 

	for(i=0;i<10;i++){ 
	  Delay_s(2);     //*Delay_s(2)
	  if(BEIDOU_ok()==FALSE){
	    i=0;					//连续10次ok,判定为稳定的ok;
	    if(IsTimeUp(BEIDOU_Limit)==TRUE){
	      return FALSE;
	    }
	  }
	}
	return TRUE;
}


BYTE BEIDOU_ok(void)
{
BYTE sta[0x40];
BYTE pw_level,r;
BYTE com_save;
BYTE *buf;

	com_save=ActiveCom;
	ActiveCom=COM3;
//
	SendAT("$QSTA,0,",0);
	Serial_Transmit(0x03);
	Serial_Transmit(0x0d);
	Serial_Transmit(0x0a);

	r=FALSE;
	buf=RX_BUF0();
	if(IsStrInStream(1,"TSTA,",WITHIN_1S*2)==TRUE){
	  if(IsStrInStream(1,&ROM_0D[0],WITHIN_1S)==TRUE){
	    sta[0]=*buf;		//channel 1 level
	    sta[1]=*(buf+2);	    //channel 2 level
	    sta[2]=*(buf+4);		//channel 1 beam
	    sta[3]=*(buf+6);		//channel 2 beam
	    sta[4]=*(buf+8);		//locked beam
	    sta[5]=*(buf+10);
	    sta[6]=*(buf+12);	
	  	//$TSTA,4,2,2,3,2,0,1,216989,1930610,60,3,1,0,....
	    if(sta[4]==sta[2]){pw_level=sta[0];}
	    else if(sta[4]==sta[3]){pw_level=sta[1];}
	    else{pw_level=0;}

	    if(pw_level>=3){r=TRUE;}
	  }
	}

	ActiveCom=com_save;
	return r;
}


//================================	
//            BDQ 
//
// 前提：北斗模块在channel
//================================
UINT getBDQ(BYTE channel)
{
UINT uBDQ;
BYTE save;
//BYTE *buf;

	save=ActiveCom;

	ActiveCom=channel;
	if(IsComOpen()){
	  uBDQ=0xffff;				//COM口被占用
	}
	else{
	  RX_IE_OFF();				//阻止返回信号触发RING_ECHO
	  SLEEP1_DIR=OUT; SLEEP1=1;	//RS232 wake up
	  Serial_Setup(BAUD_19200);

	  SendAT("$QSTA,0,",0);
	  Serial_Transmit(0x03);
	  Serial_Transmit(0x0d);
	  Serial_Transmit(0x0a);

	  uBDQ=0;
//buf-RX_BUF0();
	  if(IsStrInStream(1,"TST",WITHIN_1S)==TRUE){
	    if(IsStrInStream(1,&ROM_0D[0],WITHIN_1S)==TRUE){
//		  uBDQ=BDQ_to_UINT(buf);
//		  uBDQ=BDQ_to_UINT(&Stream[0]);
	    }  
	  }

	  EndCom();
//	  CloseComPort();
// 	  CLR_RX_IF();	//清除通信期间留下的中断标志
//	  RX_IE_ON();		//开放RING_ECHO
	}

	ActiveCom=save;
	return uBDQ;
}

UINT BDQ_to_UINT(BYTE *bdq)
{
UINT uBDQ;

	uBDQ=1;
	return uBDQ;
}

