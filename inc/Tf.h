/**************************************************************
������ʱ��debug�������
ͨ����ʱ��ͣд��&��������
����ͨ��RTC��ʱ�ж�������ʱ��д���ݹ���
***************************************************************/
#define DEBUG_ON     

//�������SD�����м�¼������ر�SD����¼����
#define TF_RECODE
//������RTC 10���Ӵ����жϣ�����1���Ӵ�����
#define TF_DEBUG_10S  

//#define TF_DEBUG_2S   

//#define TF_DEBUG_60S  

//�����Ƿ���ö����ɾ��SD�����ļ�
//#define SD_LISTANDDEL
//#define TF_RECODE_READ

/**************************************************************
//���Դ�ӡ��Ϣ����
***************************************************************/
#define  DEBUG_PRINT  

#ifdef  DEBUG_PRINT  
#define PRINTF(format,...) //printf(""format"",##__VA_ARGS__)
//ʹ��UART3��ӡ
#else  
#define PRINTF(format,...)  Nop()
//�ر�UART3��ӡ
#endif  

//**************************************************************
//�����¼��Ԫ�Ƿ������TIME_STAMP
//�����������Ԫ�����ڼ�¼�ļ�������14���ֽ�
//**************************************************************
#define TIME_STAMEP_INCLUDED   

#ifdef  TIME_STAMEP_INCLUDED  
#define SD_TIME_STAMP_LEN   14	//YYMMDDHHMMSSFF
#else
#define TIME_STAMP_LEN   	0
#endif


/****************************************************************************
//��¼��ʼʱ��ռ��6���ֽ�[yymmddhhMMSS]����СʱΪ��С��λ
��Ϊ[yymmddhh0000]
*****************************************************************************/
#define MAP_REG_RC_START_TIME   0x0100    //��¼�ն˿�ʼ��¼����ʼʱ��

/****************************************************************************
��¼��ʼʱ�䣬��ʼ����־λ
//�����ֵΪ0����ʼȡ��ǰʱ��Ϊ��¼����ʱ��,��ÿ������
ϵͳ��ʱ�򽫻��ѯ�üĴ���ֵ��
�����ֵΪ1�����ʾ��ǰʱ���Ѿ���¼������������д��
��ʼʱ��
*****************************************************************************/
#define MAP_REG_RC_START_FLG    0x0106    //��¼�ն˿�ʼ��¼����ʼʱ��
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

