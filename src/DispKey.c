#include	"header.h"
#include    "h_macro.h"
#include 	<string.h>


//==============================================================
//不显示“_”
//#define		BlinkOff()				Blink(BLINK_GAP)	//取消光标闪动
#define		ARROW_NOT_EXIST			0xff

//SCREEN_MENU
#define		LINE_MANUAL_REG		0
//#define		LINE_MANUAL_REG		0
//#define		LINE_MANUAL_REG		0
//#define		LINE_MANUAL_REG		0


//OK按钮的位置 
#define		X_OK			9		//指向 ">确定"
#define		Y_OK			3		//第三行
#define		X_CANCEL		17		//指向 ">放弃"
#define		Y_CANCEL		3		//第三行


//==============================================================
extern BYTE 	Step[];			//定义见：STEPS_

extern BYTE		Tasking[];		//0:未启动； 1：运行中
extern MSG0		Msg0;
extern MSG2		Msg2;

extern BYTE		fBlinkEnabled;
extern BYTE		KeyPollEnabled;
extern const 	char 	sVer[];	
extern volatile KEY 	key;

extern SW		sw1;
extern BYTE 	IPL_CPU;
extern BYTE		fLcdReady;
extern BYTE 	ActiveCom;

extern BYTE 	c0Table[],cxTable[];

extern BYTE 	cfg[];
extern PORT		Port[];
extern BYTE 	KeyBlindCount;

//================================
//Bit usage variables
//================================
extern MSG0			Msg0;
extern MSG1			Msg1;
extern ERRORS		Error0;
extern TIMERS		Tmr;
extern TIMEUP 		TmrUp;


const char ROM_0D[]={0x0d,0x00};


extern BYTE		ScrollIndex;	//SCREEN_STATUS: 滚动显示，自动+1

extern SCREEN Srn[];
SCREEN *ActiveSrn;

#ifdef  STOP_WATCH_ON
extern UINT 	stopwatch[];			//stopwatch
extern UINT 	ms_click;
#endif


void CursorBlink(void)
{
BYTE cChar;
BYTE c[2],x,y;

	if(fBlinkEnabled==FALSE){return;}

	x=ActiveSrn->CurX;
	y=ActiveSrn->CurY;
	if(fCursorToggle){
	  fCursorToggle=0;

	  cChar=ActiveSrn->CurChar;
	  if(cChar=='_'){
		c[0]=ActiveSrn->bufLCD[y][x];		
		c[1]='\0';
	    lcd_puts(x,y,(const char *)&c[0]);
		ActiveSrn->CurChar=c[0];
	  }
	  else{
		lcd_puts(x,y,"_");
		ActiveSrn->CurChar='_';
	  }
	}
}

void BlinkOff(void)
{
BYTE x,y;
BYTE c[2];

	x=ActiveSrn->CurX;
	y=ActiveSrn->CurY;
	c[0]=ActiveSrn->bufLCD[y][x];
	c[1]='\0';
	lcd_puts(x,y,(const char *)&c[0]);
}

//============================
//   		LCD显示
//
// COM4 
//============================
void InitDsp(void)
{
#ifdef SIM_ON
	return;
#endif

	fLCD_PW_READY=FALSE;
	PW_LCD_on();

	ScrollIndex=0;				//滚动显示，初始第一行
//	fDspWaiting=FALSE;

	//显示首页?
	fHomePageChoice=FALSE;	//不显示首页
	if(cfg[CM_YN1]&0x08){
	  fHomePageChoice=TRUE;	//显示首页
	}
}


void StartKeyAD(void)
{
	KeyPollEnabled=TRUE;
	initAdcKey();
 	initTmr3();
	_ADON=1;
	AD_IE=1;
}

void EndKeyPoll(void)
{
	KeyPollEnabled=FALSE;
	AD_IE=0;
	_ADON=0;		//shut off A/D	
	T3_ON=0;
}


//=========================================
// 根据Key，修改ActiveSrn
//
// 键盘:
//     检测是否有键按下。
//
// 定时中断: 启动键值AD；
// AD完成中断：按键判断
//=========================================
void KeyProc(void)
{
BYTE k;

	k=getKey();
	if(k){				//KEY1-4,KEY_NULL
	  if(k==KEY_NULL){
		fKeyReleased=TRUE;
	  }
	  else{				//KEY1-4
		if(fKeyReleased==FALSE){return;}
	    blight_on();
	    ActiveSrn=DoKeyFunc(ActiveSrn,k);
		fKeyReleased=FALSE;
	  }
	}
}

BYTE getKey(void)
{
BYTE k;

	k=KEY_UNKNOW;		//NULL;
	if(key.count>=KEY_CONFIRM){
	  k=key.val;
	  //
	  key.val=KEY_UNKNOW;	//NULL;
	  key.count=0;
	}
	return k;
}


