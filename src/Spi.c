//���´�����N25P16   FM25L256
//           24λ      16λ
//����    N25P16  ��FM25L256����һ��
//      
//
//    N25P16
//       BYTE xm_SectorErase( long addr)         ɾ��
//		BYTE xm_ReadData( long addr,char *data,int len)  ��ȡ
//		void xm_PageProgram( long addr,char *data,int len) д��
//      BYTE xm_CheckWriting( long addr,char *data,int len)���д���Ƿ���ȷ
//    FM25L256
//		BYTE MEM_ReadData(char Slave, long addr,char *data,int len)  ��ȡ
//		void MEM_WriteData(char Slave, long addr,char *data,int len) д��
//      BYTE MEM_CheckWriting(char Slave, long addr,char *data,int len)���д���Ƿ���ȷ
#include 	"header.h"
#include    "h_macro.h"

//Simulation
#ifdef SIM_MEM
BYTE simMEM[REC_SIM_BOTTOM+1-REC_SIM_TOP];	//4k
#endif

extern BYTE 	IPL_CPU;

//===========================
// дһ��byte
//===========================
void spi_WriteByte(BYTE output)
{
char BitCount; 

    BitCount=8;
    do{         
	  delay_WIDTH_CLK_SPI;			// delay a mininum time gap
      M_SPI_ClkOut=0;       		// Clear the SCK pin
//
      if(output&0x80)M_SPI_DI = 1;	// Set Dout to the next bit according to the MSB of data
      else M_SPI_DI=0;
      output=output*2;

	  delay_WIDTH_CLK_SPI;			// delay a mininum time gap
	  M_SPI_ClkOut=1;      			// data out, Set the SCK pin
    } while(--BitCount);
	delay_WIDTH_CLK_SPI;			// delay a mininum time gap
}

//===========================
// ��1��byte
//===========================
BYTE spi_ReadByte(void)
{
char BitCount;
BYTE input;

	BitCount=8;
	input=0;
    do{                             
      M_SPI_ClkOut = 0;       		// DO shift out, Clear the SCK pin
	  delay_WIDTH_CLK_SPI;			// delay a mininum time gap
	  input=input*2;
      if(DO_XME)input++;

	  M_SPI_ClkOut = 1;      		// Set the SCK pin
	  delay_WIDTH_CLK_SPI;			// delay a mininum time gap
    }while(--BitCount);
	return input;
}


void mem_WriteEnable(char Slave)
{
	cs(Slave);
	spi_WriteByte(SPI_MEM_WRITE_ENABLE);//spi_WriteByte(0x06)
//	cd();
}



void mem_WriteDisable(char Slave)
{
	cs(Slave);
	spi_WriteByte(SPI_MEM_WRITE_DISABLE);
//	cd();
}



BYTE mem_ReadStatus(char Slave)
{
BYTE i;

	cs(Slave);
	spi_WriteByte(SPI_MEM_READ_STATUS);
	i=spi_ReadByte();
	cd();
	return i;
}



void mem_WriteStatus(char Slave,BYTE status)
{
//
	cs(Slave);
	spi_WriteByte(SPI_MEM_WRITE_STATUS);
	spi_WriteByte(status);
	cd();
}


void xm_SectorErase(ULNG addr)
{
BYTE *p;//busy;
//int i;

//Wait for XMem idle.
	WaitMemReady(M_XME);
//
	mem_WriteEnable(M_XME);

	cs(M_XME);
//
	spi_WriteByte(XME_SECTOR_ERASE);
	p=(BYTE *)(&addr);
	spi_WriteByte(*(p+2));
	spi_WriteByte(*(p+1));
	spi_WriteByte(*p);
//
	cd();
//	Nop4();
}


/*
//===============================
// �������
// 
//===============================
BYTE xm_BulkErase(void)
{

//Wait for XMem idle.
	WaitXmReady();
//
	xm_WriteEnable();
//
	CS_XMEM=0;
	spi_WriteByte(XMEM_BULK_ERASE);
	CS_XMEM=1;
//	Nop4();

//Wait for XMem idle.
//	while(testbit0(xm_ReadStatus())){continue;}
}
*/


void mem_ReadData(char Slave,ULNG src_addr,BYTE *des,int len)
{
BYTE *p;
int i;
UINT ipl_save;
	di(ipl_save,7);

//�ڴ�ģ��M_MEM
#ifdef SIM_MEM
	if(Slave==M_MEM){
	  p=&simMEM[(UINT)src_addr];
	  for(i=0;i<len;i++){
	    *des++=*p++;
	  }
	}
	ei(ipl_save);
	return;
#endif



//Wait for XMem idle.
	WaitMemReady(Slave);

    cs(Slave);
	spi_WriteByte(SPI_MEM_READ_DATA);
	p=(BYTE *)(&src_addr);
	if(Slave==M_XME){spi_WriteByte(*(p+2));}
	spi_WriteByte(*(p+1));
	spi_WriteByte(*p);

//forѭ���������do����while������
	for(i=0;i<len;i++){
	  *des++=spi_ReadByte();
	}
//
	cd();

	ei(ipl_save);
}

