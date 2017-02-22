//========================================
// 端口定义
//========================================
#define		M_SPI_ClkOut	PORTBbits.RB12
#define		M_SPI_DI		PORTBbits.RB13
//xMEM
#define		CS_MEM			PORTFbits.RF4
#define		CS_MEM_DIR		TRISFbits.TRISF4
#define		CS_CFG			PORTFbits.RF5
#define		CS_CFG_DIR		TRISFbits.TRISF5
#define		CS_XME	        PORTBbits.RB15 
#define		CS_XME_DIR		TRISBbits.TRISB15 
#define		DO_XME			PORTBbits.RB14
#define		DO_XME_DIR		TRISBbits.TRISB14
//水位
#define		L_CLK			PORTDbits.RD1
#define		L_CLK_DIR		TRISDbits.TRISD1
#define		L_DI			PORTDbits.RD3
#define		L_DI_DIR		TRISDbits.TRISD3
#define		L_PS			PORTDbits.RD2
#define		L_PS_DIR		TRISDbits.TRISD2
//LED
#define		LED_STATUS_DIR	TRISGbits.TRISG2
#define		LED_STATUS		PORTGbits.RG2
//雨量
#define		XRST			PORTCbits.RC14
#define		XRST_DIR		TRISCbits.TRISC14

//电源控制
#define		CTL_NON			PORTDbits.RD4//V12
#define		CTL_NON_DIR		TRISDbits.TRISD4
#define		CTL_33V			PORTDbits.RD5//V3.3
#define 	CTL_33V_DIR		TRISDbits.TRISD5
#define 	CTL_NOFF		PORTDbits.RD6//V5
#define 	CTL_NOFF_DIR	TRISDbits.TRISD6
#define 	CTL_BEIDOU		PORTBbits.RB5
#define 	CTL_BEIDOU_DIR	TRISBbits.TRISB5


//BUZZER
#define		BUZZ_DIR		TRISGbits.TRISG3
#define		swBUZZ			PORTGbits.RG3


//SLEEP
#define	   	SLEEP1_DIR     	TRISBbits.TRISB10
#define	   	SLEEP1         	PORTBbits.RB10
#define	   	SLEEP2_DIR     	TRISEbits.TRISE6
#define	   	SLEEP2         	PORTEbits.RE6
#define	   	SLEEP3_DIR     	TRISEbits.TRISE5
#define	   	SLEEP3         	PORTEbits.RE5
//AN_KEY
#define		btnKey			PORTBbits.RB3

//PORTB
#define		SensorInt		PORTBbits.RB4		

//===========================
// 通信
//===========================
#define		RX1_IE		IEC0bits.U1RXIE
#define		RX2_IE  	IEC1bits.U2RXIE
#define		RX3_IE		IEC5bits.U3RXIE
#define		RX4_IE   	IEC5bits.U4RXIE

#define		TX1_IE   	IEC0bits.U1TXIE
#define		TX2_IE  	IEC1bits.U2TXIE
#define		TX3_IE  	IEC5bits.U3TXIE
#define		TX4_IE  	IEC5bits.U4TXIE

#define	    RX1_IF  	IFS0bits.U1RXIF
#define	    RX2_IF   	IFS1bits.U2RXIF
#define	    RX3_IF  	IFS5bits.U3RXIF
#define	    RX4_IF   	IFS5bits.U4RXIF

#define	    TX1_IF  	IFS0bits.U1TXIF
#define	    TX2_IF   	IFS1bits.U2TXIF
#define	    TX3_IF  	IFS5bits.U3TXIF
#define	    TX4_IF   	IFS5bits.U4TXIF

#define		COM1_EN		U1MODEbits.UARTEN
#define		COM2_EN		U2MODEbits.UARTEN
#define		COM3_EN		U3MODEbits.UARTEN
#define		COM4_EN		U4MODEbits.UARTEN

#define		TX1_EN		U1STAbits.UTXEN
#define		TX2_EN		U2STAbits.UTXEN
#define		TX3_EN		U3STAbits.UTXEN
#define		TX4_EN		U4STAbits.UTXEN


#define		TX1_DIR		TRISBbits.TRISB9
#define		RX1_DIR		TRISBbits.TRISB8
#define		TX1			PORTBbits.RB9
#define		RX1			PORTBbits.RB8
#define		TX2_DIR		TRISGbits.TRISG6
#define		RX2_DIR		TRISGbits.TRISG7
#define		TX2			PORTGbits.RG6
#define		RX2			PORTGbits.RG7
#define		TX3_DIR		TRISGbits.TRISG9
#define		RX3_DIR		TRISGbits.TRISG8
#define		TX3			PORTGbits.RG9
#define		RX3			PORTGbits.RG8
#define		TX4_DIR		TRISBbits.TRISB1
#define		RX4_DIR		TRISBbits.TRISB0
#define		TX4			PORTBbits.RB1
#define		RX4			PORTBbits.RB0

#define		LCD_TX		TX4
#define		LCD_RX		RX4
#define		LCD_TX_DIR	TX4_DIR
#define		LCD_RX_DIR	RX4_DIR

//电平变化总中断允许
#define     CN_IE       IEC1bits.CNIE  
//CN5端口电话变化中断允许
#define  	KEY_IE     	CNEN1bits.CN5IE    

//Calendar
#define		CALENDAR_IE	IEC1bits.INT1IE	
#define		CALENDAR_IF	IFS1bits.INT1IF

//A/D
#define		AD_IE		IEC0bits.AD1IE
#define		AD_IF		IFS0bits.AD1IF			

//雨量
#define     RAIN_IE     IEC1bits.INT2IE
#define		RAIN_PIN	PORTCbits.RC14
#define		RAIN_DIR	TRISCbits.TRISC14
#define		RAIN_IF		IFS1bits.INT2IF

//GPRS RING
#define     GPRS_Ring       PORTBbits.RB4
#define     GPRS_Ring_DIR   TRISBbits.TRISB4
#define     GPRS_Ring_IE    IEC3bits.INT3IE
#define		GPRS_Ring_IF	IFS3bits.INT3IF
//定时器
#define		T1_IE		IEC0bits.T1IE
#define		T1_IF		IFS0bits.T1IF
#define		T1_IP		IPC0bits.T1IP

#define		T2_IE		IEC0bits.T2IE
#define		T2_IF		IFS0bits.T2IF
#define		T2_ON		T2CONbits.TON

#define		T3_IE		IEC0bits.T3IE
#define		T3_IF		IFS0bits.T3IF
#define		T3_ON		T3CONbits.TON

#define		COM1_IF		IFS0bits.U1RXIF	
#define		COM2_IF		IFS1bits.U2RXIF	

