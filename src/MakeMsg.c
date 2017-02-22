#include 	"header.h"
#include    "h_macro.h"

//��ʱ
//UINT		BCFG_LEN;	//BCFG����

static UINT MakeBelt(BYTE fmt_id,BYTE *belt);		//����BELT����
static BYTE *BeltSelect(BYTE *des_belt,BYTE *src_belt,BYTE **sfmt);
static BYTE *GetFmt(BYTE fmt_id,BYTE *fmt);
static BYTE *ReadCxRecBelt(BYTE *belt,BYTE **sfmt);
static BYTE *AddCxBelt(BYTE *belt,UINT uAddr,BYTE CxBeltLen,UINT recTop,UINT recBottom);
static void MakeFrmHead(BYTE op,UINT lenMsg,BYTE *msg,UINT property);
static BYTE numMatched(BYTE *recDT);


extern BYTE		cfg[];
extern BYTE 	cxTable[];		//c0Table[]
extern PORT		Port[];			//֧�����8����Ԫ(C0)
extern hTIME rTime;	//���ڱ����������ʱ�䣨����15�֣���5�֣�


//===========================================
//  ��belt��ѡ��sSel�涨�ģ�������Ԫ����ɴ�
//  
// sSel samples:
//        !
//		C[001,101234567]	 ! ��Ӧ���ǻ�Ԫ������Ϊ��Ԫ��	
//
// ���أ�
//     �ַ�������λ��
//===========================================
static BYTE *BeltSelect(BYTE *des_belt,BYTE *src_belt,BYTE **sfmt)
{
BYTE *f;	//��ʽָ��
BYTE iC0,iCx;
UINT cx;
BYTE cx_len;

	f=*sfmt;
	iC0=X(*f++);					//��Ԫ
	while((*f!=',')&&(*f!=']')){	//ֻ����1����Ԫ��
	  iCx=X(*f);					//��Ԫ
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
// ��ȡfmt�ַ���
//===========================================
static BYTE *GetFmt(BYTE fmt_id,BYTE *fmt)
{
BYTE *p;	//*p0;
BYTE sn[5];
UINT lenBCFG;

	sn[0]='#';
	btoa(fmt_id&0x0f,(char *)&sn[1]);				//���16�ָ�ʽ��

	p=CMC(CMC_LINK);
	p=FindParaStr(p,"BCFG");
	p++;
	
	lenBCFG=getUINT(CMC(CMC_BCFG_LEN));
	p=InStr(p,(const char *)&sn[0],lenBCFG);		//Ѱ�ҡ�#N��	

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
// Msg�γɹ���Ӧ�����̣ܶ����Ҳ������¼�¼��
// ������ܳ���
// Ӧ��ֹ�жϣ����Ĺ���Ӧ�Ż���ȥ��
//
//
// 				����ҵ���
//
//===========================================
static UINT MakeBelt(BYTE fmt_id,BYTE *belt)		//����BELT����
{
BYTE *f;		//��ʽָ��
BYTE fmt[128];			//fmt����󳤶�	
BYTE *b0;
UINT lenBelt,sum;
BYTE r,iC0;		  
hTIME dt;
BYTE start,len,*p;
BYTE lenFmt;

	b0=belt;

//ҵ�񱨳���
	belt+=2;	//make room for lenBelt

//#n:��ʽ��
    *belt++=fmt_id;			

//��ȡ��ʽstr
	f=GetFmt(fmt_id,&fmt[0]);
	if(f==0){return 0;}			//ָ����ʽ������

//��ʱ
//	strcpy((char *)&fmt[0],"#1T06C[001,101234567],R[11:20,15:30]");
	lenFmt=strlen((char *)&fmt[0]);

//=== T���� ===
//T06
	f=InStr(&fmt[0],"T",lenFmt);
	if(f){
	  start=X(*(++f));		//fָ��T��
	  len=X(*(++f));	

	  Now(&dt);
	  p=(BYTE *)&dt;
	  while(start--){
		p++;
	  }					//ʱ��ָ��p�Ƶ���ʼ��

	  while(len--){		//���ƹ涨�ĳ���
		*belt++=*p++;
	  }
	}

//=== C���� ===
//C[001234,101234567]

	f=InStr(&fmt[0],"C[",lenFmt);
	if(f){
	  f+=2;			//ָ��˿ں�
	  while(*f!=']'){
		if(*f==','){f++;}	//skip
	    iC0=X(*f);
	    r=ReadPort(iC0,&Port[iC0].buf[0],MODE_NORMAL);		//belt_limit=64; speed_mode=MODE_NORMAL  
		belt=BeltSelect(belt,&Port[iC0].buf[0],&f);			//ָ��]��
		//belt=belt_end;
	  }	
	}

//=== R���� ===
//R[12:20,26:30]			
	f=InStr(&fmt[0],"R[",lenFmt);
	if(f){
	  f+=1;
	  while(*f!=']'){
		f++;
	    belt=ReadCxRecBelt(belt,&f);		//������1����Ԫ
		//belt=belt_end;
	  }	
	}

//���泤��
	lenBelt=belt-b0+2;			//2��CRC
	*b0=LoOfInt(lenBelt);		//SP���ȣ�lenBelt,�������ȱ���
	*(b0+1)=HiOfInt(lenBelt);

	sum=GetCRC(b0,lenBelt-2);
	*belt++=LoOfInt(sum);
	*belt=HiOfInt(sum);		

	return lenBelt;
}


//==========================================
//            
// ��sRCoxָ��ĸ�ʽ����ָ��Cx��¼,����CxRecBelt
//
// ���: �����belt��ʼ�Ļ�����
// ��ɺ�
// 1)beltָ��������ݺ���
// 2)pָ���ʽ��','��']'
//
//��ֻ����1����ʽScript��
//
// sRCox ������
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
	iC0=X(*f++);				//��Ԫ
	Cx_off=cxTable[iC0];		//��Ԫ�׵�ַ

	iCx=X(*f++);				//��Ԫ
	cx=cxTable[Cx_off+iCx]*lenCELL;
	cx_trail=Val(++f,&len);		//pָ��','��']'
	f+=len;
	*sfmt=f;

	top=getUINT(&cfg[cx+CX_REC_ENTRY])*REC_BLOCK_SIZE;			
	bottom=getUINT(&cfg[cx+CX_REC_EXIT])*REC_BLOCK_SIZE;

	uAddr=getRecPtr(iC0,iCx);	//��У��
	if((uAddr<top)||(uAddr>bottom)){
	  return belt;
	}			//ָ�볬��Χʱ������ԭbelt	

	if(uAddr){
     //if(IsCxRecorded(uCX0)){		//CxRecord��Ǳ�����top,bottom��ȷ��
	  belt=AddCxBelt(belt,uAddr,cx_trail,top,bottom);
	}	
	//��Ԫ��¼ָ��=0������Ԫ����¼�����uAddr=0����ʾ����¼
	//else{}
	
	return belt;
}


//=========================
// Calculate matched YMDHM
// ��rTime�Ƚ�
//=========================
BYTE numMatched(BYTE *recDT)
{
BYTE iMatched;

	//recDT++;	//ָ��Y
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
//  	��belt�ϼ��뵥��CxBelt��¼
//
// ��¼BELT��ʽ: 
//        [iMatched][DT...][data...]
//
// DT: 		ָ��֡ͷ��YMDHmS��ƥ��Ĳ��֣��ɱ䳤��
// data: 	������"R[XX��LL]"������
// uAddr:	ָ��0x81,YMDHmS,0xBC. S��ʱ���á�. 
// CxBeltLen: ����<=247
//
// ����ʱ��beltָ��ĩβ��
//============================================
static BYTE *AddCxBelt(BYTE *belt,UINT uAddr,BYTE CxBeltLen,UINT top,UINT bottom)
{
BYTE c,i,iMatched,len,save,RecLen;
BYTE *p;
BYTE buf[256];
BYTE *belt0;

	belt0=belt;

//�ر�ʱ���ж�,������¼,��ֹ�ڶ�����ʱ,���ݱ���д.			
	save=CALENDAR_IE;
  	CALENDAR_IE=FALSE;	

	top=top*REC_BLOCK_SIZE;
	bottom=bottom*REC_BLOCK_SIZE;

	c=GetByte(M_MEM,uAddr);
	if(c==0xBF){goto END_ADD_BELT;}

//��ȡRecArray => buf[]
	RecLen=CxBeltLen+8;						//8:��ǩ����
	DecWith(uAddr,CxBeltLen,top,bottom);	//uAddrָ���ײ�
	ReadRecArray(uAddr,&buf[0],RecLen,top,bottom);	//[......uAddr]
	if((buf[CxBeltLen]!=0x81)||(buf[CxBeltLen+7]!=0xBC)){
	  goto END_ADD_BELT;			//ERROR or NO RECORD
	}


//	if(p3[0]==0xBF){return FALSE; }					//no record
//	else if(p3[0]!=0x81){return FALSE;}				//error
	
	iMatched=numMatched(&buf[CxBeltLen+1]);

//��Make CxBelt

//[iMatched] ƥ����
	*belt++=iMatched;		//number of matched
	len=0;

//[Unmatched YMDHmS] ��ƥ��Ĳ���
	i=iMatched;
	i++;
	p=&buf[CxBeltLen+i];
	while(i++<=5){
	  *belt++=*p++;		//������S
	  len++;
	}

//[...]
	Reverse(&buf[0],CxBeltLen);
	DataCpy(belt,&buf[0],CxBeltLen);

// [iMatched] [Unmatched-...DHm] [12-34-C1-C2-56-34-80958F918781-...]
//��

//=======================
END_ADD_BELT:
	//�ָ�ʱ���ж�״̬
  	CALENDAR_IE=save;	
	return belt0+CxBeltLen;
}



//==================================
// ���أ�lenMsg
//==================================
UINT MakeMsg(BYTE protocol_id,BYTE fmt_id,BYTE *msg,UINT property)
{
UINT lenBelt;
BYTE *belt;

//��ʱ
	protocol_id=PROTOCOL_YR_STD;

//==== ��YRЭ�飬������ ====
	if(protocol_id!=PROTOCOL_YR_STD){
	  return 0;
	}


//==== YRX ϵ��Э�� ====
	else if(protocol_id==PROTOCOL_YR_STD){
	  belt=msg+32;
//����ҵ���
	  lenBelt=MakeBelt(fmt_id,belt); 
//����֡ͷ
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

//Unsolicitedʱ��ȡ0x8000(bit15=1)��������inst_index={0-0x7fff}
//	*(msg+FRM_INST_INDEX)=0;		//2 bytes
//	*(msg+FRM_INST_INDEX+1)=0x80;	

//MsgIndex++, �����յ���ͬ(StaID & MsgIndex)ʱ��ֻȡһ��
	mem_ReadData(M_MEM,(ULNG)mapMSG_INDEX,(BYTE *)(msg+TCOM_MSG_INDEX),4);
	li=getULNG(msg+TCOM_MSG_INDEX); li++;		//+1,����
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
//	1. ��'/0'��β
//  2. ���Ȳ�����0x50,�Ա�֤�ܰ�����һ��������
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

//����֡ͷ
	MakeFrmHead(OP_TEXT_MESSAGE,lenSP+32,msg,property);
	return lenSP+32;
}	
