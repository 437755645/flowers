#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "base.h"

//=====================================
//Functions
//=====================================

//In_Sensor.c
void UserBelt(BYTE iC0,BYTE *b);	//传感器集成入口
void usrSensorRecord(hTIME rT);		//用户传感器定时操作入口

void InitInnerSensor(void);

BYTE *PutCxInBelt(UINT cx,BYTE *b);

//******* 翻斗雨量 ******* 
void IncRain(void);
void RainOver(void);
void ReadRain(UINT *rain_count);
void WriteRain(UINT iRain);
BYTE EventRain(void);

//*******格雷码水位*******
void ReadEncoder(UINT *w1,UINT *w2);
void LevelCoder(UINT *w1,UINT *w2);
BYTE Shift8bits(void);
UINT GracodeToBinary(UINT g,BYTE n);
double GetSlopeCo(UINT addr_co);
BYTE ReadEncoderPolarity(void);
BYTE EventCondition(BYTE *var,BYTE iC0,BYTE iCx,BYTE *link);

//DEBUG.c
void DEBUG(void);

//ADC.C
void initAdc1(void);
void initAdcKey(void);
void initTmr3(void);

//Init.c
void UART1Init(ULNG Baudrate);
void UART2Init(ULNG Baudrate);
void UART3Init(ULNG Baudrate);
void UART4Init(ULNG Baudrate);
void InitCN(void);
void InitINTx(void);
void InitCPU(void);
void InitIPL(void);
void Timer1_Init_Function();
void Timer2_on(void);
void Timer2_off(void);
void SetIPL(BYTE event_id,UINT level);

//R4571.C
void R4571_init(void);
void R4571_WriteByte(char output);
char R4571_ReadByte(void);
void R4571_Write_Data(char addr,BYTE *data,int len);
void R4571_Read_Data(char addr,BYTE *data,int len);

//DS18B20.c
BYTE ReadID(void);

//main.c
void usrProc(void);
void KeyDisplay(void);
BYTE TaskLine(void);
void EndCom(void);
void InitCPU(void);
BYTE BasicBeRead(BYTE *p);
void SwitchToComport(BYTE port);
BYTE ShowDownResult(BYTE result);
void FillSpace(BYTE *str,BYTE width);
void PrePortReading(void);
void PostPortReading(void);
void ClrBelt(BYTE *des,BYTE belt_len);
BYTE DoMeasurement(BYTE iPort,BYTE *des,BYTE des_limit,BYTE speed_mode);
BYTE StartSensors(void);
BYTE SensorIntProc(void);
BYTE IsRxOver(void);
void TriggerPort(BYTE PortIndex);
void ConnectCell(BYTE PortID);
BYTE Inst_Analysis(BYTE connection_type);
//BYTE OutputBuf(BYTE des,BYTE *buff0,BYTE len);	//des. is define in channel object
UINT FormatUltrashort(BYTE *buff0,BYTE len);
UINT FormatSM(BYTE *buff0,BYTE len);
UINT FormatPSTN(BYTE *buff0,BYTE len);
BYTE StripOffComHeader(BYTE connection_type);
BYTE Connect(BYTE connection_type);
void Com1Proc(void);
void Com2Proc(void);
BYTE RingReport(BYTE COMi);
void DirectAccess(void);
BYTE RxPinLocked(void);
void WaitSensorEcho(BYTE maxDelay);
void Com_ConnectionMode(BYTE channel);
void ChargeManage(void);
void CheckCfgValidation(void);
void GoSleep(void);
void SystemGuard(void);
void StartScreen(void);
void ReadRecordBelt(void);
void MarkSensorRequest(BYTE *des);
BYTE LoadCfgFrom(UINT uSrc);
BYTE CheckCfg(BYTE *p);

//ReadPort.c
BYTE ReadPort(BYTE iC0,BYTE *des,BYTE speed_mode);
BYTE ReadSerialPort23(BYTE iC0,BYTE *des,BYTE des_limit,BYTE speed_mode);
BYTE IsC0Alive(UINT c0,PORT *bltC0);
void SetC0Dead(PORT *bltC0);
void KillC0s(void);

