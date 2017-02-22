#include 	"header.h"
#include    "h_macro.h"

extern BYTE 	IPL_CPU;
extern BYTE 	ActiveCom;
extern BYTE		fLcdReady;
extern MSG2		Msg2;
extern SW		sw1;

extern BYTE		cntLCDErr;

const __attribute__((space(auto_psv)))BYTE QR[]={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x3f,0xff,0x87,0xfc,0xff,0xfe,0x00,
0x3f,0xff,0x87,0xfc,0xff,0xfe,0x00,
0x30,0x01,0x86,0x30,0xc0,0x06,0x00,
0x30,0x01,0x86,0x30,0xc0,0x06,0x00,
0x33,0xf9,0x87,0xfc,0xcf,0xe6,0x00,
0x33,0xf9,0x87,0xfc,0xcf,0xe6,0x00,
0x33,0xf9,0x80,0x30,0xcf,0xe6,0x00,
0x33,0xf9,0x80,0x30,0xcf,0xe6,0x00,
0x33,0xf9,0x86,0x30,0xcf,0xe6,0x00,
0x33,0xf9,0x86,0x30,0xcf,0xe6,0x00,
0x33,0xf9,0x86,0x30,0xcf,0xe6,0x00,
0x30,0x01,0x9e,0x00,0xc0,0x06,0x00,
0x30,0x01,0x9e,0x00,0xc0,0x06,0x00,
0x3f,0xff,0x99,0xcc,0xff,0xfe,0x00,
0x3f,0xff,0x99,0xcc,0xff,0xfe,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x33,0x39,0xe0,0x0f,0x30,0x00,0x00,
0x33,0x39,0xe0,0x0f,0x30,0x00,0x00,
0x33,0x00,0x06,0x3f,0x0f,0x98,0x00,
0x33,0x00,0x06,0x3f,0x0f,0x98,0x00,
0x30,0xc7,0xe7,0xcc,0xc1,0xe0,0x00,
0x30,0xc7,0xe7,0xcc,0xc1,0xe0,0x00,
0x30,0xc7,0xe7,0xcc,0xc1,0xe0,0x00,
0x0c,0xc6,0x67,0xf3,0x0e,0x60,0x00,
0x0c,0xc6,0x67,0xf3,0x0e,0x60,0x00,
0x0f,0xc7,0x81,0xcc,0x0f,0x98,0x00,
0x0f,0xc7,0x81,0xcc,0x0f,0x98,0x00,
0x00,0x00,0x00,0x00,0xfe,0x78,0x00,
0x00,0x00,0x00,0x00,0xfe,0x78,0x00,
0x3f,0xff,0x9f,0xf3,0xf1,0xfe,0x00,
0x3f,0xff,0x9f,0xf3,0xf1,0xfe,0x00,
0x30,0x01,0x81,0xcc,0xc1,0xe6,0x00,
0x30,0x01,0x81,0xcc,0xc1,0xe6,0x00,
0x33,0xf9,0x9e,0x0c,0x3e,0x1e,0x00,
0x33,0xf9,0x9e,0x0c,0x3e,0x1e,0x00,
0x33,0xf9,0x9e,0x0c,0x3e,0x1e,0x00,
0x33,0xf9,0x80,0x0c,0x00,0x06,0x00,
0x33,0xf9,0x80,0x0c,0x00,0x06,0x00,
0x33,0xf9,0x9f,0xff,0xff,0xf8,0x00,
0x33,0xf9,0x9f,0xff,0xff,0xf8,0x00,
0x30,0x01,0x81,0xc0,0x30,0x78,0x00,
0x30,0x01,0x81,0xc0,0x30,0x78,0x00,
0x3f,0xff,0x9e,0x3c,0xc0,0x60,0x00,
0x3f,0xff,0x9e,0x3c,0xc0,0x60,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

#ifdef  STOP_WATCH_ON
extern UINT 	stopwatch[];			//stopwatch
extern UINT 	ms_click;
#endif

BYTE cntCC=0;


//#define	PW_LCD_on()		CTL_33V_DIR=OUT; CTL_33V=1
void PW_LCD_on(void)
{
	CTL_33V_DIR=OUT; 
	CTL_33V=1;
	if(fLCD_PW_READY==FALSE){
	  fLCD_PW_READY=TRUE;
	  Delay_ms(600);			//600: ��ʱ̫С�ᵼ������
	}
	lcd_init();					//lcd��ʼ��
}

//#define	PW_LCD_off()	CTL_33V_DIR=OUT; CTL_33V=LCD_ALWAYS_ON
void PW_LCD_off(void)
{
	CTL_33V_DIR=OUT; 
	CTL_33V=LCD_ALWAYS_ON;
	fLCD_PW_READY=FALSE;
}


void lcd_init(void)
{
	if(!swLCD){return;}
	if(fLCD_PW_READY==FALSE){return;}

	LCD_TX_DIR=OUT;			//�ָ����ڷ�������
	LCD_RX_DIR=IN;
	LCD_TX=1;
	LCD_RX=1;

	UART4Init(BAUD_9600);
	RX4_IE=0;        		//�ر�UART1 �������ж�����λ
	IPC22bits.U4RXIP=0;    //�ر�UART1 �������ж����ȼ�λ
}


void lcd_clear(void)//����
{
BYTE save,ipl_save;

	if(!swLCD){return;}
	if(fLCD_PW_READY==FALSE){return;}

	di(ipl_save,IPL_T2_INT-1);		//����T2_INT�ж�	
	save=ActiveCom; ActiveCom=LCD_PORT;

	if(fLcdReady==FALSE){
	  fLcdReady=lcd_stable();
	}

	RX4_IF=0;
	Serial_Transmit(0xf4);
	fLcdReady=lcd_ack();

	ActiveCom=save;
	ei(ipl_save);
//
//	Delay_ms(100);
}


//==============================
void blight_on(void)//���⿪
{
BYTE save,ipl_save;

	if(!swLCD){return;}
	if(fLCD_PW_READY==FALSE){return;}

	di(ipl_save,IPL_T2_INT-1);		//����T2_INT�ж�	
	save=ActiveCom; ActiveCom=LCD_PORT;

	if(fLcdReady==FALSE){
	  fLcdReady=lcd_stable();
	}

	RX4_IF=0;
	Serial_Transmit(0xB5);
	Serial_Transmit(0x01);

	fLcdReady=lcd_ack();
	swLCD_LIGHT=ON;
	ActiveCom=save;
	ei(ipl_save);
}



//==============================
void blight_off(void)//�����
{
BYTE save,ipl_save;

	if(!swLCD){return;}
	if(fLCD_PW_READY==FALSE){return;}

	di(ipl_save,IPL_T2_INT-1);		//����T2_INT�ж�	
	save=ActiveCom; ActiveCom=LCD_PORT;

	if(fLcdReady==FALSE){
	  fLcdReady=lcd_stable();
	}

	RX4_IF=0;
	Serial_Transmit(0xB5);
	Serial_Transmit(LIGHT_ALWAY_ON);	//Serial_Transmit(0x00);

	fLcdReady=lcd_ack();
	swLCD_LIGHT=OFF;

	ActiveCom=save;
	ei(ipl_save);
}


void lcd_puts(char x,char y,const char *str)
{
BYTE save,ipl_save;

//Skip condition
	if(!swLCD){return;}
	if(fLCD_PW_READY==FALSE){return;}
	if(*str=='\0'){return;}

//����
	di(ipl_save,IPL_T2_INT-1);		//����T2_INT�ж�	
	save=ActiveCom; ActiveCom=LCD_PORT;
  	ClrWdt();	

	if(fLcdReady==FALSE){
	  fLcdReady=lcd_stable();
	}

	RX4_IF=0;
	Serial_Transmit(0xe9);
	Serial_Transmit(y);
	Serial_Transmit(x);

	RX4_IF=0;
    while(*str!=0){
	  Serial_Transmit(*str);
 	  str++;
	}
	Serial_Transmit(0x00);

	fLcdReady=lcd_ack();
	ActiveCom=save;
	ei(ipl_save);
}


BYTE lcd_stable(void)
{
BYTE save,ipl_save;

	if(!swLCD){return TRUE;}
	if(fLCD_PW_READY==FALSE){return FALSE;}

	di(ipl_save,IPL_T2_INT-1);		//����T2_INT�ж�	
	save=ActiveCom; ActiveCom=LCD_PORT;

	RX4_IF=0;
	Serial_Transmit(0xBB);				//NOP

	fLcdReady=lcd_ack();
	ActiveCom=save;

	ei(ipl_save);

	return fLcdReady;
}


void cursor_off(void){}



//====================
// ���1s�˳�
//====================
BYTE lcd_ack(void)
{
BYTE save;
unsigned int i;
BYTE laRET;
BYTE c;

#if SIM_ON
	return 1;
#endif

	save=ActiveCom; ActiveCom=LCD_PORT;

	while(!TRMT()){continue;}	//����������ʱ���ᵼ�¶�ʧ��Ӧ???

	RX_IE_OFF();
	ClrUErr();

	i=0;
	while(i<1000){
	  if(URXDA()){		
		c=URXREG();
		if(c==0xCC){
cntCC++;
		  laRET=TRUE;
		  goto LCD_ACK_END;
		}
	  }
	  Nop();
	  delay_us(100);
	  i++;
	}
	laRET=FALSE;
cntLCDErr++;
//	while(1){ClrWdt();}

LCD_ACK_END:
	ActiveCom=save;
	return laRET;
}




/*
//===============================================
// ���³�����δ�õ�
//===============================================
void lcd_sleep(void)//ʡ��ģʽ
{
BYTE save;
	save=ActiveCom; ActiveCom=LCD_PORT;

	RX4_IF=0;
	Serial_Transmit(0xff);
	Serial_Transmit(0x01);
	
lcd_ack();
	ActiveCom=save;
//	Delay_ms(50);
}



void lcd_up(void)//��ʡ��ģʽ��ת������ģʽ
{
BYTE save;
	save=ActiveCom; ActiveCom=LCD_PORT;

	RX4_IF=0;
	Serial_Transmit(0xff);
	Serial_Transmit(0x00);

lcd_ack();
	ActiveCom=save;
//	Delay_ms(50);
}

//==============================
void lcd_soft_reset(void)//��ʡ��ģʽ��ת������ģʽ
{
BYTE save;
	save=ActiveCom; ActiveCom=LCD_PORT;

	RX4_IF=0;
	Serial_Transmit(0xef);
//	Serial_Transmit(0x00);

lcd_ack();
	ActiveCom=save;
//	Delay_ms(50);
}

//==============================
void cursor_on1(unsigned char x,unsigned char y)//��˸����x �ڼ��У�y�ڼ���
{
BYTE save;
	save=ActiveCom; ActiveCom=LCD_PORT;

	x=(8*x)-1;
	if(y==0){y=0x00;}
	if(y==1){y=0x0f;}
	if(y==2){y=0x1f;}
	if(y==3){y=0x2f;}

	RX4_IF=0;
	Serial_Transmit(0xc8);
	Serial_Transmit(0x01);
	Serial_Transmit(y);
	Serial_Transmit(x);
	Serial_Transmit(0x01);
	Serial_Transmit(0x00);
	Serial_Transmit(0x0f);

	lcd_ack();
	ActiveCom=save;
}

void cursor_off1(unsigned char x,unsigned char y)//��˸�رգ�x �ڼ��У�y�ڼ���
{
BYTE save;
	save=ActiveCom; ActiveCom=LCD_PORT;

	x=(8*x)-1;
	if(y==0){y=0x00;}
	if(y==1){y=0x0f;}
	if(y==2){y=0x1f;}
	if(y==3){y=0x2f;}

	RX4_IF=0;
	Serial_Transmit(0xc8);
	Serial_Transmit(0x00);
	Serial_Transmit(y);
	Serial_Transmit(x);
	Serial_Transmit(0x01);
	Serial_Transmit(0x00);
	Serial_Transmit(0x0f);

	lcd_ack();
	ActiveCom=save;
//	Delay_ms(50);
}


void cursor_on(void){}
*/


/*
void lcd_puts_ram(BYTE *str)
{
BYTE save;

	if(!swLCD){return;}

  	ClrWdt();	

	save=ActiveCom; ActiveCom=LCD_PORT;
  
	if(fLcdReady==FALSE){
	  fLcdReady=lcd_stable();
	}

	RX4_IF=0;
    while(*str!=0){
	  Serial_Transmit(*str);
 	  str++;
	}
 	Serial_Transmit(0x00);

	fLcdReady=lcd_ack();
	ActiveCom=save;
}
*/

//==============================
// Set cursor position
//
// y: line,{0:3}
// x: x-position,{0:7} �����ֶ�λ
//=============================
/*
void lcd_cursor(char x,char y)
{
BYTE save;

	if(!swLCD){return;}

  	ClrWdt();	

	save=ActiveCom; ActiveCom=LCD_PORT;
	di(ipl_save,IPL_T2_INT-1);		//����T2_INT�ж�	

	if(fLcdReady==FALSE){
	  fLcdReady=lcd_stable();
	}

	RX4_IF=0;
	Serial_Transmit(0xe9);
	Serial_Transmit(y);
	Serial_Transmit(x);

	ActiveCom=save;
}
*/

//======================================================
// ����ά�룺
// һ�������㣬���ֽھ���
// *str: ��00158653270000000000,00000869524753000000, ...��
//======================================================
void lcd_DrawQRCode(void)		//char R0,char C0,BYTE *str)
{
BYTE save,ipl_save;
BYTE row,col;
UINT i;

//Skip condition
	if(!swLCD){return;}
	if(fLCD_PW_READY==FALSE){return;}
//	if(*str=='\0'){return;}

lcd_init();

//����
	di(ipl_save,IPL_T2_INT-1);		//����T2_INT�ж�	
	save=ActiveCom; ActiveCom=LCD_PORT;
  	ClrWdt();	

	if(fLcdReady==FALSE){
	  fLcdReady=lcd_stable();
	}

	RX4_IF=0;

	row=60;
	col=100;
	i=0;
	while(row>10){
	  Serial_Transmit(0xf3);
	  Serial_Transmit(row);
	  Serial_Transmit(col);
	  Serial_Transmit(0x8);		//8 bits

	  //Serial_Transmit(0xF1);
	  Serial_Transmit(QR[i]);

	  fLcdReady=lcd_ack();
	  if(((i+1)%7)==0){
		row--;
		col=100;
	  }
	  else{
	    col+=8;
	  }
	  i++;
	}

	ActiveCom=save;
	ei(ipl_save);
}
