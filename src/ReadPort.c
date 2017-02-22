#include 	"header.h"
#include    "h_macro.h"

#define		PROTOCOL_YR_STANDARD	1
#define		PROTOCOL_NIHAO			2


extern BYTE 	IPL_CPU;
extern BYTE		cfg[];
extern BYTE 	c0Table[];		//,cxTable[];
extern PORT		Port[];			//֧�����8����Ԫ(C0)
extern MSG2		Msg2;
extern volatile	UINT 	ms_click;

//=========================================
//                    ���˿�
//
// �������des
//=========================================
BYTE ReadPort(BYTE iC0,BYTE *des,BYTE speed_mode)
{
BYTE r;
BYTE des_limit;
BYTE belt_len,*belt0;
BYTE port_com;	//�˿�ͨ�ŷ�ʽ
UINT ipl_save;
UINT c0;

	di(ipl_save,IPL_T2_INT-1);	//����T2_INT�ж�
	//SET_CPU_IPL(IPL_T2_INT-1);	//����T2_INT�ж�
//	SetIPL(INT_CPU,IPL_T2_INT-1);	//����T2_INT�ж�

	ClrWdt();

//Alive ?
	c0=getC0(iC0);		//&cfg[((UINT)c0Table[iC0])*lenCELL];			//��Ԫ�׵�ַ
	if(IsC0Alive(c0,&Port[iC0])){
	  r=OK; goto END_READ_PORT;						//��ˢ��
	}		

	des_limit=cfg[c0+C0_BELT_LEN];

//��Ԫ�˿�ͨ�ŷ�ʽ
	port_com=1;			//C0_PTCM(iC0);		

//���ض˿ڣ�onboard��
	if(port_com==PTCM_BUILT_IN){		//	  read belt in mainboard
	  r=BeltBeRead(iC0,des);
	  r=SS_OK;
	}

//���ж˿�
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
	  ClrBelt(des,belt_len);		//��������־
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
	case PROTOCOL_YR_STANDARD:	//��׼Э��
	  FillBufWith(inst,16,0x0);	//Ĭ��ֵ


	  Now(&dt);
	  uLen=lenHEAD-1;
//	  ulPort=(ULNG)iC0;

	  *(inst+FRM_VER)=INST_HEADER;
	  memcpy(&inst[FRM_LEN],(BYTE *)&uLen,2);

	//inst[FRM_STA_ID]=0;		Ĭ��ֵ
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

	case PROTOCOL_NIHAO:	//NIHAOЭ��
	  *inst='N';
	  *(inst+1)='I';
	  *(inst+2)=0x0D;
	  lenInst=3;
	  break;
	}
	return lenInst; 
}

//=======================================
// ��RS232��RS485�ȶ˿�(��ӦPTCM���2��3)
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
// �ж�Port�Ƿ��ʻ�
// 
//======================================
BYTE IsC0Alive(UINT c0,PORT *bltC0)
{
UINT alive_time;

	if(IsTimeUp(bltC0->time_tick)==FALSE){return TRUE;}	//if Alive

	alive_time=getUINT(&cfg[c0+C0_ALIVE_MS]);	//����
	bltC0->time_tick=ms_click+alive_time;
	return FALSE;
}

//======================================
// ��C0��
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