//Reports.c
BYTE *CMC(BYTE offset);
BYTE *CMX(BYTE offset);
void IntReports(void);
BYTE *FC_Proc(TMISSION *q,BYTE *p);		//,BYTE fTFIT);	//p:指向起始点
BYTE IffcRepeated(TMISSION *fit,TMISSION *q,BYTE fc);
void GetINTC(BYTE i,BYTE j,BYTE *str);
BYTE *TimeFit(BYTE *p,BYTE *fit);
BYTE DoReport(BYTE fc,UINT property);
BYTE *GetFC(BYTE *p,BYTE *fc);
BYTE GetSW(BYTE chan_id);
BYTE *GetComStr(BYTE comtsk_id,BYTE *cmfg);
BYTE *GetStrCHAN(BYTE COMi);
BYTE GetComResp(BYTE com_type,BYTE com_port);
void GetIpGroup(char *des,BYTE *src);
BYTE *ConvertIpStr(char *des,BYTE *src);
BYTE getChannel(BYTE tsk_id);
BYTE DoGPRM(BYTE *buf,UINT lenMsg,BYTE *cmfg);
BYTE DoBEIDOU(BYTE *buf,BYTE lenMsg,BYTE *cmfg);
BYTE DoSM(BYTE *buf,UINT lenMsg,BYTE *cmfg);
BYTE DoDIRECT(BYTE *buf,UINT lenMsg,BYTE *cmfg);
BYTE DoUsWave(BYTE *buf,UINT lenMsg,BYTE *cmfg);
BYTE XModemRet(BYTE *stat);
void DelayAfterComm(BYTE typeComm);

//MakeMsg.c
UINT MakeMsg(BYTE protocol_id,BYTE fmt_id,BYTE *belt,UINT property);
BYTE MakeTextMsg(const char *sText,BYTE *msg,UINT property);

//Comm232.c--ok
void Serial_End_Tx(void);
void RS232_on(void);
void RS232_off(void);
BYTE TRMT(void);
BYTE URXDA(void);
BYTE URXREG(void);
BYTE RX_IE(void);
void RX_IE_ON(void);
void RX_IE_OFF(void);
BYTE TX_IE(void);
void TX_IE_ON(void);
void TX_IE_OFF(void);
BYTE RX_IF(void);
void CLR_RX_IF(void);
void CLR_TX_IF(void);
BYTE IsComOpen(void);
void OpenComPort(void);
void CloseComPort(void);
void SetComTask(void);
BYTE *RX_BUF0(void);
BYTE *RX_BUF_END(void);
BYTE *getRxPtr(void);
void Set_RxPtr(BYTE *rx);
void IncPtrRx(void);
void Serial_Transmit(BYTE x);
void ClrRxBuf(void);
void ClrUErr(void);
void Serial_Setup(ULNG Baudrate);
void SerialSend(char r);
UINT InitRx(UINT maxDelayMS,BYTE Filter);
void InitTx(BYTE *BufTx);
BYTE IsModemConnected(void);
BYTE Get_AT_Response(BYTE wait_time);
BYTE IsRingIn(void);
BYTE GetComType(BYTE *sComTask);
void ModemRstIfNeeded(void);
void ShowModemVersion(void);
BYTE DoReceive(UINT EndMs);
void AT_creg(BYTE COMi);
void IsTransparantOver(void);
void InitCom(void);
BYTE InitComTask(BYTE *cmfg);
BYTE CopyParaStr(BYTE *p,BYTE *cmfg,BYTE cSign);
BYTE InitAllComTasks();
BYTE InitModem_GPRM(BYTE channel,BYTE *para);
BYTE InitModem_GPRS(BYTE channel);
BYTE InitModem_WIFI(BYTE channel,BYTE *para);
BYTE DetectSetBaud(void);
BYTE getIp(BYTE *des,BYTE *src);
BYTE getPort(BYTE *des,BYTE *src);
BYTE *NextTask(BYTE *pCmfg);
void ComMaintenance(BYTE ComSW);
void HeartBeat(BYTE COMi);
BYTE IPL_URX_INT(void);

