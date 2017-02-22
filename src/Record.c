#include 	"header.h"
#include    "h_macro.h"



#define	MARK_YEAR		0
#define	MARK_MONTH		1
#define	MARK_DAY		2
#define	MARK_HOUR		3
#define	MARK_MINUTE		4

#define	LEN_TMSTP		8		//len of timestamp: 0x81 YMDHmS 0xBC
#define MAX_OF_REC		0x32	//42+8: max-bytes(42),0x81(1),time-stamp(6),0xBC(1)

extern SW	sw1;

extern BYTE cfg[];
extern BYTE c0Table[],cxTable[];


//================================
//Bit usage variables
//================================
extern MSG0				Msg0;
extern MSG1				Msg1;
extern ERRORS			Error0;
extern TIMERS			Tmr;
extern TIMEUP 			TmrUp;

//for record
//extern BYTE RecYear;			//contain the next record time which will be loaded from
//extern BYTE RecMonth;			//  beform x-mem before
//extern BYTE RecDay;
//extern BYTE RecHour;
//extern BYTE RecMinute;

//===============================================//
// �޸ģ�
// 1��������DownMode��ȡ������ģʽ����MixTools��ɣ���
// 2������SD��ʼ����ȡ��XMEM��ʼ����
//
//
//
//===============================================//
BYTE InitRecord(void)
{
BYTE iC0,iCx,xm_fcnt[2];			//,yn;
UINT entry,addr_rec_ptr;	//,c0_rec_ptr;
hTIME Calendar;		
BYTE CXS;
UINT c0,cx;
hTIME dt;
//BYTE c;

	ClrWdt();

//���"������ʧ�ܼ���"
	xm_fcnt[0]=0;
	xm_fcnt[1]=~xm_fcnt[0];		
	mem_WriteData(M_MEM,&xm_fcnt[0],mapCNT_XM_FAIL,2);

//��ȡ����(����Ҫ�洢)
	Now(&dt);
	SaveTimeTo(dt,&Calendar);

//��ʼ��
	for(iC0=0;iC0<C0S;iC0++){
	  c0=getC0(iC0);
	  if(IsC0Enabled(&cfg[c0])==0){continue;}		//C0ʹ��
	  if(IsC0Recorded(&cfg[c0])==0){continue;}		//C0��¼ʹ��

	  CXS=cfg[c0+C0_CXS];	//��Ԫ��
	  for(iCx=0;iCx<CXS;iCx++){
		ClrWdt();
		cx=getCX(iC0,iCx);
		
		//��ڵ�ַ
		entry=getUINT(&cfg[cx+CX_REC_ENTRY]);	//mode==MODE_DOWNLOAD

		//��ȡ��¼ָ��ĵ�ַ
		addr_rec_ptr=getRecPtrAddr(iC0,iCx);
		//c0_rec_ptr=getUINT(&cfg[c0+C0_REC_PTR]);
		//addr_rec_ptr=c0_rec_ptr+iCx*4;			//�ռ任���&�ٶ�

		//��ʼ��:RecPtrָ��
		if(!SaveRecPtr(entry,addr_rec_ptr)){return 0;}

	    //cx-rec-area ��0xBF��ʶ
		if(IsCxRecorded(&cfg[cx])){
		  if(!MarkBF(entry)){return 0;}
		}
	  }
	}
	return 1;
}


UINT getRecPtrAddr(BYTE iC0,BYTE iCx)
{
UINT c0,c0_rec_ptr;

	c0=getC0(iC0);
	c0_rec_ptr=getUINT(&cfg[c0+C0_REC_PTR]);
	return (c0_rec_ptr+iCx*4);			//�ռ任���&�ٶ�
}

