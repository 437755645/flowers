#include 	"header.h"
#include    "h_macro.h"


extern volatile	UINT 	ms_click;
extern BYTE ResetCode;
extern BYTE cxTable[],c0Table[];
extern BYTE cfg[];
extern BYTE cntMemMirrorFailed;

//=============================
//       获取p指向的数值
//
//  1)自动截断后续无效字符（非数字）
//  2)至多取3个数字。
//  例如："123,"  "55]"
//  结束时：
//      p指向有效数字后。
//=============================
BYTE Val(BYTE *p,BYTE *len)
{
BYTE i,d[4];

	for(i=0;i<3;i++){
	  if(isdigit(*p)){d[i]=*p; p++;}
	  else{d[i]=0; break;}
	}
	*len=i;
	return (BYTE)atoi((const char *)(&d[0]));
}

//======================================
//                X字符的值
// ‘0’-‘9’，‘A’-‘Z’，‘a’-'z'
//
// 返回：
//    c正常：0-61
//    c错误：0xFF
//
// 与ValueHex的区别：
//  X():		A、a代表不同的值；etc.
//  ValueHex: 	A、a代表相同值,etc。
//======================================
BYTE X(BYTE c)
{
BYTE x;

	x=0;
	if(c<='9'){x=c-'0';}
	else if((c>='A')&&(c<='Z')){
	  x=c-'A'+('9'-'0'+1);
	}
	else if((c>='a')&&(c<='z')){
	  x=c-'a'+('9'-'0'+1)+('Z'-'A'+1);
	}
	else{return 0xFF;}
	return x;
}


//for Nop's delay
void Nops(BYTE x)
{
BYTE i;
	for(i=0;i<x;i++){Nop();}
}

void Serial_TransmitHL(BYTE c)
{
  	Serial_Transmit(HexHiByte(c));
  	Serial_Transmit(HexLoByte(c));
}


//===========================
ULNG getULNG(BYTE *p)		
{
ULNG ul;
//BYTE t[4];	//4：ULNG的长度

//将len个byte移入t[]
//	for(i=0;i<4;i++){
//	  if(i<len){t[i]=*p++;}
//	  else{t[i]=0;}
//	}

	ul=getUINT(p)+(((ULNG)(getUINT(p+2)))<<16);
	return ul;
}

/*
//===========================
ULLG getULLG(BYTE *p,BYTE len)
{
ULNG ull;
BYTE t[8];	//8：ULLG的长度

//将len个byte移入t[]
	for(i=0;i<8;i++){
	  if(i<len){t[i]=*p++;}
	  else{t[i]=0;}
	}

	ull=getULNG(&t[0])+((ULLG)(getULNG(&t[4]))<<32);
	return ull;
}
*/


//==========================================
// len最大：32k
//==========================================
unsigned int GetCRC(BYTE *p,int len)
{
int i;		
unsigned int sum;

	sum=0;
	for(i=len;i!=0;i--){
	  sum+=*p++;
	}

	return -1-sum;
}


//===========================================
//
// len最大：32k
//===========================================
BYTE CheckCRC(BYTE *p,int len)
{
int i;
unsigned int sum;
	sum=0;
	for(i=0;i<len;i++){
	  sum+=*p++;
	}
	sum=~sum;
	if(LoOfInt(sum)!=*p++)return 0;
	if(HiOfInt(sum)!=*p)return 0;
	return 1;
}


//=========================================================
//Dealy x ms
//Note: 
//  x ms is not accurate. When delay 1ms, at least Delay(2) 
//should be called which delay 1-2ms. 
//=========================================================

void Delay_s(BYTE x)
{
#ifdef SIM_ON
	return;
#endif

    while(x!=0){
      Delay_ms(1000);
	  x--;
    }
}

void Delay_ms(UINT x)
{
#ifdef SIM_ON
	return;
#endif

	while(x!=0){
	  ClrWdt();	
	  delay_us(916);
	  x--;
	}
}

void delay_us(UINT x)
{
UINT i;

#ifdef SIM_ON
	return;
#endif

	for(i=0;i<x;i++){
		asm("nop");
	}
}

void pureDelay_ms(UINT x)
{
	while(x!=0){
	  delay_us(916);
	  x--;
	}
}


BYTE GetMinute(void)
{
hTIME dt;

	Now(&dt);
	return dt.minute;
}

//=========================================
//File RAM area with selected char "cFill"
//=========================================
void FillBufWith(BYTE *pStart,UINT len,BYTE cFill)
{
//BYTE *pSave;
//	pSave=pStart;	//保存指针

    while(len--){*pStart++=cFill;}

//	pStart=pSave;	//恢复指针
}


