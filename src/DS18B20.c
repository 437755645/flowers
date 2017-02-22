#include 	"header.h"
#include    "h_macro.h"



#define		PORTADDRESS			PORTDbits.RD8		//PORTAbits.RA1
#define		PORTADDRESS_DIR		TRISDbits.TRISD8 	//DATA I/O

// DS18B20 ROM命令宏定义
#define ReadRom			0x33
#define MatchRom		0x55
#define SearchRom		0xf0
#define AlertSearch		0xec
#define CopyScratchpad		0x48
#define SkipRom			0xcc
#define ReadPower		0xb4

// DS18B20 功能命令宏定义
#define ConvertTemperature	0x44
#define ReadScratchpad		0xbe
#define WriteScratchpad		0x4e
#define RecallE			0xb8

BYTE ResultTemperatureLH;
BYTE ResultTemperatureH;
BYTE ResultTemperatureLL;

unsigned char decimalH[16]={00,06,12,18,25,31,37,43,50,56,62,68,75,81,87,93};
unsigned char decimalL[16]={00,25,50,75,00,25,50,75,00,25,50,75,00,25,50,75};

BYTE MacID[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
             //家族码     序列号                  CRC
             // 1byt       6byt                   1byt
BYTE Temperature[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
                    // byte0 Temperature LSB
                    // byte1 Temperature MSB
					// byte2 TL Rsgister or UserByte1
					// byte3 TL Rsgister or UserByte2
					// byte4 Configuration Rsgister
					// byte5 Reserved 
					// byte6 Reserved 
					// byte7 Reserved  
					// byte8 CRC
// Pause for exactly 'tick' number of ticks = 1us

void tickDelay(int tick) // Implementation is platform specific
{
int i;
for(i=0;i<tick;i++)
{
asm("nop");
}
}

// 'tick' values
int A,B,C,D,E,F,G,H,I,J;

//-----------------------------------------------------------------------------
// Set the 1-Wire timing to 'standard' (standard=1) or 'overdrive' (standard=0).
//
void SetSpeed()
{
        // Adjust tick values depending on speed
      
                // Standard Speed
                A = 6;
                B = 30;
                C = 30;
                D = 10;
                E = 3;
                F = 28;
                G =0 ;
                H =280;
                I = 34;
                J =160;  
        
}

//-----------------------------------------------------------------------------
// Generate a 1-Wire reset, return 1 if no presence detect was found,
// return 0 otherwise.
// (NOTE: Does not handle alarm presence from DS2404/DS1994)
//
int TouchReset(void)
{
        unsigned char result1;
       PORTADDRESS_DIR=0;

        tickDelay(G);
        PORTADDRESS=0; // Drives DQ low
        tickDelay(H);
         PORTADDRESS=1; // Releases the bus
       tickDelay(I);
         PORTADDRESS_DIR=1;   
        
        asm("nop");
       if(PORTADDRESS==0)
        {result1 =0x00; }// Sample for presence pulse from slave
       else
       { result1 =0x01;}
        tickDelay(J); // Complete the reset sequence recovery
        return result1; // Return sample presence pulse result
}

//-----------------------------------------------------------------------------
// Send a 1-Wire write bit. Provide 10us recovery time.
//
void WriteBit(int bit)
{
     PORTADDRESS_DIR=0;
        if (bit)
        {
                // Write '1' bit
                PORTADDRESS=0;  // Drives DQ low
                tickDelay(A);
               PORTADDRESS=1; // Releases the bus
                tickDelay(B); // Complete the time slot and 10us recovery
        }
        else
        {
                // Write '0' bit
                PORTADDRESS=0; // Drives DQ low
                tickDelay(C);
                PORTADDRESS=1;  // Releases the bus
                tickDelay(D);
        }
}

//-----------------------------------------------------------------------------
// Read a bit from the 1-Wire bus and return it. Provide 10us recovery time.
//
int ReadBit(void)
{
        unsigned char result2;
   PORTADDRESS_DIR=0;
        PORTADDRESS=0; // Drives DQ low
        tickDelay(A);
        PORTADDRESS=1;  // Releases the bus 

     tickDelay(E);
     PORTADDRESS_DIR=1;
     asm("nop");

       if(PORTADDRESS==0)
       { result2 =0x00;} // Sample for presence pulse from slave
       else
        {result2 =0x01;}
        tickDelay(F); // Complete the time slot and 10us recovery

        return result2;
}
//-----------------------------------------------------------------------------
// Write 1-Wire data byte
//
void WriteByte(int data)
{
        int loop;

        // Loop to write each bit in the byte, LS-bit first
        for (loop = 0; loop < 8; loop++)
        {
                WriteBit(data & 0x01);

                // shift the data byte for the next bit
                data >>= 1;
        }
}

//-----------------------------------------------------------------------------
// Read 1-Wire data byte and return it
//
BYTE ReadByte(void)
{
int loop;
BYTE result=0x00;

	for(loop=0; loop<8; loop++){
      // shift the result to get it ready for the next bit
      result>>= 1;

      // if result is one, then set MS bit
      if(ReadBit())result|=0x80;
    }
    return result;
}


//-----------------------------------------------------------------------------
// Set all devices on 1-Wire to overdrive speed. Return '1' if at least one
// overdrive capable device is detected.
//
int DS18B20_ReadROM(BYTE *data)
{
      int i;
        // set the speed to 'standard'
        SetSpeed();

        // reset all devices
  if (TouchReset()) // Reset the 1-Wire bus
     return 0; // Return if no devices found

        // standard Read  ROM command
        WriteByte(ReadRom);

     for(i=0;i<8;i++)
     {
       *data++=ReadByte();

	 } 
   
     return 1;
}
int DS18B20_ReadTemp(BYTE *data)
{
      int i;
        // set the speed to 'standard'
        SetSpeed();

        // reset all devices
  if (TouchReset()) // Reset the 1-Wire bus
     return 0; // Return if no devices found

        // standard skip command
       //   standard Convert T command
        WriteByte(SkipRom);
        WriteByte(ConvertTemperature);
    PORTADDRESS=1;
  // set the speed to 'standard'
        SetSpeed();

        // reset all devices
  if (TouchReset()) // Reset the 1-Wire bus
     return 0; // Return if no devices found

        // standard skip command
        //standard Read scratchpad command
        WriteByte(SkipRom);
        WriteByte(ReadScratchpad);
     for(i=0;i<9;i++)
     {
       *data++=ReadByte();

	 }     
     return 1;
}

BYTE CRC8_1BIT(BYTE *ptr, char len)
{
BYTE i;  
BYTE crc=0;  
  	while(len--!=0){  
      for(i=1; i!=0; i*=2){  
        if((crc&1)!=0) {crc/=2; crc^=0x8C;} 
        else crc/=2;  
        if((*ptr&i)!=0) crc^=0x8C;      
      }   
      ptr++;  
    }
	return(crc);  
}



BYTE ReadID(void)
{
if(1==DS18B20_ReadROM(&MacID[0])){ 
  if(MacID[7]==CRC8_1BIT(&MacID[0],7))
  {
   return(1);
  }
  else{return(0);}
 }
else{return(0);}
}


//===========================================
// 读取温度
// 返回：9999，表示读取失败
//===========================================
int ReadTemp(void)
{
BYTE tempH,tempL;
UINT tmp;
float fT;

	if(1==DS18B20_ReadTemp(&Temperature[0]))
 	{ 
  		if(Temperature[8]==CRC8_1BIT(&Temperature[0],8))
  		{   
            tempH=Temperature[1];
            tempL=Temperature[0];
	
			tmp=((UINT)(tempH)<<8)+tempL;
			fT=tmp*0.0625;
			tmp=(fT+100)*10;	
			//+100:保证负温度时，以正整数保存；*10：保留小数点1位的精度

		    return tmp;
         }
         else{return 9999;}
     }
   else{return 9999;}
}

