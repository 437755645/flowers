#include 	<p24fxxxx.h>
#include 	<string.h>
#include	<stdlib.h>
#include  	<ctype.h>
#include 	<stdio.h>
#include	<math.h>
#include	<libpic30.h>

#include	"HARDWARE.H"
#include    "CFG_STRUCT.H"
#include 	"BASE.H"
#include 	"LCD.H"
#include 	"FUNCTION.H"

#include 	"Tf.H"
#include 	"Tf_Ch376Func.H"
#include 	"Tf_Ch376Inc.H"
#include 	"Tf_Config.H"
#include 	"Tf_FileSys.H"

/////////////////////////////////////////////////////////////////////////

#define		WATCHDOG_ON				1
#define		DEBUG_PGM_ON			1		//运行DEBUG.c
//#define		SIM_ON					1		//模拟cfg[],No-Delay	
#define		SIM_SENSOR				1
//#define		SIM_INIT_REC		1		//内存模拟REC_AREA初始化
//#define		SIM_MEM				1		//用内存模拟REC_AREA读写
//#define		SIM_CLOCK			1
//#define		STOP_WATCH_ON			1


#define		SENSOR_RESET_PERMITTED	1		//1:可关断			0:不关断
#define		PW_NON_NO_OFF			0		//1:不关断（debug） 0:允许关断（release）
#define		CLOCK_5C372				1
#define		SENSOR_RESET_PERMITTED	1		//1:可关断			0:不关断
#define		LCD_EXIST				1
#define		LCD_ALWAYS_ON			0		//1:始终打开		0：省电关断
#define		LIGHT_ALWAY_ON			0		//1:始终背光		0：省电关断

#define		sVERSION			{"Ver: 120510     "}	


//simulation must keep 'DEBUG_ON' valid to keep timer running

////////////////////////////////////////////////////////////////////////


//MSG0	Msg0;
#define	fSrnRefresh			Msg0.bitSrnRefresh
#define	fHomePageChoice		Msg0.bitHomePageChoice
#define	fLongInst			Msg0.bitLongInst
#define	fKeyReleased		Msg0.bitKeyReleased
#define	fBasicBeWritten		Msg0.bitBasicBeWritten
#define	fTimeToBroadcast	Msg0.bitTimeToBroadcast
#define	fCursorToggle		Msg0.bitCursorToggle

//MSG1	Msg1;
#define	fSensorChanged		Msg1.bitSensorChanged
#define	fBtnNow				Msg1.bitBtnNow
#define	fModemRstNeeded		Msg1.bitModemRstNeeded
//#define	fKeyBlind			Msg1.bitKeyBlind
//#define	fDspWaiting			Msg1.bitDspWaiting
#define	fPowerOnReset		Msg1.bitPowerOnReset
#define	fCursorOn			Msg1.bitCursorOn
#define	fTabDigit			Msg1.bitTabDigit

//MSG2
#define	fLCD_PW_READY		Msg2.bitLCD_PW_READY
//#define	fLightSw			Msg2.bitLightSw				//背光开关状态标记
#define	fLightOnRequest		Msg2.bitLightOnRequest		//背光打开请求
#define	fRingIn				Msg2.bitRingIn
#define	fSensorRepRequest	Msg2.bitSensorRepRequest

//SW
#define swLCD				sw1.LCD_SW	
#define swLCD_LIGHT			sw1.LCD_LIGHT_SW	
#define swSD_CARD			sw1.SD_CARD_SW

//ERRORS	bytError;
#define	fERR_FRAM_SAVE	Error0.bitERR_FRAM_SAVE

//TIMERS	Tmr;
#define	fWaitSensorMeasure	Tmr.bitWaitSensorMeasurement

//TIMEUP TmrUp;
#define	fWaitSensorUp	TmrUp.bitWaitSensorUp

//========================================
//for ActiveTask, max:20
#define	TASK_PSW_EDIT			7
#define	TASK_LCD_DISP			8	//
#define	TASK_SENSOR_QUERY		9	//TASK_IDLE以上任务可多任务运行

#define	TASK_IDLE				10	//===分界线====

#define	TASK_COM1_IN			11	//TASK_IDLE以下任务任一时刻只能运行一项
#define	TASK_COM2_IN			12
#define	TASK_CALENDAR_INT_REP	13	//REPORT
#define	TASK_SENSOR_INT_REP		14	//REPORT
#define	TASK_CARD_RECORD		15

#define TASK_SLEEP				19


//========================================
//Step[]分时控制每个function
#define STEPS_PSW_EDIT			1		


//#define STEPS_IS_STR_IN_STREAM		1		//IsStrInStream()
//#define STEPS_CONNECT_TCP_SERVER	2		//ConnectTcpServer()


//========================================
// 区别Control和data通信状态
#define PTC_CTL					0		//Control状态
#define PTC_DATA				1		//Data状态


//========================================
//#define		RELEASE				2
//#define		OK					1
//#define		FAILED				0

#define		RECYCLE				0			//重入控制
#define     OK                  1
#define		OK_QUIT				2


#define     TRUE                1
#define     FALSE               0
#define     SUCCESS             1
#define     FAILED              0
#define     ENABLED             1
#define     DISABLED            0
#define		EXIST				1
#define		NOT_EXIST			0
#define		IN					1
#define		OUT					0




//#define     MEM1                1
#define 	Fosc		11059200			//HZ
#define 	Fcy			(Fosc/2)
#define		Tcy			2./Fosc				//(us), 0.18us when 11M
#define 	F_click   		500				//500HZ, 2ms/click
#define 	F_ad   			500				//500HZ, 
#define 	PR_TMR2		(Fcy/F_click)-1
#define 	PR_TMR3		(Fcy/F_ad)-1		//用于AD转换


#define		UNIT_SECOND			0
#define		UNIT_MINUTE			1
#define		UNIT_HOUR			2
#define		UNIT_DAY			3