//In_Sys.c
BYTE BeltBeRead(BYTE iC0,BYTE *des);
void BitMask(BYTE bits,BYTE *src);
BYTE *beltRain(BYTE *b);
BYTE *beltEncoder(BYTE *b);
BYTE DoCompact(BYTE *des,BYTE bits,BYTE *src);
BYTE Val2BELT(BYTE *des,BYTE *src,UINT cx);

//RECORD.C--ok
BYTE InitRecord(void);
BYTE MarkBF(UINT entry);
BYTE NextRecordTime(BYTE *now,BYTE *cx_cfg);
void AdjustDT(BYTE unit,BYTE interval,BYTE offset);
BYTE SaveNextRecTime(UINT time_ptr);
char DoRecord(BYTE *cx_val,hTIME *time_stamp,BYTE iC0,BYTE iCx);
BYTE WriteRecBlock(BYTE *src,UINT des,BYTE len,UINT top,UINT bottom);
BYTE CheckSucessive(UINT uAddr,hTIME *now,BYTE iC0,BYTE iCx);			
BYTE IsRecTimeValid(BYTE *now,UINT ptr_rec_time,BYTE *cx_cfg);
BYTE IsTimeValid(BYTE *pTime);
void Reverse(BYTE *buf,BYTE len);
BYTE CompareTime(BYTE *timeX,BYTE *timeY);
//BYTE EraseNextSector(UINT uDesPage);
char SaveRecPtr(UINT uAddr,UINT addr_rec_ptr);
UINT RecordRecovery(UINT addr_rec_ptr,UINT top,UINT bottom);
BYTE IsRecTimeMatched(hTIME *date_time,BYTE iC0,BYTE iCx);
char AppendNewRec(BYTE *belt,UINT des,hTIME *time_stamp,BYTE iC0,BYTE iCx);
char SaveARec(BYTE *buf,UINT des,BYTE start,hTIME *time_stamp,BYTE iC0,BYTE iCx);
char RepeatInc_Save(UINT uRepeat,UINT uAddr,hTIME *time_stamp,BYTE iC0,BYTE iCx);
BYTE IsValEqual(BYTE *p,BYTE *val,BYTE len);
UINT SkipRepeatToVal(BYTE **rec,UINT uAddr,BYTE iC0,BYTE iCx);
UINT getRecPtr(BYTE iC0,BYTE iCx);
void InitCardRecord(void);
UINT getRecPtrAddr(BYTE iC0,BYTE iCx);

//REGIST.C--ok
BYTE WR_proc(BYTE *inst);
BYTE SaveMap(BYTE *inst);
BYTE SaveBlock(BYTE *inst);
BYTE SaveSegs(BYTE *inst);		
BYTE UpLoadMap(BYTE connection_type);		
long GetStartAddr(BYTE *inst,int page_size);
UINT GetPageSize(BYTE *inst,UINT Pages,BYTE PageUnit);
UINT PageSize(BYTE PageUnit);
BYTE TREG_Authentication(BYTE *inst,BYTE connection_type);
BYTE TransmitPage(BYTE MapType,ULNG addr0,int page_size,BYTE *inst);
void TranmitHead(UINT MapSize,BYTE *inst);
BYTE Authen(BYTE *inst,BYTE connection_type);		
BYTE GetResp(BYTE ComType);
void Ack(BYTE connection_type);
BYTE DownLoadMap(BYTE connection_type,BYTE *inst);		
BYTE PostDownload(BYTE *inst);
BYTE SaveGeneralCellInfo(BYTE all_segments);
BYTE C0_BeltLen(BYTE *offset,BYTE Cellxs);
BYTE SaveBeltLenOfC0(void);
BYTE SaveNumOfRecCX(void);
BYTE ReadIndexOfRecCx(BYTE iCx);
BYTE CheckSetupAuthority(BYTE connection_type,BYTE *inst);
BYTE DoCfgCRC(BYTE *inst);
BYTE LoadCfgIfOk(UINT uSrc);
BYTE SaveCfg(BYTE *pCfg,UINT uDes);
BYTE *SkipParaStr(BYTE *inst,BYTE *p);


