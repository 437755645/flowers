#include 	"header.h"
#include    "h_macro.h"

//保存着全部IPL当前值
BYTE IPL_CPU;
BYTE IPL_Lvd;
BYTE IPL_Timer2;
BYTE IPL_Calendar;
BYTE IPL_Rain;
BYTE IPL_U1rx;
BYTE IPL_U2rx;
BYTE IPL_AD;
BYTE IPL_CN;

void UART1Init(ULNG Baudrate)
{
	asm volatile ( 
	"MOV #OSCCON, w1 \n"
	"MOV #0x46, w2 \n"
	"MOV #0x57, w3 \n"
	"MOV.b w2, [w1] \n"
	"MOV.b w3, [w1] \n"
	"BCLR OSCCON,#6");
	RPINR18bits.U1RXR =8;   //RX1=RP8
	RPOR4bits.RP9R = 3;

	asm volatile ( 
	"MOV #OSCCON, w1 \n"
	"MOV #0x46, w2 \n"
	"MOV #0x57, w3 \n"
	"MOV.b w2, [w1] \n"
	"MOV.b w3, [w1] \n"
	"BSET OSCCON, #6" );

  	U1MODEbits.RTSMD=1;       	//UxRTS引脚处于单工模式
	U1MODEbits.UEN=0;         	//使能并使用UxTX 和UxRX 引脚； UxCTS 和UxRTS/BCLKx 引脚由端口锁存器控制
	U1MODEbits.BRGH=0;        	//BRG 在每个位周期内产生16 个时钟信号（16 倍频波特率时钟，标准模式）
	U1MODEbits.PDSEL=0;       	//8 位数据，无奇偶校验
	U1MODEbits.STSEL=0;       	//1 个停止位
	U1MODEbits.WAKE=1;

  	U1BRG=((((11059200/2)/Baudrate)/16)-1);   //波特率9600
  	COM1_EN=1;      			//使能UARTx ；
//	_NSTDIS=1;     				//禁止中断嵌套
	_IPL3=0;        			//CPU 中断优先级小于等于7

	U1STAbits.URXISEL=0;		//当接收到一个字符且RSR的内容被传输给接收缓冲器时，中断标志位置1
	RX1_IE=1;        		  	//UART1 接收器中断允许位
	_U1RXIP=1;        			//UART1 接收器中断优先级位
	TX1_EN=1;        			//使能发送， UARTx 控制UxTX 引脚。

}


void UART2Init(ULNG Baudrate)
{

	asm volatile ( 
	"MOV #OSCCON, w1 \n"
	"MOV #0x46, w2 \n"
	"MOV #0x57, w3 \n"
	"MOV.b w2, [w1] \n"
	"MOV.b w3, [w1] \n"
	"BCLR OSCCON,#6");
RPINR19bits.U2RXR =26;      //RX2=RP26
RPOR10bits.RP21R = 5;

	asm volatile ( 
	"MOV #OSCCON, w1 \n"
	"MOV #0x46, w2 \n"
	"MOV #0x57, w3 \n"
	"MOV.b w2, [w1] \n"
	"MOV.b w3, [w1] \n"
	"BSET OSCCON, #6" );

  	U2MODEbits.RTSMD=1;       	//UxRTS引脚处于单工模式
//	U2MODEbits.UEN=3;         	//使能并使用UxTX 、UxRX 和BCLKx 引脚； UxCTS 引脚由端口锁存器控制
	U2MODEbits.UEN=0;         	//使能并使用UxTX 和UxRX 引脚； UxCTS 和UxRTS/BCLKx 引脚由端口锁存器控制
	U2MODEbits.BRGH=0;        	//BRG 在每个位周期内产生16 个时钟信号（16 倍频波特率时钟，标准模式）
	U2MODEbits.PDSEL=0;       	//8 位数据，无奇偶校验
	U2MODEbits.STSEL=0;       	//1 个停止位
	U2MODEbits.WAKE=1;

  	U2BRG=((((11059200/2)/Baudrate)/16)-1);   //波特率9600
  	COM2_EN=1;      			//使能UARTx ；
//	_NSTDIS=1;     				//禁止中断嵌套
	_IPL3=0;        			//CPU 中断优先级小于等于7

	U2STAbits.URXISEL=0;		//当接收到一个字符且RSR的内容被传输给接收缓冲器时，中断标志位置1
	RX2_IE=1;        		  	//UART1 接收器中断允许位
	_U2RXIP=1;        			//UART1 接收器中断优先级位
	TX2_EN=1;        			//使能发送， UARTx 控制UxTX 引脚。
}


