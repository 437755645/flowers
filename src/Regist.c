#include 	"header.h"
#include    "h_macro.h"
//#include	"X_CFG.h"

#define		WR_FAILED	0
#define		WR_OK		1
#define		WR_NULL		2

//#define	INST_CFG_CRC 	34		//32+2;
//#define	INST_CFG_INDEX	36		//32+4;

//#define WaitInst(connection_type) 		GetResp(connection_type)

BYTE	stepGET_RESP;

extern ERRORS	Error0;
extern MSG1		Msg1;

extern BYTE 	IPL_CPU;
extern BYTE 	cfg[];

extern BYTE 	ActiveCom;
extern BYTE		ModemReady[];
 	
extern BYTE 	MacID[];
//BYTE mac_id[]={0x03,0x90,0x48,0x65,0xE7,0x38};


BYTE SaveMap(BYTE *inst)		
{
BYTE op;
	op=*(inst+FRM_OP);
	switch(op){
	  case OP_WRITE_AREA:		//写入连续数据
	    SaveBlock(inst);
	    break;

	  case OP_WRITE_SEGS:		//写入离散数据
		SaveSegs(inst);
		break;

	  default:
		break;
	}
	return 1;
}


//=================================================
//               保存Segs:离散数据
// MAX: 1k
//
//=================================================
BYTE SaveSegs(BYTE *inst)		
{
UINT block_len;		//受rx[]缓冲大小限制，业务包最大1k
BYTE MapType,PageUnit;
ULNG Pages,ulDes;
ULNG PageIndex;
BYTE *src,*src_save;
BYTE r;

	ClrWdt();




//Inst
	MapType=*(inst+TSEN_RW_MAP_TYPE);		
	PageIndex=0;        //getULNG(inst+TSEN_RW_PAGE_INDEX);		//或StartAddr
	Pages=0;            //getUINT(inst+TSEN_RW_PAGES);				//或字节数

	PageUnit=0;         //*(inst+TSEN_RW_PAGE_UNIT);
	block_len=0;        //GetPageSize(inst);

	src=inst+DATA0+2;		//Pack的起始地址

switch(MapType){

	  case M_CFG:		//写入Mirror
		src+=4;			//Skip CfgCRC,CfgIndex



		ulDes=(ULNG)PageIndex*block_len+CFG_BUF0;		//指向Mirror						
		if(PageUnit==0xff){
		  block_len=getUINT(inst+32)-4;		//此时，Pages代表字节数,去除CfgCRC,CfgIndex
		}
	    break;
		
	  case M_MEM:		//写入MEM
	  case M_XME:		//写入XME
		ulDes=(ULNG)PageIndex*block_len;						
		if(PageUnit==0xff){block_len=Pages;}	//此时，Pages代表字节数
	    break;
		
	  default:
	    break;
	}
	src_save=src;
	mem_WriteData(MapType,src,ulDes,block_len);	
	Delay_ms(200); 			//是否必要?

//
	if(MapType==M_CALENDAR){return OK;}
	else{		//M_CFG
	  src=src_save;
	  r=mem_CheckWriting(MapType,src,ulDes,block_len);	
	  Delay_ms(200);			//是否必要?
	}

	if(r){return OK;}
	else{				//出错陷阱
	  fERR_FRAM_SAVE=1;
      while(1){Nop();}
	}
}

//==========================================
//  获取SPackl中字符串部分
// 返回：字符串长度
//==========================================
UINT getPackStr(BYTE *inst,BYTE *pack_str)
{
BYTE *p;
UINT uLen;
BYTE pp;

	pp=*(inst+FRM_PROPERTY_H);
	if(!testbit(pp,bitSPACK)){		//||!testbit(pp,bitXPARA)){
	  return 0;
	}

	p=inst+32+2;		//32：帧头长度；2：业务包长度
//	if(uLen>256){return 0;}
//	memcpy(pack_str,p,uLen);
	strcpy((char *)pack_str,(const char *)p);
	uLen=strlen((char *)p);
	return uLen;
}


//==========================================
// "SLU=SSSSSSSSLLLLUU"
//==========================================
BYTE getSLU(BYTE *inst,ULNG *Start,UINT *Length,BYTE *Unit)
{
BYTE x[256],*p;
UINT uLen;
	
	uLen=getPackStr(inst,&x[0]);
	if(uLen==0){return FAILED;}

	p=InStr(&x[0],"SLU=",uLen);
	if(!p){return FAILED;}
//
	p+=4;
	*Start=(((((ValueHex2(p)<<8)+ValueHex2(p+2))<<8)+ValueHex2(p+4))<<8)+ValueHex2(p+6);
	p+=8;
	*Length=(ValueHex2(p)<<8)+ValueHex2(p+2);
	p+=4;
	*Unit=ValueHex2(p);
	return 1;
}

