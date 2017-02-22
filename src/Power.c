#include	"header.h"
#include    "h_macro.h"

//=====================================
// Wakeup�󣬿�����Դ
//=====================================
void PowerRestore(void)
{
	COM4_EN=0;
	LCD_TX_DIR=IN;			//������,��ֹ��̬���
	LCD_RX_DIR=IN;
	LCD_TX=1;
	LCD_RX=1;
	PW_NON_on(); 
}

//=====================================
// v10=ʵ�ʵ�ѹx10
//=====================================
BYTE CheckVoltage(BYTE v10)
{

	if((v10<WARNING_VOLTAGE)&&(v10>DEAD_VOLTAGE)){	//��ѹ��,�澯
	  return RET_VOLTAGE_WARNING;
  	}	
	else if(v10<=DEAD_VOLTAGE){				//��ѹ̫��
	  return RET_VOLTAGE_DEAD;
  	}
	return RET_VOLTAGE_OK;
}

void DspVoltageWarning(BYTE v10,const char *msg)
{
BYTE wline[16];

	lcd_init(); cursor_off();
	lcd_puts(0,L0,"��ѹ����:               ");     
	lcd_puts(0,L1,msg);     

	sprintf(((char *)&wline[2]),"%3.1f",(double)v10*0.1);  
	wline[0]='V'; wline[1]='=';
	lcd_puts(0,L2,(const char *)&wline[0]);
}


void PowerTrap(void)
{
BYTE v,PwFailCount;

#ifdef SIM_ON
	return;
#endif

	v=VoltageNow();		

//$$$$
v=100;

	if(CheckVoltage(v)==RET_VOLTAGE_DEAD){
	  DspVoltageWarning(v,"ϵͳ��ֹͣ����  ");
	  Delay_s(2);			
	  //�رպĵ��豸
	  PW_NOFF_off(); 
//
	  SendAT("at+cfun=0",0x0d); Delay_s(1);
	  PW_NON_off(); 
//
	  blight_off();			
	  LED_STATUS_off();

	  //Լ5s������һ��,�����ѹ�ָ�,��������
	  RCON=0b0000000000000000;
	  ClrWdt();
	  PwFailCount=3;				//��СWatchdog=7ms*128=0.9s, 3*0.9=3s,
	  while(1){
        Nop();Nop(); Sleep(); Nop();Nop();		//typically wakeup every 2's
	    if(isWDTWU()){
		  PwFailCount--;		
		  if(!PwFailCount){Restart(RST_FOR_ERR_PWR_30);}
		  SystemGuard();
		}
	  }
	}
}

int isWDTWU()
{
	if(RCONbits.WDTO){return 1;}
	else{return 0;}
}
