#include "header.h"
#include "h_macro.h"

extern BYTE cfg[];
extern SW	sw1;

/******************************************************************************
this function model is support for the tf  storage
1,created by :heqing 

2,modified by :qiankechong 
   data:            20120307
ÿһ���ն˶�Ӧ����һ���ļ����ó��ļ�����ʾ
���ļ�����ʽΪAddrIDPortsCells.hyd
��Ӧ�Ķ��ļ���Ϊ:AddrID.PortsCells
����AddrID:8b;Ports:1b;Cells:2b
*******************************************************************************/

BYTE		g_uchSDInserFlg = 0; 					//SD�������־λ
BYTE    	g_LongNameBuf[LONG_NAME_BUF_LEN];    	//��ų��ļ���
FILE_NAME   FileNameBuffer[MAX_FILE_COUNT];    		//�ļ����ṹ
UINT		FileCount;
BYTE *      pFileName[MAX_FILE_COUNT];

BOOL        b_IniFlag = FALSE;


/******************************************************************************
����ʵ��:tf����ʼ��ʱ����LCD��Ļ����ʾ�Ľ�����
******************************************************************************/
/*
void CarInitProgress(void )
{
    UINT i;   
    //char msg[16] ;
    char *msg =" " ;
    
    msg=(char*)calloc(10,sizeof(char));
    
    for (i=0;i<100;i++)
    {
          lcd_cursor(0,L0); lcd_puts("tf��[  ]               ");
          sprintf(msg,"%d",100-i);
          PRINTF("msg:%s\r\n",msg );
          lcd_cursor(5,L0); lcd_puts("98"); 
         // lcd_cursor(6,L1); lcd_puts_ram(msg);          
          ClrWdt();
    }
    
}
*/

/******************************************************************************
                ��ʼ��TF���ӿ�
1����ʼ��ģ��SPI:TF����оƬCH376ͨ��I/Oģ��SPI����
2����ʼ��CH376host
******************************************************************************/
BYTE CardInit(void)
{    
    BYTE    s = 0;
    BYTE    reSet = 0;
    
    Init_SPI_ch376();    
    Delay_ms( 100 );  /* ��ʱ100���� */
    PRINTF("\r\nCH376 STDIO init !\r\n" );  
    
reStart:    
    s = mInitCH376Host( );  /* ��ʼ��CH376 */
    if(USB_INT_SUCCESS == s )
        PRINTF( "CH376 init OK!\r\n" );
    else
    {
        PRINTF( "CH376 init FAIL A,0x%x\r\n",s );
        reSet++;
        if( s ==0xfa && reSet < 3)
        {
            CH376_Reset();
            PRINTF("CH376 Reset!!\r\n" );            
            goto reStart;
        }
        else if (reSet>=3)
        {
            reSet = 0;
            PRINTF("Check connect lines,Restart module,try again\r\n" );
        }
    }    
    
   return s;            
}

/******************************************************************************
                ��ʼ��TF���Ƿ���λ��ѯ��
                
******************************************************************************/
BYTE CardMount(void)
{
    BYTE    s = 0;
    BYTE    i = 0;

    ClrWdt();

    if(FALSE == b_IniFlag )
        PRINTF( "Wait Udisk/SD \r\n" );
    s = CH376DiskConnect( );
    
    while ( CH376DiskConnect( ) != USB_INT_SUCCESS )
    { 
    
        /* ���U���Ƿ�����,�ȴ�U�̲���,
        ����SD��,�����ɵ�Ƭ��ֱ�Ӳ�ѯSD�����Ĳ��״̬���� */
        vLedTest_ms(250 );  /* LED��˸ */
        PRINTF(".");
        if(0 == g_uchSDInserFlg) //������ˢ��LCD
        {
        
            //light_off();         //�رձ���
            mStopIfError( s );
        }
        g_uchSDInserFlg = 1;    // ��־λ��1            
    }
    //��SD����κ���Ҫ���ֽ��г�ʼ��
    if( 1 == g_uchSDInserFlg)
    {
    
        g_uchSDInserFlg = 0; // ��־λ��0   
        s = mInitCH376Host( );  /* ��ʼ��CH376 */
        if(USB_INT_SUCCESS == s )
        {
            
           if(FALSE == b_IniFlag )
           PRINTF("CH376 init OK\r\n" );
           ;
        }
        else
        {
            PRINTF("CH376 init FAIL\r\n" );
            
        }
        mStopIfError( s );
        while ( CH376DiskConnect( ) != USB_INT_SUCCESS )
        { 
            /* ���U���Ƿ�����,�ȴ�U�̲���,����SD��,
            �����ɵ�Ƭ��ֱ�Ӳ�ѯSD�����Ĳ��״̬���� */
            PRINTF( "sys err ! pls reset SD module!\r\n" );
            g_uchSDInserFlg = 1;            
        }
    }
    
   // PRINTF( "Found Udisk/SD" );
    /* ��ʱ,��ѡ����,�е�USB�洢����Ҫ��ʮ�������ʱ */
    
    /* ���ڼ�⵽USB/SD�豸��,���ȴ�10*50mS */
    for ( i = 0; i < 10; i ++ ) 
    { 
    
         /* ��ȴ�ʱ��,10*50mS */
        Delay_ms( 50 );    
         
        s =  CH376DiskMount( ) ;
        
        if (  USB_INT_SUCCESS == s) 
        {
        
            if(FALSE == b_IniFlag )
            PRINTF("Let's Go !!!\r\n" );     
            break;  
        }
        else
        {
        
            /* ��ʼ�����̲����Դ����Ƿ���� */
            if(i == 9)
            {
                PRINTF("Disk_Mount_Error!\r\n" );       
                mStopIfError( s ); ;
            }
            
        }   
    }
    
    if(FALSE == b_IniFlag )
    PRINTF( "Disk Mount! \r\n" ); 
    
    return s;   

}

/*
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
*/


