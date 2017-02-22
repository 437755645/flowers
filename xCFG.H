//===============================
//       主元 definition
//===============================
//基本域
#define 	CELL_MARK                0x000       //域标识
#define 	FLD_LEN                  0x001       //域长度
#define 	FLD_INDEX                0x002       //域配置顺序
#define 	FLD_VER                  0x004       //数据结构版本号
#define 	CM_STRUCT_VER            0x005       //数据结构总版本
#define 	CM_C0S                   0x006       //基元数
#define 	CM_CXS                   0x007       //虚元数
#define 	CM_LEN_CFG               0x008       //CFG长度*
#define 	CM_STA_ID                0x009       //主机地址
#define 	CM_NAME                  0x00D       //主机名称
#define 	CM_VER_SOFTWARE          0x01D       //软件版本
#define 	CM_SPAN                  0x020       //最小记录间隔*
#define 	bitCM_MIN_REC_GAP        0           //记录间隔
#define 	bitCM_MIN_REC_UINT       6           //记录间隔单位
#define 	CM_PSW                   0x021       //注册密码
#define 	CM_CFG_INDEX             0x024       //CfgIndex
#define 	CM_C0_START              0x026       //基元偏移首地址
#define 	CM_YN1                   0x027       //YN1
#define 	bitSENSOR_FLAG_PROCESS   0           //传感器中断标记处理
#define 	bitLCD_SW                1           //LCD显示开关
#define 	bitLCD_LIGHT_SW          2           //背光是否始终打开
#define 	bitSCREEN_START          3           //首页显示内容
#define 	bitZIPPED                4           //是否引用虚元*
#define 	CM_YN2                   0x028       //YN2
#define 	bitCALENDAR_EXIST        0           //是否有日历时钟
#define 	bitCAL_AUTO              1           //是否开启日历自动校准
#define 	bitREPEATER              2           //是否有中继功能
#define 	bitREC_COMPRESS          3           //REC记录是否压缩
#define 	bitLOG_ON                4           //是否开启log日志
#define 	CM_CFG_YEAR              0x029       //CFG最后修改时间－年
#define 	CM_CFG_MONTH             0x02A       //CFG最后修改时间－月
#define 	CM_CFG_DAY               0x02B       //CFG最后修改时间－日
#define 	CM_CFG_HOUR              0x02C       //CFG最后修改时间－时
#define 	CM_CFG_MINUTE            0x02D       //CFG最后修改时间－分
#define 	CM_CFG_SECOND            0x02E       //CFG最后修改时间－秒
#define 	CM_COM0                  0x02F       //通信域入口*
#define 	CM_CFG_CRC               0x03E       //CFG_CRC*
#define 	CM_LINK                  0x040       //主元链接

//扩展域
#define 	CMX_YN                   0x005       //移动存储YN
#define 	bitSD_CARD               0           //是否启动扩展移动存储
#define 	bitSD_FMT_INHIBIT        1           //禁止手动移动存储格式化
#define 	bitSD_ENCRYPTION         2           //是否开启移动存储加密
#define 	CMX_MR_YEAR              0x006       //移动存储记录起始-年
#define 	CMX_MR_MONTH             0x007       //移动存储记录起始-月
#define 	CMX_MR_DAY               0x008       //移动存储记录起始-日
#define 	CMX_MR_HOUR              0x009       //移动存储记录起始-时
#define 	CMX_MR_MINUTE            0x00A       //移动存储记录起始-分
#define 	CMX_MR_SECOND            0x00B       //移动存储记录起始-秒
#define 	CMX_LINK                 0x040       //DISP

//通信域
#define 	CMC_CHANNELS             0x006       //通信通道数*
#define 	CMC_BCFG_LEN             0x007       //BCFG字符串长度*
#define 	CMC_CMFG_LEN             0x009       //CMFG字符串长度*
#define 	CMC_YN                   0x00B       //通信属性YN
#define 	bitSW_REPORT             0           //通信总开关
#define 	bitINSTANT_RPT_ENABLED   1           //即时通信允许
#define 	bitREPEAT_ENABLED        2           //中继允许
#define 	CMC_SM_YN                0x00C       //短信属性YN
#define 	bitSM_ON_HOUR            0           //短信仅在整点允许
#define 	bitREPEAT_DIRTY          1           //收到不可识别短信时上报
#define 	CMC_MONITOR              0x00D       //监视通信
#define 	bitMON_NO                0           //通信方式编号
#define 	bitMON_R                 3           //R
#define 	bitMON_SW                7           //开关
#define 	CMC_LINK                 0x040       //通信链接

