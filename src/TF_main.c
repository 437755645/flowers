#include "header.h"
#include "h_macro.h"

extern BYTE cfg[];
extern SW	sw1;

/******************************************************************************
this function model is support for the tf  storage
1,created by :heqing 

2,modified by :qiankechong 
   data:            20120307
每一个终端对应生成一个文件，用长文件名表示
长文件名格式为AddrIDPortsCells.hyd
对应的短文件名为:AddrID.PortsCells
其中AddrID:8b;Ports:1b;Cells:2b
*******************************************************************************/

BYTE		g_uchSDInserFlg = 0; 					//SD卡插入标志位
BYTE    	g_LongNameBuf[LONG_NAME_BUF_LEN];    	//存放长文件名
FILE_NAME   FileNameBuffer[MAX_FILE_COUNT];    		//文件名结构
UINT		FileCount;
BYTE *      pFileName[MAX_FILE_COUNT];

BOOL        b_IniFlag = FALSE;


/******************************************************************************
函数实现:tf卡初始化时，在LCD屏幕上显示的进度条
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
          lcd_cursor(0,L0); lcd_puts("tf卡[  ]               ");
          sprintf(msg,"%d",100-i);
          PRINTF("msg:%s\r\n",msg );
          lcd_cursor(5,L0); lcd_puts("98"); 
         // lcd_cursor(6,L1); lcd_puts_ram(msg);          
          ClrWdt();
    }
    
}
*/