//======================================
// 初始化：CurX,CurY
// 返回：
//		0-未找到‘>’
//		p-找到
//======================================
BYTE InitCurXY(SCREEN *iSrn)
{
BYTE i;

	for(i=0;i<iSrn->endL;i++){
	  if(FindRight(iSrn)){
		return 1;
	  }
	}
	iSrn->CurX=ARROW_NOT_EXIST;		//“无光标”标记
	iSrn->CurY=ARROW_NOT_EXIST;
	return 0;
}


//=======================================
// 刷新Cx显示缓冲
//
// 在以下几种情形下调用：
// 1、要素变化时，如雨量；
// 2、定时刷新，如水位；
//
// iSet:
//		0: 第1屏(cx)
//		1: 第2屏(cx)
//		2: 第3屏(status)
//		3: 第4屏(menu)

//		10: ID,PSW
//=======================================
void Write_DspBuf(BYTE iSet,SCREEN *iSrn)
{
BYTE *p,*pf;		//指向：fmt

	pf=FindParaStr(CMX(CMX_LINK),"DISP");
	if(pf==0){return;}
  	pf+=1;	

	//指向第一组
	switch(iSet){
	case SCREEN_CX:	//第1屏(cx)			
	  //===========================
	  //" >时雨量          12.5 >"
	  //" >日雨量          12.5 >"
	  //" >时水位         24.55 >"
	  //" >当前水位       23.26  "
	  //===========================
	  Refresh_bufLCD_Cx(iSrn,pf);		//刷新buf
	  iSrn->curL=0;					
	  InitCurXY(iSrn);
	  break;

	case SCREEN_SYS:	//第2屏(cx)
	  while(1){
	    pf++;
		if(*pf=='"'){return;}	//没有定义“第二组”
		if(*pf==','){
		  pf++;
		  break;
		}
	  }
	  Refresh_bufLCD_Cx(iSrn,pf);		//刷新buf
	  iSrn->curL=0;					
	  InitCurXY(iSrn);
	  break;

	case SCREEN_STATUS:	//第3屏(status)
	  p=&iSrn->bufLCD[0][0];
//	  if(fPowerOnReset){
	    FillBufWith(p,sizeof(SCREEN),'\0');	//结构置'\0'

	  	*(p+1)='>';							//预置'>'
	    iSrn->curL=0;					
	    iSrn->endL=LINES_LCD_BUF-1;
	    iSrn->CurX=1;						//固定的'>'位置
	    iSrn->CurY=0;						//固定的'>'位置
//	  }  
//InitCurXY(iSrn);
	  break;

	case SCREEN_MENU:	//第4屏(menu)
	  p=&iSrn->bufLCD[0][0]; strcpy((char *)p,(const char *)" >注册                  ");
	  p=&iSrn->bufLCD[1][0]; strcpy((char *)p,(const char *)" >状态                  ");
	  p=&iSrn->bufLCD[2][0]; strcpy((char *)p,(const char *)" >工具                  ");
	  p=&iSrn->bufLCD[3][0]; strcpy((char *)p,(const char *)" >维护(1小时)           ");
	  iSrn->curL=0;				
	  iSrn->endL=3;		//LINES_LCD_BUF-1;	
	  iSrn->CurX=0;
	  iSrn->CurY=0;
	  InitCurXY(iSrn);
	  break;

	case SCREEN_PSW:
	  p=&iSrn->bufLCD[0][0]; strcpy((char *)p,(const char *)"  密码 [******]         ");
	  p=&iSrn->bufLCD[1][0]; strcpy((char *)p,(const char *)"                        ");
	  p=&iSrn->bufLCD[2][0]; strcpy((char *)p,(const char *)"                        ");
	  p=&iSrn->bufLCD[3][0]; strcpy((char *)p,(const char *)" 确定                   ");
	  iSrn->curL=0;
	  iSrn->endL=LINES_LCD_BUF-1;					
	  //iSrn->endL=3;
	  iSrn->CurX=0;
	  iSrn->CurY=0;
	  InitCurXY(iSrn);
	  break;
	}
}


