#include 	"header.h"
#include    "h_macro.h"


//==========================
// Setup menu
//==========================
const BYTE SensorInName[]={"HydroBuiltIn 1.0        "};
const BYTE SensorInVersion[]={"2009-9-24               "};

//Rain
const BYTE RainSwitch[]={"Rain       [{|On |Off~On }]        "};
const BYTE RainAlarm[]={"Rain Alarm  {%dd.d~99.9}        "};

//Encode
const BYTE EncodeSwitch[]={"Encoder    [{|On |Off~Off}]        "};
const BYTE EncodeAlarm[]={"E.Alarm   {%ddd.dd~999.99}        "};
const BYTE EncodePol[]={"E.Polarity [{|Pos|Neg~Pos}]        "};
//
const BYTE EncodeCo[]={"Slope co. {%d.ddd~1.000}         "};	//EncodeCo[]={"Slope co. {%b.ddd~1.000}         "};
const BYTE EncodeCurLevel[]={"E.Level   {%ddd.dd~000.00}        "};


const BYTE blank_line[]={"                "};
const BYTE optNext[]={"          [{|Next|Save|Quit~Next}]"};
BYTE screen[100];


//================================================
// Setup for built-in sensor
//
// 改变Setup菜单步骤：
// 1、插入菜单条目，调整case顺序(from 0 to N)
// 2、重设NUM_OF_SETUP_ITEMS=N+1
// 3、修改对应的SavePortCfg()顺序号
//================================================
BYTE Setup_SensorIn(BYTE iPort,BYTE iItem,BYTE *numItem,BYTE *des)
{
BYTE *p,*pTmp;

	*numItem=numPORT1_SETUP_ITEMS;

	p=&screen[DATA0];
	switch(iItem){
	case 0:
//Device and version
	  p=str_to_ram(p,&SensorInName[0]); 		*p++=LCD_CR;		//initial with default
	  p=str_to_ram(p,&SensorInVersion[0]);  *p++=LCD_CR;		//initial with default
	  p=str_to_ram(p,&blank_line[0]); 		*p++=LCD_CR;
	  break;

//for Rain
	case 1:
	  pTmp=p;											//save p in pb
	  p=str_to_ram(p,&RainSwitch[0]); *p++=LCD_CR;		//initial with default
	  LoadSensorSwitch(pTmp,bitRAIN_SW);					//overwrite with saved data
	
	  pTmp=p;											//save p in pb
	  p=str_to_ram(p,&RainAlarm[0]); *p++=LCD_CR;		//initial with default
	  LoadRainAlarm(pTmp);								//overwrite with saved data
	  p=str_to_ram(p,&blank_line[0]); *p++=LCD_CR;
	  break;

//for Grid-code Encoder
	case 2:
	  pTmp=p;											//save p in pb
	  p=str_to_ram(p,&EncodeSwitch[0]); *p++=LCD_CR;	//initial with default
	  LoadSensorSwitch(pTmp,bitENCODE_SW);				//overwrite with saved data

	  pTmp=p;											//save p in pb
	  p=str_to_ram(p,&EncodeAlarm[0]); *p++=LCD_CR;		//initial with default
	  LoadLevelAlarm(pTmp,U_ENCODE_ALARM);				//overwrite with saved data

	  pTmp=p;											//save p in pb
	  p=str_to_ram(p,&EncodePol[0]); *p++=LCD_CR;		//initial with default
	  LoadEncodePolarity(pTmp);							//overwrite with saved data
	  break;

	case 3:
	  pTmp=p;											//save p in pb
	  p=str_to_ram(p,&EncodeCo[0]); *p++=LCD_CR;		//initial with default
	  LoadSlopeCo(pTmp,U_ENCODE_CO);					//overwrite with saved data

	  pTmp=p;											//save p in pb
	  p=str_to_ram(p,&EncodeCurLevel[0]); *p++=LCD_CR;	//initial with default
//	  LoadCurLevel(pTmp,U_ENCODE_BASE);					//overwrite with saved data
	  p=str_to_ram(p,&blank_line[0]); *p++=LCD_CR;
	  break;

	default:
	  break;
	}

//line 4
	p=str_to_ram(p,&optNext[0]); *p++=SETUP_END;

	return 1;
}


