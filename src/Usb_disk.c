#include 	"header.h"
#include    "h_macro.h"

#define		MAX_RETRY_TIMES		20		//filename.1->filename.20
#define		FILE_TIME_DATE		23

#define		UFILE_SEG_CFG		1
#define		UFILE_SEG_REC		2
//#define		UFILE_SEG_XMEM		3

#define		FILE_WRITE_LIMIT		(2048-2)	//(4096-2), V1.0�汾

//======================================
// For transfer record data to USB disk 
//======================================
#define		USB_TOUCH_CX_BOTTOM		0
#define		USB_CREAT_FILE_OK		1
#define		USB_NO_DEVICE			2
#define		USB_DEVICE_FULL			3
#define		USB_FILE_EXIST			4
#define		USB_FAILED				5
#define		USB_NO_RESPONSE			6
#define		USB_CLOSE_FILE_OK		7
#define		USB_CLOSE_FILE_FAILED	8
#define		USB_WRITE_CX_OK			10
#define		USB_CLOSE_DISK_OK		11
#define		USB_WRITE_CFG_OK		12
#define		USB_WRITE_CFG_FAILED	13

/*--------------------------------------------------------*/
/*	 			SLIP protocol constant definition 		  */


#define 	FRAMES		0xc0
#define 	FRAMEE		0xe0
#define 	ESCCHAR		0xdb
#define 	FRAMESESC	0xdc
#define 	FRAMEEESC	0xde
#define 	ESCESC		0xdd


/*
ESCCHAR + ESCESC = ESCCHAR
ESCCHAR + FRAMESESC = FRAMES
ESCCHAR + FRAMEEESC = FRAMEE
*/
/*--------------------------------------------------------*/

extern BYTE 	IPL_CPU;
extern BYTE 	filter_type;
//extern UINT		iTimeCmp;
extern volatile UINT 	ms_click;

extern BYTE 	cfg[];
extern BYTE 	c0Table[],cxTable[];
extern int		pb_count;
extern BYTE 	ActiveCom;

//================================
//Bit usage variables
//================================
//extern TASKLINE 	TaskLine; 
extern MSG0			Msg0;
extern MSG1			Msg1;
extern ERRORS		Error0;
extern TIMERS		Tmr;
extern TIMEUP 		TmrUp;


char file_id;




//unsigned char	sr_state;				/*Serial receive state*/
/*--------------------------------------------------------*/
/* the state list about serial and parallel ports sending & receiving finite state machine */
//#define SearchFrameHead		0
//#define ReadingData			1
//#define MeetESC		    	2
//#define ReadOneFrameFinish	3
/*--------------------------------------------------------*/

//unsigned int	s_read,s_write;			/*Serial read or write buffer pointer*/
//char			sr_buf[SR_BUF_LEN];		/*Serial receive buffer*/

/*
BYTE UARTGetPkt(char *p)
{
	char	c;
	unsigned int	tmpRead,i=0;
	
	tmpRead = s_read;

	if (s_read == s_write)	
		return FALSE;
	
	while (sr_state!=ReadOneFrameFinish)
	{
		c=sr_buf[s_read];
		
		p[i]=c;
		i ++;
		if (i>P_BUF_LEN+1){
			i=0;
			sr_state=SearchFrameHead;
			return FALSE;
		}
		
		s_read=(s_read+1)%SR_BUF_LEN;

		switch (sr_state)
			{
			case SearchFrameHead:
				if(c==(char)FRAMES){
					sr_state=ReadingData;
					i=0;
				}else 
					i=0;
				break;
			case ReadingData:
				if (c==(char)ESCCHAR){
					sr_state=MeetESC;
				}else if (c==(char)FRAMEE){
					i--;
					sr_state=ReadOneFrameFinish;
					return TRUE;
				}else if (c==(char)FRAMES){//�ڽ����������յ�һ��֡ͷ����Ϊǰһ֡�Ѿ��𻵣����¿�ʼ����
					sr_state=ReadingData;
					i=0;
				}
				break;
			case MeetESC:
				i --;
				if (c==(char)FRAMESESC)
					p[i-1] = (char)FRAMES;
				if (c==(char)FRAMEEESC)
					p[i-1] = (char)FRAMEE;
				else if(c==(char)ESCESC)
					p[i-1] = (char)ESCCHAR;
				sr_state=ReadingData;
				break;
			}
		if ((s_read==s_write)&&(sr_state!=ReadOneFrameFinish))
		{
			s_read=tmpRead;
			i=0;
			return FALSE;
		}
	}
	return FALSE;
}
*/


