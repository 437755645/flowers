#include 	"header.h"
#include    "h_macro.h"


#define		CMP_GE		1
#define		CMP_G		2
#define		CMP_SE		3
#define		CMP_NE		4
#define		CMP_S		5
#define		CMP_E		6


extern PORT	Port[];		//֧�����8��?
extern BYTE cfg[];
extern BYTE c0Table[];
extern BYTE CellRequest0,CellRequest1;
extern BYTE IPL_CPU;

//����
BYTE rain_stop;			//ÿ������,��ʼֵ=2,����ѯһ�μ�1.��ʹ��ͣ�����ٷ���һ������.
UINT rain_count;
UINT uCSQ,uBDQ;
UINT wEncode,wEncode2;
UINT OldLevel;
int Temp,Vol;

//ʱ������������
//UINT baseMinuteRain,baseHourRain,baseDayRain;
typedef struct{
	UINT count;
	hTIME dt;
}RAIN_BASE;

RAIN_BASE MinuteBase,HourBase,DayBase;

//======================================
// Inner sensor
//======================================
void InitInnerSensor(void)
{
	uCSQ=0;
	uBDQ=0;

	Temp=0;
	Vol=0;

	wEncode=0;
	wEncode2=0;
	OldLevel=0;
}


void IncRain(void)
{
//UINT r;

//buzz��ʾ��	
	swBUZZ=OFF;		
	
	RAIN_IE=FALSE;		//�ر��ж�
	IncCount(usrRAIN_L);

//��������ѱ仯
	rain_stop=2;
	setbit0(CellRequest0);			//Rain count changed
	setbit(CellRequest1,7);			//At least one Cx has changed.

	Delay_ms(200);					//blind time
	Delay_ms(200);					//blind time
	swBUZZ=OFF;
	RAIN_IF=0;
	RAIN_IE=TRUE;		//�ָ��ж�
}



//==================================================
//              �û��Զ��崫�����������
//
// 1���û�������������Ȼ����,������ת����BELT��ʽ��
// 2������BuiltIn��������������������и��Ի����壻
//==================================================
void UserBelt(BYTE iC0,BYTE *b)
{
UINT c0;
//hTIME dt;
BYTE iCx;
UINT cx;

//UINT ipl_save;
//	di(ipl_save,7);
//	SetIPL(INT_CPU,IPL_CPU_NORMAL);	

	switch(iC0){
	  case 0:				//Ĭ�ϵ�ϵͳ��������ѹ...��
	  c0=getC0(iC0);		//��Ԫ�׵�ַ
	  FillBufWith(b,cfg[c0+C0_BELT_LEN],0x0);	//Ĭ��Ϊ0

	  for(iCx=0;iCx<cfg[c0+C0_CXS];iCx++){
		cx=getCX(iC0,iCx);
	    if(IsCxInBelt(&cfg[cx])){
		  b=PutCxInBelt(cx,b);
		}
	  }
  	  break;

	case 1:
	  b=beltRain(b);
	  b=beltEncoder(b);

/*	//ģ�⣺ÿ����0.5mm����,ˮλÿ��������1cm
#ifdef SIM_SENSOR
	  Now(&dt);
	  rain[0]=dt.second;	//LoOfInt(1234);				
	  rain[1]=dt.minute;	//HiOfInt(1234); 		
	  rain_count=getUINT(&rain[0]);
	  wEncode=(UINT)(dt.minute)+dt.second;	//3456	
	  wEncode2=0;


//������ˮλ
#else
	  ReadCount(usrRAIN_L,&rain_count);
	  ReadEncoder(&wEncode,&wEncode2);	//��ˮλ1,ˮλ2
#endif

	  *b++=LoOfInt(rain_count);	//Rain count	
	  *b++=HiOfInt(rain_count);	
	  *b++=LoOfInt(rain_count);	//Rain 5-min
	  *b++=HiOfInt(rain_count);	
	  *b++=LoOfInt(rain_count);	//Rain Hour
	  *b++=HiOfInt(rain_count);	
	  *b++=LoOfInt(rain_count);	//Rain Day
	  *b++=HiOfInt(rain_count);	
//
	  *b++=LoOfInt(wEncode);
	  *b++=HiOfInt(wEncode);
	  *b++=LoOfInt(wEncode);
	  *b++=HiOfInt(wEncode);
	  *b++=LoOfInt(wEncode);
	  *b++=HiOfInt(wEncode);
	  *b++=LoOfInt(wEncode);
	  *b++=HiOfInt(wEncode);	*/
	  break;
	}
}

