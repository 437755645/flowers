//PIC24FJ256GA106��CH376 SPI  Ӳ�������ӿ�DEBUG.C

//Ǯ�� 2011.01.13
/*********************************************************************
* REVISION HISTORY: CH376оƬ Ӳ������� V1.0
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date          Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Ǯ �˳�               14/01/11     First release of source file
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*CH376оƬ Ӳ������� V1.0
* ADDITIONAL NOTES:
* Code uses the PERIPHERAL LIBRARY support available with MPLAB C30 Compiler
* Code Tested on:*PIC24FJ256GA106 
* The Processor starts with the External Crystal (11.059 Mhz).
*
*********************************************************************/

#include "header.h"
#include "h_macro.h"

extern BYTE cfg[];
extern SW	sw1;


/*************************configure for SW spi ************************/
void Init_SPI_ch376(void)
{
#ifdef NEWBORAD120913 

    mPORTDInputConfig(IOPORT_BIT_9);    //set RD9 as input    port for SD_INT 
    mPORTDOutputConfig(IOPORT_BIT_10);    //set RD10 as input    port for   SPI_SCS
    #ifndef HW_SPI
        mPORTBOutputConfig(IOPORT_BIT_15);    //set RE2 as OUTput port for  SPI_SCK
        mPORTFOutputConfig(IOPORT_BIT_4);    //set RF4 as OUTput port for SI
        mPORTFInputConfig(IOPORT_BIT_5);    //set RF5 as OUTput port for   SO // 
    #endif
    mPORTDInputConfig(IOPORT_BIT_11);    //set RD11 as input    port for SD_INSERT 
    mPORTDOutputConfig(IOPORT_BIT_0);    //set RF0 as OUTput    port for POW-CNT

#else
    mPORTEInputConfig(IOPORT_BIT_4);    //set RE4 as input    port for SD_INT
    mPORTEOutputConfig(IOPORT_BIT_3);    //set RE3 as input    port for   SPI_SCS
    mPORTEOutputConfig(IOPORT_BIT_2);    //set RE2 as OUTput port for  SPI_SCK
    mPORTEOutputConfig(IOPORT_BIT_1);    //set RE1 as OUTput port for SPI_SDO
    mPORTEInputConfig(IOPORT_BIT_0);    //set RE0 as OUTput port for   SPI_SDI
    mPORTFInputConfig(IOPORT_BIT_1);    //set RF1 as input    port for SD_INSERT
    mPORTFOutputConfig(IOPORT_BIT_0);    //set RF0 as OUTput    port for POW-CNT
#endif    
}


//PIC24FJ256 Main control board Led1: horse lamp function
void vLedTest_ms(UINT8 uchDelay) 
{
//    LED_toggle();
    Delay_ms(uchDelay);    
}

void vLedTest_s(UINT8 uchDelay) 
{    
//    LED_toggle();
    Delay_s(uchDelay);    
}

/******************************************************************************
// дһ��byte
******************************************************************************/
/*void CfgReg_RE(void)
{
    UINT addr,uAddr;
    UINT i;
    BYTE numCx,b_cfg;

    addr = REG0;    
    //cfgβ����ַ
    numCx = GetByte(M_CFG,CM_CXS);
//    uAddr = AddressOfCxBegin(numCx)+lenCELL;        //cfgβ����ַ  
    uAddr = cfg[CM_LEN_CFG]*lenCELL*2;        //cfgβ����ַ  

    //PRINTF( "numCx:%d\r\n",numCx );
#if 0    
    //PRINTF("CLR M_CFG from 0x%x to 0x%x \r\n",addr,uAddr);  
    for(i=addr;i<uAddr;i++){
        ClrByte(M_CFG,i);            
    }    
#endif    

    //PRINTF("READ M_CFG from 0x%x to 0x%x \r\n",addr,uAddr);  
    for(i=addr;i<uAddr;i++){
        if(0==i%0x10)            
        //PRINTF("\r\n");
        
        b_cfg = GetByte(M_CFG,i);            
        //PRINTF("%02x",b_cfg);            
    }    
}
*/