/******************************************************************************
// ���Ƹó��ļ�����ANSI�����ȫ·���Ķ��ļ���
//(8+3��ʽ), ���ļ����ɳ��ļ�������
//(ֻҪ����ͻ,��ʵ��㶨)

��ʱ��Ϊ���ļ�������
�Ҷ��ļ������������ڸ�Ŀ¼��"\"
����ļ����ĸ�ʽΪ"/XYYID.TXT"
YY = 00-99,��ʾ��ݵ����2λ
MM = 01-12����ʾΪ�·�
DD = 01-32����ʾΪ����
P = 0-2����ʾΪ�˿ں�
T = 0-9����ʾ���õ�ģʽ
***************************************************************
//������ļ�������չ��[XYYID.��Ԫ����Ԫ��]
\0YY01000.102
\03301000.101
//�ļ���ǰ�벿��,Ϊ�����ID ��
//��Ԫ������,ȡ��λ��
//��Ԫ���ں�
******************************************************************************/
BYTE getShortName(BYTE iC0,BYTE iCx,BYTE *ShortFn)
{        
char chNode[14];     
BYTE ShortName[14] = {0};		//{"\\"};		//{'\'};     
char ext[5];
ULNG ulStaId;

    //�ļ���ǰ�벿��,��Ŀ¼
   
//  bYear = GetByte(M_CFG,MAP_REG_RC_START_TIME+YEAR);
//  sprintf((char *)sYear,"/D%02d",bYear);
//	PRINTF( "sYear:%s\r\n",sYear );
    
//    strcat((char *)&ShortName[0],(const char *)sYear);
    
//  GetNodeAddress(&chNode[0]);
	ulStaId=getULNG(&cfg[CM_STA_ID]);
    sprintf(&chNode[0],"%08ld",ulStaId);

    strcat((char *)&ShortName[0],(const char *)&chNode[0]);
    
    ext[0]='.';
    //������ļ�������չ��
    ext[1]=HexLoByte(iC0);		//��Ԫ������,ȡ��λ��
    ext[2]=HexHiByte(iCx);		//��Ԫ���ں�
    ext[3]=HexLoByte(iCx);		
    ext[4]=0x0;
    
    strcat((char *)&ShortName[0],(const char *)&ext[0]);
    strcpy((char *)ShortFn,(const char *)&ShortName[0]);
    
    return  SUCCESS;
}

/******************************************************************************
1,���ļ���д�ļ�,���ļ���������
2,�������Ĺ���,������������������� 
   ��:"/YYMMDD.TXT\0"
3�����ļ�����Ҫ��һ���ĸ�ʽת��

��ʱ��Ϊ���ļ�������
�Ҷ��ļ������������ڸ�Ŀ¼��"/"
����ļ����ĸ�ʽΪ"/IDPT.hyd"
YY = 00-99,��ʾ��ݵ����2λ
MM = 01-12����ʾΪ�·�
DD = 01-32����ʾΪ����
P = 0-2����ʾΪ�˿ں�
T = 0-9����ʾ���õ�ģʽ
***************************************************************
//���쳤�ļ���,  ��չ��[M��λID.��Ԫ����Ԫ��.hyd]
01000 .102
//�ļ���ǰ�벿��,Ϊ�����ID ��
//��Ԫ������,ȡ��λ��
//��Ԫ���ں�

******************************************************************************/
void getLongName(BYTE *bShortFn,BYTE *bLongfn)
{      
    BYTE    bShortName[14] = {0};    
    BYTE    i,j ; 

    PRINTF("CreatlongFn ,shortFn:%s[%d]\r\n",bShortFn,__LINE__);

    //ɾ�����ļ����е�''.'' ��
    i = j = 0;
    while (bShortFn[i] != '\0'){
        if (bShortFn[i] >='0' && bShortFn[i] <='z')
        {
            bShortName[j] = bShortFn[i];
            i++;
            j++;
        }
        else
            i++;
    }
    bShortName[j] = '\0';
    
    //���ļ�������
    sprintf((char *)bLongfn,"H%s.hyd",bShortName);
}



/******************************************************************************
// ��ȡ��Ԫ����¼ʱ���ֵ
[YYMMDDHHMMSS]        ��¼��ʼʱ�䣬��Ӧ[REC0]
******************************************************************************/
BYTE get_SDCardStartTime(BYTE *sDateTime,BYTE * bShortFn)
{
ULNG offset; 
BYTE s;

    ClrWdt();


    /* ���ļ�,���ļ��ڸ�Ŀ¼�� */    
   	s = CH376FileOpen(bShortFn);
   	if(s!= USB_INT_SUCCESS){            
       PRINTF("Read - Open  file1 ,Error!\r\n" );
       mStopIfError( s );  /* ���ļ�ʱ���� */
       return 0;
	}

	offset=strlen(SD_CFG_HEAD);
	offset+=cfg[CM_LEN_CFG]*(lenCELL+2);		//2:for CRLF
	offset+=strlen(SD_RECODE_HEAD);
   	s=CH376ByteLocate(offset); 
   	if(s!=USB_INT_SUCCESS){            
       mStopIfError(s);  /* ���ļ�ʱ���� */
       return 0;
    }        

    //��ʾ:YYMMDDHHMMSS
   	CardReadRec(offset,SD_STIME_LEN,sDateTime,bShortFn); 
   	delay_us(100);

   	s = CH376FileClose( FALSE );  /* �ر��ļ� */
   	mStopIfError( s );

   	return  USB_INT_SUCCESS ;
}

//******************************************************************************
// ��ȡ��Ԫ����¼ʱ���ֵ
// [YYMMDDHHMMSS]        ��¼��ʼʱ�䣬��Ӧ[REC0]
// ����CFG ����
//******************************************************************************
BYTE set_CardStartTimeInCfg(hTIME dt,BYTE *bShortFn)
{
ULNG	offset; 
BYTE   	s,ret;
BYTE   	sDateTime[15];	//[YYMMDDHHMMSS]
BYTE 	ShortFn[14]={"\\"};

	strcat((char *)&ShortFn[0],(const char *)bShortFn);		//���ϡ�\��

   	ClrWdt();
    
// ���ļ�,���ļ��ڸ�Ŀ¼��     
    s = CH376FileOpen(&ShortFn[0]);		//bShortFn );
    if(s!=USB_INT_SUCCESS){            
        PRINTF("Read - Open  file2 ,Error!\r\n" );
        mStopIfError( s );  /* ���ļ�ʱ���� */
        return 0;
    }

//Skip: CFG & cfg[]
	offset=strlen(SD_CFG_HEAD);
	offset+=cfg[CM_LEN_CFG]*lenCELL*2;		
    s = CH376ByteLocate(offset); 
    if(s!=USB_INT_SUCCESS){            
        mStopIfError(s);  /* ���ļ�ʱ���� */
        return 0;
    }

//��ʾ:"RECODES:" 
    ret=CardWriteStr(offset,(BYTE *)SD_RECODE_HEAD,&ShortFn[0]);
    if(ret==FAILED){
        CALENDAR_IE=FALSE;                    
        return FAILED;
    }
	offset+=strlen(SD_RECODE_HEAD);

//��ʾ:[YYMMDDHHMMSS]
    sprintf((char *)sDateTime ,"[%02d%02d%02d%02d%02d%02d]",dt.year,dt.month,dt.day,dt.hour,dt.minute,dt.second);
    CardWriteStr(offset,sDateTime,&ShortFn[0]);
    
    return  SUCCESS;
}


