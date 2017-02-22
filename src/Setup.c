#include 	"header.h"
#include    "h_macro.h"

//#define		SIGN_DIGIT		'%'
//#define		SIGN_OPTION		'|'
//#define		SIGN_DEFAULT	'~'
//#define		SETUP_END		'^'

//for PortItemMenu()
#define		PORT_MENU_NEXT			1
#define		PORT_MENU_SAME			2
#define		PORT_MENU_UP_LAYER		3
#define		PORT_MENU_NEXT_LAYER	4


//is option key selected: save? next? quit?
#define		IsSave()	(*pDefault=='S')&&(*(pDefault+1)=='a')&&(*(pDefault+2)=='v')&&(*(pDefault+3)=='e')
#define		IsNext()	(*pDefault=='N')&&(*(pDefault+1)=='e')&&(*(pDefault+2)=='x')&&(*(pDefault+3)=='t')
#define		IsQuit()	(*pDefault=='Q')&&(*(pDefault+1)=='u')&&(*(pDefault+2)=='i')&&(*(pDefault+3)=='t')
#define		IsOk()		(*pDefault=='O')&&(*(pDefault+1)=='k')&&(*(pDefault+2)==' ')&&(*(pDefault+3)==' ')


extern volatile	UINT 	ms_click;
extern UINT		iTimeCmp;
extern BYTE 	p2[];

//for LCD
extern 	BYTE 	ActivePort;
extern 	BYTE	CurX,CurY;		//LCD cursor position,start from {0,0}
extern 	BYTE	*pFormat,*pDefault;

//================================
//Bit usage variables
//================================
//extern TASKLINE 	TaskLine; 
//extern MSG0			Msg0;
//extern MSG1			Msg1;
//extern ERRORS		Error0;
//extern TIMERS		Tmr;
//extern TIMEUP 		TmrUp;

const BYTE mpassword[]={"Password:{%dddd~0000}       "};

//
const BYTE menu0[]={" Select Port: {%b~0}         "};
const BYTE blank_line[]={"                        "};
//const BYTE blank_line[]={"                        "};

//
const BYTE mdate[]={"Date:{%dddd.dd.dd~2004.01.01}         "};
const BYTE mtime[]={"Time:{%dd:dd:dd~00:00:00}           "};
//const BYTE blank_line[]={"                        "};

//
const BYTE mID[]={"id: {%bdddd~00000}               "};
const BYTE mUDISK[]={"Udisk days: -{%ddd~100}       "};		//缺省100天
//const BYTE blank_line[]={"                        "};

//
const BYTE type_of_com1[]={"ComTask1:[{|Off  |Gprs |Sm   |Email|Beido~Off  }]        "};
const BYTE type_of_com2[]={"ComTask2:[{|Off  |Gprs |Sm   |Email~Off  }]        "};
const BYTE type_of_com3[]={"ComTask3:[{|Off  |Gprs |Sm   |Email~Off  }]        "};

//
const BYTE tel_num[2][40]={"Tel1:{%ttttttttttt~10639702   }        ","Tel2:{%ttttttttttt~           }        "};

const BYTE optOkQuit[]={"                  [{|Ok  |Quit~Ok  }]"};
const BYTE optNext[]={"                  [{|Next|Save|Quit~Next}]"};
//const BYTE en_dis[]={"Switch:[{|On |Off~On }]            "};
const BYTE ip_1[]={"IP1:**.**.**.**         "};
const BYTE ip_2[]={"IP2:**.**.**.**         "};
const BYTE email_1[]={"Email1: **@**.**        "};
const BYTE email_2[]={"Email2: **@**.**        "};
const BYTE beido_des[]={"Beido Des:{%dddddd~213759}        "};	//北斗发送目标


BYTE LCD[LCD_HEIGHT][LCD_WIDTH];
BYTE *p;


//==============================
//BYTE GetDigit(BYTE *p);
//BYTE SaveCalendar(void);




//========================================