void UART3Init(ULNG Baudrate)
{
	asm volatile ( 
	"MOV #OSCCON, w1 \n"
	"MOV #0x46, w2 \n"
	"MOV #0x57, w3 \n"
	"MOV.b w2, [w1] \n"
	"MOV.b w3, [w1] \n"
	"BCLR OSCCON,#6");
	RPINR17bits.U3RXR =19;      //RX3=RP19
	RPOR13bits.RP27R = 28;

	asm volatile ( 
	"MOV #OSCCON, w1 \n"
	"MOV #0x46, w2 \n"
	"MOV #0x57, w3 \n"
	"MOV.b w2, [w1] \n"
	"MOV.b w3, [w1] \n"
	"BSET OSCCON, #6" );

  	U3MODEbits.RTSMD=1;       	//UxRTS引脚处于单工模式
//	U3MODEbits.UEN=3;         	//使能并使用UxTX 、UxRX 和BCLKx 引脚； UxCTS 引脚由端口锁存器控制
	U3MODEbits.UEN=0;         	//使能并使用UxTX 和UxRX 引脚； UxCTS 和UxRTS/BCLKx 引脚由端口锁存器控制
	U3MODEbits.BRGH=0;        	//BRG 在每个位周期内产生16 个时钟信号（16 倍频波特率时钟，标准模式）
	U3MODEbits.PDSEL=0;       	//8 位数据，无奇偶校验
	U3MODEbits.STSEL=0;       	//1 个停止位
  	U3BRG=((((11059200/2)/Baudrate)/16)-1);   //波特率9600
  	COM3_EN=1;      			//使能UARTx ；
//	_NSTDIS=1;     				//禁止中断嵌套
	_IPL3=0;        			//CPU 中断优先级小于等于7

	U3STAbits.URXISEL=0;		//当接收到一个字符且RSR的内容被传输给接收缓冲器时，中断标志位置1
	RX3_IE=1;        		  	//UART1 接收器中断允许位
	_U3RXIP=1;        			//UART1 接收器中断优先级位
	TX3_EN=1;        			//使能发送， UARTx 控制UxTX 引脚。
}


void UART4Init(ULNG Baudrate)
{
	asm volatile ( 
	"MOV #OSCCON, w1 \n"
	"MOV #0x46, w2 \n"
	"MOV #0x57, w3 \n"
	"MOV.b w2, [w1] \n"
	"MOV.b w3, [w1] \n"
	"BCLR OSCCON,#6");

	RPINR27bits.U4RXR =0;       //RX4=RP0 与图纸对不上
	RPOR0bits.RP1R =30;

	asm volatile ( 
	"MOV #OSCCON, w1 \n"
	"MOV #0x46, w2 \n"
	"MOV #0x57, w3 \n"
	"MOV.b w2, [w1] \n"
	"MOV.b w3, [w1] \n"
	"BSET OSCCON, #6" );

  	U4MODEbits.RTSMD=1;       	//UxRTS引脚处于单工模式
//	U4MODEbits.UEN=3;         	//使能并使用UxTX 、UxRX 和BCLKx 引脚； UxCTS 引脚由端口锁存器控制
	U4MODEbits.UEN=0;         	//使能并使用UxTX 和UxRX 引脚； UxCTS 和UxRTS/BCLKx 引脚由端口锁存器控制
	U4MODEbits.BRGH=0;        	//BRG 在每个位周期内产生16 个时钟信号（16 倍频波特率时钟，标准模式）
	U4MODEbits.PDSEL=0;       	//8 位数据，无奇偶校验
	U4MODEbits.STSEL=0;       	//1 个停止位
  	U4BRG=((((11059200/2)/Baudrate)/16)-1);   //波特率9600
  	COM4_EN=1;      			//使能UARTx ；
//	_NSTDIS=1;     				//禁止中断嵌套
	_IPL3=0;        			//CPU 中断优先级小于等于7

	U4STAbits.URXISEL=0;		//当接收到一个字符且RSR的内容被传输给接收缓冲器时，中断标志位置1
	RX4_IE=0;        		  	//关闭 UART4 接收器中断允许位
	_U4RXIP=1;       			//UART4 接收器中断优先级位
	TX4_EN=1;        			//使能发送， UARTx 控制UxTX 引脚。
}