#define		WITHIN_1S				((UINT)1000)	//ms
#define		MS100					((UINT)100)		//ms

//for SPI memory access
#define		SPI_MEM_WRITE_ENABLE		0x06
#define		SPI_MEM_WRITE_DISABLE		0x04
#define		SPI_MEM_READ_STATUS			0x05
#define		SPI_MEM_WRITE_STATUS		0x01
#define		XME_SECTOR_ERASE			0xD8
#define		SPI_MEM_READ_DATA			0x03	//存储器读取
#define		SPI_MEM_PAGE_PROGRAM		0x02	//存储器写入
#define		XME_POWER_DOWN				0xb9
#define		XME_RELEASE_POWERDOWN		0xab
#define		XME_DEVICE_ID				0x90
#define		XME_JEDEC_ID				0x9f	
#define     XME_BULK_ERASE         		0xc7


//BELT
#define		typeBELT_NULL		0
#define		typeBELT_HEAD0		1
#define		typeBELT			2
#define		typeText			3
#define		LEN_BELT0			10	//belt0: include time,voltage and etc.


//KEY
#define     KEY_UNKNOW         	0
#define     KEY1                1
#define     KEY2                2
#define     KEY3                3
#define     KEY4                4
#define     KEY_NULL            5		//无键按下


#define		KEY_WIDTH			32
#define		KEY_CONFIRM			1		//count of A/D to confirm key press


#define		KEY_BLIND			50


//================================================

#define		MS_1000				1000			//1000ms
#define     ASC_CTRL_Z          0x1a




#define		ON					1
#define		OFF					0

#define		SIGN_DIGIT			'%'
#define		SIGN_OPTION			'|'
#define		SIGN_DEFAULT		'~'
#define		SETUP_END			'^'


#define		STATUS_GPRS_MSG		1
#define		STATUS_GPRS_TRANS	2


//===========================
// Constants
//===========================
#define		START_OF_CX_MAP			1
#define		numPORT0_SETUP_ITEMS	5
#define		numPORT1_SETUP_ITEMS	4

//Baudrate
#define		BAUD_300			300		
#define		BAUD_9600			9600	
#define		BAUD_19200			19200	
#define		BAUD_57600			57600	
#define		BAUD_115200			115200

#define		DOUBLE_QUOTE		0x22	//双引号
#define		SINGLE_QUOTE		0x27	//单引号

//for ActiveCom
//#define		PORT_IDLE			0

//延时常数
#define		PORT_SLEEP			0xFF	//[0,1,2,3,0xff]
#define		WAIT_FOR_DSP		30000	//15000	//0x1F	//3F	//7F
//#define		BLINK_GAP			128		//64
//#define		TIME_COM_BLIND		0x3F	//0x3f约15s,0x7f约30s

//Hardware dependent
#define		SERIAL_PORT_NUM		0
#define		NUM_OF_CELLS		8		//8 cellx's built-in 

//Voltage
#define		VOLTAGE_OVER_CHARGE	138		//equals 13.8v
#define		WARNING_VOLTAGE		100		//equals 10.0v
#define		DEAD_VOLTAGE		85		//equals 8.5v
#define		RET_VOLTAGE_OK		1
#define		RET_VOLTAGE_WARNING 2
#define		RET_VOLTAGE_DEAD	3

#define		PTCM_NULL			0
#define		PTCM_BUILT_IN		1
#define		PTCM_RS232			2
#define		PTCM_RS485			3

//LCD page definition
#define		PAGE_MAIN_MENU			0x01
#define		PAGE_DESKTOP			0x02
#define		PAGE_STATUS				0x03		//system status
#define		PAGE_STATIC_INFO		0x04
#define		PAGE_CELL_SELECTION		0x11
#define		PAGE_CUR_VALUE			0x12
#define		PAGE_HISTORY_QUERY		0x13
#define		PAGE_SETUP				0x14
#define		PAGE_TOOLS				0x41
#define		PAGE_OTHERS				0x42
#define		PAGE_WAIT_OK			0xA0
#define		PAGE_INIT_MODEM_OK		0xA1
#define		PAGE_INIT_CARD_OK		0xA2

//general
#define		lenCELL				64
#define		lenSHORT			32	//16		//length of short instruction
#define		lenHEAD				32	//16

#define		REGISTING_ON		0x38
#define		REGISTING_OFF		0x46


//For record
#define		REC_REPEAT			0b11000000	//11xxxxxx
#define		REC_INVALID			0b01000000	//01xxxxxx
#define		REC_BEG_MARK		0xBF		//10111111
#define		REC_TIME			0x81		//10000001
#define		REC_NORMAL			0x00		//00xxxxxx

#define		SIZE_OF_REC			30		//32k-2k, memory size = SIZE_OF_REC x 1024
#define		REC_FRAM_BUF		1		//start from 0x0400
#define		REC_START			2		//start from 0x0800
//#define		SIZE_OF_BLOCK		1024
#define		REC_BLOCK_SIZE		1		//1024

#define		UNIT_SECOND			0
#define		UNIT_MINUTE			1
#define		UNIT_HOUR			2
#define		UNIT_DAY			3

#define		TIME_BIGGER			1
#define		TIME_EQUAL			2
#define		TIME_SMALLER		3

#define		YEAR				0
#define		MONTH				1
#define		DAY					2
#define		HOUR				3
#define		MINUTE				4
#define		SECOND				5

#define 	QUERY_CONTINUE			0		//continue to query
#define		QUERY_OK				1		//desire date found
#define		QUERY_BF				2		//"BF" encountered
#define		QUERY_FAILED_IN_XMEM	3
#define		QUERY_FAILED			4
#define		QUERY_REC_NOT_FOUND		5
#define		QUERY_OK_IN_REC			6
#define		QUERY_OK_IN_XME			7
#define		QUERY_PAGE_NOT_FOUND	0xffff