//=======================================
// 输出显示缓冲 => LCD panel
//
// iScreen: 屏号；
// curL: 	当前行号；
//=======================================
void LCDShow(void)
{
BYTE i,*p,*p0;	
BYTE L,end,blink_save;
BYTE PL;		//窗口指针

	blink_save=fBlinkEnabled;
	fBlinkEnabled=FALSE;

	if(fLCD_PW_READY){PW_LCD_on();}

	blight_on();				//背光开启

//标记窗口在缓冲中的位置，".>"
	L=ActiveSrn->curL;	//窗口起始行
	if((ActiveSrn==&Srn[SCREEN_CX])||(ActiveSrn==&Srn[SCREEN_SYS])){		//屏：CX，SYS
	  p=&ActiveSrn->bufLCD[L][0];
	  if(L>0){*p='.';}		//加‘.’:窗口未到顶

	  p0=&ActiveSrn->bufLCD[L+3][0];
	  end=ActiveSrn->endL;
	  if((L+3)<end){*p0='.';}	//加‘.’:窗口未触底
	}
	/*else{
	  if(ActiveSrn==&Srn[2]){							//屏：STATUS，MENU
		i++;		//仅用于Debug
	  }
	}*/

//输出
	lcd_clear();
	for(i=0;i<LCD_HEIGHT;i++){
	  PL=L+i;					//窗口指针
	  if(PL>=LINES_LCD_BUF){
		PL-=LINES_LCD_BUF;
	  }
	  p=&ActiveSrn->bufLCD[PL][0];

	  *(p+LCD_WIDTH)='\0';
	  lcd_puts(0,i,(const char *)p);		//主要耗时:30ms
	}

//光标Blink
	//Delay_ms(100);
	fBlinkEnabled=blink_save;
}


//======================================
//	在活跃iScreen缓冲中：
//  1、在本行，向Right寻找下一个‘>’或'[';
//  2、如到行尾，循环
//======================================
BYTE FindRight(SCREEN *iSrn)
{
BYTE *p;	
BYTE x,y;
BYTE s[25],line[25];

	x=iSrn->CurX;
	y=iSrn->CurY;

//x为起始点的字符串
	p=&iSrn->bufLCD[y][0];
//	if(x){	
	  memcpy(&s[0],(const char *)p,x+1); s[x+1]=0;		//前半字符串
	  strcpy((char *)&line[0],(const char *)(p+x+1)); 	//后半字符串
	  strcat((char *)&line[0],(const char *)&s[0]);		//合并
//	}
//	else{
//	  strcpy((char *)&line[0],(const char *)p);
//	}

	p=InStr(&line[0],">",LCD_WIDTH);
	if(p){
	  x+=p-&line[0]+1;			//在一行内移动，y不变
	  if(x>=24){x-=24;}
	  iSrn->CurX=x;

	  x=iSrn->CurX;
	  iSrn->CurChar=iSrn->bufLCD[y][x];
	  return 1;
	}
	else{
	  p=InStr(&line[0],"[",LCD_WIDTH);
	  if(p){
		x+=p-&line[0]+1;
		iSrn->CurX=x+1;		//寻找'['时，指向后一个字符（可编辑）

		x=iSrn->CurX;		//在一行内移动，y不变
		iSrn->CurChar=iSrn->bufLCD[y][x];
		return 1;
	  }
	}
	return 0;
}


//========================================================
//	在活跃iScreen缓冲中：
//  1、下一行中，寻找第一个‘>’或'[';
//  2、如到底部，回到起始点，循环（因为取消了“向上”按键）
//  
//  每一行(col=1)显示必须有'>'
//========================================================
BYTE FindDown(SCREEN *iSrn)
{
BYTE *p;		//,*sLine,*sLine0;
BYTE x,y,L;

	x=iSrn->CurX;
	y=iSrn->CurY;
	L=iSrn->curL;
	p=&iSrn->bufLCD[y][x];
	while(1){
//curL移动
	  if((y+1)>=LCD_HEIGHT){
	    if((L+LCD_HEIGHT)<=iSrn->endL){
		  iSrn->curL++;			//窗口移动
	    }
		else{
		  iSrn->curL=0;		
		}
	  }

//光标移动
	  if(++y>iSrn->endL){y=0;}
	  //iSrn->CurX--;
	  iSrn->CurX=0;
	  iSrn->CurY=y;
	  if(FindRight(iSrn)){
	    return 1;
	  }
	}
	return 1;
}




//================================================
// View  模式：寻找'>';
// Editor模式：寻找'[', 并++;
//================================================
SCREEN *DoKeyFunc(SCREEN *iSrn,BYTE key)	
{
BYTE x,y;

	switch(key){
	  case KEY1:		//向下寻找下一个‘>’
		if(iSrn->CurX!=ARROW_NOT_EXIST){
		  BlinkOff();			//取消光标闪动
		  FindDown(iSrn);
		}		
		break;

	  case KEY2:		//向右寻找下一个‘>’
		if(iSrn->CurX!=ARROW_NOT_EXIST){
		  BlinkOff();			//取消光标闪动
		  FindRight(iSrn);
		}	
		break;

	  case KEY3:		//标记选中的功能Func
		x=iSrn->CurX;
		y=iSrn->CurY;
		MarkFunc(iSrn,x,y);
		break;

	  case KEY4:		//魔法键：
		if(iSrn<&Srn[3]){iSrn++;}
		else{iSrn=&Srn[0];}
		fSrnRefresh=TRUE;
		break;
	}
	return iSrn;
}