/* EEPROM write and read */
/*
//void eeprom_write(BYTE addr, BYTE value)
//{
//	while(EECON1bits.WR)continue;
//	EEADR=addr;
//	EEDATA=value;
//	EECON1bits.EEPGD=0;
//	STATUSbits.C=0;
//	if(INTCONbits.GIE)STATUSbits.C=1;
//	di();
//	EECON1bits.WREN=1;
//	EECON2=0x55;
//	EECON2=0xAA;
//	EECON1bits.CFGS=0;
//	EECON1bits.WR=1; EECON1bits.WREN=0; 
//	if(STATUSbits.C)INTCONbits.GIE=1;
//	ei();
//	while(EECON1bits.WR)continue;
//}

*/



//BYTE eeprom_read(BYTE addr)
//{return 1;}


/*{ 
BYTE i;
	EEADR=addr;
	EECON1bits.EEPGD=0;

//	di();
	EECON1bits.CFGS=0;
	EECON1bits.RD=1;
	i=EEDATA;
//	ei();
	return i;			//EEDATA;
}
*/

//==========================
// 代替Macro
//==========================
//BYTE NumOfPorts(BYTE ports)		i2c_ReadBytes16(M_CFG,REG0+MAIN_NUM_OF_PORTS,&ports,1)


//read a 1% second as random byte
/*
BYTE Random(void)
{
BYTE r;
	i2c_ReadBytes(ADDR_PCF8583,0x01,&r,1);
	return r;
}
*/


BYTE HexHiByte(BYTE b)
{  
    b=b>>4;
    if(b<=9){b+='0';}  
    else{b+=('A'-10);}
    return b;                        
}
                            
BYTE HexLoByte(BYTE b)
{  
    b=b & 0x0f;
    if(b<=9){b+='0';}  
    else{b+=('A'-10);}
    return b;                        
}

//=============================
// 0x38--->00111000,0x0
// str数组大小：[9]
//=============================
void BinOfByte(BYTE c,BYTE *str)
{
char i;
	for(i=0;i<8;i++){
	  if(testbit0(c)==0x01){
	    *str++='1';
	  }
	  else{
		*str++='0';
	  }
	}
	*str=0x0;
}


int Abs(int x,int y)
{
    if(x>=y){return (x-y);}
    else{return (y-x);} 
} 

void DataCpy(BYTE *des,BYTE *src,BYTE len)
{
	while(len--){ *des++=*src++; }
}

//Compare two byte array to see if exact same.
BYTE IsDataSame(BYTE *des,BYTE *src,BYTE len)
{
	while(len--){
	  if(*des++!=*src++){return FALSE;}
	}
	return TRUE;
}


void DataCpyrom2ram(BYTE *des,const BYTE *src,BYTE len)
{
	while(len--){ *des++=*src++; }
}

//for time-stamp
//00xxxxxx -> 10xxxxxx
void TimeStampCpy(BYTE *des,hTIME *time_stamp,BYTE len)
{
BYTE t;
BYTE *src;

	src=(BYTE *)time_stamp;
	while(len--){ 
	  t=*src&0x3f;
	  *des=t|0x80; 
	  des++; src++;
 	}
}

//===========================
//convert byte to string
// "5"->"05" "124"->"12"
//return: pointer
//		0: failed
//		other: pointer; 
//===========================
/*void Format(signed char b,char *dStr,char type)
{
char tmp_save;
//
	if(type==FORMAT_BYTE_DATE_TIME){  
	  btoa(b,dStr);
	  if(!*(++dStr)){				//eg: '5'-->'05'
	    dStr--;
	    tmp_save=*dStr;
	    *dStr++='0';
		*dStr++=tmp_save;
	    *dStr=0x0;
	  }
	  else{
		*(++dStr)=0x0;				//"124"->"12",0x0. cut anyway if 2nd byte<>0
	  }
	  return;
	}

//陷阱
//	ErrorReset(0x31);
    return;
}
*/

//====================================
// 获取bits对应的模
// bit=5  => 00000000000100000
//====================================
ULNG Module(BYTE bits)
{
ULNG ul;
	ul=1;
	while(bits--){ul<<=1;}
	return ul;
}

void btoa(signed char b,char *dStr)
{
	sprintf(dStr,"%d",b);
}

//=========================================================
// "5309000100E803D3EAC1"
// =>{0x53,0x09,0x00,0x01,0x00,0xE8,0x03,0xD3,0xEA,0xC1}
//=========================================================
BYTE Txt2Bin(BYTE *bin,BYTE *txt,BYTE lenBin)	//bin的个数
{
BYTE i;
BYTE *p;

	p=txt;
	for(i=0;i<lenBin;i++){
	  if((isxdigit(*p)==FALSE)||(isxdigit(*(p+1))==FALSE)){
		return FAILED;			//非法数字
	  }
	  *bin++=ValueHex2(p);		//"1A"->0x1A 
	  p+=2;
	}
	return OK;
}