//=======================================
// ��ʼ��: �����Ԫ��Ӧ�����ȫ��Sectors
//=======================================
/*
BYTE ClrCxXmem(BYTE *cx_cfg)
{
BYTE iSec;	//,xm_fail;
UINT uPg;

//���ó�ʼ��������(ʧ��)
	SetCfgBit(REG0+((UINT)(MAIN_XMEM_FAIL)),bitXMEM_INIT_FAILED);
	
//���ִ��ʧ�ܣ�����ѭ������Watchdog��λ����������ʾ�����ġ��ɹ���ʾ����
	if(IsCxXMem(cx_cfg)==TRUE){
	  iSec=*(cx_cfg+CX_SECTOR_ENTRY);
	  while(iSec!=*(cx_cfg+CX_SECTOR_EXIT)){
	    uPg=((UINT)(iSec))<<8;
	  	xm_SectorErase((long)(uPg)<<8);	//ɾ��һ��sector
		WaitXmReadyForErase();					//�ȴ����.
		iSec++;
	  }
	}

//��������ʱ,�����ʼ��������.
	ClrCfgBit(REG0+((UINT)(MAIN_XMEM_FAIL)),bitXMEM_INIT_FAILED);
	return 1;
}
*/

/*
BYTE EraseNextSector(UINT uDesPage)
{
//BYTE iSec,xm_fail;
//UINT uPg;

//���ó�ʼ��������(ʧ��)
	SetCfgBit(REG0+((UINT)(MAIN_XMEM_FAIL)),bitXMEM_CLR_CX_SECTOR_FAILED);
	
//���ִ��ʧ�ܣ�����ѭ������Watchdog��λ����������ʾ�����ġ��ɹ���ʾ����
	xm_SectorErase((long)(uDesPage)<<8);	//ɾ��һ��sector
	WaitXmReadyForErase();						//�ȴ����.

//��������ʱ,�����ʼ��������.
	ClrCfgBit(REG0+((UINT)(MAIN_XMEM_FAIL)),bitXMEM_CLR_CX_SECTOR_FAILED);
	return 1;
}

*/

BYTE MarkBF(UINT entry)
{
UINT uEntry;
BYTE cBF;

	uEntry=entry*REC_BLOCK_SIZE;

//mark a begin mark(0xBF), 1st record and loop back need this mark
	cBF=REC_FMT_BEGIN;
	mem_WriteData(M_MEM,&cBF,uEntry,1);					
	if(!mem_CheckWriting(M_MEM,&cBF,uEntry,1)){
		return 0;}
	return 1;	
}

//=============================================
// ָ������ʼ��
//
// Save begin mark
// Save real address to RecPtrSave 
// Save Next Record Time
// 
// to record area(1st 1k)
//
//=============================================
/*
BYTE InitRecPtr(UINT entry,UINT RecPtr)
{
UINT uEntry;
BYTE rec[16],*p,err_code;

	uEntry=entry*REC_BLOCK_SIZE;

//save pRecBuf(��¼ָ��) & ����(2)
	p=&rec[0];
	*p++=LoOfInt(uEntry); 
	*p++=HiOfInt(uEntry);
	*p++=~rec[0];			//����������(2)
	*p++=~rec[1];

//save pRecBuf, pointer is now point to the beginning of corresponding record area
	mem_WriteData(M_MEM,&rec[0],RecPtr,4);					
	if(!mem_CheckWriting(M_MEM,&rec[0],RecPtr,4)){
	  err_code=0xfe; goto ERR1;
	}	
	return 1;

ERR1:
	return 0;
}
*/