//GsmSm.c--ok
void SM_process(void);
void SendBackSM(BYTE *sm_index);
BYTE *Merge2Bytes(UINT LimitMS);
char IsStrInStream(BYTE cmp_start,const char *sCmp,UINT LimitMS);		
BYTE Get_tel_number(BYTE *tel_num);
BYTE GetSM(void);
char Decode_SM_To_src(BYTE sm_len);
BYTE Get_sm_index(BYTE *sm_index);
BYTE CheckSM(BYTE *sm_index);
BYTE IsMobileQuerying(void);
void DeleteSM(BYTE *sm_index);
char Send_AT_CMGS(BYTE *tel);
BYTE Reserve_6_bits(BYTE *p,BYTE len);
void BinaryVisible(BYTE *des,BYTE *src,BYTE len);
void SendAT( const char *cmd,BYTE end);
void RepeatSM(BYTE *sm);
BYTE MobileQueryResponse(BYTE inst_type,BYTE *tel);
BYTE TransmitTitle(BYTE len,const char *hz_title,BYTE title_len);
BYTE TransmitValue(BYTE *buf,BYTE cx_index,UINT addr,BYTE len,BYTE cx_offset);
BYTE TransmitId(BYTE len);
BYTE TransmitCSQ(BYTE len);
BYTE TransmitVoltage(BYTE len);
BYTE TransmitHZdigits(BYTE len,BYTE *pCxValue);
BYTE TransmitMQuery_head(BYTE len,BYTE *tel);
BYTE Transmit(BYTE date_or_time,BYTE len,BYTE *src);
BYTE EndSM(void);
BYTE *Make_SM_Head(BYTE *tel,BYTE *pdu_head,BYTE *pTailStart);
BYTE TransmitPARA(BYTE *tel);
void SM_ReportError(BYTE *err_msg,UINT max_len);
UINT getCSQ(BYTE channel);
UINT CSQ_to_UINT(BYTE *csq);

//GPRS.C--ok
BYTE GPRS_connection(void);
BYTE GPRS_transmit(BYTE *p,UINT len);
BYTE GetGprsResp(void);
BYTE CheckGprsResp(BYTE *p);
void SendIp(BYTE com_index);
void Send_otcp(void);
void Send_tcptxdelay(void);
void Send_tcpport(void);
BYTE ConnectTcpServer(BYTE *ip,BYTE *port);
BYTE SetGprsPara(void);
BYTE GPRS_end(void);
BYTE GPRM_end(void);
BYTE DIRECT_end(void);
void TransmitIpStr(BYTE com_index);
void TransmitComStr(BYTE com_index);
BYTE ComTransparant(void);

//LCD.c--ok
void PW_LCD_on(void);
void PW_LCD_off(void);
void lcd_init(void);
void lcd_clear(void);			//清屏
void lcd_sleep(void);			//省电模式
void lcd_up(void);				//从省电模式中转到正常模式
void blight_on(void);			//背光开
void blight_off(void);			//背光关
void cursor_on();
void cursor_off();
void cursor_on1(unsigned char x,unsigned char y);//闪烁开，x 第几列，y第几行
void cursor_off1(unsigned char x,unsigned char y);//闪烁关闭，x 第几列，y第几行
//void lcd_cursor(char x,char y);
//void lcd_puts(const char *str);
//void lcd_puts_ram(BYTE *str);
void lcd_puts(char x,char y,const char *str);
BYTE lcd_ack(void);
BYTE lcd_stable(void);
//void Scroll(const char *msg);

