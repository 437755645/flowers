#include 	"header.h"
#include    "h_macro.h"

//analog keypad                        	//计算值
//#define     KEY_NULL            0x03ff	//3.36V
//#define     KEY1                0x02AA	//2.25V	
//#define     KEY2                0x0158	//1.14V	
//#define     KEY3                0x0020	//0.00V		

//analog keypad                        	//计算值
//#define     KEY_NULL            0x03ff	//3.36V
//#define     KEY1                0x02AA	//2.25V	
//#define     KEY2                0x0158	//1.14V	
//#define     KEY3                0x0020	//0.00V
		

void initAdc1(void)
{
    AD1CON1bits.FORM = 0;   // Data Output Format: Signed Fraction (Q15 format)
    AD1CON1bits.SSRC = 2;   // Sample Clock Source: Timer 3 starts conversion
    AD1CON1bits.ASAM = 1;   // ADC Sample Control: Sampling begins immediately after conversion
		
    AD1CON2bits.SMPI = 15;  // Generate interrupt every 16 sample/conversion
	AD1CON2bits.BUFM = 0;   // Buffer configured as one 16-word buffers
		
	AD1CON3bits.ADRC = 0;   // ADC Clock is derived from Systems Clock
	AD1CON3bits.ADCS = 31;	// ADC Conversion Clock Tad=Tcy*(ADCS+1)=(1/8M)*2*32 = 8us (125Khz)
							// ADC Conversion Time for 10-bit Tc=12*Tab = 96us		
    AD1CHS0bits.CH0SA = 11; // AD1CHS0: A/D Input Select Register
    AD1CHS0bits.CH0NA = 0;	// MUXA +ve input selection (AIN11) for CH0
	  						// MUXA -ve input selection (Vref-) for CH0
	AD1PCFG = 0xFFFF;    	// AD1PCFGH/AD1PCFGL: Port Configuration Register
    AD1PCFGbits.PCFG11 = 0;	// AN11 as Analog Input

	AD1CSSL = 0x0000;    	// Channel Scan is disabled, default state
//    IFS0bits.AD1IF = 0;		// Clear the A/D interrupt flag bit
//    IEC0bits.AD1IE = 1;		// Enable A/D interrupt 
	AD_IF=0;
	AD_IE=0;
   	AD1CON1bits.ADON = 1;	// Turn on the A/D converter	
}




void initAdcKey(void)
{
    AD1CON1bits.FORM = 0;   // Data Output Format: Signed Fraction (Q15 format)
	AD1CON1bits.SSRC = 2;   // Sample Clock Source: Timer 3 starts conversion
    AD1CON1bits.ASAM = 1;   // ADC Sample Control: Sampling begins immediately after conversion
		
    AD1CON2bits.SMPI = 15;  // Generate interrupt every 16 sample/conversion
	AD1CON2bits.BUFM = 0;   // Buffer configured as one 16-word buffers

	AD1CON3bits.ADRC = 0;   // ADC Clock is derived from Systems Clock
	AD1CON3bits.ADCS = 31;	// ADC Conversion Clock Tad=Tcy*(ADCS+1)=(1/8M)*2*32 = 8us (125Khz)
							// ADC Conversion Time for 10-bit Tc=12*Tab = 96us			
	AD1CHS0bits.CH0SA = 2;	// ANA_KEY端口是AN2    // MUXA +ve input selection (AIN2) for CH0
	AD1CHS0bits.CH0NA = 0;	// MUXA -ve input selection (Vref-) for CH0

	AD1PCFG = 0xFFFF;    	// AD1PCFGH/AD1PCFGL: Port Configuration Register
    AD1PCFGbits.PCFG2 = 0;	// AN2 as Analog Input

	AD1CSSL = 0x0000;	    // Channel Scan is disabled, default state
//    IFS0bits.AD1IF = 0;		// Clear the A/D interrupt flag bit
//    IEC0bits.AD1IE = 1;		// Enable A/D interrupt 
	AD_IF=0;
	AD_IE=0;
   	AD1CON1bits.ADON = 1;	// Turn on the A/D converter	
}

/*====================================================================
Timer 3 is setup to time-out every 250 microseconds (4Khz Rate). 
As a result, the module will stop sampling and trigger a conversion on
 every Timer3 time-out, i.e., Ts=250us. At that time, the conversion
 process starts and completes Tc=14*Tad periods later.

When the conversion completes, the module starts sampling again. 
However, since Timer3 is already on and counting, about (Ts-Tc)us
 later, Timer3 will expire again and trigger next conversion. 
====================================================================*/

void initTmr3(void) 
{
    TMR3=0x0000;
  	PR3=PR_TMR3;
    T3_IF=0;
    T3_IE=0;
    T3_ON=1;
}




BYTE VoltageNow(void)
{
BYTE v;	
UINT u;

#ifdef SIM_ON
	return 0;
#endif

    initAdc1();    //配置AD
	initTmr3();
 	_ADON=1;
	
	while(!_DONE)continue;//等待转换完成

	u=ADC1BUF0;
 	_ADON=0;	//shut off A/D
  	T3_ON=0;

	u=u*0.193;	//+6.5;		//u=((x/1024)*3.3*((R1+R2)/R2)+Vdiode)*10,返回=实际电压*10
	v=LoOfInt(u);
	return v;		//  Vdiode=0.65*10;
}