/******************************************************************************
// ��ȡ��Ԫ��
//CFG ����
******************************************************************************/
ULNG getSDCfgLen(void)
{
ULNG STimeOffset;

    STimeOffset=(ULNG)cfg[CM_LEN_CFG]*(lenCELL*2);
    STimeOffset+=strlen(SD_CFG_HEAD)+strlen(SD_RECODE_HEAD)+SD_STIME_LEN;
    return STimeOffset;
}


/******************************************************************************
// ��ȡ��Ԫ����¼�ļ���
// [M] [վ��] [վ��] [��Ԫ��] [��Ԫ��] . hyd

******************************************************************************/
//BYTE GetCxCardFilename(BYTE *cx_cfg,BYTE *fn)
/*
BYTE GetCxCardFilename(BYTE *fn) //modified by qian 
{
    fn=(BYTE *)calloc(16,sizeof(char));
#if 0
    strcpy((char *)fn,(char *)g_shortName);    
    PRINTF("################################%s:%d##\r\n",__FILE__,__LINE__);
    PRINTF("g_shortName:%s\r\n",g_shortName);
#endif
    return  0 ;
}
*/

/******************************************************************************
�������ļ���
�ڲ�ͬ�Ķ˿��£�ϵͳ�Զ����ݵ�ǰ�˿ڣ�ID��������Ϣ
���ɸ��Եĳ� &���ļ���
ͬʱ���ض��ļ�����Ϊ������д����

2,����ϵͳcfg���ã��Զ����ɶ�Ӧ�ĳ����ļ�����

******************************************************************************/
BYTE CreatCxCardFile(BYTE *bShortFn,BYTE *bLongFn)
{
BYTE    bLongNameBuf[64]; // ��ų��ļ���UNICODE��ʽ
BYTE    s = 0;
BYTE    j = 0;
BYTE 	ShortFn[14]={"\\"};

	strcat((char *)&ShortFn[0],(const char *)bShortFn);		//���ϡ�\��

/* ���Ʋ�ת�����ļ�����LongNameBuf���ļ��������� */
/* ��Ӣ���ַ�ת��Ϊ���ֽڵ�С��UNICODE���� */
    for ( j = 0; bLongFn[j] != 0; j++ ){  
        bLongNameBuf[j*2] = bLongFn[j];  
        bLongNameBuf[j*2+1] = 0x00;
    }
    bLongNameBuf[j*2] = 0x00;  // ĩβ������0��ʾ����
    bLongNameBuf[j*2+1] = 0x00;    

    ClrWdt(); 

    s = CH376CreateLongName(&ShortFn[0], bLongNameBuf);
    Delay_ms(50);        
    if(s==USB_INT_SUCCESS) 
        PRINTF( "Created English Long Name OK!!\r\n" );
    else if(s==ERR_NAME_EXIST) 
    {
        PRINTF( "The short name already exist !\r\n" );
    }
    else 
    {     //����ʧ���˳�
        mStopIfError( s );
    }
        
    return  s ;
}


//=================================
// ��ʼ��������ԪTF��¼
//
//=================================
/*BYTE InitCxCardRec(BYTE *cx_cfg)
{
//BYTE fn[64];
//BYTE r;
#if 0
    GetCxCardFilename(&fn[0]);
    r=CreatCxCardFile();
    if(r==FAILED)
    {
        return FAILED;
    }

//    WriteCfg();
//    WriteStartTime();
#endif
    return 0 ;
}
*/

//===================================
// ��������Ϊ��λ��ʱ���
//===================================
/*
long DeltaToSec(hTIME *curT,hTIME *staT)
{
long delta;

//��ǰʱ��cT��
//    delta=((((((YEA*12+MON)*32+DAY)*24+HOU)*60+MIN)*60+SEC)-
//                ((((((yea*12+mon)*32+day)*24+hou)*60+min)*60+sec)
//Ϊ��ֹ YEA*12 �������ʽ�޸�����?
    if(curT->year>90)
        {curT->year=20; return FAILED;}
    if(curT->month>12)
        {curT->month=1; return FAILED;}
    if(curT->day>31)
        {curT->day=1; return FAILED;}
    if(curT->hour>23)
        {curT->hour=0; return FAILED;}
    if(curT->minute>59)
        {curT->minute=0; return FAILED;}
    if(curT->second>59)
        {curT->second=0; return FAILED;}

    
    if((curT->year<staT->year)||(curT->year>=staT->year+3))
    {
       return FAILED;    
    }

    delta=(long)((curT->year-staT->year)*YEAR_SEC);
    delta+=(long)((curT->month-staT->month)*MON_SEC);
    delta+=(long)((curT->day-staT->day)*DAY_SEC);
    delta+=(long)((curT->hour-staT->hour)*((long)(HOUR_SEC)));
    delta+=(long)((curT->minute-staT->minute)*MIN_SEC);
    delta+=(long)(curT->second-staT->second);

    


    PRINTF( "curT  TIME:" );
    PRINTF("%.2d:%.2d:%.2d" ,curT->year,curT->month,curT->day);
    PRINTF(":%.2d:%.2d:%.2d\r\n" ,curT->hour,curT->minute,curT->second);
    

    PRINTF( "START TIME:");
    PRINTF("%.2d:%.2d:%.2d" ,staT->year,staT->month,staT->day);
    PRINTF(":%.2d:%.2d:%.2d\r\n" ,staT->hour,staT->minute,staT->second);

    
    PRINTF("delta YEA_SEC:%-9ld" ,(long)((curT->year-staT->year)*YEAR_SEC));
    PRINTF("MON_SEC:%-8ld" ,(long)((curT->month-staT->month)*MON_SEC));
    PRINTF("DAY_SEC:%-8ld\r\n" ,(long)((curT->day-staT->day)*DAY_SEC));
    PRINTF("delta HOU_SEC:%-9ld" ,(long)((curT->hour-staT->hour)*((long)(HOUR_SEC))));
    PRINTF("MIN_SEC:%-8ld" ,(long)((curT->minute-staT->minute)*MIN_SEC));
    PRINTF("SEC_SEC:%-8ld\r\n" ,(long)(curT->second-staT->second));

 //   PRINTF( "delta:%ld\r\n",delta );

    return delta;
}
*/

long DeltaToSec(hTIME *curT,hTIME *starT)
{
long cur,start;

	start=hTime2Sec(*starT);
	cur=hTime2Sec(*curT);
	return (cur-start);
}

//====================================================================
//    delta=((((((YEA*12+MON)*32+DAY)*24+HOU)*60+MIN)*60+SEC)-
//                ((((((yea*12+mon)*32+day)*24+hou)*60+min)*60+sec)*/
//====================================================================
long hTime2Sec(hTIME dt)
{
long t;
	t=dt.year*12+dt.month;
	t=(t*31)+dt.day;
	t=(t*24)+dt.hour;
	t=(t*60)+dt.minute;
	t=(t*60)+dt.second;
	return t;
	//return ((((dt.year*12+dt.month)*31+dt.day)*24+dt.hour)*60+dt.minute)*60+dt.second;
}