//==========================================
// "DT=YYMMDDHHmmSS"
//==========================================
BYTE getDT(BYTE *inst,hTIME *dt)
{
BYTE x[256],*p;
UINT uLen;
	
	uLen=getPackStr(inst,&x[0]);
	if(uLen==0){return FAILED;}

	p=InStr(&x[0],"DT=",uLen);
	if(!p){return FAILED;}
	p+=3;
//
	dt->year=ValueHex2(p);	//dt->year=0x0E; 
	p+=2;
	dt->month=ValueHex2(p);	//dt->month=6; 
	p+=2;
	dt->day=ValueHex2(p);	//dt->day=18; 
	p+=2;
	dt->hour=ValueHex2(p);	//dt->hour=17; 
	p+=2;
	dt->minute=ValueHex2(p);	//dt->minute=47; 
	p+=2;
	dt->second=ValueHex2(p);	//dt->second=30;

	return OK;
}

//==========================================
// "DEST=dddd"
//==========================================
BYTE getDEST(BYTE *inst)
{
	return 1;
}

//============================
// Skip参数字符串
//============================
BYTE *SkipParaStr(BYTE *inst,BYTE *p)
{
int lenStr;
BYTE c;

	c=*(inst+FRM_PROPERTY_H);
c=0x15;
	if(testbit(c,bitSPACK)==FALSE){
	  return p;
	}		//不变
	if(testbit(c,bitXPARA)==FALSE){
	  return p;
	}		//不变

	lenStr=strlen((char *)p);
	if(lenStr>256){
	  return p;
	}		//XPara字符串长度不超过256
	return p+lenStr+1;
}

//=================================================
//               保存Block:连续数据
// MAX: 1k
//
//=================================================
BYTE SaveBlock(BYTE *inst)		
{
UINT block_len;		//受rx[]缓冲大小限制，业务包最大1k
BYTE MapType,PageUnit;
ULNG ulDes;
ULNG PageIndex;
BYTE *src0,*src,*src_save;
BYTE r;
UINT Pages;
UINT lenSPack;

	ClrWdt();

//Inst
	MapType=*(inst+TSEN_RW_MAP_TYPE);
//MapType=M_CFG;
	if(getSLU(inst,&PageIndex,&Pages,&PageUnit)==FAILED){
	  return 0;
	}		
//	PageIndex=getULNG(inst+TSEN_RW_PAGE_INDEX);		//或StartAddr
//	Pages=getUINT(inst+TSEN_RW_PAGES);				//或字节数

//	PageUnit=*(inst+TSEN_RW_PAGE_UNIT);
	block_len=GetPageSize(inst,Pages,PageUnit);

	src0=inst+DATA0+2;				//src0:SPack的起始地址; 2:长度
	src=SkipParaStr(inst,src0);		//二进制block
	lenSPack=getUINT(inst+32);
	switch(MapType){
	  case M_CFG:		//写入Mirror
		src+=4;			//Skip CfgCRC,CfgIndex
		ulDes=(ULNG)PageIndex*block_len+CFG_BUF0;		//指向Mirror						
		if(PageUnit==0xff){
		  block_len=lenSPack-(src-src0);		//此时，Pages代表字节数,去除CfgCRC,CfgIndex
		}
	    break;
		
	  case M_MEM:		//写入MEM
	  case M_XME:		//写入XME
		ulDes=(ULNG)PageIndex*block_len;						
		if(PageUnit==0xff){block_len=Pages;}	//此时，Pages代表字节数
	    break;
		
	  default:
	    break;
	}
	src_save=src;
	mem_WriteData(MapType,src,ulDes,block_len);	
	Delay_ms(200); 			//是否必要?

//
	if(MapType==M_CALENDAR){return OK;}
	else{		//M_CFG
	  src=src_save;
	  r=mem_CheckWriting(MapType,src,ulDes,block_len);	
	  Delay_ms(200);			//是否必要?
	}

	if(r){return OK;}
	else{				//出错陷阱
	  fERR_FRAM_SAVE=1;
      while(1){Nop();}
	}
}


