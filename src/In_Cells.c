#include 	"header.h"
#include    "h_macro.h"

#define		rain_RESOLUTION			0b00101101	//5e-1
#define		rain_NUM_OF_BYTE		2


//主机
const BYTE Station[64*4]={
0x88,	//00H, 总配置字
0x88,
0x88,	//02H	主机序列号
0x88,
0xE9,	//04H	主机地址
0x03,
0xB5,	//06H	主机名称
0xF7,
0xCA,
0xD4,
0xD7,
0xA8,
0xD3,
0xC3,
0x31,
0x30,
0x30,
0x31,
0x00,
0x00,
0x00,
0x00,
0x0C,	//16H	版本（年月日）
0x21,
0x00,	//18H	虚元数
0x01,	//19H	端口数，built-in端口
0x04,	//1AH	本表长度
0x18,	//1BH	年偏置(取消)
0x0A,	//1CH	CSQ
0x00,	//1DH	通信失败计数
0x00,	//1EH	外存操作失败
0x01,	//1FH	MODEM版本号
0x00,	//20H	注册密码
0x00,
0x00,	//22H	致命错误
0x00,	//23H	U盘输出，天数设定
0x00,	//24H	以下空
0x00,
0x00,
0x4F,	//27H	自报间隔
0x07,	//28H	自报带结构
0x05,	//29H	YN1
0x00,	//2AH	YN2
0x00,	//2BH	以下空
0x00,
0x00,
0x00,
0x00,	//2FH	电源电压
0x1B,	//30H	年
0x82,	//31H	月
0x02,	//32H	日
0x0F,	//33H	时
0x17,	//34H	分
0x19,	//35H	秒
0x13,	//36H	本机号码
0x63,
0x41,
0x09,
0x90,
0x4F,
0x00,	//36H	以下空
0x00,
0x1E,	//CRC_L
0xF2,	//CRC_H, Cell bottom

0xC0,	//40H	开关/通信类型/通道号/备用
0x00,
0xDA,
0x6C,
0x10,
0x95,
0x00,
0x00,
0x40,	//48H	开关/通信类型/通道号/备用
0x00,
0xDA,
0x6C,
0x0D,
0x6E,
0x00,
0x00,
0x18,	//50H	开关/通信类型/通道号/备用
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x18,	//58H	开关/通信类型/通道号/备用
0x00,
0x07,
0x60,
0x2F,
0xFF,
0xFF,
0xFF,
0xA0,	//60H	监视电话
0x01,
0x07,
0x60,
0x2F,
0xFF,
0xFF,
0xFF,
0x00,	//以下空
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x5C,	//CRC_L
0xF3,	//CRC_H
};








//基元
const BYTE C0[64]={
0x00,	//0x00, cfg_word
0x00,
0x80,	//0x02, serial num
0x80,
0x48,	//0x04, C0名称
0x79,
0x64,
0x72,
0x6F,
0x42,
0x61,
0x73,
0x69,
0x63,
0x20,
0x31,
0x2E,
0x30,
0x20,
0x20,
0x80,	//0x14, Production Date
0x80,
0x08,	//0x16, num of cellxs
0x00,	//0x17, num of ports
0x82,	//0x18, setting byte	
0x02,	//0x19, C0's len
0x00,	//0x1a, 
0x00,	//0x1b, belt consisted of
0x00,
0x00,
0x03,	//0x1e, num of setup pages
0x00,	//0x1f, max delay
0x00,	//0x20,以下空白
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x02,	//0x36, YN2
0x00,	//0x37, voltage
0x07,	//0x38, Year
0x08,	//0x39, Month
0x0A,	//0x3a, Day
0x0F,	//0x3b, Hour
0x2F,	//0x3c, Minute
0x19,	//0x3d, Second
0x28,	//0x3e, cx's request bits
0xF8
};