void FramLock(char Slave)
{
	mem_WriteEnable(Slave);
	mem_WriteStatus(Slave,0x8C);
	mem_WriteDisable(Slave);
}

void FramUnlock(char Slave)
{
	mem_WriteEnable(Slave);
	mem_WriteStatus(Slave,0x00);
}



void mem_WriteData(char Slave,BYTE *src,ULNG des_addr,int len)
{
BYTE *p;
int i;
UINT ipl_save;
	di(ipl_save,7);

//�ڴ�ģ��M_MEM
#ifdef SIM_MEM
	if(Slave==M_MEM){
	  p=&simMEM[(UINT)des_addr];		
	  for(i=0;i<len;i++){
	    *p++=*src++;
	  }
	}

	ei(ipl_save);
	return;
#endif


//����
	if((Slave==M_CFG)||(Slave==M_MEM)){
	  FramUnlock(Slave);
	}

//Wait for XMem idle.
	WaitMemReady(Slave);
	mem_WriteEnable(Slave);
//
	cs(Slave);
	spi_WriteByte(SPI_MEM_PAGE_PROGRAM);
	p=(BYTE *)(&des_addr);
	if(Slave==M_XME){spi_WriteByte(*(p+2));}
	spi_WriteByte(*(p+1));
	spi_WriteByte(*p);

	for(i=0;i<len;i++){
	  spi_WriteByte(*src++);
	  Nop();
	  Nop();	
	}
//
	cd();

//����
	if((Slave==M_CFG)||(Slave==M_MEM)){
	  FramLock(Slave);
	}

	ei(ipl_save);
//	Nop4();
}



BYTE mem_CheckWriting(char Slave,BYTE *data,ULNG addr,int len)
{
BYTE *p,tmp;
int i;

#ifdef SIM_MEM
	return TRUE;
#endif


//Wait for XMem idle.
	WaitMemReady(Slave);
//
	cs(Slave);
//
	spi_WriteByte(SPI_MEM_READ_DATA);
	p=(BYTE *)(&addr);
	if(Slave==M_XME){spi_WriteByte(*(p+2));}
	spi_WriteByte(*(p+1));
	spi_WriteByte(*p);

//
	for(i=0;i<len;i++){
	  tmp=spi_ReadByte();
	  if(*data!=tmp){
		cd();
		goto SAVE_ERR;
	  }
	  data++;
	}
	cd();

	return TRUE;

SAVE_ERR:		//FRAM�洢�������
	if(Slave==M_CFG){IncCount(mapCNT_FRAM_CFG_FAIL);}
	else if(Slave==M_MEM){IncCount(mapCNT_FRAM_MEM_FAIL);}
	return FALSE;
}




void WaitMemReady(char Slave)
{
//	ClrWdt();
	while(testbit0(mem_ReadStatus(Slave))){continue;}
}


//========================================================
// ����Sector Erase��ʱ��ϳ�(���1.5s),���ܳ���Watchdog
// ���ڳ���(��С0.9s), ���Ե������ǵȴ�����.
//========================================================
void WaitXmReadyForErase(void)
{
char i;

//delay 0.6s(XME���Sector���ͺ�ʱ)
	for(i=0;i<6;i++)
{Delay_ms(100);}
//
	while(testbit0(mem_ReadStatus(M_XME))){continue;}
}







//========================================================
// ����Sector Erase��ʱ��ϳ�(���1.5s),���ܳ���Watchdog
// ���ڳ���(��С0.9s), ���Ե������ǵȴ�����.
//========================================================
void WaitMemReadyForErase(char Slave)
{
char i;

//delay 0.6s(XMEM���Sector���ͺ�ʱ)
	for(i=0;i<6;i++){Delay_ms(100);}
//
	while(testbit0(mem_ReadStatus(Slave))){continue;}
}


//===================================
// Slave: M_CFG/M_MEM
// sw: 	0(ѡ��)
//		1(�ر�)
//===================================
void cs(char Slave)		//chip select 
{
	CS_CFG_DIR=OUT;	CS_CFG=1; 
	CS_MEM_DIR=OUT;	CS_MEM=1; 
	CS_XME_DIR=OUT;	CS_XME=1;

	if(Slave==M_CFG){CS_CFG=0; return;}
	if(Slave==M_MEM){CS_MEM=0; return;}
	if(Slave==M_XME){CS_XME=0; return;}
}

void cd(void)		//chip deselect
{
	CS_CFG_DIR=OUT;	CS_CFG=1; Nop();
	CS_MEM_DIR=OUT;	CS_MEM=1; Nop();
	CS_XME_DIR=OUT;	CS_XME=1; Nop();
}

//=================================================
// ����¼
//
// uAddr: ��ʼ��ַ 
// uLen�� ���� 
//=================================================
void ReadRecArray(UINT uAddr,BYTE *buf,UINT uLen,UINT top,UINT bottom)
{
	if((uAddr+uLen-1)>bottom){		//���
	  mem_ReadData(M_MEM,uAddr,buf,bottom-uAddr+1);
	  buf+=bottom-uAddr+1;
	  mem_ReadData(M_MEM,top,buf,uLen-(bottom-uAddr+1));
	}
	else{	//�����
	  mem_ReadData(M_MEM,uAddr,buf,uLen);
	} 
}