//===================================
// COMx: x={1,2,...}
//===================================
BYTE getCOMx(BYTE *cx_link)
{
BYTE *p;

	p=InStr(cx_link,"COM",lenCELL);
	if(p){
	  return X(*(p+3));
	}
	return 0;
}


//==============================================
//
//  ��Ԫ�У���Ԫ˳�����п������⣬��Cx����Ϊ׼
//
//==============================================
BYTE *PutCxInBelt(UINT cx,BYTE *b)
{
BYTE *name;
hTIME dt;
BYTE iCom;

	name=&cfg[cx+CX_NAME];
	if(InStr(&cfg[cx],"��ѹ",16)){
	  Vol=VoltageNow();			//Real*10
	  *b++=LoOfInt(Vol);	//Voltage	
	  *b++=HiOfInt(Vol);
	}
	else if(InStr(&cfg[cx],"�¶�",16)){
	  Temp=ReadTemp();	//Real+100
//	  if(temperature!=999){}
	  *b++=LoOfInt(Temp);	//Temperature	
	  *b++=HiOfInt(Temp);
	}
	else if(InStr(&cfg[cx],"CSQ",16)){		//��֧�ֶ��CSQ
	  iCom=getCOMx(&cfg[cx+CX_LINK]);
	  uCSQ=getCSQ(iCom);
	  *b++=LoOfInt(uCSQ);	//CSQ	
	  *b++=HiOfInt(uCSQ);
	}	
	else if(InStr(&cfg[cx],"BDQ",16)){		//��֧�ֶ��BDQ
//	  iCom=getCOMx(&cfg[cx+CX_LINK]);
//	  uBDQ=getBDQ(iCom);
	  uBDQ=8;		
	  *b++=LoOfInt(uBDQ);	//BDQ	
	  *b++=HiOfInt(uBDQ);
	}	
	else if(InStr(&cfg[cx],"ʱ��",16)){
	  Now(&dt);
	  *b++=dt.year;			//����
	  *b++=dt.month;
	  *b++=dt.day;
	  *b++=dt.hour;			//ʱ��
	  *b++=dt.minute;
	  *b++=dt.second;
	}
	return b;
}



//======================================
//
//               �����¼�
//
//======================================
BYTE EventRain(void)
{
BYTE r,*link;
UINT rain_count;
ULNG ulCount,ulMod;
UINT cx;
BYTE *var; 
BYTE iC0,iCx;

	ReadCount(usrRAIN_L,&rain_count);
//
	iC0=1; iCx=1;
	cx=getCX(iC0,iCx);
	link=&cfg[cx+CX_LINK];

//���Minute����
	if(rain_count>=MinuteBase.count){
	  ulCount=rain_count-MinuteBase.count;
	}
	else{
	  ulMod=Module(cfg[cx+CX_BITS]);		//4096; bits��Ӧ�����ֵ
	  ulCount=rain_count+ulMod-MinuteBase.count;
	}

	var=(BYTE *)&ulCount;
	r=EventCondition(var,iC0,iCx,link);
	return r;
}


