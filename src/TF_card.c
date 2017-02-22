#include 	"header.h"
#include    "h_macro.h"

extern BYTE cfg[];

//=========================================
// 通常用于手动完成的初始化：
// 包括对全部需要转储、即时信息查询的虚元，
//1、建立文件名；
//2、输出cfg；
//3、计算下一个起始记录时间；
//4、写入起始记录时间；
//=========================================
void InitCardFiles(void)
{
char ext[5];
char ShortFn[14],LongFn[30];		//文件名
ULNG StaID;
BYTE iC0,iCx;
UINT c0,cx;

//文件名前半部分
//	GetNodeAddress(&fname[0]);
	StaID=getULNG(&cfg[CM_STA_ID]);		//Address
	sprintf(&ShortFn[0],"%08ld",StaID);

	strcpy(&LongFn[0],"test");

	ext[0]='.';
	for(iC0=0;iC0<C0S;iC0++){	 			//Loop through all C0's
	  c0=getC0(iC0);
	
	  if(!IsC0Enabled(&cfg[c0])){continue;}		//C0使能
	  if(!IsC0Recorded(&cfg[c0])){continue;}	//C0记录使能
//
	  for(iCx=0;iCx<cfg[c0+C0_CXS];iCx++){
		cx=getCX(iC0,iCx);
	    if(IsCxSDRecorded(&cfg[cx])){						//if should be TF recorded

          //文件名扩展名
	      ext[1]=HexLoByte(iC0);	//(dsp[CX_INDEX]);	//虚元号在先,取低位。
	      ext[2]=HexHiByte(iCx);	//dsp[CX_INDEX+1]);	//基元号在后
	      ext[3]=HexLoByte(iCx);	//dsp[CX_INDEX+1]);
		  ext[4]=0x0;
	      strcat(&ShortFn[0],(const char *)&ext[0]);
	      strcat(&LongFn[0],&ShortFn[0]);

		  CreatCxCardFile((BYTE *)&ShortFn[0],(BYTE *)&LongFn[0]);
		  ExportCfg();

//		  ExportStartTime();	//根据当前时间、CX_REC_INTERVAL 和 CX_INTERVAL_UNIT 计算出下一个记录时间
		}
	  }
	}
}