//==================================
//res -> float
//resolution's format is designed
//in iDEAL, 
//[sddddsee]: sdddd = mantissa
//            see   = exp
//            s= sign  
//e.g: 0.5=>00101101=>[5e-1]
//
// return: float of res
//==================================
/*
float ResToFloat(BYTE res)
{
float f0;
char i;
	f0=(float)(res>>3);		//mantissa
	i=(res)&(0x03);			//exp
	while(i!=0){
	  if(!testbit(res,2))
		f0=f0*10;
	  else{
	    f0=f0/10;
	  }
	  i--;
	}
	return f0;
}
*/
	
//================================
// f0->string with 四舍五入
// fmt: [xxxx yyyy]
// s: formated string
//================================
char *FloatToStr(char *s,float f0,BYTE fmt)
{
char *p;
char i;
long lng;
BYTE endings;
char t[20];
//f0=0
	if(fabsf(f0)<0.000001){
	  *s='0'; *(s+1)=0; return s;
	}

//f0!=0 below
//multiply with power of 10
	i=(fmt)&(0x0f);
	i++;                //多取1位，为四舍五入
	while(i!=0){
	  f0=f0*10;
	  i--;
	}								

//get integer string. e.g: 31415926
	lng=(long)f0;		//Get integer
//	sprintf(s,"%ld",lng);
	sprintf(&t[0],"%li",lng);
	strcpy(s,&t[0]);
//
	p=s;
	i=0;
	while(*p!=0x0){p++; continue;}
	p--;	
	if(*p>='5'){				//四舍五入
	  lng=atol(s);
	  lng++;
	  sprintf(s,"%li",lng);	//ltoa(lng,s);
	}
	p=s;
	while(*p!=0x0){p++; continue;}	//find 0x0
	p--;
	*p=0x0;                     //去尾: 去掉多取的一位	

//add decimal dot
	endings=(fmt)&(0x0f);
	i=endings+1; 
	while(i--){
	  if((p+1)==s){
		*(p+1)='0';		//目标位是首位，补'0'
		i=(fmt)&(0x0f); //右移，为首位加'.'
		i++;
		p=s; 
		while(*p!=0x0){p++; continue;}	//find 0x0
		while(1){
		  *(p+1)=*p; 
		  if(p==s){goto ADD_DOT;}
		  p--;
		}
	  }
	  else{*(p+1)=*p;}
	  p--; 
	}
	p++;

ADD_DOT:
	if(endings!=0){
	  *p='.';

	  //add '0' before '.'
      if(p==s){                           //1st digit=0x0
	    while(*p!=0x0){p++; continue;}    //come to end
	    while(1){
		  *(p+1)=*p;
		  if(p==s){break;}
		  p--;
	    }
	    *p='0';	  	  
	  }    
	}
	return s;
}

//=====================================
//
// 返回：*sOutput 
//       0:失败
   
// *sOutput=*pVal x res()
// sOutput: point to the data
//
// type=FMT_UNSIGNED_BYTES, return long
// type=FMT_MCC18_FLOAT, return float
//=====================================
/*
BYTE *Val2Str(BYTE *pVal,BYTE *sOutput,BYTE *cx)
{
float f0,*pf0;
long *lng;
//BYTE type,res;

	type=(*(cx+CX_REC_TYPE))>>4;
	res=*(cx+CX_RESOLUTION);
	DspFormat=*(cx+CX_DSP_FORMAT);

	if(type==FMT_ZERO){
	  *sOutput++='0';	  
	  *sOutput++=0x0;
	  return sOutput;	  
	}

//resolution=1:
    if(res==RESOLUTION_ONE){ 
	  if((type==FMT_UNSIGNED_BYTES)||(type==FMT_SIGNED_BYTES)){
		lng=(long *)pVal;
	    sprintf((char *)sOutput,"%li",*lng);	//ltoa(*lng,(char *)sOutput);
		return sOutput;
      }
      else if(type==FMT_MCC18_FLOAT){
        pf0=(float *)pVal;
		FloatToStr((char *)sOutput,*pf0,DspFormat);
		return sOutput;
      }
      else{return 0;}
	}
	
//resolution!=1, all below results are in FMT_MCC18_FLOAT	
	if((type==FMT_UNSIGNED_BYTES)||(type==FMT_SIGNED_BYTES)){
	  lng=(long *)pVal;	  	
      f0=((float)(*lng))*ResToFloat(res);
	}
	else if(type==FMT_MCC18_FLOAT){
	  pf0=(float *)pVal;      //pVal;   //(float *)pVal
	  f0=(*pf0)*ResToFloat(res);
	}
	else{
//	  ErrorReset(0x32);
      return 0;
	}		

	FloatToStr((char *)sOutput,f0,DspFormat);
	return sOutput;	
}
*/