/*
	Based on "SLIP Protocol" , RFC 1055
	call: Uart0_SendByte(), send a byte via uart0, user cpu related
*/
void UARTSendPkt(char *p,int len)
{
	while(len--) {
	  UARTSendByte(p);
	  p++;
	}
}


void UARTSendByte(char *p)
{
	switch(*p){
	case (char)FRAMES:
	  Serial_Transmit(ESCCHAR);
	  Serial_Transmit(FRAMESESC);
	  break;
	case (char)FRAMEE:
	  Serial_Transmit(ESCCHAR);
	  Serial_Transmit(FRAMEEESC);
	  break;
	case (char)ESCCHAR:
	  Serial_Transmit(ESCCHAR);
	  Serial_Transmit(ESCESC);
	  break;
	default:
	  Serial_Transmit(*p);
	}
}



//==================================
// File name:
//   dddd_yyyy-mm-dd hh-mm-ss.hyd
//==================================
void GetFileName(BYTE *filename)
{
UINT uID;
BYTE *p;

//Read station id
 mem_ReadData(M_CFG,REG0+CM_STA_ID,(BYTE *)&uID,2);	
	p=filename;
	sprintf((char *)filename,"%04d",uID);

	filename+=4;				//skip "dddd"
	*filename++='_';
	GetCurDateTime((char *)filename);	//with ':'s in filename
	while(*p!=0){				//replace all ':' with '-'
	  if(*p==':'){*p='-';}
	  p++;
	}
//tail
	*p++='.'; *p++='h'; *p++='y';*p++='d'; *p++=0x0;
}


BYTE SetHostBaudrate(void)
{
BYTE command[8];
char i;
BYTE *rx;
	rx=RX_BUF0();

	FillBufWith(&command[0],8,0x0);

	command[0]=0x01;
	command[1]=0xf0;
	command[2]=0x05;		//57600
//	command[2]=0x06;		//115200

	Serial_Transmit(FRAMES);
	UARTSendPkt(((char *)&command[0]),8);
	Serial_Transmit(FRAMEE);
	while(!TRMT()){continue;}

//��Ӧ
	Serial_Setup(BAUD_57600);
//	Serial_Setup(BAUD_115200);
	i=GetUsbDiskResp();
	if(i==OK){							//��ʾ�豸����Ӧ
	  if(*(rx+3)==1){return USB_CLOSE_DISK_OK;}			
	  else{								//ʧ��
		return USB_FAILED;
	  }
	}
	else{return USB_NO_RESPONSE;}
}




BYTE OpenUSBDisk(void)
{
BYTE command[64];
char i;
BYTE *rx;
	rx=RX_BUF0();

	FillBufWith(&command[0],64,0x0);

	command[0]=0x01;
	command[1]=0x02;

	Serial_Transmit(FRAMES);
	UARTSendPkt(((char *)&command[0]),8);
	Serial_Transmit(FRAMEE);

//��Ӧ
	i=GetUsbDiskResp();
	if(i==OK){							//��ʾ�豸����Ӧ
	  if(*(rx+3)==1){return OK;}			
	  else{								//ʧ��
		return USB_FAILED;
	  }
	}
	else{return USB_NO_RESPONSE;}
}