//滚动显示的方法
#define		WAY_CRLF  				0	//回车;
#define		WAY_CATCH  				1	//接连(不换行);
#define		WAY_OVERLAP				2	//覆盖(不换行)

#define		BLINK_GAP				128		//128ms,光标闪动一次		



//指针条定义
//#define 	REC_PTR_MOVING		0	//MEM
//#define 	REC_PTR_FIX			2	//MEM
//#define 	REC_NEXT_YMDHM		4
//#define 	REC_PAGE_ADDR		9	//XMem

//MEM1分配,临时缓冲区
//#define		DEBUG_BYTES				0x0		//临时用于DEBUG,16bytes
//#define 	BUF_FOR_XME_TO_UDISK	0x400	//FRAM缓冲区,用于写U盘.
//#define		BUF_FOR_QUERY			0x600	//用于随机查询.

//Map of Registration table
/*#define		MAP_MARK			0x7		
  #define	  bitPWR_ON			  0		//bit0:上电标志
  #define	  bitID_CHANGED		  1		//bit1:ID被重新设置标记

#define		MAP_ID_MIRROR		0x8		//2字节，放置ID镜像，用于判断ID是否已被修改。
*/
//#define		MAP_XSTP_CNT		0xb		//停振次数



//#define		MAP_C0_BLIND		0x20	//blind bytes of C0,len_c0[0],len_c0[1],...len_c0[n]

#define		REG0				((UINT)0x0)		//beginning address of cfg map


#define		PROTOCOL_YR_TEXT		0x1E	//YR文本格式
#define		PROTOCOL_YR_STD			0x1F	//YRX标准协议
#define		PROTOCOL_HYDRO			0x20	//水文规约

//for memory allocaton in InitRecord
#define		MODE_CALCULATE			0		
#define		MODE_DOWNLOAD			1		//specifid manually on computer

//for post-write
  #define	bitRecordInit 			0
  #define	bitCalendarRefresh		1
  #define	bitDoReset				2	
  #define   bitDoCfgCRC				3

//帧定义（固定）
#define		FRM_VER					0		/*1*/
#define		FRM_LEN					1		/*2*/
#define		FRM_SRC_ADDR			3		/*4*/
#define		FRM_OP					7		/*1*/
#define		FRM_PROPERTY_L			8		/*1*/
#define		FRM_PROPERTY_H			9		/*1*/
  #define	bitSPACK				0		//是否存在业务包
  #define	bitDES_CHECK			1
  #define	bitXPARA				2		//是否含扩展字符串
  #define	bitLOGIN				3
  #define   bitDO_CFG_CRC			4		//???
  #define   bitRET					7

#define		FRM_INST_INDEX			10		/*4*/



//=================== TCOM ======================
//#define		TCOM_MSG_INDEX			14		/*4*/ 
//#define		TCOM_CFG_INDEX			18		/*2*/ 
//#define		TCOM_PSW				20		/*2*/
//#define		TCOM_FAMILY_CO			23		/*1*/
//#define		TCOM_MAC_ID				24		/*6*/
//===============================================
//=================== TCOM ======================
#define		TCOM_MSG_INDEX			10		/*4*/ 
#define		TCOM_CFG_INDEX			14		/*2*/ 

#define		TCOM_FAMILY_CO			19		/*1*/
#define		TCOM_MAC_ID				20		/*6*/
#define		TCOM_PSW				26		/*4*/
//===============================================



//================ TSEN BELT ====================
//BELT(指令)
#define		TSEN_BELT_DT			14		/*6*/ 
  #define	FRM_DELAY_NEEDED		16		/*1*/ //DoMesurement,延时秒数 ????
#define		TSEN_BELT_FMT_ID		20

//BELT(响应)
#define		TSEN_BELT_CFG_INDEX		18
//1 byte reserved					20
#define		TSEN_BELT_PSW			21		/*3*/
#define		TSEN_BELT_MAC_ID		24		/*6*/
//===============================================



//=============== TSEN_RW =======================
#define		TSEN_RW_MAP_TYPE		14		/*1*/
#define		TSEN_RW_POST_WRITE		15		/*1*/
//reserved 10 bytes
#define		TSEN_RW_PSW				26		/*4*/
//===============================================

//=============== TSEN_RECS =======================
#define		TSEN_RECS_YEAR			15
#define		TSEN_RECS_MONTH			16
#define		TSEN_RECS_DAY			17
#define		TSEN_RECS_HOUR			18
#define		TSEN_RECS_MINUTE		19
#define		TSEN_RECS_SECOND		20

#define		TSEN_LEN_LIMIT			21
#define		TSEN_RECFMT				23
#define		TSEN_IC0				24
#define		TSEN_ICX				25
//===============================================



//=============== TREG_STAID ====================
//指令
#define		TREG_STAID_APPLY_PSW	14		/*3*/
#define		TREG_STAID_APPLY_FCO	23		/*1*/
#define		TREG_STAID_APPLY_MACID	24		/*6*/

//响应
#define		TREG_IDRET_CFG_INDEX	10		/*2*/
#define		TREG_IDRET_YDMHMS		12		/*6*/
//===============================================


#define		FRM_RET					29		/*1*/
#define		HCRC_L					30	
#define		HCRC_H					31

#define		FRM_BELT_LEN			32

//
#define		END_SEGMENT 		254
#define		SENDING_SEG_INDEX	10		//sending seg index
#define		SENDING_SEG_END		11		//sending num. of segs
#define		ALL_SEGMENTS		13		//seg nums



//消息
#define		MSG_REAL_DATA		0x20
#define		MSG_UPLOAD_AUTHEN	0x21
#define		MSG_UPLOAD_MAP		0x22