//=========================================================
//  *seg转化为浮点数
//	转化前已保证 *seg&0xC0==0
//
//	seg: 指向数据元第一个字节       
//	return: OK,FAILED	
//=========================================================
BYTE Seg2Float(float *f,BYTE *seg,BYTE *cx)
{
BYTE len,vType,c,bits;
float k,b,x;

	len=*(cx+CX_VAL_BLEN);
	vType=(*(cx+CX_BELT_TYPE))>>4;
	bits=*(cx+CX_BITS);
	c=*(cx+CX_CORRECTION);
	k=*((float *)(cx+CX_K));
	b=*((float *)(cx+CX_B));


	switch(vType){
	  case FMT_COMPACT_U:		//紧凑格式
		DeCompactBelt((BYTE *)&x,seg,bits);	//正整数
		break;

	  case FMT_FLOAT_F:						//浮点格式
		DataCpy((BYTE *)&x,seg+1,4);		//skip v-header
		break;
	}


//修正
	switch(c){
	  case 0:		//不修正
		break;
	  case 1:		//线性修正
	  	*f=k*x+b;
		break;
	  case 2:		//非线性修正
//		*f=Correction(x);
		break;
	}
	return 1;
}


/*
//BYTE Seg2Val(BYTE *pVal,BYTE *pSeg,BYTE *cx)
//{
//long lng;
//BYTE sign;	//1:负数 0：正数
//float k,b;

//	len=*(cx+CX_VAL_BLEN);
//	type=(*(cx+CX_REC_TYPE))>>4;

//紧凑格式
//	if((*type==FMT_UNSIGNED_BYTES)||(*type==FMT_SIGNED_BYTES)){
//	  if(!RestoreBytes(pSeg,len,*type,&sign)){return FAILED;}  
//      lng=BytesToLong(lng,pSeg,len,&sign);
//	  DataCpy(pVal,(BYTE *)(&lng),4);	
//	  return OK;						//{pVal,FMT_LONG}
//    }

//通用格式
//    else if(*type==FMT_MCC18_FLOAT){
//	  DataCpy(pVal,pSeg,4);
//	  return OK;						//{pVal,FMT_MCC18_FLOAT}
//    }
//    else{return FAILED;}
//}
*/


//=======================================
//
//  {pData}={pData}-{pBase}
//=======================================
/*
void DeltaBase(BYTE *pData,BYTE *pBase,BYTE type)
{
long lData,lBase;
long fData,fBase;

	if((type==FMT_UNSIGNED_BYTES)||(type==FMT_SIGNED_BYTES)){
	  DataCpy((BYTE *)(&lData),pData,4);
	  DataCpy((BYTE *)(&lBase),pBase,4);
	  lData=lData-lBase;
	  DataCpy(pData,(BYTE *)(&lData),4);
	}
	else if(type==FMT_MCC18_FLOAT){
	  DataCpy((BYTE *)(&fData),pData,4);
	  DataCpy((BYTE *)(&fBase),pBase,4);
	  fData=fData-fBase;
	  DataCpy(pData,(BYTE *)(&fData),4);
	}
	else{
	}
}
*/	

//=======================================
// Get valid bits
// Omit unused bits
//=======================================
/*
void ValidBits(BYTE *pVal,BYTE *cx)
{
long mask;
char i;
BYTE *tmp;

	val_len=*(cx+CX_VAL_BLEN);
	type=(*(cx+CX_REC_TYPE))>>4;
	bits=(*(cx+CX_VALID_BITS))>>4;

	if(type==FMT_UNSIGNED_BYTES){
	  mask=1;
	  i=(val_len-1)*8+bits-1;
	  while((i--)!=0){mask=(mask<<1)+1;}
//
	  tmp=(BYTE *)(&mask);
	  for(i=0;i<4;i++){
		*pVal=(*Val)&(*tmp++);
		pVal++;
	  }
	}
}
*/