BYTE EnterSetup(void)
{
BYTE ports,i,iPort,iItem;
BYTE fReset,fItemSaved;
BYTE numItem;	//iPort拥有的菜单页数

	//INTCON=0b11010000;
//	TIMER_IE=FALSE;

	lcd_init(); 
	ports=GetByte(M_CFG,MAP_REG0+MAIN_NUM_OF_PORTS);
	if(ports>9){
	  lcd_cursor(0,L1); lcd_puts("Too many ports          ");
	  goto ES_END;		//FAILED
	}

	iPort=0xff;		
	iItem=0;
	fItemSaved=FALSE;
	fReset=FALSE;
	while(1){
	  lcd_clear();
	  i=PortItemMenu(iPort,iItem,&numItem,ports,&fItemSaved);
	  if(fItemSaved==TRUE){fReset=TRUE;}
//
	  switch(i){
	  	case PORT_MENU_NEXT:		//read next item-menu
		  iItem++;
		  if(iItem>=numItem){iItem=0;}
		  break;

		case PORT_MENU_SAME:
		  continue;  				//item no change

		case PORT_MENU_UP_LAYER:
		  if(iPort<=1){iItem=0; iPort=0xff;}
		  else{goto ES_END;}		//OK
		  break;

		case PORT_MENU_NEXT_LAYER:
		  iPort=*pDefault-0x30;			//Get selected value
		  break;

		default:
		  break;
	  }
	}

ES_END:
	return fReset;
}


//=======================================
// Read port(i)'s setup menu
//
// iPort:{0,1,-->},
// 其中
// 0：主板；
// 1：SensorIn；
// 2：SensorEx
//=======================================
BYTE PortItemMenu(BYTE iPort,BYTE iItem,BYTE *numItem,BYTE ports,BYTE *fItemSaved)
{
BYTE k;
UINT idle_cmp;

	*fItemSaved=FALSE;
  	if(ReadPortSetup(iPort,iItem,numItem,&p2[0])==OK){
		ShowPortMenu();
	}

	CurX=0; CurY=0;
	CursorLastTab();
	fCursorOn=ENABLED;
	iTimeCmp=ms_click+BLINK_GAP;		//128;	//time control for cursor blink

	idle_cmp=ms_click+WAIT_FOR_DSP;	//break while when time is up

	while(1){
	  	k=GetKey();
	  	Cursor_blink(CurX,CurY);

//Is idle ?
	  	if(k!=FUN_NULL){						//key pressed
	      idle_cmp=ms_click+WAIT_FOR_DSP;		//sleep after time is up
	  	}
	  	else{
		  if(IsTimeUp(idle_cmp)==TRUE){
		    fDspWaiting=FALSE;
		    iPort=0xff;	
		    return PORT_MENU_UP_LAYER;		//time over,exit
		  }
	    }

		switch(k){
		  //Cursor "Tab"
		  case FUN1:
			CursorTab(CurX,CurY);
			break;
		  //Cursor "->"
		  case FUN2:
			CursorRight();
			break;
		  //Cursor "Sel"
		  case FUN3:
			CursorSel();
			break;
		  //Cursor "Ok"
	  	  case FUN4:
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
			      lcd_cursor(0,L1), lcd_puts("    Save ok             ");
			    }
			    else{lcd_cursor(0,L1), lcd_puts(" Save failed !          ");}
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
	}	
}


//=======================================
// Read port's cfg string
// result begin from p2[0]
//
// port_id: index of port
// iItem:   index of setup menu item
//=======================================
BYTE ReadPortSetup(BYTE iPort,BYTE iItem,BYTE *numItem,BYTE *des)
{

	if(iPort==0xfe){
	  Setup_Password();
	  return OK;
	}

	if(iPort==0xff){
	  Setup_Top();
	  return OK;
	}

	//mainboard
	if(iPort==0){		
	  Setup_Mainboard(iItem,numItem);
	  return OK;
	}
	//sensor-in
	else if(iPort==1){
	  Setup_SensorIn(iPort,iItem,numItem,des);
	  return OK;
	}
	//sensor-ex
	else{
	  Setup_SensorEx(iPort,iItem,numItem,des);
	  return OK;
	}
}





BYTE Setup_SensorEx(BYTE iPort,BYTE iItem,BYTE *numItem,BYTE *des)
{
BYTE i,inst[16];
BYTE des_limit;
UINT sum;

	des_limit=200;

	FillBufWith(&inst[0],(UINT)des_limit,0x88);

	inst[VERSION]=INST_HEADER;
	inst[LEN_OF_INST]=lenHEAD-1;
	inst[ADDR_DES_L]=LoOfInt(ADDR_CARELESS);
	inst[ADDR_DES_H]=HiOfInt(ADDR_CARELESS);
	inst[ADDR_SRC_L]=LoOfInt(ADDR_CARELESS);
	inst[ADDR_SRC_H]=HiOfInt(ADDR_CARELESS);

	i=0;
	setbit0(i);				//bitINST_DIR		//inst
	clrbit(i,bitNO_ANSWER_NEEDED);		//answer needed
	setbit(i,bitIGNORE_TIME);			//no time stamp
	inst[PROPERTY]=i;

	inst[OPERATION]=OP_READ_PORT_CFG;
	inst[PARA1]=iItem;
	inst[PARA2]=CfgPages(iPort); *numItem=inst[PARA2];
	inst[PARA3]=0;
	inst[PARA4]=0;
	inst[PARA5]=0;
	inst[PARA6]=0;
	sum=GetCRC(&inst[0],lenHEAD-2);
	inst[CRC_L]=LoOfInt(sum);
	inst[CRC_H]=HiOfInt(sum);
//
	ConnectCell(iPort);
	Delay_ms(10);
	
//	i=M_SPI_OPERATION(&p2[0],p2[LEN_OF_INST]+1,des,des_limit);
	i=RWSerialSensor(&inst[0],inst[LEN_OF_INST]+1,des,des_limit);
	if(i==OK){
	  return OK;
	}
	else{
	 return FAILED;
	}
}