//============================================
// Get a YYYY-MM-DD HH:MM:SS,0x0 string 
//
// Return: 
// When display record:  No [:SS]
// When display current value: No [YYYY-] 
//============================================
void GetCurDateTime(char *fs)
{
hTIME dt;
UINT ipl_save;

	di(ipl_save,7);			//SET_CPU_IPL(7)
	Now(&dt);
	ei(ipl_save);			//SET_CPU_IPL(0)
	FormatDateTime(0,6,dt,fs);
}



//=====================================
// format dt---> fs (formated string)
// 2009.03.24  10:09:12
// 
// Offset: 起始字节，0表示从Year开始；
// Len：表示长度
// Offset=0，Len=6：表示YMDHMS
// Offset=3，Len=3：表示HMS
//=====================================
void FormatDateTime(BYTE Offset,BYTE Len,hTIME dt,char *fs)
{
int iY;

	switch(Offset){
	case 0:
	  iY=((int)dt.year)+2000;		//2000;			
	  sprintf(fs,"%04d",iY);
	  fs++; fs++; fs++; fs++;
	  *fs++='-';
	  if(!(--Len)){break;}
//Month
	case 1:
	  sprintf(fs,"%02d",dt.month);
	  fs++; fs++;
	  *fs++='-';
	  if(!(--Len)){break;}

//Day
	case 2:
	  sprintf(fs,"%02d",dt.day);
	  fs++; fs++;
	  *fs++=' ';
	  if(!(--Len)){break;}
	
//Hour
	case 3:
	  sprintf(fs,"%02d",dt.hour);
	  fs++; fs++;
	  *fs++=':';
	  if(!(--Len)){break;}

//Minute
	case 4:
	  sprintf(fs,"%02d",dt.minute);
	  fs++; fs++;
	  *fs++=':';
	  if(!(--Len)){break;}

//Second
	case 5:
	  sprintf(fs,"%02d",dt.second);
	  fs++; fs++;
	  *fs=0x0;		//string end
	  //if(!(--Len)){break;}
	}
}


//=============================================
// 日期转字符串
//=============================================
void DT2Str(BYTE *sOutput,hTIME *dt,BYTE *cx)
{
BYTE tmp,Offset,Len;
	
	tmp=(*(cx+CX_DSP_FMT))&0x3f;
	Offset=tmp>>3;
	Len=tmp&0x07;

	FormatDateTime(Offset,Len,*dt,(char *)sOutput);
}


//=============================================
//		//[L H]=>a,b
//
// 用于CSQ，BCD等显示
//=============================================
void Int2DoubleStr(char *sOutput,UINT *uInput)
{
char tmp[10];  	//最大：“256,256”

	*sOutput='\0';
	btoa(LoOfInt(*uInput),&tmp[0]);
	strcat(sOutput,&tmp[0]);
	strcat(sOutput,",");
	btoa(HiOfInt(*uInput),&tmp[0]);
	strcat(sOutput,&tmp[0]);
}

//============================================
// 显示无效数据：ERRxx
// *sOutput: 需要足够空间，至少“ERRxx”
//============================================
void OutputStatus(BYTE *sOutput,BYTE *Input)
{
BYTE err,code;
char sCode[3];

	*sOutput='\0';

	err=*Input&0xC0;
	code=*Input&0x3f;	
	if(err==0x40){		//无效数据
	  strcpy((char *)sOutput,"ERR");
	  sprintf(&sCode[0],"%d",code);	//最大63
	  strcat((char *)sOutput,&sCode[0]);
	}
}

void ClrStr(BYTE *s)
{
	*s='\0';
}

//=============================================
// 			显示(输出)sInput中虚元
//
// 返回：sOutput
//=============================================
BYTE Seg2Str(BYTE *Input,BYTE *sOutput,BYTE *cx)
{
BYTE r,DspFmt;
float f;

	if((*Input)&0xC0){
	  OutputStatus(sOutput,Input);		//输出“错误状态ERRx”等
	  return OK;
	}

	DspFmt=*(cx+CX_DSP_FMT);

//日期[11 XXX LLL]
	if((DspFmt&0xC0)==0xC0){
	  DT2Str(sOutput,(hTIME *)(Input+1),cx);	  
	}

//HEX格式[1000 0011]
	else if(DspFmt==0x83){
	  ClrStr(sOutput);			//输出""
	}

//双数格式[1000 0100]
	else if(DspFmt==0x84){
	  Int2DoubleStr((char *)sOutput,(UINT *)Input);		//[L H]=>a,b
	}

//STR格式[1000 0101]
	else if(DspFmt==0x85){
	  ClrStr(sOutput);			//输出""
	}


//正整数[1000 0001] / 浮点数[0aaa bbbb]
	else{
	  r=Seg2Float(&f,Input,cx);		
	  if(r==FAILED){return FAILED;}						

	  //float -> string.
	  FloatToStr((char *)sOutput,f,DspFmt);
      //return sOutput;
	}
	return OK;		
}