//=====================================================
// 从COMPACT格式恢复原始数据：
// 		[00aaaaaa 0bbbbbbb 0ccccccc 0ddddddd]
//  =>  [bbaaaaaa cccbbbbb ddddcccc 00000ddd]
//
// valid len={1:3}
// return : 0 failed
//			1 success
//
//Restore from
//    	[00aaaaaa 0aabbbbb 0bbbcccc 0ccccddd 0dddddee 0eeeeeef 0fffffff 0ggggggg 0ghhhhhh 0hhiiiii 0iiijjjj 0jjjjkkk 0kkkkkll 0llllllm 0mmmmmmm 0nnnnnnn 0n000000
//  =>	[aaaaaaaa bbbbbbbb cccccccc dddddddd eeeeeeee ffffffff gggggggg hhhhhhhh iiiiiiii jjjjjjjj kkkkkkkk llllllll mmmmmmmm nnnnnnnn
//=====================================================
void DeCompactBelt(BYTE *des,BYTE *src,BYTE cx_bits)
{
//==================
//''       修正
//''==================
//h(0) = ((Val("&H" + Mid(s, -1, 2)) * 2) Mod 256) + Int(Val("&H" + Mid(s, 1, 2)) / 64)     '无用，纯粹为了凑循环规律
//'
//h(1) = ((Val("&H" + Mid(s, 1, 2)) * 4) Mod 256) + Int(Val("&H" + Mid(s, 3, 2)) / 32)
//h(2) = ((Val("&H" + Mid(s, 3, 2)) * 8) Mod 256) + Int(Val("&H" + Mid(s, 5, 2)) / 16)
//h(3) = ((Val("&H" + Mid(s, 5, 2)) * 16) Mod 256) + Int(Val("&H" + Mid(s, 7, 2)) / 8)
//h(4) = ((Val("&H" + Mid(s, 7, 2)) * 32) Mod 256) + Int(Val("&H" + Mid(s, 9, 2)) / 4)
//h(5) = ((Val("&H" + Mid(s, 9, 2)) * 64) Mod 256) + Int(Val("&H" + Mid(s, 11, 2)) / 2)
//h(6) = ((Val("&H" + Mid(s, 11, 2)) * 128) Mod 256) + Int(Val("&H" + Mid(s, 13, 2)) / 1)
//'
//'''''''
//h(7) = ((Val("&H" + Mid(s, 15, 2)) * 2) Mod 256) + Int(Val("&H" + Mid(s, 17, 2)) / 64)
//h(8) = ((Val("&H" + Mid(s, 17, 2)) * 4) Mod 256) + Int(Val("&H" + Mid(s, 19, 2)) / 32)
//h(9) = ((Val("&H" + Mid(s, 19, 2)) * 8) Mod 256) + Int(Val("&H" + Mid(s, 21, 2)) / 16)
//h(10) = ((Val("&H" + Mid(s, 21, 2)) * 16) Mod 256) + Int(Val("&H" + Mid(s, 23, 2)) / 8)
//h(11) = ((Val("&H" + Mid(s, 23, 2)) * 32) Mod 256) + Int(Val("&H" + Mid(s, 25, 2)) / 4)
//h(12) = ((Val("&H" + Mid(s, 25, 2)) * 64) Mod 256) + Int(Val("&H" + Mid(s, 27, 2)) / 2)
//h(13) = ((Val("&H" + Mid(s, 27, 2)) * 128) Mod 256) + Int(Val("&H" + Mid(s, 29, 2)) / 1)
//'
//'''''''
//h(14) = ((Val("&H" + Mid(s, 31, 2)) * 2) Mod 256) + Int(Val("&H" + Mid(s, 33, 2)) / 64)
//h(15) = ((Val("&H" + Mid(s, 33, 2)) * 4) Mod 256) + Int(Val("&H" + Mid(s, 35, 2)) / 32)
//h(16) = ((Val("&H" + Mid(s, 35, 2)) * 8) Mod 256) + Int(Val("&H" + Mid(s, 37, 2)) / 16)
//h(17) = ((Val("&H" + Mid(s, 37, 2)) * 16) Mod 256) + Int(Val("&H" + Mid(s, 39, 2)) / 8)
//h(18) = ((Val("&H" + Mid(s, 39, 2)) * 32) Mod 256) + Int(Val("&H" + Mid(s, 41, 2)) / 4)
//h(19) = ((Val("&H" + Mid(s, 41, 2)) * 64) Mod 256) + Int(Val("&H" + Mid(s, 43, 2)) / 2)
//h(20) = ((Val("&H" + Mid(s, 43, 2)) * 128) Mod 256) + Int(Val("&H" + Mid(s, 45, 2)) / 1)
//'
//'''''''
//h(21) = ((Val("&H" + Mid(s, 47, 2)) * 2) Mod 256) + Int(Val("&H" + Mid(s, 49, 2)) / 64)
//h(22) = ((Val("&H" + Mid(s, 49, 2)) * 4) Mod 256) + Int(Val("&H" + Mid(s, 51, 2)) / 32)
//h(23) = ((Val("&H" + Mid(s, 51, 2)) * 8) Mod 256) + Int(Val("&H" + Mid(s, 53, 2)) / 16)
//h(24) = ((Val("&H" + Mid(s, 53, 2)) * 16) Mod 256) + Int(Val("&H" + Mid(s, 55, 2)) / 8)
//h(25) = ((Val("&H" + Mid(s, 55, 2)) * 32) Mod 256) + Int(Val("&H" + Mid(s, 57, 2)) / 4)
//h(26) = ((Val("&H" + Mid(s, 57, 2)) * 64) Mod 256) + Int(Val("&H" + Mid(s, 59, 2)) / 2)
//h(27) = ((Val("&H" + Mid(s, 59, 2)) * 128) Mod 256) + Int(Val("&H" + Mid(s, 61, 2)) / 1)
//'''''''
//h(28) = ((Val("&H" + Mid(s, 63, 2)) * 2) Mod 256) + Int(Val("&H" + Mid(s, 65, 2)) / 64)
//h(29) = ((Val("&H" + Mid(s, 65, 2)) * 4) Mod 256) + Int(Val("&H" + Mid(s, 67, 2)) / 32)

BYTE i,srcLen;
BYTE *p;

	srcLen=cx_bits/8;
	if((cx_bits%8)!=0){srcLen++;}

	i=0;
	p=src;
	int m=4,n=32;
	while(i<srcLen){
	  if((i+1)%7 == 0){
		p++;		//j+=4;
		m=2;
		n=64;
	  }

	  *des++=((*p)*m)+(int)(*(p+1)/n);
	  p++;
	  m*=2;
	  n=n/2;
	  i++;
	}
}