//=====================
//Read cfg-port
//=====================
BYTE CfgPages(BYTE iPort)
{
BYTE addr_c0,pages;
BYTE offset,i;

	addr_c0=GetByte(M_CFG,CFG_CELL0_BEG);
//	
	i=0;
	while(1){
	  pages=GetByte(M_CFG,MAP_REG0+((UINT)(addr_c0))*64+C0_CFG_PAGES);
	  if(i++==iPort){break;}
	  offset=GetByte(M_CFG,MAP_REG0+((UINT)(addr_c0))*64+C0_LEN);	//C0_LEN
      addr_c0+=offset;
	}

	return pages;
}

//=======================================
// To add setup menu items for mainboard,
// modify: 
//		numPort_SETUP_ITEMS
//
//=======================================

void Setup_Mainboard(BYTE iItem,BYTE *numItem)
{
UINT sum;
BYTE i,pDateTime[20],pID[16],pUDISK_DAYS[10],sTel[16];
BYTE *pTmp,tel_len,r,com_type;
BYTE task_index;
//const BYTE *pTel_num;

	p2[VERSION]=INST_HEADER;
	p2[LEN_OF_INST]=lenHEAD-1;
	p2[ADDR_DES_L]=LoOfInt(ADDR_CARELESS);
	p2[ADDR_DES_H]=HiOfInt(ADDR_CARELESS);
	p2[ADDR_SRC_L]=LoOfInt(ADDR_CARELESS);
	p2[ADDR_SRC_H]=HiOfInt(ADDR_CARELESS);

	i=0;
	setbit0(i);		//,bitINST_DIR);			//inst
	clrbit(i,bitNO_ANSWER_NEEDED);	//answer needed
	setbit(i,bitIGNORE_TIME);		//no time stamp
	p2[PROPERTY]=i;

	p2[OPERATION]=OP_WRITE_PORT_CFG;
	p2[PARA1]=iItem;						//index of item
	p2[PARA2]=numPORT0_SETUP_ITEMS;			//number of setup items
	p2[PARA3]=0;
	p2[PARA4]=0;
	p2[PARA5]=0;
	p2[PARA6]=0;
	sum=GetCRC(&p2[0],lenHEAD-2);
	p2[CRC_L]=LoOfInt(sum);
	p2[CRC_H]=HiOfInt(sum);


	*numItem=numPORT0_SETUP_ITEMS;

//data
	p=&p2[DATA0];
	switch(iItem){

//日期，时间
	case 0:
	  GetCurDateTime((char *)&pDateTime[0]);

	  //Date
	  pTmp=p;
	  p=str_to_ram(p,&mdate[0]); *p++=LCD_CR;	
	  AssignDefault(pTmp,&pDateTime[0],10);
	  //Time
	  pTmp=p;
  	  p=str_to_ram(p,&mtime[0]); *p++=LCD_CR;
	  AssignDefault(pTmp,&pDateTime[11],8);

	  p=str_to_ram(p,&blank_line[0]); *p++=LCD_CR;
 	  p=str_to_ram(p,&optNext[0]); *p=SETUP_END;
	  break;

//站号
	case 1:
	  GetNodeAddress((char *)&pID[0]);
	  GetUDiskDays((char *)&pUDISK_DAYS[0]);	//Udisk days

	  //node-id
	  pTmp=p; 
	  p=str_to_ram(p,&mID[0]); *p++=LCD_CR;
	  AssignDefault(pTmp,&pID[0],5);			

	  pTmp=p; 
	  p=str_to_ram(p,&mUDISK[0]); *p++=LCD_CR;			//initial with default
	  AssignDefault(pTmp,&pUDISK_DAYS[0],3);									
//
	  p=str_to_ram(p,&blank_line[0]); *p++=LCD_CR;
 	  p=str_to_ram(p,&optNext[0]); *p=SETUP_END;
	  break;

//通信任务表
	case 2:
	  pTmp=p;											//save p in pb
	  p=str_to_ram(p,&type_of_com1[0]); *p++=LCD_CR;	//initial with default
	  r=LoadComSelect(0,pTmp);							//overwrite with saved data
//
	  pTmp=p;											//save p in pb
	  p=str_to_ram(p,&type_of_com2[0]); *p++=LCD_CR;	//initial with default
	  r=LoadComSelect(1,pTmp);							//overwrite with saved data
//
	  pTmp=p;											//save p in pb
	  p=str_to_ram(p,&type_of_com3[0]); *p++=LCD_CR;	//initial with default
	  r=LoadComSelect(2,pTmp);							//overwrite with saved data
//
 	  p=str_to_ram(p,&optNext[0]); *p=SETUP_END;
	  break;

//通信任务参数1
	case 3:
TASK_SW:
	  task_index=iItem-3;
	  r=GetComByte(task_index);										
	  
	  //缺省方式为(GSM_SM或BEIDOU),需要电话号码
	  tel_len=LoadTelNum(task_index,&sTel[0]);

	  pTmp=p;
	  p=str_to_ram(p,&(tel_num[0][task_index*32])); *p++=LCD_CR;	//initial with default
	  AssignDefault(pTmp,&sTel[0],tel_len);			

	  //如果不是缺省方式,更改...
	  if(testbit(r,7)){
	  //sw==ON
	    com_type=(r>>3)&0x0f;
	    if(com_type==CONNECT_GPRS){
		  p=pTmp;
		  p=str_to_ram(p,&ip_1[0]); *p++=LCD_CR;
	      p=str_to_ram(p,&blank_line[0]); *p++=LCD_CR;
	    }
	    else if(com_type==CONNECT_EMAIL){
		  p=pTmp;
		  p=str_to_ram(p,&email_1[0]); *p++=LCD_CR;
	      p=str_to_ram(p,&blank_line[0]); *p++=LCD_CR;
	    }
/*		else if(com_type==CONNECT_BEIDOU){			//北斗通信
		  p=pTmp;
		  p=str_to_ram(p,&beido_des[0]); *p++=LCD_CR;
	      p=str_to_ram(p,&blank_line[0]); *p++=LCD_CR;
		}	*/
	    else{
	      p=str_to_ram(p,&blank_line[0]); *p++=LCD_CR;
	    }
	  }
	  else{
		p=str_to_ram(p,&blank_line[0]); *p++=LCD_CR;
	  }			
//
	  p=str_to_ram(p,&blank_line[0]); *p++=LCD_CR;
 	  p=str_to_ram(p,&optNext[0]); *p=SETUP_END;
	  break;

//通信任务参数2
	case 4:
	  goto TASK_SW;
	}
}

