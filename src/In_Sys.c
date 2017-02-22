#include 	"header.h"
#include    "h_macro.h"

BYTE *BELTs(BYTE iC0,BYTE *des);
//BYTE Val2BELT(BYTE *des,BYTE bits,BYTE *src);
BYTE LenOfBits(BYTE bits);


extern BYTE cfg[];
extern BYTE c0Table[],cxTable[];


//=================================================
//  为了降低普通传感器接入复杂度，一般格式号：#n=1
//
//=================================================
BYTE BeltBeRead(BYTE iC0,BYTE *des)
{
UINT iCRC;		
BYTE *p,*p0;
UINT belt_len,frame_len;
      
//clr buffer
	FillBufWith(des,64,0x0);       

//header
    *(des+FRM_VER)=INST_HEADER;
//  *(des+FRM_LEN)=;  				//calculated length later
//    *(des+FRM_DES_ADDR)=0;
//    *(des+FRM_DES_ADDR+1)=0;
//    *(des+FRM_DES_ADDR+2)=0;        
//    *(des+FRM_DES_ADDR+3)=0;
//source
    *(des+FRM_SRC_ADDR)=0;
    *(des+FRM_SRC_ADDR+1)=0;
    *(des+FRM_SRC_ADDR+2)=0;        
    *(des+FRM_SRC_ADDR+3)=0;
//
	*(des+FRM_OP)=OP_READ_BELT;
	*(des+FRM_PROPERTY_L)=0x01;		//debug（无后续）（无级联）（时标有效）（需应答）（应答）
	*(des+FRM_PROPERTY_H)=0x00;		//debug（无后续）（无级联）（时标有效）（需应答）（应答）

	*(des+FRM_INST_INDEX)=0x00;
	*(des+FRM_INST_INDEX+1)=0x00;
	*(des+FRM_INST_INDEX+2)=0x00;
	*(des+FRM_INST_INDEX+3)=0x00;

	*(des+TSEN_BELT_CFG_INDEX)=0;
	*(des+TSEN_BELT_CFG_INDEX+1)=0;

	*(des+TSEN_BELT_PSW)=0;
	*(des+TSEN_BELT_PSW+1)=0;
	*(des+TSEN_BELT_PSW+2)=0;

	//1 byte Reserved 

	*(des+TSEN_BELT_MAC_ID)=0;
	*(des+TSEN_BELT_MAC_ID+1)=0;
	*(des+TSEN_BELT_MAC_ID+2)=0;
	*(des+TSEN_BELT_MAC_ID+3)=0;
	*(des+TSEN_BELT_MAC_ID+4)=0;
	*(des+TSEN_BELT_MAC_ID+5)=0;


//===============
	p=des+DATA0;
	p0=p;		//业务包起始地址

//skip,业务包长度，assign later
	p++;p++;
		
//#n:格式号
    *p++=1;		//默认格式号#n=1			 

//===============
//对基元（系统参数、内嵌传感器）作BELT处理
	  p=BELTs(iC0,p);	

//帧长度
	frame_len=(p+2)-des-1;
	*(des+FRM_LEN)=LoOfInt(frame_len);
	*(des+FRM_LEN+1)=HiOfInt(frame_len);

//CRC of header
    iCRC=GetCRC(des,lenHEAD-2);
    *(des+HCRC_L)=LoOfInt(iCRC);
    *(des+HCRC_H)=HiOfInt(iCRC);

//业务包belt长度
	belt_len=(UINT)(p-p0);
	*(des+FRM_BELT_LEN)=LoOfInt(belt_len);
	*(des+FRM_BELT_LEN+1)=HiOfInt(belt_len);

//CRC of DATA0
    iCRC=GetCRC(des+DATA0,belt_len);          
    *p++=LoOfInt(iCRC);
    *p++=HiOfInt(iCRC);

	return 1;
}



//===============================
// 串联UserBelt，建立BELTs
//
// 返回：指向BELTs后
//===============================
BYTE *BELTs(BYTE iC0,BYTE *des)
{
BYTE iCx,offset;
BYTE numCx;
UINT c0,cx;
BYTE *p0;
BYTE cx_val_len;
BYTE user[100];
BYTE cx_bits,des_len;

	UserBelt(iC0,&user[0]);		//原始数据

//========make belt========
	c0=getC0(iC0);			//基元首地址
	numCx=cfg[c0+C0_CXS];
	offset=0;
	p0=des;
    for(iCx=0;iCx<numCx;iCx++){		//NUM_OF_CELLS

	  //一旦越界，不再继续赋值
	  cx=getCX(iC0,iCx);
	  cx_val_len=cfg[cx+CX_VAL_BLEN];		//虚元长度
	  if((des+cx_val_len-p0)>32){break;}	//belt长度<=32

	  cx_bits=cfg[cx+CX_BITS];			//bits

	  des_len=Val2BELT(des,&user[offset],cx);
	  offset+=cx_val_len;
	  des+=des_len;
	}
	return des;
}