const BYTE Cx[8][64]={
{//========雨量计数========
0x00,	//0x00, 虚元配置字
0x00,
0x01,	//0x02, 虚元序列号
0x01,
0xD3,	//0x04,	虚元中文名称（含单位）
0xEA,
0xC1,
0xBF,
0xBC,
0xC6,
0xCA,
0xFD,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0xB4,
0xCE,
0x08,	//14H	分辨力
0x02,	//15H	数据有效长度
0x00,	//16H	记录间隔
0x50,	//17H	记录间隔单位+最高字节有效比特数
0x01,	//18H	记录输出方式+数据格式
0x00,	//19H-1AH	记录文件入口
0x00,
0x01,	//1BH	元长度
0x00,	//1CH	显示格式
0x00,	//1DH	记录次间隔偏移
0x00,	//1EH	余迹长度（belt内）
0x40,	//1FH-20H	记录文件出口
0x00,
0x00,	//21H	虚元最大延时：由此参数可计算“基元最大延时”
0x00,	//22H 	以下空
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,	//29H	转储Sector起始
0x00,	//2AH	转储Sector出口
0x00,
0x00,
0x00,
0x00,
0x00,
0x14,	//30H	YN1
0x00,	//31H	YN2
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x31,
0xF6,	//3FH	当前值
}
,
{//========5min Rain========
0x00,	//0x00, 虚元配置字
0x00,
0x01,	//0x02, 虚元序列号
0x02,
0xD3,
0xEA,
0xC1,
0xBF,
0x35,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0xBA,
0xC1,
0xC3,
0xD7,
0x2D,	//14H	分辨力
0x02,	//15H	数据有效长度
0x05,	//16H	记录间隔
0x51,	//17H	记录间隔单位+最高字节有效比特数
0x01,	//18H	记录输出方式+数据格式
0x02,	//19H-1AH	记录文件入口
0x00,
0x01,	//1BH	元长度
0x01,	//1CH	显示格式
0x00,	//1DH	记录次间隔偏移
0x32,	//1EH	余迹长度（belt内）
0x07,	//1FH-20H	记录文件出口
0x00,
0x00,	//21H	虚元最大延时：由此参数可计算“基元最大延时”
0x00,	//22H 	以下空
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,	
0x00,	//29H	转储Sector起始
0x02,	//2AH	转储Sector出口
0x00,
0x00,
0x00,
0x00,
0x00,
0x97,	//30H	YN1
0x00,	//31H	YN2
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0xD7,
0xF6,	//3FH	当前值
}
,
{//========Hour Rain========
0x00,	//0x00, 虚元配置字
0x00,
0x01,	//0x02, 虚元序列号
0x03,
0xCA,	//0x04,	虚元中文名称（含单位）
0xB1,
0xD3,
0xEA,
0xC1,
0xBF,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0xBA,
0xC1,
0xC3,
0xD7,
0x2D,	//14H	分辨力
0x02,	//15H	数据有效长度
0x01,	//16H	记录间隔
0x52,	//17H	记录间隔单位+最高字节有效比特数
0x01,	//18H	记录输出方式+数据格式
0x07,	//19H-1AH	记录文件入口
0x00,
0x01,	//1BH	元长度
0x01,	//1CH	显示格式
0x00,	//1DH	记录次间隔偏移
0x00,	//1EH	余迹长度（belt内）
0x0C,	//1FH-20H	记录文件出口
0x00,
0x00,	//21H	虚元最大延时：由此参数可计算“基元最大延时”
0x00,	//22H 	以下空
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,	//29H	转储Sector起始
0x00,	//2AH	转储Sector出口
0x00,
0x00,
0x00,
0x00,
0x00,
0x96,	//30H	YN1
0x00,	//31H	YN2
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0xEC,
0xF5,	//3FH	当前值
}
,
{//========Day Rain========
0x00,	//0x00, 虚元配置字
0x00,
0x01,	//0x02, 虚元序列号
0x04,
0xC8,	//0x04,	虚元中文名称（含单位）
0xD5,
0xD3,
0xEA,
0xC1,
0xBF,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0xBA,
0xC1,
0xC3,
0xD7,
0x2D,	//14H	分辨力
0x02,	//15H	数据有效长度
0x01,	//16H	记录间隔
0x53,	//17H	记录间隔单位+最高字节有效比特数
0x01,	//18H	记录输出方式+数据格式
0x0C,	//19H-1AH	记录文件入口
0x00,
0x01,	//1BH	元长度
0x01,	//1CH	显示格式
0x08,	//1DH	记录次间隔偏移
0x00,	//1EH	余迹长度（belt内）
0x11,	//1FH-20H	记录文件出口
0x00,
0x00,	//21H	虚元最大延时：由此参数可计算“基元最大延时”
0x00,	//22H 	以下空
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,	//29H	转储Sector起始
0x00,	//2AH	转储Sector出口
0x00,
0x00,
0x00,
0x00,
0x00,
0x96,	//30H	YN1
0x00,	//31H	YN2
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0xB6,
0xF5,	//3FH	当前值
}
,
{//========当前水位========
0x00,	//0x00, 虚元配置字
0x00,
0x01,	//0x02, 虚元序列号
0x05,
0xB5,	//0x04,	虚元中文名称（含单位）
0xB1,
0xC7,
0xB0,
0xCB,
0xAE,
0xCE,
0xBB,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0xC3,
0xD7,
0x0E,	//14H	分辨力
0x02,	//15H	数据有效长度
0x00,	//16H	记录间隔
0x50,	//17H	记录间隔单位+最高字节有效比特数
0x00,	//18H	记录输出方式+数据格式
0x00,	//19H-1AH	记录文件入口
0x00,
0x01,	//1BH	元长度
0x02,	//1CH	显示格式
0x00,	//1DH	记录次间隔偏移
0x00,	//1EH	余迹长度（belt内）
0x40,	//1FH-20H	记录文件出口
0x00,
0x00,	//21H	虚元最大延时：由此参数可计算“基元最大延时”
0x00,	//22H 	以下空
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,	//29H	转储Sector起始
0x00,	//2AH	转储Sector出口
0x00,
0x00,
0x00,
0x00,
0x00,
0x16,	//30H	YN1
0x00,	//31H	YN2
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0xB3,
0xF6,	//3FH	当前值
}
,
{//========5-min level========
0x00,	//0x00, 虚元配置字
0x00,
0x01,	//0x02, 虚元序列号
0x06,
0xCB,	//0x04,	虚元中文名称（含单位）
0xAE,
0xCE,
0xBB,
0x35,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0xC3,
0xD7,
0x0E,	//14H	分辨力
0x02,	//15H	数据有效长度
0x05,	//16H	记录间隔
0x51,	//17H	记录间隔单位+最高字节有效比特数
0x00,	//18H	记录输出方式+数据格式
0x11,	//19H-1AH	记录文件入口
0x00,
0x01,	//1BH	元长度
0x02,	//1CH	显示格式
0x00,	//1DH	记录次间隔偏移
0x20,	//1EH	余迹长度（belt内）
0x16,	//1FH-20H	记录文件出口
0x00,
0x00,	//21H	虚元最大延时：由此参数可计算“基元最大延时”
0x00,	//22H 	以下空
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x02,	//29H	转储Sector起始
0x0A,	//2AH	转储Sector出口
0x00,
0x00,
0x00,
0x00,
0x00,
0x97,	//30H	YN1
0x00,	//31H	YN2
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x36,
0xF8,	//3FH	当前值
}
,
{//========hour level========
0x00,	//0x00, 虚元配置字
0x00,
0x01,	//0x02, 虚元序列号
0x07,
0xCA,	//0x04,	虚元中文名称（含单位）
0xB1,
0xCB,
0xAE,
0xCE,
0xBB,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0xC3,
0xD7,
0x0E,	//14H	分辨力
0x02,	//15H	数据有效长度
0x01,	//16H	记录间隔
0x52,	//17H	记录间隔单位+最高字节有效比特数
0x00,	//18H	记录输出方式+数据格式
0x16,	//19H-1AH	记录文件入口
0x00,
0x01,	//1BH	元长度
0x02,	//1CH	显示格式
0x00,	//1DH	记录次间隔偏移
0x00,	//1EH	余迹长度（belt内）
0x1B,	//1FH-20H	记录文件出口
0x00,
0x00,	//21H	虚元最大延时：由此参数可计算“基元最大延时”
0x00,	//22H 	以下空
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,	//29H	转储Sector起始
0x00,	//2AH	转储Sector出口
0x00,
0x00,
0x00,
0x00,
0x00,
0x94,	//30H	YN1
0x00,	//31H	YN2
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x45,
0xF7,	//3FH	当前值
}
,
{//========day level========
0x00,	//0x00, 虚元配置字
0x00,
0x01,	//0x02, 虚元序列号
0x08,
0xC8,	//0x04,	虚元中文名称（含单位）
0xD5,
0xCB,
0xAE,
0xCE,
0xBB,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0x2E,
0xC3,
0xD7,
0x0E,	//14H	分辨力
0x02,	//15H	数据有效长度
0x01,	//16H	记录间隔
0x53,	//17H	记录间隔单位+最高字节有效比特数
0x00,	//18H	记录输出方式+数据格式
0x1B,	//19H-1AH	记录文件入口
0x00,
0x01,	//1BH	元长度
0x02,	//1CH	显示格式
0x08,	//1DH	记录次间隔偏移
0x00,	//1EH	余迹长度（belt内）
0x20,	//1FH-20H	记录文件出口
0x00,
0x00,	//21H	虚元最大延时：由此参数可计算“基元最大延时”
0x00,	//22H 	以下空
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,	//29H	转储Sector起始
0x00,	//2AH	转储Sector出口
0x00,
0x00,
0x00,
0x00,
0x00,
0x94,	//30H	YN1
0x00,	//31H	YN2
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x0F,
0xF7,	//3FH	当前值
}};


