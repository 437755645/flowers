#include 	"header.h"
#include    "h_macro.h"

//extern UINT stopwatch[];			//stopwatch
extern UINT ms_click;

//Calendar
#ifdef SIM_ON
extern hTIME simDT;
#endif

const BYTE yy0[]={31,29,31,30,31,30,31,31,30,31,30,31};
const BYTE yy1[]={31,28,31,30,31,30,31,31,30,31,30,31};


const char debug_tel[]={"13958108670"};


//================================
//Bit usage variables
//================================
extern MSG0			Msg0;
extern MSG1			Msg1;
extern ERRORS		Error0;
extern TIMERS		Tmr;
extern TIMEUP 		TmrUp;

extern BYTE	cntRecFailed;

extern hTIME rTime;	//用于保存整点或整时间（如整15分，整5分）
extern BYTE	cfg[];
extern BYTE c0Table[],cxTable[];
extern PORT	Port[];		//支持最多8个基元(C0)

//============================
// hYear's -> *saveTime
//============================
void SaveTimeTo(hTIME dt,hTIME *saveTime)
{
	saveTime->year=dt.year;
	saveTime->month=dt.month;
	saveTime->day=dt.day;
    saveTime->hour=dt.hour;             		//save hHour hMinute
	saveTime->minute=dt.minute;					//  which will be modified in AddTime()
	saveTime->second=dt.second;
}

//===========================
// *saveTime -> hYear's
//===========================
void RestoreTimeFrom(hTIME *saveTime,hTIME *dt)
{
	dt->year=(saveTime->year)&0x3f;
	dt->month=(saveTime->month)&0x3f;
	dt->day=(saveTime->day)&0x3f;
	dt->hour=(saveTime->hour)&0x3f;
	dt->minute=(saveTime->minute)&0x3f;
}

void InitCalendar(void)
{
BYTE reg[5];

	reg[2]=0x0F;	//WCTL_EXT_REG;
	reg[3]=0x00;	//WCTL_FLAG_REG;
	reg[4]=0x00;	//WCTL_REG;
	R4571_Write_Data(0x0D,&reg[2],3);

	reg[0]=1;	//60;
	reg[1]=0;
	reg[2]=0x1F;	//WCTL_EXT_REG;
	reg[3]=0x00;	//WCTL_FLAG_REG;
	reg[4]=0x10;	//WCTL_REG;
	R4571_Write_Data(0x0B,&reg[0],5);
}

void CheckCalendar(void)
{
BYTE reg[5];

#ifdef SIM_ON
	return;
#endif

	R4571_Read_Data(0x0b,&reg[0],5);		//读入控制字

	
	//发现时钟不正常后。。。
	if(((reg[2]&0x1F)!=0x1F)		//0x12---[TEST1  WADA  ??  TE   FSEL1  FSEL0  TSEL1  TSEL0]
	||((reg[3]&0x02)==0x02)			//0x02---[TEST2  ??     ??   TF   AF     ??      VLF    RSV  ]
	||((reg[4]&0x1A)!=0x10)){		//0x10---[TEST3  ??    ??  TIE  AIE    ??     STOP   ??   ]
	  Restart(RST_FOR_ERR_CALENDAR_1);
	}
}


//==============================
//Now  
//==============================
//Refresh [hYear Month Day Hour Min Sec]
//Mask unused format
//result in hex-format
char Now(hTIME *dt)
{
char r;
BYTE c0[16];

#ifdef SIM_ON
	dt->year=simDT.year;
	dt->month=simDT.month;
	dt->day=simDT.day;
	dt->hour=simDT.hour;
	dt->minute=simDT.minute;
	dt->second=simDT.second;
	return SUCCESS;
#endif


	r=SUCCESS;

    R4571_Read_Data(ADDR_CLOCK0,&c0[0],7);//时钟已经修改

//YEAR
	dt->year=BCD2HEX(c0[6]);		//从2000起
//MONTH
	dt->month=BCD2HEX(c0[5]&MON_MASK);
//DAY
	dt->day=BCD2HEX(c0[4]&DAY_MASK);
//HOUR
	dt->hour=BCD2HEX(c0[2]&HOUR_MASK);
//MINUTE
	dt->minute=BCD2HEX(c0[1]&MIN_MASK);
//SECOND
	dt->second=BCD2HEX(c0[0]&SEC_MASK);
	
//NOW_END:
	if(dt->year>90){dt->year=0; r=FAILED;}
	if(dt->month>12){dt->month=1; r=FAILED;}
	if(dt->day>31){dt->day=1; r=FAILED;}
	if(dt->hour>23){dt->hour=0; r=FAILED;}
	if(dt->minute>59){dt->minute=0; r=FAILED;}
	if(dt->second>59){dt->second=0; r=FAILED;}

	return r;			//SUCCESS;
}