//****************************************************************
//offset ��1��ʼ������һ���ֽ�offsetΪ1
//offset Ϊ0xFFFFFFFF ��ʾ���ļ�ĩβд��
//
// ���ַ���bufд��bShortFn,λ��Offset
//*****************************************************************/
BYTE CardWriteStr(ULNG offset,BYTE *buff,BYTE *bShortFn)
{
ULNG  	FileSize,Diff; 
BYTE    s;    
BYTE    buf_1[2]="0";      		//���1�ֽ�,read/write buffer   
BYTE    buf_fill[513]="0";   	//���512�ֽ�,read/write buffer   
long    i;   
UINT    j;		//65536 ����¼����
UINT 	len;
    
#ifdef    WATCHDOG_ON
    ClrWdt();
#endif

//�����ж�
    if(offset <0/*||offset > (1<<32)*/){
//        PRINTF( "FAILED!offset :%ld error,must offset>0 && offset <2<<32\r\n", offset );
        return FAILED;
    }

    
//���ļ�,���ļ��ڸ�Ŀ¼��    
    s=CH376FileOpen(bShortFn);
    if (s!=USB_INT_SUCCESS){            
        mStopIfError(s);  /* ���ļ�ʱ���� */
        CALENDAR_IE = FALSE;                    
        return FAILED;
    }
    
    FileSize=CH376GetFileSize();

    //�ж��ļ���ƫ��λ�ã����ļ�����FileSize�ıȽ�
    if(offset<=FileSize){
        s = CH376ByteLocate(offset);  /* �Ƶ��ļ�ָ����λ�� */ 
        mStopIfError( s );
    }


    //�ж��Ƿ����´������ļ����´����ļ�Ĭ��1�ֽڣ�
    //��ʱ����Ҫ���ļ�ָ���Ƶ��ļ���ͷ���������ƶ�ָ�뵽�ļ���β��
    else{   
        if(FileSize==1){
            s=CH376ByteLocate(0);  /* �Ƶ��ļ���ͷ�� */    
            FileSize = 0;
        }
        else{
            s=CH376ByteLocate(0xFFFFFFFF);  /* �Ƶ��ļ���β�� */            
        }    
                
        Diff=offset-FileSize; 
        //��Ҫ����������(ƫ��-��),ÿд��65535(=ONETIME_MAX_RW)���ֽں�����ļ����ȣ�
        //��ֹ��Ϊ̫����;�쳣��ԭ�򣬶�������һ����Ҫ��ͷ������.
        
        //��������ִ�д�룬ʵʱ�����ļ�����
		//Ϊ����߶�д������Ч�ʣ�����һ�ζ�д�ϴ�����ݿ飬���Ϊ65535������Ϊ512�ı�����
        for(j=0;j<=Diff/BUF65536;j++){
            if(Diff>=BUF65536){
                for(i=0;i<(BUF65536/BUF512);i++){
                    sprintf((char *)buf_fill, "%0512d",0);	//���㴦��
                    s=CH376ByteWrite(buf_fill,BUF512,NULL);  
                    if(s!=USB_INT_SUCCESS){
                        mStopIfError(s);
                    }                
                    delay_us(2);             
                    if(4==i%5){
                       ClrWdt();
                    }    
                }
            }
            else{	
                for(i=0;i<(long)(Diff/BUF512);i++){                
                    sprintf((char *)buf_fill,"%0512d",0);	//���㴦��
                    s=CH376ByteWrite(buf_fill,BUF512,NULL);  
                    if(s!=USB_INT_SUCCESS){
                        mStopIfError(s);
                    }                
                    delay_us(2);             
                    if(4 == i%5){
                       ClrWdt();
                    }
                }
                //512���������������
                for(i=0;i<(Diff%BUF512);i++){               
                    sprintf((char *)buf_1,"%01d",0);//���㴦��
                    s=CH376ByteWrite(buf_1,1,NULL);   
                     if(s!=USB_INT_SUCCESS){
                        mStopIfError( s );
                    }
                    delay_us(2); 
                    if(4==i%5){
                       ClrWdt();
                    }
                } 
            }
//          PRINTF( "Write 65536 end \r\n" );
      
            delay_us(2);
            
            //���³���
            CH376ByteWrite(&buf_1[0],0,NULL);  
            //���ֽ�Ϊ��λ���ļ�д������,��Ϊ��0�ֽ�д��,����ֻ���ڸ����ļ��ĳ���,���׶���д�����ݺ�,���������ְ취�����ļ�����
                
            FileSize=CH376GetFileSize();  	//���»�ȡ�ļ���С
            s=CH376ByteLocate(0xFFFFFFFF); 	//�Ƶ��ļ���β��  
            Diff=offset-FileSize; 
        }   
    }
    
//��offsetλ�ÿ�ʼд������buff
	len=strlen((char *)buff);
    s=CH376ByteWrite((BYTE *)buff,len,NULL);        
    mStopIfError(s);

//���³���
	CH376ByteWrite(&buf_1[0],0,NULL);

//CloseFile
    s=CH376FileClose(FALSE);  /* �ر��ļ� */
    mStopIfError( s );

    return SUCCESS;
}

