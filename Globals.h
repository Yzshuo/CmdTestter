/******************************************************************************\*
**  Copyright (C), 2016-, Shanghai Jingyu Intelligent Control Co., Ltd.
**  File name:    Globals.h
**  Author:       Version:        Date:
**  Liu Shengju   1.1.1.0         16/..
**  Description:    // 用于详细说明此程序文件完成的主要功能，与其他模块
**                  // 或函数的接口，输出值、取值范围、含义及参数间的控
**                  // 制、顺序、独立或依赖等关系
**  Others:         // 其它内容的说明
**  Function List:  // 主要函数列表，每条记录应包括函数名及功能简要说明
**    1. ....
**  History:        // 修改历史记录列表，每条修改记录应包括修改日期、修改
**                  // 者及修改内容简述
**    1. Date:
**       Author:
**       Modification:
**    2. ...
\*******************************************************************************/


#ifndef _KIN_GLOBAL_PARAMETER_H_
#define _KIN_GLOBAL_PARAMETER_H_

#include <QtCore>
#include "SystemLog.h"
#include<QColor>

//
#define PROCESS_FAIL            -1
#define Assert_Enable			1
#define Assert_Kinbio  {\
    if (Assert_Enable)\
{\
    qDebug() << "Runing: "__FILE__ << __LINE__ << endl; \
    }\
    }


//--------------------------------------模块类型 及 通道定义--------------------------------
typedef enum EModuleTypeTag
{
    MODULE_TYPE_REV         = 0,
    MODULE_TYPE_TEMP_CTRL   = 1,              // 温控模块
    MODULE_TYPE_FLOR_SAMPLE = 2,                // 荧光采集模块
    MODULE_TYPE_SYSTEM_ET   = 3,                  // 代表系统， 热盖、环境温度等等
    MODULE_TYPE_MAX
}E_ModuleType;

typedef enum E_ExperimentStepTag
{
    EXP_STEP_IDLE = 1,
    EXP_STEP_PREPARE,
    EXP_STEP_DNA_AMP,
    EXP_STEP_DNA_MELT,
    EXP_STEP_MAX
}E_ExperimentStep;

// ----------------温控常量-------------------------
typedef enum E_TempFunIDTag
{
    TEMP_FUN_ID_OPEN_CLOSE      = 1,
    TEMP_FUN_ID_STATUS_CFG      = 2,
    TEMP_FUN_ID_TARGET_TEMP_CFG = 3,
    TEMP_FUN_ID_TEMP_UPTO_REPORT= 4,
    TEMP_FUN_ID_MODULE_PAR_CFG  =5,
    TEMP_FUN_ID_EXPERIMENT_PAR_CFG = 6,
    TEMP_FUN_ID_TEMP_REPORT     = 7,
    TEMP_FUN_ID_ERR_REPORT      = 8,
    TEMP_FUN_ID_MAX
}E_TempFunID;

typedef enum E_TempCtrlStatusTag
{
    TEMP_STATUS_DNA_AMP             = 1,
    TEMP_STATUS_DNA_MELT            = 2,
    TEMP_STATUS_TEC_UNABLE_CTRL     = 3,
    TEMP_STATUS_HOT_LID_UNABLE_CTRL = 4,
    TEMP_STATUS_SAMPLE_FLOR_START   = 5,
    TEMP_STATUS_SAMPLE_FLOR_STORP   = 6,
    TEMP_STATUS_MODULE_DEBUG        = 7,
    TEMP_STATUS_MAX
}E_TempCtrlStatus;

typedef enum E_TempModuleParTag
{
    TEMP_MOD_PAR_TEMP_MAKE_UP = 1,
    TEMP_MOD_PAR_TEMP_CTRL_KP = 2,
    TEMP_MOD_PAR_TEMP_CTRL_KI = 3,
    TEMP_MOD_PAR_TEMP_CTRL_KD = 4,
    TEMP_MOD_PAR_SOFTWARE_VER = 5,
    TEMP_MOD_PAR_HARDWARE_VER = 6,
    TEMP_MOD_PAR_PRODUCTION_DATE = 7,
    TEMP_MOD_PAR_MAX
}E_TempModulePar;