void Waiting_CSQ(void)
{
}

void Waiting_BDQ(void)
{
}


//==========================================================
// Fill spaces ' ' at end of string to expand to 'width' of LCD
// e.g: "123" to "123  " 
// in some case to overwrite the wrong char's in LCD
//==========================================================
void FillSpace(BYTE *str,BYTE width)
{
char i,len;
	len=strlen((const char *)str);
	if(len<width){		//宽度不足，用' '补足
	  str+=len;
	  for(i=len;i<width;i++){
	    *str=' ';
	    str++;
	  }
	  *str=0;
	}
	else{				//超过width,截断
	  *(str+width)='\0';
	}
}

//============================================================
// 					按格式描述刷新缓冲
// 1.最多4行虚元
//
// pf: 指向格式
//       !
// DISP" !
//       !       
//	     1312151617,		//屏1“日雨量累计”、“时雨量”、“当前水位”，“时水位”，“日水位”
//	     000102				//屏2“电压、环境温度、CSQ
//	“
//
// iSet:
//		0: 第一屏
//		1: 第二屏
//
// 返回：缓冲行数
//============================================================
BYTE Refresh_bufLCD_Cx(SCREEN *iSrn,BYTE *pf)		//pf: p for fmt
{
BYTE iC0,iCx;
BYTE iLine;	
BYTE r;		
BYTE *cx;	//表示：cx_start
BYTE *seg;
BYTE *p;		
BYTE iBuf;		//缓冲行数
BYTE vSeg[20],sVal[20];	//数值
BYTE sCxName[20];	//虚元名称
char line[25];
BYTE len;

	iBuf=0;
	iLine=0;	
	while(1){
	  iC0=X(*pf++);
	  iCx=X(*pf++);
	  r=ReadPort(iC0,&Port[iC0].buf[0],MODE_NORMAL);	//belt_limit=64，主要耗时
	  cx=&cfg[getCX(iC0,iCx)];

	  //显示'>'(第2列)
	  line[0]='\0';
	  strcat(&line[0],(const char *)" >");

	  //显示虚元名称
	  memcpy(&sCxName[0],cx+CX_NAME,16);
	  strcat(&line[0],(const char *)&sCxName[0]);

	  //显示数值[ >XXXXXXXX      1234.56789 >]
	  seg=&Port[iC0].buf[0]+*(cx+CX_BELT_OFFSET);
	  //RestoreBELT(&vSeg[0],seg,cx);
	  FillBufWith(&sVal[0],20,'\0');
	  Seg2Str(&vSeg[0],&sVal[0],cx);

	  //数值右对齐
	  len=strlen((char *)&sVal[0]);
	  FillSpace((BYTE *)&line[0],LCD_WIDTH-(len+2));		//虚元名称扩展，填充' '
	  strcat(&line[0],(const char *)&sVal[0]);

	  //显示' >'(尾列)
	  line[LCD_WIDTH-2]=' ';
	  if(IsCxSDRecorded(cx)){		//SD记录cx，可查询历史记录
	  	line[LCD_WIDTH-1]='>';
	  }
	  else{line[LCD_WIDTH-1]=' ';}
		
	  //结束符
	  line[LCD_WIDTH]='\0';

	  p=&iSrn->bufLCD[iBuf][0];
	  strcpy((char *)p,&line[0]);

	  iSrn->ic0=iC0;
	  iSrn->icx=iCx;
	  
	  iBuf++;

	  if((*pf==',')||(*pf=='"')){break;}
	}

	iSrn->endL=iBuf-1;

	return 1;
}