/******************************************************************************
    ��������
    ָ��ƫ�ƣ��ض�����ΪulLen������
    ����ֵ:*des
    ��������:ÿ������ȡ256�ֽ����ݣ�
                           offset-ulen>=1;
                           offset<=filesize
******************************************************************************/
BYTE CardReadRec(ULNG offset,ULNG ulLen,BYTE *des,BYTE *bShortFn)
{
BYTE	s;    
long   	lDiff;                  //offset ��ulLen�Ĳ�ֵ��
ULNG  	i,j ;
ULNG  	ulFileSize =0;
UINT  	uRealCount;

    ClrWdt();
    
//���ļ�,���ļ��ڸ�Ŀ¼��
    s=CH376FileOpen( bShortFn );
    if(s!=USB_INT_SUCCESS){            
        PRINTF( "Read - Open  file4 ,Error!\r\n" );
        mStopIfError( s );  /* ���ļ�ʱ���� */
        return FAILED;
    }
    
    ulFileSize = CH376GetFileSize( );  //׼����ȡ�ܳ���
    if(offset>ulFileSize){
        PRINTF( "Error! the offset is big than the filesize\r\n" );
        return FAILED ; 
    }

    //offset��ulLen�Ĳ�ֵ����ֵС��0��Խ��
    lDiff=(long)(offset-ulLen);
    if(lDiff<0){
        PRINTF( "Error!offset-ulLen:%ld<0,Verify the input data\r\n" ,offset-ulLen);
        return FAILED; 
    }        
    /* �ļ����ڲ����Ѿ�����,�ƶ��ļ�ָ�뵽β���Ա�������� */

    s=CH376ByteLocate( offset-ulLen );  /* �Ƶ��ļ�ָ��λ�� */             
    if(s!=USB_INT_SUCCESS){
        PRINTF( "move file locate,Error! \r\n" );
        mStopIfError( s );
    }                
   
    //PRINTF( "Read the pre %ld chars from %ld of the file:",ulLen,offset );

	//����ļ��Ƚϴ�,һ�ζ�����,�����ٵ���CH376ByteRead������ȡ,�ļ�ָ���Զ�����ƶ�
    while(ulLen){  
        if(ulLen>256){i = 256;}    	//ʣ�����ݽ϶�,���Ƶ��ζ�д�ĳ��Ȳ��ܳ�����������С
        else{i=ulLen;}  			//���ʣ����ֽ���
        
        s=CH376ByteRead(des,i,&uRealCount);  //���ֽ�Ϊ��λ��ȡ���ݿ�,���ζ�д�ĳ��Ȳ��ܳ�����������С,�ڶ��ε���ʱ���Ÿղŵ�`���� */
        mStopIfError(s);
        ulLen-=uRealCount;             
            
        //����,��ȥ��ǰʵ���Ѿ��������ַ���
        for(j=0;j<uRealCount;j++){
            //PRINTF( "%c", des[j] );  /* ��ʾ�������ַ� */  
        }
        des[j]='\0';           	//������
        if(uRealCount<i){  	//ʵ�ʶ������ַ�������Ҫ��������ַ���,˵���Ѿ����ļ��Ľ�β
            //PRINTF( "\r\n" );
            //PRINTF( "The file point arrive  to the  end!\r\n" );
            break;
        }
    }
    
    //PRINTF( ",Close \r\n" );
    s=CH376FileClose(FALSE);  	//�ر��ļ�
    mStopIfError(s);
    
    return OK;
}  

/******************************************************************************
// ͨ�������ֶ���ɵĳ�ʼ����
// ������ȫ����Ҫת������ʱ��Ϣ��ѯ����Ԫ��
//1�������ļ�����
//2�����cfg��
//3��������һ����ʼ��¼ʱ�䣻
//4��д����ʼ��¼ʱ�䣻
******************************************************************************/
void InitCardFiles(void)
{
UINT c0,cx;
BYTE iC0,iCx;
BYTE fsName[14],flName[32];        // "/SSSSSSSS.XYY\0"; 14 bytes
hTIME dt;
BYTE s;

	Now(&dt);

//Loop through all Cx's
	for(iC0=0;iC0<C0S;iC0++){	 			//Loop through all C0's
	  c0=getC0(iC0);
	
	  if(!IsC0Enabled(&cfg[c0])){continue;}		//C0ʹ��
	  if(!IsC0Recorded(&cfg[c0])){continue;}	//C0��¼ʹ��

	  for(iCx=0;iCx<cfg[c0+C0_CXS];iCx++){
		cx=getCX(iC0,iCx);
	    if(IsCxInBelt(&cfg[cx])){						//if in belt
	      if(IsCxSDRecorded(&cfg[cx])){					//if should be recorded

            memset((char*)&flName[0],0, sizeof(char)*32);
            memset((char*)&fsName[0],0, sizeof(char)*14);
                
            getShortName(iC0,iCx,&fsName[0]);
            getLongName(&fsName[0],&flName[0]);
                
            //1�����ݳ����ļ�����������¼�ļ�(���ļ���)
            s=CreatCxCardFile(&fsName[0],&flName[0]);
			if(s==ERR_NAME_EXIST){
				Nop();
			}

            //2������cfg��Ϣ����¼��SD-card��
            s=ExportCfg2SDCard((BYTE *)fsName);
			if(s==ERR_NAME_EXIST){
				Nop();
			}

    		//3�������ļ��ĸ�ʽ1[��¼��ʼʱ���]
            set_CardStartTimeInCfg(dt,(BYTE *)fsName);
		  }
	    }
      }  
  	}

    // lcd_cursor(0,L3); lcd_puts("�����ļ��ɹ�            ");       
    TX1_IE   = FALSE;
    RX2_IE   = TRUE;
    RX1_IE   = TRUE;
}

//========================================================
// ���Recλ��
//========================================================
ULNG getRecPosition(hTIME *this,BYTE iC0,BYTE iCx)	//,BYTE *bShortFn)
{
UINT 	cx;
hTIME   startSD;
long    rGAP; 
ULNG    delta,ulOffset ; 
ULNG    SD_REC0;
//BYTE 	fsName[13];
//BYTE 	ShortFn[14]={"\\"};
BYTE 	lenOneRecord;
BYTE    val_len,unit,interval;

    //1,��ȡcfg ����    
    SD_REC0=getSDCfgLen();

    //���ݵ�ǰʱ�䡢CX_REC_INTERVAL��CX_INTERVAL_UNIT �������һ����¼ʱ��
	cx=getCX(iC0,iCx);
    unit=cfg[cx+CX_REC_GAP]&0x0f;
    interval=cfg[cx+CX_REC_GAP];
    val_len=cfg[cx+CX_VAL_BLEN];    
    rGAP=Gap2Sec(unit,interval);

    //1,��ȡstarttime    
    get_RecordStartTime(&startSD);
     
	//2, ����ʱ���ֵ    
    delta=DeltaToSec(this,&startSD);
	if((delta<0)||(delta>100000)){
	}

	lenOneRecord=(val_len*2+1+6+1)*2;
    
    ulOffset=SD_REC0+(delta/rGAP)*((ULNG)(lenOneRecord));
	return ulOffset;
}



//=========================================================
//
// 1��Belts is ready when calendar() is called previously
//
//=========================================================
char ReadSDRecord(hTIME *this,BYTE *des,BYTE iC0,BYTE iCx,BYTE *bShortFn)
{
UINT 	cx;
ULNG    ulOffset ; 
BYTE 	fsName[13];
BYTE 	ShortFn[14]={"\\"};
BYTE 	lenOneRecord;
BYTE    val_len;		//,unit,interval;

    ClrWdt();    

    //�ر�ʱ���ж�,������¼,��ֹ�ڶ�����ʱ,���ݱ���д.          
    CALENDAR_IE = FALSE; 

//Recλ��
	ulOffset=getRecPosition(this,iC0,iCx);

//��¼����
	cx=getCX(iC0,iCx);
    val_len=cfg[cx+CX_VAL_BLEN];    
	lenOneRecord=(val_len*2+1+6+1)*2;

//�ļ���
	getShortName(iC0,iCx,&fsName[0]);
	strcat((char *)&ShortFn[0],(const char *)&fsName[0]);		//���ϡ�\��

    CardReadRec(ulOffset,lenOneRecord,des,&ShortFn[0]); 
        
    //����ʱ���ж�,�������
    CALENDAR_IE = TRUE;     
    
    return SUCCESS;
}