//TSEN
#define     OP_READ_BELT		0x10
#define     OP_READ_AREA		0x11
#define     OP_WRITE_AREA		0x12
#define     OP_CLOSE_TRANS		0x13
//#define     OP_READ_RECS		0x14
//#define     OP_WRITE_RECS		0x15

//TSEN
//#define		OP_READ_BELT		0x10
//#define		OP_READ_AREA		0x11
//#define		OP_WRITE_AREA		0x12
//#define		OP_CLOSE_TRANS		0x13
//#define		OP_READ_RECS		0x14
//#define		OP_WRITE_RECS		0x15
//#define		OP_READ_RANDOM		0x14
#define		OP_REPEAT			0x16
#define		OP_DO_MEASUREMENT	0x17
#define		OP_WRITE_SEGS		0x18


//TCOM
#define		OP_REAL_MSG			0x21
//#define	OP_ ACK			0x90
//#define	OP_NAK			0x91

//TREG
#define		OP_GET_STAID		0x31
#define		OP_GET_REG_STATUS	0x32
#define		OP_UP_AUTHEN		0x33
#define		OP_DOWN_AUTHEN		0x34
//
//#define	OP_READ_AREA		0x11
#define		OP_UPLOAD_MAP		0x11
//#define	OP_WRITE_AREA		0x12
#define		OP_DOWNLOAD_MAP		0x12
//
#define		OP_ACK				0x90
#define		OP_NAK				0x91



#define		OP_TEXT_MESSAGE		0x04
#define		OP_READ_PORT_CFG	0x05
#define		OP_WRITE_PORT_CFG	0x06

//功能码
#define		OP_NULL				0x00
#define		OP_TIMESTAMP		0x16



//TCOM
#define     OP_REAL_MSG			0x21
#define     OP_ACK				0x90
#define     OP_NAK				0x91

//TREG
#define     OP_GET_STAID		0x31
#define     OP_GET_REG_STATUS	0x32
#define     OP_UP_AUTHEN		0x33
#define     OP_DOWN_AUTHEN		0x34


#define		OP_XMEM_REPORTING		0x84	//上载计划中的xmem页

#define     UPLOAD_CFG_MAP      0
#define     UPLOAD_CALENDAR_MAP 1    
#define     UPLOAD_REC_MAP     	2
#define     UPLOAD_XME_MAP     	3

//
#define		LEN_OF_CX_QUERY		0x50

//BROADCAST instruction array(see definition about broadcast)
#define		BROADCAST_YEAR		9	
#define		BROADCAST_MONTH		10
#define		BROADCAST_DAY		11
#define		BROADCAST_HOUR		12
#define		BROADCAST_MINUTE	13
#define		BROADCAST_SEC		14

//
#define		LONGEST_INST		79

//bank2
#define		HEAD0				0
#define		DATA0				32

//com
#define		COM_NULL			0		//ActiveCom: 表示空闲
#define		COM1				1
#define		COM2				2
#define		COM3				3
#define		COM4				4


//header of comm-string
#define		INST_HEADER			'H'
#define		INST_HYD_TRANS		'%'	
#define		ADDR_CARELESS		0xffffffff

//Flag1 bit definition
#define		bit0				0
#define		bit1				1
#define		bit2				2
#define		bit3				3
#define		bit4				4
#define		bit5				5
#define		bit6				6
#define		bit7				7

//LCD line definition
#define		MAX_LCD_LINES		4	//4：LCD显示行数；0:表示没有显示器

#define		L0					0	//第一行
#define		L1					1	//第一行
#define		L2					2	//第一行
#define		L3					3	//第一行



#define 	MultiTasksOver		1
#define		MultiTasksWait		2

//For Rx
#define		RX_WAIT					0
#define		RX_OVER_TIME_OVERFLOW	1
#define		RX_OVER_WITH_ERR		2
#define		RX_OVER_WITH_ERR_FMT	3	//接收格式错误，如找不到预期的字符串“DEST=...”
#define		RX_OVER_OK				4

//For I2C
#ifdef CLOCK_5C372
  	#define		ADDR_5C372			0x64	//dev. address=[0110 010 R/W]
#else
  	#define		ADDR_PCF8583		0xA0	//dev. address=[1010 A2 A1 A0 R/W]
#endif

//IPL名称定义
#define INT_CPU			100
#define INT_LVD			101
#define INT_TIMER2		102
#define INT_CALENDER	103
#define INT_RAIN		104

#define INT_U1RX		105
#define INT_U2RX		106
//#define INT_U3RX		107
//#define INT_U4RX		108
//#define INT_U1TX		109
//#define INT_U2TX		110
//#define INT_U3TX		111
//#define INT_U4TX		112

#define	INT_AD			113
#define	INT_CN			114


//设定的Int. priority
#define		IPL_LVD_INT				7
#define		IPL_T2_INT				5
#define		IPL_CALENDAR_INT		4
#define		IPL_RAIN_INT			4

//#define	IPL_U1RX_BUSY			3
#define		IPL_U1RX_INT			2
#define		IPL_U1TX_INT			2

//#define	IPL_U2RX_BUSY			3
#define		IPL_U2RX_INT			2
#define		IPL_U2TX_INT			2

//#define	IPL_U3RX_BUSY			3
#define		IPL_U3RX_INT			2
#define		IPL_U3TX_INT			2

//#define	IPL_U4RX_BUSY			3
#define		IPL_U4RX_INT			2
#define		IPL_U4TX_INT			2

#define		IPL_AD_INT				2
#define		IPL_CN_INT				2

#define		IPL_CPU_NORMAL			0

//for RX-4571
//控制字
#define		WCTL_EXT_REG		0x1F	//[00010010], 定时器使能,1Hz,once per second.
#define		WCTL_FLAG_REG		0x00	//[00000000]，清除timer/alarm/votage-low中断标志		
#define		WCTL_REG			0x10	//[00010000]，启动时钟，使能中断		

//地址
#define		ADDR_CLOCK0			0