//=================================================
// Upload Map to REG-Server/Be read by terminal
//
// TREG
// 适用于DIRECT、GPRS
//
//=================================================
BYTE UpLoadMap(BYTE connection_type)		
{
signed char no_response_count;
int block_size;	//采用int是为了便于越界判断
BYTE MapType,t,r;
ULNG ul_addr;
UINT ipl_save;
long lStart;

ULNG iPage;
UINT Pages;
BYTE Unit;
BYTE *rx;

	di(ipl_save,IPL_T2_INT-1);		//允许T2_INT中断	
//	SetIPL(INT_CPU,IPL_T2_INT-1);	//允许T2_INT中断

	rx=RX_BUF0();

	if(TREG_Authentication(rx,connection_type)==FAILED){
	  r=STEP_AUTHEN_FAILURE;
	  goto END_UPLOAD;
	}
	//if OK => 回执中含新指令（下行读）

//

	if(getSLU(&rx[0],&iPage,&Pages,&Unit)==FAILED){
	  r=RX_OVER_WITH_ERR_FMT;					//找不到预期的参数
	  goto END_UPLOAD;
	}		

	no_response_count=0;
	MapType=rx[TSEN_RW_MAP_TYPE];
	do{
	  block_size=GetPageSize(&rx[0],Pages,Unit);		//发送块的大小

  	  lStart=GetStartAddr(&rx[0],block_size);
	  if(lStart>=0){
		ul_addr=(ULNG)lStart;
	  }
	  else{
		r=STEP_UPLOAD_FAILURE;
		goto END_UPLOAD; 
	  }

	  if((Unit==0xff)		//全被读
		||(Unit==0x0)){		//字节被读，MAX：32k
		Pages=1; iPage=1;					//只循环1次
	  }
/*	  else{		//分页读
		Pages=getUINT(&rx[TSEN_RW_PAGES]);
		iPage=getULNG(&rx[TSEN_RW_PAGE_INDEX]);			
	  }*/
//
	  TranmitHead(block_size,&rx[0]);			//上载
	  r=TransmitPage(MapType,ul_addr,block_size,&rx[0]);
	  if(r==FAILED){r=STEP_ERR_CFG; goto END_UPLOAD;}

	  stepGET_RESP=0;					//确认响应
	  do{
		ClrWdt();
		t=GetResp(connection_type);		//此处：返回中含指令
	  }while(t==RX_WAIT);


      if((t!=RX_OVER_OK)||(rx[FRM_RET]!=OK)){
		goto ERR_UPLOAD;
      }	
	}while(++iPage<Pages);

	r=STEP_UPLOAD_OVER;
END_UPLOAD:
	ei(ipl_save);			//enable all interrupts after registration
	return r;

ERR_UPLOAD:
//异常
	/*if(t==RX_OVER_TIME_OVERFLOW){				//超时，无回音	  
	  if(no_response_count<3){				//超过3次，退出
	    no_response_count++;		
	  }
	  else{r=STEP_READ_MAP_FAILURE; }
	}*/
	if(t==RX_OVER_TIME_OVERFLOW){r=STEP_UPLOAD_FAILURE;}	//超时，无回音	  
	else if(t==RX_OVER_WITH_ERR){r=STEP_UPLOAD_FAILURE;}
	else if(t!=RX_OVER_OK){r=STEP_UPLOAD_FAILURE;}
	goto END_UPLOAD;
}

//================================
// 获取StartAddr
//
// PageIndex: 起始页号 或 起始地址
//================================
long GetStartAddr(BYTE *inst,int page_size)
{
long lStart;
ULNG PageIndex;
UINT Pages;
BYTE PageUnit;

//	PageIndex=getULNG(inst+TSEN_RW_PAGE_INDEX);	//或 StartAddr
//	PageUnit=*(inst+TSEN_RW_PAGE_UNIT);

	if(getSLU(inst,&PageIndex,&Pages,&PageUnit)==FAILED){
	  return -1;					//找不到预期的参数
	}

	if(PageUnit==0xff){		//读全部AREA的字节
	  lStart=0;
	}
	else if(PageUnit==0x0){	//读数个字节数
	  lStart=(long)PageIndex;
	}
	else{					//读1页
	  lStart=(long)PageIndex*page_size;
	}
	return lStart; 
}


//================================
// 获取Page的大小
//================================
UINT GetPageSize(BYTE *inst,UINT Pages,BYTE PageUnit)
{
UINT page_size;
BYTE MapType;

	MapType=*(inst+TSEN_RW_MAP_TYPE);			
//MapType=M_CFG;

	if(PageUnit==0xff){		//一次读全部AREA的字节
	  switch(MapType){
	    case M_CFG:		//读取CFG时，上传全部CFG
	      page_size=lenCELL*GetByte(MapType,CM_LEN_CFG);
		  break;
		default:
		  page_size=1024;	//默认的PageSize
		  break;
	  }
	}
	else if(PageUnit==0x0){	//读数个字节数
	  page_size=Pages;
	}
	else{					//读1个Page
	  page_size=PageSize(PageUnit);
	}
	return page_size;
}

//==============================
//获取页大小，一般情况
//==============================
UINT PageSize(BYTE PageUnit)
{
UINT page_size;

	page_size=1;
	while(PageUnit--){page_size*=4;}
	return page_size;
}