//=================================================
// ��ȡpָ��ı��ʽ��ʾ�ıȽ����
// ���أ�
//		1\ָ��pָ��ȽϷ���;
//		2\cmp_type
//=================================================
BYTE *getCmpType(BYTE *p,BYTE *cmp_type)
{
	if(*p=='>'){
	  if(*(p+1)=='='){
		p++;
		*cmp_type=CMP_GE;		//	">="
	  }
	  else{*cmp_type=CMP_G;}	//	">"
	}
	else if(*p=='<'){
	  if(*(p+1)=='='){
		p++;
		*cmp_type=CMP_SE;		//	"<="
	  }
	  else if(*(p+1)=='>'){
		p++;		
	    *cmp_type=CMP_NE;		//  "<>"
	  }
	  else{
		*cmp_type=CMP_S;		//	"<"
	  }		
	}
	else if(*p=='='){
	  *cmp_type=CMP_E;
	}
	return p;
}

//================================================
// ��ȡpָ��ĳ�������
// �ޣ���������
//================================================
ULNG getVal_ULNG(BYTE *p)
{
ULNG ul;
BYTE i,d[10];

	for(i=0;i<10;i++){
	  if(isdigit(*p)){d[i]=*p++;}
	  else{d[i]='\0'; break;}
	}
	ul=(ULNG)atol((const char *)(&d[0]));
	return ul;
}

//================================================
// ��ȡpָ��ĸ�����
//================================================
float getVal_FLOAT(BYTE*p)
{
float f;
BYTE i,d[10];

	for(i=0;i<10;i++){
	  if((isdigit(*p))||(*p=='.')){d[i]=*p++;}
	  else{d[i]='\0'; break;}
	}

	f=atof((const char *)(&d[0]));
	return f;
}

//================================================
// �Ƚ�
//================================================
BYTE CompareUL(BYTE *var,ULNG ulDDD,BYTE cmp_type)
{
ULNG ul;
long delta;
BYTE r;

	ul=*((ULNG *)var);
	delta=(long)(ul-ulDDD);
	r=FALSE;
	switch(cmp_type){
	  case CMP_GE:
		if(delta>=0){r=TRUE;}
		break;		
	  case CMP_G:		
		if(delta>0){r=TRUE;}
		break;		
	  case CMP_SE:		
		if(delta<=0){r=TRUE;}
		break;		
	  case CMP_NE:
		if((delta<0)|(delta>0)){r=TRUE;}
		break;		
	  case CMP_S:
		if(delta<0){r=TRUE;}
		break;		
	  case CMP_E:
		if(delta==0){r=TRUE;}
		break;		
	}
	return r;	
}

BYTE CompareFLOAT(BYTE *var,float fFFF,BYTE cmp_type)
{
float f;
long delta;
BYTE r;

	f=*((float *)var);
	delta=(long)(f-fFFF);
	r=FALSE;
	switch(cmp_type){
	  case CMP_GE:
		if(delta>=0){r=TRUE;}
		break;		
	  case CMP_G:		
		if(delta>0){r=TRUE;}
		break;		
	  case CMP_SE:		
		if(delta<=0){r=TRUE;}
		break;		
	  case CMP_NE:
		r=TRUE;					//�������Ƚϣ���Զ�����
		//if(delta<>0){r=TRUE;}
		break;		
	  case CMP_S:
		if(delta<0){r=TRUE;}
		break;		
	  case CMP_E:				//�������Ƚϣ���Զ�����
		//if(delta==0){r=TRUE;}
		break;		
	}
	return r;	
}



