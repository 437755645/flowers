/********************************************************************
*
* PIC24F Serial Bootloader
*
* Configuration for PIC24F Serial Bootloader
* 
*
* Change History:
*
* Author          Revision #      Date        Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* jonnyqian  1.00               01-17-2013    Initial release 
********************************************************************/
#ifndef CONFIG_H
#define CONFIG_H

#define USE_AND_OR /* To enable AND_OR mask setting */
//Includes **********************************************************************
#include <p24fxxxx.h>
#include <string.h>
#include <stdio.h>
#include <pps.h>
#include <Ports.h>
#include <Uart.h>
#include <spi.h>
#include <wdt.h>


/*******************************************************************************
额外的定义，在MIX中分布在各相应的头文件中
*******************************************************************************/
#define BOOTLOADER_ONLY 

#ifdef BOOTLOADER_ONLY

extern DWORD_VAL   userTimeout   ;     //bootloader entry timeout value

#endif
/******************************************************************************/

//=============================================================

//#define SPIFLASH_FLG_CLR_TEST    

// PRE_BOOT_RFLASH2FRAM=1 :bootloader 的前奏，读取flash信息到FRAM
// PRE_BOOT_RFLASH2FRAM=0 :正常bootloader 的前奏，读取FRAM信息到flash                             


//Configuration Defines ************************************************************
//HT == TAB
#define HT                    0x09 
#define SPACE                 0x20

//Bootloader device configuration
#define DEV_HAS_WORD_WRITE    //Device has word write capability (24FJxxx devices)

//Bootloader feature configuration
#define USE_BOOT_PROTECT        //Use BL block protection 
#define USE_RUNAWAY_PROTECT        //Provide runaway code protection using program flow

//Bootloader Operation Configuration
#define MAJOR_VERSION           0x01    //Bootloader FW version
#define MINOR_VERSION           0x02

#define FCY                     11059200/2    //Instruction clock speed (Fosc/2)



#define MAX_PACKET_SIZE         261    //Max packet size

//USER_PROG_RESET should be the location of a pointer to the start of user code, 
//not the location of the first instruction of the user application.
//Device Flash Memory Configuration
//Size of flash memory sections in bytes
//includes upper phantom byte (32-bit wide memory)
#define USER_PROG_RESET        0x100    //User app reset vector location
#define DELAY_TIME_ADDR        0x102    //BL entry delay location
#define BOOT_ADDR_LOW          0x400    //start of BL protection area
#define BOOT_ADDR_HI           0x3FFF    //end of BL protection area ** USE 0x13FF for AES support

#define FLASHMEM_LAST_ADDR     0x004    //IVT_RESERVED indica to last flash memory addr
#define PM_INSTR_SIZE           4        //bytes per instruction 
#define PM_ROW_SIZE             0x100    //256      //user flash row size offset 
#define PM_PAGE_SIZE            2048     //user flash page size offset 0x800
#define PM_PAGE_ADDR_SIZE       0x400     //user flash page size offset
#define PM_PERROW_ADDR_SIZE     0x80     //PM_ROW_SIZE/PM_INSTR_SIZE*2
#define PM_PERROW_INSTRUCTIONS  (PM_ROW_SIZE/PM_INSTR_SIZE)     //PM_ROW_SIZE/PM_INSTR_SIZE*2


#define PM_CHIP_SIZE            0x40200  //0x2AC00 instrcutions/2*3     //user flash size,bytes

//Vector section is either 0 to 0x200 or 0 to end of first page, whichever is larger
#define VECTOR_SECTION          ((0x200>(PM_PAGE_SIZE/2))?0x200:(PM_PAGE_SIZE/2)) 

#define CONFIG_START            0x2ABFA    
#define CONFIG_END              0x2ABFE

//IVT ROW 是固定值//NUM OF IVT ROWS
//由于IVT 只去0x400的一半
#define PM_IVT_START           0x0    //start of BL protection area
#define PM_IVT_LENS               0x400    //start of BL protection area
#define PM_IVT_ROWS             ((PM_IVT_LENS>>1)/PM_PERROW_ADDR_SIZE)  
#define PM_IVT_BYTE_LENS        ((PM_IVT_LENS>>1)*3)    //0x600 

