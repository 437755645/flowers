#include 	"header.h"
#include    "h_macro.h"

#define		PROTOCOL_YR_STANDARD	1
#define		PROTOCOL_NIHAO			2


extern BYTE 	IPL_CPU;
extern BYTE		cfg[];
extern BYTE 	c0Table[];		//,cxTable[];
extern PORT		Port[];			//支持最多8个基元(C0)
extern MSG2		Msg2;
extern volatile	UINT 	ms_click;

//=========================================
//                    读端口
//
// 结果存入des
//=========================================
BYTE ReadPort(BYTE iC0,BYTE *des,BYTE speed_mode)
{
BYTE r;
BYTE des_limit;
BYTE belt_len,*belt0;
BYTE port_com;	//端口通信方式
UINT ipl_save;
UINT c0;

	di(ipl_save,IPL_T2_INT-1);	//允许T2_INT中断
	//SET_CPU_IPL(IPL_T2_INT-1);	//允许T2_INT中断
//	SetIPL(INT_CPU,IPL_T2_INT-1);	//允许T2_INT中断

	ClrWdt();

//Alive ?
	c0=getC0(iC0);		//&cfg[((UINT)c0Table[iC0])*lenCELL];			//基元首地址
	if(IsC0Alive(c0,&Port[iC0])){
	  r=OK; goto END_READ_PORT;						//不刷新
	}		

	des_limit=cfg[c0+C0_BELT_LEN];

//基元端口通信方式
	port_com=1;			//C0_PTCM(iC0);		

//本地端口（onboard）
	if(port_com==PTCM_BUILT_IN){		//	  read belt in mainboard
	  r=BeltBeRead(iC0,des);
	  r=SS_OK;
	}

//串行端口
	else if((port_com==PTCM_RS232)||(port_com==PTCM_RS485)){
	  r=ReadSerialPort23(iC0,des,des_limit,speed_mode);
	}
	else{r=FAILED;}

//	PW_NOFF_off();

	belt_len=cfg[c0+C0_BELT_LEN];
	if(r==SS_OK){
	  belt0=des+DATA0+2+1;	//2:SPlen, 1:fmt_no
	  memcpy(des,belt0,belt_len);
	  fSensorRepRequest=1;
	  r=OK;
	}
	else{
	  ClrBelt(des,belt_len);		//填入错误标志
	  r=FAILED;
	}

END_READ_PORT:
	ei(ipl_save);
	return r;
}


UINT InstWrite(BYTE *inst,BYTE iProtocol)
{
UINT lenInst;
UINT uLen;
hTIME dt;
UINT sum;
//ULNG ulPort;

	switch(iProtocol){
	case PROTOCOL_YR_STANDARD:	//标准协议
	  FillBufWith(inst,16,0x0);	//默认值


	  Now(&dt);
	  uLen=lenHEAD-1;
//	  ulPort=(ULNG)iC0;

	  *(inst+FRM_VER)=INST_HEADER;
	  memcpy(&inst[FRM_LEN],(BYTE *)&uLen,2);

	//inst[FRM_STA_ID]=0;		默认值
	  *(inst+FRM_OP)=OP_READ_BELT;
	//inst[FRM_PROPERTY]=0;
	//inst[FRM_INST_INDEX]=0;
	  *(inst+TSEN_BELT_DT)=dt.year;
	  *(inst+TSEN_BELT_DT+1)=dt.month;
	  *(inst+TSEN_BELT_DT+2)=dt.day;
	  *(inst+TSEN_BELT_DT+3)=dt.hour;
	  *(inst+TSEN_BELT_DT+4)=dt.minute;
	  *(inst+TSEN_BELT_DT+5)=dt.second;
	  sum=GetCRC(inst,lenHEAD-2);
	  *(inst+HCRC_L)=LoOfInt(sum);
	  *(inst+HCRC_H)=HiOfInt(sum);
	
	  lenInst=uLen;
	  break;

	case PROTOCOL_NIHAO:	//NIHAO协议
	  *inst='N';
	  *(inst+1)='I';
	  *(inst+2)=0x0D;
	  lenInst=3;
	  break;
	}
	return lenInst; 
}

//=======================================
// 读RS232、RS485等端口(对应PTCM编号2、3)
//=======================================
BYTE ReadSerialPort23(BYTE iC0,BYTE *des,BYTE des_limit,BYTE speed_mode)
{
BYTE buf[128];
BYTE r,inst[16];
BYTE iProtocol;
UINT lenInst;

	ConnectCell(iC0);
	Delay_ms(10);

    iProtocol=PROTOCOL_YR_STANDARD;
    
	if(iProtocol==PROTOCOL_YR_STANDARD){
	  lenInst=InstWrite(&inst[0],PROTOCOL_YR_STANDARD);
	}
	else if(iProtocol==PROTOCOL_NIHAO){
	  lenInst=InstWrite(&inst[0],PROTOCOL_NIHAO);
	}
	else{return FAILED;}
//
	r=RWSerialSensor(&inst[0],lenInst,&buf[0],des_limit,speed_mode);

	return r;
}

//======================================
// 判断Port是否鲜活
// 
//======================================
BYTE IsC0Alive(UINT c0,PORT *bltC0)
{
UINT alive_time;

	if(IsTimeUp(bltC0->time_tick)==FALSE){return TRUE;}	//if Alive

	alive_time=getUINT(&cfg[c0+C0_ALIVE_MS]);	//更新
	bltC0->time_tick=ms_click+alive_time;
	return FALSE;
}

//======================================
// 置C0死
//======================================
void SetC0Dead(PORT *bltC0)
{
	bltC0->time_tick=ms_click;
}

void KillC0s(void)
{
BYTE iC0;

	for(iC0=0;iC0<C0S;iC0++){
	  SetC0Dead(&Port[iC0]);
	}
}