//================================================
// 1�����㣺var-DDD �� var-FFF
// 2��link�к����ʽ
//
// "SEVT>DDD�� ��  "SEVT>DDD��
//
// "SEVT>1&<19.6"
// "SEVT>=10.5|<-2.6"
// "SEVT=0.5"
// "SEVT<=0.5"
// "SEVT<0.5"
// "SEVT<>0.5"
//================================================
BYTE EventCondition(BYTE *var,BYTE iC0,BYTE iCx,BYTE *link)
{
BYTE *p;
BYTE cmp_type;	//">=; >; <=; <>; <"
BYTE type;		//��Ԫ��ֵ����
UINT cx;
ULNG ulDDD;
float fFFF;
BYTE r;

	p=InStr(link,"SEVT",lenCELL);
	if(!p){return 0;}

	p+=4;
	p=getCmpType(p,&cmp_type);


	cx=getCX(iC0,iCx);
	type=cfg[cx+CX_BELT_TYPE];
	switch(type){
	  case FMT_COMPACT_U:		//���ո�ʽ
	    ulDDD=getVal_ULNG(p);
		r=CompareUL(var,ulDDD,cmp_type);
		break;

	  case FMT_FLOAT_F:			//�����ʽ
		fFFF=getVal_FLOAT(p);
		r=CompareFLOAT(var,fFFF,cmp_type);
		break;
	}
	return r;
}

//===================================
// ����
//===================================
BYTE *beltRain(BYTE *b)
{
UINT HourRain,DayRain;  //,MinuteRain;

#ifdef SIM_SENSOR	//ģ�⣺ÿ����0.5mm����
BYTE rain[2];
hTIME dt;

	Now(&dt);
//	rain[0]=dt.second;	
	rain[0]=LoOfInt(1234);
				
//	rain[1]=dt.minute;	
	rain[1]=HiOfInt(1234); 		
	rain_count=getUINT(&rain[0]);

#else	//����
	ReadCount(usrRAIN_L,&rain_count);
#endif

	*b++=LoOfInt(rain_count);	//Rain count,�ۼ�ֵ	
	*b++=HiOfInt(rain_count);	

//	MinuteRain=rain_count-MinuteBase.count;
//	*b++=LoOfInt(MinuteRain);	//Rain 5-min,ʱ��ֵ
//	*b++=HiOfInt(MinuteRain);	

	*b++=LoOfInt(rain_count);	//Rain 5-min,�ۼ�ֵ
	*b++=HiOfInt(rain_count);	


	HourRain=rain_count-HourBase.count;	
	*b++=LoOfInt(HourRain);		//Rain Hour,ʱ����
	*b++=HiOfInt(HourRain);

	DayRain=rain_count-DayBase.count;				
	*b++=LoOfInt(DayRain);		//Rain Day,������
	*b++=HiOfInt(DayRain);	
	return b;
}



//===================================
// ˮλ
//===================================
BYTE *beltEncoder(BYTE *b)
{
#ifdef SIM_SENSOR		//ģ�⣺ˮλÿ��������1cm
hTIME dt;

	Now(&dt);
	wEncode=(UINT)(dt.minute)+dt.second;	//3456	
	wEncode2=0;

#else	//ˮλ
	ReadEncoder(&wEncode,&wEncode2);	//��ˮλ1,ˮλ2
#endif

	*b++=LoOfInt(wEncode);
	*b++=HiOfInt(wEncode);
	*b++=LoOfInt(wEncode);
	*b++=HiOfInt(wEncode);
	*b++=LoOfInt(wEncode);
	*b++=HiOfInt(wEncode);
	*b++=LoOfInt(wEncode);
	*b++=HiOfInt(wEncode);	
	return b;
}



//=======================================================
// ����ļ�¼�㡣
// ��������£������ж����ε�ԭ������ʱ
// ��¼���ܷ�����rightʱ���⡣
//
// when span=5,
// 10:13 ==> 10:10
// 8:01  ==> 8:00
//=======================================================
hTIME getRightMinute(hTIME dt,BYTE span)
{
	dt.minute=(dt.minute/span)*span;
	dt.second=0;
	return dt;
}

hTIME getRightHour(hTIME dt,BYTE span)
{
	dt.hour=(dt.hour/span)*span;
	dt.minute=0;
	dt.second=0;
	return dt;
}

hTIME getRightDay(hTIME dt,BYTE span)
{
	dt.day=(dt.day/span)*span;
	dt.hour=0;
	dt.minute=0;
	dt.second=0;
	return dt;
}