//===========================================
//val_len: number of bytes
//addr_rec_ptr: record pointer is here
//
// 1. REC_FMT_BEGIN may be overwritten when
//	data cycles back. so when record being
//  read, length should be limitted.
//===========================================
char DoRecord(BYTE *cx_val,hTIME *time_stamp,BYTE iC0,BYTE iCx)
{
UINT addr_rec_ptr;
UINT uAddr,uRepeat;
BYTE i;
BYTE rec[0x30];
UINT top,bottom;
BYTE *p;
BYTE val_len;
UINT cx;

	ClrWdt();	

	cx=getCX(iC0,iCx);		//��ȡcx�׵�ַ

//	addr_rec_ptr=getUINT(&cfg[cx+CX_REC_PTR]);
	addr_rec_ptr=getRecPtrAddr(iC0,iCx);

	val_len=cfg[cx+CX_VAL_BLEN];
	top=getUINT(&cfg[cx+CX_REC_ENTRY])*REC_BLOCK_SIZE;
	bottom=getUINT(&cfg[cx+CX_REC_EXIT])*REC_BLOCK_SIZE;

	if(bottom>REC_MEM_BOTTOM){bottom=REC_MEM_BOTTOM;}

//��ȡ��Ԫ��ǰ��¼ָ��
	uAddr=getRecPtr(iC0,iCx);
	if(uAddr==0){return 0;}

//�߽���
	if((uAddr<top)||(uAddr>bottom))
	{
	  //Add recovery procedure
	  //uAddr=RecordRecovery(addr_rec_ptr,top,bottom);
	  return FAILED;
	}


//�ж�uAddrָ���...
	i=GetByte(M_MEM,uAddr);
	switch(i){
	  case REC_FMT_BEGIN:  			//�Ƿ�BF��		Check if stamp is REC_FMT_BEGIN
	    Inc(uAddr,top,bottom);		//skip REC_FMT_BEGIN
		return AppendNewRec(cx_val,uAddr,time_stamp,iC0,iCx);

	  case 0x81: //�Ƿ�0x81��
      	if(!CheckSucessive(uAddr,time_stamp,iC0,iCx)){	//�Ƿ�������not successive
	  	  if(iCx==5){
			i++;
	  	  }	
		  AddWith(uAddr,7,top,bottom);				//7: skip time stamp
		  return AppendNewRec(cx_val,uAddr,time_stamp,iC0,iCx);
	    }
		else{	//������¼
		  if(!IsCxCompress(&cfg[cx])){	//��������ѹ��
			return AppendNewRec(cx_val,uAddr,time_stamp,iC0,iCx);
		  }
		}
	    break;

	  default: //���� //error encounter, set new begin... 											
	    i=REC_FMT_BEGIN;
	    mem_WriteData(M_MEM,&i,uAddr,1);
	    return SaveRecPtr(uAddr,addr_rec_ptr);	//�����µĵ�ǰ��¼ָ��
	}


//ѹ��(uAddr -> 0x81)
//Read [val: val_len][repeat: 2(max)][0x81] => rec[]
	DecWith(uAddr,val_len+2,top,bottom);
	ReadRecArray(uAddr,&rec[0],val_len+2,top,bottom); 	//val_len+2

	AddWith(uAddr,val_len+2,top,bottom);	//uAddr -> 0x81
	p=&rec[val_len+1];
	uRepeat=SkipRepeatToVal(&p,uAddr,iC0,iCx);	//pָ��Val��uAddrָ��0x81

//Compare REC/*cx_val
	//�����
	if(!IsValEqual(p,cx_val,val_len)){	
	  return AppendNewRec(cx_val,uAddr,time_stamp,iC0,iCx);
	}

	//���
	if(uRepeat==0xfff){
	  return AppendNewRec(cx_val,uAddr,time_stamp,iC0,iCx);
	}					//uAddr still point to 0x81

//Equal and Inc
	if(uRepeat==0x2E){
	 i++;
	}
	return RepeatInc_Save(uRepeat,uAddr,time_stamp,iC0,iCx);
}

//==================================
// 1)Inc Repeat
// 2)Save a record
//
// ��ʼ��uAddr -> 0x81
//==================================
char RepeatInc_Save(UINT uRepeat,UINT uAddr,hTIME *time_stamp,BYTE iC0,BYTE iCx)
{
BYTE buf[MAX_OF_REC];
BYTE start;		//uRepeat�ĳ���
BYTE *p;
BYTE val_len;
UINT cx,top,bottom;

	cx=getCX(iC0,iCx);		//��ȡcx�׵�ַ

	val_len=cfg[cx+CX_VAL_BLEN];
	top=getUINT(&cfg[cx+CX_REC_ENTRY])*REC_BLOCK_SIZE;
	bottom=getUINT(&cfg[cx+CX_REC_EXIT])*REC_BLOCK_SIZE;

	if(bottom>REC_MEM_BOTTOM){bottom=REC_MEM_BOTTOM;}


//����uRepeat������uAddr,ָ����д���λ�á�
	if(uRepeat>0){
	  if(uRepeat<=0x3f){
	    Dec(uAddr,top,bottom);  
	  }
	  else{
		DecWith(uAddr,2,top,bottom);
	  }
	}

//uRepeat -> buf[]
	uRepeat++;
	start=0;
	p=&buf[0];
	*p++=(LoOfInt(uRepeat)&0x3f)|0xC0;					//C0=0b11000000, sign of repeat
	start++;
	if(uRepeat>0x3f){
	  *p++=((HiOfInt(uRepeat)<<2)|(LoOfInt(uRepeat)>>6))|0xC0;	//C0=0b11000000, sign of repeat
	  start++;
	}
	return SaveARec(&buf[0],uAddr,start,time_stamp,iC0,iCx);
}