//=====================================================
// ������״̬,�����������ʾ������벢ͣ��,
// Ӧ���滻Ϊʵ�ʵĴ����ʩ,
// ������ʾ������Ϣ,�ȴ��û�ȷ�Ϻ����Ե� 
//=====================================================
void mStopIfError( UINT8 iError )
{
BYTE   fReset=0;
BYTE   ErrMsg[8];
    
    if(iError==USB_INT_SUCCESS)return;  /* �����ɹ� */
    
    
    PW_LCD_on();
    lcd_init();        //lcd��ʼ��
    lcd_clear();
    
    sprintf((char *)ErrMsg,"0x%x",iError);  
    lcd_puts(0,L0,"TF Card Failed!");
    lcd_puts(0,L1,"Error code:");
    lcd_puts(12,L1,(const char*)ErrMsg);     
    lcd_puts(0,L0,"���ȳ�ʼ�����TF���Ƿ���!");
    
    if(iError==USB_INT_DISCONNECT){
        lcd_puts(0,L2,"���TF���Ƿ���!");  
        return;       
    }   
    
    if(iError==0x42){
        lcd_puts(0,L2,"TF���ڳ�ʼ��...");  
        
        InitCardFiles();
        Restart(RST_FOR_ERR_MAIN_14);
        return;       
    }

//��¼1��CH376ByteLocate()   
    if(iError==0xB4){
	  fReset++;			//ERR_FILE_CLOSE       0xB4            /* �ļ��Ѿ��ر�,�����Ҫʹ��,
	  //while(1){;}
	}

//��¼1��CH376ByteLocate()   
    if(iError==0xFA){
	  fReset++;			//ERR_USB_UNKNOWN        0xFA              /* �ļ��Ѿ��ر�,�����Ҫʹ��,
	  //while(1){;}
	}

    //PRINTF( "Error: %02X\n", (UINT16)iError );  /* ��ʾ���� */

    lcd_puts(0,L2,"10�������:");   
    
    while(1){
        vLedTest_ms( 250 );  /* LED��˸ */
        fReset++;
        if(fReset%4==3){   
            sprintf((char *)ErrMsg,"%d",10 - fReset/4);  
            lcd_puts(12,L2,(const char*)ErrMsg);    
        }   
        
        if(fReset>40){
			fReset++;
            Restart(RST_FOR_ERR_MAIN_14);
            fReset=0;
            break;
        }        //Уʱ���,����
    }
}


/* ����ʹ�����ģ��SPI��дʱ��,���Խ��г�ʼ�� */
void CH376_PORT_INIT(void)  
{        
    CH376_SPI_SCS = 1;  /* ��ֹSPIƬѡ */
    CH376_SPI_ClkOut = 1;  //Ĭ��Ϊ�ߵ�ƽ
}
/******************************************************************************
// дһ��byte
******************************************************************************/
void spi_WriteByte_ch376(BYTE output) //SPI���8��λ���� 
{
    char BitCount; 

    BitCount=8;
    do{         
      delay_WIDTH_CLK_SPI_376;            // delay a mininum time gap
      CH376_SPI_ClkOut=0;               // Clear the SCK pin
//
      if(output&0x80)CH376_SPI_DI = 1;    // Set Dout to the next bit according to the MSB of data
      else CH376_SPI_DI=0;
      output<<=1;                     // ����λ�Ǹ�λ��ǰ 

      delay_WIDTH_CLK_SPI_376;            // delay a mininum time gap
      CH376_SPI_ClkOut=1;                  // data out, Set the SCK pin, ��ʱ�������ز���
    } while(--BitCount);
    delay_WIDTH_CLK_SPI_376;            // delay a mininum time gap
}
//===========================
// ��1��byte
//===========================
BYTE spi_ReadByte_ch376(void)
{
    char BitCount;
    BYTE input;

    BitCount=8;
    input=0;
    do{                             
      CH376_SPI_ClkOut = 0;               // DO shift out, Clear the SCK pin,
                                  //CH376��ʱ���½������
      delay_WIDTH_CLK_SPI_376;            // delay a mininum time gap
      input<<=1;                  // ����λ�Ǹ�λ��ǰ
      if(CH376_SPI_Do)input++;

      CH376_SPI_ClkOut = 1;              // Set the SCK pin
      delay_WIDTH_CLK_SPI_376;            // delay a mininum time gap
    }while(--BitCount);
    return input;
}