BYTE SavePort1Cfg(BYTE iItem)
{
BYTE *p;

	p=&screen[DATA0];
	switch(iItem){				//setup page num.
	  case 0:
		break;

//for Rain
	  case 1:
		p=GetDefaultValue(p); if(!p){return FAILED;}; p++;		//point to '~'
	    if(!SaveSensorSwitch(p,bitRAIN_SW)){return FAILED;}
		p=GetDefaultValue(p); if(!p){return FAILED;}; p++;		//point to '~'
	    if(!SaveRainAlarm(p,U_RAIN_ALARM)){return FAILED;}
		break;

//for Encoder
	  case 2:
		p=GetDefaultValue(p); if(!p){return FAILED;}; p++;		//point to '~'
	    if(!SaveSensorSwitch(p,bitENCODE_SW)){return FAILED;}
		p=GetDefaultValue(p); if(!p){return FAILED;}; p++;		//point to '~'
	    if(!SaveLevelAlarm(p,U_ENCODE_ALARM)){return FAILED;}
		p=GetDefaultValue(p); if(!p){return FAILED;}; p++;		//point to '~'
	    if(!SaveEncodePolarity(p)){return FAILED;}
		break;
//
	  case 3:
		p=GetDefaultValue(p); if(!p){return FAILED;}; p++;		//point to '~'
		if(!SaveSlopeCo(p,U_ENCODE_CO)){return FAILED;}
		p=GetDefaultValue(p); if(!p){return FAILED;}; p++;		//point to '~'
//		if(!SaveCurLevel(p,U_ENCODE_CO)){return FAILED;}
		break;
	  default:
		return FAILED;
	}
	
	return OK;
}



//====================== LOAD START ==============================


//=======================================
// Load from eeprom for setup menu items
// 
//=======================================


BYTE LoadSensorSwitch(BYTE *p,BYTE iSensor)		//overwrite with saved data
{
BYTE i;

  	i=GetByte(M_CFG,U_BITS_SETTINGS);
	i=testbit(i,iSensor);

//
	p=GetDefaultValue(p);
	if(!p){return FAILED;}
//
	p++;
	if(!i){*p++='O'; *p++='f'; *p='f';}
	else{*p++='O'; *p++='n'; *p=' ';}
	return OK;
}

//==============================================
// Read eeprom bytes to array
// check if digits
//==============================================
BYTE CheckDigits(UINT addr,BYTE *array,BYTE len)
{
char i;

	for(i=0;i<len;i++){
	  *array=GetByte(M_CFG,addr++);
	  if(isdigit(*array++)){continue;}
	  else{return 0;}
	}
	return 1;
}

BYTE LoadRainAlarm(BYTE *p)						//overwrite with saved data
{
BYTE ra[4];	//,i;
//BYTE addr_co;

//Check digit.
	if(!CheckDigits(U_RAIN_ALARM,&ra[0],3)){
	  ra[0]='9';ra[1]='9';ra[2]='.';ra[3]='9';	//load default value
	  SaveRainAlarm(&ra[0],U_RAIN_ALARM);		//save to eeprom
	  ra[0]='9';ra[1]='9';ra[2]='9';
	}

	p=GetDefaultValue(p); if(!p){return FAILED;}; p++;
	*p++=ra[0]; 
	*p++=ra[1];
	*p++='.'; 
	*p=ra[2];
	return OK;
}

BYTE LoadLevelAlarm(BYTE *p,UINT addrLevel)	//overwrite with saved data
{
BYTE la[6];

//Check digit.
	if(!CheckDigits(addrLevel,&la[0],5)){
	  la[0]='9';la[1]='9';la[2]='9';la[3]='.';la[4]='9';la[5]='9';	//load default value
	  SaveLevelAlarm(&la[0],addrLevel);		//save to eeprom
	  la[0]='9';la[1]='9';la[2]='9';la[3]='9';la[4]='9';			//load default value
	}

	p=GetDefaultValue(p);
	if(!p){return FAILED;}
//
	p++;
	*p++=la[0]; *p++=la[1]; *p++=la[2]; *p++='.'; *p=la[3]; *p=la[4];
	return OK;
}



//==========================
// load setup from EEPROM
// if at least one byte is
// not a digit,abort
// return:
//		OK
// 		FAILED
//==========================
BYTE LoadSlopeCo(BYTE *p,UINT addr_co)
{
BYTE co[5];		//,dco[17];

//Check digit.
	if(!CheckDigits(addr_co,&co[0],4)){
	  co[0]='1';co[1]='.';co[2]='0';co[3]='0';co[4]='0';	//load default value
	  SaveSlopeCo(&co[0],addr_co);							//save to eeprom
	  co[0]='1';co[1]='0';co[2]='0';co[3]='0';				//load default value
	}

	p=GetDefaultValue(p); if(!p){return FAILED;}; p++;
	*p++=co[0]; *p++='.'; *p++=co[1]; *p++=co[2]; *p=co[3];
	return OK;
}

