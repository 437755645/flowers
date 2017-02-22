/* CH376芯片 硬件抽象层 V1.0 */
/* 提供I/O接口子程序 */
#include "Tf_Ch376Inc.h"


#ifndef __CH376_FUNC_H__
#define __CH376_FUNC_H__

#define delay_WIDTH_CLK_SPI_376        delay_us(2)    

//填充的buf大小
#define BUF512            	512
#define MAX_FILE_COUNT      40
#define CFGLEN              64
#define BUF65536      		65536
//例举文件
typedef struct _FILE_NAME {
    ULNG    DirStartClust;             /* 文件所在目录的起始簇号*/
//  ULNG    Size;                      /* 文件长度 */
    UINT8   Name[8+1+3+1];             /* 文件名,共8+3字节,分隔符,结束符,因为未包含上级目录名所以是相对路径 */
    UINT8   Attr;                      /* 文件属性 */
}FILE_NAME;


extern struct hTIME dt;


//modified by qian 20120306
//sw spi IF define 


#ifdef NEWBORAD120913  
//新版本SPI 改为HW SPI
#define CH376_INT_WIRE          PORTDbits.RD9    // SD_INT 
#define CH376_SPI_SCS           PORTDbits.RD10   // CH376的 SCS
#define CH376_SPI_ClkOut        PORTBbits.RB15   // CH376的CLK 
#define CH376_SPI_DI            PORTFbits.RF4    //CH376的SDI引脚,对应是PIC 的SDO引脚 
#define CH376_SPI_Do            PORTFbits.RF5    // CH376的SDO引脚,对应的PIC 的SDI引脚
#define CH376_SD_INSERT         PORTDbits.RD11   // SD_insert
#define CH376_POWER_CN          PORTDbits.RD0    // SD_PWCNT
#else
#define CH376_INT_WIRE          PORTEbits.RE4    // SD_INT 
#define CH376_SPI_SCS           PORTEbits.RE3    // CH376的SCS引脚
#define CH376_SPI_ClkOut        PORTEbits.RE2    // CH376的SCK引脚
#define CH376_SPI_DI            PORTEbits.RE1    // CH376的SDO引脚,对应是PIC 的SDI 
#define CH376_SPI_Do            PORTEbits.RE0    // CH376的SDI引脚,对应的PIC 的SD0引脚 
#define CH376_SD_INSERT         PORTFbits.RF1    // SD_insert
#define CH376_POWER_CN          PORTFbits.RF0    // SD_PWCNT
#endif

#define SD_CFG_HEAD        	"CFG:\r\n"
#define SD_RECODE_HEAD     	"RECODES:\r\n"
#define SD_STIME_LEN        12 //[YYMMDDHHMMSS]
#define TIMEFLG_LEN     	14 //[YYMMDDHHMMSSFF]

#define SD_MIN_SEC         (60) 
#define SD_HOUR_SEC        (3600) 
#define SD_DAY_SEC         (86400) 
#define SD_MON_SEC         (2764800)
#define SD_YEAR_SEC        (33177600)

/* 附加的USB操作状态定义 */
/* 未知错误,不应该发生的情况,需检查硬件或者程序错误 */
#define ERR_USB_UNKNOWN        0xFA    


//SPI 初始化
void Init_SPI_ch376(void);
void TfCardProc(void);

void getLongName(BYTE *bShortFn,BYTE *bLongfn);

BYTE set_CardStartTimeInCfg(hTIME dt,BYTE *bShortFn);
BYTE get_SDCardStartTime(BYTE * pbSTime,BYTE * bShortFn);


void UART3Init_SenCom(ULNG Baudrate);
void UART2Init_Cfgcom(ULNG Baudrate);

//错误处理函数
void mStopIfError( UINT8 iError );
void vLedTest_ms( UINT8 uchDelay ) ;
void vLedTest_s( UINT8 uchDelay ) ;


/* CH376通讯接口初始化 */
void CH376_PORT_INIT( void );          

/* 结束CH376命令,仅用于SPI接口方式 */
void xEndCH376Cmd( void );            

/* 向CH376写命令 */
void xWriteCH376Cmd( UINT8 mCmd );    

/* 向CH376写数据 */
void xWriteCH376Data( UINT8 mData );

/* 从CH376读数据 */
UINT8 xReadCH376Data( void );    

/* 查询CH376中断(INT#引脚为低电平) */
UINT8 Query376Interrupt( void );        

/* 初始化CH376 */
UINT8 mInitCH376Host( void );            

/* CH376复位 */
void CH376_Reset( void );                  

/*CH376 进入休眠状态*/
void CH376_SleepEnable(BOOL bFlag);     


#endif