BYTE CloseDisk(void)
{
BYTE command[8];
char i;
BYTE *rx;
	rx=RX_BUF0();

	FillBufWith(&command[0],8,0x0);

	command[0]=0x01;
	command[1]=0x03;

	Serial_Transmit(FRAMES);
	UARTSendPkt(((char *)&command[0]),8);
	Serial_Transmit(FRAMEE);

//��Ӧ
	i=GetUsbDiskResp();
	if(i==OK){							//��ʾ�豸����Ӧ
	  if(*(rx+3)==1){return USB_CLOSE_DISK_OK;}			
	  else{								//ʧ��
		return USB_FAILED;
	  }
	}
	else{return USB_NO_RESPONSE;}
}



char CreatFile(BYTE *filename)
{
BYTE command[64];
int i;
BYTE *rx;
	rx=RX_BUF0();

	FillBufWith(&command[0],64,0x0);

	command[0]=0x01;
	command[1]=0x30;
	command[2]=0x02;	//���Ƿ�ʽ

	i=8;
	command[i++]='C';
	command[i++]=':';
	command[i++]=0x5c;	//'\';

	while(1){
	  command[i++]=*filename;
	  if(*filename==0){break;}
	  filename++;
	}				//i=length of OpenFile command

	Serial_Transmit(FRAMES);
	UARTSendPkt(((char *)&command[0]),i);
	Serial_Transmit(FRAMEE);

//��Ӧ
	i=GetUsbDiskResp();
	if(i==OK){			//��ʾ�豸����Ӧ
	  if(*(rx+3)==1){return USB_CREAT_FILE_OK;}
	  else{	//ʧ��
		if(*(rx+4)==4){return USB_NO_DEVICE;}
		else if(*(rx+4)==11){return USB_DEVICE_FULL;}
		else if(*(rx+4)==25){return USB_FILE_EXIST;}
		return USB_FAILED;
	  }
	}
	else{return USB_NO_RESPONSE;}
}


char GetUsbDiskResp(void)
{
BYTE 	c,i;
UINT	iCmp;
BYTE *rx;
	rx=RX_BUF0();

	ClrUErr();
	while(URXDA()){c=URXREG();}		//clear FIFO
	
	TX_IE_OFF();	
	RX_IE_OFF();	

	iCmp=ms_click+2000/2;	//LimitMS/2,���2s;

	FillBufWith(rx,64,0x88);
	i=0;
	while(1){
      ClrWdt();

	  if(URXDA()){		
    	c=URXREG();		
		if(i==0){if(c!=0xC0){continue;}}	
		if(i<64){rx[i++]=c;}
		if(c==0xE0){
		  return TRUE;
		}
    	iCmp=ms_click+WITHIN_1S/2;	//�յ�1���ַ��󣬵ȴ�ʱ������Ϊ���1s.
	  }
	  if(IsTimeUp(iCmp)==TRUE){
		return FALSE;
	  }
	}
}

void WriteFile(UINT len)
{
BYTE command[8];

	FillBufWith(&command[0],8,0x0);
	command[0]=0x01;			
	command[1]=0x33;			//WriteFile
	command[2]=file_id;			
	command[3]=LoOfInt(len);
	command[4]=HiOfInt(len);

	UARTSendPkt(((char *)&command[0]),8);
}



BYTE CloseFile(void)
{
BYTE command[8],i;
BYTE *rx;
	rx=RX_BUF0();

	FillBufWith(&command[0],8,0x0);
	command[0]=0x01;			
	command[1]=0x31;			//CloseFile
	command[2]=file_id;			

	Serial_Transmit(FRAMES);
	UARTSendPkt(((char *)&command[0]),8);
	Serial_Transmit(FRAMEE);

//��Ӧ
	i=GetUsbDiskResp();
	if(i==OK){			//��ʾ�豸����Ӧ
	  if(*(rx+3)!=1){return USB_CLOSE_FILE_FAILED;}
	  //ok...go CloseDisk
	  command[1]=0x03;			//CloseDisk
	  command[2]=0;			

	  Serial_Transmit(FRAMES);
	  UARTSendPkt(((char *)&command[0]),8);
	  Serial_Transmit(FRAMEE);

	  i=GetUsbDiskResp();
	  if(i==OK){			//��ʾ�豸����Ӧ
	    if(*(rx+3)==1){return USB_CLOSE_FILE_OK;}
	  }
	}

	return USB_CLOSE_FILE_FAILED;
}