//=======================================================
// �û���������ʱ�������
// ��Calendar()�е���
//
// ������������ֵ��ʱ����right-timeʱ��ҲҪ�к��ʵ�ֵ��
//=======================================================
void usrSensorRecord(hTIME rT)
{
UINT cx;
BYTE span,unit,iC0,iCx;
hTIME dt0;


//����BASE
	iC0=1; iCx=1;	//Minute����
	cx=getCX(iC0,iCx);
	span=cfg[cx+CX_REC_GAP]&0x3f;
	unit=cfg[cx+CX_REC_GAP]>>6;

	switch(unit){
	  case UNIT_MINUTE:
	    dt0=getRightMinute(MinuteBase.dt,span);
	    AddMinute(&dt0,span);
	    if(rT.minute>=dt0.minute){			//�������Ϊ��=
	      ReadCount(usrRAIN_L,&MinuteBase.count);	//X������������
		  SaveTimeTo(rT,&MinuteBase.dt);
 	    }
	    break;

	  case UNIT_HOUR:
	    dt0=getRightHour(HourBase.dt,span);
	    AddHour(&dt0,span);
	    if(rT.hour>=dt0.hour){				
	      ReadCount(usrRAIN_L,&HourBase.count);		//ʱ��������
		  SaveTimeTo(rT,&HourBase.dt);
 	    }
	    break;

  	  case UNIT_DAY:
	    dt0=getRightDay(DayBase.dt,span);
	    AddDay(&dt0,span);
	    if(rT.day>=dt0.day){
	      ReadCount(usrRAIN_L,&DayBase.count);		//����������
		  SaveTimeTo(rT,&DayBase.dt);
 	    }
	    break;


	  /*if((rT.minute==0)&&(rT.second==0)){
	    ReadCount(usrRAIN_L,&baseHourRain);	//ʱ��������

	    if(rT.hour==8){						//����������
	      ReadCount(usrRAIN_L,baseDayRain);
	    }
	  }*/	
	}
}


//=================================
// ������ˮλ��
// w1: ˮλ1(��ѡ������/�Ƕ�ϵ��)
// w2: ˮλ2
//=================================
void ReadEncoder(UINT *w1,UINT *w2)
{
double coe;

	LevelCoder(w1,w2);

//�Ƕ�ϵ��
	coe=GetSlopeCo(U_ENCODE_CO);

//<1
	if((1.0-coe)>0.001){
	  *w1=(UINT)((*w1)*coe);		//angle coeficient multiplied here
	}
//==1

	if((*w1&0xFE)!=(OldLevel&0xFE)){
	  OldLevel=*w1;
	  setbit(CellRequest0,4);			//Rain count changed
	  setbit(CellRequest1,7);			//At least one Cx has changed.
	}
}

//=================================
// ������ˮλ��
// w1: ˮλ1(��ѡ������)
// w2: ˮλ2
//=================================
void LevelCoder(UINT *w1,UINT *w2)
{
//BYTE i;
BYTE b1,b0;
BYTE c1,c0;
UINT u;

//������Դ
//	CTL_NOFF=1;
if(CTL_NOFF==0)
{PW_NOFF_on();}
else
 {PW_NOFF_on();
}
//initialize direction and pin level
	Delay_ms(1);
	L_CLK=0; L_CLK_DIR=OUT; Delay_ms(1);
	L_PS=0;  L_PS_DIR=OUT;
	L_DI_DIR=IN;

//latch
  	L_PS=0;	Delay_ms(2);	//Parallel,  �������ȴ������ϳ�����;
	L_CLK=1; Nops(100);		//latch
	L_CLK=0; Nops(100);		

  	L_PS=1;	Delay_ms(1);	//Serial,	 �µ磬Nops(100);
	b1=Shift8bits();
	b0=Shift8bits();
	c1=Shift8bits();
	c0=Shift8bits();

//�͹���
  	L_PS=1;					//�µ磬ʹˮλ��������ʧ	
	L_CLK=1;
//	PW_NOFF_off();	//�رյ�Դ
	

//������
	Nop();
	Nop();
	if(!ReadEncoderPolarity()){	//0: not-operation
	  b1=~b1; b0=~b0;
	}

//������ת��������
	b1=b1&0xf;
	u=((UINT)(b1)<<8)+b0;
	*w1=GracodeToBinary(u,12);

	c1=c1&0xf;
	u=((UINT)(c1)<<8)+c0;
	*w2=GracodeToBinary(u,12);

}


