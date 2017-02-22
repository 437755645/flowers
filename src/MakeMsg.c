#include 	"header.h"
#include    "h_macro.h"

//临时
//UINT		BCFG_LEN;	//BCFG长度

static UINT MakeBelt(BYTE fmt_id,BYTE *belt);		//返回BELT长度
static BYTE *BeltSelect(BYTE *des_belt,BYTE *src_belt,BYTE **sfmt);
static BYTE *GetFmt(BYTE fmt_id,BYTE *fmt);
static BYTE *ReadCxRecBelt(BYTE *belt,BYTE **sfmt);
static BYTE *AddCxBelt(BYTE *belt,UINT uAddr,BYTE CxBeltLen,UINT recTop,UINT recBottom);
static void MakeFrmHead(BYTE op,UINT lenMsg,BYTE *msg,UINT property);
static BYTE numMatched(BYTE *recDT);


extern BYTE		cfg[];
extern BYTE 	cxTable[];		//c0Table[]
extern PORT		Port[];			//支持最多8个基元(C0)
extern hTIME rTime;	//用于保存整点或整时间（如整15分，整5分）


//===========================================
//  在belt中选择sSel规定的（基）虚元，组成串
//  
// sSel samples:
//        !
//		C[001,101234567]	 ! 对应的是基元，后续为虚元。	
//
// 返回：
//     字符串结束位置
//===========================================
static BYTE *BeltSelect(BYTE *des_belt,BYTE *src_belt,BYTE **sfmt)
{
BYTE *f;	//格式指针
BYTE iC0,iCx;
UINT cx;
BYTE cx_len;

	f=*sfmt;
	iC0=X(*f++);					//基元
	while((*f!=',')&&(*f!=']')){	//只处理1个基元。
	  iCx=X(*f);					//虚元
	  cx=getCX(iC0,iCx);
	  cx_len=cfg[cx+CX_VAL_BLEN];
	  
	  memcpy(des_belt,src_belt,cx_len);
	  des_belt+=cx_len;
	  src_belt+=cx_len;
	  f++;
	}
	*sfmt=f;

	//belt_end=des_belt;
	return des_belt;
}



//===========================================
//
// 获取fmt字符串
//===========================================
static BYTE *GetFmt(BYTE fmt_id,BYTE *fmt)
{
BYTE *p;	//*p0;
BYTE sn[5];
UINT lenBCFG;

	sn[0]='#';
	btoa(fmt_id&0x0f,(char *)&sn[1]);				//最多16种格式。

	p=CMC(CMC_LINK);
	p=FindParaStr(p,"BCFG");
	p++;
	
	lenBCFG=getUINT(CMC(CMC_BCFG_LEN));
	p=InStr(p,(const char *)&sn[0],lenBCFG);		//寻找“#N”	

	if(p){
	  CopyParaStr(p,fmt,'#');
	}

	return p;
}


//===========================================
//
// Make message ready for sending
//
// belt's are joined together in bank3,
// and is to be moved to bank2
//
// Msg形成过程应尽可能短，并且不允许新记录，
// 否则可能出错
// 应禁止中断，慢的过程应优化或去处
//
//
// 				建立业务包
//
//===========================================
static UINT MakeBelt(BYTE fmt_id,BYTE *belt)		//返回BELT长度
{
BYTE *f;		//格式指针
BYTE fmt[128];			//fmt的最大长度	
BYTE *b0;
UINT lenBelt,sum;
BYTE r,iC0;		  
hTIME dt;
BYTE start,len,*p;
BYTE lenFmt;

	b0=belt;

//业务报长度
	belt+=2;	//make room for lenBelt

//#n:格式号
    *belt++=fmt_id;			

//获取格式str
	f=GetFmt(fmt_id,&fmt[0]);
	if(f==0){return 0;}			//指定格式不存在

//临时
//	strcpy((char *)&fmt[0],"#1T06C[001,101234567],R[11:20,15:30]");
	lenFmt=strlen((char *)&fmt[0]);

//=== T参数 ===
//T06
	f=InStr(&fmt[0],"T",lenFmt);
	if(f){
	  start=X(*(++f));		//f指向T后
	  len=X(*(++f));	

	  Now(&dt);
	  p=(BYTE *)&dt;
	  while(start--){
		p++;
	  }					//时间指针p移到起始点

	  while(len--){		//复制规定的长度
		*belt++=*p++;
	  }
	}

//=== C参数 ===
//C[001234,101234567]

	f=InStr(&fmt[0],"C[",lenFmt);
	if(f){
	  f+=2;			//指向端口号
	  while(*f!=']'){
		if(*f==','){f++;}	//skip
	    iC0=X(*f);
	    r=ReadPort(iC0,&Port[iC0].buf[0],MODE_NORMAL);		//belt_limit=64; speed_mode=MODE_NORMAL  
		belt=BeltSelect(belt,&Port[iC0].buf[0],&f);			//指向‘]’
		//belt=belt_end;
	  }	
	}

//=== R参数 ===
//R[12:20,26:30]			
	f=InStr(&fmt[0],"R[",lenFmt);
	if(f){
	  f+=1;
	  while(*f!=']'){
		f++;
	    belt=ReadCxRecBelt(belt,&f);		//仅处理1个虚元
		//belt=belt_end;
	  }	
	}

//保存长度
	lenBelt=belt-b0+2;			//2：CRC
	*b0=LoOfInt(lenBelt);		//SP长度：lenBelt,包括长度本身
	*(b0+1)=HiOfInt(lenBelt);

	sum=GetCRC(b0,lenBelt-2);
	*belt++=LoOfInt(sum);
	*belt=HiOfInt(sum);		

	return lenBelt;
}


