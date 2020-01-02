#ifndef _GLOABLE_
#define _GLOABLE_
#pragma once
#ifndef _JSON_HPP_H_ 

#define _JSON_HPP_H_ 
#include "json.hpp"
#include "csv.hpp"
#endif

namespace Gloable_Bes{


// 我门期望 所有的 define 参数都可用从配置文件中进行修改

#pragma pack(1)
#define MAX_ELECTRIC_COUNT  100      //电表参数个数
#define MAX_ELECTRIC_PARAM_COUNT  10 // 电能参数 个数 因为这个个数是硬件写死的,所以要在这里修改，后面可用改成可用配置的
#define MAX_STORE_HOUR_COUNT 24



/*BES200函数错误码定义*/
enum Bes200ErrorCode{
	BES200_ERROR_OK = 0,			//无错误，正常返回
	BES200_ERROR_CONNECTFAILED,	    //与服务器建立连接失败
	BES200_ERROR_SENDFAILED,		//数据发送失败
	BES200_ERROR_SENDTIMEOUT,		//发送超时
	BES200_ERROR_RECEIVEFAILED,		//数据接收失败
	BES200_ERROR_RECEIVETIMEOUT,    //接收超时
	BES200_ERROR_REFUSED,			//请求被服务器拒绝
	BES200_ERROR_MALLOCERROR,		//申请空间错误
	BES200_ERROR_SOCKETFAILED,		//申请socket错误
	BES200_ERROR_CRCERROR,			//校验不对
	BES200_ERROR_METERIDEXISTED,	//电表ID已经存在
	BES200_ERROR_MAXMETERCOUNT,		//电表数目达到上限
	BES200_ERROR_METERID_NOTFOUND,	//电表ID不存在
	BES200_ERROR_DATASIZEERR,		//数据长度不对
	BES200_ERROR_DATANOTRIGHT,		//数据不符合要求
	BES200_ERROR_FAILED,			//失败
	BES200_ERROR_HEAD,				//报文头部错误
	BES200_ERROR_OTHERS,			//其他错误
};


/*************************************链接库函数声明*********************************/



typedef struct CollectControllerParameter{
	uint32_t 	ID;					//控制器的ID，必须唯一
	char     	Name[32];			//控制器的名字
	char     	Alias[64];			//控制器别名，支持汉字
	char     	Description[128];	//控制器的描述信息，支持汉字
	char     	Location[64];		//控制器的安装位置
	char     	Zone[64];			//区域信息
	uint8_t 	Active;				//控制器的使能状态，只有使能时才会正常工作，才会与下位机通信
	uint8_t  	SamplePeriod;	 	//采样周期		   单位为分钟
	uint8_t  	HisdataSavePeriod;	//历史数据保存周期  单位为小时
}_TAG_CollectControllerParameter;

#define  BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_SIZE_TEST   sizeof(struct CollectControllerParameterTest)
typedef struct CollectControllerParameterTest {
	uint32_t 	ID;					//控制器的ID，必须唯一
	string     	Name;				//控制器的名字
	string     	Alias;				//控制器别名，支持汉字
	string     	Description;		//控制器的描述信息，支持汉字
	string     	Location;			//控制器的安装位置
	string     	Zone;				//区域信息
	uint8_t 	Active;				//控制器的使能状态，只有使能时才会正常工作，才会与下位机通信
	uint8_t  	SamplePeriod;	 	//采样周期		   单位为分钟
	uint8_t  	HisdataSavePeriod;	//历史数据保存周期  单位为小时
}_TAG_CollectControllerParameterTest;
REFLECTION(CollectControllerParameterTest, ID, Name, Alias, Description, Location, Zone, Active, SamplePeriod, HisdataSavePeriod)

struct CollectControllerInfo{
	char		Name[32];				//控制器名字
	char		CampanyName[32];		//公司名字
	char		Model[32];				//型号
	uint8_t		HWVer;					//硬件版本号
	uint8_t		FWVer;					//软件版本号
	uint8_t		IP[4];					//IP地址
	uint8_t		Mask[4];				//Mask地址
	uint8_t		GateWay[4];				//网关
	uint8_t		Mac[6];					//Mac地址
	uint32_t 	ID;					//控制器的ID，必须唯一
	char     	Alias[64];			//控制器别名，支持汉字
	char     	Description[128];	//控制器的描述信息，支持汉字
	char     	Location[64];		//控制器的安装位置
	char     	Zone[64];			//区域信息
	uint8_t 	Active;				//控制器的使能状态，只有使能时才会正常工作，才会与下位机通信
	uint8_t  	SamplePeriod;	 	//采样周期		   单位为分钟
	uint8_t  	HisdataSavePeriod;	//历史数据保存周期  单位为小时
};


/*************************************8********电表相关*****************************************************************/
//对应17类能源类型
//enum MeterType
//{
//	METER_TYPE_ELETRIC = 0,					//电表
//	METER_TYPE_WATER,						//水表
//	METER_TYPE_GAS,							//燃气
//	METER_TYPE_CENTERAL_HEATING,			//集中供暖
//	METER_TYPE_CENTERAL_COOLING,			//集中供冷
//	METER_TYPE_ARTIFICAL,					//燃煤
//	METER_TYPE_LPGAS,						//液化石油气
//	METER_TYPE_ACGAS,						//人工煤气
//	METER_TYPE_GASOLINE, 					//汽油
//	METER_TYPE_KEROSENE,					//煤油
//	METER_TYPE_DIESELOIL,					//柴油
//	METER_TYPE_RENEWABLE_ENERGY, 			//可再生
//	METER_TYPE_OTHER						//其他能耗
//};
enum MeterType
{
	METER_TYPE_ELETRIC = 0,					//电表
	METER_TYPE_ARTIFICAL,					//燃煤
	METER_TYPE_LPGAS,						//液化石油气
	METER_TYPE_ACGAS,						//人工煤气
	METER_TYPE_GASOLINE, 				    //汽油
	METER_TYPE_KEROSENE = 5,			    //煤油
	METER_TYPE_DIESELOIL,				    //柴油
	METER_TYPE_RENEWABLE_ENERGY, 		    //可再生
	METER_TYPE_COOL_WATER = 0x10,           //冷水水表
	METER_TYPE_HOT_WATER = 0x11,            //生活热水水表
	METER_TYPE_DIRECT_WATER = 0x12,         //直饮水水表
	METER_TYPE_MIDDLE_WATER = 0x13,         //中水水表
	METER_TYPE_HEAT = 0x20,                 //热量表（计热量）
	METER_TYPE_COLD = 0x21,                 //热量表（计冷量）
	METER_TYPE_GAS = 0x30,                  //燃气表
	METER_TYPE_OTHER = 50				    //其他
};
enum ComRate
{
	COMRATE_600 = 0,					//波特率 600bps
	COMRATE_1200,						//波特率 1200bps
	COMRATE_2400,						//波特率 2400bps
	COMRATE_4800,						//波特率 4800bps
	COMRATE_7200,						//波特率 7200bps
	COMRATE_9600,						//波特率 9600bps
	COMRATE_19200,						//波特率 19200bps
	COMRATE_115200,						//波特率 115200bps
	COMRATE_OTHER
};
enum ComPort{
	COMPORT_ONE = 0,					//端口1
	COMPORT_TWO,						//端口2
	COMPORT_THREE,						//端口3
	COMPORT_FOUR						//端口4
};
//enum ComAgreementType{
//	COMAGREEMENTTYPE_DLT97 = 0,			//协议dlt97
//	COMAGREEMENTTYPE_DLT07,				//协议dlt07
//	COMAGREEMENTTYPE_MODBUS,			//modbus
//	COMAGREEMENTTYPE_OTHER
//};
enum ComAgreementType {
	COMAGREEMENTTYPE_DLT97 = 0,			//协议dlt97
	COMAGREEMENTTYPE_DLT07,				//协议dlt07
	COMAGREEMENTTYPE_MODBUS,			//modbus
	COMAGREEMENTTYPE_CJT188,            //标准188协议
	COMAGREEMENTTYPE_SIMPLE_CJT188,     //精简188协议
	COMAGREEMENTTYPE_EN1434,            //欧标1434协议
	COMAGREEMENTTYPE_W645,              //水表645协议
	COMAGREEMENTTYPE_SDJYL_WaterMeter,					//add 20190713
	COMAGREEMENTTYPE_OTHER
};
//能耗电表能耗数据
struct MeterElectricData{
	uint32_t 	MeterID;			    //电表序列号
	uint32_t 	ElectricData[MAX_ELECTRIC_COUNT];		//最多一百个电能参数
	uint8_t     CollectCount;		    //实际采集的电能参数个数
	uint8_t  	DateYear;				//日期年，如14
	uint8_t  	DateMonth; 				//日期月，1-12
	uint8_t  	DateDay; 				//日期日，1-31
	uint8_t  	TimeHour;				//时间时，0-23
	uint8_t  	TimeMinute; 		    //时间分, 0-59
	uint8_t 	TimeSecond; 			//时间秒, 0-59
};
//能耗电表能耗参数
enum DataEncodeType{
	DATA_ENCODETYPE_BCD = 0,
	DATA_ENCODETYPE_DEC,
	DATA_ENCODETYPE_OTHER
};

struct ElectricDataInfo{
	uint32_t 	ElectricID;			//能耗参数 ID
	char    	ElectricName[32];	//能耗名称
	uint32_t 	OffsetAddr;      	//寄存器偏移地址 
	uint8_t 	DataLength;			//数据长度 字节数		
	uint8_t 	DataEncodeType;	    //编码规则 BCD编码，或者是10进制编码
	char  	    UintType[32];	    //按照规范119页的编码规则来
	uint8_t		PointLocation;		//小数点位置 解析规则
};

//电表采集方案
struct ElectricDataCollectMethod{
	uint32_t CollectMethodID;						//采集方案ID
	struct ElectricDataInfo  ElectricDataInfo[MAX_ELECTRIC_COUNT];	//最多采集一百个电表个数
	uint8_t CollectCount;							//要采集的实际的电表个数

};
//能耗电表配置参数 
struct MeterParameter{
	uint32_t	MeterID;					//能耗电表序列号
	uint8_t		MeterType;					//电表类型 		 
	uint8_t		ComRate;					//通信速率 		 
	uint8_t		ComPort;					//通信端口号	     
	uint8_t		ComAgreementType;			//通信规约类型	 
	uint8_t		PhyAddr[7];					//物理地址	采用BCD编码的形式	参照规范 136页 按照格式03 BYTE1-BYTE6   在上位机按照 个十百千写入数
	char		Alias[64];					//表的别名
	char		Description[128];			//表的描述信息
	char		Location[64];				//电表的安装位置
	uint8_t		Active;						//电表的使能状态 ACTIVE_ENABLE  使能  ACTIVE_DISABLE 禁用
	struct  ElectricDataCollectMethod CollectMethod;//采集方案
};
//历史数据 电表能耗数据
struct MeterElectricHisData{
	uint32_t 	MeterID;			    						   //电表序列号
	uint32_t 	ElectricData[MAX_ELECTRIC_PARAM_COUNT];			   //最多一百个电能参数
	uint8_t     CollectCount;		    						   //实际采集的电能参数个数
	uint8_t  	DateYear;										   //开始日期年，如14
	uint8_t  	DateMonth; 										   //开始日期月，1-12
	uint8_t  	DateDay; 										   //开始日期日，1-31
	uint8_t  	TimeHour;										   //开始时间时，0-23
	uint8_t  	TimeMinute; 									   //开始时间分, 0-59
	uint8_t 	TimeSecond; 									   //开始时间秒, 0-59
};
//历史数据 即一个电表一天的数据，最多是24小时的
struct MeterHisElectricData{
	uint32_t 										MeterID;			    	   //电表序列号
	struct MeterElectricHisData MeterElectricData[MAX_STORE_HOUR_COUNT];		   //最多是一个小时一次数据
	uint8_t 										HisDataHourCount;			   //小时个数
};

/*********************************************************报警相关信息*******************************************************/
enum AlarmInforCategory{
	CATEGORY_SYSTEM = 0,			//系统事件
	CATEGORY_METER,					//电表事件
	CATEGORY_OTHER
};
enum AlarmInforType{
	ALARMINFO_TYPE_NANDERROR = 0,			//存储失败
	ALARMINFO_TYPE_BATTERY_UNDER_VOLTAGE,	//电池欠压
	ALARMINFO_TYPE_COMBREAK,				//电表通信失败  与电脑通信可以忽略，因为实时查询的没有意义
	ALARMINFO_TYPE_RESET,					//系统复位
	ALARMINFO_TYPE_NONE,					//没有报警信息  
	ALARMINFO_TYPE_OTHER					//其他
};

struct AlarmInfor{
	uint32_t 	AlarmInforID;				    //报警信息序列号
	uint8_t  	AlarmInforDateYear;				//开始日期年，如14
	uint8_t  	AlarmInforDateMonth; 			//开始日期月，1-12
	uint8_t  	AlarmInforDateDay; 				//开始日期日，1-31
	uint8_t     AlarmInforTimeHour;				//开始时间时，0-23
	uint8_t     AlarmInforTimeMinute; 			//开始时间分, 0-59
	uint8_t     AlarmInforTimeSecond; 			//开始时间秒, 0-59
	uint8_t 	AlarmInforCategory;				//事件分类
	uint16_t 	AlarmInforSeqNum;				//仪表序列号 或者  为采集器的ID
	uint8_t 	AlarmInforType;					//事件类型
};


/////////////////////////////////////////////////////////////////
////////////////////////////
//////////////////
/////////////  begp

#define  SENDBUF_SIZE   1*1024*1024
#define  RECVBUF_SIZE   1*1024*1024

#define   HISDATA_SENDBUF_SIZE    1*1024*1024
#define   HISDATA_RECVBUF_SIZE    1*1024*1024

#define   CONTROL_MODE_SIZE    10
#define   COLLECT_MODE_SIZE    10
#define   CONTROL_POINT_SIZE    100
#define   COLLECT_POINT_SIZE    100

#define   MAX_STORE_HOUR_COUNT   24

/* *****************************************************************定义通信功能码 **************************************/
typedef enum
{
	/* 和采集器相关 */
	BEGP_FUNCODE_ADD_CONTROLLER_PARAMETER = 0,				//新增一个控制器
	BEGP_FUNCODE_SET_CONTROLLER_PARAMETER,					//设置控制器的所有参数
	BEGP_FUNCODE_GET_CONTROLLER_PARAMETER,					//获取控制器的所有参数
	BEGP_FUNCODE_SET_CONTROLLER_TIME,					    //设置控制器的时间
	BEGP_FUNCODE_GET_CONTROLLER_TIME,				        //获取控制器的时间
	BEGP_FUNCODE_REBOOT_CONTROLLER,						    //重启控制器，相当于重启复位
	BEGP_FUNCODE_RESET_CONTROLLER,							//重置控制器，恢复出厂设置，并重启
	BEGP_FUNCODE_DELETE_CONTROLLER,						    //删除一个控制器，并删除和它相关的电表
	BEGP_FUNCODE_GET_CONTROLLER_INFO,						//通过UDP广播形式获取所有控制器的信息
	BEGP_FUNCODE_SET_CONTROLLER_INFO,						//通过UDP广播形式设置控制器的参数
	/* 电表配置相关 */
	BEGP_FUNCODE_ADD_METER,									//新增加一个电表信息
	BEGP_FUNCODE_DELETE_METER,								//删除一个电表
	BEGP_FUNCODE_SET_METER_PARAMETER,						//设置一个电表的所有参数
	BEGP_FUNCODE_GET_METER_PARAMETER,						//获取一个电表的所有配置信息
	/* 实时数据相关 */
	BEGP_FUNCODE_GET_METER_RTDATA,							//获取实时数据
	/* 历史数据相关 */
	BEGP_FUNCODE_GET_METER_HISDATA,							//获取历史数据
	/* 报警相关 */
	BEGP_FUNCODE_GET_ALARMINFO, 						    //获取一条报警信息
	/*远程升级*/
	BEGP_FUNCODE_IAP										//远程升级
}BEGP_FUNCODE_TYPE_T;

typedef enum
{
	BEGP_PACKETDATA_ECHO_CODE_SUCCESS = 0,					//成功
	BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR,					//数据长度不对
	BEGP_PACKETDATA_ECHO_CODE_METERNOTFOUND,				//meterid 不对
	BEGP_PACKETDATA_ECHO_CODE_METERALREADYEXIST,		    //电表已经存在
	BEGP_PACKETDATA_ECHO_CODE_VALUEERR,				  		//数据不对错误 比如波特率 ，端口号等
	BEGP_PACKETDATA_ECHO_CODE_CRCERROR,				  		//crc不对
	BEGP_PACKETDATA_ECHO_CODE_MAXMETERCOUNT,				//电表数达到上限不能再增加
	BEGP_PACKETDATA_ECHO_CODE_ERROR

}BEGP_PACKETDATA_ECHO_CODE_TYPE_T;

/* 请求数据帧头 */
#define BEGP_REQ_HEAD_SIZE	sizeof(struct _BEGP_REQ_HEAD_)
typedef struct _BEGP_REQ_HEAD_
{
	uint8_t    SlaveAddress;		//DDC用485通信时的地址
	uint8_t	   FunctionCode;		//功能码
	uint16_t   SequenceNumber;		//序列号
	uint32_t   DataSize;			//要发送的数据长度
}BEGP_REQ_HEAD_T;

REFLECTION(_BEGP_REQ_HEAD_, SlaveAddress, FunctionCode, SequenceNumber, DataSize)

/* 响应数据帧头 */
#define BEGP_RSP_HEAD_SIZE   sizeof(struct _BEGP_RSP_HEAD_)
typedef struct _BEGP_RSP_HEAD_
{
	uint8_t    SlaveAddress;		//DDC用485通信时的地址
	uint8_t	   FunctionCode;		//功能码
	uint16_t   SequenceNumber;		//序列号
	uint32_t   DataSize;			//要发送的数据长度
}BEGP_RSP_HEAD_T;
REFLECTION(_BEGP_RSP_HEAD_, SlaveAddress, FunctionCode, SequenceNumber, DataSize)
/* 设置请求的响应数据正文 */
#define BEGP_RSP_PACKETDATA_ECHO_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_ECHO_)
typedef struct _BEGP_RSP_PACKETDATA_ECHO_
{
	uint8_t  Code;
}BEGP_RSP_PACKETDATA_ECHO_T;
REFLECTION(_BEGP_RSP_PACKETDATA_ECHO_, Code)
/********************************** 采集器相关 **********************************/
/* 新增一个采集器控制器 */
#define BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_)
typedef struct _BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_
{
	uint32_t 	ID;						//控制器的ID，必须唯一
	char     	Name[32];				//控制器的名字
	char     	Alias[64];				//控制器别名，支持汉字
	char     	Description[128];		//控制器的描述信息，支持汉字
	char     	Location[64];			//控制器的安装位置
	char     	Zone[64];				//区域信息
	uint8_t 	Active;					//控制器的使能状态，只有使能时才会正常工作，才会与下位机通信
	uint8_t  	SamplePeriod;	 		//采样周期			单位为分钟
	uint8_t  	HisdataSavePeriod;		//历史数据保存周期	单位为小时
}BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T;

/* 设置控制器的所有参数 */
#define  BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_)
typedef struct  _BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_
{
	uint32_t 	ID;						//控制器的ID，必须唯一
	char     	Name[32];				//控制器的名字
	char     	Alias[64];				//控制器别名，支持汉字
	char     	Description[128];		//控制器的描述信息，支持汉字
	char     	Location[64];			//控制器的安装位置
	char     	Zone[64];				//区域信息
	uint8_t 	Active;					//控制器的使能状态，只有使能时才会正常工作，才会与下位机通信
	uint8_t  	SamplePeriod;	 		//采样周期		   单位为分钟
	uint8_t  	HisdataSavePeriod;		//历史数据保存周期 单位为小时
}BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_T;
REFLECTION(_BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_, ID, Name, Alias, Description, Location, Zone, Active, SamplePeriod, HisdataSavePeriod);
/* 获取控制器的所有配置参数 */
#define BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_SIZE    sizeof(struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_)
typedef struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_
{
	uint32_t 	ID;						//控制器的ID，必须唯一
	char     	Name[32];				//控制器的名字
	char     	Alias[64];				//控制器别名，支持汉字
	char     	Description[128];		//控制器的描述信息，支持汉字
	char     	Location[64];			//控制器的安装位置
	char     	Zone[64];				//区域信息
	uint8_t 	Active;					//控制器的使能状态，只有使能时才会正常工作，才会与下位机通信
	uint8_t  	SamplePeriod;	 		//采样周期		   单位为分钟
	uint8_t  	HisdataSavePeriod;		//历史数据保存周期  单位为小时
}BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_T;

/* 设置控制器的时间 */
#define  BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_SIZE    sizeof(struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_)
typedef struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_
{
	uint8_t year;			//年份表示2000年以后的年数如12代表2012年
	uint8_t month;			//1-12
	uint8_t day;			//1-31
	uint8_t wday;			//星期几,取值范围MONDAY-SUNDAY(1-7)
	uint8_t hour;			//0-23
	uint8_t minute;			//0-59
	uint8_t second;			//0-59
}BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_T;

REFLECTION(_BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_, year,month,day,wday,hour,minute,second)

/* 获取控制器的时间 */
#define BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_)
typedef struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_
{
	uint8_t year;		//年份表示2000年以后的年数如12代表2012年
	uint8_t month;		//1-12
	uint8_t day;		//1-31
	uint8_t wday;		//星期几,取值范围MONDAY-SUNDAY(1-7)
	uint8_t hour;		//0-23
	uint8_t minute;		//0-59
	uint8_t second;		//0-59
}BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_T;



/* 获取控制器的所有配置参数 */
#define BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_SIZE    sizeof(struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_)
typedef struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_
{
	char	Name[32];				//名字
	char	CampanyName[32];		//公司名字
	char	Model[32];				//型号
	uint8_t	HWVer;					//硬件版本号
	uint8_t	FWVer;					//软件版本号
	uint8_t	IP[4];					//IP地址
	uint8_t Mask[4];				//Mask地址
	uint8_t	GateWay[4];				//网关
	uint8_t Mac[6];					//Mac地址
	uint32_t 	ID;						//控制器的ID，必须唯一
	char     	Alias[64];				//控制器别名，支持汉字
	char     	Description[128];		//控制器的描述信息，支持汉字
	char     	Location[64];			//控制器的安装位置
	char     	Zone[64];				//区域信息
	uint8_t 	Active;					//控制器的使能状态，只有使能时才会正常工作，才会与下位机通信
	uint8_t  	SamplePeriod;	 		//采样周期		   单位为分钟
	uint8_t  	HisdataSavePeriod;		//历史数据保存周期 单位为小时
}BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_T;

/* 设置控制器的所有配置参数 */
#define BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_SIZE    sizeof(struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_)
typedef struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_
{
	char	Name[32];			//名字
	char	CampanyName[32];	//公司名字
	char	Model[32];			//型号
	uint8_t	HWVer;				//硬件版本号
	uint8_t	FWVer;				//软件版本号
	uint8_t	IP[4];				//IP地址
	uint8_t Mask[4];			//Mask地址
	uint8_t	GateWay[4];			//网关
	uint8_t Mac[6];				//Mac地址
	uint32_t 	ID;						//控制器的ID，必须唯一
	char     	Alias[64];				//控制器别名，支持汉字
	char     	Description[128];		//控制器的描述信息，支持汉字
	char     	Location[64];			//控制器的安装位置
	char     	Zone[64];				//区域信息
	uint8_t 	Active;					//控制器的使能状态，只有使能时才会正常工作，才会与下位机通信
	uint8_t  	SamplePeriod;	 		//采样周期		   单位为分钟
	uint8_t  	HisdataSavePeriod;		//历史数据保存周期 单位为小时
}BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_T;











/********************************** 电表相关 **********************************/
/*增加一个电表*/
//请求帧 
#define BEGP_REQ_PACKETDATA_ADD_METER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_ADD_METER_)
typedef struct _BEGP_REQ_PACKETDATA_ADD_METER_
{
	uint32_t		MeterID;			    //电表序列号
	uint8_t			MeterType;				//电表类型 		参照规范 114页
	uint8_t			ComRate;				//通信速率 		参照规范 114页
	uint8_t			ComPort;				//通信端口号	参照规范 114页
	uint8_t			ComAgreementType;		//通信规约类型	参照规范 114页
	uint8_t			PhyAddr[7];			    //物理地址		参照规范 136页 按照格式03 BYTE1-BYTE6 采用BCD编码的形式  在上位机按照 个十百千写入数
	char			Alias[64];				//表的别名
	char			Description[128];		//表的描述信息
	char			Location[64];			//电表的安装位置
	uint8_t			Active;					//电表的使能状态 ACTIVE_ENABLE  使能  ACTIVE_DISABLE 禁用
	struct ElectricDataCollectMethod CollectMethod;//采集方案
}BEGP_REQ_PACKETDATA_ADD_METER_T;
//响应帧

/* 配置模块的所有配置参数 */
//请求帧
#define BEGP_REQ_PACKETDATA_SET_METER_PARAMETER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_SET_METER_PARAMETER_)
typedef struct _BEGP_REQ_PACKETDATA_SET_METER_PARAMETER_
{
	uint32_t    MeterID;								//电表序列号
	uint8_t		MeterType;								//电表类型 		参照规范 114页
	uint8_t		ComRate;								//通信速率 		参照规范 114页
	uint8_t		ComPort;								//通信端口号	参照规范 114页
	uint8_t		ComAgreementType;						//通信规约类型	参照规范 114页
	uint8_t		PhyAddr[7];							    //物理地址		参照规范 136页 按照格式03 BYTE1-BYTE6 采用BCD编码的形式  在上位机按照 个十百千写入数
	char		Alias[64];								//表的别名
	char		Description[128];						//表的描述信息
	char		Location[64];							//电表的安装位置
	uint8_t		Active;									//电表的使能状态 ACTIVE_ENABLE  使能  ACTIVE_DISABLE 禁用
	struct ElectricDataCollectMethod	 CollectMethod;//采集方案
}BEGP_REQ_PACKETDATA_SET_METER_PARAMETER_T;
//响应帧
//echo 1、id不存在 2、其他通用

/* 删除一个电表 */
//请求帧
#define BEGP_REQ_PACKETDATA_DELETE_METER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_DELETE_METER_)
typedef struct _BEGP_REQ_PACKETDATA_DELETE_METER_
{
	uint32_t MeterID;//模块的识别码
}BEGP_REQ_PACKETDATA_DELETE_METER_T;
//响应帧
//echo 1、id不存在2，通用错误


/* **********************************获取电表的所有配置信息 ********************************************/
/* 请求帧数据 */
#define BEGP_REQ_PACKETDATA_GET_METER_PARAMETER_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_GET_METER_PARAMETER_)
typedef struct _BEGP_REQ_PACKETDATA_GET_METER_PARAMETER_
{
	uint32_t MeterID;//电表的ID
}BEGP_REQ_PACKETDATA_GET_METER_PARAMETER_T;
/* 响应帧数据 */
#define BEGP_RSP_PACKETDATA_GET_METER_PARAMETER_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_GET_METER_PARAMETER_)
typedef struct _BEGP_RSP_PACKETDATA_GET_METER_PARAMETER_
{

	uint32_t    MeterID;								//电表序列号
	uint8_t		MeterType;								//电表类型 		参照规范 114页
	uint8_t		ComRate;								//通信速率 		参照规范 114页
	uint8_t		ComPort;								//通信端口号	参照规范 114页
	uint8_t		ComAgreementType;						//通信规约类型	参照规范 114页
	uint8_t		PhyAddr[7];							//物理地址		参照规范 136页 按照格式03 BYTE1-BYTE6 采用BCD编码的形式  在上位机按照 个十百千写入数
	char		Alias[64];								//表的别名
	char		Description[128];						//表的描述信息
	char		Location[64];							//电表的安装位置
	uint8_t		Active;									//电表的使能状态 ACTIVE_ENABLE  使能  ACTIVE_DISABLE 禁用
	struct ElectricDataCollectMethod	 CollectMethod;//采集方案
}BEGP_RSP_PACKETDATA_GET_METER_PARAMETER_T;

/**********************************请求实时数据****************************/

#define	BEGP_REQ_PACKETDATA_GET_METER_RTDATA_SIZE sizeof(struct _BEGP_REQ_PACKETDATA_GET_METER_RTDATA_)
//请求帧
typedef struct _BEGP_REQ_PACKETDATA_GET_METER_RTDATA_
{
	uint32_t MeterID;//电表的ID
}BEGP_REQ_PACKETDATA_GET_METER_RTDATA_T;

// 响应帧数据  
#define BEGP_RSP_PACKETDATA_GET_METER_RTDATA_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_GET_METER_RTDATA_)
typedef struct _BEGP_RSP_PACKETDATA_GET_METER_RTDATA_
{
	uint32_t 	MeterID;			    //电表序列号
	uint32_t 	ElectricData[MAX_ELECTRIC_COUNT];		//最多一百个电能参数
	uint8_t     CollectCount;		    //实际采集的电能参数个数
	uint8_t  	DateYear;																			//日期年，如14
	uint8_t  	DateMonth; 																			//日期月，1-12
	uint8_t  	DateDay; 																			//日期日，1-31
	uint8_t  	TimeHour;																			//时间时，0-23
	uint8_t  	TimeMinute; 																		//时间分, 0-59
	uint8_t 	TimeSecond; 																		//时间秒, 0-59
}BEGP_RSP_PACKETDATA_GET_METER_RTDATA_T;


/**********************************请求历史数据****************************/

#define	BEGP_REQ_PACKETDATA_GET_METER_HISDATA_SIZE sizeof(struct _BEGP_REQ_PACKETDATA_GET_METER_HISDATA_)
//请求帧
typedef struct _BEGP_REQ_PACKETDATA_GET_METER_HISDATA_
{
	uint32_t MeterID;	//电表的ID
	uint8_t  SeletDay;	//选择那一天
}BEGP_REQ_PACKETDATA_GET_METER_HISDATA_T;

// 响应帧数据  
#define BEGP_RSP_PACKETDATA_GET_METER_HISDATA_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_GET_METER_HISDATA_)
typedef struct _BEGP_RSP_PACKETDATA_GET_METER_HISDATA_
{
	uint32_t 	  MeterID;			    										     //电表序列号
	struct MeterElectricHisData MeterElectricData[MAX_STORE_HOUR_COUNT];			 //最多是一个小时一次数据
	uint8_t	     HisDataHourCount;													 //小时个数
}BEGP_RSP_PACKETDATA_GET_METER_HISDATA_T;


/*************************************请求实时报警信息**********************************/

// 响应帧数据  
#define BEGP_RSP_PACKETDATA_GET_ALARMINFO_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_GET_ALARMINFO_)
typedef struct _BEGP_RSP_PACKETDATA_GET_ALARMINFO_
{
	uint32_t 	AlarmInforID;				    //报警信息序列号
	uint8_t  	AlarmInforDateYear;				//开始日期年，如14
	uint8_t  	AlarmInforDateMonth; 			//开始日期月，1-12
	uint8_t  	AlarmInforDateDay; 				//开始日期日，1-31
	uint8_t     AlarmInforTimeHour;				//开始时间时，0-23
	uint8_t     AlarmInforTimeMinute; 			//开始时间分, 0-59
	uint8_t     AlarmInforTimeSecond; 			//开始时间秒, 0-59
	uint8_t 	AlarmInforCategory;				//事件分类
	uint16_t 	AlarmInforSeqNum;				//仪表序列号 或者  为采集器的ID
	uint8_t 	AlarmInforType;					//事件类型
}BEGP_RSP_PACKETDATA_GET_ALARMINFO_T;

#define BEGP_CRC_SIZE 2

#pragma pack() // 取消 对齐 成对出现的

}
#endif