//===========================================
// Assign default value to setup-ram-buffer
// ram -> rem
// {%dddd-dd-dd~1234-56-78} 
//			 -->{%dddd-dd-dd~2004-10-26}
//===========================================
void AssignDefault(BYTE *p,BYTE *pDef,BYTE len)
{
char i;
	for(i=0;i<30;i++){
	  if(*p==SIGN_DEFAULT){
	    p++;
		DataCpy(p,pDef,len);
		break;
	  }
	  p++;
	}
}

void Setup_Top(void)
{
	p=&p2[DATA0];
	p=str_to_ram(p,&menu0[0]); *p++=LCD_CR;
	p=str_to_ram(p,&blank_line[0]); *p++=LCD_CR;
	p=str_to_ram(p,&blank_line[0]); *p++=LCD_CR;
	p=str_to_ram(p,&optOkQuit[0]); *p=SETUP_END;
}

void Setup_Password(void)
{
	p=&p2[DATA0];
	p=str_to_ram(p,&mpassword[0]); *p++=LCD_CR;
	p=str_to_ram(p,&blank_line[0]); *p++=LCD_CR;
	p=str_to_ram(p,&blank_line[0]); *p++=LCD_CR;
	p=str_to_ram(p,&optOkQuit[0]); *p=SETUP_END;
}



/*
 _____________________
|   Setup item: ____  |
|   1234567890123456  |
|             [Next]  |
|   Tab  ->  Ok  Sel  |
|_____________________|
*/

//
//
void ShowPortMenu(void)
{
	CurX=0; CurY=0;
	lcd_cursor(0,L0); lcd_printf(&p2[DATA0]);
//  	lcd_cursor(0,L3); lcd_puts("Tab  ->  Sel  Ok");
}