/******************************************************************************
//Record data begin from belt
// 1. REC_FMT_BEGIN may be overwritten when
//    data cycles back. so when record being
//  read, length should be limitted.
******************************************************************************/
char TFRecord(BYTE *val,hTIME *rTime,BYTE iC0,BYTE iCx)
{
hTIME   startSD;
BYTE	one_record[129];    
BYTE    val_len,unit,interval;
BYTE    ret;
long    rG;
ULNG    SD_REC0;
ULNG    offset,delta;
UINT 	cx;
BYTE 	lenOneRecord;
BYTE 	fsName[13];
BYTE 	ShortFn[14]={"\\"};

	if(swSD_CARD==OFF){return 1;}

    ClrWdt();    

	getShortName(iC0,iCx,&fsName[0]);
	strcat((char *)&ShortFn[0],(const char *)&fsName[0]);		//���ϡ�\��
    
    //��¼��ʼλ��    
    SD_REC0=getSDCfgLen();
    
	//���ݵ�ǰʱ�䡢CX_REC_INTERVAL ��CX_INTERVAL_UNIT �������һ����¼ʱ��
    cx=getCX(iC0,iCx);
	unit=cfg[cx+CX_REC_GAP]>>6;
    interval=cfg[cx+CX_REC_GAP]&0x3f;
    val_len=cfg[cx+CX_VAL_BLEN];    
    rG=Gap2Sec(unit,interval);		//�������secΪ��λ
    
	//1,��ȡ��ʼ��¼ʱ��startSD    
    get_RecordStartTime(&startSD);
    
	//2, ����ʱ���ֵ    
    delta=DeltaToSec(rTime,&startSD);
    if(delta==FAILED){
        PRINTF("Error - DeltaToSec [%d]\r\n",__LINE__ );
    } 

	getRecordStr(val,val_len,*rTime,&one_record[0]);
	lenOneRecord=(val_len*2+1+6+1)*2;
    
//4, ����offset //ÿ���ֽ�ռ�������ַ����ռ�
    //"0XFA"->"F","A";"0X03"->"0","3"
    //valen*4+2+TIME_STAMP_LEN 
    //lOffset=lTfCfgLen+(delta/rG)*((long)((val_len<<2)+2+TIME_STAMP_LEN));

	offset=SD_REC0+(delta/rG)*((ULNG)(lenOneRecord));

	ret=CardWriteStr(offset,&one_record[0],&ShortFn[0]);
    if(FAILED == ret){
        CALENDAR_IE = FALSE;                    
        return FAILED;
    }
    return SUCCESS;
}

//*************************************************************************
// ͨ��offset & pBelts &val_len ����ȡ��des��д��tf��
// ����д��cx
    
// ��¼����Ա����
// 1,��׼��ʽ:��Ԫ��¼+CPL����Ԫ��¼��+��־��
// ����:valen*2+valen*2+2 =vlan<<2+2

// 2,���Ը�ʽ
//     ��Ԫ��¼+CPL����Ԫ��¼��+��־��(1 byte)+ʱ���+0xFF
// ����:(valen +  valen          +1              +6    +1)*2

// 3 ����offset //ÿ���ֽ�ռ�������ַ����ռ�
// "0XFA"->"F","A";"0X03"->"0","3"
//
// ���أ� ��¼����
//*************************************************************************
void getRecordStr(BYTE *val,BYTE val_len,hTIME dt,BYTE *des)
{
BYTE i,buf[129];
BYTE *p;
BYTE len;
BYTE TimeStamp[15];	//YYMMDDHHmmSS FF

//���ֽ�(val_len)
	p=&buf[0];
    DataCpy(p,val,val_len);
	p+=val_len;

//���ֽ�(val_len)��CPL. cx ȡ��
    for(i=0;i<val_len;i++){
        *p++=~(buf[i]);         //ȡ��
    }

//��־��(1)
    *p++=0x03;

	//split
	len=(BYTE)(p-&buf[0]);
    for(i=0;i<len;i++){
      sprintf((char *)des,"%02x",buf[i]);
	  des+=2;
    }
    *des='\0';     
    
//��ǰʱ���,�������
    sprintf((char *)&TimeStamp[0] ,"%02d%02d%02d%02d%02d%02d%2x" ,
        dt.year,dt.month,dt.day,dt.hour,dt.minute,dt.second,0xFF);

    strcat((char *)des,(const char *)&TimeStamp[0]);
}

/******************************************************************************
//Record data begin from belt
// 1. REC_FMT_BEGIN may be overwritten when
//    data cycles back. so when record being
//  read, length should be limitted.
******************************************************************************/
void TfCardProc(void)
{
#ifdef SD_LISTANDDEL
    BYTE   ret;
    BYTE    i;    
    BYTE    bufferShortN[128];
    BOOL    b_IniFlag = FALSE;
#endif    
    ClrWdt();

    CardInit();      
    CardMount();      

#ifdef SD_LISTANDDEL
    /***********************************************************************
       1��ö���ļ� 
     **************************************************************************/

    PRINTF( "List all file \r\n" );
    ret = ListAll( );  /* ö������U���е������ļ���Ŀ¼ */
    mStopIfError( ret );
    PRINTF( "List all file end \r\n" );

    /***********************************************************************
       2�� ɾ���ļ� 
     **************************************************************************/

    for(i=1;i<FileCount;i++){
        sprintf((char *)bufferShortN, "%s%s", "/", pFileName[i]);//�������ļ���

        ret = CH376FileErase(bufferShortN); // ɾ���ļ�
        if(ret!=USB_INT_SUCCESS) 
        printf( "Error: %02X\r\n", (UINT16)ret );  //��ʾ����

        PRINTF( "Erase File :%s,Succeed\r\n",bufferShortN );  
    }        

    PRINTF( "Erase all file end ,K:%d\r\n\r\n",i++ );
#endif

#ifdef DEBUG_ON
/***********************************************************************
  �����ļ� 
  �Զ����ɳ�&���ļ���
1�������ο���ʱ��
��Ҫ��tf�����г�ʼ�����ڲ˵���->����->"��ʼ���洢��"
 **************************************************************************/
    if(b_IniFlag==FALSE){
        PRINTF( "InitCardFiles\r\n");        
        InitCardFiles();      
        b_IniFlag = TRUE;   
    }
    return ;
#endif    

}