#define		ADDR_SEC     	    0x00
#define		ADDR_MIN     	    0x01
#define		ADDR_HOUR    		0x02
#define		ADDR_DAY     		0x04
#define		ADDR_MON     		0x05
#define		ADDR_YEAR			0x06

#define		ADDR_EXT_REG		0x0D
#define		ADDR_FLAG_REG		0x0E	//  见Datasheet
#define		ADDR_CTL_REG		0x0F

//屏蔽字
#define		SEC_MASK			0x7F
#define		MIN_MASK			0x7F
#define		HOUR_MASK       	0x3F
#define		DAY_MASK			0x3F
#define		MON_MASK			0x1F


//
#define		MASK_PORT			0b00011111
#define		MASK_DATA_TYPE		0xf0		//for cellx[18H]
#define		MASK_REC_TYPE		0x0f


//For SPI
#define		SPI_CLK_LO			1	//spi pin logic definition
#define		SPI_CLK_HI			0
#define		SPI_DO_LO			1
#define		SPI_DO_HI			0
#define		SPI_DI_LO			1
#define		SPI_DI_HI			0

//#define		WIDTH_CLK_SPI		50	//equivelent with 9600 when int. isn't taken into account
#define		TIMER_LIMIT_SPI_0	20
#define		TIMER_LIMIT_SPI_1	255	//time over
#define		SPI_BUF_LEN			64	//string len read/write through SPI


#define		SS_ERR1			1	//R
#define		SS_ERR2			2	//超时
#define		SS_ERR3			3	//短指令校验错误
#define		SS_ERR4			4	//长指令校验错误
#define		SS_ERR5			4	//数据长度超过缓冲区大小错误
#define		SS_OK			9
#define		SS_ERR			0


//北斗卫星
#define		BEIDOU_HEAD_LEN		22

//Protocol related
#define		Buf2Len				p2[2]+2
#define		DAT0				0xC		//first address of data packet


//for GPRS
#define		GPRS_CONNECTION_TIMEOUT		0
#define		GPRS_CONNECTION_OK			1
#define		GPRS_CONNECTION_ERR			2
/*
#define		GPRS_ERR_OTCP				3
#define		GPRS_ERR_ATTACH				4
#define		GPRS_ERR_CGATT				5
#define		GPRS_ERR_CGREG				6
#define		GPRS_ERR_CMEE				7
#define		GPRS_ERR_GPRSMODE			8
#define		GPRS_ERR_TCPTXDELAY			9
#define		GPRS_ERR_APNSERV			10
#define		GPRS_ERR_TCPPORT			11
#define		GPRS_ERR_ATH				12
#define		GPRS_ERR_TRANS				13
*/

//
#define		GENERAL_PORT_ENTRY	0x0000
#define		EE24C64				0x00	//dev. address

//Cell0 definition
#define		INT_REQUEST			0x3E
#define		bitBE_READ_REQUEST	5

//Cell definition
#define		CELL_WIDTH			0x23
#define		RECORD_TYPE			0x26
#define		FILE_ENTRY			0x27

//connection type
#define		CONNECT_NULL		0
#define		CONNECT_PSTN		1
#define		CONNECT_DIRECT		2
#define		CONNECT_USWAVE	  	3
#define		CONNECT_SM		    4
#define		CONNECT_GPRS		5
#define		CONNECT_GPRM		6
#define		CONNECT_WIFI		7
#define		CONNECT_BEIDOU		8
#define		CONNECT_EMAIL		9
#define     CONNECT_RING_ECHO   10      //for GSM ring-echo mode
#define     CONNECT_GPRS_TRANS	11		//GPRS透传
#define		CONNECT_ERROR		15

//filter type: filter for first char
#define     FILTER_NULL         0       //no filter needed
#define     FILTER_DIRECT       1       //filter for direct cable, INST_HEADER normally 
#define		FILTER_SLIP			2		//SLIP: HAED=0xC0,END=0xE0
#define		FILTER_GPRS_RESP	3
#define		FILTER_NIHAO		4

//record type
#define		RECORD_ABS			0
#define		RECORD_REL			1

/*
//format 
#define		FORMAT_BYTE_DATE_TIME 	1	//"2002-9-7 12:3:7" ->"2002-09-07 12:03:07" 
//数据类型
#define		FMT_UNSIGNED_BYTES		0
#define		FMT_SIGNED_BYTES		1
//#define	FMT_MCC18_FLOAT			2
#define		FMT_FLOAT				2
#define		FMT_LONG				3
#define		FMT_ZERO				4
#define		FMT_BINARY				5
#define		FMT_ERROR				0xff
*/

//数据元格式
#define		FMT_COMPACT_U			0
#define		FMT_FLOAT_F				1
#define		FMT_HEX_X				2
#define		FMT_STRING_S			3


#define     RESOLUTION_ONE          0b00001000  //0x08

//split
#define     MINI_STR_LIMIT			10

//==========================================
//      动态区(与CFG物理分开)
//
// *与REC记录区在同一芯片（0-0x800）
//
//
// 动态区（0-0x7FF）：包括运行中变化的参数：
// 	1、ptrREC指针
// 	2、MsgIndex等
//
//
// 记录区（0x800-0x7FFF)     
//  1、Rec记录
//==========================================

//1)指针宽度2+2，均为等宽冗余结构（相同字节的冗余，互为反码）
//2)范围：0x10-0x410,可支持256个记录虚元。
//3)为方便寻址，不存储的虚元也分配指针。不存储的基元不分配指针。
#define mapPTR_REC0				0x10	//62*（2+2）=0xf8。支持62个记录虚元，对用户开放50个。

#define	mapMSG_INDEX			0x500	//4+4 bytes