//========================================
char ExportAllRecords(void)
{
BYTE iC0,iCx;				//,yn;
UINT addr_rec_ptr,u;
UINT top,bottom;
BYTE err_code;
UINT c0,cx;

	addr_rec_ptr=mapPTR_REC0;
	err_code=0;

//Loop through all ports,Read corresponding belts
	for(iC0=0;iC0<C0S;iC0++){
//	  c0_off=c0Table[iC0];					//��Ԫƫ��
//	  c0=((UINT)c0_off)*lenCELL;			//��Ԫ�׵�ַ
	  c0=getC0(iC0);

	  //Skip����Ҫ����Ļ�Ԫ
	  //yn=cfg[c0+C0_YN1];
	  if(!IsC0Enabled(&cfg[c0])){continue;}		//C0ʹ��
	  if(!IsC0Recorded(&cfg[c0])){continue;}	//C0��¼ʹ��

	  for(iCx=0;iCx<cfg[c0+C0_CXS];iCx++){
		cx=getCX(iC0,iCx);
	    //cx=((UINT)(cTable[c0_off+iCx]))*lenCELL;
	    if(IsCxRecorded(&cfg[cx])){						//if should be recorded
		  top=getUINT(&cfg[cx+CX_REC_ENTRY])*REC_BLOCK_SIZE;			
		  bottom=getUINT(&cfg[cx+CX_REC_EXIT])*REC_BLOCK_SIZE;			

		  //дREC
		  WriteTitle(UFILE_SEG_REC,cx);
		  u=ExportCxRec(M_MEM,addr_rec_ptr,top,bottom);
		  if(u!=USB_WRITE_CX_OK){
			err_code=1;
			goto END_EXPORT_RECS;}		//write abort
		}
  		addr_rec_ptr+=0x4;				//next rec_ptr. ���ȣ�4
	  }  
  	}	
//other type of ports
//...	  
	return OK;

END_EXPORT_RECS:
	return FAILED;
}


BYTE WriteTitle(BYTE type,UINT cx)
{
BYTE title[30];
char i;
int len;
BYTE *rx;
	rx=RX_BUF0();

	len=GetUFileSegTitle(type,cx,&title[0]);
//
	Serial_Transmit(FRAMES);				//transmission start
	WriteFile(len);
	UARTSendPkt((char *)&title[0],len);
	Serial_Transmit(FRAMEE);				//transmission end

//��Ӧ
	i=GetUsbDiskResp();
	if(i==OK){							//��ʾ�豸����Ӧ
	  if(*(rx+3)==1){return OK;}			//δ���ף���һ����addrRec��ʼ
	  else{								//ʧ��
		return USB_FAILED;
	  }
	}
	else{return USB_NO_RESPONSE;}
}