//#define    xEndCH376Cmd( )    { CH376_SPI_SCS = 1;}  
///SPIƬѡ��Ч,����CH376����,������SPI�ӿڷ�ʽ /
//����CH376����,������SPI�ӿڷ�ʽ /
void xEndCH376Cmd(void)            
{
    CH376_SPI_SCS = 1;
}

/* ��CH376д���� */
void xWriteCH376Cmd(UINT8 mCmd)
{
    CH376_SPI_SCS = 1;  
    /* ��ֹ֮ǰδͨ��xEndCH376Cmd    ��ֹSPIƬѡ */    
    delay_us(2);
    CH376_SPI_SCS = 0;  /* SPIƬѡ��Ч */
    spi_WriteByte_ch376(mCmd);  /* ���������� */    
    delay_us(4);
    /* ��ʱ2uSȷ����д���ڴ���1.5uS */
}


void xWriteCH376Data(UINT8 mData)  /* ��CH376д���� */
{
    spi_WriteByte_ch376( mData );
    delay_us(4);  /* ȷ����д���ڴ���0.6uS */
}
UINT8 xReadCH376Data(void)  /* ��CH376������ */
{
    delay_us(4);  /* ȷ����д���ڴ���0.6uS */
    return(spi_ReadByte_ch376());
}


/* ��ѯCH376�ж�(INT#�͵�ƽ) */
UINT8 Query376Interrupt(void)
{    
    return(CH376_INT_WIRE ? FALSE:TRUE); 
    delay_us(4);
}

UINT8 mInitCH376Host(void)  /* ��ʼ��CH376 */
{
    UINT8 res;
    CH376_PORT_INIT();  
    /* �ӿ�Ӳ����ʼ�� */
    xWriteCH376Cmd(CMD11_CHECK_EXIST); 
    /* ���Ե�Ƭ����CH376֮���ͨѶ�ӿ� */
    xWriteCH376Data(0x65);
    res = xReadCH376Data();
    xEndCH376Cmd();
    if(res!=0x9A)return(ERR_USB_UNKNOWN);  		
	/* ͨѶ�ӿڲ�����,����ԭ����:�ӿ������쳣,�����豸Ӱ��(Ƭѡ��Ψһ),
	���ڲ�����,һֱ�ڸ�λ,���񲻹��� */

    xWriteCH376Cmd( CMD11_SET_USB_MODE );  /* �豸USB����ģʽ */
    xWriteCH376Data(0x03);               //ѡ��ģʽ��03 SD��
    delay_us(20);
    res = xReadCH376Data();
    xEndCH376Cmd();
#ifndef    CH376_INT_WIRE
#ifdef    M_SPI_Do
    xWriteCH376Cmd(CMD20_SET_SDO_INT);  /* ����SPI��SDO���ŵ��жϷ�ʽ */
    xWriteCH376Data(0x16);
    xWriteCH376Data(0x90);  /* SDO������SCSƬѡ��Чʱ�����ж�������� */
    xEndCH376Cmd();
#endif
#endif
    if (res==CMD_RET_SUCCESS)
        return(USB_INT_SUCCESS);
    else 
        return(ERR_USB_UNKNOWN);  /* ����ģʽ���� */
}

void CH376_Reset(void)  /* CH376��λ */
{
    UINT8  i;

    CH376_PORT_INIT();  /* �ӿ�Ӳ����ʼ�� */

    xWriteCH376Cmd(CMD11_CHECK_EXIST);  /* ���Ե�Ƭ����CH376֮���ͨѶ�ӿ� */
    for ( i=80;i!=0;i--){
        xWriteCH376Cmd(CMD00_RESET_ALL);  /* ����ظ�������,ִ��Ӳ����λ */
        xReadCH376Data();
        xEndCH376Cmd();
    }
    xWriteCH376Cmd(0);
    Delay_ms(50);  /* ��ʱ100ms */
}

//==================================
// bFlag:
//    	TRUE: go sleep;
//		FALSE: wakeup
//==================================
void CH376_SleepEnable(BOOL bFlag)
{
	if(swSD_CARD==OFF){return;}

    if(bFlag){  //sleep
        xWriteCH376Cmd(CMD00_ENTER_SLEEP);  
        xEndCH376Cmd();   
        Delay_ms(250);
    }
    else{      //����
        xWriteCH376Cmd (CMD01_GET_IC_VER);        
        xEndCH376Cmd();
        Delay_ms(250);
    }
}





