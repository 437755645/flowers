//PIC24FJ256GA106与CH376 SPI  硬件驱动接口DEBUG.C

//钱澄 2011.01.13
/*********************************************************************
* REVISION HISTORY: CH376芯片 硬件抽象层 V1.0
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date          Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* 钱 克宠               14/01/11     First release of source file
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*CH376芯片 硬件抽象层 V1.0
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
// 写一个byte
******************************************************************************/
/*void CfgReg_RE(void)
{
    UINT addr,uAddr;
    UINT i;
    BYTE numCx,b_cfg;

    addr = REG0;    
    //cfg尾部地址
    numCx = GetByte(M_CFG,CM_CXS);
//    uAddr = AddressOfCxBegin(numCx)+lenCELL;        //cfg尾部地址  
    uAddr = cfg[CM_LEN_CFG]*lenCELL*2;        //cfg尾部地址  

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
// 检查操作状态,如果错误则显示错误代码并停机,
// 应该替换为实际的处理措施,
// 例如显示错误信息,等待用户确认后重试等 
//=====================================================
void mStopIfError( UINT8 iError )
{
BYTE   fReset=0;
BYTE   ErrMsg[8];
    
    if(iError==USB_INT_SUCCESS)return;  /* 操作成功 */
    
    
    PW_LCD_on();
    lcd_init();        //lcd初始化
    lcd_clear();
    
    sprintf((char *)ErrMsg,"0x%x",iError);  
    lcd_puts(0,L0,"TF Card Failed!");
    lcd_puts(0,L1,"Error code:");
    lcd_puts(12,L1,(const char*)ErrMsg);     
    lcd_puts(0,L0,"请先初始化检查TF卡是否插好!");
    
    if(iError==USB_INT_DISCONNECT){
        lcd_puts(0,L2,"检查TF卡是否插好!");  
        return;       
    }   
    
    if(iError==0x42){
        lcd_puts(0,L2,"TF正在初始化...");  
        
        InitCardFiles();
        Restart(RST_FOR_ERR_MAIN_14);
        return;       
    }

//记录1：CH376ByteLocate()   
    if(iError==0xB4){
	  fReset++;			//ERR_FILE_CLOSE       0xB4            /* 文件已经关闭,如果需要使用,
	  //while(1){;}
	}

//记录1：CH376ByteLocate()   
    if(iError==0xFA){
	  fReset++;			//ERR_USB_UNKNOWN        0xFA              /* 文件已经关闭,如果需要使用,
	  //while(1){;}
	}

    //PRINTF( "Error: %02X\n", (UINT16)iError );  /* 显示错误 */

    lcd_puts(0,L2,"10秒后重启:");   
    
    while(1){
        vLedTest_ms( 250 );  /* LED闪烁 */
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
        }        //校时完成,重启
    }
}


/* 由于使用软件模拟SPI读写时序,所以进行初始化 */
void CH376_PORT_INIT(void)  
{        
    CH376_SPI_SCS = 1;  /* 禁止SPI片选 */
    CH376_SPI_ClkOut = 1;  //默认为高电平
}
/******************************************************************************
// 写一个byte
******************************************************************************/
void spi_WriteByte_ch376(BYTE output) //SPI输出8个位数据 
{
    char BitCount; 

    BitCount=8;
    do{         
      delay_WIDTH_CLK_SPI_376;            // delay a mininum time gap
      CH376_SPI_ClkOut=0;               // Clear the SCK pin
//
      if(output&0x80)CH376_SPI_DI = 1;    // Set Dout to the next bit according to the MSB of data
      else CH376_SPI_DI=0;
      output<<=1;                     // 数据位是高位在前 

      delay_WIDTH_CLK_SPI_376;            // delay a mininum time gap
      CH376_SPI_ClkOut=1;                  // data out, Set the SCK pin, 在时钟上升沿采样
    } while(--BitCount);
    delay_WIDTH_CLK_SPI_376;            // delay a mininum time gap
}
//===========================
// 读1个byte
//===========================
BYTE spi_ReadByte_ch376(void)
{
    char BitCount;
    BYTE input;

    BitCount=8;
    input=0;
    do{                             
      CH376_SPI_ClkOut = 0;               // DO shift out, Clear the SCK pin,
                                  //CH376在时钟下降沿输出
      delay_WIDTH_CLK_SPI_376;            // delay a mininum time gap
      input<<=1;                  // 数据位是高位在前
      if(CH376_SPI_Do)input++;

      CH376_SPI_ClkOut = 1;              // Set the SCK pin
      delay_WIDTH_CLK_SPI_376;            // delay a mininum time gap
    }while(--BitCount);
    return input;
}