//sub.c--ok
BYTE Val(BYTE *p,BYTE *len);
BYTE X(BYTE c);
ULNG getULNG(BYTE *p);
void Nops(BYTE x);
void btoa(signed char b,char *dStr);
void Serial_TransmitHL(BYTE c);
UINT GetCRC(BYTE *p,int len);
BYTE CheckCRC(BYTE *p,int len);
void Delay_s(BYTE x);
void Delay_ms(UINT x);
void delay_us(UINT x);
void pureDelay_ms(UINT x);
BYTE GetMinute(void);
void FillBufWith(BYTE *pStart,UINT len,BYTE cFill);
BYTE HexHiByte(BYTE b);     //b=0x78----> return '7'
BYTE HexLoByte(BYTE b);     //b=0x78----> return '8'    
void BinOfByte(BYTE c,BYTE *str);	//c=0x35----> 00110101,0x0
void DataCpy(BYTE *des,BYTE *src,BYTE len);
BYTE IsDataSame(BYTE *des,BYTE *src,BYTE len);
void DataCpyrom2ram(BYTE *des,const BYTE *src,BYTE len);
void TimeStampCpy(BYTE *des,hTIME *time_stamp,BYTE len);
float ResToFloat(BYTE res);
char *FloatToStr(char *s,float f0,BYTE fmt);
BYTE *Val2Str(BYTE *pVal,BYTE *sOutput,BYTE *cx);
BYTE Seg2Val(BYTE *pVal,BYTE *pSeg,BYTE *cx);
//BYTE RestoreBytes(float *f,BYTE *val,BYTE len);
void BELT2Bytes(BYTE *des,BYTE *src);
//long BytesToLong(long lng,BYTE *pVal,BYTE len,BYTE *sign);
BYTE *str_to_ram(BYTE *dest,const BYTE *src);
UINT getC0(BYTE iC0);
UINT getCX(BYTE iC0,BYTE iCx);
BYTE IsTimeUp(UINT iCmp);
BYTE *GetDefaultValue(BYTE *p);
BYTE *InStr(BYTE *string,const char *s,UINT limit);
BYTE *InStrChar(BYTE *string,BYTE c,UINT lenString);
BYTE ValueHex(BYTE c);
BYTE ValueHex2(BYTE *p);
BYTE WriteCfg(BYTE val,UINT uAddr);
void MsgBox(const char *msg);
UINT AddressOfCxBegin(BYTE iCx);
BYTE GetByte(BYTE Slave,UINT src_addr);
void SaveVersionToCfg(void);
void TransmitStr(BYTE *p);
void ResetPeri(BYTE sec);
BYTE IsSensorReady(void);		
void ResetSensor(void);
void ReadCount(UINT uMap,UINT *count);
void WriteCount(UINT uMap,UINT count);
void IncCount(UINT uMap);
BYTE ClrByte(BYTE Slave,UINT src_addr);
BYTE ClrCfgBit(UINT addr,BYTE ibit);
BYTE SetCfgBit(UINT addr,BYTE ibit);
void Restart(BYTE RstCode);
int Abs(int x,int y);
BYTE *FindParaStr(BYTE *pXSTR,const char *label);
BYTE Seg2Float(float *f,BYTE *seg,BYTE *cx);
BYTE getSLU(BYTE *inst,ULNG *Start,UINT *Length,BYTE *Unit);
BYTE Txt2Bin(BYTE *bin,BYTE *txt,BYTE lenBin);
BYTE Split(const char *Str,BYTE *miniStr);
ULNG Module(BYTE bits);		//获取bits对应的模（最大值）
void DeCompactBelt(BYTE *des,BYTE *src,BYTE cx_bits);

//DispKey.c--ok
void BlinkOff(void);
void CursorBlink(void);
//BYTE Editor(BYTE iEditScreen);
BYTE FindDown(SCREEN *iSrn);
BYTE FindRight(SCREEN *iSrn);
void DigitToggle(SCREEN *iSrn);
void DoEditOK(void);
void DoEditCancel(void);
void DoEditFunc(SCREEN *iSrn);
BYTE getKey(void);
//void DoFunc(SCREEN *iSrn,BYTE x,BYTE y);
void MarkFunc(SCREEN *iSrn,BYTE x,BYTE y);
BYTE InitCurXY(SCREEN *iSrn);
void Blink(UINT ms);
void StartKeyAD(void);
void InitDsp(void);
//BYTE ReadKey(void);
//BYTE GetKey(void);		//Read key twice to confirm the key
void KeyProc(void);
void Write_DspBuf(BYTE iSet,SCREEN *iSrn);
void LCDShow(void);
SCREEN *DoKeyFunc(SCREEN *iSrn,BYTE key);
void GetCurDateTime(char *p);
void RealValue(BYTE *pDes,BYTE *pSrc,BYTE *type,UINT top,UINT bottom);		  
//void FormatDateTime(hTIME dt,char *fs);
void FormatDateTime(BYTE Offset,BYTE Len,hTIME dt,char *fs);
void Int2DoubleStr(char *sOutput,UINT *uInput);
BYTE Seg2Str(BYTE *sInput,BYTE *sOutput,BYTE *cx);
void bracket(BYTE *line);
void DspBlankData(BYTE *p);
void FormatStr(BYTE *str,BYTE length);
BYTE Refresh_bufLCD_Cx(SCREEN *iSrn,BYTE *pf);
void Show_bufLCD(BYTE iScreen,BYTE curL);
void StartKeyPoll(void);
void EndKeyPoll(void);
void Scroll(BYTE way,const char *msg);
void Editor(BYTE iEScreen);