//==============================
//
//==============================
void lcd_printf(BYTE *p)
{
BYTE save;
	save=ActivePort; ActivePort=LCD_PORT;
  
	RX4_IF=0;

	while(*p!=SETUP_END){
	  if(*p=='{'){
		p++;
		p=DefaultValue(p);
		continue;
	  }
	  else if((*p==LCD_CR)||(*p==LCD_LF)){
		putCRLF();
	  }
	  else{putC(*p);}
 	  p++;
	}

 	Serial_Transmit(0x00);

	lcd_ack();
	ActivePort=save;
}

BYTE *DefaultValue(BYTE *p)
{
	while(*p!=0){
	  if(*p==SIGN_DEFAULT){
	  	p++;
	    while(*p!='}'){putC(*p); p++;}
		p++; break;			//return
	  }
	  p++;
	}
	return p;
}


//==============================
void putC(BYTE c)
{
	LCD[CurY][CurX]=c; 
	//LCD_Transmit(c);		//lcd_write(LCD_DATA,c);
	Serial_Transmit(c);

//move cursor
	if(++CurX>=LCD_WIDTH){
	  CurX=0;						//超出范围时，不换行
//	  CurY++; if(CurY>=LCD_HEIGHT){CurY=0;}
	}
}


//==============================
void putCRLF(void)
{
	CurX=0;
 	CurY++; if(CurY==LCD_HEIGHT){CurY=0;}	//rollback

	Serial_Transmit(0x00);		//结束前面的显示字符串
	lcd_ack();
	lcd_cursor(0,CurY);
}

//===================================
// Find the next 1st cursor position
// from char. pointed by p:
//	 CurX,CurY,pFormat,pDefault 
// CurY may changed
//===================================
void GetCursor(BYTE *p)
{
	CurX=0;
	while(1){
	  if(*p==SETUP_END){
		p=&p2[DATA0]; 
		CurX=0; CurY=0; 
		continue;
	  }
	  if(p!=&p2[DATA0]){
	    if(*(p-1)==LCD_CR){
		  CurX=0; 
		  CurY++; if(CurY>(LCD_HEIGHT-1)){CurY=0;}
	    }		//前一字符为回车
	  }
	  if(*p=='{'){break;}
	  p++;
	  CurX++;
	}

//'{' found.
	p++; 
	fTabDigit=FALSE;
	if(*p==SIGN_DIGIT){fTabDigit=TRUE;}
//
	p++;
	pFormat=p++;

//looking for '~'
	PointTo(p,SIGN_DEFAULT); p++;	//	while(*p!=SIGN_DEFAULT){p++;}
	pDefault=p;
}

//================================
// Find the first cursor position
// backward from pointer p:
//	 CurX,CurY,pFormat,pDefault 
// CurY not changed
//================================
void FindCursorBack(BYTE *p)
{
	while(1){
	  if(CurX>0){
		if((*p!='{')&&(*p!=SIGN_DIGIT)&&(*p!='|')){
		  CurX--;
		}					//skip reserved-word
	    p--; 
		if(*p=='{'){break;}
	  }
	}
	CurX++;

//'{' found.
	p++;p++;
	pFormat=p++;

//looking for '~'
	PointTo(p,SIGN_DEFAULT); p++;	//while(*p!=SIGN_DEFAULT){p++;}
	pDefault=p;
}


//==============================
// Tab cursor:
//
// Looking for next tab-stop
//
//==============================
void CursorTab(BYTE x,BYTE y)
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

	lcd_cursor(x,y);
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
	lcd_cursor(0,line); lcd_puts_ram(&LCD[line][0]);
}

//===============================
// Toggle between '--' and CHAR
//===============================
void Cursor_blink(BYTE x,BYTE y)
{
signed char delta;
BYTE c[2];

	delta=(signed char)(ms_click-iTimeCmp);
	if(delta>=0){
	  iTimeCmp=ms_click+BLINK_GAP;			//128;
	  if(fCursorOn==ENABLED){		
		c[0]='_';	//显示光标
		fCursorOn=DISABLED;
	  }
	  else{	    	//显示内容
		c[0]=LCD[y][x];
	  	fCursorOn=ENABLED;
	  }
	  c[1]=0;
//定位
	  lcd_cursor(x,y); lcd_puts_ram(&c[0]);
	}
}