//==========================================
//            
// 按sRCox指向的格式，读指定Cx记录,建立CxRecBelt
//
// 结果: 存放在belt开始的缓冲区
// 完成后
// 1)belt指向最后数据后面
// 2)p指向格式中','或']'
//
//（只处理1个格式Script）
//
// sRCox 举例：
//        !
// 		R[12:20,26:30]
//==========================================
static BYTE *ReadCxRecBelt(BYTE *belt,BYTE **sfmt)
{
BYTE *f,iC0,iCx;		
UINT uAddr,top,bottom;
BYTE Cx_off,cx_trail;		//,cx_len;
UINT cx;
BYTE len;

	f=*sfmt;
	iC0=X(*f++);				//基元
	Cx_off=cxTable[iC0];		//虚元首地址

	iCx=X(*f++);				//虚元
	cx=cxTable[Cx_off+iCx]*lenCELL;
	cx_trail=Val(++f,&len);		//p指向','或']'
	f+=len;
	*sfmt=f;

	top=getUINT(&cfg[cx+CX_REC_ENTRY])*REC_BLOCK_SIZE;			
	bottom=getUINT(&cfg[cx+CX_REC_EXIT])*REC_BLOCK_SIZE;

	uAddr=getRecPtr(iC0,iCx);	//含校验
	if((uAddr<top)||(uAddr>bottom)){
	  return belt;
	}			//指针超范围时，返回原belt	

	if(uAddr){
     //if(IsCxRecorded(uCX0)){		//CxRecord标记必须与top,bottom正确绑定
	  belt=AddCxBelt(belt,uAddr,cx_trail,top,bottom);
	}	
	//基元记录指针=0，或虚元不记录。如果uAddr=0，表示不记录
	//else{}
	
	return belt;
}


//=========================
// Calculate matched YMDHM
// 与rTime比较
//=========================
BYTE numMatched(BYTE *recDT)
{
BYTE iMatched;

	//recDT++;	//指向Y
	iMatched=0;
	if((*recDT++&0x3f)==rTime.year){
	  iMatched++;
	  if((*recDT++&0x3f)==rTime.month){
		iMatched++;
		if((*recDT++&0x3f)==rTime.day){
		  iMatched++;
		  if((*recDT++&0x3f)==rTime.hour){
		    iMatched++;
			if((*recDT++&0x3f)==rTime.minute){
			  iMatched++;
 	}}}}}
	return iMatched;
}



//============================================
//  	在belt上加入单个CxBelt记录
//
// 记录BELT格式: 
//        [iMatched][DT...][data...]
//
// DT: 		指与帧头中YMDHmS不匹配的部分，可变长；
// data: 	长度由"R[XX：LL]"描述。
// uAddr:	指向0x81,YMDHmS,0xBC. S暂时不用。. 
// CxBeltLen: 长度<=247
//
// 结束时：belt指向末尾后。
//============================================
static BYTE *AddCxBelt(BYTE *belt,UINT uAddr,BYTE CxBeltLen,UINT top,UINT bottom)
{
BYTE c,i,iMatched,len,save,RecLen;
BYTE *p;
BYTE buf[256];
BYTE *belt0;

	belt0=belt;

//关闭时钟中断,锁定记录,防止在读数据时,数据被改写.			
	save=CALENDAR_IE;
  	CALENDAR_IE=FALSE;	

	top=top*REC_BLOCK_SIZE;
	bottom=bottom*REC_BLOCK_SIZE;

	c=GetByte(M_MEM,uAddr);
	if(c==0xBF){goto END_ADD_BELT;}

//读取RecArray => buf[]
	RecLen=CxBeltLen+8;						//8:标签长度
	DecWith(uAddr,CxBeltLen,top,bottom);	//uAddr指向首部
	ReadRecArray(uAddr,&buf[0],RecLen,top,bottom);	//[......uAddr]
	if((buf[CxBeltLen]!=0x81)||(buf[CxBeltLen+7]!=0xBC)){
	  goto END_ADD_BELT;			//ERROR or NO RECORD
	}


//	if(p3[0]==0xBF){return FALSE; }					//no record
//	else if(p3[0]!=0x81){return FALSE;}				//error
	
	iMatched=numMatched(&buf[CxBeltLen+1]);

//【Make CxBelt

//[iMatched] 匹配数
	*belt++=iMatched;		//number of matched
	len=0;

//[Unmatched YMDHmS] 不匹配的部分
	i=iMatched;
	i++;
	p=&buf[CxBeltLen+i];
	while(i++<=5){
	  *belt++=*p++;		//忽略秒S
	  len++;
	}

//[...]
	Reverse(&buf[0],CxBeltLen);
	DataCpy(belt,&buf[0],CxBeltLen);

// [iMatched] [Unmatched-...DHm] [12-34-C1-C2-56-34-80958F918781-...]
//】

//=======================
END_ADD_BELT:
	//恢复时钟中断状态
  	CALENDAR_IE=save;	
	return belt0+CxBeltLen;
}