typedef enum E_ExperimentTempParTag
{
    EXP_TEMP_PAR_MELT_TYPE  = 1,    // 溶解曲线类型
    EXP_TEMP_PAR_INC_RATE   = 2,     // 升温速率
    EXP_TEMP_PAR_DEC_RATE   = 3,     // 降温速率
    EXP_TEMP_PAR_START_TEMP = 4,   // 溶解起始温度
    EXP_TEMP_PAR_END_TEMP   = 5,     // 溶解终止温度
    EXP_TEMP_PAR_MAX_TEMP   = 6,
    EXP_TEMP_PAR_MAX
}E_ExperimentTempPar;

// ------------------荧光模块命令常量-------------------
typedef enum FluoFunIDTag
{
    FLOR_FUN_ID_RESERVE      = 0, //保留
    FLOR_FUN_ID_START_SAMPLE = 1, //开始采集荧光
    FLOR_FUN_ID_FLOR_REPORT  = 2, //报告荧光值
    FLOR_FUN_ID_ADC_REPORT   = 3, //采样频率
    FLOR_FUN_ID_LED_INT_REPORT = 4, //
    FLOR_FUN_ID_MOD_PAR_CFG  = 5,  //模块参数配置
    FLOR_FUN_ID_ERROR_REPORT = 6,  //错误状态报告
    FLOR_FUN_ID_CONTINUE_NU  = 7,  //连续采样次数
    FLOR_FUN_ID_MOD_TEST     = 8,  //
    FLOR_FUN_ID_MAX//
}FluoFunID;

// 模块参数配置（FLOR_FUN_ID_MOD_PAR_CFG）
typedef enum E_FluoModParCmdTag
{
    FLOR_MOD_PAR_CMD_RESERVE        = 0, //保留
    FLOR_MOD_PAR_CMD_LED_INTENSITY  = 1,
    FLOR_MOD_PAR_CMD_ADC_RATE       = 2,
    FLOR_MOD_PAR_CMD_ADC_NU         = 3,
    FLOR_MOD_PAR_CMD_LED_ON_GAP     = 4,
    FLOR_MOD_PAR_CMD_DELAY_TIME     = 5,
    FLOR_MOD_PAR_CMD_CHANNEL_NO     = 6,
    FLOR_MOD_PAR_CMD_CALCULATE_NU   = 7,
    FLOR_MOD_PAR_SOFTWARE_VER       = 8,
    FLOR_MOD_PAR_HARDWARE_VER       = 9,
    FLOR_MOD_PAR_PRODUCTION_DATE    = 10,
    FLOR_MOD_PAR_CMD_MAX
}E_FluoModParCmd;



// ------------------主板模块命令常量-------------------
typedef enum MainFunIDTag
{
    SYSTEM_CMD_COM_LINK             = 1, //仪器握手
    SYSTEM_CMD_FAN_CTRL             = 2, //风扇控制
    SYSTEM_CMD_REPORT_FAN_STATUS    = 3,
    SYSTEM_CMD_HOT_LID_CTRL         = 4, //热盖控制
    SYSTEM_CMD_REPORT_HOT_LID_STATUS= 5, //热盖状态报告
    SYSTEM_CMD_EXPERIMENT_STEP      = 6,//实验过程通知
    SYSTEM_CMD_SYSTEM_PAR_WR        = 7, // 仪器参数读写
    SYSTEM_CMD_HOT_LID_TEMP_REPORT  = 8, // 热盖温度的读写   调试时使用。
    SYSTEM_CMD_TEMP_COMPENSATE_WR   = 9, // 温度补偿参数读写
    SYSTEM_CMD_ERROR_REPORT         = 10,
    SYSTEM_CMD_MAX
}MainFunID;