//=========================
// Cursor stay on the last
// tab-stop, which is [Next]
// CurX=11(fixed)
//
//=========================
BYTE *CursorLastTab(void)
{
BYTE *p,*pLast,lastTab;

	CurX=0;
	p=&p2[DATA0];
	while(*p!=SETUP_END){
	  if(*p==LCD_CR){
		CurX=0;
		if(CurY<(LCD_HEIGHT-1)){CurY++;}
	  }
	  else if(*p!='{'){if(CurX<LCD_WIDTH)CurX++;}
	  else{lastTab=CurX;}		//='{'
	  p++;
	}
	pLast=p+1;
	p--;
	CurX=lastTab;

//	CurX=11;					//
//	CurY=LCD_HEIGHT-2;
//SETUP_END('^') found

//looking for '{'
	while(*p!='{'){p--;};

//'{' found.
	p++; 
	fTabDigit=FALSE;
	if(*p==SIGN_DIGIT){fTabDigit=TRUE;}
//
	p++;
	pFormat=p++;

//looking for '~'
	PointTo(p,SIGN_DEFAULT); p++;	//while(*p!=SIGN_DEFAULT){p++;}
	pDefault=p;
	return pLast;
}

//================================================
// 返回：
// 显示缓冲区中 从起始位置 到 当前光标位置 的距离
//================================================
BYTE LenToCursor(void)
{
BYTE *p,len_of_inst;

	p=CursorLastTab();
	len_of_inst=p-&p2[0]+1;
	return len_of_inst;
}


//===================================================
//
// p2[DATA0]: Menu item's
//===================================================
BYTE WritePortSetup(BYTE iPort,BYTE iItem,BYTE Ports)
{
BYTE i,*p,des_limit;
UINT sum;

	des_limit=200;

	if(iPort==0){
	  WriteMainPort(iItem);
	  return OK;
	}

	if(iPort==1){
	  SavePort1Cfg(iItem);	//Sensor bulit-in setup
	  return OK;
	}


//	FillBufWith(&p2[0],des_limit,0x88);

	p2[VERSION]=INST_HEADER;
//	p2[LEN_OF_INST]=lenHEAD-1;
	p2[ADDR_DES_L]=LoOfInt(ADDR_CARELESS);
	p2[ADDR_DES_H]=HiOfInt(ADDR_CARELESS);
	p2[ADDR_SRC_L]=LoOfInt(ADDR_CARELESS);
	p2[ADDR_SRC_H]=HiOfInt(ADDR_CARELESS);
	p2[PROPERTY]=0;
	p2[OPERATION]=OP_WRITE_PORT_CFG;
	p2[PARA1]=iItem;		//Port;
	p2[PARA2]=iPort;		//Ports;
	p2[PARA3]=0;
	p2[PARA4]=0;
	p2[PARA5]=0;
	p2[PARA6]=0;

//	p=CursorLastTab();
//	p2[LEN_OF_INST]=p-&p2[0]+1;
	p2[LEN_OF_INST]=LenToCursor();
	
	sum=GetCRC(&p2[0],lenHEAD-2);
	p2[CRC_L]=LoOfInt(sum);
	p2[CRC_H]=HiOfInt(sum);

	sum=GetCRC(&p2[DATA0],(int)(p2[LEN_OF_INST]-lenHEAD-1));
	*p++=LoOfInt(sum);
	*p=HiOfInt(sum);
//
	ConnectCell(iPort);
	Delay_ms(10);
//
//	i=M_SPI_OPERATION(&p2[0],p2[LEN_OF_INST]+1,&p2[0],des_limit);
	i=RWSerialSensor(&p2[0],p2[LEN_OF_INST]+1,&p2[0],des_limit);
	if(i==OK){
	  return OK;
	}
	else return FAILED;
}

