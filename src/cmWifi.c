#include 	"header.h"
#include    "h_macro.h"


extern BYTE 	Step[];			//¶¨Òå¼û£ºSTEPS_



//===============================================
// ip_port: ffffffff-ffff
//===============================================
char WifiConnection(BYTE *ip_port,BYTE *psw)
{
BYTE err_wifi;
BYTE sPort[10],sIp[20];

	while(1){
	  //A  =>	+ok
	  SendAT("+++",0x0);
	  if(IsStrInStream(0,"a",WITHIN_1S*6)==FALSE){
	    err_wifi=0;
	    break;
	  }

	  SendAT("a",0x0);
	  if(IsStrInStream(0,"+ok",WITHIN_1S*3)==FALSE){
	    err_wifi=-1;
	    break;
	  }

/*	  SendAT("at+e",0x0);
	  if(IsStrInStream(0,"+ok",WITHIN_1S*3)==FALSE){
	    err_wifi=-1;
	    break;
	  }
*/
	  //at+wmode=apsta =>	+ok
	  SendAT("at+wmode=apsta",0x0d);
	  if(IsStrInStream(0,"+ok",WITHIN_1S*2)==FALSE){
	    err_wifi=-2;
	    break;
	  }

/* Skip
at+wscan	+ok=
Ch,SSID,BSSID,Security,Indicator
1,ZJSW_GUEST,3C:E5:A6:61:B4:40,WPA2PSK/AES,76
1,ZJSW,3C:E5:A6:61:B4:41,OPEN/NONE,74
1,ZJSW_GUEST,C4:CA:D9:27:ED:C0,WPA2PSK/AES,44
1,ZJSW,C4:CA:D9:27:ED:C1,OPEN/NONE,46
1,TP-LINK_qiandan,D0:C7:C0:CC:A0:10,WPAPSKWPA2PSK/AES,0
¡­¡­
*/

/*	  SendAT("at+wscan",0x0d);
	  if(IsStrInStream(0,"+ok",WITHIN_1S*10)==FALSE){
	    err_wifi=-2;
	    break;
	  }

*/

	  //AT+WSSSID=ZJSW  =>	+ok
	  SendAT("at+WSSSID=401",0x0d);
	  if(IsStrInStream(0,"+ok",WITHIN_1S*3)==FALSE){
	    err_wifi=-3;
	    break;
	  }

	  //AT+WSKEY=wpa2psk,aes,psw  =>  +ok
	  SendAT("AT+WSKEY=wpa2psk,aes,",0x0);
	  SendAT((const char *)psw,0x0d);
	  if(IsStrInStream(0,"+ok",WITHIN_1S*5)==FALSE){
	    err_wifi=-4;
	    break;
	  }
	  
	  //AT+NETP=TCP,Client,40071, 115.236.28.50	=> +ok
	  getPort(&sPort[0],ip_port+8);
	  getIp(&sIp[0],ip_port);

	  SendAT("AT+NETP=TCP,Client,",0x0);
	  TransmitStr(&sPort[0]);
	  Serial_Transmit(',');
	  SendAT((const char *)&sIp[0],0x0d);
	  if(IsStrInStream(0,"+ok",WITHIN_1S*10)==FALSE){
	    err_wifi=-5;
	    break;
	  }

	  //at+entm =>	+ok
	  SendAT("at+entm",0x0d);
	  if(IsStrInStream(0,"+ok",WITHIN_1S*3)==FALSE){
	    err_wifi=-9;
	    break;
	  }
	  err_wifi=1;
	  break; 
	}
	return err_wifi;
}