// 实验过程的代码如下
typedef enum  E_DnaExperimentStepTag
{
    EXPERIMENT_STEP_STANDBY         = 0,
    EXPERIMENT_STEP_PRE_PROCESS     = 1,//
    EXPERIMENT_STEP_AMPLIFYING      = 2,
    EXPERIMENT_STEP_MELTING         = 3,
    EXPERIMENT_STEP_SAMPLE_FLOR_START = 4,
    EXPERIMENT_STEP_SAMPLE_FLOR_STOP= 5,
    EXPERIMENT_STEP_DEBUG           = 6,
    EXPERIMENT_STEP_MAX
}E_DnaExperimentStep;

// Byte6-错误代码： bit0～bit3为错误子代码， bit4～bit7为模块代码
typedef enum  E_ErrorCodeTag
{
    // 温控板错误
    ERROR_CODE_TEMP_CTRL_MOD_CAN_COM_FAILED     = 0X11,
    ERROR_CODE_TEMP_CTRL_MOD_TEMP_INC_FAILED    = 0X12,
    ERROR_CODE_TEMP_CTRL_MOD_TEMP_DEC_FAILED    = 0X13,
    ERROR_CODE_TEMP_CTRL_MOD_TEMP_INC_SLOWLY    = 0X14,
    ERROR_CODE_TEMP_CTRL_MOD_TEMP_DEC_SLOWLY    = 0X15,
    ERROR_CODE_TEMP_CTRL_MOD_RIPPLE_BIG         = 0X16,
    ERROR_CODE_TEMP_CTRL_MOD_TEMP_SENSE_INVALID = 0X17,

    //  荧光采集模块错误
    ERROR_CODE_FLOR_SAMPLE_MOD_CAN_COM_FAILED   = 0X21,
    ERROR_CODE_FLOR_SAMPLE_MOD_ADC_COM_FAILED   = 0X22,
    ERROR_CODE_FLOR_SAMPLE_MOD_ADC_ERROR        = 0X23,
    ERROR_CODE_FLOR_SAMPLE_MOD_CALCULATE_SLOWLY = 0X24,
    ERROR_CODE_FLOR_SAMPLE_MOD_LED_UNLIGHTED    = 0X25,
    ERROR_CODE_FLOR_SAMPLE_MOD_LED_INTENSITY_LOW= 0X26,
    ERROR_CODE_FLOR_SAMPLE_MOD_LED_SLOWLY       = 0X27,

    // 主板错误
    ERROR_CODE_MAIN_BORAD_CAN_COM_FAILED        = 0X31,
    ERROR_CODE_MAIN_BORAD_UART_COM_FAILED       = 0X32,
    ERROR_CODE_MAIN_BORAD_HOT_LID_INC_FAILED    = 0X33,
    ERROR_CODE_MAIN_BORAD_HOT_LID_CTRL_FAILED   = 0X34,
    ERROR_CODE_MAIN_BORAD_TEMP_SENSER_INVALID   = 0X35,
    ERROR_CODE_MAIN_BORAD_FAN_NO_WORK           = 0X36,
    ERROR_CODE_MAIN_BORAD_SINK_TEMP_SENSER_INVALID = 0X37,
}E_ErrorCode;

//=====================================命令的定义==============================================================
//------------------------------- command main Id---------------------------------------------------
#define CMD_MAIN_ID_DAN_AMP_MELT_METHOD_MAN   0X01
#define CMD_MAIN_ID_EXPERIMENT_MANAGMENT      0X02
#define CMD_MAIN_ID_EXPERIMENT_REVIEW         0X03
#define CMD_MAIN_ID_INSTUMENT_MANAGMENT       0X04
#define CMD_MAIN_ID_DNA_EXTRACT_METHOD_MAN    0X05
#define CMD_MAIN_ID_UART_DATA_PACKET          0X06