void WriteMainPort(BYTE iItem)
{
BYTE *p;
BYTE tel_index,r,com_type;
hTIME dt;

	p=&p2[DATA0];
	switch(iItem){

//date & time
	  case 0:			
		//Year,Month,Day
	    PointTo(p,SIGN_DEFAULT);	//while(*p!=SIGN_DEFAULT){p++;}
		p++;p++;p++; dt.year=GetDigit(p);		//Year
		p++;p++;p++; dt.month=GetDigit(p);	//Month
		p++;p++;p++; dt.day=GetDigit(p);		//Day

		//Hour,Minute,Second
	    PointTo(p,SIGN_DEFAULT); p++;	//while(*p!=SIGN_DEFAULT){p++;};  
		dt.hour=GetDigit(p);				//Hour
		p++;p++;p++; dt.minute=GetDigit(p);	//Minute
		p++;p++;p++; dt.second=GetDigit(p);	//Second

		WriteCalendar(dt);		
	    break;

//测站ID，Node address
	  case 1: 
	    PointTo(p,SIGN_DEFAULT); p++;	//while(*p!=SIGN_DEFAULT){p++;}; //point to 1st digit 
		SaveNodeAddress(p);
		//保存ID后，清除“重启计数”
		ClrByte(M_MEM,MAP_RESTART_CNT);	//重启次数清零

	    PointTo(p,SIGN_DEFAULT); p++;	//while(*p!=SIGN_DEFAULT){p++;}; //point to 1st digit 
		SaveUDiskDays(p);
		break; 

//通信任务表
	  case 2:		
	    PointTo(p,SIGN_DEFAULT); p++;	//while(*p!=SIGN_DEFAULT){p++;}; //point to 1st digit 
		SaveComSelect(0,p);
	    PointTo(p,SIGN_DEFAULT); p++;	//while(*p!=SIGN_DEFAULT){p++;}; //point to 1st digit 
		SaveComSelect(1,p);
	    PointTo(p,SIGN_DEFAULT); p++;	//while(*p!=SIGN_DEFAULT){p++;}; //point to 1st digit 
		SaveComSelect(2,p);
		break;

//TelNum 1
	  case 3:
WR_TEL_CFG:		
		tel_index=iItem-3;
		r=GetComByte(tel_index);		
	  	com_type=(r>>3)&0x0f;
	    if((com_type==CONNECT_GSM_SM)||(com_type==CONNECT_BEIDOU)){
	      PointTo(p,SIGN_DEFAULT);	p++;	//while(*p!=SIGN_DEFAULT){p++;}; //point to 1st digit 
		  SaveTelephoneNum(tel_index,p);
//		  while(*p!=SIGN_DEFAULT){p++;}; p++;								//point 'on/off' 
//		  SaveComSelect(1,p);
		}
	  	break;

//TelNum 2
	  case 4:
		goto WR_TEL_CFG;
	}
}


BYTE GetDigit(BYTE *p)
{
BYTE b0;
	if(isdigit(*p)){
	  b0=*p-0x30; 
	  p++;
	  if(isdigit(*p)){
		b0=b0*10+(*p-0x30);
//	  	b0=HEX2BCD(b0);
		return b0;
	  }
	  else{return 0;}
	}
	else{return 0;}
}



//==============================
// Get string of Node-address
// max-address <= 19999
//==============================
void GetNodeAddress(char *pID)
{
int iNodeAddress;

	mem_ReadData(M_CFG,MAP_REG0+(UINT)MAIN_ADDRESS,(BYTE *)&iNodeAddress,2);	
	if(iNodeAddress>19999){iNodeAddress=19999;}		//<19999; 19999以上用于其他用途
//
	sprintf(pID,"%05d",iNodeAddress);
}

BYTE SaveNodeAddress(BYTE *p)		
{
int iAddr;
BYTE d[6];

	DataCpy(&d[0],p,5);
	d[5]=0;
//
	iAddr=atoi((const char *)&d[0]);
  	mem_WriteData(M_CFG,(BYTE *)&iAddr,MAP_REG0+(UINT)MAIN_ADDRESS,2);	
	return OK;
}

//=============================
// 设置 USB DISK days
//=============================
void GetUDiskDays(char *pDays)
{
BYTE days;

// 	mem_ReadData(M_CFG,MAP_REG0+(UINT)MAIN_UDISK_DAYS,&days,1);	
//
	days=GetByte(M_CFG,MAP_REG0+(UINT)MAIN_UDISK_DAYS);
	sprintf(pDays,"%03d",days);
}

BYTE SaveUDiskDays(BYTE *pDay)		
{
BYTE days;
BYTE d[6];

	DataCpy(&d[0],pDay,3);
	d[3]=0;
//
	days=(BYTE)(atoi((const char *)&d[0]));
	WriteCfg(days,MAP_REG0+(UINT)MAIN_UDISK_DAYS);
	return OK;
}



//=============
//保存电话号码
//=============
char SaveTelephoneNum(BYTE index,BYTE *p)
{
BYTE i;
BYTE tel[6];	//MaxLength=12
UINT uAddr;

	for(i=0;i<6;){
	  if(isdigit(*p)){tel[i]=*p-0x30;}
	  else{tel[i]=0x0f;}
	  p++;

	  if(isdigit(*p)){tel[i]=(tel[i]<<4)+(*p-0x30);}
	  else{tel[i]=(tel[i]<<4)+0x0f;}
	  p++;
	  i++;
	}

//save
	uAddr=MAP_REG0+(UINT)MAIN_TEL1;
	uAddr+=(((UINT)(index))*8)+2;
	mem_WriteData(M_CFG,&tel[0],uAddr,6);	
	return OK;
}


