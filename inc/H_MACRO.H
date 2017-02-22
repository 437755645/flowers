//===============================================
//Macros
//===============================================
#define		inbyte(x,n)		(*(((BYTE *)&(x))+n))


//===============================================
#define		testbit0(var)		((var) & 0x01)
#define		testbit(var,bit)	((var) & (1<<(bit)))

#define		setbit0(var)		((var) |= 0x01)
#define		setbit(var,bit)		((var) |= (1<< (bit)))

#define		clrbit0(var)		((var) &= 0xfe)
#define		clrbit(var,bit)		((var) &= ~(1<< (bit)))


//===============================================
#define 	HiOfInt(x)			((BYTE)((x)>>8))
#define 	LoOfInt(x)			((BYTE)((x) & 0xff))

#define		getUINT(p)			(*(p)+(((UINT)(*((p)+1)))<<8))

//===============================================
//old:			#define		di()	SET_CPU_IPL(7) 			/* disable interrupts */
//2012-10-4:	#define		di()	SET_CPU_IPL(1) 			/* disable interrupts */
//old:			#define		ei()	SET_CPU_IPL(0); __asm__ volatile("disi #0x0000")	/* enable interrupts */
//2012-10-4:	#define		ei()	SET_CPU_IPL(0)			/* enable interrupts */

#define		di(save_to,cpu_ipl)		SET_AND_SAVE_CPU_IPL(save_to, cpu_ipl);	\
									IPL_CPU=cpu_ipl

#define		ei(saved)		RESTORE_CPU_IPL(saved);	\
							IPL_CPU=saved
//enable all int.
#define		ei_all()		SET_CPU_IPL(0);	\
							IPL_CPU=0
//disable all int.
#define		di_all()		SET_CPU_IPL(7);	\
							IPL_CPU=7

#define		BCD2HEX(x)		(((x)>>4)*10+((x) & 0x0f))
#define     HEX2BCD(x)      ((x)+(((x)/10)*6))

//===============================================
//(2)For Asy-serial comm.
//===============================================		

#define	S1_Transmit(x)						\
		while(U1STAbits.UTXBF)continue;		\
		U1TXREG=x;                 		

#define	S2_Transmit(x)						\
		while(U2STAbits.UTXBF)continue;		\
		U2TXREG=x;                      
		

//#define	Serial3_Transmit(x)			
//		while(U3STAbits.UTXBF)continue;	
//		U3TXREG=x;                      


//===============================================
#define		delay_WIDTH_CLK_SPI		delay_us(10)
#define     StatusReset()    while(_POR){_POR=0;}



//===============================================
#define    	NOP    	asm("nop")
//#define		Nop4()	NOP;NOP;NOP;NOP
#define		NOP10	NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;



//===============================================
//For LED control
#define LED_STATUS_toggle()    LED_STATUS_DIR=OUT; LED_STATUS=~LED_STATUS
#define LED_STATUS_on()        LED_STATUS_DIR=OUT; LED_STATUS=0
#define LED_STATUS_off()       LED_STATUS_DIR=OUT; LED_STATUS=1


//===============================================
//for LCD
//#define	PW_LCD_on()		CTL_33V_DIR=OUT; CTL_33V=1
//#define	PW_LCD_off()	CTL_33V_DIR=OUT; CTL_33V=LCD_ALWAYS_ON


//===============================================
//For CTL_PW on-off control
#define	PW_NOFF_on()	CTL_NOFF_DIR=OUT; CTL_NOFF=1
#define	PW_NOFF_off()	CTL_NOFF_DIR=OUT; CTL_NOFF=0
#define	PW_NON_on()		CTL_NON_DIR=OUT; CTL_NON=1
#define	PW_NON_off()	CTL_NON_DIR=OUT; CTL_NON=0
#define	PW_BEIDOU_on()	CTL_BEIDOU_DIR=OUT; CTL_BEIDOU=1
#define	PW_BEIDOU_off()	CTL_BEIDOU_DIR=OUT; CTL_BEIDOU=0

#define PW_SD_on()		CH376_SleepEnable(TRUE)	
#define PW_SD_off()		CH376_SleepEnable(FALSE)	

//===============================================
//for Record
#define Inc(uAddr,top,bottom)		\
	  	uAddr++;					\
	  	if(uAddr==(bottom+1)){uAddr=top;}

#define Dec(uAddr,top,bottom)	 		\
		if(uAddr==top){uAddr=bottom;}	\
	  	else{uAddr--;}

//rec. area: [top, bottom]
#define	AddWith(uAddr,offset,top,bottom)		\
		uAddr=uAddr+offset;						\
		if(uAddr>bottom){uAddr=top+(uAddr-bottom-1);}


//rec. area: [top, bottom]
#define	DecWith(uAddr,offset,top,bottom)		\
		uAddr=uAddr-(offset);						\
		if(uAddr<top){uAddr=bottom-(top-uAddr-1);}



//===============================================
#define	NoKeyPressed()		((Abs(k,KEY_NULL)<=10)&&(btn==FALSE))


//===============================================
#define IsC0Enabled(c0)		testbit(*(c0+C0_YN1),bitC0_ENABLED)
#define IsC0Recorded(c0)	testbit(*(c0+C0_YN1),bitC0_REC_ENABLED)


#define IsCxRecorded(cx)		testbit(*(cx+CX_YN1),bitREC)
#define IsCxInBelt(cx)			testbit(*(cx+CX_YN1),bitIN_BELT)
#define IsCxSDRecorded(cx)		testbit(*(cx+CX_YN1),bitSD_REC)
#define IsCxTFRecorded(cx)		testbit(*(cx+CX_YN2),bitTF_CARD)

#define IsCxCompress(cx)		testbit(*(cx+CX_YN1),bitREC_COMPRESS) 	

//===============================================
//#define	RS232_1_off()		SLEEP1_DIR=OUT; SLEEP1=0;	//go sleep
//#define	RS232_1_on()		SLEEP1_DIR=OUT; SLEEP1=1;	//RS232 wake up

//#define	RS232_2_off()		SLEEP2_DIR=OUT; SLEEP2=0;	//go sleep
//#define	RS232_2_on()		SLEEP2_DIR=OUT; SLEEP2=1;	//RS232 wake up

//#define	RS232_3_off()		SLEEP3_DIR=OUT; SLEEP3=0;	//go sleep
//#define	RS232_3_on()		SLEEP3_DIR=OUT; SLEEP3=1;	//RS232 wake up

//===============================================
#define Trap()	trap=1; while(trap){;}

#define SensorOn()	XRST_DIR=OUT; XRST=0
#define SensorOff() XRST_DIR=OUT; XRST=SENSOR_RESET_PERMITTED

//===============================================
#define PointTo(p,sign)		while(*p!=sign){p++;}


//===============================================
//#define	RS232_off()		SLEEP232_DIR=OUT; SLEEP232=0;	//go sleep
//#define	RS232_on()		SLEEP232_DIR=OUT; SLEEP232=1;	//RS232 wake up

#define	C0S				cfg[CM_C0S]
#define	fLCD_SW			testbit(cfg[CM_YN1],bitLCD_SW)
#define	fSD_CARD		testbit(cfg[CMX_YN],bitSD_CARD)
#define	IsModemReady()	ModemReady[ActiveCom]