//======================================================================
// 将1个数据从自然格式 => BELT格式
//
//======================================================================
BYTE Val2BELT(BYTE *des,BYTE *src,UINT cx)
{
BYTE deslen;
BYTE vType,bits;

	vType=cfg[cx+CX_BELT_TYPE];
	bits=cfg[cx+CX_BITS];

	if(vType==FMT_COMPACT_U){
	  deslen=DoCompact(des,bits,src);
	}
	else{
	  deslen=cfg[cx+CX_VAL_BLEN];
	}
	return deslen;	
}


//======================================================================
//                   原始数据(二进制)转化为BELT格式
//
//      aaaaaaaa bbbbbbbb cccccccc dddddddd eeeeeeee ffffffff gggggggg
//   => 00aaaaaa 0bbbbbaa 0ccccbbb 0dddcccc 0eeddddd 0feeeeee 0fffffff 
//
//  bits: 有效bit;
//  src:  源数组;
//  des:  目标数组.
//
//  返回：结果数组*p的长度
//
//  BELT格式能包含特殊信息：错误及其类型等附加信息。
//======================================================================
BYTE DoCompact(BYTE *des,BYTE bits,BYTE *src)
{
BYTE deslen;
BYTE i,d;

//屏蔽无用的bits
	BitMask(bits,src);

//
	deslen=LenOfBits(bits);

	FillBufWith(des,deslen,0x0);	//des清零

	*des++=(*src)&0x3f;
	//if(--deslen==0){return deslen;}
	if(deslen==1){return deslen;}

	d=2;
	i=deslen-1;
	while(i--){
	  *des++=((*(src+1)<<d)+(*src>>(8-d)))&0x7f;
	  src++;
	  if(d==7){d=0;}
	  d++;
	}
	return deslen;

/*	((src+1)<<2+(src>>6))&0x7f;
	((src+1)<<3+(src>>5))&0x7f;
	((src+1)<<4+(src>>4))&0x7f;
	(src+1)<<2+(src>>6);
*/
}

//====================================================================
// BELT array => des 恢复到正常数组
// bLen：BLEN of cx
//
// 转换后,数组长度 <= 原长度
//
//    00aaaaaa 0bbbbbaa 0ccccbbb 0dddcccc 0eeddddd 0feeeeee 0fffffff 
// => aaaaaaaa bbbbbbbb cccccccc dddddddd eeeeeeee ffffffff gggggggg
//====================================================================
/*
void BELT2Val(BYTE *vSeg,BYTE *src,BYTE *cx)
{
BYTE *p;
BYTE bLen,Len;
char bits;

	bits=*(cx+CX_BITS);
	Len=0;

	p=src;
	tmp=*p;
	Len++;
	bits-=8;
	if(bits>0){			//bit>0时，需继续处理
	  while(bits>0){
		p++;
		Len++;
		if(Len==2){*p=(*p)<<6+(tmp&0x3f);}
		else{*p=(*p)<<5+(tmp>>2);}
//
	    if(bits<=8){LL0=LL0*64+*p; break;}
	    else{LL0=LL0*128+*p;}
		bits-=8;
	  }
	}

	if(Len>20){Len=20;}		//限制
	memcpy(vSeg,(BYTE *)&LL0,Len);
}
*/

/*
void BELT2Val(BYTE *vSeg,BYTE *src,BYTE *cx)
{
//double D0;		//因为采用了浮点数，超过4个字节的BELT，无法准确恢复。
unsigned long long LL0;		//最多8个字节
BYTE *p;
BYTE bLen,Len;
char bits;

	bits=*(cx+CX_BITS);
	Len=0;

	bLen=*(cx+CX_VAL_BLEN);
	p=src+bLen-1;
	LL0=*p; 
	Len++;
	bits-=8;
	if(bits>0){			//bit>0时，需继续处理
	  while(bits>0){
		p--;
		if(p<src){break;}
//
		Len++;
	    if(bits<=8){LL0=LL0*64+*p; break;}
	    else{LL0=LL0*128+*p;}
		bits-=8;
	  }
	}

	if(Len>20){Len=20;}		//限制
	memcpy(vSeg,(BYTE *)&LL0,Len);
}*/

//===================================
// 用bits去限制src数据
// 0b000011111111111111 & *src
//===================================
void BitMask(BYTE bits,BYTE *src)
{
BYTE i,r,mask;
BYTE *p;

	p=src;
	r=bits%8;
	i=bits/8;
	p=src+i;
	mask=0xff>>(8-r);
	*p=(*p)&mask;
}

//==============================
// 获取bits对应的byte长度：
//
// bits转化至belt后对应的字节长度
//==============================
BYTE LenOfBits(BYTE bits)
{
BYTE rlen;	//bits转化至belt后的长度
//BYTE i;

	if(bits==0){rlen=0; return rlen;}
	else{
	  rlen=1;
	  if(bits<=6){return rlen;}
	  //
	  bits-=6;
	  rlen+=(bits/7);
	  if(bits%7){		//非整数倍
		rlen++;
	  }
	}
	return rlen;
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
	if(delta>0){				//雨量累计值有变化
	  setbit(CellRequest0,1);	//Cx=1 ==> 雨量5min
//	  SensorInt();
	}
	oldRain=newRain;
	TaskLine.bitBroadcast=0;
}
*/