//setup.c--ok
BYTE EnterSetup(void);
void Setup_Top(void);
void Setup_Mainboard(BYTE iItem,BYTE *numItem);
BYTE Setup_SensorIn(BYTE iPort,BYTE iItem,BYTE *numItem,BYTE *des);
BYTE Setup_SensorEx(BYTE iPort,BYTE iItem,BYTE *numItem,BYTE *des,BYTE speed_mode);
BYTE PortItemMenu(BYTE iPort,BYTE iItem,BYTE *numItem,BYTE ports,BYTE *fItemSaved);
BYTE ReadPortSetup(BYTE iPort,BYTE iItem,BYTE *numItem,BYTE *des);
void AssignDefault(BYTE *p,BYTE *pDefault,BYTE len);
void ShowPortMenu(void);
void lcd_printf(BYTE *p);
BYTE *DefaultValue(BYTE *p);		
void putC(BYTE c);
void putCRLF(void);
void GetCursor(BYTE *p);
void FindCursorBack(BYTE *p);
void CursorTab(BYTE x,BYTE y);
void CharStamp(BYTE x,BYTE y);
void CursorRight(void);
void CursorSel(void);
void ShowLine(BYTE line);
void Cursor_blink(BYTE x,BYTE y);
BYTE *CursorLastTab(void);
BYTE LenToCursor(void);
BYTE WritePortSetup(BYTE iPort,BYTE iItem,BYTE Ports,BYTE speed_mode);
void WriteMainPort(BYTE iItem);
BYTE GetDigit(BYTE *p);
void GetNodeAddress(char *pID);
BYTE SaveNodeAddress(BYTE *p);		//,BYTE node_addr);
void GetUDiskDays(char *pDays);

//SetupIn.c
//BYTE Setup_SensorIn(BYTE iPort,BYTE iItem,BYTE *numItem,BYTE *des);
BYTE SavePort1Cfg(BYTE iItem);
BYTE LoadSensorSwitch(BYTE *p,BYTE iSensor);
BYTE CheckDigits(UINT addr,BYTE *array,BYTE len);
BYTE LoadRainAlarm(BYTE *p);
BYTE LoadLevelAlarm(BYTE *p,UINT addrLevel);
BYTE LoadSlopeCo(BYTE *p,UINT addr_co);
BYTE LoadEncodePolarity(BYTE *p);
BYTE SaveSensorSwitch(BYTE *p,BYTE iSensor);
BYTE SaveRainAlarm(BYTE *p,UINT addr_rain );
BYTE SaveLevelAlarm(BYTE *p,UINT addrLevel);
BYTE SaveSlopeCo(BYTE *p,UINT addr_co);
BYTE SaveEncodePolarity(BYTE *p);

//SerialSensor.c
BYTE RWSerialSensor(BYTE *inst,BYTE len0,BYTE *resp,UINT resp_limit,BYTE speed_mode);
void TransmitBytes(BYTE *buff0,BYTE len0);
BYTE RxFromSensor(UINT LimitMS);		

