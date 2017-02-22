#include	"header.h"
#include    "h_macro.h"
#include 	<string.h>


#define		BLINK_GAP				128		//128ms,光标闪动一次		
#define		LINES_LCD_BUF			8
#define		ARROW_NOT_EXIST			0xff

extern BYTE		fBlink;
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

//================================
//Bit usage variables
//================================
extern MSG0			Msg0;
extern MSG1			Msg1;
extern ERRORS		Error0;
extern TIMERS		Tmr;
extern TIMEUP 		TmrUp;


const char ROM_0D[]={0x0d,0x00};

typedef struct {
	BYTE bufLCD[LINES_LCD_BUF][LCD_WIDTH+1];
	BYTE ic0;
	BYTE icx;
	BYTE curL;		//窗口指针
	BYTE endL;		//缓冲结束行
	BYTE CurX;		//光标所在列
	BYTE CurY;		//光标所在行
}SCREEN;

extern BYTE		ScrollIndex;	//SCREEN_STATUS: 滚动显示，自动+1

SCREEN Srn[4];
BYTE iScreen;

#ifdef  STOP_WATCH_ON
extern UINT 	stopwatch[];			//stopwatch
extern UINT 	ms_click;
#endif


void Blink(UINT ms)
{
BYTE x,y;
BYTE c[2]; 

//16 x 时blink	
	if((ms&(BLINK_GAP-1))!=0){return;}

	x=Srn[iScreen].CurX;
	y=Srn[iScreen].CurY;


	if(ms&BLINK_GAP){	
	  lcd_cursor(x,y); lcd_puts("_");
	}
	else{
	  c[0]=Srn[iScreen].bufLCD[y][x];
	  c[1]='\0';
	  lcd_cursor(x,y); lcd_puts((const char *)&c[0]);
	}
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

	PW_LCD_on();
	lcd_init();					//lcd初始化

	fLcdReady=lcd_stable();		//等待LCD稳定响应

	blight_on();				//启动背光

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
// 键盘
//=========================================
void KeyRoutine(void)
{
	if(key.count>=KEY_CONFIRM){
	  key.count=0;
	  blight_on();
	  DoKeyFunc(key.val);
	  key.val=KEY_NULL;
	}
}

//===================================
// return: key value
//===================================
/*
BYTE ReadKey(void)
{
UINT ik;

#ifdef SIM_ON
	return FUN_NULL;
#endif

	initAdcKey();
 	initTmr3();
	_ADON=1;
	while(!_DONE)continue;
	ik=ADC1BUF0;
	_ADON=0;		//shut off A/D	
	T3_ON=0;


//无键按下！
	if((fBtnNow==0)&&(ik>=0x3f0)){
	  return FUN_NULL;
	}

//有键按下
	if(fBtnNow==1){fBtnNow=0; return FUN4;}
	if(Abs(ik,KEY1)<=KEY_WIDTH){
		return FUN1;}	//FUN1
	if(Abs(ik,KEY2)<=KEY_WIDTH){
		return FUN2;}	//FUN2
	if(Abs(ik,KEY3)<=KEY_WIDTH){
		return FUN3;}	//FUN3
	return FUN_BAD;
}



BYTE GetKey(void)
{
BYTE key;

	if(fKeyBlind==TRUE){return FUN_NULL;}

	key=ReadKey();

	if((key!=FUN_NULL)&&(key!=FUN_BAD)){	//normal key value
	  fKeyBlind=TRUE;
	  if(key==FUN4){KeyBlindCount=KEY_BLIND_BTN;}
	  else{KeyBlindCount=KEY_BLIND_AD;}		//A/D转换的键，blind时间缩短一点。
	  return key;
	}
	return FUN_NULL;
}
*/

/*
//==================================
// 将字符串不足24bytes长度的填充空格
// 扩展到24bytes。可改善显示效果
//==================================
void ExpandTo24(BYTE *line)			//&line[0])
{
char i,fStrEnd;

	fStrEnd=FALSE;
	for(i=0;i<LCD_WIDTH;i++){
	  if(*line==0){
		fStrEnd=TRUE;
	  }
	  if(fStrEnd==TRUE){
	    *line=' ';
	  }
	  line++;
	}
	*line=0;
}
*/

void Last_iCx(void)
{}

void Next_iCx(void)
{}

//==========================
// 扩展域LINK
//==========================
/*BYTE *CMX(void)
{
	return &cfg[0]+cfg[FLD_LEN]*lenCELL+CMX_LINK;
}
*/


//========================================================================================
// 									显	示
//
//  格式描述：
//	DISP“
//	1312151617,				//Page1: “日雨量累计”、“时雨量”、“当前水位”，“时水位”，“日水位”
//	000102,					//Page2: “电压、环境温度、CSQ
//	“
//========================================================================================

/*
void LCDShow(void)
{
static BYTE *pf;		//指向格式描述
static BYTE *page0;

	switch(stepDISP){
	  case 0:
		blight_on();
	    pf=FindParaStr(CMX(),"DISP");
	    pf+=1;
		page0=pf;
		stepDISP=1;

	  case 1:
		ShowDesktopCx(pf);				//按格式显示虚元数值
		fSensorChanged=0;
		iRefreshDly=ms_click+500;		//about 1s
		stepDISP=2;
		break;

	  case 2:		//等待
		if((fSensorChanged)||(IsTimeUp(iRefreshDly)==TRUE)){
		  stepDISP=1;
		}
		
		break;

	  default:break;
	}

//键盘
	key=GetKey();
	if(key){						//Any key
	  iIdleKey=ms_click+WAIT_FOR_DSP;			//sleep after time is up
	  blight_on();
	  DoKeyFunc(pf,key,page0);
	}
//	else{
//	  stepDISP=2;	//等待
//	}
}
*/


//======================================
// 初始化：CurX,CurY
// 返回：
//		0-未找到‘>’
//		1-找到
//======================================
BYTE InitCurXY(BYTE iScreen)
{
BYTE x,y;
BYTE *p;

	for(y=0;y<Srn[iScreen].endL;y++){
	  p=&Srn[iScreen].bufLCD[y][0];
	  for(x=0;x<LCD_WIDTH;x++){
	    if(*p++=='>'){
		  Srn[iScreen].CurX=x;
		  Srn[iScreen].CurY=y;
		  return 1;
		}
	  }
	}
	Srn[iScreen].CurX=ARROW_NOT_EXIST;		//“无光标”标记
	Srn[iScreen].CurY=ARROW_NOT_EXIST;
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
//=======================================
void Write_DspBuf(BYTE iSet)
{
BYTE *p,*pf;		//指向：fmt

	pf=FindParaStr(CMX(CMX_LINK),"DISP");
	if(pf==0){return;}
  	pf+=1;	

	//指向第一组
	switch(iSet){
	case 0:	//第1屏(cx)
	  Refresh_bufLCD_Cx(iSet,pf);		//刷新buf
	  break;

	case 1:	//第2屏(cx)
	  while(1){
	    pf++;
		if(*pf=='"'){return;}	//没有定义“第二组”
		if(*pf==','){
		  pf++;
		  break;
		}
	  }
	  Refresh_bufLCD_Cx(iSet,pf);		//刷新buf
	  break;

	case 2:	//第3屏(status)
	  p=&Srn[iSet].bufLCD[0][0];
	  FillBufWith(p,sizeof(SCREEN),' ');	//结构清零
	  break;

	case 3:	//第4屏(menu)
	  p=&Srn[iSet].bufLCD[0][0]; strcpy((char *)p,(const char *)" >状态                  ");
	  p=&Srn[iSet].bufLCD[1][0]; strcpy((char *)p,(const char *)" >设置                  ");
	  p=&Srn[iSet].bufLCD[2][0]; strcpy((char *)p,(const char *)" >工具                  ");
	  p=&Srn[iSet].bufLCD[3][0]; strcpy((char *)p,(const char *)" >维护(1小时)           ");
	  break;
	}

  	Srn[iSet].curL=0;					
	InitCurXY(iSet);
//	fBlink=TRUE;
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
BYTE L,end;

	blight_on();				//背光开启

//标记窗口在缓冲中的位置，".>"
	L=Srn[iScreen].curL;	//窗口起始行
	if(iScreen<=1){
	  p=&Srn[iScreen].bufLCD[L][0];
	  if(L>0){*p='.';}		//加‘.’:窗口未到顶

	  p0=&Srn[iScreen].bufLCD[L+3][0];
	  end=Srn[iScreen].endL;
	  if((L+3)<end){*p0='.';}	//加‘.’:窗口未触底
	}

//输出
	for(i=0;i<4;i++){
	  p=&Srn[iScreen].bufLCD[L+i][0];
	  *(p+LCD_WIDTH)='\0';
	  lcd_cursor(0,i); lcd_puts_ram(p);				//主要耗时:30ms
	}

//光标Blink
	fBlink=TRUE;
}

//================================
// 刷新屏幕显示
//================================
/*void RefreshScreen(BYTE iScreen)
{
	switch(iScreen){
	case 0:	//要素监视
	  Refresh_CxDspBuf(1);
	  break;

	case 1:	//设备状态
	  Refresh_CxDspBuf(2);
	  break;

	case 2:	//运行过程
//	  ProgDsp();
	  break;

	case 3: //菜单
//	  Menu();
	  break;
	}

	Show_bufLCD(BYTE iScreen,BYTE curL);
}*/



//==========================================
// 执行对应的func
//==========================================
void DoFunc(BYTE iScreen,BYTE x,BYTE y)
{
}

/*
BYTE curL(void)
{
	switch(iScreen){
	case 0:
	  return curL1;
	case 1:
	  return curL2;
	}
}


void inc_curL(void)
{
	switch(iScreen){
	case 0:
	  ++curL1; 
	  break;
	case 1:
	  ++curL2; 
	  break;
	}
}
*/

//======================================
//	在活跃iScreen缓冲中：
//  1、向Right寻找下一个‘>’;
//  2、如到底部，循环
//======================================
BYTE FindArrowRight(BYTE iScreen)
{
BYTE *p;
BYTE x,y;	//,E;
BYTE i;

	x=Srn[iScreen].CurX;
	y=Srn[iScreen].CurY;
	//E=Srn[iScreen].endL;
	p=&Srn[iScreen].bufLCD[y][x];
	for(i=0;i<LCD_WIDTH;i++){
	  p++;
	  if(p>&Srn[iScreen].bufLCD[y][LCD_WIDTH]){
		p=&Srn[iScreen].bufLCD[y][0];
	  }

	  if(*p=='>'){
		Srn[iScreen].CurX=x;
		Srn[iScreen].CurY=y;
		return 1;
	  }
	}
	return 0;
}

//======================================
//	在活跃iScreen缓冲中：
//  1、向Down寻找下一个‘>’;
//  2、如到底部，循环
//======================================
BYTE FindArrowDown(BYTE iScreen)
{
BYTE *p;
BYTE x,y;

	x=Srn[iScreen].CurX;
	y=Srn[iScreen].CurY;
	p=&Srn[iScreen].bufLCD[y][x];
	while(1){
	  //curL移动
	  if(y>3){
	    if((y+4)>=Srn[iScreen].endL){
		  Srn[iScreen].curL++;
	    }
	  }

	  //光标移动
	  p+=LCD_WIDTH;
	  y++;
	  if(y>Srn[iScreen].endL){return 0;}
	  if(*p=='>'){
		Srn[iScreen].CurY=y;
	    return 1;
	  }

	  if(FindArrowRight(iScreen)){
	    return 1;
	  }
	}
	return 1;
}





void DoKeyFunc(BYTE key)	
{
BYTE x,y;

	switch(key){
	  case KEY1:		//向下寻找下一个‘>’
		if(Srn[iScreen].CurX!=ARROW_NOT_EXIST){
		  FindArrowDown(iScreen);
		}		
		break;

	  case KEY2:		//向右寻找下一个‘>’
		if(Srn[iScreen].CurX!=ARROW_NOT_EXIST){
		  FindArrowRight(iScreen);
		}	
		break;

	  case KEY3:		//执行选中的任务
		x=Srn[iScreen].CurX;
		y=Srn[iScreen].CurY;
		DoFunc(iScreen,x,y);
		break;

	  case KEY4:		//魔法键：
		if(iScreen<3){iScreen++;}
		else{iScreen=0;}
		break;
	}
}

//============================================
//                    Key
// 1、NextLine----下一行；
//    虚元数超过一屏幕时，移至下一屏？
// 2、MoveRight---行内右移；
// 3、Select------光标内选择;
// 4、OK----------魔法按钮：
//	  在“虚元当前值/终端状态/菜单”之间toggle;
//    
//
// 根据key,确定pf
//
//  格式描述：
//	DISP“
//	1312151617,				//Page1: “日雨量累计”、“时雨量”、“当前水位”，“时水位”，“日水位”
//	000102,					//Page2: “电压、环境温度、CSQ
//	“						//Page3: “查询/设置/工具“，逐行显示，/。
//
//  page0:	首页首地址
//	pf0:	本页首地址
//  pf: 	当前指向Cx
//
//
//	~设置|维护|工具.记录存入U盘/格式化TF卡/初始化Modem|测试|~		
//
//
//============================================
/*
void DoKeyFunc(BYTE *pf,BYTE key,BYTE *page0)	
{
//static BYTE *pf0;

	switch(key){
	  case FUN1:					//快进(翻页):=>Next page
		
	 	//if((curL()+4)<3){inc_curL();}

		//pf=NextPage(pf,page0);
		//pf0=pf;
		break;

	  case FUN2:	//左右toggle
		tmp=curC;
		if(curC==0){curC=LCD_WIDTH;}
		else{curC=0;}
		if(bufLCD1[curL][curC]!='>'){
		  curC=tmp;
		}

		break;

	  case FUN3:					//选择:本page中选Cx
		//pf=SlideThisPage(pf,pf0);
		break;

	  case FUN4:					//OK键：
		iScreen++;

		//Func(pf);
		break;
	}
}
*/

/*
BYTE Menu1(BYTE *pf)
{
BYTE r;
BYTE item[24];

	r=Password();
	if(r==PSW_WAIT){return PSW_WAIT;}
	else if(r==PSW_FAILED){
	  return PSW_FAILED;
	}
//
	pf0=pf;
	i=0;
	while(1){
	  if(*pf!='~'){
		return;
	  }

	  if(*pf!='.'){
		pf=InStr(pf,"|",40){	
		  pf++;
		  continue;
		}
	  }

	  if(*pf!='|'){item[i]=*pf;}
	  else{
		item[i]=0;
		lcd_puts(&item[0]);
		i=0;
	  }
	  pf++;
	}	
}
*/

//====================================
// 在本页循环显示：指针在本页移动
//====================================
BYTE *SlideThisPage(BYTE *pf,BYTE *pf0)
{
BYTE i;

	if(*pf0=='~'){

	}

	for(i=0;i<LCD_HEIGHT;i++){
	  pf+=2;
	  if(*pf==','){
		return pf0;		//回卷显示首页
	  }
	}
  	return pf;
}

//====================================
// 显示下一页：指针指向下一页
//
// 返回指针：指向下一页
//====================================
BYTE *NextPage(BYTE *pf,BYTE *page0)
{
BYTE i;

	for(i=0;i<64;i++){		//描述符长度限制：64/页
	  pf++;
	  if(*pf==','){
		return ++pf; 
	  }
	  if(*pf=='"'){
		return page0;		//回卷至首页
	  }
	}
	return 0;	//格式错误
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
	FormatDateTime(dt,fs);
}



//=====================================
// format dt---> fs (formated string)
// 2009.03.24  10:09:12
//=====================================
void FormatDateTime(hTIME dt,char *fs)
{
int iY;

	iY=((int)dt.year)+2000;		//2000;			
	sprintf(fs,"%04d",iY);
	fs++; fs++; fs++; fs++;
	*fs++='-';

//Month
	sprintf(fs,"%02d",dt.month);
	fs++; fs++;
	*fs++='-';

//Day
	sprintf(fs,"%02d",dt.day);
	fs++; fs++;
	*fs++=' ';
	
//Hour
	sprintf(fs,"%02d",dt.hour);
	fs++; fs++;
	*fs++=':';

//Minute
	sprintf(fs,"%02d",dt.minute);
	fs++; fs++;
	*fs++=':';

//Second
	sprintf(fs,"%02d",dt.second);
	fs++; fs++;
	*fs=0x0;		//string end
}


//=============================================
// 			显示(输出)sInput中虚元
//
// 返回：sOutput
//=============================================
//BYTE *Seg2Str(BYTE *sInput,BYTE *sOutput,BYTE *cx)
BYTE Seg2Str(BYTE *sInput,BYTE *sOutput,BYTE *cx)
{
BYTE r,DspFmt;
float f;

	r=Seg2Float(&f,sInput,cx);		
	if(r==FAILED){return FAILED;}						

	//float -> string.
	DspFmt=*(cx+CX_DSP_FMT);
	FloatToStr((char *)sOutput,f,DspFmt);
//	return sOutput;
	return OK;		
}


//====================================================
// bracket the value.
// condition:
//		1.relative para. is to be show.
//		2.no base found. show absolute value intead
//====================================================
void bracket(BYTE *vline)
{
char i;
BYTE *p;

	p=vline+15;
	for(i=0;i<15;i++){
	  p--;
	  *(p+1)=*p;
	}
	*p++='{';

	for(i=0;i<15;i++){
	  if(*p==0){*p++='}'; *p=0; return;}
	  p++;
	}
	*p++='}'; *p=0;
}


void DspBlankData(BYTE *p)
{	
char i;
	*p++=' ';
	i=14; 					//Reading data failed
	while(i!=0){*p++='-'; i--;}
	*p++=' ';
	*p=0x0;	  	
}


//======================================
// Find if the latest record can be used
// as relative-base
// It is specifically for current-value
// calculation.
//======================================
// what is true ?
// condition:
//	{'previous-record-time'+'interval'} > Now >= 'previous-record-time' 
// step 1: previous < current
// step 2: previous+interval > current

/*
BYTE TimeWithinInterval(BYTE *previous,BYTE *current,BYTE unit,BYTE interval,BYTE sub_offset)
{
BYTE i;
hTIME dt;

	i=CompareTime(current,previous);
	if((i==TIME_BIGGER)||(i==TIME_EQUAL)){
	  RestoreTimeFrom(previous,&dt);		//previous -> hYear...
	  AddInterval(&dt,unit,interval,sub_offset);
//	  SaveTimeTo(previous);

	  i=CompareTime(current,(BYTE *)&dt);		//previous);
	  if((i==TIME_SMALLER)||(i==TIME_EQUAL)){return TRUE;}
	}
	return FALSE;
}

*/	

void Waiting_CSQ(void)
{
}

void Waiting_BDQ(void)
{
}


//==========================================
//  根据格式描述，显示虚元
//
// p: 指向格式描述
//==========================================
/*
void ShowDesktopCx(BYTE *p)
{
static BYTE *p0;
BYTE iPort,r;

	p0=p;

	  while((*p!=',')||(*p!=']')){
	    iPort=X(*p);
	    r=ReadPort(iPort,&PortBelt[0],64,MODE_NORMAL);	//belt_limit=64; speed_mode=MODE_NORMAL  
		p=BeltSelect(belt,&PortBelt[0],p);		//指向‘]’
	  }	
}
*/


//==========================================================
// Fill spaces ' ' at end of string to expand to 'width' of LCD
// e.g: "123" to "123  " 
// in some case to overwrite the wrong char's in LCD
//==========================================================
void FillSpace(BYTE *str,BYTE width)
{
char i,len;
	len=strlen((const char *)str);
	str+=len;
	for(i=len;i<width;i++){
	  *str=' ';
	  str++;
	}
	*str=0;
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
BYTE Refresh_bufLCD_Cx(BYTE iSet,BYTE *pf)		//pf: p for fmt
{
BYTE iC0,iCx;
BYTE iLine;	
BYTE r;		
BYTE *cx;	//表示：cx_start
BYTE *seg;
BYTE *p;		
BYTE iBuf;		//缓冲行数
BYTE sVal[20];	//数值
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
	  Seg2Str(seg,&sVal[0],cx);
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

	  p=&Srn[iSet].bufLCD[iBuf][0];
	  strcpy((char *)p,&line[0]);

	  Srn[iScreen].ic0=iC0;
	  Srn[iScreen].icx=iCx;
	  
	  iBuf++;

/*	  if((p+LCD_WIDTH+1)<p_end){
		p+=LCD_WIDTH+1;			//缓冲下一行
	    iLine++;
	  }
	  else{break;}	*/
	
	  if((*pf==',')||(*pf=='"')){break;}
	}

//行数不足
/*	while(++iLine<LCD_HEIGHT){
	  FillBufWith(p,LCD_WIDTH,' ');			//初始化:填入空格
	  p+=LCD_WIDTH+1;
	  break;
	}*/
		
	Srn[iScreen].endL=iBuf;

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
	  if(way==WAY_CRLF){
	    if(ScrollIndex==0){ScrollIndex=LINES_LCD_BUF;}	//计算下一个ScrollIndex
	    else{ScrollIndex--;}
	  }

	case WAY_OVERLAP:	//覆盖
	  //msg => bufLCD
	  len=strlen(msg);
	  if(len>=24){len=24;}		//限制在1行内

	  p=&Srn[SCREEN_STATUS].bufLCD[ScrollIndex][0];
	  memcpy(p,msg,len);
	  break;

	case WAY_CATCH:
	  strcat(&buf[0],(const char *)p);
	  strcat(&buf[0],msg);
	  len=strlen(&buf[0]);
	  if(len>=24){len=24;}		//限制在1行内

	  p=&Srn[SCREEN_STATUS].bufLCD[ScrollIndex][0];
	  memcpy(p,&buf[0],len);
	  *(p+len)='\0';
	  break;
	}
	*(p+len)='\0';

	  //curL指向最新行
	Srn[SCREEN_STATUS].curL=ScrollIndex;
}