/*****************************************
*外部中断设置
*****************************************/
void InitINTx(void)
{
asm volatile ( 
	"MOV #OSCCON, w1 \n"
	"MOV #0x46, w2 \n"
	"MOV #0x57, w3 \n"
	"MOV.b w2, [w1] \n"
	"MOV.b w3, [w1] \n"
	"BCLR OSCCON,#6");
	RPINR0bits.INT1R =16;//时钟
	RPINR1bits.INT2R =37;//雨量
	RPINR1bits.INT3R =28;//GPRS RING
	asm volatile ( 
	"MOV #OSCCON, w1 \n"
	"MOV #0x46, w2 \n"
	"MOV #0x57, w3 \n"
	"MOV.b w2, [w1] \n"
	"MOV.b w3, [w1] \n"
	"BSET OSCCON, #6" );

INTCON2bits.INT1EP=1;//时钟外部中断1,下降沿
INTCON2bits.INT2EP=1;//雨量外部中断2,下降沿

CALENDAR_IF=0;					//IFS1bits.INT1IF=0;
CALENDAR_IE=FALSE;				//IEC1bits.INT1IE=0;
//*
RAIN_IF=0;					//IFS1bits.INT2IF=0;
RAIN_IE=FALSE;				//IEC1bits.INT2IE=0;

GPRS_Ring_IE=0;					
GPRS_Ring_IF=FALSE;				
}

/*********************************
*CN外部输入电平变化初始化
*********************************/
void InitCN(void)
{ 	
   CNEN1bits.CN5IE = 1; 		//Key 外部输入电平变化中断控制
    IPC4bits.CNIP = 2;        	//中断优先级设置
    
    IFS1bits.CNIF = 0;         	//中断标志清零 
    IEC1bits.CNIE = 1;         	//中断允许   
}




//初始化I/O
void InitCPU(void)
{
/* INTCON1 */
	_OSCFAIL=0;
	_STKERR=0;
	_ADDRERR=0;
	_MATHERR=0;
	_NSTDIS=0;				//0：嵌套允许，1：嵌套不允许

//
	_IPL3=0;				//CPU 中断优先级小于等于7

//
	_ALTIVT=0;

	di_all();
	AD1PCFG=0xffff;




//UART1
	TRISBbits.TRISB8=1;		//GPRS RX
	TRISBbits.TRISB9=0;		//GPRS TX
	TRISBbits.TRISB10=0;	//SLEEP1
	PORTBbits.RB10=0;

//UART2
	TRISGbits.TRISG7=1;    	//RX
	TRISGbits.TRISG6=0;		//TX
	TRISEbits.TRISE6=0;		//SLEEP2
	PORTEbits.RE6=0;
	TRISEbits.TRISE7=0;		//RST2
	PORTEbits.RE7=0;

//UART3
	TRISGbits.TRISG8=1;		//RX
	TRISGbits.TRISG9=0;		//TX
	TRISEbits.TRISE5=0;		//SLEEP3
	PORTEbits.RE5=0;
	TRISBbits.TRISB4=0;		//U3RST
	PORTBbits.RB4=0;
	TRISBbits.TRISB5=0;		//U3CTS
	PORTBbits.RB5=0;

//UART4
	TRISBbits.TRISB1=1;		//LCD RX
	TRISBbits.TRISB0=0;		//LCD TX

//水位/雨量
	TRISDbits.TRISD3=1;		//P_DI
	TRISDbits.TRISD2=0;		//P_MS!
	PORTDbits.RD2=1;
	TRISDbits.TRISD1=0;		//P_CLK
	PORTDbits.RD1=1;

	RAIN_DIR=IN;		//TRISCbits.TRISC14=1;	//P
//GPRS RING
    GPRS_Ring_DIR=1;    //ring
//存储器
	TRISFbits.TRISF5=0;		//CS_XME
	PORTFbits.RF5=1;
	TRISFbits.TRISF4=0;		//CS_MEM1
	PORTFbits.RF4=1;

	TRISBbits.TRISB15=0;	//CS_MEM2
	PORTBbits.RB15=1;
	TRISBbits.TRISB14=1;	//S0
	TRISBbits.TRISB13=0;	//SI
	PORTBbits.RB13=1;

//初始化RX4571 I/O
	TRISFbits.TRISF2=0;		//SDA
	PORTFbits.RF2=1;
	TRISBbits.TRISB12=0;	//SCLK
	PORTBbits.RB12=1;
	TRISFbits.TRISF6=0;		//CE
	PORTFbits.RF6=0;
//DS_2411
	TRISDbits.TRISD8=1;		//DS_2411

//电源控制
	TRISDbits.TRISD4=0;		//v12
	PORTDbits.RD4=0;
	TRISDbits.TRISD5=0;		//V3.3
	PORTDbits.RD5=0;
	TRISDbits.TRISD6=0;		//V5
	PORTDbits.RD6=0;
//KEY
	TRISBbits.TRISB3=1;		//KEY4
//	TRISBbits.TRISB2=1;		//AN_KEY
//AD_PWR
//	TRISBbits.TRISB11=1;	//AD
//BUZZ1
	BUZZ_DIR=OUT;			//BUZZ1
    swBUZZ=OFF;
}