//USB_DISK.c-ok
void UARTSendPkt(char *p,int len);
void UARTSendByte(char *p);
void GetFileName(BYTE *filename);
BYTE SetHostBaudrate(void);
BYTE OpenUSBDisk(void);
BYTE CloseDisk(void);
char CreatFile(BYTE *filename);
char GetUsbDiskResp(void);
void WriteFile(UINT len);
BYTE CloseFile(void);
char ExportAllRecords(void);
UINT ExportCxRec(BYTE Slave,UINT addr_rec_ptr,UINT recTOP,UINT recBOTTOM);
void ProgressBar(void);
UINT ExportOnePage(BYTE Slave,UINT addrRec,UINT addrEnd,BYTE mark_end);
void SaveToUsbDisk(void);
BYTE WriteTitle(BYTE type,UINT cx);
UINT GetUFileSegTitle(BYTE type,UINT cx,BYTE *title);
char ExportCfg(void);
BYTE IsPageEmpty(UINT xmPage);
BYTE LoadxmPageToFRamBuf(UINT xmPage,UINT buf_start);

//RecordExtract.c
void RecordExtract(void);
BYTE AddTime(BYTE unit,UINT uRepeat,char interval,BYTE sub_offset);

//spi.c
void spi_WriteByte(BYTE output);
BYTE spi_ReadByte(void);
void xm_SectorErase(ULNG addr);
void WaitMemReady(char Slave);
void mem_WriteEnable(char Slave);
void mem_WriteDisable(char Slave);
BYTE mem_ReadStatus(char Slave);
void mem_SectorErase(char Slave, long addr);
void mem_ReadData(char Slave,ULNG src_addr,BYTE *des,int len);
void mem_WriteData(char Slave,BYTE *src,ULNG des_addr,int len);
BYTE mem_CheckWriting(char Slave,BYTE *data,ULNG addr,int len);
void cs(char Slave);
void cd(void);
void WaitXmReadyForErase(void);
BYTE CheckMirrorCRC(int len);
void FramLock(char Slave);
void FramUnlock(char Slave);
void ReadRecArray(UINT uAddr,BYTE *buf,UINT RecLen,UINT recTop,UINT recBottom);

//xMem_access.c
UINT xSaveStartCx(UINT uPtrFixed,BYTE ValLen,UINT top,UINT bottom);
UINT xSaveEndCx(UINT uPtrMoving,BYTE ValLen,UINT top,UINT bottom);
BYTE EnoughCxRecToXSave(UINT moving,UINT fixed,UINT top,UINT bottom);

//power.c
BYTE VoltageNow(void);
void PowerTrap(void);
BYTE CheckVoltage(BYTE v10);
void DspVoltageWarning(BYTE v10,const char *msg);
int isWDTWU(void);
void PowerRestore(void);

//calendar.c
void SaveTimeTo(hTIME dt,hTIME *saveTime);
void RestoreTimeFrom(hTIME *saveTime,hTIME *dt);
void InitCalendar(void);
void CheckCalendar(void);
BYTE IsSpaned(hTIME rT);
char Now(hTIME *dt);
void WriteCalendar(hTIME dt);
BYTE DayEnd(BYTE month,BYTE year);
void AddMinute(hTIME *dt,char min_interval);
void AddHour(hTIME *dt,char hour_interval);
void AddDay(hTIME *dt,char day_interval);
void AddInterval(hTIME *dt,BYTE unit,char interval,BYTE hour_offset);
BYTE UpdateCalendar(hTIME dt);
void CleanDateTime(hTIME *dt,BYTE *p);
BYTE CalendarAutoAdjust(BYTE *inst_calendar);
void CalendarProc(void);
void end_CalendarProc(void);
BYTE RecordBelts(hTIME *rDateTime);

//beidou.c
BYTE BEIDOU_Transmit(BYTE *buf,BYTE lenMsg,BYTE *cmfg);
BYTE BEIDOU_Chk(BYTE *p,BYTE len);
void BEIDOU_TimeAutoAdjust(void);
BYTE BEIDOU_GetTime(BYTE *cal);
BYTE BEIDOU_ok(void);
BYTE Is_BEIDOU_Ready(void);
UINT getBDQ(BYTE channel);
UINT BDQ_to_UINT(BYTE *bdq);

//wifi.c
char WifiConnection(BYTE *ip_port,BYTE *psw);

//BlockTrans.c
BYTE MakeMapMsg(BYTE MapType,UINT xmPage);
void TransmitCxBlock(BYTE iCx,BYTE CxBlockLen,UINT recTop,UINT recBottom);
void TranmitAllBlocks(void);

int ReadTemp(void);


#endif