//===============================
//       基元 definition
//===============================
#define 	C0_CXS                   0x005       //虚元数
#define 	C0_NEXT_START            0x006       //下一个基元首地址*
#define 	C0_NAME                  0x007       //基元名称
#define 	C0_VER_SOFTWARE          0x017       //基元软件版本
#define 	C0_SPAN                  0x01A       //记录间隔
#define 	C0_MIN_INTERVAL          0           //最小记录间隔*
#define 	C0_UNIT                  6           //间隔单位*
#define 	C0_REFER                 0x01B       //引用
#define 	C0_BELT_LEN              0x01C       //基元belt长度*
#define 	C0_ALIVE_MS              0x01D       //基元belt寿命
#define 	C0_PORT_NUM              0x01E       //端口数
#define 	C0_YN1                   0x01F       //基元YN1
#define 	bitCALENDAR_EXIST        0           //有无日历时钟
#define 	bitC0_ENABLED            1           //本基元是否启用
#define 	bitCAL_AUTO_ADJUST       2           //时钟是否允许更新
#define 	bitC0_REC_ENABLED        3           //是否记录*
#define 	bitAS_REPEATER           4           //中继是否允许
#define 	bitC0_TRIG               5           //是否存在触发*
#define 	C0_YN2                   0x020       //基元YN2
#define 	bitINSTANT_COM           0           //Instant通信允许
#define 	bitBUS_COM               1           //Bus通信允许
#define 	bitC0_IN_MLOOP           2           //是否加入主进程
#define 	C0_MAX_DELAY             0x022       //最大延时*
#define 	C0_REC_PTR               0x023       //起始记录指针*
#define 	C0_LINK                  0x040       //Co链接

//===============================
//       虚元 definition
//===============================
#define 	CX_INDEX                 0x005       //虚元基元号*
#define 	CX_NAME                  0x007       //虚元名称
#define 	CX_UNIT                  0x017       //单位
#define 	CX_DSP_COL0              0x01F       //数值显示起始列
#define 	CX_BELT_OFFSET           0x020       //虚元BeltOff*
#define 	CX_VAL_BLEN              0x021       //数据B长度*
#define 	CX_BITS                  0x022       //有效bits
#define 	CX_REC_GAP               0x023       //记录间隔及单位
#define 	bitREC_INTERVAL          0           //记录间隔
#define 	bitINTERVAL_UNIT         6           //记录间隔单位
#define 	CX_SUB_GAP_OFFSET        0x024       //记录次间隔偏移
#define 	CX_BELT_TYPE             0x025       //数据元格式
#define 	CX_REC_ENTRY             0x026       //记录文件入口
#define 	CX_REC_EXIT              0x028       //记录文件出口
#define 	CX_REC_PTR_DEL           0x02A       //记录指针
#define 	CX_REC_OUTPUT            0x02C       //显示输出方式
#define 	CX_DSP_FMT               0x02D       //显示格式
#define 	CX_DELAY                 0x02E       //虚元延时
#define 	CX_B                     0x02F       //偏移基数B
#define 	CX_K                     0x033       //修正系数K
#define 	CX_CORRECTION            0x037       //修正方式
#define 	CX_PRECISION             0x038       //数据精度等级
#define 	CX_YN1                   0x039       //虚元YN1
#define 	bitSD_REC                0           //是否参与移动存储
#define 	bitREC                   1           //是否记录*
#define 	bitIF_COMPRESS           2           //记录是否压缩
#define 	bitIN_BELT               3           //是否包含于belt
#define 	bitBUS_COM_CX            4           //是否允许Bus通信
#define 	bitINSTANT_COM_CX        5           //是否允许Instant通信
#define 	bitCX_TRIG               6           //是否存在触发条件
#define 	bitCX_IN_MLOOP           7           //是否加入主进程
#define 	CX_LIFE_SEC              0x03A       //寿命
#define 	CX_LINK                  0x040       //***