BYTE IsValEqual(BYTE *p,BYTE *val,BYTE len)
{
BYTE i;
	for(i=1;i<=len;i++){
	  if(*p!=*(val+len-i)){
	    return FALSE;
	  }												
	  p--;
	}
	return TRUE;
}

//==========================================
// 1����ȡRepeat��
// 2��ͬʱͬ���ƶ�rec��ָ��Val
//
// rec:    RAMָ��
// uAddr:  MEMָ��
//==========================================
UINT SkipRepeatToVal(BYTE **rec,UINT uAddr,BYTE iC0,BYTE iCx)	//*cx)
{
UINT repeat;
BYTE *p;
BYTE val_len;
UINT cx,top,bottom;

	cx=getCX(iC0,iCx);		//��ȡcx�׵�ַ

	val_len=cfg[cx+CX_VAL_BLEN];
	top=getUINT(&cfg[cx+CX_REC_ENTRY])*REC_BLOCK_SIZE;
	bottom=getUINT(&cfg[cx+CX_REC_EXIT])*REC_BLOCK_SIZE;

	if(bottom>REC_MEM_BOTTOM){bottom=REC_MEM_BOTTOM;}
  
	p=*rec;						//eg: C1 06 27
	if((*p&0xC0)!=0xC0){		//0b11000000
	  repeat=0;
	  return repeat;
	}
	else{
	  repeat=(*p&0x3f); 
	  //p=*rec;
	  p--;
	  if((*p&0xC0)==0xC0){							//0b11000000
	    repeat=(*p&0x3f)+(repeat<<6);
		//rec--;
		p--; 
	  }									
	}
	*rec=p;							
	return repeat;    				
}


//========================================
// Append A New Rec
// uAddr is ready on desire blank address
//========================================
char AppendNewRec(BYTE *belt,UINT des,hTIME *time_stamp,BYTE iC0,BYTE iCx)
{
BYTE buf[MAX_OF_REC];
BYTE start;
BYTE val_len;
UINT top,bottom;
UINT cx;

	cx=getCX(iC0,iCx);		//��ȡcx�׵�ַ

//	addr_rec_ptr=getUINT(&cfg[cx+CX_REC_PTR]);
//	addr_rec_ptr=getRecPtr(iC0,iCx);
//	if(addr_rec_ptr==0){return 0;}

	val_len=cfg[cx+CX_VAL_BLEN];
	top=getUINT(&cfg[cx+CX_REC_ENTRY])*REC_BLOCK_SIZE;
	bottom=getUINT(&cfg[cx+CX_REC_EXIT])*REC_BLOCK_SIZE;
	if(bottom>REC_MEM_BOTTOM){bottom=REC_MEM_BOTTOM;}


	DataCpy(&buf[0],belt,val_len);						//val_len bytes are ready
	start=val_len;
	return SaveARec(&buf[0],des,start,time_stamp,iC0,iCx);
}