void Timer1_Init_Function()     //定时器1用于工作和连续状态的计时                
{
	T1CON =0X0020;              //定时器1为64分频 在空闲模式下模块继续工作
	TMR1  =0;
	PR1=6912;          			//定时10ms
	T1_IE=1;            		//中断使能
	T1_IF=0;
	T1_IP=4;
	T1CON =0X8022;  
}


void Timer2_on(void)
{			
	T2CON=0x0000;		//00000000 00000000	1:1分频
    TMR2=0x0000; 		
    PR2=PR_TMR2;		//0x1598 when fosc=11M		
    T2_IF = 0;			
    T2_IE = 1;			
    T2_ON = 1;
}

void Timer2_off(void)
{			
    T2_IE = 0;			
    T2_ON = 0;
}


void InitIPL(void)
{
	di_all();

//LVD 7
	SetIPL(INT_LVD,IPL_LVD_INT);

//for 2ms ms_click
	SetIPL(INT_TIMER2,IPL_T2_INT);

//for calendar int
	SetIPL(INT_CALENDER,IPL_CALENDAR_INT);

//for rain signal
	SetIPL(INT_RAIN,IPL_RAIN_INT);

//for U1Rx,U2Rx
	SetIPL(INT_U1RX,IPL_U1RX_INT);
	SetIPL(INT_U2RX,IPL_U2RX_INT);

//for A/D
	SetIPL(INT_AD,IPL_AD_INT);

//for CN(键盘)
	SetIPL(INT_CN,IPL_CN_INT);


//	ei_all();
}

//=========================================
// level: 0-15
//=========================================
void SetIPL(BYTE event_id,UINT level)
{
	level&=0x000f;
	switch(event_id){
	case INT_CPU:
	  SET_CPU_IPL(level);
	  IPL_CPU=level;
	  break;

	case INT_LVD:
	  IPC18=(IPC18&0xfff8)|level;	//IPC18<2:0>
	  IPL_Lvd=level;
	  break;

	case INT_TIMER2:
	  //IPC1=(IPC1&0x0fff) | 0x6000;
	  IPC1=(IPC1&0x0fff) | (level<<12);
	  IPL_Timer2=level;
	  break;

	case INT_CALENDER:
	  //IPC5=((IPC5&0xfff0)|0x0005);
	  IPC5=(IPC5&0xfff0)|level;
	  IPL_Calendar=level;
	  break;

	case INT_RAIN:
	  //IPC7=(IPC7&0xff0f)|0x0050;
	  IPC7=(IPC7&0xff0f)|(level<<4);
	  IPL_Rain=level;
	  break;

	case INT_U1RX:				//IPC2<14:12>	
	  IPC2=(IPC2&0x8fff)|(level<<12);
	  IPL_U1rx=level;
	  break;

	case INT_U2RX:				//IPC7<10:8>	
	  IPC7=(IPC7&0xf8ff)|(level<<8);
	  IPL_U2rx=level;
	  break;

	case INT_AD:				//IPC3<6:4>	
	  IPC3=(IPC3&0xff8f)|(level<<4);
	  IPL_AD=level;
	  break;

	case INT_CN:				//IPC4<14:12>	
	  IPC4=(IPC4&0x8fff)|(level<<12);
	  IPL_CN=level;
	  break;
	}
}

/*
#define SET_CPU_IPL(ipl) {       \
  int DISI_save;                 \
                                 \
  DISI_save = DISICNT;           \
  asm volatile ("disi #0x3FFF"); \
  SRbits.IPL = ipl;              \
  DISICNT = DISI_save; } (void) 0;
*/