/******************************************************************************
                初始化TF卡接口
1，初始化模拟SPI:TF卡主芯片CH376通过I/O模拟SPI控制
2，初始化CH376host
******************************************************************************/
BYTE CardInit(void)
{    
    BYTE    s = 0;
    BYTE    reSet = 0;
    
    Init_SPI_ch376();    
    Delay_ms( 100 );  /* 延时100毫秒 */
    PRINTF("\r\nCH376 STDIO init !\r\n" );  
    
reStart:    
    s = mInitCH376Host( );  /* 初始化CH376 */
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
                初始化TF卡是否在位轮询，
                
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
    
        /* 检查U盘是否连接,等待U盘插入,
        对于SD卡,可以由单片机直接查询SD卡座的插拔状态引脚 */
        vLedTest_ms(250 );  /* LED闪烁 */
        PRINTF(".");
        if(0 == g_uchSDInserFlg) //不反复刷新LCD
        {
        
            //light_off();         //关闭背光
            mStopIfError( s );
        }
        g_uchSDInserFlg = 1;    // 标志位置1            
    }
    //当SD被插拔后，需要重现进行初始化
    if( 1 == g_uchSDInserFlg)
    {
    
        g_uchSDInserFlg = 0; // 标志位清0   
        s = mInitCH376Host( );  /* 初始化CH376 */
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
            /* 检查U盘是否连接,等待U盘插入,对于SD卡,
            可以由单片机直接查询SD卡座的插拔状态引脚 */
            PRINTF( "sys err ! pls reset SD module!\r\n" );
            g_uchSDInserFlg = 1;            
        }
    }
    
   // PRINTF( "Found Udisk/SD" );
    /* 延时,可选操作,有的USB存储器需要几十毫秒的延时 */
    
    /* 对于检测到USB/SD设备的,最多等待10*50mS */
    for ( i = 0; i < 10; i ++ ) 
    { 
    
         /* 最长等待时间,10*50mS */
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
        
            /* 初始化磁盘并测试磁盘是否就绪 */
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
*/


/******************************************************************************
// 复制该长文件名的ANSI编码的全路径的短文件名
//(8+3格式), 短文件名由长文件名生成
//(只要不冲突,其实随便定)

以时间为短文件名命名
且短文件名，仅存在于根目录下"\"
因此文件名的格式为"/XYYID.TXT"
YY = 00-99,表示年份的最后2位
MM = 01-12，表示为月份
DD = 01-32，表示为日期
P = 0-2，表示为端口号
T = 0-9，表示配置的模式
***************************************************************
//构造短文件名，扩展名[XYYID.基元号虚元号]
\0YY01000.102
\03301000.101
//文件名前半部分,为单板的ID 号
//基元号在先,取低位。
//虚元号在后
******************************************************************************/
BYTE getShortName(BYTE iC0,BYTE iCx,BYTE *ShortFn)
{        
char chNode[14];     
BYTE ShortName[14] = {0};		//{"\\"};		//{'\'};     
char ext[5];
ULNG ulStaId;

    //文件名前半部分,根目录
   
//  bYear = GetByte(M_CFG,MAP_REG_RC_START_TIME+YEAR);
//  sprintf((char *)sYear,"/D%02d",bYear);
//	PRINTF( "sYear:%s\r\n",sYear );
    
//    strcat((char *)&ShortName[0],(const char *)sYear);
    
//  GetNodeAddress(&chNode[0]);
	ulStaId=getULNG(&cfg[CM_STA_ID]);
    sprintf(&chNode[0],"%08ld",ulStaId);

    strcat((char *)&ShortName[0],(const char *)&chNode[0]);
    
    ext[0]='.';
    //构造短文件名，扩展名
    ext[1]=HexLoByte(iC0);		//基元号在先,取低位。
    ext[2]=HexHiByte(iCx);		//虚元号在后
    ext[3]=HexLoByte(iCx);		
    ext[4]=0x0;
    
    strcat((char *)&ShortName[0],(const char *)&ext[0]);
    strcpy((char *)ShortFn,(const char *)&ShortName[0]);
    
    return  SUCCESS;
}

/******************************************************************************
1,长文件名写文件,长文件名的配置
2,短文名的构造,利用年月日组成来构造 
   如:"/YYMMDD.TXT\0"
3，长文件名需要进一步的格式转换

以时间为长文件名命名
且短文件名，仅存在于根目录下"/"
因此文件名的格式为"/IDPT.hyd"
YY = 00-99,表示年份的最后2位
MM = 01-12，表示为月份
DD = 01-32，表示为日期
P = 0-2，表示为端口号
T = 0-9，表示配置的模式
***************************************************************
//构造长文件名,  扩展名[M五位ID.基元号虚元号.hyd]
01000 .102
//文件名前半部分,为单板的ID 号
//基元号在先,取低位。
//虚元号在后

******************************************************************************/
void getLongName(BYTE *bShortFn,BYTE *bLongfn)
{      
    BYTE    bShortName[14] = {0};    
    BYTE    i,j ; 

    PRINTF("CreatlongFn ,shortFn:%s[%d]\r\n",bShortFn,__LINE__);

    //删除短文件名中的''.'' 号
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
    
    //长文件名构造
    sprintf((char *)bLongfn,"H%s.hyd",bShortName);
}



/******************************************************************************
// 获取虚元卡记录时间初值
[YYMMDDHHMMSS]        记录起始时间，对应[REC0]
******************************************************************************/
BYTE get_SDCardStartTime(BYTE *sDateTime,BYTE * bShortFn)
{
ULNG offset; 
BYTE s;

    ClrWdt();


    /* 打开文件,该文件在根目录下 */    
   	s = CH376FileOpen(bShortFn);
   	if(s!= USB_INT_SUCCESS){            
       PRINTF("Read - Open  file1 ,Error!\r\n" );
       mStopIfError( s );  /* 打开文件时出错 */
       return 0;
	}

	offset=strlen(SD_CFG_HEAD);
	offset+=cfg[CM_LEN_CFG]*(lenCELL+2);		//2:for CRLF
	offset+=strlen(SD_RECODE_HEAD);
   	s=CH376ByteLocate(offset); 
   	if(s!=USB_INT_SUCCESS){            
       mStopIfError(s);  /* 打开文件时出错 */
       return 0;
    }        

    //标示:YYMMDDHHMMSS
   	CardReadRec(offset,SD_STIME_LEN,sDateTime,bShortFn); 
   	delay_us(100);

   	s = CH376FileClose( FALSE );  /* 关闭文件 */
   	mStopIfError( s );

   	return  USB_INT_SUCCESS ;
}

//******************************************************************************
// 获取虚元卡记录时间初值
// [YYMMDDHHMMSS]        记录起始时间，对应[REC0]
// 返回CFG 长度
//******************************************************************************
BYTE set_CardStartTimeInCfg(hTIME dt,BYTE *bShortFn)
{
ULNG	offset; 
BYTE   	s,ret;
BYTE   	sDateTime[15];	//[YYMMDDHHMMSS]
BYTE 	ShortFn[14]={"\\"};

	strcat((char *)&ShortFn[0],(const char *)bShortFn);		//加上“\”

   	ClrWdt();
    
// 打开文件,该文件在根目录下     
    s = CH376FileOpen(&ShortFn[0]);		//bShortFn );
    if(s!=USB_INT_SUCCESS){            
        PRINTF("Read - Open  file2 ,Error!\r\n" );
        mStopIfError( s );  /* 打开文件时出错 */
        return 0;
    }

//Skip: CFG & cfg[]
	offset=strlen(SD_CFG_HEAD);
	offset+=cfg[CM_LEN_CFG]*lenCELL*2;		
    s = CH376ByteLocate(offset); 
    if(s!=USB_INT_SUCCESS){            
        mStopIfError(s);  /* 打开文件时出错 */
        return 0;
    }

//标示:"RECODES:" 
    ret=CardWriteStr(offset,(BYTE *)SD_RECODE_HEAD,&ShortFn[0]);
    if(ret==FAILED){
        CALENDAR_IE=FALSE;                    
        return FAILED;
    }
	offset+=strlen(SD_RECODE_HEAD);

//标示:[YYMMDDHHMMSS]
    sprintf((char *)sDateTime ,"[%02d%02d%02d%02d%02d%02d]",dt.year,dt.month,dt.day,dt.hour,dt.minute,dt.second);
    CardWriteStr(offset,sDateTime,&ShortFn[0]);
    
    return  SUCCESS;
}


/******************************************************************************
// 获取虚元卡
//CFG 长度
******************************************************************************/
ULNG getSDCfgLen(void)
{
ULNG STimeOffset;

    STimeOffset=(ULNG)cfg[CM_LEN_CFG]*(lenCELL*2);
    STimeOffset+=strlen(SD_CFG_HEAD)+strlen(SD_RECODE_HEAD)+SD_STIME_LEN;
    return STimeOffset;
}


/******************************************************************************
// 获取虚元卡记录文件名
// [M] [站号] [站名] [虚元号] [虚元名] . hyd

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
创建长文件名
在不同的端口下，系统自动根据当前端口，ID等配置信息
生成各自的长 &短文件名
同时返回短文件名做为后续读写所用

2,根据系统cfg配置，自动生成对应的长短文件名。

******************************************************************************/
BYTE CreatCxCardFile(BYTE *bShortFn,BYTE *bLongFn)
{
BYTE    bLongNameBuf[64]; // 存放长文件名UNICODE格式
BYTE    s = 0;
BYTE    j = 0;
BYTE 	ShortFn[14]={"\\"};

	strcat((char *)&ShortFn[0],(const char *)bShortFn);		//加上“\”

/* 复制并转换长文件名到LongNameBuf长文件名缓冲区 */
/* 将英文字符转换为两字节的小端UNICODE编码 */
    for ( j = 0; bLongFn[j] != 0; j++ ){  
        bLongNameBuf[j*2] = bLongFn[j];  
        bLongNameBuf[j*2+1] = 0x00;
    }
    bLongNameBuf[j*2] = 0x00;  // 末尾用两个0表示结束
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
    {     //创建失败退出
        mStopIfError( s );
    }
        
    return  s ;
}


//=================================
// 初始化单个虚元TF记录
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
// 计算以秒为单位的时间差
//===================================
/*
long DeltaToSec(hTIME *curT,hTIME *staT)
{
long delta;

//当前时间cT：
//    delta=((((((YEA*12+MON)*32+DAY)*24+HOU)*60+MIN)*60+SEC)-
//                ((((((yea*12+mon)*32+day)*24+hou)*60+min)*60+sec)
//为防止 YEA*12 溢出，算式修改如下?
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
//offset 从1开始，即第一个字节offset为1
//offset 为0xFFFFFFFF 表示从文件末尾写入
//
// 将字符串buf写入bShortFn,位置Offset
//*****************************************************************/
BYTE CardWriteStr(ULNG offset,BYTE *buff,BYTE *bShortFn)
{
ULNG  	FileSize,Diff; 
BYTE    s;    
BYTE    buf_1[2]="0";      		//最大1字节,read/write buffer   
BYTE    buf_fill[513]="0";   	//最大512字节,read/write buffer   
long    i;   
UINT    j;		//65536 最大记录次数
UINT 	len;
    
#ifdef    WATCHDOG_ON
    ClrWdt();
#endif

//参数判断
    if(offset <0/*||offset > (1<<32)*/){
//        PRINTF( "FAILED!offset :%ld error,must offset>0 && offset <2<<32\r\n", offset );
        return FAILED;
    }

    
//打开文件,该文件在根目录下    
    s=CH376FileOpen(bShortFn);
    if (s!=USB_INT_SUCCESS){            
        mStopIfError(s);  /* 打开文件时出错 */
        CALENDAR_IE = FALSE;                    
        return FAILED;
    }
    
    FileSize=CH376GetFileSize();

    //判读文件的偏移位置，与文件长度FileSize的比较
    if(offset<=FileSize){
        s = CH376ByteLocate(offset);  /* 移到文件指定的位置 */ 
        mStopIfError( s );
    }


    //判读是否是新创建的文件，新创建文件默认1字节，
    //此时，需要将文件指针移到文件的头部，否则移动指针到文件的尾部
    else{   
        if(FileSize==1){
            s=CH376ByteLocate(0);  /* 移到文件的头部 */    
            FileSize = 0;
        }
        else{
            s=CH376ByteLocate(0xFFFFFFFF);  /* 移到文件的尾部 */            
        }    
                
        Diff=offset-FileSize; 
        //需要填充的数量是(偏移-长),每写入65535(=ONETIME_MAX_RW)个字节后更新文件长度，
        //防止因为太长中途异常的原因，而导致下一次又要重头来过的.
        
        //大数据则分次写入，实时更新文件长度
		//为了提高读写操作的效率，尽量一次读写较大的数据块，最大为65535，建议为512的倍数。
        for(j=0;j<=Diff/BUF65536;j++){
            if(Diff>=BUF65536){
                for(i=0;i<(BUF65536/BUF512);i++){
                    sprintf((char *)buf_fill, "%0512d",0);	//补零处理
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
                    sprintf((char *)buf_fill,"%0512d",0);	//补零处理
                    s=CH376ByteWrite(buf_fill,BUF512,NULL);  
                    if(s!=USB_INT_SUCCESS){
                        mStopIfError(s);
                    }                
                    delay_us(2);             
                    if(4 == i%5){
                       ClrWdt();
                    }
                }
                //512整除后的余数部分
                for(i=0;i<(Diff%BUF512);i++){               
                    sprintf((char *)buf_1,"%01d",0);//补零处理
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
            
            //更新长度
            CH376ByteWrite(&buf_1[0],0,NULL);  
            //以字节为单位向文件写入数据,因为是0字节写入,所以只用于更新文件的长度,当阶段性写入数据后,可以用这种办法更新文件长度
                
            FileSize=CH376GetFileSize();  	//重新获取文件大小
            s=CH376ByteLocate(0xFFFFFFFF); 	//移到文件的尾部  
            Diff=offset-FileSize; 
        }   
    }
    
//从offset位置开始写入数据buff
	len=strlen((char *)buff);
    s=CH376ByteWrite((BYTE *)buff,len,NULL);        
    mStopIfError(s);

//更新长度
	CH376ByteWrite(&buf_1[0],0,NULL);

//CloseFile
    s=CH376FileClose(FALSE);  /* 关闭文件 */
    mStopIfError( s );

    return SUCCESS;
}

/******************************************************************************
    读函数，
    指定偏移，回读长度为ulLen的数据
    返回值:*des
    限制条件:每次最多读取256字节数据，
                           offset-ulen>=1;
                           offset<=filesize
******************************************************************************/
BYTE CardReadRec(ULNG offset,ULNG ulLen,BYTE *des,BYTE *bShortFn)
{
BYTE	s;    
long   	lDiff;                  //offset 与ulLen的差值，
ULNG  	i,j ;
ULNG  	ulFileSize =0;
UINT  	uRealCount;

    ClrWdt();
    
//打开文件,该文件在根目录下
    s=CH376FileOpen( bShortFn );
    if(s!=USB_INT_SUCCESS){            
        PRINTF( "Read - Open  file4 ,Error!\r\n" );
        mStopIfError( s );  /* 打开文件时出错 */
        return FAILED;
    }
    
    ulFileSize = CH376GetFileSize( );  //准备读取总长度
    if(offset>ulFileSize){
        PRINTF( "Error! the offset is big than the filesize\r\n" );
        return FAILED ; 
    }

    //offset与ulLen的差值，差值小于0则，越界
    lDiff=(long)(offset-ulLen);
    if(lDiff<0){
        PRINTF( "Error!offset-ulLen:%ld<0,Verify the input data\r\n" ,offset-ulLen);
        return FAILED; 
    }        
    /* 文件存在并且已经被打开,移动文件指针到尾部以便添加数据 */

    s=CH376ByteLocate( offset-ulLen );  /* 移到文件指定位置 */             
    if(s!=USB_INT_SUCCESS){
        PRINTF( "move file locate,Error! \r\n" );
        mStopIfError( s );
    }                
   
    //PRINTF( "Read the pre %ld chars from %ld of the file:",ulLen,offset );

	//如果文件比较大,一次读不完,可以再调用CH376ByteRead继续读取,文件指针自动向后移动
    while(ulLen){  
        if(ulLen>256){i = 256;}    	//剩余数据较多,限制单次读写的长度不能超过缓冲区大小
        else{i=ulLen;}  			//最后剩余的字节数
        
        s=CH376ByteRead(des,i,&uRealCount);  //以字节为单位读取数据块,单次读写的长度不能超过缓冲区大小,第二次调用时接着刚才的`向后读 */
        mStopIfError(s);
        ulLen-=uRealCount;             
            
        //计数,减去当前实际已经读出的字符数
        for(j=0;j<uRealCount;j++){
            //PRINTF( "%c", des[j] );  /* 显示读出的字符 */  
        }
        des[j]='\0';           	//结束符
        if(uRealCount<i){  	//实际读出的字符数少于要求读出的字符数,说明已经到文件的结尾
            //PRINTF( "\r\n" );
            //PRINTF( "The file point arrive  to the  end!\r\n" );
            break;
        }
    }
    
    //PRINTF( ",Close \r\n" );
    s=CH376FileClose(FALSE);  	//关闭文件
    mStopIfError(s);
    
    return OK;
}  

/******************************************************************************
// 通常用于手动完成的初始化：
// 包括对全部需要转储、即时信息查询的虚元，
//1、建立文件名；
//2、输出cfg；
//3、计算下一个起始记录时间；
//4、写入起始记录时间；
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
	
	  if(!IsC0Enabled(&cfg[c0])){continue;}		//C0使能
	  if(!IsC0Recorded(&cfg[c0])){continue;}	//C0记录使能

	  for(iCx=0;iCx<cfg[c0+C0_CXS];iCx++){
		cx=getCX(iC0,iCx);
	    if(IsCxInBelt(&cfg[cx])){						//if in belt
	      if(IsCxSDRecorded(&cfg[cx])){					//if should be recorded

            memset((char*)&flName[0],0, sizeof(char)*32);
            memset((char*)&fsName[0],0, sizeof(char)*14);
                
            getShortName(iC0,iCx,&fsName[0]);
            getLongName(&fsName[0],&flName[0]);
                
            //1，根据长短文件名，创建记录文件(长文件名)
            s=CreatCxCardFile(&fsName[0],&flName[0]);
			if(s==ERR_NAME_EXIST){
				Nop();
			}

            //2，导出cfg信息并记录到SD-card中
            s=ExportCfg2SDCard((BYTE *)fsName);
			if(s==ERR_NAME_EXIST){
				Nop();
			}

    		//3，设置文件的格式1[记录起始时间戳]
            set_CardStartTimeInCfg(dt,(BYTE *)fsName);
		  }
	    }
      }  
  	}

    // lcd_cursor(0,L3); lcd_puts("创建文件成功            ");       
    TX1_IE   = FALSE;
    RX2_IE   = TRUE;
    RX1_IE   = TRUE;
}

//========================================================
// 获得Rec位置
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

    //1,获取cfg 长度    
    SD_REC0=getSDCfgLen();

    //根据当前时间、CX_REC_INTERVAL和CX_INTERVAL_UNIT 计算出下一个记录时间
	cx=getCX(iC0,iCx);
    unit=cfg[cx+CX_REC_GAP]&0x0f;
    interval=cfg[cx+CX_REC_GAP];
    val_len=cfg[cx+CX_VAL_BLEN];    
    rGAP=Gap2Sec(unit,interval);

    //1,获取starttime    
    get_RecordStartTime(&startSD);
     
	//2, 计算时间差值    
    delta=DeltaToSec(this,&startSD);
	if((delta<0)||(delta>100000)){
	}

	lenOneRecord=(val_len*2+1+6+1)*2;
    
    ulOffset=SD_REC0+(delta/rGAP)*((ULNG)(lenOneRecord));
	return ulOffset;
}



//=========================================================
//
// 1、Belts is ready when calendar() is called previously
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

    //关闭时钟中断,锁定记录,防止在读数据时,数据被改写.          
    CALENDAR_IE = FALSE; 

//Rec位置
	ulOffset=getRecPosition(this,iC0,iCx);

//记录长度
	cx=getCX(iC0,iCx);
    val_len=cfg[cx+CX_VAL_BLEN];    
	lenOneRecord=(val_len*2+1+6+1)*2;

//文件名
	getShortName(iC0,iCx,&fsName[0]);
	strcat((char *)&ShortFn[0],(const char *)&fsName[0]);		//加上“\”

    CardReadRec(ulOffset,lenOneRecord,des,&ShortFn[0]); 
        
    //开启时钟中断,解除锁定
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
	strcat((char *)&ShortFn[0],(const char *)&fsName[0]);		//加上“\”
    
    //记录起始位置    
    SD_REC0=getSDCfgLen();
    
	//根据当前时间、CX_REC_INTERVAL 和CX_INTERVAL_UNIT 计算出下一个记录时间
    cx=getCX(iC0,iCx);
	unit=cfg[cx+CX_REC_GAP]>>6;
    interval=cfg[cx+CX_REC_GAP]&0x3f;
    val_len=cfg[cx+CX_VAL_BLEN];    
    rG=Gap2Sec(unit,interval);		//间隔：以sec为单位
    
	//1,获取起始记录时间startSD    
    get_RecordStartTime(&startSD);
    
	//2, 计算时间差值    
    delta=DeltaToSec(rTime,&startSD);
    if(delta==FAILED){
        PRINTF("Error - DeltaToSec [%d]\r\n",__LINE__ );
    } 

	getRecordStr(val,val_len,*rTime,&one_record[0]);
	lenOneRecord=(val_len*2+1+6+1)*2;
    
//4, 计算offset //每个字节占用两个字符串空间
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
// 通过offset & pBelts &val_len 长度取出des，写入tf卡
// 构造写入cx
    
// 记录的虚员构造
// 1,标准格式:虚元记录+CPL（虚元记录）+标志字
// 长度:valen*2+valen*2+2 =vlan<<2+2

// 2,测试格式
//     虚元记录+CPL（虚元记录）+标志字(1 byte)+时间戳+0xFF
// 长度:(valen +  valen          +1              +6    +1)*2

// 3 计算offset //每个字节占用两个字符串空间
// "0XFA"->"F","A";"0X03"->"0","3"
//
// 返回： 记录长度
//*************************************************************************
void getRecordStr(BYTE *val,BYTE val_len,hTIME dt,BYTE *des)
{
BYTE i,buf[129];
BYTE *p;
BYTE len;
BYTE TimeStamp[15];	//YYMMDDHHmmSS FF

//正字节(val_len)
	p=&buf[0];
    DataCpy(p,val,val_len);
	p+=val_len;

//反字节(val_len)。CPL. cx 取反
    for(i=0;i<val_len;i++){
        *p++=~(buf[i]);         //取反
    }

//标志字(1)
    *p++=0x03;

	//split
	len=(BYTE)(p-&buf[0]);
    for(i=0;i<len;i++){
      sprintf((char *)des,"%02x",buf[i]);
	  des+=2;
    }
    *des='\0';     
    
//当前时间戳,结束添加
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
       1，枚举文件 
     **************************************************************************/

    PRINTF( "List all file \r\n" );
    ret = ListAll( );  /* 枚举整个U盘中的所有文件及目录 */
    mStopIfError( ret );
    PRINTF( "List all file end \r\n" );

    /***********************************************************************
       2， 删除文件 
     **************************************************************************/

    for(i=1;i<FileCount;i++){
        sprintf((char *)bufferShortN, "%s%s", "/", pFileName[i]);//构建短文件名

        ret = CH376FileErase(bufferShortN); // 删除文件
        if(ret!=USB_INT_SUCCESS) 
        printf( "Error: %02X\r\n", (UINT16)ret );  //显示错误

        PRINTF( "Erase File :%s,Succeed\r\n",bufferShortN );  
    }        

    PRINTF( "Erase all file end ,K:%d\r\n\r\n",i++ );
#endif

#ifdef DEBUG_ON
/***********************************************************************
  创建文件 
  自动生成长&短文件名
1，当初次开启时，
需要对tf卡进行初始化，在菜单项->工具->"初始化存储卡"
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
//	输入str_time [YYMMDDHHMMSS]
//	输出hTIME结构的h_Time; 
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
根据CX_REC_INTERVAL和CX_INTERVAL_UNIT 计算出时间间隔,换算成秒为单位
******************************************************************************/
ULNG Gap2Sec(BYTE unit,BYTE interval)
{
//由于rG 不为0，并且最小单位设置为1分钟60秒
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
      	intervalSec=(ULNG)60; //最小间隔单位60s

/*
#ifdef TF_DEBUG_2S
      intervalSec = (long)2; //60//最小间隔单位60s
#endif

#ifdef TF_DEBUG_10S
       intervalSec = (long)10; //60//最小间隔单位60s
#endif

#ifdef TF_DEBUG_60S     
       intervalSec = (long)60; //60//最小间隔单位60s
#endif  */
    }

    return intervalSec;
}



//******************************************************************************
//添加时间戳标志结尾,以0xFF结束
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
   BYTE cc0[64],ccx[64];        //用于CalendarProc()
   BYTE recode_belt[256];            //用于CalendarProc()
   BYTE fsName[14];            // "/SSSSSSSS.XYY\0" 13 bytes

    Delay_ms(10);    
    TaskOver();
//UINT ms1,ms2,ms3;
#ifdef DEBUG_PRINT //终端打印信息必须开启
    ActivePort = 3;
    UART2Init_Cfgcom(BAUD_115200);
    RS232_on();    
    Delay_ms(10);    
    PRINTF("\r\n");
#endif
    PRINTF("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\r\n");

    i=Now(&rT);                                //read time
    
    if(i==FAILED){iRecFailed=1; goto END_CALENDAR;}
//保存time
    //rTime[]：只在此处1分钟刷新1次，可作为其它时间判断的依据
    SaveTimeTo(rT,&rTime[0]);        
    NOP10;
    PRINTF("CurrTime:%.2d:%.2d:%.2d" ,rT.year,rT.month,rT.day);
    NOP10;
    PRINTF(":%.2d:%.2d:%.2d\r\n" ,rT.hour,rT.minute,rT.second);
    
    //最小记录间隔,广播间隔
#if 1
        span=1;            //5;
        if((rT.minute % span)!=0){
          iRecFailed=2; return;
        }

#endif    

//读端口数
    ports=GetByte(M_CFG,REG0+CM_C0S); 
    if((ports==0)||(ports>10)){
      iRecFailed=3;
      goto END_CALENDAR;
    }


//记录
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

        //转储 XMem
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
// 输出测站Cfg到SD卡
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

	strcat((char *)&ShortFn[0],(const char *)bShortFn);		//加上“\”

//    memset(cfg_buf, 0, sizeof(BYTE)*512);

//文件cfg起始位置标示符"CFG:" 
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

      //如果文件比较大,一次读不完,分批读取
//      memset(pCfg, 0, sizeof(BYTE)*(CFGLEN*2+3));		//3: 1-CR;1-LF;1-0
      if(ulLen>CFGLEN){   
        left=CFGLEN; 	//剩余数据较多,限制单次读写的长度不能超过缓冲区大小
      }
      else{
        left=ulLen;  	//最后剩余的字节数 
      }					//计数,减去当前实际已经读出的字符数 
 
      i=left;
      while(i--){
        *pCfg++=HexHiByte(*p);			 
        *pCfg++=HexLoByte(*p);
		p++;			      }
      //添加回车换行符
      //*pCfg++=CR; 
      //*pCfg++=LF;
	  *pCfg=0; 		
      ret=CardWriteStr(offset,&cfg_buf[0],&ShortFn[0]);		//写字符串
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
// 建立历史记录
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


//业务包
	memcpy((BYTE *)&dt,inst+TSEN_RECS_YEAR,6);
	iC0=*(inst+TSEN_IC0);
	iCx=*(inst+TSEN_ICX);

//文件名
	getShortName(iC0,iCx,&fsName[0]);
	strcat((char *)&ShortFn[0],(const char *)&fsName[0]);		//加上“\”

//位置
	ulOffset=getRecPosition(&dt,iC0,iCx);

//长度
	des_len=*(inst+TSEN_LEN_LIMIT);
	des_len=30;

    CardReadRec(ulOffset,des_len,&des[0],&ShortFn[0]); 
	return des_len;
}


//=====================================
// 读取历史Head
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

//Unsolicited时，取0x8000(bit15=1)。区别于inst_index={0-0x7fff}
//	*(msg+FRM_INST_INDEX)=0;		//2 bytes
//	*(msg+FRM_INST_INDEX+1)=0x80;	

//MsgIndex++, 中心收到相同(StaID & MsgIndex)时，只取一条
	mem_ReadData(M_MEM,(ULNG)mapMSG_INDEX,(BYTE *)(msg+TCOM_MSG_INDEX),4);
	li=getULNG(msg+TCOM_MSG_INDEX); li++;		//+1,保存
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
// 读取历史记录
//=====================================
void UpLoadSDRecs(BYTE *inst)
{
UINT lenBelt;
BYTE *belt;
BYTE msg[256];
UINT property;

	belt=&msg[0]+32;

//建立业务包
	lenBelt=MakeHistoryBelt(belt,inst); 

//建立帧头
	property=0b0000000100000001;
	MakeHistoryHead(OP_READ_RECS,lenBelt+32,&msg[0],property);
//	return lenBelt+32;

jdcnklajnkjd

}
*/