//====================================
// �������������title
// ����:
//		���ⳤ��. 
//====================================
UINT GetUFileSegTitle(BYTE type,UINT cx,BYTE *title)
{
BYTE *p,i;
BYTE *cx_name,iC0,iCx;
UINT len;

	p=title;
	*p++='['; 			//[
	if(type==UFILE_SEG_CFG){
		*p++='c'; 		//[cfg]
		*p++='f'; 
		*p++='g';
	}
	else{
	  if(type==UFILE_SEG_REC){
		*p++='r'; 		//[rec]
		*p++='e'; 
		*p++='c';
	  }
/*	  else if(type==UFILE_SEG_XMEM){
		*p++='x'; 		//[xme]
		*p++='m'; 
		*p++='e';
	  }*/
	  else{;}

//��ʾcx����
	  *p++=':';
	  cx_name=&cfg[cx+CX_NAME];
	  for(i=0;i<16;i++){
		*p++=*cx_name++;					//Name
	  }
	  *p++='/';							//'/'

//��ʾ"iC0,iCx"
	  iCx=cfg[cx+CX_INDEX];
	  iC0=cfg[cx+CX_INDEX+1];
	  *p++=HexHiByte(iC0); *p++=HexLoByte(iC0);
	  *p++=HexHiByte(iCx); *p++=HexLoByte(iCx);
	}

//"]CRLF"
	*p++=']'; *p++=CR; *p++=LF;
	len=p-title;			
	return len;
}

//===================================
// Return:
//
//===================================
UINT ExportCxRec(BYTE Slave,UINT addr_rec_ptr,UINT recTOP,UINT recBOTTOM)
{
BYTE BeginByte;
UINT addr,uAddr,top,bottom;

//BYTE s1,s2;
//UINT ui;

//Read pointer
    mem_ReadData(M_MEM,addr_rec_ptr,(BYTE *)&uAddr,2);

	
//
	top=recTOP*REC_BLOCK_SIZE;
	bottom=recBOTTOM*REC_BLOCK_SIZE;

//Check validation
	if((uAddr<top)||(uAddr>bottom))
	{
	  //Add recovery procedure
	  return 0;	
	}
	AddWith(uAddr,7,top,bottom)	//skip to end-of-time-stamp

//#ifdef DEBUG_ON
//��������ʱ
//	uAddr=0x7FFE;	
//	top=0x800;
//	bottom=0x7FFF;
//	R4571_Read_Data(ADDR_CLOCK0,&s1,1);//ʱ���Ѿ��޸�
//#endif


//BeginByte==0xBF or not? determine read method
  	mem_ReadData(M_MEM,top,&BeginByte,1);	
//
	if(BeginByte==0xBF){		//����һҳ
	  addr=top;
//	  ui=0;
	  while(1){
//		ui++;
	    addr=ExportOnePage(Slave,addr,uAddr,TRUE);
	   	if(addr==USB_TOUCH_CX_BOTTOM){goto USB_CX_OK;}
		ProgressBar();
		if(addr<0x10){return addr;}				//error code < 0x10
	  }
	}
	else{						//��һҳ
	  addr=uAddr;
	  while(1){					//���°��
	    addr=ExportOnePage(Slave,addr,bottom,FALSE);	//not end
	   	if(addr==USB_TOUCH_CX_BOTTOM){break;}
		ProgressBar();
		if(addr<0x10){return addr;}				//error code < 0x10
	  }
	  addr=top;
	  while(1){					//���ϰ��
	    addr=ExportOnePage(Slave,addr,uAddr,TRUE);		//mark end
	   	if(addr==USB_TOUCH_CX_BOTTOM){goto USB_CX_OK;}
		ProgressBar();
		if(addr<0x10){return addr;}				//error code < 0x10
	  }
	}
USB_CX_OK:
//    R4571_Read_Data(ADDR_CLOCK0,&s2,1);//ʱ���Ѿ��޸�
	return USB_WRITE_CX_OK;
}

//========================
// progress bar
//========================
void ProgressBar(void)
{
char bar[6];
	if(pb_count<0){pb_count=0;}
	sprintf(&bar[0],"%d",pb_count++);
	lcd_puts(11,L2,"      ");				//clear
	lcd_puts(11,L2,(const char *)&bar[0]);   //fill  
}

