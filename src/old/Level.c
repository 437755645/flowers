
#include 	"header.h"
#include    "h_macro.h"

extern int w1,w2;
void Nops(BYTE x)
{
BYTE i;
          	for(i=0;i<x;i++){asm("nop");asm("nop");asm("nop");}
}
//=================================
// 格雷码水位计
// w1: 水位1(可选正反码/角度系数)
// w2: 水位2
//=================================
void ReadEncoder()
{
double coe;

//	level=LevelCoder();
	LevelCoder(w1,w2);

//角度系数
	coe=1;//=GetSlopeCo(U_ENCODE_CO);
/*	if((1.0-coe)<=0.001){
	  Nop();
	  return;
	}
*/
//<1
//	w1=(UINT)(coe*(w1));		//angle coeficient multiplied here
//	*w1=(UINT)(level*coe);	//angle coeficient multiplied here

}

//=================================
// 格雷码水位计
// w1: 水位1(可选正反码)
// w2: 水位2
//=================================
void LevelCoder()
{
//BYTE i;
BYTE b1,b0;
//BYTE c1,c0;
UINT u;
//unsigned int w1,w2;

//initialize direction and pin level
	L_CLK=0; L_CLK_DIR=OUT;
	L_PS=1; L_PS_DIR=OUT;
	L_DI_DIR=IN;

//latch
  	L_PS=1;	Nops(200);	//parallel
	L_CLK=1; Nops(200);
	L_CLK=0; Nops(200);

  	L_PS=0;	Nops(200);	//serial
	b1=Shift8bits();
	b0=Shift8bits();
//	c1=Shift8bits();
//	c0=Shift8bits();

/*	b0=0;
	for(i=0;i<8;i++)
	{
	  b0=b0*2;
	  if(L_DI==1){ b0++; }
	  //pulse
	  L_CLK=1; Nops(100);
	  L_CLK=0; Nops(100);
	}
	b1=b0;

	b0=0;
	for(i=0;i<8;i++){
	  b0=b0*2;
	  if(L_DI==1){ b0++; }
	  //pulse
	  L_CLK=1; Nops(100);
	  L_CLK=0; Nops(100);
	}
*/


#ifdef DEBUG_ON
	Nop();
  	L_PS=1;	Nops(200);	//parallel
#endif
/*

//正反码
	Nop();
	if(!ReadEncoderPolarity()){	//0: not-operation
	  b1=~b1; b0=~b0;
	}*/

//格雷码转换二进制
	b1=b1&0xf;
	u=((UINT)(b1)<<8)+b0;
	GracodeToBinary(u,12);
//w1=
//	c1=c1&0xf;
//	u=((UINT)(c1)<<8)+c0;
//	w2=GracodeToBinary(u,12);
}


BYTE Shift8bits(void)
{
char i;
BYTE b0;
	b0=0;
	for(i=0;i<8;i++){
	  b0=b0*2;
	  if(L_DI==1){ b0++; }
	  //pulse
	  L_CLK=1; Nops(200);
	  L_CLK=0; Nops(200);
	}
	return b0;
}	



//=================
// 格雷码转换二进制
//=================
// for WFH-2 compatible, g={0:0xfff}
void GracodeToBinary(UINT g,BYTE n)
{
signed char i;
UINT r;

	r=0;
	if(g&(((UINT)(1))<<11)){r++;}	
//
	for(i=n-2;i>=0;i--){
	  if((r&0x01)^((g>>i)&0x01)){r=r*2+1;}	
	  else{r=r*2;}
	}
w1=r;
//	return r;
}