BYTE TREG_Authentication(BYTE *inst,BYTE connection_type)
{
	if(connection_type==CONNECT_DIRECT){return OK;}

//检查Modem状态
	if(IsModemReady()==FALSE){
	  AT_creg(ActiveCom);		//马上刷新，保证时钟停振时，也能响应	
	  if(IsModemReady()==FALSE){
		return FAILED;
	  }
	}

	if(Connect(CONNECT_GPRM)==FAILED){return FAILED;} 

//连接REG服务器
	ConnectTcpServer((BYTE *)("122.224.172.53"),(BYTE *)("5005"));		//(IP_REG_SERVER);


//身份校验
	if(Authen(inst,connection_type)==FAILED){
	  return FAILED;
	}
	return OK;
}


//===========================================
// 从inst中提取iC0,iCx
//===========================================
BYTE getiC0iCx(BYTE *iC0,BYTE *iCx,BYTE *inst)
{
BYTE x[256],*p;
UINT uLen;
	
	uLen=getPackStr(inst,&x[0]);
	if(uLen==0){return FAILED;}

	p=InStr(&x[0],"C0CX=",uLen);		//"C0CX=1F": iC0=1;iCx=15,X字符
	if(!p){return FAILED;}
	p+=5;

	*iC0=X(*p++);
	if(*iC0==0xFF){return FAILED;}

	*iCx=X(*p);
	if(*iCx==0xFF){return FAILED;}

	return OK;
}


//====================================================
//  发送一页(含小于1页)
//
// 受page_size限制，max:32k/page
//====================================================
BYTE TransmitPage(BYTE MapType,ULNG addr0,int page_size,BYTE *inst)
{
BYTE 	r,i,size;
BYTE 	reg[64];
UINT 	sum;
ULNG 	ul_addr;
BYTE 	iC0,iCx;
BYTE 	fsName[13];
BYTE 	ShortFn[14]={"\\"};


//应锁定中断，防止发送区被改变；


//获取文件名
	if(MapType==M_CARD){
	  r=getiC0iCx(&iC0,&iCx,inst);
	  if(r==FAILED){return FAILED;}
	  getShortName(iC0,iCx,&fsName[0]);
	  strcat((char *)&ShortFn[0],(const char *)&fsName[0]);		//加上“\”
	}

//开始发送
	Serial_Transmit(LoOfInt(page_size));
	Serial_Transmit(HiOfInt(page_size));

	sum=LoOfInt(page_size)+HiOfInt(page_size);

	ul_addr=addr0;
	while(page_size>0){
	  //确定size				
	  if(page_size>=lenCELL){size=lenCELL;}
	  else{size=page_size;}

	  //读入>>reg[]
	  if(MapType==M_CALENDAR){				
		R4571_Read_Data(ADDR_CLOCK0,&reg[0],size);//时钟已经修改
	  }
	  else if(MapType==M_CARD){
		CardReadRec(ul_addr,size,&reg[0],&ShortFn[0]);
	  }
	  else{
	    mem_ReadData(MapType,ul_addr,&reg[0],size);	//datas ready now with CRC in last word
	  }

	  //发送
	  for(i=0;i<size;i++){
		Serial_Transmit(reg[i]);
		sum+=reg[i];
	  }
	  ul_addr+=size;
	  page_size-=size;
	}
	  	  
//可考虑在此处释放中断，允许记录事件发生。
	Serial_Transmit(LoOfInt(-1-sum));
	Serial_Transmit(HiOfInt(-1-sum));
	Delay_ms(100);
	return OK;
}

//============================================
//                用于上载Map
//============================================
void TranmitHead(UINT MapSize,BYTE *inst)
{
BYTE Msg[32],*p;
BYTE SrcAddr[]={0xE8,0x03,0,0};
UINT len_pack,sum;
BYTE Op;
//UINT Pages;

	FillBufWith(&Msg[0],32,0x88);

	Op=*(inst+FRM_OP);
	if(Op==OP_READ_AREA){
	  Msg[FRM_VER]='H';

	  len_pack=32+MapSize+2+2;
	  len_pack--;
	  Msg[FRM_LEN]=LoOfInt(len_pack); 
	  Msg[FRM_LEN+1]=HiOfInt(len_pack);	

//	  Msg[FRM_DES_ADDR]=0; Msg[FRM_DES_ADDR+1]=0;
//	  Msg[FRM_DES_ADDR+2]=0; Msg[FRM_DES_ADDR+3]=0;

	  memcpy(&Msg[FRM_SRC_ADDR],&SrcAddr[0],4);

	  Msg[FRM_OP]=Op;
	  Msg[FRM_PROPERTY_L]=0b00010011; 

	  Msg[FRM_PROPERTY_H]=0b00000001;
	  setbit(Msg[FRM_PROPERTY_H],bitSPACK);		//含SPACK
	  clrbit(Msg[FRM_PROPERTY_H],bitXPARA);		//不含XPARA
//
	  Msg[FRM_INST_INDEX]=*(inst+FRM_INST_INDEX);
	  Msg[FRM_INST_INDEX+1]=*(inst+FRM_INST_INDEX+1);

	  Msg[TSEN_RW_MAP_TYPE]=*(inst+TSEN_RW_MAP_TYPE);

//	  Msg[TSEN_RW_PAGE_INDEX]=*(inst+TSEN_RW_PAGE_INDEX);
//	  Msg[TSEN_RW_PAGE_INDEX+1]=*(inst+TSEN_RW_PAGE_INDEX+1);
//	  Msg[TSEN_RW_PAGE_INDEX+2]=*(inst+TSEN_RW_PAGE_INDEX+2);
//	  Msg[TSEN_RW_PAGE_INDEX+3]=*(inst+TSEN_RW_PAGE_INDEX+3);

//	  Msg[TSEN_RW_PAGES]=*(inst+TSEN_RW_PAGES);
//	  Msg[TSEN_RW_PAGES+1]=*(inst+TSEN_RW_PAGES+1);

//	  Msg[TSEN_RW_PAGE_UNIT]=*(inst+TSEN_RW_PAGE_UNIT);

//	  p=&Msg[TSEN_RW_PAGE_UNIT+1];
//	  *p++=0;*p++=0;*p++=0;*p++=0;*p++=0;	//reserve 5 bytes

	  Msg[FRM_RET]=OK;

	  sum=GetCRC(&Msg[0],30);
	  Msg[HCRC_L]=LoOfInt(sum);
	  Msg[HCRC_H]=HiOfInt(sum);

	  p=&Msg[0];
	  TransmitBytes(p,32);
	  /*i=32;
	  while(i--){Serial_Transmit(*p++);}*/

	  Delay_ms(100);
	  
	}	
}



