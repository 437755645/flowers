/**************************************************************
开启后时能debug调试入口
通过延时不停写入&读出数据
否则通过RTC定时中断启动定时读写数据功能
***************************************************************/
#define DEBUG_ON     

//开启后对SD卡进行记录，否则关闭SD卡记录功能
#define TF_RECODE
//开启后RTC 10秒钟触发中断，否则1分钟触发，
#define TF_DEBUG_10S  

//#define TF_DEBUG_2S   

//#define TF_DEBUG_60S  

//定义是否开启枚举且删除SD卡中文件
//#define SD_LISTANDDEL
//#define TF_RECODE_READ

/**************************************************************
//调试打印信息开关
***************************************************************/
#define  DEBUG_PRINT  

#ifdef  DEBUG_PRINT  
#define PRINTF(format,...) //printf(""format"",##__VA_ARGS__)
//使能UART3打印
#else  
#define PRINTF(format,...)  Nop()
//关闭UART3打印
#endif  

//**************************************************************
//定义记录虚元是否包含的TIME_STAMP
//如果包含则虚元长度在记录文件中增加14个字节
//**************************************************************
#define TIME_STAMEP_INCLUDED   

#ifdef  TIME_STAMEP_INCLUDED  
#define SD_TIME_STAMP_LEN   14	//YYMMDDHHMMSSFF
#else
#define TIME_STAMP_LEN   	0
#endif


/****************************************************************************
//记录初始时候占用6个字节[yymmddhhMMSS]，以小时为最小单位
即为[yymmddhh0000]
*****************************************************************************/
#define MAP_REG_RC_START_TIME   0x0100    //记录终端开始记录的起始时间

/****************************************************************************
记录初始时间，初始化标志位
//如果该值为0，则开始取当前时间为记录初试时间,再每次重启
系统的时候将会查询该寄存器值；
如果该值为1，则表示当前时间已经记录，不对其重新写入
初始时间
*****************************************************************************/
#define MAP_REG_RC_START_FLG    0x0106    //记录终端开始记录的起始时间
#define RC_START_CLR    0x00    
#define RC_START_SET    0x01   

void CarInitProgress(void );
BYTE CardInit(void);
BYTE CardMount(void);
BYTE getShortName(BYTE iC0,BYTE iCx,BYTE *ShortFn);
void CreatLongName(BYTE *bShortFn,BYTE *bLongfn);
BYTE get_SDCardStartTime(BYTE *pbSTime,BYTE *bShortFn);
ULNG getSDCfgLen(void);
BYTE GetCxCardFilename(BYTE *fn); //modified by qian 
BYTE CreatCxCardFile(BYTE *bShortFn,BYTE *bLongFn);
//BYTE InitCxCardRec(BYTE *cx_cfg);
long DeltaToSec(hTIME *curT,hTIME *staT);
BYTE ListFile(BYTE index);
BYTE ListAll(void);
BYTE CardWriteStr(ULNG Offset,BYTE *cx_cfg,BYTE *bShortFn);
BYTE CardReadRec(ULNG Offset,ULNG ulLen,BYTE *cx_cfg,BYTE *bShortFn);
void InitCardFiles(void);
char ReadTfRecodeBack(hTIME *cur_hTime,BYTE *cx_cfg,BYTE *bShortFn,BYTE *recode_belt);
char TFRecord(BYTE *val,hTIME *rTime,BYTE iC0,BYTE iCx);
void TfCardProc(void);
void TimeStr2Struct(BYTE* str_time,hTIME *h_Time);
//long getRgSec(BYTE unit,BYTE interval);
void hTimeToStrFlag(hTIME *dt,BYTE *strTime);
void CxReadProc(void);
BYTE ExportCfg2SDCard(BYTE *bShortFn);

void set_RecordStartTime(hTIME dt);
void get_RecordStartTime(hTIME *pdt);
long hTime2Sec(hTIME dt);
long DeltaToSec(hTIME *curT,hTIME *starT);
void getRecordStr(BYTE *val,BYTE val_len,hTIME dt,BYTE *des);
ULNG Gap2Sec(BYTE unit,BYTE interval);