//=======================================
//Write Calendar 
//=======================================
void WriteCalendar(hTIME dt)
{
BYTE c[16];

    c[0]=HEX2BCD(dt.second)&SEC_MASK;
	c[1]=HEX2BCD(dt.minute)&MIN_MASK;
    c[2]=HEX2BCD(dt.hour)&HOUR_MASK;
	c[3]=0;							//Week of the day
	c[4]=HEX2BCD(dt.day)&DAY_MASK;	
    c[5]=HEX2BCD(dt.month)&MON_MASK;	
    c[6]=HEX2BCD(dt.year);	
	c[7]=0;
	c[8]=0;	c[9]=0;	c[10]=0; c[11]=0; c[12]=0;

	R4571_Write_Data(ADDR_CLOCK0,&c[0],13);
}




BYTE DayEnd(BYTE month,BYTE year)
{
	if(year%4==0){return yy0[month-1];}
	else return yy1[month-1];
}

//============================================
//for alarm
//Calculate and adjust next interruption time.
//alarm minute is adjusted
//min_span is from 1-30(minutes)
//============================================
void AddMinute(hTIME *dt,char min_interval)
{
	dt->minute+=min_interval;
//
	if(((char)(dt->minute))>=60){
	  dt->minute-=60;
	  AddHour(dt,1);
	  return;
	}
	else if(((char)(dt->minute))<0){
	  dt->minute=((char)(dt->minute))+60;
	  AddHour(dt,-1);
	  return;
	}
	else{return;}
}


//=============================
// Hour always start from 0
//=============================
void AddHour(hTIME *dt,char hour_interval)
{
	dt->hour+=hour_interval;
//
	if(((char)(dt->hour))>=24){
	  dt->hour-=24;
	  AddDay(dt,1);
	  return;
	}
	else if(((char)(dt->hour))<0){
	  dt->hour=((char)(dt->hour))+24;
	  AddDay(dt,-1);
	  return;
	}
	else{return;}
}

//=============================
// day_interval=1 or -1
//=============================
void AddDay(hTIME *dt,char day_interval)
{
	dt->day+=day_interval;
	if(((char)(dt->day))>DayEnd(dt->month,dt->year)){
	  dt->day=1;
	  dt->month+=1;
	  if(dt->month<=12){return;}
	  dt->month=1;
	  dt->year+=1; 
	}
	else if(dt->day==0){
	  if(dt->month==1){
		dt->month=12;
		dt->year--; if(((char)(dt->year))<0){dt->year=99;}
	  }
	  else{dt->month--;}
	  dt->day=DayEnd(dt->month,dt->year);
	}
	else{return;}
}


void AddInterval(hTIME *dt,BYTE unit,char interval,BYTE hour_offset)
{
	switch(unit){
	case UNIT_DAY:						//At right hour every day
	  if(dt->hour>=8){AddDay(dt,interval);}
	  dt->hour=hour_offset;		
	  dt->minute=0;
	  dt->second=0;
	  break;
	case UNIT_HOUR:						//At every x-hour
	  AddHour(dt,interval);
	  dt->minute=0;					
	  dt->second=0;
	  break;
	case UNIT_MINUTE:
	  AddMinute(dt,interval);
	  dt->second=0;					
	  break;
	}
}

//========================
//Update date and time 
//========================
BYTE UpdateCalendar(hTIME dt)
{
//BYTE bias;


	ClrWdt();

//时间应该在别的地方取！！！
//    mem_ReadData(M_CFG,REG0+((UINT)(MAIN_DATE)),(BYTE *)&dt,6);     

	dt.year=dt.year;	//从2000算起;
	dt.month&=MON_MASK;
	dt.day&=DAY_MASK;
	dt.hour&=HOUR_MASK;
	dt.minute&=MIN_MASK;
	dt.second&=SEC_MASK;

//Setup calendar	
    WriteCalendar(dt);

	return 1;
}



//=============================================
// load new date&time to hYear,hMonth...
// 0x81 0x83 0x82...->0x03,0x02...->hYear...
//=============================================
void CleanDateTime(hTIME *dt,BYTE *p)
{
//	if(*p++!=0x81){return;}
	dt->year=(*p++)&0x7f;	
	dt->month=(*p++)&0x7f;	
	dt->day=(*p++)&0x7f;	
	dt->hour=(*p++)&0x7f;	
	dt->minute=(*p)&0x7f;
	dt->second=0;	
}