BYTE Authen(BYTE *inst,BYTE connection_type)		
{
BYTE Msg[60],OpType,t;
BYTE SrcAddr[]={0xE8,0x03,0,0};
BYTE *p,*p_end;
UINT sum;

	OpType=*(inst+FRM_OP);

	if(OpType==OP_UP_AUTHEN){
	  Msg[FRM_VER]='H';
	  Msg[FRM_LEN]=31; Msg[FRM_LEN+1]=0;	
//	  Msg[FRM_DES_ADDR]=0; Msg[FRM_DES_ADDR+1]=0;

	  memcpy(&Msg[FRM_SRC_ADDR],&SrcAddr[0],4);

	  Msg[FRM_OP]=OpType;
	  Msg[FRM_PROPERTY_L]=0b00000000; Msg[FRM_PROPERTY_H]=0b00000000;

	  inst=&Msg[0];
//	  *(inst+14)=0;			//InstIndex,保持原值
//	  *(inst+15)=0;
//	  *(inst+16)=MapType;	//MapType
	  *(inst+20)=0x40;		//PSW
	  *(inst+15)=0xE2;
	  *(inst+15)=0x01;

	  memcpy((inst+23),&MacID[1],6);	//MAC_ID

	  sum=GetCRC(&Msg[0],30);
	  Msg[HCRC_L]=LoOfInt(sum);
	  Msg[HCRC_H]=HiOfInt(sum);	

//扩展参数
	  p=inst+32;
	  *p++=9;			//LenPack
	  *p++=0;
	  *p++=0;			//PageIndex
	  *p++=1;
	  *p++=2;
	  *p++=3;
	  *p++=1;			//Pages
	  *p++=0;			
	  *p++=0;			//PageUnit
	  sum=GetCRC(inst+32,9);
	  *p++=LoOfInt(sum);
	  *p=HiOfInt(sum);
	  p_end=p;

//身份验证请求
	  p=&Msg[0];
	  while(p<=p_end){Serial_Transmit(*p); p++;}	
	  Delay_ms(200);

//验证响应
	  stepGET_RESP=0;
	  do{
		t=GetResp(connection_type);		//此处：返回中含指令
	  }while(t==RX_WAIT);				//use rx[]

	  return t; 
	}	
	return FAILED;
}


BYTE GetResp(BYTE ComType)
{
BYTE t;
static UINT EndMs;
ULNG MaxWait;
	
  switch(stepGET_RESP){
    case 0:
	  MaxWait=1500;									//1.5s,默认为CONNECT_DIRECT。
	  if(ComType==CONNECT_GPRM){MaxWait=30000;}		//30s

	  EndMs=InitRx(MaxWait,FILTER_DIRECT);
	  stepGET_RESP=1;

    case 1:
	  t=DoReceive(EndMs);			
	  if(t==RX_WAIT){
	    stepGET_RESP=1;
	    return RX_WAIT;
	  }
	  else{return t;}		//成功或失败
  }
  return 0;
}