/*
BYTE RestoreBytes(float *f,BYTE *val,BYTE len)
{
BYTE v0,v1,v2;
BYTE b0,b1,b2;
BYTE *p;
UINT ui;
ULNG ul;
float f0;

	if(len==0){return 0;}

//[00xx-xxxx 0xxx-xxxx 0xxx-xxxx]
	if(len==1){return 1;}

	//len>=2
	p=val;
	v0=*p++;
	if(v0>0x3f){return 0;}
	v1=*p++;
	if(v1>0x7f){return 0;}
//
	if(len==2){
	  b0=(v1<<6)+v0;
	  b1=v1>>2;
	  ui=b1*256+b0;
	  f0=(float)ui;
	  *f=f0;
	  return 1;
	}
	else if(len==3){
	  v2=*p++;
	  if(v2>0x7f){return 0;}

	  b0=(v1<<6)+v0;
	  b1=(v2<<5)+(v1>>2);
	  b2=v2>>3;
	  ul=(b2*256+b1)*256+b0;
	  f0=(float)ul;
	  *f=f0;
	  return 1;
	}
	return 0;
}
*/


//============================
// 使long(4bytes)高位无用字节=0
//============================
/*
long BytesToLong(long lng,BYTE *pVal,BYTE len,BYTE *sign)
{

BYTE *p;
char i;
	
	p=(BYTE *)&lng;
	i=len;
    while(i){
	  *p++=*pVal++;  //低位赋值
	  i--;  
	}
	i=4-len;            //高位清零
	while(i){
	  *p++=0x0;
	  i--;
	}

	if(*sign==1){lng=-lng;}    
	return lng;	
}	
*/

//=================================
// copy string to ram not include 
// the terminating null charactor
// return: pointer to the ending null
//=================================
BYTE *str_to_ram(BYTE *dest,const BYTE *src)
{
	while(*src!=0){*dest++=*src++;}
	return dest;
}

//============================
// 获取c0首地址
//============================
UINT getC0(BYTE iC0)
{
BYTE c0_off;
UINT c0;

	c0_off=c0Table[iC0];				//基元偏移
	c0=((UINT)c0_off)*lenCELL;			//基元首地址
	return c0;
}

//============================
// 获取cx首地址
//============================
UINT getCX(BYTE iC0,BYTE iCx)
{
BYTE cx_off;	//,lenC0;
UINT cx_start;

	cx_off=cxTable[iC0];
	cx_start=((UINT)cxTable[cx_off+iCx])*lenCELL;
	return cx_start;
}

//==================================
// compare with 'HiOfInt(ms_click)'
//==================================
BYTE IsTimeUp(UINT iCmp)
{
int delta;
  	delta=(int)(ms_click-iCmp);
	if(delta>=0){return TRUE;}
	return FALSE;
}


//=================================
// Seaching for the pointer which 
// pointing to SIGN_DEFAULT('~'). 
// begin from 'p'
//=============================
BYTE *GetDefaultValue(BYTE *p)
{
BYTE i;
	i=0;
	while(*p!='{'){
	  if(i>=0x20){return 0;}				//limit the length
	  p++; i++;
	}				//'{' found

	p++;
	if((*p!=SIGN_DIGIT)&&(*p!=SIGN_OPTION)){return 0;}		//confirm the correct format
	while(*p!=SIGN_DEFAULT){
	  p++; i++;
	}				//'~' found
	return p;
}