char SaveComSelect(char index,BYTE *p)
{
BYTE i,sw[3];
BYTE TelProperty;
UINT uAddr;

//validation
	for(i=0;i<3;i++){
	  sw[i]=*p++;
	}
//	
	uAddr=MAP_REG0+(UINT)MAIN_TEL1;
	uAddr+=((UINT)(index))*8;
	TelProperty=GetByte(M_CFG,uAddr);
//
	if((sw[0]=='O')&&(sw[1]=='f')&&(sw[2]=='f')){			//off
	  TelProperty&=0x7f;
	}
	else{
	  TelProperty|=0x80;
	  if((sw[0]=='S')&&(sw[1]=='m')&&(sw[2]==' ')){			//sm
		TelProperty&=0b10000111;
		TelProperty+=CONNECT_GSM_SM<<3;
	  }
	  else if((sw[0]=='G')&&(sw[1]=='p')&&(sw[2]=='r')){	//gprs
		TelProperty&=0b10000111;
		TelProperty+=CONNECT_GPRS<<3;
	  }
	  else if((sw[0]=='E')&&(sw[1]=='m')&&(sw[2]=='a')){	//email
		TelProperty&=0b10000111;
		TelProperty+=CONNECT_EMAIL<<3;
	  }
	  else if((sw[0]=='B')&&(sw[1]=='e')&&(sw[2]=='i')){	//beidou
		TelProperty&=0b10000111;
		TelProperty+=CONNECT_BEIDOU<<3;
	  }

	}

	WriteCfg(TelProperty,uAddr);
	return OK;
}

//============================
// 遇无效数据以'\0'结尾
// tel长度>=13
// 返回: 电话号码长度
// index: 0-4,其中4为监视电话
//============================
BYTE LoadTelNum(char index,BYTE *tel)
{
UINT uAddr;
BYTE i,tel_buf[6],len;
BYTE *psave,fZeroLater;

	uAddr=MAP_REG0+(UINT)MAIN_TEL1;
	uAddr+=(((UINT)(index))*8)+2;
	mem_ReadData(M_CFG,uAddr,&tel_buf[0],6);

	psave=tel;
	for(i=0;i<6;i++){
	  *tel++=HexHiByte(tel_buf[i]);
	  *tel++=HexLoByte(tel_buf[i]);
	}
	tel=psave;		//恢复

	len=0;
	fZeroLater=FALSE;
	for(i=0;i<12;i++){
	  if(fZeroLater==FALSE){
	    if((*tel<'0')||(*tel>'9')){
		  *tel=0; fZeroLater=TRUE;	
	    }
		else{len++;}
		tel++;
	  }
	  else{
	    *tel++=0xFF;	//后续全部清零
	  }
	}
	return len;
}


//===================================
// bCom: [S LLLL TT B]
// 返回：com_type
//===================================
BYTE LoadComSelect(char index,BYTE *p)		//overwrite with saved data
{
BYTE bCom,com_type,sw;

	bCom=GetComByte(index);
//
	p=GetDefaultValue(p);
	if(!p){return CONNECT_ERROR;}
//
	p++;
	sw=bCom>>7;
	if(!sw){*p++='O'; *p++='f'; *p++='f'; *p++=' '; *p=' ';}
	else{
	  com_type=(bCom>>3)&0x0f;
	  switch(com_type){
	  case CONNECT_GPRS:
		*p++='G'; *p++='p'; *p++='r'; *p++='s'; *p=' '; break;
	  case CONNECT_GSM_SM:
		*p++='S'; *p++='m'; *p++=' '; *p++=' '; *p=' '; break;
	  case CONNECT_EMAIL:
		*p++='E'; *p++='m'; *p++='a'; *p++='i'; *p='l'; break;
	  case CONNECT_BEIDOU:
		*p++='B'; *p++='e'; *p++='i'; *p++='d'; *p='o'; break;
	  default: 
	    *p++='O'; *p++='f'; *p++='f'; *p++=' '; *p=' '; break;
	  }
	}
	return com_type;
}

//===================================
// bCom: [S LLLL TT B]
// 返回：com_type
//===================================
BYTE GetComByte(char index)
{
BYTE bCom;	//,com_type,sw;
UINT uAddr;

	uAddr=MAP_REG0+(UINT)MAIN_COM1;
	uAddr+=((UINT)(index))*8;
	bCom=GetByte(M_CFG,uAddr);
	return bCom;
}