//***********************************************************************
//	����str_time [YYMMDDHHMMSS]
//	���hTIME�ṹ��h_Time; 
//**************************************************************************
void TimeStr2Struct(BYTE *str_time,hTIME *h_Time)
{
    BYTE  str_tmpTime[14],str_tmpTime1[14];
    
    strcpy((char *)str_tmpTime1,(const char *)str_time);   
    
    strcpy((char *)str_tmpTime,(const char *)&str_tmpTime1[0]);
    
    
    str_tmpTime[2] = '\0';    
    h_Time->year = (BYTE)atoi((const char *)(str_tmpTime));
    
    strcpy((char *)str_tmpTime,(const char *)&str_tmpTime1[2]);
    
    str_tmpTime[2] = '\0';
    h_Time->month= (BYTE)atoi((const char *)(str_tmpTime));   

    strcpy((char *)str_tmpTime,(const char *)&str_tmpTime1[4]);
    
    str_tmpTime[2] = '\0';    
    h_Time->day= (BYTE)atoi((const char *)(str_tmpTime));  

    strcpy((char *)str_tmpTime,(const char *)&str_tmpTime1[6]);
    str_tmpTime[2] = '\0';    
    h_Time->hour= (BYTE)atoi((const char *)(str_tmpTime)); 

    strcpy((char *)str_tmpTime,(const char *)&str_tmpTime1[8]);
    str_tmpTime[2] = '\0';    
    h_Time->minute= (BYTE)atoi((const char *)(str_tmpTime));    

    strcpy((char *)str_tmpTime,(const char *)&str_tmpTime1[10]);
    str_tmpTime[2] = '\0';    
    h_Time->second= (BYTE)atoi((const char *)(str_tmpTime));    
    
      PRINTF( "TimeStr2Struct:%d ",h_Time->year );
      PRINTF( ":%.2d ",h_Time->month );
      PRINTF( ":%.2d ",h_Time->day );
      PRINTF( ":%.2d ",h_Time->hour );
      PRINTF( ":%.2d ",h_Time->minute );
      PRINTF( ":%.2d \r\n",h_Time->second );
    
}

/******************************************************************************
����CX_REC_INTERVAL��CX_INTERVAL_UNIT �����ʱ����,�������Ϊ��λ
******************************************************************************/
ULNG Gap2Sec(BYTE unit,BYTE interval)
{
//����rG ��Ϊ0��������С��λ����Ϊ1����60��
ULNG intervalSec = 1;

    switch(unit){
    case UNIT_DAY:                        
        intervalSec=(ULNG)interval*SD_DAY_SEC;
      	break;
    case UNIT_HOUR:                        
        intervalSec=(ULNG)interval*SD_HOUR_SEC;
      	break;
    case UNIT_MINUTE:
        intervalSec=(ULNG)interval*SD_MIN_SEC;
      	break;
    default:
      	intervalSec=(ULNG)60; //��С�����λ60s

/*
#ifdef TF_DEBUG_2S
      intervalSec = (long)2; //60//��С�����λ60s
#endif

#ifdef TF_DEBUG_10S
       intervalSec = (long)10; //60//��С�����λ60s
#endif

#ifdef TF_DEBUG_60S     
       intervalSec = (long)60; //60//��С�����λ60s
#endif  */
    }

    return intervalSec;
}



//******************************************************************************
//���ʱ�����־��β,��0xFF����
//******************************************************************************
/*
void hTimeToStrFlag(hTIME *dt,BYTE *strTime)
{
    sprintf((char *)strTime ,"%02d%02d%02d%02d%02d%02d%2x" ,
        dt->year,dt->month,dt->day,dt->hour,dt->minute,dt->second,0xFF);
}
*/

/*
void CxReadProc(void)
{
   char i,iRec,span;
   BYTE j,ports,offset;            
   UINT addr0,addrX,addr_rec_ptr;
   hTIME rT;
   BYTE cc0[64],ccx[64];        //����CalendarProc()
   BYTE recode_belt[256];            //����CalendarProc()
   BYTE fsName[14];            // "/SSSSSSSS.XYY\0" 13 bytes

    Delay_ms(10);    
    TaskOver();
//UINT ms1,ms2,ms3;
#ifdef DEBUG_PRINT //�ն˴�ӡ��Ϣ���뿪��
    ActivePort = 3;
    UART2Init_Cfgcom(BAUD_115200);
    RS232_on();    
    Delay_ms(10);    
    PRINTF("\r\n");
#endif
    PRINTF("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\r\n");

    i=Now(&rT);                                //read time
    
    if(i==FAILED){iRecFailed=1; goto END_CALENDAR;}
//����time
    //rTime[]��ֻ�ڴ˴�1����ˢ��1�Σ�����Ϊ����ʱ���жϵ�����
    SaveTimeTo(rT,&rTime[0]);        
    NOP10;
    PRINTF("CurrTime:%.2d:%.2d:%.2d" ,rT.year,rT.month,rT.day);
    NOP10;
    PRINTF(":%.2d:%.2d:%.2d\r\n" ,rT.hour,rT.minute,rT.second);
    
    //��С��¼���,�㲥���
#if 1
        span=1;            //5;
        if((rT.minute % span)!=0){
          iRecFailed=2; return;
        }

#endif    

//���˿���
    ports=GetByte(M_CFG,REG0+CM_C0S); 
    if((ports==0)||(ports>10)){
      iRecFailed=3;
      goto END_CALENDAR;
    }


//��¼
//Loop through all ports,record corresponding data
    addr0=MAP_CELL0;                                    //start address of C0
    addrX=AddressOfCxBegin(START_OF_CX_MAP);            //start address of CX
    addr_rec_ptr=ADDR_PTR_REC0;
    offset=0;                                            //byte offset in belt
    i=0;                                                //port=0, mainboard


    while(i<ports){              //Loop through all Cx's
        mem_ReadData(M_CFG,addr0,&cc0[0],lenCELL);        //c0's cfg begin in p2[64]
        j=0;                                                //refer to cellx
        while(j<cc0[C0_CXS]){

        mem_ReadData(M_CFG,addrX,&ccx[0],lenCELL);        //Read cx[]        
        g_bISCX_INBELT = IsCxInBelt(&ccx[0]);
        g_bISCX_RECORD = IsCxRecorded(&ccx[0]);
        g_bISTF_RECORD = IsCxTFRecorded(&ccx[0]);        
        g_bISREC_TIMEMATCH = IsRecTimeMatched(&ccx[0],(BYTE *)&rT);

       PRINTF("CxRecord :%.3d ",g_bISCX_RECORD);  
       NOP10;
       PRINTF("CxInBelt :%.3d ",g_bISCX_INBELT); 
       NOP10;
       PRINTF("TFRecode :%.3d ",g_bISTF_RECORD);
       NOP10;
       PRINTF("TimeMatchflg:%.3d\r\n",g_bISREC_TIMEMATCH);         
        
    if(g_bISCX_INBELT){    //if in belt
      if(g_bISCX_RECORD)
      {   //if should be recorded 
          if(g_bISTF_RECORD)
          { 
          
            NOP10;
            PRINTF("**********%s:%d*********\r\n",__FILE__,__LINE__);
            CreatShortName(&ccx[0],(BYTE*)fsName);
            iRec = ReadTfRecodeBack(&rT,&ccx[0],(BYTE *)fsName,recode_belt);
          }     
          if(iRec!=0)
          {
               //iRecFailed=iRec;
          }

        //ת�� XMem
      }
      addr_rec_ptr+=0x10;                //next rec_ptr
    }
    
        j++;
        offset+=ccx[CX_VAL_BLEN];
          addrX=addrX+((UINT)(64));
      }  
        addr0+=cc0[FLD_LEN]*64;        
      i++;        //refer to port
      }    
    
    ClrWdt();
    NOP10;
    PRINTF("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\r\n");

END_CALENDAR:    
    NOP10;
    TaskOver();
     


}


*/