//===============================================
//          ����val|uRepeat + timestamp
//
// ���һ��(uRepeat & timestamp)
// �������(val & timestamp)
//
// buf: 	ָ��uRepeat��val��ʼ
// start��	uRepeat��val����
//===============================================
char SaveARec(BYTE *buf,UINT des,BYTE start,hTIME *time_stamp,BYTE iC0,BYTE iCx)
{
BYTE *p;
UINT cx;
UINT addr_rec_ptr,top,bottom;

	addr_rec_ptr=getRecPtrAddr(iC0,iCx);
	//c0=getC0(iC0);
	//c0_rec_ptr=getUINT(&cfg[c0+C0_REC_PTR]);
	//addr_rec_ptr=c0_rec_ptr+iCx*4;			//�ռ任���&�ٶ�

	cx=getCX(iC0,iCx);		//��ȡcx�׵�ַ

//	addr_rec_ptr=getUINT(&cfg[cx+CX_REC_PTR]);
//	addr_rec_ptr=getRecPtr(iC0,iCx);
//	if(addr_rec_ptr==0){return 0;}

	top=getUINT(&cfg[cx+CX_REC_ENTRY])*REC_BLOCK_SIZE;
	bottom=getUINT(&cfg[cx+CX_REC_EXIT])*REC_BLOCK_SIZE;
	if(bottom>REC_MEM_BOTTOM){bottom=REC_MEM_BOTTOM;}


	p=buf+start;			//ָ��0x81...
	*p=REC_FMT_TIME;						//1-- 0x81
	TimeStampCpy(p+1,time_stamp,6);			//6-- YMDHmS
	*(p+7)=REC_FMT_END;						//1-- 0xBC, ������

	WriteRecBlock(buf,des,start+LEN_TMSTP,top,bottom);
	AddWith(des,start,top,bottom);			//point to 0x81
	return SaveRecPtr(des,addr_rec_ptr);	//�����µĵ�ǰ��¼ָ��
}


//==============================================
// ��ָ��uAddr���浽ָ����
// CRC�����ƻ�
//==============================================
char SaveRecPtr(UINT uAddr,UINT addr_rec_ptr)
{
BYTE ptr[4];
	ptr[0]=LoOfInt(uAddr);
	ptr[1]=HiOfInt(uAddr);
	ptr[2]=~ptr[0];			//mirror
	ptr[3]=~ptr[1];			//mirror

	mem_WriteData(M_MEM,&ptr[0],addr_rec_ptr,4);					

	if(!mem_CheckWriting(M_MEM,&ptr[0],addr_rec_ptr,4)){
 	  return FAILED; }
	return OK;
}


//=========================================================
// �ڼ�¼ָ�����ʱ������ָ�룺
//	1��Ѱ�Ҽ�¼������REC_FMT_END[0xBC]���ָ�ָ�뵽ǰ[0x81]��
//  2�����Ҳ���REC_FMT_END���ָ�ָ�뵽����(from bottom)��һ��0x81��������û�����壡
//  3����ʧ�ܣ��ָ�ָ�뵽[0xBF]��ʼ״̬��
//=========================================================
UINT RecordRecovery(UINT addr_rec_ptr,UINT top,UINT bottom)
{
UINT uAddr;
BYTE c,i;

//step 1: Ѱ�Ҽ�¼������REC_FMT_END[0xBC]���ָ�ָ�뵽ǰ[0x81]��
	uAddr=bottom;
	while(1){
	  ClrWdt();
	  c=GetByte(M_MEM,uAddr);
	  if(c==REC_FMT_END){				//�ҵ�������
		DecWith(uAddr,7,top,bottom)		//[0x81��YMDHmS, REC_FMT_END]
		c=GetByte(M_MEM,uAddr);
		if(c==0x81){					//�ҵ�ָ��ָ���
		  if(SaveRecPtr(uAddr,addr_rec_ptr)==OK){
			ClrWdt();
			return uAddr;
		  }	
		}
		else{		//���صĴ��󣺽�����ǰû���ҵ�Ԥ�ڵ�0x81
		  break;
		}
	  }
	  uAddr--;
	  if(uAddr==top){break;}	//��ͷ��
	}

/*����û�����壬���ҷ�ʱ�䣬��ʱȡ��
//step 2: �ָ�ָ�뵽����(from bottom)��һ��0x81��
	uAddr=bottom-1;
	while(1){
	  c=GetByte(M_MEM,uAddr);
	  if(c==REC_FMT_BEGIN){				//�ҵ���ʼ��
		if(SaveRecPtr(uAddr,addr_rec_ptr)==0){
		  return uAddr;
		}	
	  }
	  uAddr--;
	  if(uAddr==top){break;}	//��ͷ��
	}
*/

//step 3: �ָ�ָ�뵽[0xBF]��ʼ״̬��	if(uAddr==top){	//��ͷ��
	i=REC_FMT_BEGIN;
	mem_WriteData(M_MEM,&i,uAddr,1);
	  //
    if(SaveRecPtr(uAddr,addr_rec_ptr)==OK){
	  return uAddr;
	}
	return 0;	//������
}