//#define    xEndCH376Cmd( )    { CH376_SPI_SCS = 1;}  
///SPI片选无效,结束CH376命令,仅用于SPI接口方式 /
//结束CH376命令,仅用于SPI接口方式 /
void xEndCH376Cmd(void)            
{
    CH376_SPI_SCS = 1;
}

/* 向CH376写命令 */
void xWriteCH376Cmd(UINT8 mCmd)
{
    CH376_SPI_SCS = 1;  
    /* 防止之前未通过xEndCH376Cmd    禁止SPI片选 */    
    delay_us(2);
    CH376_SPI_SCS = 0;  /* SPI片选有效 */
    spi_WriteByte_ch376(mCmd);  /* 发出命令码 */    
    delay_us(4);
    /* 延时2uS确保读写周期大于1.5uS */
}


void xWriteCH376Data(UINT8 mData)  /* 向CH376写数据 */
{
    spi_WriteByte_ch376( mData );
    delay_us(4);  /* 确保读写周期大于0.6uS */
}
UINT8 xReadCH376Data(void)  /* 从CH376读数据 */
{
    delay_us(4);  /* 确保读写周期大于0.6uS */
    return(spi_ReadByte_ch376());
}


/* 查询CH376中断(INT#低电平) */
UINT8 Query376Interrupt(void)
{    
    return(CH376_INT_WIRE ? FALSE:TRUE); 
    delay_us(4);
}

UINT8 mInitCH376Host(void)  /* 初始化CH376 */
{
    UINT8 res;
    CH376_PORT_INIT();  
    /* 接口硬件初始化 */
    xWriteCH376Cmd(CMD11_CHECK_EXIST); 
    /* 测试单片机与CH376之间的通讯接口 */
    xWriteCH376Data(0x65);
    res = xReadCH376Data();
    xEndCH376Cmd();
    if(res!=0x9A)return(ERR_USB_UNKNOWN);  		
	/* 通讯接口不正常,可能原因有:接口连接异常,其它设备影响(片选不唯一),
	串口波特率,一直在复位,晶振不工作 */

    xWriteCH376Cmd( CMD11_SET_USB_MODE );  /* 设备USB工作模式 */
    xWriteCH376Data(0x03);               //选择模式是03 SD卡
    delay_us(20);
    res = xReadCH376Data();
    xEndCH376Cmd();
#ifndef    CH376_INT_WIRE
#ifdef    M_SPI_Do
    xWriteCH376Cmd(CMD20_SET_SDO_INT);  /* 设置SPI的SDO引脚的中断方式 */
    xWriteCH376Data(0x16);
    xWriteCH376Data(0x90);  /* SDO引脚在SCS片选无效时兼做中断请求输出 */
    xEndCH376Cmd();
#endif
#endif
    if (res==CMD_RET_SUCCESS)
        return(USB_INT_SUCCESS);
    else 
        return(ERR_USB_UNKNOWN);  /* 设置模式错误 */
}

void CH376_Reset(void)  /* CH376复位 */
{
    UINT8  i;

    CH376_PORT_INIT();  /* 接口硬件初始化 */

    xWriteCH376Cmd(CMD11_CHECK_EXIST);  /* 测试单片机与CH376之间的通讯接口 */
    for ( i=80;i!=0;i--){
        xWriteCH376Cmd(CMD00_RESET_ALL);  /* 多次重复发命令,执行硬件复位 */
        xReadCH376Data();
        xEndCH376Cmd();
    }
    xWriteCH376Cmd(0);
    Delay_ms(50);  /* 延时100ms */
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
    else{      //唤醒
        xWriteCH376Cmd (CMD01_GET_IC_VER);        
        xEndCH376Cmd();
        Delay_ms(250);
    }
}