#define PM_BOOT_START           0x400  
#define PM_BOOT_LENS            0x3C00   
#define PM_BOOT_ROWS             (PM_BOOT_LENS/PM_PERROW_ADDR_SIZE)  
#define PM_BOOT_BYTE_LENS        ((PM_BOOT_LENS>>1)*3)   //0x5a00

#define PM_APP_START           0x4000   //0xC00    //start of APPUSE protection area
#define PM_APP_LENS               0x26000   //0xC00    //start of APPUSE protection area
#define PM_APP_ROWS             (PM_APP_LENS/PM_PERROW_ADDR_SIZE)  
#define PM_APP_BYTE_LENS        ((PM_APP_LENS>>1)*3)   //0x39000

//Self-write NVMCON opcodes    
#define PM_PAGE_ERASE           0x4042    //NVM page erase opcode
#define PM_ROW_WRITE            0x4001    //NVM row write opcode
#define PM_WORD_WRITE           0x4003    //NVM word write opcode


//******************************************************************************

//Constant Defines ***************************************************************
//Protocol Commands
#define RD_VER          0x00
#define RD_FLASH        0x01
#define WT_FLASH        0x02
#define ER_FLASH        0x03
#define RD_EEDATA       0x04
#define WT_EEDATA       0x05
#define RD_CONFIG       0x06
#define WT_CONFIG       0x07
#define VERIFY_OK       0x08

//Communications Control bytes
#define STX            0x55
#define ETX            0x04
#define DLE            0x05
//******************************************************************************


//UART Configuration *************************************************************
#define UARTNUM            1       //Which device UART to use

#define UARTREG2(a,b)        U##a##b
#define UARTREG(a,b)       UARTREG2(a,b)

#define UxMODE              UARTREG(UARTNUM,MODE)
#define UxBRG               UARTREG(UARTNUM,BRG)
#define UxSTA               UARTREG(UARTNUM,STA)
#define UxRXREG             UARTREG(UARTNUM,RXREG)
#define UxTXREG             UARTREG(UARTNUM,TXREG)
#define UxMODEbits          UARTREG(UARTNUM,MODEbits)
#define UxSTAbits           UARTREG(UARTNUM,STAbits)

//******************************************************************************

#define COM1_EN              U1MODEbits.UARTEN
#define _NSTDIS              INTCON1bits.NSTDIS
#define RX1_IE               IEC0bits.U1RXIE
#define TX1_IE               IEC0bits.U1TXIE
#define TX1_EN               U1STAbits.UTXEN

//SLEEP
#define SLEEP3_DIR            TRISEbits.TRISE5
#define SLEEP3                PORTEbits.RE5
#define OUT                 0

// 1 -- HW VERSION YR-MIX V1.15 2012-09-13
// 0---HW VERSION before YR-MIX V1.15 2012-09-13



//XME 容量定义
#define XME_1PAGE_SIZE      0x100    //256      
#define XME_SECTOR_SIZE     0x1000   // 4096                 
#define XME_BLOCK_SIZE      0x10000  //65536=16*XME_SECTOR_SIZE      
#define XME_CHIP_SIZE       0x100000 //8Mbit//32*XME_BLOCK_SIZE 0-0X1F FFFF =0X200000    

#define XME_0p5BLOCK_SIZE   (XME_BLOCK_SIZE/2)  //16*XME_SECTOR_SIZE      //1/2BLOCK


#define XME_BL_OFFSET       (XME_CHIP_SIZE>>1)          //0X80000
#define XME_IVT_START        XME_BL_OFFSET               
#define XME_BOOT_START       (XME_BL_OFFSET+PM_IVT_BYTE_LENS)              
#define XME_APP_START       (XME_BOOT_START+PM_BOOT_BYTE_LENS)


//版本号占用3个字节AA.BB.CC显示
/*对比新旧版本号，如果新的版本号，与旧版本号，不同
则通过app调用bootloader 函数，
通过bootloader 函数加载由app更新到spiflash中的新版本的
appcode16进制值
版本号，取年未2位，月，日121214
大小的比较就按照Y*365+31*M+D*/

#define BL_FLG_OFFSET_IN_CFGREG     0x4000    
// BL所需标志字在CFG寄存器中的地址偏移

#define APPCODE_NEWVER             (BL_FLG_OFFSET_IN_CFGREG+0x0)   // FRAM 大小
//#define APPCODE_NEWVERB             (BL_FLG_OFFSET_IN_CFGREG+0x1)   // FRAM 大小
//#define APPCODE_NEWVERC             (BL_FLG_OFFSET_IN_CFGREG+0x2)   // FRAM 大小