BYTE WriteRecBlock(BYTE *src,UINT des,BYTE len,UINT top,UINT bottom)
{
BYTE i;

	for(i=0;i<len;i++){
 	  mem_WriteData(M_MEM,src,des,1);					
	  if(!mem_CheckWriting(M_MEM,src,des,1)){
		return 0; 
	  }
	  src++;
	  Inc(des,top,bottom);
	}
	return 1;
}


//========================================
// Check if time-stamp is in successive
// that:
//		time-stamp+interval==Now ?
//
// 1.time-stamp is ready in &hYear.
//
//
// cx's cfg begin in p2[0]
// 0: not sucessive
// 1: in successive
//
//========================================
BYTE CheckSucessive(UINT uAddr,hTIME *now,BYTE iC0,BYTE iCx)			
{
BYTE TimeStamp[5];
BYTE unit,interval,offset;
hTIME dt;
UINT cx,top,bottom;

	cx=getCX(iC0,iCx);		//��ȡcx�׵�ַ

	top=getUINT(&cfg[cx+CX_REC_ENTRY])*REC_BLOCK_SIZE;
	bottom=getUINT(&cfg[cx+CX_REC_EXIT])*REC_BLOCK_SIZE;
	if(bottom>REC_MEM_BOTTOM){bottom=REC_MEM_BOTTOM;}

	Inc(uAddr,top,bottom);
	ReadRecArray(uAddr,(BYTE *)&dt,5,top,bottom);	//read YMDHM

//10xxxxxx->00xxxxxx
	dt.year&=0x3f;
	dt.month&=0x3f;
	dt.day&=0x3f;
	dt.hour&=0x3f;
	dt.minute&=0x3f;

//add interval
	unit=cfg[cx+CX_REC_GAP]>>6;		//INTERVAL_UNIT)&0x0f;
	interval=cfg[cx+CX_REC_GAP]&0x3f;
	offset=cfg[cx+CX_SUB_GAP_OFFSET];

//	unit=*(cx_cfg+CX_REC_GAP)>>6;		//INTERVAL_UNIT)&0x0f;
//	interval=*(cx_cfg+CX_REC_GAP)&0x3f;
//	offset=*(cx_cfg+CX_SUB_GAP_OFFSET);

	AddInterval(&dt,unit,interval,offset);
	DataCpy(&TimeStamp[0],(BYTE *)&dt,5);

//refresh Now
	RestoreTimeFrom(now,&dt);	//now -> dt

//�Ƚ�
    if((TimeStamp[0]!=dt.year)
	||(TimeStamp[1]!=dt.month)
	||(TimeStamp[2]!=dt.day)
	||(TimeStamp[3]!=dt.hour)
	||(TimeStamp[4]!=dt.minute)){
	  return 0;
	}
	return 1;
}


//=======================================
// array reverse:
// b[H],b[M1],b[M2],b[L] 
//			=> b[L],b[M2],b[M1],b[H]
// "little-end-first"
//
// note: 
//	1. *src and *des can be same after modifing
//  2. max-arrary is 8
//=======================================
//void Reverse(BYTE *des,BYTE *src,BYTE len)
void Reverse(BYTE *buf,BYTE len)
{
BYTE i;
BYTE b[256];
BYTE *buf0;

	buf0=buf;
	buf+=len-1;
	for(i=0;i<len;i++){
	  b[i]=*buf--;
	}

	DataCpy(buf0,&b[0],len);
}



