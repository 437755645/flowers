//=====================================================
//LCD screen description
#define		LCD_PORT			4				//�˿�4

#define		LCD_CR				0x0a
#define		LCD_LF				0x0d
#define		CR					0x0d
#define		LF					0x0a

#define		LCD_WIDTH			24				//ASCII
#define 	LCD_HEIGHT			4
#define		LINES_LCD_BUF		8

//��ʾ
#define		SCREEN_CX			0
#define		SCREEN_SYS			1
#define		SCREEN_STATUS		2
#define		SCREEN_MENU			3

#define		SCREEN_PSW			10

typedef struct {
	BYTE bufLCD[LINES_LCD_BUF][LCD_WIDTH+1];
	BYTE ic0;
	BYTE icx;
	BYTE curL;		//����ָ��
	BYTE endL;		//���������
	BYTE CurX;		//���������
	BYTE CurY;		//���������
	BYTE CurChar;	//����ַ�
}SCREEN;
//=====================================================