//==============================
// 指针移到第一个s的位置
//
// limit: 
//	string搜索限制{0:limit-1}
//==============================
BYTE *InStr(BYTE *string,const char *s,UINT limit)
{
BYTE *pStr;			//,t;
const char *s0;
UINT L;

	L=strlen(s);
	if(L>limit){return 0;}

	s0=s;
	while(limit--)
	{
	  if(*string!=*s){
		string++;
	  }
	  else{
		pStr=string;
		string++; s++;
		while(*s!=0){
		  if(*string++!=*s++){s=s0; goto INSTR_CONTINUE;}
		}
		return pStr;
	  }
INSTR_CONTINUE:
	  continue;
	}
	return 0;
}


BYTE *InStrChar(BYTE *string,BYTE c,UINT limit)
{
	while(limit--){
	  if(*string!=c){string++;}
	  else{return string;}
	}
	return 0;
}

//===========================================
//                  Get value
// input:
//		'0'-'9','A'-'F','a'-'f'
// output:
//		0x0-0x9,0x0a-0x0f
//
// !!!注意与X()的区别: 
//        在处理与外部数据接口时(如GPRS接收)，
//    应采用ValueHex()。
//===========================================
BYTE ValueHex(BYTE c)
{
	if(c<='9'){
	  c=c-'0'; 
	}
	else if(c>='a'){
	  c=c-'a'+10;
	}
	else{
	  c=c-'A'+10;
	}
	return c;
}

//===========================================
// "12"=>0x12
//===========================================
BYTE ValueHex2(BYTE *p)
{
BYTE c;

	c=ValueHex(*p++);
	c=(c<<4)+ValueHex(*p);
	return c;
}


//==================================
// begin:
//	REG0: 定位MAIN_xxxx,C0_xxxx,CX_xxxx
//  0x0:	  定位MAP_xxx
//==================================

BYTE WriteCfg(BYTE val,UINT uAddr)
{
	mem_WriteData(M_CFG,&val,uAddr,1);			//save MAX_SEGMENTS
	return 1;
}


void MsgBox(const char *msg)
{
	lcd_clear();
	lcd_puts(0,L0,msg);     
}

/*UINT AddressOfCxBegin(BYTE iCx)
{
BYTE i;
UINT uAddr;

	i=GetByte(M_CFG,MAP_CELLX_BEG);
	uAddr=REG0+(((UINT)(i+iCx-1))*64);
	return uAddr;
}*/

BYTE GetByte(BYTE Slave,UINT src_addr)
{
BYTE i;
	mem_ReadData(Slave,(ULNG)src_addr,&i,1);
	return i;
}


//=================================
// 	 对MEM中的UINT进行计数运算
//=================================
void ReadCount(UINT uMap,UINT *count)
{
UINT cnt[2],mirror;

	mem_ReadData(M_MEM,(ULNG)uMap,(BYTE *)&cnt[0],4);
	mirror=~cnt[1];
	if(cnt[0]==mirror){		//校验		//发现这样直接比较会出错，为什么？	
	  *count=cnt[0];
	  return;
	}
	else{
	  cntMemMirrorFailed++;	//镜像在WriteCount中将自动修复！
	  return;
	}
}	

void WriteCount(UINT uMap,UINT count)
{
UINT cnt[2];
	cnt[0]=count;
	cnt[1]=~count;
  	mem_WriteData(M_MEM,(BYTE *)&cnt[0],(ULNG)uMap,4);	
}

void IncCount(UINT uMap)
{
UINT count;

	ReadCount(uMap,&count);
//	count++;
	WriteCount(uMap,++count);	//镜像在WriteCount中将自动修复！
}

//=================================
// 清零
//=================================
BYTE ClrByte(BYTE Slave,UINT src_addr)
{
BYTE i;

	i=0;
	mem_WriteData(Slave,&i,src_addr,1);
	return 1;
}

//==============================
// 清除CFG中,某个字的某一位
//==============================
BYTE ClrCfgBit(UINT addr,BYTE ibit)
{
BYTE c;

	c=GetByte(M_CFG,addr);
  	clrbit(c,ibit);
	WriteCfg(c,addr);
	c=GetByte(M_CFG,addr);
	return 1;
}

//==============================
// 清除CFG中,某个字的某一位
//==============================
BYTE SetCfgBit(UINT addr,BYTE ibit)
{
BYTE c;
	c=GetByte(M_CFG,addr);
  	setbit(c,ibit);
	WriteCfg(c,addr);
	return 1;
}

//=========================================
//                分割字符串
//=========================================
BYTE Split(const char *Str,BYTE *miniStr)
{
BYTE i,L;			
BYTE *p;			
BYTE numCmp;	//字符串的个数

	p=miniStr;
//	p=(BYTE *)&sFind[0];
	i=0;
	L=strlen(Str);
	while(L--){
	  if(*Str!='|'){*p++=*Str;}
	  else{
		*p='\0';
//		p=(BYTE *)&sFind[++i];
		p+=MINI_STR_LIMIT;
	  }
	  Str++;
	}
	*p='\0';
	numCmp=i+1;		//i: 字符串的个数
	return numCmp;
}