#define APPCODE_OLDVER            (BL_FLG_OFFSET_IN_CFGREG+0x3)    // FRAM 大小
//#define APPCODE_OLDVERB             (BL_FLG_OFFSET_IN_CFGREG+0x4)   // FRAM 大小
//#define APPCODE_OLDVERC             (BL_FLG_OFFSET_IN_CFGREG+0x5)   // FRAM 大小

/*版本更新日期，最小单位为小时
格式YYYYMMDDHH,2012123024*/

#define APPCODE_DATAYEAR1           (BL_FLG_OFFSET_IN_CFGREG+0x6)    // FRAM 大小
#define APPCODE_DATAYEAR2           (BL_FLG_OFFSET_IN_CFGREG+0x7)    // FRAM 大小
#define APPCODE_DATAMOUTH           (BL_FLG_OFFSET_IN_CFGREG+0x8)   // FRAM 大小
#define APPCODE_DATADAY             (BL_FLG_OFFSET_IN_CFGREG+0x9)   // FRAM 大小
#define APPCODE_DATAHOUR            (BL_FLG_OFFSET_IN_CFGREG+0xA)   // FRAM 大小

/*************************************************************************
 SPI FLASH 是否需要擦除标志，
如果擦除，将标志位置0x00
如果写入结束，将标志位置0x01
该标志位，通过判读是否有新的appcode代码，来对pic24flash
操作。
如果有新的appcode，则擦除pic24 appcode部分
否则不擦除。
**************************************************************************/
#define XMEMCODE_EXIST            (BL_FLG_OFFSET_IN_CFGREG+0xB)
#define XMEMCODE_NTEXIST_FLG   0x00         
#define XMEMCODE_EXIST_FLG        0x01 
/*************************************************************************
APPCODE 是否在pic24flash的code 位置出现标志
如果该appcode的首地址值为0xff ff ff
则表示appcode已经不存在在pic24flash中
否则表示存在在pic24flash中
标志位置位表达:
如果 不存在则标志位为0xff
如果存在 则标志位为0x00   
**************************************************************************/
#define PMCODE_EXIST                (BL_FLG_OFFSET_IN_CFGREG+0xC)
#define PMCODE_EXIST_FLG           0x01         
#define PMCODE_NTEXIST_FLG         0x00  

/*************************************************************************
APPCODE  是否需要写入SPI FLASH 标志位
如果需要写入标志位置位0xFF,(因为默认是oxff寄存器值)
否则，标志位为0x00
因此在appcode完全写入spiflash后，需要将该寄存器值改为0x00
**************************************************************************/
#define APPCODE_2_SPIFLASH            (BL_FLG_OFFSET_IN_CFGREG+0xD)
#define W2SPIFLASH_DONE               0x00         
#define W2SPIFLASH_CLR                0xFF  


/*************************************************************************
 PIC24 PM 是否需要擦除标志，
如果擦除，将标志位置0x00
如果写入结束，将标志位置0x01
该标志位，通过判读是否有新的appcode代码，来对pic24flash
操作。
如果有新的appcode，则擦除pic24 appcode部分
否则不擦除。
**************************************************************************/
#define PIC24PM_ERASE        (BL_FLG_OFFSET_IN_CFGREG+0xE)

//Map of Registration table
//每一条flash 指令占用3 BYTE
//因此地址偏移按3的整数倍放置
#define FRAM_MAP_VERSION              0x00         
#define FRAM_MAP_GOTO_INSTR           0x03        // 
#define FRAM_MAP_RESET_ADDR           0x06
#define FRAM_MAP_IVT                  0x09       
//num. of 64-byte blocks to be uploaded when registrating. 
#define FRAM_MAP_RESERV               0x04  //x 64, really start in i*64
#define FRAM_MAP_CELLX_BEG            0x05  //x 64
#define FRAM_MAP_NUM_OF_RESTART       0x06  //重启次数存储
#define FRAM_MAP_MARK                 0x07        
#define FRAM_MAP_ID_MIRROR            0x08  //2字节，放置ID镜像，用于判断ID是否已被修改。


 
/* // 总空间为0-0x400，每行12 个字节，总共行数为
0x400/8 行*/