//=================================
// ��FRAM�洢����1ҳ,�����U��
//
// end_mark:
//	�ڱ�ҳ�������ʱ���Ƿ���ӻس�
//
// return:
//	address: {>=1k}address of next page-start
//  other resulut code:{<1k}
//		USB_NO_DEVICE
//		USB_FAILED
//		USB_NO_RESPONSE
//=================================
UINT ExportOnePage(BYTE Slave,UINT addrRec,UINT addrEnd,BYTE end_mark)
{
char i;
BYTE c,half,*p;
UINT count;
UINT len,lenFile;
BYTE *rx;

	rx=RX_BUF0();

//Wait for XMem idle.
	WaitMemReady(Slave);

    cs(Slave);
	spi_WriteByte(SPI_MEM_READ_DATA);
	p=(BYTE *)(&addrRec);
	spi_WriteByte(*(p+1));
	spi_WriteByte(*p);

//=====================
//transmission start
	Serial_Transmit(FRAMES);

	len=(addrEnd-addrRec)*2;
//	len=(addrEnd-addrRec);
//	if(len>2046){len=2046; lenFile=2046;}		//no [CR] added
	if(len>FILE_WRITE_LIMIT){
	  len=FILE_WRITE_LIMIT; 
	  lenFile=FILE_WRITE_LIMIT;		//no [CR] added
	}		
	else{
	  if(end_mark==TRUE){lenFile=len+2;}
	  else{lenFile=len;}
	}

	WriteFile(lenFile);
//
	count=len>>1;		//devided by 2
//	count=len;			//devided by 2
	while(count--){
	  c=spi_ReadByte();

	  half=HexHiByte(c); UARTSendByte((char *)&half);				//to USB
	  half=HexLoByte(c); UARTSendByte((char *)&half);				//to USB
//	  UARTSendByte((char *)&c);

	  addrRec++;
	}
//	
	if(addrRec==addrEnd){					//����
	  if(end_mark==TRUE){					//
	    c=0x0a;								//[CR]
	    UARTSendByte((char *)&c);			//to USB
	    c=0x0d;								//[CR]
	    UARTSendByte((char *)&c);			//to USB
	  }
	  addrRec=USB_TOUCH_CX_BOTTOM;
	}	

	Serial_Transmit(FRAMEE);
    cd();
//transmission end
//======================

	i=GetUsbDiskResp();
/*note: addrRec<1k is reserved. so no conflict between
addrRec and usb-return as USB_XXX_XXX.*/
	if(i==OK){							//��ʾ�豸����Ӧ
	  if(*(rx+3)==1){return addrRec;}		//δ���ף���һ����addrRec��ʼ
	  else{								//ʧ��
//		if(rx[4]==0x01){return USB_NO_DEVICE;}
		return USB_FAILED;
	  }
	}
	else{
	  return USB_NO_RESPONSE;
	}
}


//============================
// �����վCfg
//============================
char ExportCfg(void)
{
UINT addr,uAddr;
BYTE Slave;		
UINT null=0; //����Ҫcx����

//[cfg][CR][LF]
	WriteTitle(UFILE_SEG_CFG,null);		//����Ҫcx����

//дcfg
	Slave=M_CFG;
	addr=REG0;

//cfgβ����ַ
	uAddr=((UINT)cfg[CM_LEN_CFG])*lenCELL;
//	numCx=GetByte(M_CFG,MAP_NUM_CX);
//	uAddr=AddressOfCxBegin(numCx)+lenCELL;		//cfgβ����ַ
//
	while(1){
	  addr=ExportOnePage(Slave,addr,uAddr,TRUE);
	  if(addr==USB_TOUCH_CX_BOTTOM){return USB_WRITE_CFG_OK;}
	  if(addr<0x10){return USB_WRITE_CFG_FAILED;}		//error code < 0x10
	}
}