#define	mapCNT_RST				0x508	//2+2 bytes
#define	mapCNT_XM_FAIL			0x50C	//2+2 bytes
#define	mapCNT_MODEM_FAIL		0x510	//2+2 bytes
#define	mapCNT_FRAM_CFG_FAIL	0x514	//2+2 bytes		//FRAM_CFG存储失败
#define	mapCNT_FRAM_MEM_FAIL	0x518	//2+2 bytes		//FRAM_CFG存储失败

//#define	mapFIRMWARE_VER			0x10E	//3+3 bytes

#define	usrRAIN_L				0x540	//2+2 bytes. rain,use bak-of-rain when rain crc-error
#define	usrRAIN_H				0x541

//记录区
#define	REC_MEM_TOP				0x800	
#define	REC_MEM_BOTTOM			0x7FFF	//for 24c256


//==========================================
//以下在模拟DEBUG：
//在内存中模拟记录区
#ifdef SIM_MEM
  #define 	mapPTR_REC0				0x10	//62*（2+2）=0xf8。支持62个记录虚元，对用户开放50个。
  #define	mapMSG_INDEX			0x100	//4+4 bytes

  #define	mapCNT_RST				0x108	//2+2 bytes
  #define	mapCNT_XM_FAIL			0x10C	//2+2 bytes
  #define	mapCNT_MODEM_FAIL		0x110	//2+2 bytes
  #define	mapCNT_FRAM_CFG_FAIL	0x114	//2+2 bytes		//FRAM_CFG存储失败
  #define	mapCNT_FRAM_MEM_FAIL	0x118	//2+2 bytes		//FRAM_CFG存储失败

//#define	mapFIRMWARE_VER			0x10E	//3+3 bytes

  #define	usrRAIN_L				0x140	//2+2 bytes. rain,use bak-of-rain when rain crc-error
  #define	usrRAIN_H				0x141

//记录区
  #define	REC_SIM_TOP			0x200		
  #define	REC_SIM_BOTTOM		0x11FF
#endif



//==========================================
//      CFG & BUF
//
// *与CFG在一个芯片内;
// **CFG/CFG_MIR MAX: 16k
//==========================================
#define	CFG_BUF0			0x0000		//0000-3FFF,
#define CFG0				0x4000		//4000-7FFF,BP1:BP0=1:0,Protected	


#define		SIZE_OF_REC_STRUCT	10
#define		PTR_XMEM_REPORTING	11			//


#define		REC_FMT_BEGIN		0xBF		//0b10111111
#define		REC_FMT_END			0xBC		//0b10111100
#define		REC_FMT_TIME		0x81		//0b10000001

#define		TIME_FIT			1
#define		NO_TIME_FIT			0



//===========================


//type of sensor sensor
#define		SENSOR_PORT_SPI		1
#define		SENSOR_PORT_SERIAL	2

//SerialSensor RW speed mode
#define		MODE_QUICK			0		//不等待传感器，通常用于显示慢速传感器
#define		MODE_NORMAL			1		//等待传感器

//RESPONSE
#define		RESPONSE_ERR		0		//Instruction can't be executed.
#define		RESPONSE_OK			1		//Instruction executed successfully.


#define		STEP_DOWNLOAD_OVER			3
#define		STEP_DOWNLOAD_FAILURE		4
#define		STEP_DOWNLOAD_ACK	        5

#define		STEP_UPLOAD_OVER	        6
#define		STEP_UPLOAD_FAILURE	    	7
#define     STEP_UPLOAD_ACK           	8

#define		STEP_RESET					10
#define		STEP_AUTHEN_FAILURE			11
#define		STEP_ERR_CFG				12
//
#define		NUM_OF_SPI			4
#define     LEN_OF_AT_CMGS      12  //len("AT+CMGS=xxx[CR]")
#define     LEN_OF_SM_HEAD


//BELT
#define		typeBELT_NULL		0
#define		typeBELT_HEAD0		1
#define		typeBELT			2
#define		typeText			3
#define		LEN_BELT0			10	//belt0: include time,voltage and etc.


//================================
#define		KEY0_UP				1
#define		KEY0_DOWN			2
#define		KEY0_OK				3
//==
#define		KEY11_RETURN		1
#define		KEY11_UP			2
#define		KEY11_DOWN			3
#define		KEY11_OK			4
//==
#define		KEY12_RETURN		1
#define		KEY12_UP			2
#define		KEY12_DOWN			3
#define		KEY12_REFRESH		4
//==
#define		KEY13_RETURN		1
#define		KEY13_BACKWARD		2
#define		KEY13_FORWARD		3
#define		KEY13_REFRESH		4
//==
#define		KEY_A				1
#define		KEY_B				2
#define		KEY_C				3
#define		KEY_D				4


/*
//Ux接收任务编号：U1RX_ROUTINE/U2RX_ROUTINE
#define		URX_GPRS_MSG		1
#define		URX_GPRS_TRANS		2
#define		URX_CSQ				3
#define		URX_BDQ				4	
*/

//Port not respond
#define		ERR_PORT_ACCESS		0x41	//01xxxxxx=01_000001 


//Restart (error) code
#define		RST_FOR_MAIN_90			90		//0x5a 重启_写注册后
#define		RST_FOR_MAIN_91			91		//0x5b	
#define		RST_FOR_MAIN_92			92		//0x5c 重启_注册后	
#define		RST_FOR_MAIN_93			93		//0x5d 重启_外部指令	
#define		RST_FOR_MAIN_94			94		//0x5e 重启_自动校时后	
#define		RST_FOR_MAIN_95			95		//0x5f 重启_进入“设置”后	