//-------------------------------command sub Id----------------------------------------------------
//   Uart data packet communication sub Id
#define UART_DATA_PACKET_CMD_SUB_ID_TX       0X01
#define UART_DATA_PACKET_CMD_SUB_ID_RX       0X02

#define M_READ                                  0
#define M_WRITE                                 1

//数据共用体
union DataContent
{
    quint8  m_uint8data[4];
    quint32 m_uint32data;
    double  mdoubledata;
    float   m_floatdata;
};

// 串口通信协议的包头
typedef struct SUartCommandTag
{    
    quint32 unStartTag      : 8;    //0xFF
    quint32 unDataLen_empty : 8;
    quint32 unDataLen       : 8;
    quint32 unCrc           : 8;

    quint32 unFunId         : 4;
    quint32 unRorW          : 1;
    quint32 unNodeType      : 3;
    quint32 unStatusAck     : 8;   //0请求 1回复
    quint32 unCh            : 4;
    quint32 unModuleType    : 4;
    quint32 unSubCmd        : 8;
}SUartCommand;

typedef union UUartCommandTag
{
    SUartCommand sUartCmd;
    quint8       ucCmd[8];
}UUartCommand;

typedef struct SCmdStructTag
{
    quint32 unCmdMainId   : 8;            // main   cmd
    quint32 unCmdSubId    : 8;            // sub    cmd
    quint32 unCmdSonId    : 8;            // Son    cmd
    quint32 unOtherId     : 8;            //
}SCmdStruct;


typedef struct SRequestCommandInfoTag
{ 
    SCmdStruct			sCmdId;			    // 请求命
    UUartCommand        uUartCmd;
    QString				strCmdContent;		// 数据内容  QString        格式
    QByteArray			baCmdContent;		// 数据内容  QByteArray： 可以是串口上传的荧光值等数据
    quint32             unOtherInfo;        // 通常用来  保存指针，比如PCR扩增的检测的方法
}SRequestCmdInfo;

//static float data_temp_1;
struct Table_Data
{
    int table_num;
    QList<bool> celldata_list;
};

struct CellInfo{
    QString SampleName;//样本名称
    QString ID;//唯一表示符
    QString Name;//患者姓名
    QString Sex;//性别
    QString Age;//年龄
    QString SampleType;//标本类型
    QString SampleDate;//采样日期
    QString DoctorName;//送检医生
    QString DoctorRoom;//送检科室
    QString DoctorID;//病历号
    QString DoctorRemarks;//临床诊断
    QString Remarks;//备注

    //添加
    QString ProperName;//管名
    QString Passage;//通道
    QString DyesTuff;//染料
    QString Target;//目标
    QString Color;//颜色
    QString Type;//类型
    QString Ct;//Ct
    QString AverCt;//平均Ct
    QString CtSD;//Ct SD
    QString CtCv;//Ct CV(%)
    QString Concen;//浓度
    QString AverConcen;//平均浓度
    QString CnSd;//Cn SD
    QString CnCv;//Cn CV(%)
    QString Rn;//Rn
    QString InterRes;//中间结果
    QString Conclu;//结论
    QString BedNumber;//病床号
    QString AdmissNumber;//住院号
    QString OutpatNumber;//门诊号

};

struct CellData{
    QString     Reapore;//反应孔
    QString     CellName;//选择的项目名
    int         SampleType      = 0;//选择样品类型
    QString     SampleAttr;//样品属性
    QString     SampleName;//样品名称
    bool        SampleRepeat    = false;//复管是否选中
    int         SampleRepeatID  = 0;//复管编号
};