void Ack(BYTE connection_type)
{
UINT iCRC;	
BYTE ack[32],*p;

	FillBufWith(&ack[0],32,0x0);

	ack[FRM_VER]=INST_HEADER;
//LEN
	ack[FRM_LEN]=lenHEAD-1;		
	ack[FRM_LEN+1]=0;			
//DES
//	ack[FRM_DES_ADDR]=0;
//	ack[FRM_DES_ADDR+1]=0;
//	ack[FRM_DES_ADDR+2]=0;
//	ack[FRM_DES_ADDR+3]=0;
//STA_ID
	ack[FRM_SRC_ADDR]=0; 
	ack[FRM_SRC_ADDR+1]=0; 
	ack[FRM_SRC_ADDR+2]=0; 
	ack[FRM_SRC_ADDR+3]=0; 
//
	ack[FRM_OP]=OP_ACK;				
	ack[FRM_PROPERTY_L]=0b00001001; ack[FRM_PROPERTY_H]=0b00001001;			

	ack[FRM_INST_INDEX]=0;
	ack[FRM_INST_INDEX+1]=0;
	ack[TSEN_RW_MAP_TYPE]=M_CFG;

//	ack[TSEN_RW_PAGE_INDEX]=0;
//	ack[TSEN_RW_PAGE_INDEX+1]=0;
//	ack[TSEN_RW_PAGE_INDEX+2]=0;
//	ack[TSEN_RW_PAGE_INDEX+3]=0;

//	ack[TSEN_RW_PAGES]=0;
//	ack[TSEN_RW_PAGES+1]=0;

//	ack[TSEN_RW_PAGE_UNIT]=0;;
	ack[FRM_RET]=OK;


//	p=&ack[0]; 
//	ack[PARA1]=header[PARA1];			//addrPAGE
//	ack[PARA2]=header[PARA2];
//	ack[PARA3]=header[SENDING_SEG_INDEX];
//	ack[PARA4]=header[SENDING_SEG_END];
//	ack[PARA5]=0;		
//	ack[PARA6]=header[ALL_SEGMENTS];
	iCRC=GetCRC(&ack[0],lenHEAD-2);
	ack[HCRC_L]=LoOfInt(iCRC);
	ack[HCRC_H]=HiOfInt(iCRC);
	
	switch(connection_type){
	  case CONNECT_PSTN:
	  case CONNECT_DIRECT:
	  case CONNECT_USWAVE:
	  case CONNECT_WIFI:  
	    p=&ack[HEAD0];
		TransmitBytes(p,lenHEAD);
	    /*for(i=0;i<lenHEAD;i++){
		Serial_Transmit(*p++);
	    }*/
	    break;
	  case CONNECT_SM:
	    //skip ack 
	    break;

//	  default: ErrorReset(0x11);
	}
}


void getXPara(BYTE *inst,BYTE *pX)
{
//kjdsahkcasdlnljkvsjd
}

//===========================================
// Accept Map pages from terminal/Server
// Ack after writing a page of Map
// &rx[0]== inst
//===========================================
BYTE DownLoadMap(BYTE connection_type,BYTE *inst)		
{
BYTE t,err_code;
UINT ipl_save;
ULNG iPage;
UINT Pages;
BYTE PageUnit;
BYTE r;
BYTE *rx;
	rx=RX_BUF0();

//当从服务器下载时
	if(TREG_Authentication(inst,connection_type)==FAILED){
	  r=STEP_AUTHEN_FAILURE;
	  goto DOWN_FAILED;		//return STEP_AUTHEN_FAILURE;
	}
	//if OK => 回执中含新指令（下行读）

//
	di(ipl_save,IPL_T2_INT-1);	//允许T2_INT中断;


//处理“WR=31:888888,....”
	r=WR_proc(inst);
	if(r==WR_OK){goto POST_DOWN;}
	else if(r==WR_FAILED){goto DOWN_FAILED;}
	//r==WR_NULL

//处理“SLU=SSSSSSSSLLLLUU”
	if(getSLU(inst,&iPage,&Pages,&PageUnit)==FAILED){
	  r=FAILED;			//STEP_SLU_PARA_ERR;
	  goto DOWN_FAILED;
	}

	iPage=0;
	if((PageUnit==0xff)||(PageUnit==0x0)){		//协议转化为字节操作,单页内
	  Pages=1;
	}

//下载
	err_code=0;
	while(iPage++<Pages){
	  SaveMap(inst);
	  if(iPage>=Pages-1){break;}
	  Ack(connection_type);

	  stepGET_RESP=0;
	  do{
		t=GetResp(connection_type);		//此处：返回中含指令
	  }while(t==RX_WAIT);

	  if((t==RX_OVER_TIME_OVERFLOW)||(t==RX_OVER_WITH_ERR)||(t!=RX_OVER_OK)){err_code=1; goto DOWN_FAILED;}	
	  if(!StripOffComHeader(connection_type)){err_code=2; goto DOWN_FAILED;}
	  if((*rx!=INST_HEADER)||(*(rx+FRM_OP)!=OP_WRITE_AREA)){err_code=3; goto DOWN_FAILED;}
	}

POST_DOWN:	//数据接受后，处理相关更新
	ClrWdt(); 
//	Ack(connection_type);
	r=PostDownload(inst);			//下载后必须完成的操作处理
	if(r==OK){  					//出错标志只在全部操作正常完成后,清除
	  Ack(connection_type);
	  Error0.ALLbits=0;           	//正常结束
	}
//	ei(ipl_save);
	
DOWN_FAILED:
//	post_write=*(inst+TSEN_RW_POST_WRITE);		//27
	/*if(!testbit(post_write,bitDoReset)){
	  ei(ipl_save);
	}*/
	return r;  		//出错结束
}