#define		RST_FOR_ERR_MAIN_0		0		//0
#define		RST_FOR_ERR_MAIN_1		1		//1
#define		RST_FOR_ERR_MAIN_2		2		//2
#define		RST_FOR_ERR_MAIN_3		3		//3
#define		RST_FOR_ERR_MAIN_4		4		//4
#define		RST_FOR_ERR_MAIN_5		5		//5
#define		RST_FOR_ERR_MAIN_6		6		//6
#define		RST_FOR_ERR_MAIN_7		7		//7
#define		RST_FOR_ERR_MAIN_8		8		//8
#define		RST_FOR_ERR_MAIN_9		9		//9
#define		RST_FOR_ERR_MAIN_10		10		//0xa
#define		RST_FOR_ERR_MAIN_11		11		//0xb	
#define		RST_FOR_ERR_MAIN_12		12		//0xc
#define		RST_FOR_ERR_MAIN_13		13		//0xd	
#define		RST_FOR_ERR_MAIN_14		14		//0xe

	
#define		RST_FOR_ERR_DSP_20		20		//0x14	
#define		RST_FOR_ERR_DSP_21		21		//0x15	
#define		RST_FOR_ERR_DSP_22		22		//0x16	

#define		RST_FOR_ERR_PWR_30		30		//0x1e

#define		RST_FOR_ERR_CALENDAR_1	35		//0x23

//为避免过多占用有限的内存资源，设置本表。
//理想情况为全部一致，可以使每个COM口通用，本组数据应根据CPU型号内存的大小调整。
#define		RX_BUF_SIZE1			2088	//最大接收缓冲：32+(2+4+2048)+2=2088	
#define		RX_BUF_SIZE2			2088	//最大接收缓冲：32+(2+4+2048)+2=2088	
#define		RX_BUF_SIZE3			256		//传感器接口	


//Macros
#define		BUF_TX_BEG			(&tx[0])
#define		BUF_TX_END			(&tx[255])

#define		BUF3_END			(&rx3[255])		//传感器接口      

//===============================
//Station Info definition
//===============================
/*
//#define		MAIN_CHN_NAME 			0x6        	//主机名称
#define		MAIN_VERSION  			0x16   		//软件版本
//#define		MAIN_NUM_OF_CELLS 		0x18       	//虚元数
//#define		MAIN_NUM_OF_PORTS 		0x19     	//模块数
//#define		MAIN_LEN_IN_PAGES 		0x1A       	//本表长度
//#define		MAIN_YEAR_BIAS			0x1B		//年偏置
#define		MAIN_CSQ				0x1C		//CSQ	
#define		MAIN_COM_FAIL_COUNT		0x1D		//通信失败计数	
  #define	  bitCOM_FAILURE	  			7
#define		MAIN_XMEM_FAIL			0x1E		//外存操作失败	
  #define	  bitXMEM_INIT_FAILED	  		7
  #define	  bitXMEM_CLR_CX_SECTOR_FAILED  6
  #define	  bitXMEM_PROGRAM_FAILED  		5

#define		MAIN_MODEM_VERSION		0x1F		//Modem型号或版本标记
#define		MAIN_PASSWORD			0x20		//终端设置密码[2]
#define		MAIN_FATAL_ERROR		0x22		//致命错误[1]		
#define		MAIN_UDISK_DAYS			0x23		//U盘天数，0：全部至空白页，1-250：天数



//'.......................
#define		MAIN_UNSO_INTERVAL		0x27		
#define		MAIN_DEF_BELT_CFG		0x28       	//系统信息有无，当前值有无，记录有无
#define		bitSYS_INFO				2
#define		bitCUR_VAL				1
#define		bitRECORD				0

#define		MAIN_PROPERTY1 			0x29       	//时钟有无，是否中继站，记录是否
  #define bitHOME_PAGE_SWITCH		3		//无请求不发	1：允许；0：必须发

#define		MAIN_PROPERTY2 			0x2A       	//Bus通信允许，Instant通信允许
  #define bitNO_BUS_NO_REQUEST			3		//无请求不发	1：允许；0：必须发
  #define bitSENSOR_REPORT_REQUEST		4		//传感器请求(参数变化)
  #define bitSM_ON_HOUR					6
  #define bitINSTANT_REPORTING_ENABLED	7		//立即发送允许


#define		MAIN_VOLTAGE 			0x2F

#define		MAIN_DATE 				0x30        //年月日
#define		MAIN_YEAR				0x30
#define		MAIN_TIME 				0x33        //时分秒

#define		MAIN_SELF_TEL_NUM 		0x36   		//本机号码

#define		MAIN_CRC_L 		0x3E
#define		MAIN_CRC_H 		0x3F

//page-2
#define		MAIN_COM1				0x40
#define		MAIN_COM2				0x48
#define		MAIN_COM3				0x50
#define		MAIN_COM4				0x58

#define		MAIN_TEL1				0x40
#define		MAIN_TEL2				0x48
#define		MAIN_TEL3				0x50
#define		MAIN_TEL4				0x58

#define		MAIN_MONITOR_TEL		0x60		//上报监视电话
*/

//===============================
//Cell0(basic) definition
//===============================
//#define		C0_CFG_WORD				0x00	//总配置字		2	配置区特征值
//#define		C0_SERIAL_NUM			0x02	//模块序列号	2	Y	
//#define		C0_CHN_NAME				0x04	//模块中文名称	16	Y	
//#define		C0_PRODUCTION_DATE		0x14	//生产年月		2	Y	(BCD)
//#define		C0_NUM_OF_CELLX			0x16	//虚元数		1		
//#define		C0_NUM_OF_PORT			0x17	//端口数		1	Y	