struct CellUnit{
    CellData cell_unit_data;
    CellInfo cell_unit_info;
};
#define  ROW    8
#define  COL    6
struct ExperimentData
{
    QString     EquipmentType;//仪器型号
    QString     EquipmentID;//仪器编号
    QString     Exp_Name;//实验名称
    QString     Exp_SaveAddr;//保存路径
    int         Exp_Type        = 0;//实验类型
    QString     Exp_User;//操作人员
    QString     Exp_UserCheck;//审核人员
    QString     Exp_Remarks;//备注
    int         Exp_Modules     = 0;//反应模块是模块A还是B
    bool        Exp_Hotlid      = false;//是否选择热盖
    int         Exp_TemControl  = 0;//模块控温还是试管控温
    int         Exp_ChannelScan = 0;//扫描通道设置
    CellUnit    Exp_Table[(ROW*COL)];
};
//-----------------------------------------工程使用结构体定义------------------------------------------------------------//
//选择的试管结构体
typedef struct Test_Tube
{
    QList<int> Passageway;//选择的通道
    QList<QString> Passageway_Name;//通道名称
    QString  TestTube_Name; //试管名
}Q268PCR_TEST_TUBE;
//实验参数结构体
typedef struct Exper_Para
{
    QString Passageway_Name;//通道名称
    QString  TestTube_Name; //试管名
    QString Passageway;//选择的通道号
    QString Anatype;//分析类型
    int Linestart;//基线起点
    int Lineend;//基线终点
    QString Lineopti;//基线优化
    bool Autovalue;//自动阈值
    double Manualvalue;//手动阈值
    QString Numberwave;//数字滤波
}Q268PCR_EXPER_PARA;
//界面判定界面基本规则界面数据结构体
typedef struct Resue_Dete
{
    QString SampeType;//样品类型
    QString CheckObject;//检测目标
    int     ObjectVale;//数值(目标)
    QString ObjectCompara;//比较符(目标)
    QString DataType;//数据类型
    QString DataCompara;//比较符(数据类型)
    int     DataVale;//数值(数据类型)
   // bool    ResueDete;//是否勾选结果判定
    QString Conclusion;//结论
    QColor  Color;//颜色
}Q268PCR_RESUE_DETE;
typedef struct Senior_Rule
{
   // bool    SeniorRule;//是否勾选高级规则
    QString Path;//选用的高级规则文件路径
    QString Interpre;//结果和解释

}Q268PCR_SENIOR_RULE;


typedef  struct Field_Sett
{
    bool ItemCheck;
    QString ItemName;

}Q268PCR_FIELD_SETTING;

typedef  struct Tool_Config
{

   //排序方式
    int Sortorder;
   //数据选择方式
    int Dataselect;
   //表格缩放
    int Tablezoom;
   //显示样本间隔线
    bool Sampleinter;
   //浓度值以科学计数法显示
    bool Sciencoun;
   //Y轴显示方式
    int Ydispalymode;
   //曲线样式
    int Curvestyle;
   //扩增曲线算法
    int Amplcurvealgor;
   //选中的样本显示到标准曲线上
    bool Sampledisplaycurve;
   //扩增曲线图中显示阀值线
    bool  Amplcurvethres;
   //在峰值曲线上显示峰值点
    bool Peakvalue;
   //峰型倒置
    bool Peakinvwe;
   //多管实验反应孔选择方式
    int Porechoose;
    //允许Ctrl+单孔选择
    bool Allowhole;
   //通道选择方式
    int Passagechoose;
   //热盖设置
    int Hotsetting;
   //数据联动显示
    bool Datadisplay;
   //界面显示,分析参数表
    bool Uiparameter;
   //自定义孔板
    bool Customhole;
   //自定义导出实验数据
    bool Customexperdata;
   //自动导出选项
    int Autoexporyop;
   //导出文件格式
    QString Fileformat;




}Q268PCR_TOOL_CONFIG;

#endif /* _KIN_GLOBAL_PARAMETER_H_  */

/*----------------------------------------- (C) COPYRIGHT 2011  ---END OF FILE----------------------------------*/

