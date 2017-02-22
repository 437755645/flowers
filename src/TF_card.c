#include 	"header.h"
#include    "h_macro.h"

extern BYTE cfg[];

//=========================================
// ͨ�������ֶ���ɵĳ�ʼ����
// ������ȫ����Ҫת������ʱ��Ϣ��ѯ����Ԫ��
//1�������ļ�����
//2�����cfg��
//3��������һ����ʼ��¼ʱ�䣻
//4��д����ʼ��¼ʱ�䣻
//=========================================
void InitCardFiles(void)
{
char ext[5];
char ShortFn[14],LongFn[30];		//�ļ���
ULNG StaID;
BYTE iC0,iCx;
UINT c0,cx;

//�ļ���ǰ�벿��
//	GetNodeAddress(&fname[0]);
	StaID=getULNG(&cfg[CM_STA_ID]);		//Address
	sprintf(&ShortFn[0],"%08ld",StaID);

	strcpy(&LongFn[0],"test");

	ext[0]='.';
	for(iC0=0;iC0<C0S;iC0++){	 			//Loop through all C0's
	  c0=getC0(iC0);
	
	  if(!IsC0Enabled(&cfg[c0])){continue;}		//C0ʹ��
	  if(!IsC0Recorded(&cfg[c0])){continue;}	//C0��¼ʹ��
//
	  for(iCx=0;iCx<cfg[c0+C0_CXS];iCx++){
		cx=getCX(iC0,iCx);
	    if(IsCxSDRecorded(&cfg[cx])){						//if should be TF recorded

          //�ļ�����չ��
	      ext[1]=HexLoByte(iC0);	//(dsp[CX_INDEX]);	//��Ԫ������,ȡ��λ��
	      ext[2]=HexHiByte(iCx);	//dsp[CX_INDEX+1]);	//��Ԫ���ں�
	      ext[3]=HexLoByte(iCx);	//dsp[CX_INDEX+1]);
		  ext[4]=0x0;
	      strcat(&ShortFn[0],(const char *)&ext[0]);
	      strcat(&LongFn[0],&ShortFn[0]);

		  CreatCxCardFile((BYTE *)&ShortFn[0],(BYTE *)&LongFn[0]);
		  ExportCfg();

//		  ExportStartTime();	//���ݵ�ǰʱ�䡢CX_REC_INTERVAL �� CX_INTERVAL_UNIT �������һ����¼ʱ��
		}
	  }
	}
}

