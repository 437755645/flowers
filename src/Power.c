#include	"header.h"
#include    "h_macro.h"

//=====================================
// Wakeup后，开启电源
//=====================================
void PowerRestore(void)
{
	COM4_EN=0;
	LCD_TX_DIR=IN;			//置输入,防止暂态冲击
	LCD_RX_DIR=IN;
	LCD_TX=1;
	LCD_RX=1;
	PW_NON_on(); 
}

//=====================================
// v10=实际电压x10
//=====================================
BYTE CheckVoltage(BYTE v10)
{

	if((v10<WARNING_VOLTAGE)&&(v10>DEAD_VOLTAGE)){	//电压低,告警
	  return RET_VOLTAGE_WARNING;
  	}	
	else if(v10<=DEAD_VOLTAGE){				//电压太低
	  return RET_VOLTAGE_DEAD;
  	}
	return RET_VOLTAGE_OK;
}

void DspVoltageWarning(BYTE v10,const char *msg)
{
BYTE wline[16];

	lcd_init(); cursor_off();
	lcd_puts(0,L0,"电压警告:               ");     
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
	  DspVoltageWarning(v,"系统将停止工作  ");
	  Delay_s(2);			
	  //关闭耗电设备
	  PW_NOFF_off(); 
//
	  SendAT("at+cfun=0",0x0d); Delay_s(1);
	  PW_NON_off(); 
//
	  blight_off();			
	  LED_STATUS_off();

	  //约5s内重启一次,如果电压恢复,正常运行
	  RCON=0b0000000000000000;
	  ClrWdt();
	  PwFailCount=3;				//最小Watchdog=7ms*128=0.9s, 3*0.9=3s,
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