//================================================================
//
// 1. file extension name start from '.001', and retry as many as 
//	MAX_RETRY_TIMES times if necesary 
// 2���������ж�Level�����T2һ������Calendarͬ����˼�¼ָ�벻�ᱻ
// ��;�޸ġ�lcd�к���ei()Ӧȥ����
//================================================================
void SaveToUsbDisk(void)
{
char r;
BYTE FileName[30];			//0100_2005-12-31 11-05-46.hyd'/0'
BYTE cTmp;
BYTE err_code;
UINT ipl_save;
BYTE *rx;
BYTE com_save;

	di(ipl_save,IPL_T2_INT-1);

	com_save=ActiveCom;
	ActiveCom=COM2;

	MsgBox("����U��...              ");

	PW_NOFF_on();
	Delay_s(4);

	TX_IE_OFF();			//TX2_IE=FALSE;
	CALENDAR_IE=FALSE;
	RX1_IE=FALSE;	

	RX_IE_OFF();			//RX2_IE=FALSE;

	blight_off();	//�رձ��⣬��ֹ��Դ����

	RS232_on();		//SLEEP2_DIR=OUT; SLEEP2=1;	//wake-up MAX242

	Serial_Setup(BAUD_57600);
//	Serial_Setup(BAUD_115200);
//	SetHostBaudrate();

	rx=RX_BUF0();

	err_code=0;
	r=OpenUSBDisk();
	if(r!=OK){
	  lcd_puts(0,L2,"�豸δ��Ӧ              ");       
	  err_code=5; 
	  goto USB_END;
	}

	lcd_init(); 
 	lcd_puts(0,L0,"�����ļ�:               ");       
	GetFileName(&FileName[0]);
	cTmp=FileName[15]; FileName[15]=0;				//�ָ�filename
 	lcd_puts(0,L1,(const char *)&FileName[0]);  	//��ʾ�ļ���ǰ�벿��
    FileName[15]=cTmp;								//�ָ��ļ���
 	lcd_puts(0,L2,(const char *)&FileName[15]); 	//��ʾ�ļ�����벿��
//
	r=CreatFile(&FileName[0]);
	switch(r){
	  case USB_NO_DEVICE:
 	    lcd_puts(0,L2,"����δ����              ");       
	    err_code=6; goto USB_END;
	  case USB_DEVICE_FULL:
 		lcd_puts(0,L2,"��������                ");       
	    err_code=7; goto USB_END;
	  case USB_NO_RESPONSE:
 		lcd_puts(0,L2,"�豸δ��Ӧ              ");       
	    err_code=8; goto USB_END;
	  case USB_FAILED:
 		lcd_puts(0,L2,"�豸������Ӧ            ");       
	    err_code=9; goto USB_END;
//	  case USB_FILE_EXIST:
//		continue;
	  case USB_CREAT_FILE_OK:
		//creat file ok
		file_id=*(rx+5);
 		lcd_puts(0,L2,"�������...             ");       
//
		pb_count=0;
		if(ExportCfg()==USB_WRITE_CFG_OK){
		  if(ExportAllRecords()==OK){
		    if(CloseFile()==USB_CLOSE_FILE_OK){
			  if(CloseDisk()==USB_CLOSE_DISK_OK){
 		        lcd_puts(0,L3,"���, ��ȡ���豸        ");       
			    goto USB_END;
			  }
			  else{err_code=1; goto USB_END;}
		    }
		    CloseDisk();
			err_code=2; goto USB_END;
		  }
		  err_code=3; goto USB_END;
		}
		err_code=4; goto USB_END;
 		lcd_puts(0,L3,"ʧ��                    ");
		goto USB_END;
	}

 	lcd_puts(0,L0,"�����ļ�ʧ��            ");       
//

USB_END:
	blight_on();		//�ָ�����

	TX1_IE=FALSE;
	RX2_IE=TRUE;
	RX1_IE=TRUE;
	CALENDAR_IE=TRUE;
	if(err_code!=0){
	  Nop();
	} 

	ActiveCom=com_save;
	ei(ipl_save);		//SET_CPU_IPL(0);		//	ei();
}

