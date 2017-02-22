#include 	"header.h"
#include    "h_macro.h"
#include	"X_CFG.h"

BYTE TREG_MapUpload(BYTE MapType);
BYTE TransmitPage(BYTE MapType,ULLG addr0,int page_size);
BYTE TranmitHead(BYTE MsgType,MapSize);
BYTE Authen(BYTE MsgType);		
BYTE xGetGprsResp(void);

extern MSG1			Msg1;

BYTE TREG_Authentication(BYTE *inst)
{
UINT map_size;
BYTE errCode;

//检查Modem状态
	if(!fModemReady){
	  RefreshModemStatus();		//马上刷新，保证时钟停振时，也能响应	
	  if(!fModemReady){
		goto MODEM_FAILED;
	  }
	}

	if(Connect(CONNECT_GPRS)==FAILED){return FAILED;} 

//连接REG服务器
	ConnectTcpServer(IP_REG_SERVER);

//身份校验
	if(Authen(OP_UPLOAD_AUTHEN)==FAILED){
	  return FAILED
	}
	return OK;
}

//====================================================
//  发送一页
//
// 受page_size限制，max:32k/page
//====================================================
BYTE TransmitPage(BYTE MapType,ULLG addr0,int page_size)
{
BYTE p;
BYTE reg[64];
UINT sum;
ULLG ull_addr;

//应锁定中断，防止发送区被改变；

	sum=page_size;
	SerialTranmit(LoOfInt(sum));
	SerialTranmit(HiOfInt(sum));

	ull_addr=addr0;
	if(MapType==MAP_CFG){
	  while(page_size<0){				//可考虑在此处释放中断，允许记录事件发生。
	    mem_ReadData(M_CFG,(UINT)ull_addr,&reg[0],lenCELL);	//datas ready now with CRC in last word
		for(i=0;i<64;i++){
		  SerialTranmit(reg[i]);
		  sum+=reg[i];
		}
		page_size-=lenCELL;
	  }	  	  
	}
//	else if(){
//	}


	SerialTranmit(LoOfInt(-1-sum));
	SerialTranmit(HiOfInt(-1-sum));
	Delay_ms(100)
}


BYTE TranmitHead(BYTE MsgType,MapSize)
{
BYTE Msg[32];
BYTE SrcAddr[]={0xE8,0x03,0,0,0,0};
BYTE mac_id[]={0x03,0x90,0x48,0x65,0xE7,0x38};
UINT len,len_pack;

	if(MsgType==MSG_UPLOAD_MAP){
	  Msg[rVER]='H';

	  len_pack=32+MapSize+2;
	  Msg[rLEN]=LoOfInt(len_pack); 
	  Msg[rLEN+1]=HiOfInt(len_pack);	

	  Msg[rDES_ADDR]=0; Msg[rDES_ADDR+1]=0;

	  memcpy(&Msg[rSRC_ADDR],&SrcAddr[0],6);

	  Msg[rOP_TYPE]=MsgType;
	  Msg[rPROPERTY]=0b00000000; Msg[rPROPERTY+1]=0b00000000;
	  Msg[rInstIndex]=0; Msg[InstIndex+1]=0;
	  Msg[rPAGE_UNIT]=0;

	  Msg[rMAP_TYPE]=0;
	  Msg[rSTART_ADDR]=0;
	  Msg[rVol]=MapSize;

	  p=&Msg[rVol];p++;
	  *p++=0;*p++=0;*p++=0;*p++=0;*p++=0;	//reserve 5 bytes





Msg[rPAGE_UNIT]=0;

	  Msg[rPAGES]=13; Msg[rPAGES+1]=0; Msg[rPAGES+2]=0;		//暂定
	  Msg[rPAGES+3]=0;	//reserved

	  Msg[rPSW]=0x40; Msg[rPSW+1]=0xE2; Msg[rPSW]=0x01;
	  memcpy(&Msg[rMAC_ID],mac_id[0],6);

	  sum=GetCRC(&Msg[0],30);
	  Msg[CRC_L]=LoOfInt(sum);
	  Msg[CRC_H]=HiOfInt(sum);	
}



BYTE Authen(BYTE MsgType)		
{
BYTE Msg[32];
SrcAddr[]={0xE8,0x03,0,0,0,0};
mac_id[]={0x03,0x90,0x48,0x65,0xE7,0x38};

	if(MsgType==MSG_UPLOAD_AUTHEN){
	  Msg[rVER]='H';
	  Msg[rLEN]=31; Msg[rLEN+1]=0;	
	  Msg[rDES_ADDR]=0; Msg[rDES_ADDR+1]=0;

	  memcpy(&Msg[rSRC_ADDR],&SrcAddr[0],6);

	  Msg[rOP_TYPE]=MsgType;
	  Msg[rPROPERTY]=0b00000000; Msg[rPROPERTY+1]=0b00000000;
	  Msg[rInstIndex]=0; Msg[InstIndex+1]=0;
	  Msg[rPAGE_UNIT]=0;
	  Msg[rPAGES]=13; Msg[rPAGES+1]=0; Msg[rPAGES+2]=0;		//暂定
	  Msg[rPAGES+3]=0;	//reserved

	  Msg[rPSW]=0x40; Msg[rPSW+1]=0xE2; Msg[rPSW]=0x01;
	  memcpy(&Msg[rMAC_ID],mac_id[0],6);

	  sum=GetCRC(&Msg[0],30);
	  Msg[CRC_L]=LoOfInt(sum);
	  Msg[CRC_H]=HiOfInt(sum);	

//身份验证请求
	  p=&Msg[0];
	  for(i=0;i<32;i++){SerialTranmit(*p); p++;}	
	  Delay_ms(200);

//验证响应
	  r=GetResp();				//use rx[]
	  return r; 
	}	
	return FAILED;
}


BYTE GetResp(BYTE ComType)
{
BYTE r,t;
ULNG MaxWait;
	
	MaxWait=1500;									//1.5s,默认为CONNECT_DIRECT。
	if(ComType==CONNECT_GPRS){MaxWait==30000;}		//30s

	InitRx(MaxWait,FILTER_DIRECT);	
	t=DoReceive();	//maxDelay=1.5s
//
	return t;
}