//==============================================
//
//       SCREEN_STATUS滚动显示
//
// 为统一使用LCDShow(),缓冲应反向填写
//
// way: 
//		0: WAY_CRLF  	回车;
//		1: WAY_CATCH  	接连(不换行);
//		2: WAY_OVERLAP	覆盖(不换行)
//==============================================
void Scroll(BYTE way,const char *msg)
{
BYTE *p;
BYTE len;
char buf[64]={0};

	if(!swLCD){return;}

	switch(way){
	case WAY_CRLF:		//换行
//	  if(way==WAY_CRLF){
	  if(ScrollIndex==0){ScrollIndex=LINES_LCD_BUF-1;}	//计算下一个ScrollIndex
	  else{ScrollIndex--;}
//	  }

	case WAY_OVERLAP:	//覆盖
	  //msg => bufLCD
	  len=strlen(msg);
	  if(len>(LCD_WIDTH-2)){len=LCD_WIDTH-2;}		//限制在1行内; -2:预留空格位置

	  p=&Srn[SCREEN_STATUS].bufLCD[ScrollIndex][0];
	  //*p++=' ';				//预留一个空格，用于光标
	  //*p++=' ';				//预留一个空格，用于光标
	  memcpy(p,msg,len);
	  break;

	case WAY_CATCH:
/*	  strcat(&buf[0],(const char *)p);
	  strcat(&buf[0],msg);
	  len=strlen(&buf[0]);
	  if(len>(LCD_WIDTH-2)){len=LCD_WIDTH-2;}		//限制在1行内; -2:预留空格位置

	  p=&Srn[SCREEN_STATUS].bufLCD[ScrollIndex][0];
	  *p++=' ';				//预留一个空格，用于光标
	  *p++=' ';				//预留一个空格，用于光标
	  memcpy(p,&buf[0],len);
	  *(p+len)='\0';
*/
	  p=&Srn[SCREEN_STATUS].bufLCD[ScrollIndex][0];
	  strcpy(&buf[0],(const char *)p);		//复制已有字符串
	  strcat(&buf[0],msg);
	  buf[LCD_WIDTH]='\0';		//限制字符串长度

	  len=strlen(&buf[0]);
	  memcpy(p,&buf[0],len);
	  break;
	}

	*(p+len)='\0';

//for DEBUG
	if(ScrollIndex>=8){
	  len++;
	}

	if(Tasking[TASK_LCD_DISP]){
	  fSrnRefresh=TRUE;				//无显示时，不刷新fSrnRefresh
	}
	  //curL指向最新行
	Srn[SCREEN_STATUS].curL=ScrollIndex;
}


void DoEditOK(void)
{}

void DoEditCancel(void)
{}



//==========================================
//           光标处数字TOGGLE
//
//         _ 0 1 2 3 4 5 6 7 8 9
//           |_________________| 
//==========================================
void DigitToggle(SCREEN *iSrn)
{
BYTE c,x,y;

	x=iSrn->CurX;
	y=iSrn->CurY;
	c=iSrn->bufLCD[y][x];

	if(c==' '){c='0';}
	else{
	  if(c<'9'){c++;}
	  else{c='0';}
	}
}


void DoEditFunc(SCREEN *iSrn)
{
BYTE c,x,y;

	x=iSrn->CurX;
	y=iSrn->CurY;
	c=iSrn->bufLCD[y][x];

	if((c==' ')||((c>='0')&&(c<='9'))){
	  DigitToggle(iSrn);
	}
	else{
	  if((x==X_OK)&&(y==Y_OK)){		//中文
	    DoEditOK();
	  }
	  else if((x==X_CANCEL)&&(y==Y_CANCEL)){
		DoEditCancel();
	  }
	}
}




//==========================================
// 标记对应的func，在主循环执行
//==========================================
void MarkFunc(SCREEN *iSrn,BYTE x,BYTE y)
{
//MENU界面
	if(iSrn==&Srn[SCREEN_MENU]){
	  if(y==0){						//第一行：注册
		Step[STEPS_PSW_EDIT]=1;
		Tasking[TASK_PSW_EDIT]=1;
		//Editor(SCREEN_MENU);
	  }
	}

//PSW界面
	else if(iSrn==&Srn[SCREEN_PSW]){
	  if((y==3)&&(x==2)){			//【确定】
		Step[STEPS_PSW_EDIT]=3;
	  }
	  else if((y==3)&&(x==2)){			//【放弃】
		Step[STEPS_PSW_EDIT]=4;
	  }
	}
}




//====================================
// 含编辑输入的界面
//
// 在主程序LOOP运行
//====================================
void Editor(BYTE iEScreen)
{
static SCREEN bufEdit;

	switch(Step[STEPS_PSW_EDIT]){
	  case 1:	//显示界面
		ActiveSrn=&bufEdit;
		Write_DspBuf(iEScreen,ActiveSrn);
		fSrnRefresh=TRUE;
		Step[STEPS_PSW_EDIT]=2;
		break;

	  case 2:	//等待,无按键时，由LOOP定时关闭
		break;

	  case 3:	//启动注册进程

		break;

	  case 4:	//退回MENU
		Write_DspBuf(SCREEN_MENU,&Srn[SCREEN_MENU]);
		fSrnRefresh=TRUE;
		Step[STEPS_PSW_EDIT]=0;
		break;

	  default: break;
	}
}