BYTE CompareTime(BYTE *timeX,BYTE *timeY)
{
BYTE *p,i;

//�����λ
	p=timeX; for(i=0;i<=4;i++){*p=(*p)&0x3f;p++;}
	p=timeY; for(i=0;i<=4;i++){*p=(*p)&0x3f;p++;}

//year,month,day,hour,minute
	for(i=0;i<=4;i++){
	  if(*timeX<*timeY){return TIME_SMALLER;}
	  if(*timeX>*timeY){return TIME_BIGGER;}
	  timeX++; timeY++;
	}
	return TIME_EQUAL;
}

//=========================================
// date_time: ���жϵ�ʱ��
//=========================================
BYTE IsRecTimeMatched(hTIME *date_time,BYTE iC0,BYTE iCx)
{
BYTE interval,unit,offset;
BYTE fMatched,day,hour,minute;
UINT cx;

	cx=getCX(iC0,iCx);

	unit=cfg[cx+CX_REC_GAP]>>6;		//&0x0f;
	interval=cfg[cx+CX_REC_GAP]&0x3f;
	offset=cfg[cx+CX_SUB_GAP_OFFSET];

	day=date_time->day;
	hour=date_time->hour;
	minute=date_time->minute;

	fMatched=FALSE;

	switch(unit){
	case UNIT_DAY:						//At right hour every day
	  if((day%interval)==0){
		if((hour==offset)&&(minute==0)){
		  fMatched=TRUE;
		}
	  }
	  break;
	case UNIT_HOUR:						//At every x-hour
	  if((hour%interval)==0){
		if(minute==0){
		  fMatched=TRUE;
		}
	  }
	  break;
	case UNIT_MINUTE:
	  if((minute%interval)==0){
		fMatched=TRUE;
	  }
	  break;
	}
	return fMatched;
}

//======================================
// ��ȡ��Ԫ��ǰ�ļ�¼ָ��
//
// 0: ��ʾ��Чָ��
//======================================
UINT getRecPtr(BYTE iC0,BYTE iCx)
{
UINT c0,addr_rec_ptr;
BYTE ptr[4],m1,m2;

//	cx=getCX(iC0,iCx);
//	addr_rec_ptr=getUINT(&cfg[cx+CX_REC_PTR]);

	c0=getC0(iC0);
	if(IsC0Recorded(&cfg[c0])==0){return 0;}		//��Ԫ����¼����Ԫ������¼��

	addr_rec_ptr=getRecPtrAddr(iC0,iCx);
	//c0_rec_ptr=getUINT(&cfg[c0+C0_REC_PTR]);
	//addr_rec_ptr=c0_rec_ptr+iCx*4;			//�ռ任���&�ٶ�

//��ȡָ��REC_PTR
	mem_ReadData(M_MEM,addr_rec_ptr,&ptr[0],4);		//Recptr structure

//У��ָ��
	m1=~ptr[2];
	m2=~ptr[3];

//	if((ptr[0]==~ptr[2])&&(ptr[1]==~ptr[3])){		//��������ֱ�ӱȽϻ����Ϊʲô��
	if((ptr[0]==m1)&&(ptr[1]==m2)){
	    return getUINT(&ptr[0]);
	}
	else{
	  return 0;	//ָ�����:��ַ0������cx��¼��
	  //goto FIX_REC_POINTER	//����REC_PTR����
	}		
}

//=============================================
//               DO CARD RECORD 
//=============================================
void InitCardRecord(void)
{
BYTE r;

	if(swSD_CARD==OFF){return;}

//1����ʼ��ģ��SPI:TF����оƬCH376ͨ��I/Oģ��SPI����
//2����ʼ��CH376host
	r=CardInit();	
	if(r!=USB_INT_SUCCESS){return;}


//TF���Ƿ���λ
	r=CardMount();
	if(r!=USB_INT_SUCCESS){return;}


//1�������ļ�����
//2�����cfg��
//3��������һ����ʼ��¼ʱ�䣻
//4��д����ʼ��¼ʱ�䣻
	InitCardFiles();

}

/*
void DoCardRecord(void)
{
}
*/