/******************************************************************************
// �����վCfg��SD��
******************************************************************************/
BYTE ExportCfg2SDCard(BYTE *bShortFn)
{
BYTE 	*p,*pCfg;
UINT 	i,left;
BYTE 	cfg_buf[512];
BYTE 	ret;
UINT 	ulLen;
ULNG 	offset;
BYTE 	ShortFn[14]={"\\"};

	strcat((char *)&ShortFn[0],(const char *)bShortFn);		//���ϡ�\��

//    memset(cfg_buf, 0, sizeof(BYTE)*512);

//�ļ�cfg��ʼλ�ñ�ʾ��"CFG:" 
	offset=0;
    ret=CardWriteStr(offset,(BYTE *)SD_CFG_HEAD,&ShortFn[0]);	//bShortFn);
    if(ret==FAILED){
        CALENDAR_IE=FALSE;                    
        return FAILED;
    }
    
	ulLen=cfg[CM_LEN_CFG]*lenCELL;
	offset+=strlen(SD_CFG_HEAD);
	p=&cfg[0];
    while(ulLen){
      ClrWdt();

	  pCfg=&cfg_buf[0];

      //����ļ��Ƚϴ�,һ�ζ�����,������ȡ
//      memset(pCfg, 0, sizeof(BYTE)*(CFGLEN*2+3));		//3: 1-CR;1-LF;1-0
      if(ulLen>CFGLEN){   
        left=CFGLEN; 	//ʣ�����ݽ϶�,���Ƶ��ζ�д�ĳ��Ȳ��ܳ�����������С
      }
      else{
        left=ulLen;  	//���ʣ����ֽ��� 
      }					//����,��ȥ��ǰʵ���Ѿ��������ַ��� 
 
      i=left;
      while(i--){
        *pCfg++=HexHiByte(*p);			 
        *pCfg++=HexLoByte(*p);
		p++;			      }
      //��ӻس����з�
      //*pCfg++=CR; 
      //*pCfg++=LF;
	  *pCfg=0; 		
      ret=CardWriteStr(offset,&cfg_buf[0],&ShortFn[0]);		//д�ַ���
      if(ret==FAILED){
        CALENDAR_IE=FALSE;                    
        return FAILED;}
	  offset+=left*2;	//+3;
      ulLen-=left;
    }
    ClrWdt();
   
    return SUCCESS;
}

/*
//=====================================
// ������ʷ��¼
//=====================================
UINT MakeHistoryBelt(BYTE *belt,BYTE *inst)
{
hTIME 	dt;
BYTE 	iC0,iCx;
//BYTE 	i;
BYTE 	ShortFn[14]={"\\"};
BYTE 	fsName[14];
BYTE des[256]; 
ULNG 	ulOffset;
UINT des_len;


//ҵ���
	memcpy((BYTE *)&dt,inst+TSEN_RECS_YEAR,6);
	iC0=*(inst+TSEN_IC0);
	iCx=*(inst+TSEN_ICX);

//�ļ���
	getShortName(iC0,iCx,&fsName[0]);
	strcat((char *)&ShortFn[0],(const char *)&fsName[0]);		//���ϡ�\��

//λ��
	ulOffset=getRecPosition(&dt,iC0,iCx);

//����
	des_len=*(inst+TSEN_LEN_LIMIT);
	des_len=30;

    CardReadRec(ulOffset,des_len,&des[0],&ShortFn[0]); 
	return des_len;
}


//=====================================
// ��ȡ��ʷHead
//=====================================
void MakeHistoryHead(BYTE op,UINT lenMsg,BYTE *msg,UINT property)
{
UINT sum;
ULNG li;

	*(msg+FRM_VER)=INST_HEADER;

	*(msg+FRM_LEN)=LoOfInt(lenMsg-1);		
	*(msg+FRM_LEN+1)=HiOfInt(lenMsg-1);

//source address
	memcpy((BYTE *)(msg+FRM_SRC_ADDR),&cfg[CM_STA_ID],4);

	*(msg+FRM_OP)=op;
	*(msg+FRM_PROPERTY_L)=LoOfInt(property);
	*(msg+FRM_PROPERTY_H)=HiOfInt(property);

//Unsolicitedʱ��ȡ0x8000(bit15=1)��������inst_index={0-0x7fff}
//	*(msg+FRM_INST_INDEX)=0;		//2 bytes
//	*(msg+FRM_INST_INDEX+1)=0x80;	

//MsgIndex++, �����յ���ͬ(StaID & MsgIndex)ʱ��ֻȡһ��
	mem_ReadData(M_MEM,(ULNG)mapMSG_INDEX,(BYTE *)(msg+TCOM_MSG_INDEX),4);
	li=getULNG(msg+TCOM_MSG_INDEX); li++;		//+1,����
	mem_WriteData(M_MEM,(BYTE *)&li,(ULNG)mapMSG_INDEX,4);

//CfgIndex
	mem_ReadData(M_CFG,(ULNG)CM_CFG_INDEX,(BYTE *)(msg+TCOM_CFG_INDEX),2);

	*(msg+TCOM_PSW)=0;				//3 bytes
	*(msg+TCOM_FAMILY_CO)=0;		//1 bytes
	*(msg+TCOM_MAC_ID)=0;			//6 bytes

	sum=GetCRC(msg,lenHEAD-2);
	*(msg+HCRC_L)=LoOfInt(sum);
	*(msg+HCRC_H)=HiOfInt(sum);		
}

//=====================================
// ��ȡ��ʷ��¼
//=====================================
void UpLoadSDRecs(BYTE *inst)
{
UINT lenBelt;
BYTE *belt;
BYTE msg[256];
UINT property;

	belt=&msg[0]+32;

//����ҵ���
	lenBelt=MakeHistoryBelt(belt,inst); 

//����֡ͷ
	property=0b0000000100000001;
	MakeHistoryHead(OP_READ_RECS,lenBelt+32,&msg[0],property);
//	return lenBelt+32;

jdcnklajnkjd

}
*/