/*
#define		C0_PROPERTY1			0x18
#define     bitC0P1_CALENDAR_EXIST  0       //18H.0 时钟（有无）	1-bit	1：有  		0：无
#define     bitC0P1_ENABLED    		1	    //18H.1 基元使能        1-bit   1: TRUE		0：FALSE
#define     bitC0P1_Update_Calendar 2	    //18H.2 Update Date & Time      1-bit       1:Yes  0:No
#define     bitC0P1_AS_REPEATER     6       //18H.6	中继站（是否）1-bit		1：是（路由表有效）0：否（路由表无效）
#define     bitC0P1_MODULE_RECORD   7       //18H.7	记录（是否）	1-bit		1：是  0：否

//#define		C0_LEN					0x19	// x 64
#define		C0_BELT_CFG				0x1B	//3 bytes
#define		C0_CFG_PAGES			0x1E	//1 bytes
//#define		C0_MAX_DELAY			0x1F
			
#define		C0_PROPERTY2			0x36
//36H.7	Instant通信（是否）	1-bit		1：是  0：否
//36H.6	Bus通信（是否）		1-bit		1：是  0：否
//36H.5	请求（YN）			1-bit		所有请求的"或"，[1：Y][0：N]
//36H.4	启动测量标志（YN）			

#define		C0_VOLTAGE				0x37	//电源电压	1	Y	
#define		C0_YMD					0x38	//年月日	3		
#define		C0_HMS					0x3B	//时分秒	3	

//1 请求    0 无请求[3EH][3FH]=[v8 v7 … v1][v16 v15 …v9]	
#define		C0_REQUEST_WORD			0x3E	//虚元请求标志	2
*/

//=============================
//Cellx definition
//=============================
/*
#define		CX_CFG					0x00	//虚元配置字		2		虚元配置特征值
#define		CX_SERIAL_NUM			0x02	//虚元序列号（基元号[0x02],虚元号[0x03]）	
#define		CX_CHN_NAME				0x04	//虚元中文名称（含单位）	16	Y	
#define		CX_RESOLUTION			0x14	//分辨力			1		
//#define		CX_VAL_LEN				0x15	//有效长度		1		数据长度
#define		CX_REC_INTERVAL			0x16	//记录间隔		1		05=5分钟
#define		CX_INTERVAL_UNIT		0x17	//记录间隔单位	bit0-3		1：second2: minute3：hour4：day
#define		CX_VALID_BITS			0x17	//最高字节有效比特数 bit4-6 范围0-7
#define		CX_REC_TYPE				0x18	//记录方式：		1		1：绝对值2：相对值3：开关量4：图元（任意数据）
//#define		CX_REC_ENTRY			0x19	//记录文件入口	2		绝对地址=入口 * SIZE_OF_BLOCK 
#define		CX_LEN					0x1B	//Len of CELL, long cell is permitted
#define		CX_DSP_FORMAT			0x1C
#define		CX_SUB_INTERVAL_OFFSET	0x1D	//sub interval unit offset
#define		CX_TRAIL_LEN			0x1E	//length of trail
//#define		CX_REC_EXIT				0x1F	//记录文件出口	2		绝对地址=入口 * SIZE_OF_BLOCK 
#define		CX_DELAY				0x21

#define		CX_SECTOR_ENTRY			0x29
#define		CX_SECTOR_EXIT			0x2A	//实际出口=CX_SECTOR_EXIT-1

#define		CX_YES_NO_1				0x30				
#define		bitYN_REC				7		//30H.7	记录（是否）			1：是  0：否
#define		bitYN_INSTANT_COM		6		//30H.6	Instant通信（是否）	1：是  0：否
#define		bitYN_BUS_COM			5		//30H.5	Bus通信（是否）		1：是  0：否
#define		bitYN_IN_BELT			4		//30H.4	当前值带				1：是  0：否
#define		bitYN_INTERRUPT_COM		3		//30H.3	Interrupt通信		1：是  0：否	
#define		bitYN_DSP_ENABLED		2		//30H.2	显示允许				1：是  0：否
#define		bitYN_DESKTOP_SHOW		1		//30H.1 桌面虚元显示			1：是  0：否
#define		bitYN_SAVE_XMEM			0		//30H.0 转储虚元			1：是  0：否
				
#define		CX_YES_NO_2				0x31
#define		bitYN_ZB				0		//31H.0	请求自报（YN）	1		1 自报， 0 不自报
#define		bitYN_ZB_CHANNEL1		1		//31H.1	自报通道1(YN)			
#define		bitYN_ZB_CHANNEL2		2		//31H.2	自报通道2(YN)			
#define		bitYN_TF_CARD			3		//31H.3 TF卡记录	
				
#define		CX_CURRENT_VAL			0x3F	//3FH	当前值	2        line backward		

#define		CX_CRC_L				0x3E	//when registration 
#define		CX_CRC_H				0x3F	//  for crc
*/

//======================================
// MapType定义
//======================================
#define		M_CFG			1
#define		M_MEM			2
#define		M_XME			3		//用于存放下载的APP
#define		M_CALENDAR		4
#define		M_CARD			5
#define		M_PROGRAM		6
#define		M_PICTURE		7
#define		M_LOG			8
#define		M_REC2			9		//虚元记录，反向

//==========================
//        USER map
//==========================
//Start from 0x40-0x7F 

#define		U_BELT_INCLUDE		(UINT)(CM_C0_START)+0x40	//L: USER_CFG0

//address of byte which include a bit(bitLEVEL) defining 
#define		U_BITS_SETTINGS		(UINT)(CM_C0_START)+0x41	
//{
#define		bitLEVEL1_NOT		0	//if a not-operation should be applied
#define		bitRAIN_SW			1	//if a rain sensor exist
#define		bitENCODE_SW		2	//if an encoder exist
//}

#define		U_ENCODE_CO			(UINT)(CM_C0_START)+0x42	//length 4bytes
#define		U_RAIN_ALARM		(UINT)(CM_C0_START)+0x46	//len=3
#define		U_ENCODE_ALARM		(UINT)(CM_C0_START)+0x49	//len=5
#define		U_ENCODE_BASE		(UINT)(CM_C0_START)+0x4E	//len=5
//								

//#define		U_RAIN_L			(UINT)(CM_C0_START)+0x70	//rain,use bak-of-rain when rain crc-error
//#define		U_RAIN_H			(UINT)(CM_C0_START)+0x71


//USER map end
//==============================