//=======================================
// 处理“WR=..."(长度不超过254)
// e.g:
//		"WR=33:888888,62:E001"
//=======================================
BYTE WR_proc(BYTE *inst)
//BYTE WR_test(void)
{
BYTE x[256],t[256],*p;
UINT uLen;
UINT offset,len;
BYTE r,fEnd;
	
	uLen=getPackStr(inst,&x[0]);
	if(uLen==0){return FAILED;}

	fEnd=FALSE;
//	strcpy((char *)&x[0],"WR=33:888888,62:E001");
	uLen=strlen((char *)&x[0]);
	p=InStr(&x[0],"WR=",uLen);
	if(!p){return WR_NULL;}		//WR_NULL: 不存在
//
	p+=3;
	while(1){
	  sscanf((const char *)p,(const char *)"%u:%s",&offset,&t[0]);
	  strcpy((char *)&x[0],(char *)&t[0]);		//x[]="888888,62:E001"
	  p=(BYTE *)strchr((const char *)&x[0],',');
	  if(p){
		len=(p-&x[0])/2;
	    Txt2Bin(&t[0],(BYTE *)&x[0],len);

		p++;	//指向下一"XX:XXXX"
		strcpy((char *)&x[0],(char *)p);	//x[0]="62:E001"
	    p=&x[0];
	  }
	  else{
		fEnd=TRUE;
		len=strlen((char *)&x[0])/2;
		Txt2Bin(&t[0],(BYTE *)&x[0],len);
	  }

	  mem_WriteData(M_CFG,&t[0],(ULNG)CFG_BUF0+offset,len);			//更新CFG
	  r=mem_CheckWriting(M_CFG,&t[0],(ULNG)CFG_BUF0+offset,len);
//	  r=1;	
	  if(!r){return WR_FAILED;}
	  if(fEnd){break;}
	}
	return WR_OK;
}


//=======================================
//  下载后操作
//
//=======================================
BYTE PostDownload(BYTE *inst)
{
BYTE post_write,err_code;
BYTE MapType;
BYTE r;
//UINT ipl_save;
hTIME dt;

//	di(ipl_save);
	err_code=0;

//Inst
	MapType=*(inst+TSEN_RW_MAP_TYPE);		
	switch(MapType){
	  case M_CFG:
		post_write=*(inst+TSEN_RW_POST_WRITE);		//27

//下载修改CFG：
		if(testbit(post_write,bitDoCfgCRC)){
	  	  if(!(DoCfgCRC(inst))){				//如果cfg[]正确 -> 加载
			err_code=7; r=FAILED; goto END_POST_DOWNLOAD;
	  	  }
		}
		//break;

//记录重新分区：RECORD_INIT
		if(testbit0(post_write)){
	  	  if(!InitRecord()){
			err_code=4; r=FAILED; goto END_POST_DOWNLOAD;
	  	  }									//or MODE_DOWNLOAD
		}	    

//时钟刷新：CALENDAR_REFRESH
		if(testbit(post_write,bitCalendarRefresh)){
		  if(!getDT(inst,&dt)){
			err_code=5; r=FAILED; goto END_POST_DOWNLOAD;
		  }
	  	  if(!UpdateCalendar(dt)){
			err_code=6; r=FAILED; goto END_POST_DOWNLOAD;
	  	  }
		}
		r=OK;
		break;

	  default: 
		r=FAILED;
		break;
	}
	
 
END_POST_DOWNLOAD:
//	ei(ipl_save);
	return r;
}

BYTE DoCfgCRC(BYTE *inst)
{
BYTE *src0,*src;
UINT sp_len;

	sp_len=getUINT(inst+DATA0);
	src0=inst+DATA0+2;
	src=SkipParaStr(inst,src0);		//指向：二进制block

	if(sp_len>src-src0){	//存在“二进制”，优先处理
	  //step 1: CFG_CRC、CFG_INDEX写入Mirror
	  mem_WriteData(M_CFG,src,(ULNG)CFG_BUF0+CM_CFG_CRC,2);		//CfgCRC --> CM_CFG_CRC	
	  mem_WriteData(M_CFG,src+2,(ULNG)CFG_BUF0+CM_CFG_INDEX,2);	//CfgIndex --> CM_CFG_INDEX	
	}
	else{		//处理“字符串”

	}

//step 2: Check CFG_BUF's CRC
	if(LoadCfgIfOk(CFG_BUF0)){		//校验CFG_BUF0,并加载至cfg[]
	  if(SaveCfg(&cfg[0],CFG0)==FAILED){return 0;}		//Save to CFG, FRAM
	  if(SaveCfg(&cfg[0],CFG_BUF0)==FAILED){return 0;}	//加载CFG到CFG_BUF，为远程设置（数个字节）的校验做准备。
	  return 1;
	}
	return 0;  //加载old CFG,丢弃下载的修改
}

