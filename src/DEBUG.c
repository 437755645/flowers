#include 	"header.h"
#include    "h_macro.h"

extern BYTE 	cfg[];
extern BYTE 	ComStatus;			//通信模式
extern SCREEN 	*ActiveSrn;
extern BYTE 	KeyPollEnabled;
extern volatile KEY 	key;
extern SCREEN 	Srn[];
extern BYTE		fBlinkEnabled;
extern MSG0		Msg0;



void DEBUG(void)
{
#ifdef DEBUG_PGM_ON


//lcd_DrawQRCode();
PW_LCD_off();

//
/*
Scroll(WAY_CRLF," >注册中...");
Scroll(WAY_CRLF," >端码: 67890276-345677");
while(1){;}

//WR_test();

//Inst_Analysis(1);

//SM_process();

ActiveSrn=&Srn[0];
KeyPollEnabled=FALSE;
fSrnRefresh=TRUE;
fKeyReleased=TRUE;
//Editor(10);

  while(1){
	if(fSrnRefresh){
	  LCDShow();
	  fSrnRefresh=FALSE;
	  fBlinkEnabled=TRUE;
	  KeyPollEnabled=TRUE;
	}

	CursorBlink();

	KeyRoutine();
	
  }
*/
#endif
}