BYTE LoadEncodePolarity(BYTE *p)
{
BYTE i;

  	i=GetByte(M_CFG,U_BITS_SETTINGS)&0x01;	//&0xfe;
//
	p=GetDefaultValue(p); if(!p){return FAILED;}; p++;

	if(i){*p++='P'; *p++='o'; *p='s';}		//1:no change, 0:not-operation
	else{*p++='N'; *p++='e'; *p='g';}
	return OK;
}



//======================  LOAD END  ==============================

//======================  SAVE START =============================
//===========================
// Save setup para.
//
//===========================
BYTE SaveSensorSwitch(BYTE *p,BYTE iSensor)
{
BYTE i,sw[3];
//BYTE j;

//validation
	for(i=0;i<3;i++){
	  sw[i]=*p++;
	}	
//
	if((sw[0]=='O')&&(sw[1]=='n')&&(sw[2]==' ')){
	  i=GetByte(M_CFG,U_BITS_SETTINGS);
	  if(!testbit(i,iSensor)){
	    setbit(i,iSensor);
	    WriteCfg(i,U_BITS_SETTINGS);			
	  }
	}
	else if((sw[0]=='O')&&(sw[1]=='f')&&(sw[2]=='f')){
	  i=GetByte(M_CFG,U_BITS_SETTINGS);
	  if(testbit(i,iSensor)){
	    clrbit(i,iSensor);
	    WriteCfg(i,U_BITS_SETTINGS);
	  }
	}
	else{return FAILED;}
	return OK;
}

//============================================
BYTE SaveRainAlarm(BYTE *p,UINT addr_rain )
{

//save, omitted '.'  
	WriteCfg(*p++,addr_rain++);		//Rain[0]);
	WriteCfg(*p++,addr_rain++);		//Rain[1]);
	p++;								//'.'
	WriteCfg(*p,addr_rain++);		//Rain[3]);

	return OK;
}


//============================================
// 999.99
//============================================
BYTE SaveLevelAlarm(BYTE *p,UINT addrLevel)
{
//BYTE i;

//save, omitted '.'  ~999.99
	WriteCfg(*p++,addrLevel++);	//Level[0]);
	WriteCfg(*p++,addrLevel++);	//Level[1]);
	WriteCfg(*p++,addrLevel++);	//Level[2]);
	p++;
	WriteCfg(*p++,addrLevel++);	//Level[4]);
	WriteCfg(*p,addrLevel++);		//Level[5]);

	return OK;
}


//========================
// Write slope coeficient
// eg: 1.000
//========================
BYTE SaveSlopeCo(BYTE *p,UINT addr_co)
{
BYTE i,Co[6];
double f0;

//check validation
	for(i=0;i<5;i++){
	  Co[i]=*p++;
	}	
	Co[5]=0;
	f0=atof((const char *)&Co[0]);
	if(f0>3.000){return FAILED;}    //if(f0>1.000){return FAILED;}

//save, omitted '.' and '\0' 
	WriteCfg(Co[0],addr_co++);
	WriteCfg(Co[2],addr_co++);
	WriteCfg(Co[3],addr_co++);
	WriteCfg(Co[4],addr_co);

	return OK;
}


//========================
// Write encoder polarity
//========================
BYTE SaveEncodePolarity(BYTE *p)
{
BYTE i,Pol[6];
//double f0;

//validation
	for(i=0;i<3;i++){
	  Pol[i]=*p++;
	}	
//
	if((Pol[0]=='P')&&(Pol[1]=='o')&&(Pol[2]=='s')){
	  i=GetByte(M_CFG,U_BITS_SETTINGS);
	  i=i|0x01;
	  WriteCfg(i,U_BITS_SETTINGS);			//0: not-operation 1: no change
	}
	else if((Pol[0]=='N')&&(Pol[1]=='e')&&(Pol[2]=='g')){
	  i=GetByte(M_CFG,U_BITS_SETTINGS);
	  i=i&0xfe;
	  WriteCfg(i,U_BITS_SETTINGS);			//0: not-operation 1: no change
	}
	else{return FAILED;}

	return OK;
}



//======================  SAVE END  ==============================