//=======================
//    Cfg[] -> CFG0
//=======================
BYTE SaveCfg(BYTE *pCfg,UINT uDes)
{
int len;
BYTE r;
	len=cfg[CM_LEN_CFG]*lenCELL;
	mem_WriteData(M_CFG,pCfg,(ULNG)uDes,len);			//更新CFG

//src=src_save;
	r=mem_CheckWriting(M_CFG,pCfg,(ULNG)uDes,len);	
	return r;  
}
	
/*
	p=&cfg[0];
	uAddr=CFG0;
	for(i=0;i<cfgs;i++){
	  mem_ReadData(M_CFG,(ULNG)uAddr+CFG_BUF0,p,lenCELL);	//刷新cfg[]
	  mem_WriteData(M_CFG,p,(ULNG)(uAddr),lenCELL);			//更新CFG
	  uAddr+=lenCELL;
	  p+=lenCELL;	  
	}
*/



/*BYTE CheckMirrorCRC(int len)
{
int i;
int sum;
BYTE c;
BYTE crc_l,crc_h;

	sum=0;
	for(i=0;i<len;i++){
	  if((i!=62)&&(i!=63)){
		mem_ReadData(M_CFG,(ULNG)i+CFG_BUF0,&c,1);
	    sum+=c;
	  }
	}
	mem_ReadData(M_CFG,(ULNG)62+CFG_BUF0,&crc_l,1);
	mem_ReadData(M_CFG,(ULNG)63+CFG_BUF0,&crc_h,1);
	return 1;
}
*/

//=========================================
// 密码身份认证
//
// DIRECT时，忽略。考虑现场优先和降低使用难度的原则；
// GPRS时，忽略，此时为TER发起。
// 超级密码，忽略。
//
// SM时，必须。
//=========================================
BYTE CheckSetupAuthority(BYTE connection_type,BYTE *inst)
{
BYTE i,Psw_inst[3],Psw_inside[3];

//DIRECT
	if((connection_type==CONNECT_DIRECT)||(connection_type==CONNECT_GPRM)){return TRUE;} 

//Super User(0x3ffff=262143)
	Psw_inst[0]=*(inst+24);		//Password
	Psw_inst[1]=*(inst+25);
	Psw_inst[2]=*(inst+26);
	if((Psw_inst[0]==0x03)&&(Psw_inst[1]==0xff)&&(Psw_inst[2]==0xff)){return TRUE;}

//比较密码
	mem_ReadData(M_CFG,REG0+(UINT)CM_PSW,&Psw_inside[0],3);
	for(i=0;i<3;i++){
	  if(Psw_inst[i]!=Psw_inside[i]){return FALSE;}
	}
	return TRUE;
}



/*
BYTE ApplyStaID()
{
UINT sum,ui;	
UINT lenBELT;
BYTE msg[32];
BYTE *p;

//建立帧头
	msg[FRM_VER]=INST_HEADER;
	msg[FRM_LEN]=31;			
	msg[FRM_LEN+1]=0;
	
//StaID
	p=&msg[FRM_STA_ID];
	*p++=0; *p++=0; *p++=0; *p++=0;

//OpType,Property
	msg[FRM_OP]=OP_GET_STAID;
	msg[FRM_PROPERTY]=0x00;			//debug（无后续）（无级联）（时标有效）（需应答）（应答）
	msg[FRM_PROPERTY+1]=0x00;		//debug（无后续）（无级联）（时标有效）（需应答）（应答）

//InstIndex: Unsolicited时，取0x8000(bit15=1)。区别于inst_index={0-0x7fff}
	msg[FRM_INST_INDEX]=0;		//2 bytes
	msg[FRM_INST_INDEX+1]=0x80;	

//PSW
	mem_ReadData(M_MEM,(UINT)CM_PSW,&msg[TSEN_RW_RW_PSW],3);	//3 bytes

//FAMILY_CO
	msg[TCOM_FAMILY_CO]=0;

//MAC_ID
	memcpy(&msg[FRM_MAC_ID],&MacID[1],6);				//6 bytes

	sum=GetCRC(&msg[FRM_VER],lenHEAD-2);
	msg[HCRC_L]=LoOfInt(sum);
	msg[HCRC_H]=HiOfInt(sum);		

	return 1;
}	

*/
