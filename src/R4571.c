#include 	"header.h"
#include    "h_macro.h"

#define		R4571_CE			PORTFbits.RF6
#define		R4571_CE_DIR		TRISFbits.TRISF6   //CE

#define		R4571_SDA			PORTFbits.RF2
#define		R4571_SDA_DIR		TRISFbits.TRISF2  //SDA
#define		R4571_SCLK			PORTBbits.RB12
#define		R4571_SCLK_DIR		TRISBbits.TRISB12   //SCLK
//#define		R4571_IRQ			PORTFbits.RF3
//#define		R4571_IRQ_DIR			TRISFbits.TRISF3    //IRQ



//===========================
// Init
//===========================
void R4571_init(void)
{
	R4571_SDA_DIR=0;
	R4571_SCLK_DIR=0;
	R4571_SCLK=1;
	R4571_CE_DIR=0;
	R4571_CE=0;			//高有效
}


//===========================
// 写1个byte
//===========================
void R4571_WriteByte(char output)
{
char BitCount; 

    BitCount=8;
    do{         
      if(output&0x80)R4571_SDA = 1;	// Set Dout to the next bit according to the MSB of data
      else R4571_SDA=0;
      output=output*2;
      delay_WIDTH_CLK_SPI;
   	  R4571_SCLK=0;       		// Clear the SCK pin
	  delay_WIDTH_CLK_SPI;			// delay a mininum time gap
	  R4571_SCLK=1;      			// data out, Set the SCK pin
      delay_WIDTH_CLK_SPI;
    } while(--BitCount);
    delay_WIDTH_CLK_SPI;			// delay a mininum time gap
}

//===========================
// 读1个byte
//===========================
char R4571_ReadByte(void)
{
char BitCount;
char input1;

	BitCount=8;
	input1=0;
    do{ 
	  delay_WIDTH_CLK_SPI;                            
      R4571_SCLK = 0;       		// DO shift out, Clear the SCK pin
  	  delay_WIDTH_CLK_SPI;
	  R4571_SCLK = 1;      		// Set the SCK pin
	  			// delay a mininum time gap
	  input1=input1*2;
      if(R4571_SDA)input1++;
			// delay a mininum time gap
    }while(--BitCount);
	return input1;
}

//===========================
// 写len个数据
//===========================
void R4571_Write_Data(char addr,BYTE *data,int len)
{
	char i;
	R4571_SDA_DIR=0;
	R4571_SCLK_DIR=0;
	R4571_SCLK=1;
	R4571_CE_DIR=0;
	R4571_CE=1;

	delay_WIDTH_CLK_SPI;
	delay_WIDTH_CLK_SPI;
	//写入“写”标志0001
	R4571_SDA=0;   //0
	delay_WIDTH_CLK_SPI;
	R4571_SCLK=0;
	delay_WIDTH_CLK_SPI;
	R4571_SCLK=1;
	delay_WIDTH_CLK_SPI;
	R4571_SDA=0;  //0
	delay_WIDTH_CLK_SPI;
	R4571_SCLK=0;
	delay_WIDTH_CLK_SPI;
	R4571_SCLK=1;
	delay_WIDTH_CLK_SPI;
	R4571_SDA=0;  //0
	delay_WIDTH_CLK_SPI;
	R4571_SCLK=0;
	delay_WIDTH_CLK_SPI;
	R4571_SCLK=1;
	delay_WIDTH_CLK_SPI;
	R4571_SDA=1;  //1
	delay_WIDTH_CLK_SPI;
	R4571_SCLK=0;
	delay_WIDTH_CLK_SPI;
	R4571_SCLK=1;
	delay_WIDTH_CLK_SPI;

	for(i=0;i<4;i++)//写入地址4bit
	{
	if(addr&0x08)
		R4571_SDA=1;
	else
		R4571_SDA=0;
		R4571_SCLK=0;
		delay_WIDTH_CLK_SPI;
		R4571_SCLK=1;
		delay_WIDTH_CLK_SPI;
		addr=addr*2;
	}
	for(i=0;i<len;i++)//写入数据
   {
	  R4571_WriteByte(*data++);	
	}
	R4571_CE=0;
}

//===========================
// 读len个byte
//===========================
void R4571_Read_Data(char addr,BYTE *data,int len)
{
char i,BitCount;

	R4571_SDA_DIR=0;
	R4571_SCLK_DIR=0;
	R4571_SCLK=1;
	R4571_CE_DIR=0;
	R4571_CE=1;

    BitCount=8;

	delay_WIDTH_CLK_SPI;
	delay_WIDTH_CLK_SPI;
	//写入“读取标志”1001
	R4571_SDA=1;   			//1
	delay_WIDTH_CLK_SPI;
	R4571_SCLK=0;
	delay_WIDTH_CLK_SPI;
	R4571_SCLK=1;
	delay_WIDTH_CLK_SPI;

	R4571_SDA=0;  			//0
	delay_WIDTH_CLK_SPI;
	R4571_SCLK=0;
	delay_WIDTH_CLK_SPI;
	R4571_SCLK=1;
	delay_WIDTH_CLK_SPI;

	R4571_SDA=0;  			//0
	delay_WIDTH_CLK_SPI;
	R4571_SCLK=0;
	delay_WIDTH_CLK_SPI;
	R4571_SCLK=1;
	delay_WIDTH_CLK_SPI;

	R4571_SDA=1;  			//1
	delay_WIDTH_CLK_SPI;
	R4571_SCLK=0;
	delay_WIDTH_CLK_SPI;
	R4571_SCLK=1;
	delay_WIDTH_CLK_SPI;

	for(i=0;i<4;i++){		//写入地址4bit
	  if(addr&0x08)
		R4571_SDA=1;
	  else
		R4571_SDA=0;
    	addr=addr*2;
		R4571_SCLK=0;
		delay_WIDTH_CLK_SPI;
		R4571_SCLK=1;
		delay_WIDTH_CLK_SPI;
	}
	R4571_SDA_DIR=1;delay_WIDTH_CLK_SPI;
	delay_WIDTH_CLK_SPI;
	delay_WIDTH_CLK_SPI;
	delay_WIDTH_CLK_SPI;	//SDA从输出转为输入
	
	for(i=0;i<len;i++){
	  *data++=R4571_ReadByte();
	}
	R4571_CE=0;
}