BYTE Shift8bits(void)
{
char i;
BYTE b0;
	b0=0;
	for(i=0;i<8;i++){
	  b0=b0*2;
	  if(L_DI==1){ b0++; }
	  //pulse
	  L_CLK=1; Nops(200);
	  L_CLK=0; Nops(200);
	}
	return b0;
}	



//=================
// ������ת��������
//=================
// for WFH-2 compatible, g={0:0xfff}
UINT GracodeToBinary(UINT g,BYTE n)
{
signed char i;
UINT r;

	r=0;
	if(g&(((UINT)(1))<<11)){r++;}	
//
	for(i=n-2;i>=0;i--){
	  if((r&0x01)^((g>>i)&0x01)){r=r*2+1;}	
	  else{r=r*2;}
	}
	return r;
}


//=======================
// Read slope coeficient
//		0<coe<=1
// return:
//	2: right exact 0x1
//  other: value
//=======================
double GetSlopeCo(UINT addr_co)
{
BYTE sc[6];	//,*p;
double d;

	sc[0]=GetByte(M_CFG,addr_co++);
	sc[1]='.';
	sc[2]=GetByte(M_CFG,addr_co++);
	sc[3]=GetByte(M_CFG,addr_co++);
	sc[4]=GetByte(M_CFG,addr_co);
	sc[5]=0;

//==1
	if(sc[0]=='1'){return ((double)(1));}
//<1
	d=atof((const char *)&sc[0]);
	return d;
}


//========================
// Read encoder |Pos|Neg
//========================
BYTE ReadEncoderPolarity(void)
{
BYTE i;
	i=GetByte(M_CFG,U_BITS_SETTINGS);  //0: not-operation 1: no change
	return i&0x01;
}

//====================================
// new date&time updated by broadcast
//
//====================================
/*void UserBroadcast(void)
{
UINT newRain;
signed int delta;

#ifdef SIM_RAIN
	newRain=simRAIN;
#else
	newRain=ReadRain();
#endif
	delta=(signed int)(newRain-oldRain);
	if(delta>0){				//�����ۼ�ֵ�б仯
	  setbit(CellRequest0,1);	//Cx=1 ==> ����5min
//	  SensorInt();
	}
	oldRain=newRain;
	TaskLine.bitBroadcast=0;
}
*/


//=======================================
// ��MLOOP�У�ÿ�θ���1����Ԫֵ
//=======================================
void usrProc(void)
{
static BYTE iC0=0;
static BYTE iCx=0;
UINT c0,cx;
BYTE CXS,C0S;

	c0=getC0(iC0);
	//C0S=cfg[c0+C0_C0S];
	if(!testbit(cfg[c0+C0_YN2],bitC0_IN_MLOOP)){
	  if(iC0<C0S){iC0++;}
	  else{iC0=0;}
	  return;
	}

	CXS=cfg[c0+C0_CXS];	//��Ԫ��
	if(iCx++<CXS){
	  cx=getCX(iC0,iCx);
	  if(IsCxInBelt(&cfg[cx])){
		PutCxInBelt(cx,&Port[iC0].buf[0]);			//ÿѭ��1�Σ�����1����Ԫ��
	  }
	}
	else{
	  if(iC0<C0S){iC0++;}
	  else{iC0=0;}
	  iCx=0; 
	}
}