//===========================
// 误差范围+/-1分钟
// 返回fReset: 
//		0: 不需复位
//		1: 需复位
//===========================
BYTE CalendarAutoAdjust(BYTE *inst_calendar)
{
BYTE s[6];	//标准时间
BYTE yn2;		
BYTE fReset;
hTIME dt;

	fReset=FALSE;

//时间校正允许?
	yn2=cfg[CM_YN2];	//GetByte(M_CFG,REG0+((UINT)(CM_YN2)));	
	if(!testbit(yn2,bitCAL_AUTO_ADJUST)){return fReset;}		//校正禁止

//返回的指令为"空操作",不校正.
	if(*(inst_calendar+7)==OP_NULL){return fReset;}

//允许
	Now(&dt);
//
	inst_calendar+=8;
	s[YEAR]=*inst_calendar++;
	s[MONTH]=(*inst_calendar++)&0x0f;		//CONTROL(4)+MONTH(4)
	s[DAY]=*inst_calendar++;
	s[HOUR]=*inst_calendar++;
	s[MINUTE]=*inst_calendar++;
	s[SECOND]=*inst_calendar++;

	//完全一致
	if((dt.year==s[YEAR])&&(dt.month==s[MONTH])&&(dt.day==s[DAY])&&(dt.hour==s[HOUR])&&(dt.minute==s[MINUTE])){
	  return FALSE;		//年月日时分一致
	}

	//终端慢1分
	AddMinute(&dt,1);		//当前时间加1分钟
	if((dt.year==s[YEAR])&&(dt.month==s[MONTH])&&(dt.day==s[DAY])&&(dt.hour==s[HOUR])&&(dt.minute==s[MINUTE])){
	  return FALSE;		//年月日时分一致
	}

	//终端快1分
	AddMinute(&dt,-1);
	AddMinute(&dt,-1);
	if((dt.year==s[YEAR])&&(dt.month==s[MONTH])&&(dt.day==s[DAY])&&(dt.hour==s[HOUR])&&(dt.minute==s[MINUTE])){
	  return FALSE;		//年月日时分一致
	}

//至少1项不一致,校准时钟
	blight_on();
	MsgBox("时钟校准........");
	Delay_s(1);

	dt.year=s[YEAR];		//2000年起
	dt.month=s[MONTH];
	dt.day=s[DAY];
	dt.hour=s[HOUR];
	dt.minute=s[MINUTE];
	dt.second=s[SECOND];
//
	WriteCalendar(dt);
	fReset=TRUE;
	return fReset;				//校准后,需Reset
}


//========================
// rT是否是span整数倍
//========================
BYTE IsSpaned(hTIME rT)
{
BYTE unit,span;
BYTE t;


	span=cfg[CM_SPAN]&0x3f;		
span=1;//1分钟
	if(span==0){return FALSE;}
	
	unit=cfg[CM_SPAN]>>6;
unit=1;
	switch(unit){
	  case UNIT_SECOND:
		t=rT.second;
	    break; 

	  case UNIT_MINUTE:
		t=rT.minute;
	    break;
	  default:
		return FALSE;	//
	}
	
	if((t % span)!=0){return FALSE;}
	return TRUE;
}


void CalendarProc(void)
{
char i;
hTIME rT;
 
	ClrWdt();

//清除INTF,periodic中断.
	CheckCalendar();		//set control-byte,clear int flag
//
	i=Now(&rT);			            
	if(i==FAILED){cntRecFailed++; goto ABORT_CALENDAR;}

//  SensorQuery();

//保存time
	SaveTimeTo(rT,&rTime);	//rTime：只在此处1分钟刷新1次，可作为其它时间判断的依据

//最小记录间隔,广播间隔
	if(!IsSpaned(rT)){return;}

	//启动Sensor等；
	PrePortReading();

	//用户Sensor定时操作
	usrSensorRecord(rT);

	RecordBelts(&rT);

	//维持背光如果有人工介入；
	PostPortReading();

END_CALENDAR:
	end_CalendarProc();
	return;

ABORT_CALENDAR:
	goto END_CALENDAR;
}

//===================================================
// 记录All Belt
// Loop through all ports,record corresponding data
//===================================================
BYTE RecordBelts(hTIME *rDateTime)
{
BYTE iC0,iRec;
BYTE iCx;				
UINT c0;	
UINT cx;	
BYTE *des,*cx_val;

	for(iC0=0;iC0<C0S;iC0++){	 			//Loop through all C0's
	  c0=getC0(iC0);
	  if(!IsC0Enabled(&cfg[c0])){continue;}		//C0使能
	  if(!IsC0Recorded(&cfg[c0])){continue;}	//C0记录使能

	  des=&Port[iC0].buf[0];
	  ReadPort(iC0,des,MODE_NORMAL);		//读端口
//
	  for(iCx=0;iCx<cfg[c0+C0_CXS];iCx++){
		cx=getCX(iC0,iCx);
	    if(IsCxInBelt(&cfg[cx])){						//if in belt
	      if(IsCxRecorded(&cfg[cx])){					//if should be recorded

			//Is time matched?
		    if(IsRecTimeMatched(rDateTime,iC0,iCx)){
			  cx_val=des+cfg[cx+CX_BELT_OFFSET];	//offset;
		      iRec=DoRecord(cx_val,rDateTime,iC0,iCx);
			  if(iRec==0){cntRecFailed++;}

			  if(IsCxSDRecorded(&cfg[cx])){
			    TFRecord(cx_val,rDateTime,iC0,iCx);
			  }			  		    }
		  }
          //offset+=cfg[cx+CX_VAL_LEN];
	    }
      }  
  	}	

//other type of ports
//...	  
	return OK;
}


void end_CalendarProc(void)
{
        CALENDAR_IF=0;						
		CALENDAR_IE=TRUE;					
}