/*
void DoEditFunc(SCREEN *iSrn)
{
BYTE c,x,y;

	x=iSrn->CurX;
	y=iSrn->CurY;
	c=iSrn->bufLCD[x][y];

	if((c==' ')||((c>='0')&&(c<='9'))){
	  DigitToggle(iSrn);
	}
	else{
	  if((x==X_OK)&&(y==Y_OK)){		//中文
	    DoEditOK();
	  }
	  else((x==X_CANCEL)&&(y==Y_CANCEL)){
		DoEditCancel();
	  }
	}
}


	LoadRegMenu();
	ShowRegMenu();

	CurX=0; CurY=0;
	CursorLastTab();
	fCursorOn=ENABLED;
	iTimeCmp=ms_click+BLINK_GAP;		//128;	//time control for cursor blink

	idle_cmp=ms_click+WAIT_FOR_DSP;	//break while when time is up
	while(1){
	  if(IsTimeUp(idle_cmp)==TRUE){
	    return 0;		//time over,exit
	  }

	  k=GetKey();
	  switch(k){
		case KEY1:		//Cursor "Down"
			CursorDown(CurX,CurY);
			break;
		case KEY2:		//Cursor "->"
			CursorRight();
			break;
		case KEY3:		//Cursor "Sel"
			CursorSel();
			break;
	  	case KEY4:		//Cursor "Ok"
			//根菜单
		    if(iPort==0xff){
			  if(IsQuit()){
			    return PORT_MENU_UP_LAYER;
			  }
			  else if(IsOk()){
			    GetCursor(&p2[DATA0]);
			    return PORT_MENU_NEXT_LAYER;
			  }
			  else{;}
		    }

			//第2层菜单
		    else{
			  //===[Save]===
		      if(IsSave()){
				*fItemSaved=TRUE;		//存储发生后，设备应复位
			    lcd_clear();
			    if(WritePortSetup(iPort,iItem,ports)==OK){	//save setup value
			      lcd_puts(0,L1), lcd_puts("    Save ok             ");
			    }
			    else{lcd_puts(0,L1), lcd_puts(" Save failed !          ");}
			    Delay_s(1);
			    return PORT_MENU_SAME;
		      }

			  //===[Next]===
		      else if(IsNext()){	//Next
			    return PORT_MENU_NEXT;
		      }

			  //===[Quit]===
		      else if(IsQuit()){return PORT_MENU_UP_LAYER;}				//Quit

			  //other ?
		      else{continue;}
		    }
			break;

		  default: 
			continue;
		}

	return 1;
}*/

