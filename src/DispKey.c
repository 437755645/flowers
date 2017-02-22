#include	"header.h"
#include    "h_macro.h"
#include 	<string.h>


//==============================================================
//����ʾ��_��
//#define		BlinkOff()				Blink(BLINK_GAP)	//ȡ���������
#define		ARROW_NOT_EXIST			0xff

//SCREEN_MENU
#define		LINE_MANUAL_REG		0
//#define		LINE_MANUAL_REG		0
//#define		LINE_MANUAL_REG		0
//#define		LINE_MANUAL_REG		0


//OK��ť��λ�� 
#define		X_OK			9		//ָ�� ">ȷ��"
#define		Y_OK			3		//������
#define		X_CANCEL		17		//ָ�� ">����"
#define		Y_CANCEL		3		//������


//==============================================================
extern BYTE 	Step[];			//�������STEPS_

extern BYTE		Tasking[];		//0:δ������ 1��������
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


extern BYTE		ScrollIndex;	//SCREEN_STATUS: ������ʾ���Զ�+1

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
//   		LCD��ʾ
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

	ScrollIndex=0;				//������ʾ����ʼ��һ��
//	fDspWaiting=FALSE;

	//��ʾ��ҳ?
	fHomePageChoice=FALSE;	//����ʾ��ҳ
	if(cfg[CM_YN1]&0x08){
	  fHomePageChoice=TRUE;	//��ʾ��ҳ
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
// ����Key���޸�ActiveSrn
//
// ����:
//     ����Ƿ��м����¡�
//
// ��ʱ�ж�: ������ֵAD��
// AD����жϣ������ж�
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
// ��ʼ����CurX,CurY
// ���أ�
//		0-δ�ҵ���>��
//		p-�ҵ�
//======================================
BYTE InitCurXY(SCREEN *iSrn)
{
BYTE i;

	for(i=0;i<iSrn->endL;i++){
	  if(FindRight(iSrn)){
		return 1;
	  }
	}
	iSrn->CurX=ARROW_NOT_EXIST;		//���޹�ꡱ���
	iSrn->CurY=ARROW_NOT_EXIST;
	return 0;
}


//=======================================
// ˢ��Cx��ʾ����
//
// �����¼��������µ��ã�
// 1��Ҫ�ر仯ʱ����������
// 2����ʱˢ�£���ˮλ��
//
// iSet:
//		0: ��1��(cx)
//		1: ��2��(cx)
//		2: ��3��(status)
//		3: ��4��(menu)

//		10: ID,PSW
//=======================================
void Write_DspBuf(BYTE iSet,SCREEN *iSrn)
{
BYTE *p,*pf;		//ָ��fmt

	pf=FindParaStr(CMX(CMX_LINK),"DISP");
	if(pf==0){return;}
  	pf+=1;	

	//ָ���һ��
	switch(iSet){
	case SCREEN_CX:	//��1��(cx)			
	  //===========================
	  //" >ʱ����          12.5 >"
	  //" >������          12.5 >"
	  //" >ʱˮλ         24.55 >"
	  //" >��ǰˮλ       23.26  "
	  //===========================
	  Refresh_bufLCD_Cx(iSrn,pf);		//ˢ��buf
	  iSrn->curL=0;					
	  InitCurXY(iSrn);
	  break;

	case SCREEN_SYS:	//��2��(cx)
	  while(1){
	    pf++;
		if(*pf=='"'){return;}	//û�ж��塰�ڶ��顱
		if(*pf==','){
		  pf++;
		  break;
		}
	  }
	  Refresh_bufLCD_Cx(iSrn,pf);		//ˢ��buf
	  iSrn->curL=0;					
	  InitCurXY(iSrn);
	  break;

	case SCREEN_STATUS:	//��3��(status)
	  p=&iSrn->bufLCD[0][0];
//	  if(fPowerOnReset){
	    FillBufWith(p,sizeof(SCREEN),'\0');	//�ṹ��'\0'

	  	*(p+1)='>';							//Ԥ��'>'
	    iSrn->curL=0;					
	    iSrn->endL=LINES_LCD_BUF-1;
	    iSrn->CurX=1;						//�̶���'>'λ��
	    iSrn->CurY=0;						//�̶���'>'λ��
//	  }  
//InitCurXY(iSrn);
	  break;

	case SCREEN_MENU:	//��4��(menu)
	  p=&iSrn->bufLCD[0][0]; strcpy((char *)p,(const char *)" >ע��                  ");
	  p=&iSrn->bufLCD[1][0]; strcpy((char *)p,(const char *)" >״̬                  ");
	  p=&iSrn->bufLCD[2][0]; strcpy((char *)p,(const char *)" >����                  ");
	  p=&iSrn->bufLCD[3][0]; strcpy((char *)p,(const char *)" >ά��(1Сʱ)           ");
	  iSrn->curL=0;				
	  iSrn->endL=3;		//LINES_LCD_BUF-1;	
	  iSrn->CurX=0;
	  iSrn->CurY=0;
	  InitCurXY(iSrn);
	  break;

	case SCREEN_PSW:
	  p=&iSrn->bufLCD[0][0]; strcpy((char *)p,(const char *)"  ���� [******]         ");
	  p=&iSrn->bufLCD[1][0]; strcpy((char *)p,(const char *)"                        ");
	  p=&iSrn->bufLCD[2][0]; strcpy((char *)p,(const char *)"                        ");
	  p=&iSrn->bufLCD[3][0]; strcpy((char *)p,(const char *)" ȷ��                   ");
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
// �����ʾ���� => LCD panel
//
// iScreen: ���ţ�
// curL: 	��ǰ�кţ�
//=======================================
void LCDShow(void)
{
BYTE i,*p,*p0;	
BYTE L,end,blink_save;
BYTE PL;		//����ָ��

	blink_save=fBlinkEnabled;
	fBlinkEnabled=FALSE;

	if(fLCD_PW_READY){PW_LCD_on();}

	blight_on();				//���⿪��

//��Ǵ����ڻ����е�λ�ã�".>"
	L=ActiveSrn->curL;	//������ʼ��
	if((ActiveSrn==&Srn[SCREEN_CX])||(ActiveSrn==&Srn[SCREEN_SYS])){		//����CX��SYS
	  p=&ActiveSrn->bufLCD[L][0];
	  if(L>0){*p='.';}		//�ӡ�.��:����δ����

	  p0=&ActiveSrn->bufLCD[L+3][0];
	  end=ActiveSrn->endL;
	  if((L+3)<end){*p0='.';}	//�ӡ�.��:����δ����
	}
	/*else{
	  if(ActiveSrn==&Srn[2]){							//����STATUS��MENU
		i++;		//������Debug
	  }
	}*/

//���
	lcd_clear();
	for(i=0;i<LCD_HEIGHT;i++){
	  PL=L+i;					//����ָ��
	  if(PL>=LINES_LCD_BUF){
		PL-=LINES_LCD_BUF;
	  }
	  p=&ActiveSrn->bufLCD[PL][0];

	  *(p+LCD_WIDTH)='\0';
	  lcd_puts(0,i,(const char *)p);		//��Ҫ��ʱ:30ms
	}

//���Blink
	//Delay_ms(100);
	fBlinkEnabled=blink_save;
}


//======================================
//	�ڻ�ԾiScreen�����У�
//  1���ڱ��У���RightѰ����һ����>����'[';
//  2���絽��β��ѭ��
//======================================
BYTE FindRight(SCREEN *iSrn)
{
BYTE *p;	
BYTE x,y;
BYTE s[25],line[25];

	x=iSrn->CurX;
	y=iSrn->CurY;

//xΪ��ʼ����ַ���
	p=&iSrn->bufLCD[y][0];
//	if(x){	
	  memcpy(&s[0],(const char *)p,x+1); s[x+1]=0;		//ǰ���ַ���
	  strcpy((char *)&line[0],(const char *)(p+x+1)); 	//����ַ���
	  strcat((char *)&line[0],(const char *)&s[0]);		//�ϲ�
//	}
//	else{
//	  strcpy((char *)&line[0],(const char *)p);
//	}

	p=InStr(&line[0],">",LCD_WIDTH);
	if(p){
	  x+=p-&line[0]+1;			//��һ�����ƶ���y����
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
		iSrn->CurX=x+1;		//Ѱ��'['ʱ��ָ���һ���ַ����ɱ༭��

		x=iSrn->CurX;		//��һ�����ƶ���y����
		iSrn->CurChar=iSrn->bufLCD[y][x];
		return 1;
	  }
	}
	return 0;
}


//========================================================
//	�ڻ�ԾiScreen�����У�
//  1����һ���У�Ѱ�ҵ�һ����>����'[';
//  2���絽�ײ����ص���ʼ�㣬ѭ������Ϊȡ���ˡ����ϡ�������
//  
//  ÿһ��(col=1)��ʾ������'>'
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
//curL�ƶ�
	  if((y+1)>=LCD_HEIGHT){
	    if((L+LCD_HEIGHT)<=iSrn->endL){
		  iSrn->curL++;			//�����ƶ�
	    }
		else{
		  iSrn->curL=0;		
		}
	  }

//����ƶ�
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
// View  ģʽ��Ѱ��'>';
// Editorģʽ��Ѱ��'[', ��++;
//================================================
SCREEN *DoKeyFunc(SCREEN *iSrn,BYTE key)	
{
BYTE x,y;

	switch(key){
	  case KEY1:		//����Ѱ����һ����>��
		if(iSrn->CurX!=ARROW_NOT_EXIST){
		  BlinkOff();			//ȡ���������
		  FindDown(iSrn);
		}		
		break;

	  case KEY2:		//����Ѱ����һ����>��
		if(iSrn->CurX!=ARROW_NOT_EXIST){
		  BlinkOff();			//ȡ���������
		  FindRight(iSrn);
		}	
		break;

	  case KEY3:		//���ѡ�еĹ���Func
		x=iSrn->CurX;
		y=iSrn->CurY;
		MarkFunc(iSrn,x,y);
		break;

	  case KEY4:		//ħ������
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
// Offset: ��ʼ�ֽڣ�0��ʾ��Year��ʼ��
// Len����ʾ����
// Offset=0��Len=6����ʾYMDHMS
// Offset=3��Len=3����ʾHMS
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
// ����ת�ַ���
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
// ����CSQ��BCD����ʾ
//=============================================
void Int2DoubleStr(char *sOutput,UINT *uInput)
{
char tmp[10];  	//��󣺡�256,256��

	*sOutput='\0';
	btoa(LoOfInt(*uInput),&tmp[0]);
	strcat(sOutput,&tmp[0]);
	strcat(sOutput,",");
	btoa(HiOfInt(*uInput),&tmp[0]);
	strcat(sOutput,&tmp[0]);
}

//============================================
// ��ʾ��Ч���ݣ�ERRxx
// *sOutput: ��Ҫ�㹻�ռ䣬���١�ERRxx��
//============================================
void OutputStatus(BYTE *sOutput,BYTE *Input)
{
BYTE err,code;
char sCode[3];

	*sOutput='\0';

	err=*Input&0xC0;
	code=*Input&0x3f;	
	if(err==0x40){		//��Ч����
	  strcpy((char *)sOutput,"ERR");
	  sprintf(&sCode[0],"%d",code);	//���63
	  strcat((char *)sOutput,&sCode[0]);
	}
}

void ClrStr(BYTE *s)
{
	*s='\0';
}

//=============================================
// 			��ʾ(���)sInput����Ԫ
//
// ���أ�sOutput
//=============================================
BYTE Seg2Str(BYTE *Input,BYTE *sOutput,BYTE *cx)
{
BYTE r,DspFmt;
float f;

	if((*Input)&0xC0){
	  OutputStatus(sOutput,Input);		//���������״̬ERRx����
	  return OK;
	}

	DspFmt=*(cx+CX_DSP_FMT);

//����[11 XXX LLL]
	if((DspFmt&0xC0)==0xC0){
	  DT2Str(sOutput,(hTIME *)(Input+1),cx);	  
	}

//HEX��ʽ[1000 0011]
	else if(DspFmt==0x83){
	  ClrStr(sOutput);			//���""
	}

//˫����ʽ[1000 0100]
	else if(DspFmt==0x84){
	  Int2DoubleStr((char *)sOutput,(UINT *)Input);		//[L H]=>a,b
	}

//STR��ʽ[1000 0101]
	else if(DspFmt==0x85){
	  ClrStr(sOutput);			//���""
	}


//������[1000 0001] / ������[0aaa bbbb]
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
	if(len<width){		//��Ȳ��㣬��' '����
	  str+=len;
	  for(i=len;i<width;i++){
	    *str=' ';
	    str++;
	  }
	  *str=0;
	}
	else{				//����width,�ض�
	  *(str+width)='\0';
	}
}

//============================================================
// 					����ʽ����ˢ�»���
// 1.���4����Ԫ
//
// pf: ָ���ʽ
//       !
// DISP" !
//       !       
//	     1312151617,		//��1���������ۼơ�����ʱ������������ǰˮλ������ʱˮλ��������ˮλ��
//	     000102				//��2����ѹ�������¶ȡ�CSQ
//	��
//
// iSet:
//		0: ��һ��
//		1: �ڶ���
//
// ���أ���������
//============================================================
BYTE Refresh_bufLCD_Cx(SCREEN *iSrn,BYTE *pf)		//pf: p for fmt
{
BYTE iC0,iCx;
BYTE iLine;	
BYTE r;		
BYTE *cx;	//��ʾ��cx_start
BYTE *seg;
BYTE *p;		
BYTE iBuf;		//��������
BYTE vSeg[20],sVal[20];	//��ֵ
BYTE sCxName[20];	//��Ԫ����
char line[25];
BYTE len;

	iBuf=0;
	iLine=0;	
	while(1){
	  iC0=X(*pf++);
	  iCx=X(*pf++);
	  r=ReadPort(iC0,&Port[iC0].buf[0],MODE_NORMAL);	//belt_limit=64����Ҫ��ʱ
	  cx=&cfg[getCX(iC0,iCx)];

	  //��ʾ'>'(��2��)
	  line[0]='\0';
	  strcat(&line[0],(const char *)" >");

	  //��ʾ��Ԫ����
	  memcpy(&sCxName[0],cx+CX_NAME,16);
	  strcat(&line[0],(const char *)&sCxName[0]);

	  //��ʾ��ֵ[ >XXXXXXXX      1234.56789 >]
	  seg=&Port[iC0].buf[0]+*(cx+CX_BELT_OFFSET);
	  //RestoreBELT(&vSeg[0],seg,cx);
	  FillBufWith(&sVal[0],20,'\0');
	  Seg2Str(&vSeg[0],&sVal[0],cx);

	  //��ֵ�Ҷ���
	  len=strlen((char *)&sVal[0]);
	  FillSpace((BYTE *)&line[0],LCD_WIDTH-(len+2));		//��Ԫ������չ�����' '
	  strcat(&line[0],(const char *)&sVal[0]);

	  //��ʾ' >'(β��)
	  line[LCD_WIDTH-2]=' ';
	  if(IsCxSDRecorded(cx)){		//SD��¼cx���ɲ�ѯ��ʷ��¼
	  	line[LCD_WIDTH-1]='>';
	  }
	  else{line[LCD_WIDTH-1]=' ';}
		
	  //������
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
//       SCREEN_STATUS������ʾ
//
// Ϊͳһʹ��LCDShow(),����Ӧ������д
//
// way: 
//		0: WAY_CRLF  	�س�;
//		1: WAY_CATCH  	����(������);
//		2: WAY_OVERLAP	����(������)
//==============================================
void Scroll(BYTE way,const char *msg)
{
BYTE *p;
BYTE len;
char buf[64]={0};

	if(!swLCD){return;}

	switch(way){
	case WAY_CRLF:		//����
//	  if(way==WAY_CRLF){
	  if(ScrollIndex==0){ScrollIndex=LINES_LCD_BUF-1;}	//������һ��ScrollIndex
	  else{ScrollIndex--;}
//	  }

	case WAY_OVERLAP:	//����
	  //msg => bufLCD
	  len=strlen(msg);
	  if(len>(LCD_WIDTH-2)){len=LCD_WIDTH-2;}		//������1����; -2:Ԥ���ո�λ��

	  p=&Srn[SCREEN_STATUS].bufLCD[ScrollIndex][0];
	  //*p++=' ';				//Ԥ��һ���ո����ڹ��
	  //*p++=' ';				//Ԥ��һ���ո����ڹ��
	  memcpy(p,msg,len);
	  break;

	case WAY_CATCH:
/*	  strcat(&buf[0],(const char *)p);
	  strcat(&buf[0],msg);
	  len=strlen(&buf[0]);
	  if(len>(LCD_WIDTH-2)){len=LCD_WIDTH-2;}		//������1����; -2:Ԥ���ո�λ��

	  p=&Srn[SCREEN_STATUS].bufLCD[ScrollIndex][0];
	  *p++=' ';				//Ԥ��һ���ո����ڹ��
	  *p++=' ';				//Ԥ��һ���ո����ڹ��
	  memcpy(p,&buf[0],len);
	  *(p+len)='\0';
*/
	  p=&Srn[SCREEN_STATUS].bufLCD[ScrollIndex][0];
	  strcpy(&buf[0],(const char *)p);		//���������ַ���
	  strcat(&buf[0],msg);
	  buf[LCD_WIDTH]='\0';		//�����ַ�������

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
	  fSrnRefresh=TRUE;				//����ʾʱ����ˢ��fSrnRefresh
	}
	  //curLָ��������
	Srn[SCREEN_STATUS].curL=ScrollIndex;
}


void DoEditOK(void)
{}

void DoEditCancel(void)
{}



//==========================================
//           ��괦����TOGGLE
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
	  if((x==X_OK)&&(y==Y_OK)){		//����
	    DoEditOK();
	  }
	  else if((x==X_CANCEL)&&(y==Y_CANCEL)){
		DoEditCancel();
	  }
	}
}




//==========================================
// ��Ƕ�Ӧ��func������ѭ��ִ��
//==========================================
void MarkFunc(SCREEN *iSrn,BYTE x,BYTE y)
{
//MENU����
	if(iSrn==&Srn[SCREEN_MENU]){
	  if(y==0){						//��һ�У�ע��
		Step[STEPS_PSW_EDIT]=1;
		Tasking[TASK_PSW_EDIT]=1;
		//Editor(SCREEN_MENU);
	  }
	}

//PSW����
	else if(iSrn==&Srn[SCREEN_PSW]){
	  if((y==3)&&(x==2)){			//��ȷ����
		Step[STEPS_PSW_EDIT]=3;
	  }
	  else if((y==3)&&(x==2)){			//��������
		Step[STEPS_PSW_EDIT]=4;
	  }
	}
}




//====================================
// ���༭����Ľ���
//
// ��������LOOP����
//====================================
void Editor(BYTE iEScreen)
{
static SCREEN bufEdit;

	switch(Step[STEPS_PSW_EDIT]){
	  case 1:	//��ʾ����
		ActiveSrn=&bufEdit;
		Write_DspBuf(iEScreen,ActiveSrn);
		fSrnRefresh=TRUE;
		Step[STEPS_PSW_EDIT]=2;
		break;

	  case 2:	//�ȴ�,�ް���ʱ����LOOP��ʱ�ر�
		break;

	  case 3:	//����ע�����

		break;

	  case 4:	//�˻�MENU
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
	  if((x==X_OK)&&(y==Y_OK)){		//����
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
			//���˵�
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

			//��2��˵�
		    else{
			  //===[Save]===
		      if(IsSave()){
				*fItemSaved=TRUE;		//�洢�������豸Ӧ��λ
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
//ѡ����

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
//������
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

//+1,scope{0:9}+�ո�
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
// line: �к�
//=========================
void ShowLine(BYTE line)
{
	LCD[line][LCD_WIDTH-1]=0;
	lcd_puts(0,line); lcd_puts_ram(&LCD[line][0]);
}
*/



//=================================================
//                 ����ƶ�����
//
// 1) "[  ]"ÿ�����һ����
// 2) bufLCD[][]����һ��'>'��'['����ֹ��ѭ������
// 3) FindDown(): Ѱ����һ���У���1��'>'��'['��
// 4) FindRight(): 
//		* �����ڣ�ѭ��Ѱ�ң���һ��'>'��'[';
//		* �ҵ�'['������Editģʽ����FindDownʱ�˳���
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
	    case KEY1:		//���£�Ѱ����һ����[��
		  if(ActiveSrn->CurX!=ARROW_NOT_EXIST){
		    BlinkOff();			//ȡ���������
		    FindDown(ActiveSrn);
		  }		
		  break;

	    case KEY2:		//����,����Ѱ����һ����[��
		  if(ActiveSrn->CurX!=ARROW_NOT_EXIST){
		    BlinkOff();			//ȡ���������
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

	    case KEY3:		//ִ��ѡ�е�����
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
// ms=BLINK_GAPʱ����ʾ��_��
// msȡֵ��{16,32,64...}
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