#define SUCCESS         1
#define FAILED          0

#define BCD2HEX(x)      (((x)>>4)*10+((x) & 0x0f))
#define HEX2BCD(x)      ((x)+(((x)/10)*6))

//===============================================
#define testbit0(var)           ((var) & 0x01)
#define testbit(var,bit)        ((var) & (1<<(bit)))

#define setbit0(var)            ((var) |= 0x01)
#define setbit(var,bit)         ((var) |= (1<< (bit)))

#define clrbit0(var)            ((var) &= 0xfe)
#define clrbit(var,bit)         ((var) &= ~(1<< (bit)))

//Function Prototypes **************************************************************

//bootflash.c
void Serial_putStr(const char *str);
void PutChar(BYTE txChar);
void GetChar(BYTE *);
void ReadPM(UINT16 length, DWORD_VAL ulPmAddr);
void WritePM(UINT16 length, DWORD_VAL ulPmAddr);
void ErasePM(UINT16 length, DWORD_VAL ulPmAddr);
void replaceBLReset(DWORD_VAL ulPmAddr);

//sub.c 
void vLedTest_ms( UINT8 uchDelay );

//memory.c
void WriteMem(UINT16);
void WriteLatch(UINT16, UINT16, UINT16, UINT16);
UINT32 ReadLatch(UINT16, UINT16);
void ResetDevice(UINT16);
void Erase(UINT16, UINT16, UINT16);

void Write_PmWord(DWORD_VAL data, DWORD_VAL ulPmAddr);
void Write_PmRow(DWORD_VAL ulPmAddr,BYTE *pBuffer);

void Read_PmWord(UINT32 length, DWORD_VAL ulPmAddr);
void Read_PmRow(DWORD_VAL ulPmAddr,BYTE *pBuffer,BOOL bPrint);

void WriteTimeout();
void replaceBLReset(DWORD_VAL);

//spi.c
// SPI FLASH 操作
void Read_XmemWord(UINT32 ulSpiAddr,BYTE *pBuffer);
void Read_XmemRow(UINT32 ulSpiAddr,BYTE *pBuffer,BOOL bPrint);
void rPmAndw2Xmem_Row(DWORD_VAL ulPmAddr,UINT32 ulSpiAddr,BOOL bPrint);
void rXmemAndw2Pm_Row(UINT32 ulSpiAddr,DWORD_VAL ulPmAddr);
void rXmemAndw2Pm_Word(DWORD length, DWORD_VAL ulPmAddr);

    
void Spi_Flash_Init(void);
void Spi_Flash_ChipErase(void);
void Spi_Flash_Erase_Sector(UINT32 ulAddr);
void Spi_Flash_BlockErase32K(UINT32 ulAddr);
void Spi_Flash_BlockErase64K(UINT32 ulAddr);

void Spi_Flash_Busy(void);
void Spi_Flash_BusyAAI(void) ;
void Spi_Flash_AAI_WordProgramA(BYTE Byte1,BYTE Byte2, UINT32 Addr);
void Spi_Flash_AAI_WordProgramB(BYTE state,BYTE Byte1, BYTE Byte2) ;

void Spi_Flash_WritePage(UINT8* pBuffer, UINT32 WriteAddr, UINT NumByte2W);
void Spi_Flash_Read(BYTE *pBuffer,UINT32 ReadAddr,UINT NumByteToRead);
void Spi_Flash_WriteByte(BYTE Buffer,UINT32 WriteAddr);
BYTE Spi_Flash_ReadByte(UINT32 ReadAddr);
BYTE Spi_Flash_DeviceId(void);
void Spi_Flash_UniqueId(BYTE * UniqueId);
UINT32 Spi_Flash_JedecID(void); // 00--Manf--Type--Capacity
UINT16 Spi_Flash_ManfDeviceID(UINT32 ReadManfDeviceID_Addr);
void Spi_Flash_EnableWriteStatus(void);
void Spi_Flash_HighSpeedRead(BYTE *pBuffer,ULNG ReadAddr, UINT NumByteToRead);
void Spi_Flash_EBSY(void);
void Spi_Flash_DBSY(void);
BYTE WriteXme(BYTE val,UINT32 ulAddr);
BYTE WriteXMem2(BYTE val,UINT32 ulAddr);


//******************************************************************************

#endif //ifdef CONFIG_H