//==================================
// 保存版本号入Cfg(MAIN_VERSION)
//==================================
/*
void SaveVersionToCfg(void)
{
char vs[20];
BYTE i;
char vYear,vMon,vDay;
char cVer[2];

	strcpy(&vs[0],&version[0]);
//
	for(i=5;i<=10;i++){
	  vs[i]-='0';
	}
//
	vYear=vs[5]*10+vs[6];
	vMon=vs[7]*10+vs[8];
	vDay=vs[9]*10+vs[10];
//
	cVer[0]=(vYear<<1)+	(vMon>>3);
	cVer[1]=(vMon<<5)+vDay;
//
	mem_WriteData(M_CFG,(BYTE *)&cVer[0],REG0+(UINT)mapFIRMWARE_VER,3);
}
*/

void TransmitStr(BYTE *p)
{
	while(*p!=0){Serial_Transmit(*p++);}
}


//===================================================================
//Write bytes to Serial sensor
//===================================================================
void TransmitBytes(BYTE *buf,BYTE len)
{
	do{
	  Serial_Transmit(*buf++);
	}while(--len);	
}

//========================
// 复位外部设备（PW_NON）
//========================
void ResetPeri(BYTE sec)
{
char t[6];

#ifdef SIM_ON
	return;
#endif

//保存lcd控制端状态
	t[0]=LCD_TX_DIR;		
	t[1]=LCD_RX_DIR;
	t[2]=LCD_TX;
	t[3]=LCD_RX;

	LCD_TX_DIR=IN;			//置输入,防止暂态冲击
	LCD_RX_DIR=IN;
	LCD_TX=1;
	LCD_RX=1;

//复位外围设备
	PW_NON_off(); 			//reset all peripheral
	Delay_s(sec);				
	PW_NON_on(); 
	Delay_s(1);				

//恢复lcd控制端状态
	LCD_TX_DIR=t[0];		
	LCD_RX_DIR=t[1];
	LCD_TX=t[2];
	LCD_RX=t[3];
}

//=======================================
// 检测到至少1ms宽度的高电平
//=======================================
BYTE IsSensorReady(void)			//最多400ms
{
#ifdef WATCHDOG_ON
	ClrWdt();	
#endif

//临时
return TRUE;
//临时end

	if(!SensorInt){return FALSE;}
	Delay_ms(1);
	if(!SensorInt){return FALSE;}	//检测到至少1ms宽度的高电平
	return TRUE;
}


void ResetSensor(void)
{
//#ifdef SENSOR_RESET_PERMITTED
	SensorOff();
	Delay_s(10);
	SensorOn();
//#endif
}


void Restart(BYTE RstCode)
{
	ResetCode=RstCode;
	Nop();
	asm("reset");
}

//=====================================
//
//           定位label字符串
//
// pXSTR: 起始指针；
// 返回：p指向'"'
//=====================================
BYTE *FindParaStr(BYTE *pXSTR,const char *label)
{
BYTE *p;

	p=InStr(pXSTR,label,lenCELL*10);
	if(p){
	  p=InStrChar(p,'"',lenCELL);		//找到起始“
	}
	return p;		//指向“
}

void set_RecordStartTime(hTIME dt)
{
    //记录开始时间设置
    if(RC_START_SET != GetByte(M_CFG,MAP_REG_RC_START_FLG))
    {
        
        PRINTF("Record StartTime flag in CFGREG!\r\n" );

        WriteCfg(dt.year,MAP_REG_RC_START_TIME+YEAR);
        WriteCfg(dt.month,MAP_REG_RC_START_TIME+MONTH);
        WriteCfg(dt.day,MAP_REG_RC_START_TIME+DAY);
        WriteCfg(dt.hour,MAP_REG_RC_START_TIME+HOUR);
        WriteCfg(0x00,MAP_REG_RC_START_TIME+MINUTE);
        WriteCfg(0x00,MAP_REG_RC_START_TIME+SECOND);
        
        WriteCfg(RC_START_SET,MAP_REG_RC_START_FLG);
    }
}


/******************************************************************************
// 获取虚元卡记录时间初值
[YYMMDDHHMMSS]        记录起始时间，对应[REC0]
******************************************************************************/
void get_RecordStartTime(hTIME *dt)
{
BYTE *p;

//记录开始时间设置
	p=CMX(CMX_MR_YEAR);
/*	dt->year  =*p++;
    dt->month =*p++;
    dt->day   =*p++;
    dt->hour  =*p++;
    dt->minute=*p++;
    dt->second=*p;		*/
//
	dt->year  =14;
    dt->month =10;
    dt->day   =1;
    dt->hour  =0;
    dt->minute=0;
    dt->second=0;
}