/*


//========雨量计数========
	case 1:
	  p2[DATA0+CX_RESOLUTION]=RESOLUTION_ONE;
	  p2[DATA0+CX_VAL_LEN]=rain_NUM_OF_BYTE;
	  p2[DATA0+CX_REC_INTERVAL]=0;
	  p2[DATA0+CX_INTERVAL_UNIT]=0x50;    //CX_VALID_BITS=5
	  p2[DATA0+CX_REC_TYPE]=0x01;         //REC_RELATIVE;
	  p2[DATA0+CX_REC_ENTRY]=0;
	  p2[DATA0+CX_LEN]=0x01;
	  p2[DATA0+CX_DSP_FORMAT]=0x0x00,;       //整数
	  p2[DATA0+CX_DELAY]=0x0x00,;			//

//YesNo Property
//[RECORD:7         INSTANT_COMM:6   BUS_COMM:5        BELT_INCLUDE:4 
//[INTERRUPT_COM:3  DSP_ENABLED:2    
	  i=0b00010110;
	  p2[DATA0+CX_PROPERTY1]=SetBeltInclude(i,iCx);	

//current value	
      wRain=ReadRain();	
	  p2[DATA0+CX_CURRENT_VAL-1]=ShortByteIn_int(wRain,0); 
	  p2[DATA0+CX_CURRENT_VAL]=ShortByteIn_int(wRain,1);
	  break;


//========5min Rain========
	case 2:
	  p2[DATA0+CX_RESOLUTION]=rain_RESOLUTION;
	  p2[DATA0+CX_VAL_LEN]=rain_NUM_OF_BYTE;
	  p2[DATA0+CX_REC_INTERVAL]=5;
	  p2[DATA0+CX_INTERVAL_UNIT]=0x50+UNIT_MINUTE;
	  p2[DATA0+CX_REC_TYPE]=0x01;         //REC_RELATIVE;
	  p2[DATA0+CX_REC_ENTRY]=0;
	  p2[DATA0+CX_LEN]=0x01;
	  p2[DATA0+CX_DSP_FORMAT]=0x01;       //小数点后1位
	  p2[DATA0+CX_DELAY]=0x00;			//

//YesNo Property
//[RECORD:7         INSTANT_COMM:6   BUS_COMM:5        BELT_INCLUDE:4 
//[INTERRUPT_COM:3  DSP_ENABLED:2    
	  i=0b10010110;	
	  p2[DATA0+CX_PROPERTY1]=SetBeltInclude(i,iCx);	

//current value	
      wRain=ReadRain();	
	  p2[DATA0+CX_CURRENT_VAL-1]=ShortByteIn_int(wRain,0); 
	  p2[DATA0+CX_CURRENT_VAL]=ShortByteIn_int(wRain,1);
	  break;


//========Hour Rain========
	case 3:
	  p2[DATA0+CX_RESOLUTION]=rain_RESOLUTION;
	  p2[DATA0+CX_VAL_LEN]=rain_NUM_OF_BYTE;
	  p2[DATA0+CX_REC_INTERVAL]=1;
	  p2[DATA0+CX_INTERVAL_UNIT]=0x50+UNIT_HOUR;
	  p2[DATA0+CX_REC_TYPE]=0x01;         //REC_RELATIVE;
	  p2[DATA0+CX_REC_ENTRY]=0;
	  p2[DATA0+CX_LEN]=0x01;
	  p2[DATA0+CX_DSP_FORMAT]=0x01;       //小数点后1位
	  p2[DATA0+CX_DELAY]=0x00;			//

//YesNo Property
//[RECORD:7         INSTANT_COMM:6   BUS_COMM:5        BELT_INCLUDE:4 
//[INTERRUPT_COM:3  DSP_ENABLED:2    
	  i=0b10010110;	
	  p2[DATA0+CX_PROPERTY1]=SetBeltInclude(i,iCx);	

//current value	
      wRain=ReadRain();	
	  p2[DATA0+CX_CURRENT_VAL-1]=ShortByteIn_int(wRain,0); 
	  p2[DATA0+CX_CURRENT_VAL]=ShortByteIn_int(wRain,1);
	  break;



//========Day Rain========
	case 4:
	  p2[DATA0+CX_RESOLUTION]=rain_RESOLUTION;
	  p2[DATA0+CX_VAL_LEN]=rain_NUM_OF_BYTE;
	  p2[DATA0+CX_REC_INTERVAL]=1;
	  p2[DATA0+CX_INTERVAL_UNIT]=0x50+UNIT_DAY;
	  p2[DATA0+CX_REC_TYPE]=0x01;         //REC_RELATIVE;
	  p2[DATA0+CX_REC_ENTRY]=0;
	  p2[DATA0+CX_LEN]=0x01;
      p2[DATA0+CX_SUB_INTERVAL_OFFSET]=8;
	  p2[DATA0+CX_DSP_FORMAT]=0x01;       //小数点后1位
	  p2[DATA0+CX_DELAY]=0x00;			//

//YesNo Property
//[RECORD:7         INSTANT_COMM:6   BUS_COMM:5        BELT_INCLUDE:4 
//[INTERRUPT_COM:3  DSP_ENABLED:2    
	  i=0b10010110;	
	  p2[DATA0+CX_PROPERTY1]=SetBeltInclude(i,iCx);	

//current value
      wRain=ReadRain();	
	  p2[DATA0+CX_CURRENT_VAL-1]=ShortByteIn_int(wRain,0); 
	  p2[DATA0+CX_CURRENT_VAL]=ShortByteIn_int(wRain,1);
	  break;


#ifdef ENCODER1
//========当前水位========
	case 5:
	  p2[DATA0+CX_RESOLUTION]=current_level1_RESOLUTION;
	  p2[DATA0+CX_VAL_LEN]=2;
	  p2[DATA0+CX_REC_INTERVAL]=current_level1_RECORD_GAP;
	  p2[DATA0+CX_INTERVAL_UNIT]=0x50+current_level1_RECORD_GAP_UNIT;
	  p2[DATA0+CX_REC_TYPE]=0x00;     //unsigned bytes,abstract;
	  p2[DATA0+CX_REC_ENTRY]=current_level1_RECORD_ENTRY;
	  p2[DATA0+CX_LEN]=0x01;
	  p2[DATA0+CX_DSP_FORMAT]=0x00;
	  p2[DATA0+CX_DELAY]=0x00;			//

//YesNo Property
//[RECORD:7         INSTANT_COMM:6   BUS_COMM:5        BELT_INCLUDE:4 
//[INTERRUPT_COM:3  DSP_ENABLED:2    
	  i=0b00010110;					//no record
	  p2[DATA0+CX_PROPERTY1]=SetBeltInclude(i,iCx);	

//current value
	  if(wLevel!=0xffff){	
	    p2[DATA0+CX_CURRENT_VAL-1]=ShortByteIn_int(wLevel,0); 
	    p2[DATA0+CX_CURRENT_VAL]=ShortByteIn_int(wLevel,1);
	  }
	  else{
	    p2[DATA0+CX_CURRENT_VAL-1]=0x7f;
	    p2[DATA0+CX_CURRENT_VAL]=0x7f;
	  }
	  break;


//========水位1:5-min water level========
	case 6:
	  p2[DATA0+CX_RESOLUTION]=level_5min_RESOLUTION;
	  p2[DATA0+CX_VAL_LEN]=2;
	  p2[DATA0+CX_REC_INTERVAL]=5;
	  p2[DATA0+CX_INTERVAL_UNIT]=0x50+UNIT_MINUTE;
	  p2[DATA0+CX_REC_TYPE]=0x00;     //unsigned bytes,abstract;
	  p2[DATA0+CX_REC_ENTRY]=level_5min_RECORD_ENTRY;
	  p2[DATA0+CX_LEN]=0x01;
	  p2[DATA0+CX_DSP_FORMAT]=0x00;
	  p2[DATA0+CX_DELAY]=0x00;			//

//YesNo Property
//[RECORD:7         INSTANT_COMM:6   BUS_COMM:5        BELT_INCLUDE:4 
//[INTERRUPT_COM:3  DSP_ENABLED:2    
	  i=0b10010110;	
	  p2[DATA0+CX_PROPERTY1]=SetBeltInclude(i,iCx);	

//current value
	  if(wLevel!=0xffff){	
	    p2[DATA0+CX_CURRENT_VAL-1]=ShortByteIn_int(wLevel,0); 
	    p2[DATA0+CX_CURRENT_VAL]=ShortByteIn_int(wLevel,1);
	  }
	  else{
	    p2[DATA0+CX_CURRENT_VAL-1]=0x7f;
	    p2[DATA0+CX_CURRENT_VAL]=0x7f;
	  }
	  break;

//========水位1: hour level========
	case 7:
	  p2[DATA0+CX_RESOLUTION]=level_hour_RESOLUTION;
	  p2[DATA0+CX_VAL_LEN]=2;
	  p2[DATA0+CX_REC_INTERVAL]=1;
	  p2[DATA0+CX_INTERVAL_UNIT]=0x50+UNIT_HOUR;
	  p2[DATA0+CX_REC_TYPE]=0x00;     //unsigned bytes,abstract;
	  p2[DATA0+CX_REC_ENTRY]=level_hour_RECORD_ENTRY;
	  p2[DATA0+CX_LEN]=0x01;
	  p2[DATA0+CX_DSP_FORMAT]=0x00;
	  p2[DATA0+CX_DELAY]=0x00;			//

//YesNo Property
//[RECORD:7         INSTANT_COMM:6   BUS_COMM:5        BELT_INCLUDE:4 
//[INTERRUPT_COM:3  DSP_ENABLED:2    
	  i=0b10010110;	
	  p2[DATA0+CX_PROPERTY1]=SetBeltInclude(i,iCx);	

//current value
	  if(wLevel!=0xffff){	
	    p2[DATA0+CX_CURRENT_VAL-1]=ShortByteIn_int(wLevel,0); 
	    p2[DATA0+CX_CURRENT_VAL]=ShortByteIn_int(wLevel,1);
	  }
	  else{
	    p2[DATA0+CX_CURRENT_VAL-1]=0x7f;
	    p2[DATA0+CX_CURRENT_VAL]=0x7f;
	  }
	  break;


//========水位: day level========
	case 8:
	  p2[DATA0+CX_RESOLUTION]=level_day_RESOLUTION;
	  p2[DATA0+CX_VAL_LEN]=2;
	  p2[DATA0+CX_REC_INTERVAL]=1;
	  p2[DATA0+CX_INTERVAL_UNIT]=0x50+UNIT_DAY;
	  p2[DATA0+CX_REC_TYPE]=0x00;     //unsigned bytes,abstract;
	  p2[DATA0+CX_REC_ENTRY]=level_day_RECORD_ENTRY;
	  p2[DATA0+CX_LEN]=0x01;
      p2[DATA0+CX_SUB_INTERVAL_OFFSET]=8;
	  p2[DATA0+CX_DSP_FORMAT]=0x00;
	  p2[DATA0+CX_DELAY]=0x00;			//
    
//YesNo Property
//[RECORD:7         INSTANT_COMM:6   BUS_COMM:5        BELT_INCLUDE:4 
//[INTERRUPT_COM:3  DSP_ENABLED:2    
	  i=0b10010110;	
	  p2[DATA0+CX_PROPERTY1]=SetBeltInclude(i,iCx);	

//current value
	  if(wLevel!=0xffff){	
	    p2[DATA0+CX_CURRENT_VAL-1]=ShortByteIn_int(wLevel,0); 
	    p2[DATA0+CX_CURRENT_VAL]=ShortByteIn_int(wLevel,1);
	  }
	  else{
	    p2[DATA0+CX_CURRENT_VAL-1]=0x7f;
	    p2[DATA0+CX_CURRENT_VAL]=0x7f;
	  }
	  break;
#endif




#ifdef WEATHER_VANTAGE_PRO

//========气象信息========
	case 9:
	  p2[DATA0+CX_RESOLUTION]=RESOLUTION_ONE;
	  p2[DATA0+CX_VAL_LEN]=12;
	  p2[DATA0+CX_REC_INTERVAL]=0;
	  p2[DATA0+CX_INTERVAL_UNIT]=0x00;
	  p2[DATA0+CX_REC_TYPE]=0x50;     	//二进制,abstract;
	  p2[DATA0+CX_REC_ENTRY]=0x00;
	  p2[DATA0+CX_LEN]=0x01;
      p2[DATA0+CX_SUB_INTERVAL_OFFSET]=0;
	  p2[DATA0+CX_DSP_FORMAT]=16;		//hex string
	  p2[DATA0+CX_DELAY]=0x01;			//1s
  
  
//YesNo Property
//[RECORD:7         INSTANT_COMM:6   BUS_COMM:5        BELT_INCLUDE:4 
//[INTERRUPT_COM:3  DSP_ENABLED:2    
	  i=0b00010000;		//不显示
	  p2[DATA0+CX_PROPERTY1]=SetBeltInclude(i,iCx);	

//current value
	  if(wLevel!=0xffff){	
	    p2[DATA0+CX_CURRENT_VAL-1]=ShortByteIn_int(wLevel,0); 
	    p2[DATA0+CX_CURRENT_VAL]=ShortByteIn_int(wLevel,1);
	  }
	  else{
	    p2[DATA0+CX_CURRENT_VAL-1]=0x7f;
	    p2[DATA0+CX_CURRENT_VAL]=0x7f;
	  }			
	  break;

#endif


#ifdef ENCODER2

//========当前水位2========
	case 9:
	  p2[DATA0+CX_RESOLUTION]=current_level1_RESOLUTION;
	  p2[DATA0+CX_VAL_LEN]=2;
	  p2[DATA0+CX_REC_INTERVAL]=current_level1_RECORD_GAP;
	  p2[DATA0+CX_INTERVAL_UNIT]=0x50+current_level1_RECORD_GAP_UNIT;
	  p2[DATA0+CX_REC_TYPE]=0x00;     //unsigned bytes,abstract;
	  p2[DATA0+CX_REC_ENTRY]=current_level1_RECORD_ENTRY;
	  p2[DATA0+CX_LEN]=0x01;
	  p2[DATA0+CX_DSP_FORMAT]=0x00;
	  p2[DATA0+CX_DELAY]=0x00;			//

//YesNo Property
//[RECORD:7         INSTANT_COMM:6   BUS_COMM:5        BELT_INCLUDE:4 
//[INTERRUPT_COM:3  DSP_ENABLED:2    
	  i=0b00010110;					//no record
	  p2[DATA0+CX_PROPERTY1]=SetBeltInclude(i,iCx);	

//current value
	  if(wLevel!=0xffff){	
	    p2[DATA0+CX_CURRENT_VAL-1]=ShortByteIn_int(wLevel,0); 
	    p2[DATA0+CX_CURRENT_VAL]=ShortByteIn_int(wLevel,1);
	  }
	  else{
	    p2[DATA0+CX_CURRENT_VAL-1]=0x7f;
	    p2[DATA0+CX_CURRENT_VAL]=0x7f;
	  }
	  break;


//========水位2:5-min water level========
	case 10:
	  p2[DATA0+CX_RESOLUTION]=level_5min_RESOLUTION;
	  p2[DATA0+CX_VAL_LEN]=2;
	  p2[DATA0+CX_REC_INTERVAL]=5;
	  p2[DATA0+CX_INTERVAL_UNIT]=0x50+UNIT_MINUTE;
	  p2[DATA0+CX_REC_TYPE]=0x00;     //unsigned bytes,abstract;
	  p2[DATA0+CX_REC_ENTRY]=level_5min_RECORD_ENTRY;
	  p2[DATA0+CX_LEN]=0x01;
	  p2[DATA0+CX_DSP_FORMAT]=0x00;
	  p2[DATA0+CX_DELAY]=0x00;			//

//YesNo Property
//[RECORD:7         INSTANT_COMM:6   BUS_COMM:5        BELT_INCLUDE:4 
//[INTERRUPT_COM:3  DSP_ENABLED:2    
	  i=0b10010110;	
	  p2[DATA0+CX_PROPERTY1]=SetBeltInclude(i,iCx);	

//current value
	  if(wLevel!=0xffff){	
	    p2[DATA0+CX_CURRENT_VAL-1]=ShortByteIn_int(wLevel,0); 
	    p2[DATA0+CX_CURRENT_VAL]=ShortByteIn_int(wLevel,1);
	  }
	  else{
	    p2[DATA0+CX_CURRENT_VAL-1]=0x7f;
	    p2[DATA0+CX_CURRENT_VAL]=0x7f;
	  }
	  break;


//========水位2: hour level========
	case 11:
	  p2[DATA0+CX_RESOLUTION]=level_hour_RESOLUTION;
	  p2[DATA0+CX_VAL_LEN]=2;
	  p2[DATA0+CX_REC_INTERVAL]=1;
	  p2[DATA0+CX_INTERVAL_UNIT]=0x50+UNIT_HOUR;
	  p2[DATA0+CX_REC_TYPE]=0x00;     //unsigned bytes,abstract;
	  p2[DATA0+CX_REC_ENTRY]=level_hour_RECORD_ENTRY;
	  p2[DATA0+CX_LEN]=0x01;
	  p2[DATA0+CX_DSP_FORMAT]=0x00;
	  p2[DATA0+CX_DELAY]=0x00;			//

//YesNo Property
//[RECORD:7         INSTANT_COMM:6   BUS_COMM:5        BELT_INCLUDE:4 
//[INTERRUPT_COM:3  DSP_ENABLED:2    
	  i=0b10010110;	
	  p2[DATA0+CX_PROPERTY1]=SetBeltInclude(i,iCx);	

//current value
	  if(wLevel!=0xffff){	
	    p2[DATA0+CX_CURRENT_VAL-1]=ShortByteIn_int(wLevel,0); 
	    p2[DATA0+CX_CURRENT_VAL]=ShortByteIn_int(wLevel,1);
	  }
	  else{
	    p2[DATA0+CX_CURRENT_VAL-1]=0x7f;
	    p2[DATA0+CX_CURRENT_VAL]=0x7f;
	  }
	  break;


//========水位2: day level========
	case 12:
	  p2[DATA0+CX_RESOLUTION]=level_day_RESOLUTION;
	  p2[DATA0+CX_VAL_LEN]=2;
	  p2[DATA0+CX_REC_INTERVAL]=1;
	  p2[DATA0+CX_INTERVAL_UNIT]=0x50+UNIT_DAY;
	  p2[DATA0+CX_REC_TYPE]=0x00;     //unsigned bytes,abstract;
	  p2[DATA0+CX_REC_ENTRY]=level_day_RECORD_ENTRY;
	  p2[DATA0+CX_LEN]=0x01;
      p2[DATA0+CX_SUB_INTERVAL_OFFSET]=8;
	  p2[DATA0+CX_DSP_FORMAT]=0x00;
	  p2[DATA0+CX_DELAY]=0x00;			//
    
//YesNo Property
//[RECORD:7         INSTANT_COMM:6   BUS_COMM:5        BELT_INCLUDE:4 
//[INTERRUPT_COM:3  DSP_ENABLED:2    
	  i=0b10010110;	
	  p2[DATA0+CX_PROPERTY1]=SetBeltInclude(i,iCx);	

//current value
	  if(wLevel!=0xffff){	
	    p2[DATA0+CX_CURRENT_VAL-1]=ShortByteIn_int(wLevel,0); 
	    p2[DATA0+CX_CURRENT_VAL]=ShortByteIn_int(wLevel,1);
	  }
	  else{
	    p2[DATA0+CX_CURRENT_VAL-1]=0x7f;
	    p2[DATA0+CX_CURRENT_VAL]=0x7f;
	  }
	  break;
#endif




#ifdef EVAPORATION_WS
//========蒸发信息(相对于8:00)========
	case 1:
	  p2[DATA0+CX_RESOLUTION]=RESOLUTION_ONE;
	  p2[DATA0+CX_VAL_LEN]=12;
	  p2[DATA0+CX_REC_INTERVAL]=0;
	  p2[DATA0+CX_INTERVAL_UNIT]=0x00;
	  p2[DATA0+CX_REC_TYPE]=0x50;     	//二进制,abstract;
	  p2[DATA0+CX_REC_ENTRY]=0x00;
	  p2[DATA0+CX_LEN]=0x01;
      p2[DATA0+CX_SUB_INTERVAL_OFFSET]=0;
	  p2[DATA0+CX_DSP_FORMAT]=16;		//hex string
	  p2[DATA0+CX_DELAY]=0x01;			//1s
  
  
//YesNo Property
//[RECORD:7         INSTANT_COMM:6   BUS_COMM:5        BELT_INCLUDE:4 
//[INTERRUPT_COM:3  DSP_ENABLED:2    
	  i=0b00010000;		//不显示
	  p2[DATA0+CX_PROPERTY1]=SetBeltInclude(i,iCx);	

//current value
	  if(wLevel!=0xffff){	
	    p2[DATA0+CX_CURRENT_VAL-1]=ShortByteIn_int(wLevel,0); 
	    p2[DATA0+CX_CURRENT_VAL]=ShortByteIn_int(wLevel,1);
	  }
	  else{
	    p2[DATA0+CX_CURRENT_VAL-1]=0x7f;
	    p2[DATA0+CX_CURRENT_VAL]=0x7f;
	  }			
	  break;

#endif


//abort bellow
	  default:
	    return STEP_ABORT;
	}

//CRC below===================================	
	iCRC=GetCRC(&p2[DATA0],lenCELL);
	p2[DATA0+CX_CRC_L]=LoOfInt(iCRC);
	p2[DATA0+CX_CRC_H]=HiOfInt(iCRC);

//Init len($+<header><data><CRC>
//         1   16     64    2
	Tx_Counter=lenHEAD+lenCELL+3;				//79;	
    Msg1.bitSPI_RESPONSE_READY=1;
	return STEP_ONLY_ONE;
}

*/