/*
//==============================
// Tab cursor:
//
// Looking for next tab-stop
//
//==============================
void CursorDown(BYTE x,BYTE y)
{
//refresh char, not '_'
	CharStamp(x,y);
//get 1st cursor behind pDefault
	GetCursor(pDefault);
}

//=================================
// show character on cursor position
// not '_'
//=================================
void CharStamp(BYTE x,BYTE y)
{
BYTE c[3];
	
	c[0]=LCD[y][x];
	c[1]=0;

	lcd_puts(x,y);
	lcd_puts_ram(&c[0]);
}


//==============================
// Move cursor RIGHT:
// 
// CurX,CurY,pForamt,pDefault
// CurY not changed.
//==============================
void CursorRight(void)
{
BYTE *p;
//
	if(fTabDigit){
	  CharStamp(CurX,CurY);
//
  	  p=pFormat;
	  if(CurX<(LCD_WIDTH-1)){
	    if(*(++p)!=SIGN_DEFAULT){
	  	  pFormat++;
	  	  pDefault++;	  
	      CurX++;
	    }
		else{
		  FindCursorBack(pFormat);
		}

	    //is dot?
	   	p=pFormat;
	    if((*p=='.')||(*p==':')){
		  pFormat++;
		  pDefault++;
		  CurX++;
	    }
		
	  }
	  else{			//CurX=LCD_WIDTH
	    FindCursorBack(pFormat);
	  }
	}
}


//=============================
// Select value in the specific 
// scope 
//
//=============================
void CursorSel(void)
{
BYTE *saveDefault,saveCurX;
BYTE d0;
BYTE *p;

//==================================
//选项类

//looking for next option
	if((*(pFormat-1))==SIGN_OPTION){	
	  pFormat++;
	  while(1){
		if(*pFormat!=SIGN_OPTION){
		  if(*pFormat!=SIGN_DEFAULT){
			pFormat++;
			continue;
		  }
		  else{		//come across "~" region
			while(*pFormat!='{'){pFormat--;}		//point to '{'
			pFormat++; pFormat++;
			break;
		  }
		}
//'|' found
		else{
		  pFormat++;	//point to new item
		  break;
		}
	  }

//looking for pDefault
	  p=pFormat;
	  PointTo(p,SIGN_DEFAULT);	//while(*p!=SIGN_DEFAULT){p++;}
	  pDefault=++p;

//
	  saveDefault=pDefault;
	  saveCurX=CurX;

//update default area with new option.
	  p=pFormat;
	  while((*p!=SIGN_OPTION)&&(*p!=SIGN_DEFAULT)){
		*pDefault++=*p;							//update default area
		LCD[CurY][CurX]=*p;						//update LCD area
		p++; CurX++;
	  }

//restore pDefault,CurX
	  pDefault=saveDefault;
	  CurX=saveCurX;		
	}

//=================================
//数字类
	else{
//+1,scope{0:1},toggle between 0,1
	  if(*pFormat=='b'){
	    if(*pDefault=='0'){LCD[CurY][CurX]='1'; *pDefault='1';}
	    else{LCD[CurY][CurX]='0'; *pDefault='0';}
	  }

//+1,scope{0:9}
	  else if(*pFormat=='d'){
	    d0=*pDefault-'0';
	    d0++; if(d0>=0x0a){d0=0;}
	    d0=d0+'0';
	    LCD[CurY][CurX]=d0; *pDefault=d0;
	  }

//+1,scope{0:9}+空格
	  else if(*pFormat=='t'){
		if(*pDefault==' '){d0='0';}
		else if(*pDefault=='9'){d0=' ';}
		else{
	      d0=*pDefault-'0';
	      d0++; 
	      d0=d0+'0';
		}
	    LCD[CurY][CurX]=d0; *pDefault=d0;
	  }
	}

//refresh screen
	ShowLine(CurY);

}

//=========================
// Write one line to LCD
//
// line: 行号
//=========================
void ShowLine(BYTE line)
{
	LCD[line][LCD_WIDTH-1]=0;
	lcd_puts(0,line); lcd_puts_ram(&LCD[line][0]);
}
*/



//=================================================
//                 光标移动规则
//
// 1) "[  ]"每行最多一处；
// 2) bufLCD[][]至少一处'>'或'['（防止死循环）；
// 3) FindDown(): 寻找下一行中，第1处'>'或'['；
// 4) FindRight(): 
//		* 本行内，循环寻找，下一处'>'或'[';
//		* 找到'['，进入Edit模式。在FindDown时退出；
//=================================================
/*BYTE Editor(BYTE iEditScreen)
{
SCREEN Edit;
BYTE k;			//,x,y;

	ActiveSrn=&Edit;
	Write_DspBuf(iEditScreen,ActiveSrn);
	LCDShow();

	while(1){
	  k=getKey();
//	  k=1;
	  if(k){
		switch(k){
	    case KEY1:		//向下，寻找下一个‘[’
		  if(ActiveSrn->CurX!=ARROW_NOT_EXIST){
		    BlinkOff();			//取消光标闪动
		    FindDown(ActiveSrn);
		  }		
		  break;

	    case KEY2:		//向右,行内寻找下一个‘[’
		  if(ActiveSrn->CurX!=ARROW_NOT_EXIST){
		    BlinkOff();			//取消光标闪动
		    if(FindRight(ActiveSrn)){
		      //ActiveSrn->CurX++;
			  //ActiveSrn->CurX;
		    }

//		  x=ActiveSrn->CurX;
//		  y=ActiveSrn->CurY;
//		  if(bufLCD[y][x]==']'){
//		    FindDown('[',ActiveSrn);
//		  }
		  }	
		  break;

	    case KEY3:		//执行选中的任务
		  DoEditFunc(ActiveSrn);
		  break;
		}

		Delay_ms(200);
		key.val=KEY_NULL;
		key.count=0;
	  }	
	}
}
*/

//=============================================
// ms=BLINK_GAP时，显示“_”
// ms取值：{16,32,64...}
//=============================================
/*void Blink(UINT ms)
{
BYTE x,y;
BYTE c[2]; 
BYTE toggle;

	if((ms%BLINK_GAP)==0){
	
	  x=ActiveSrn->CurX;
	  y=ActiveSrn->CurY;

	  toggle=(ms/BLINK_GAP)&0x01;
	  if(toggle){	
	    c[0]=ActiveSrn->bufLCD[y][x];
	    c[1]='\0';
	    lcd_puts(x,y); lcd_puts((const char *)&c[0]);
	  }
	  else{
	    lcd_puts(x,y); lcd_puts("_");
	  }
	}
}
*/