//==================================
// 返回：lenMsg
//==================================
UINT MakeMsg(BYTE protocol_id,BYTE fmt_id,BYTE *msg,UINT property)
{
UINT lenBelt;
BYTE *belt;

//临时
	protocol_id=PROTOCOL_YR_STD;

//==== 非YR协议，兼容性 ====
	if(protocol_id!=PROTOCOL_YR_STD){
	  return 0;
	}


//==== YRX 系列协议 ====
	else if(protocol_id==PROTOCOL_YR_STD){
	  belt=msg+32;
//建立业务包
	  lenBelt=MakeBelt(fmt_id,belt); 
//建立帧头
	  MakeFrmHead(OP_REAL_MSG,lenBelt+32,msg,property);
	  return lenBelt+32;
	}
	return 0;
}

void MakeFrmHead(BYTE op,UINT lenMsg,BYTE *msg,UINT property)
{
UINT sum;
ULNG li;

	*(msg+FRM_VER)=INST_HEADER;

	*(msg+FRM_LEN)=LoOfInt(lenMsg-1);		
	*(msg+FRM_LEN+1)=HiOfInt(lenMsg-1);

//source address
	memcpy((BYTE *)(msg+FRM_SRC_ADDR),&cfg[CM_STA_ID],4);

	*(msg+FRM_OP)=op;
	*(msg+FRM_PROPERTY_L)=LoOfInt(property);
	*(msg+FRM_PROPERTY_H)=HiOfInt(property);

//Unsolicited时，取0x8000(bit15=1)。区别于inst_index={0-0x7fff}
//	*(msg+FRM_INST_INDEX)=0;		//2 bytes
//	*(msg+FRM_INST_INDEX+1)=0x80;	

//MsgIndex++, 中心收到相同(StaID & MsgIndex)时，只取一条
	mem_ReadData(M_MEM,(ULNG)mapMSG_INDEX,(BYTE *)(msg+TCOM_MSG_INDEX),4);
	li=getULNG(msg+TCOM_MSG_INDEX); li++;		//+1,保存
	mem_WriteData(M_MEM,(BYTE *)&li,(ULNG)mapMSG_INDEX,4);

//CfgIndex
	mem_ReadData(M_CFG,(ULNG)CM_CFG_INDEX,(BYTE *)(msg+TCOM_CFG_INDEX),2);

	*(msg+TCOM_PSW)=0;				//3 bytes
	*(msg+TCOM_FAMILY_CO)=0;		//1 bytes
	*(msg+TCOM_MAC_ID)=0;			//6 bytes

	sum=GetCRC(msg,lenHEAD-2);
	*(msg+HCRC_L)=LoOfInt(sum);
	*(msg+HCRC_H)=HiOfInt(sum);		
}	


//=============================================
//
// Make Text message ready for sending
//txt: 
//	1. 以'/0'结尾
//  2. 长度不超过0x50,以保证能包含在一条短信内
//=============================================
BYTE MakeTextMsg(const char *sText,BYTE *msg,UINT property)
{
UINT sum;	
BYTE *p,txt_len;
BYTE lenSP;
BYTE *p0;

	txt_len=strlen((const char *)sText)+1;
//txt
  	p0=msg+DATA0;
	p=p0;
	*p++=txt_len;
	*p++=0;
	memcpy(p,sText,txt_len);
	p+=txt_len;
	lenSP=(BYTE)(p-p0);

	sum=GetCRC(p0,lenSP);
	*p++=LoOfInt(sum);
	*p=HiOfInt(sum);		

//建立帧头
	MakeFrmHead(OP_TEXT_MESSAGE,lenSP+32,msg,property);
	return lenSP+32;
}	
