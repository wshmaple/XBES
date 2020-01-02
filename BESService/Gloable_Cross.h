#ifndef _GLOABLE_CROSS_
#define _GLOABLE_CROSS_
namespace Gloable_Cross{
	// 原DllTest  BES200.h
#define BEGP_CRC_SIZE 2
/*函数中需要用到的整型位数定义*/
	typedef  unsigned       char uint8_t;
	typedef  unsigned short int  uint16_t;
	typedef  unsigned       int  uint32_t;
	typedef    signed       char int8_t;
	typedef    signed short int  int16_t;
	typedef    signed       int  int32_t;

#pragma pack(1)

	/* 是否启用Active取值 */
	enum Active
	{
		ACTIVE_DISABLE = 0,  //禁止
		ACTIVE_ENABLE        //使能
	};

	/*BES200函数错误码定义*/
	enum Bes200ErrorCode {
		BES200_ERROR_OK = 0,         //无错误，正常返回
		BES200_ERROR_CONNECT = 1,   //与服务器建立连接失败
		BES200_ERROR_SEND = 2,     //数据发送失败
		BES200_ERROR_RECIVE = 3,  //数据接收失败
		BES200_ERROR_REFUSED = 4,//请求被服务器拒绝
		BES200_ERROR_OTHERS = 5 //其他错误
	};
	/*DDC错误码取值和模块的错误码取值*/
	enum ErrorCode
	{
		ERROR_CODE_OK = 0x00,//没有错误
		ERROR_CODE_OFF_LINE = 0x01,//掉线
		ERROR_CODE_INIT_FAILED = 0x02,//初始化失败
	};

	/****************************点相关取值定义*******************************/
	/* 点类型定义 */
	enum PointType
	{
		POINT_TYPE_LAI = 0,   //实点AI
		POINT_TYPE_LAO = 1,   //实点AO
		POINT_TYPE_LDI = 2,   //实点DO
		POINT_TYPE_LDO = 3,   //实点DI
		POINT_TYPE_VIRTUAL_AI = 4,//虚点AI类型
		POINT_TYPE_VIRTUAL_AO = 5,//虚点AO类型
		POINT_TYPE_VIRTUAL_DI = 6,//虚点DI类型
		POINT_TYPE_VIRTUAL_DO = 7,//虚点DO类型
	};
	/* 点工作模式定义 */
	enum WorkMode
	{
		WORK_MODE_AUTO = 0,//逻辑点工作模式自动
		WORK_MODE_MANUAL = 1, //逻辑点工作模式手动
	};
	/* 点极性定义 */
	enum Polarity
	{
		POLARITY_DIRECT = 0,//正向
		POLARITY_REVERSE = 1,//反向
	};
	/* 点报警是否启用定义 */
	enum AlarmActive
	{
		ALARM_ACTIVE_DISABLE = 0,//报警禁止
		AlARM_ACTIVE_ENABLE = 1,//报警启用
	};
	/* 点报警类型定义 */
	enum AlarmType
	{
		ALARM_TYPE_OK = 0, //不报警
		ALARM_TYPE_STANDARD = 1, //标准报警
		ALARM_TYPE_ENHANCED = 2, //增强报警
	};
	/* 报警优先级定义 */
	enum AlarmPriority
	{
		AlARM_PRIORITY_GENERAL = 0,//一般
		AlARM_PRIORITY_LARGER = 1,//较大
		AlARM_PRIORITY_MAJOR = 2,//重大
	};
	/* 数字量报警触发定义 */
	enum AlarmTrigger
	{
		ALARM_TRIGGER_ON = 0,//闭合报警
		ALARM_TRIGGER_OFF = 1,//断开报警
	};
	/* 点报警状态定义 */
	enum AlarmState
	{
		ALARM_STATE_OK = 0,//没有报警
		ALARM_STATE_HIGH_LIMIT = 1,//高限报警
		ALARM_STATE_LOW_LIMIT = 2,//低限报警
		ALARM_STATE_ON = 3,//闭合报警
		ALARM_STATE_OFF = 4,//断开报警
	};
	/*模拟量通道类型定义*/
	enum LineType
	{
		LV0010 = 0,  //线性电压0..10V 
		LC0420 = 1,  //线性电流4..20mA 
		LC0020 = 2,  //线性电流0..20mA
	};

	/* 数字量点值定义 */
#define OFF     0
#define ON      255

/****************************场景相关取值定义**************************/
/* 采集方式定义 */
	enum CollectID
	{
		COLLECT_ID_TIME_INTERVAL = 1,//时间间隔
		COLLECT_ID_COV = 2,//变化量
	};
	/* 采集时间间隔单位定义 */
	enum TimeUnit
	{
		TIME_UNIT_DAY = 1,//时间间隔单位天
		TIME_UNIT_HOUR = 2,//时间间隔单位时
		TIME_UNIT_MINUTE = 3,//时间间隔单位分
		TIME_UNIT_SECOND = 4,//时间间隔单位秒
	};

	/**************************计划相关取值定义**************************/
	/* 计划类型定义 */
	enum PlanType
	{
		PLAN_TYPE_REGULAR = 0, //常规的
		PLAN_TYPE_HOLIDAY = 1, //假期的
	};
	/* 执行方式定义 */
	enum ExecutionWay
	{
		EXECUTION_WAY_DAILY = 0, //每天执行
		EXECUTION_WAY_SPAN = 1, //持续执行
	};
	/* 场景类型定义 */
	enum SceneType
	{
		SCENE_TYPE_CONTROL = 0, //控制场景
		SCENE_TYPE_COLLECT = 1, //采集场景
	};
	/*周掩码定义*/
	//第一位表示星期一：0表示不执行，1表示执行，即0x01；
	//第二位表示星期二：0表示不执行，1表示执行，即0x02；
	//第三位表示星期三：0表示不执行，1表示执行，即0x04；
	//第四位表示星期四：0表示不执行，1表示执行，即0x08；
	//第五位表示星期五：0表示不执行，1表示执行，即0x10；
	//第六位表示星期六：0表示不执行，1表示执行，即0x20；
	//第七位表示星期七：0表示不执行，1表示执行，即0x40；
	//8位全为0，则表示每天执行。

	/*BES200函数中可能用到的结构体定义*/
	/********************************* 控制器相关 ***********************************/
	struct ControllerParameter
	{
		char     Name[32];//控制器的名字
		char     Alias[64];//控制器别名，支持汉字
		char     Description[128];//控制器的描述信息，支持汉字
		char     Location[64];//控制器的安装位置
		char     Zone[64];//区域信息
		uint16_t ID;//控制器的ID，必须唯一
		uint8_t  Active;//控制器的使能状态，只有使能时才会正常工作，才会与下位机通信
	};


	struct ControllerInfo {
		char	Name[32];//名字
		char	CampanyName[32];//公司名字
		char	Model[12];//型号
		uint8_t	HWVer;//硬件版本号
		uint8_t	FWVer;//软件版本号
		uint8_t	IP[4];//IP地址
		uint8_t Mask[4];//Mask地址
		uint8_t	GateWay[4];//网关
		uint8_t Mac[6];//Mac地址
	};

	//struct tm {
	//	int tm_sec;     /* seconds after the minute - [0,59] */
	//	int tm_min;     /* minutes after the hour - [0,59] */
	//	int tm_hour;    /* hours since midnight - [0,23] */
	//	int tm_mday;    /* day of the month - [1,31] */
	//	int tm_mon;     /* months since January - [0,11] */
	//	int tm_year;    /* years since 1900 */
	//	int tm_wday;    /* days since Sunday - [0,6] */
	//	int tm_yday;    /* days since January 1 - [0,365] */
	//	int tm_isdst;   /* daylight savings time flag */
	//};
	/************************************ 模块相关 *********************************/
	struct ModuleParameter
	{
		char Name[32];//模块的名字
		char Alias[64];//模块的别名
		char Description[128];//模块的描述信息
		char Location[64];//模块的安装位置

		//模块挂在哪条总线上的宏定义
#define FLN1 1
#define FLN2 2
#define FLN3 3
#define FLN4 4
		uint8_t  ModelID;//模块型号编码
		uint8_t  FlnID;//模块挂在哪条FLN总线上，可能取值FLN1、FLN2、FLN3、FLN4
		uint8_t  SlaveAddress;//模块的通信地址
		uint8_t  Active;//模块的使能状态 ACTIVE_ENABLE  使能  ACTIVE_DISABLE 禁用
		uint16_t ID;//模块的识别码
	};

	/********************************** 逻辑点相关 **********************************/
	/* 模拟量输入实点AI */
	struct LogicalPointParameterLAI
	{
		uint8_t  PointType; //点类型  实点AI
		uint16_t ID; //点的ID
		uint8_t  Active;  //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char     Name[32]; //点的名字
		char     Alias[64]; //点的别名
		char     Description[64]; //描述
		uint16_t ModuleID;  //点所在模块的ID号
		uint8_t  ChannelIndex;  //点所在通道索引
		uint8_t  WorkMode;  //工作模式，手动模式或自动模式
		uint8_t  Polarity;  //极性，正向或反向
		int32_t  InitValue;//值

		/*报警相关*/
		uint8_t  AlarmActive;//报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		uint8_t  AlarmType;  //报警类型，不报警、标准报警、加强报警
		uint8_t  AlarmPriority;  //报警优先级

		uint8_t  LineType;//有效输入类型，包括0-10V、0-20mA、4-20mA
		char     Unit[12]; //工程单位，如℉、℃、KWh
		int32_t  HighRange; //最高阀值
		int32_t  LowRange;  //最低阀值
		uint8_t  Precision;  //精度
		int32_t  AlarmHighValue; //高限报警值
		int32_t  AlarmLowValue;//底限报警值
	};
	/* 模拟量输出实点AO */
	struct LogicalPointParameterLAO
	{
		uint8_t  PointType;  //点类型  实点AO
		uint16_t ID; //点的ID
		uint8_t  Active;     //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char     Name[32];  //点的名字
		char     Alias[64]; //点的别名
		char     Description[64];//描述
		uint16_t ModuleID; //点所在模块的ID号
		uint8_t  ChannelIndex; //点所在通道索引
		uint8_t  WorkMode;  //工作模式，手动模式或自动模式
		uint8_t  Polarity;  //极性，正向或反向
		int32_t  InitValue;//初始值

		/*报警相关*/
		uint8_t AlarmActive;  //报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		uint8_t AlarmType;   //报警类型，不报警、标准报警、加强报警
		uint8_t AlarmPriority;   //报警优先级

		uint8_t LineType;  //有效输出类型，包括0-10V、0-20mA、4-20mA
		char    Unit[12];  //工程单位，如℉、℃、KWh
		int32_t HighRange;  //最高阀值
		int32_t LowRange;   //最低阀值
		uint8_t Precision;   //精度
		int32_t AlarmHighValue;  //高限报警值
		int32_t AlarmLowValue;  //底限报警值
	};
	/* 数字量输入实点DI */
	struct LogicalPointParameterLDI
	{
		uint8_t  PointType;  //点类型    实点DI
		uint16_t ID; //点的ID
		uint8_t  Active;     //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char     Name[32];  //点的名字
		char     Alias[64]; //点的别名
		char     Description[64];//描述
		uint16_t ModuleID; //点所在模块的ID号
		uint8_t  ChannelIndex; //点所在通道索引
		uint8_t  WorkMode;  //工作模式，手动模式或自动模式
		uint8_t  Polarity;  //极性，正向或反向
		int32_t  InitValue;//值

		/*报警相关*/
		uint8_t AlarmActive;  //报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		uint8_t AlarmType;   //报警类型，不报警、标准报警、加强报警
		uint8_t AlarmPriority;   //报警优先级

		uint8_t AlarmTrigger;//报警触发
		uint8_t ActivePassive;//有源无源
	};
	/* 数字量输出实点DO */
	struct LogicalPointParameterLDO
	{
		uint8_t  PointType;  //点类型   实点DO
		uint16_t ID; //点的ID
		uint8_t  Active;     //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char     Name[32];  //点的名字
		char     Alias[64]; //点的别名
		char     Description[64];//描述
		uint16_t ModuleID; //点所在模块的ID号
		uint8_t  ChannelIndex; //点所在通道索引
		uint8_t  WorkMode;  //工作模式，手动模式或自动模式
		uint8_t  Polarity;  //极性，正向或反向
		int32_t  InitValue;//初始值

		/*报警相关*/
		uint8_t AlarmActive;  //报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		uint8_t AlarmType;   //报警类型，不报警、标准报警、加强报警
		uint8_t AlarmPriority;   //报警优先级

		uint8_t AlarmTrigger;//报警触发  开或关
	};
	/* 虚点AI */
	struct LogicalPointParameterVirtualAI
	{
		uint8_t   PointType;  //点类型  虚点AI
		uint16_t  ID; //点的ID
		uint8_t   Active;     //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char      Name[32];  //点的名字
		char      Alias[64]; //点的别名
		char      Description[64];//描述
		int32_t   InitValue;//初始值
		char      Unit[12];  //工程单位，如℉、℃、KWh
		uint8_t   Precision;  //精度

		/*报警相关*/
		uint8_t  AlarmActive;  //报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		uint8_t  AlarmType;   //报警类型，不报警、标准报警、加强报警
		uint8_t  AlarmPriority;   //报警优先级
		int32_t  AlarmHighValue;
		int32_t  AlarmLowValue;
	};
	/* 虚点AO */
	struct LogicalPointParameterVirtualAO
	{
		uint8_t   PointType;  //点类型  虚点AO
		uint16_t  ID; //点的ID
		uint8_t   Active;     //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char      Name[32];  //点的名字
		char      Alias[64]; //点的别名
		char      Description[64];//描述
		int32_t   InitValue;//初始值
		char      Unit[12];  //工程单位，如℉、℃、KWh
		uint8_t   Precision;  //精度

		/*报警相关*/
		uint8_t  AlarmActive;  //报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		uint8_t  AlarmType;   //报警类型，不报警、标准报警、加强报警
		uint8_t  AlarmPriority;   //报警优先级
		int32_t  AlarmHighValue;
		int32_t  AlarmLowValue;
	};
	/* 虚点DI */
	struct LogicalPointParameterVirtualDI
	{
		uint8_t   PointType;  //点类型  虚点DI
		uint16_t  ID; //点的ID
		uint8_t   Active;     //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char      Name[32];  //点的名字
		char      Alias[64]; //点的别名
		char      Description[64];//描述
		int32_t   InitValue;//初始值

		/*报警相关*/
		uint8_t AlarmActive;  //报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		uint8_t AlarmType;   //报警类型，不报警、标准报警、加强报警
		uint8_t AlarmPriority;   //报警优先级

		uint8_t AlarmTrigger;//报警触发  开或关
	};
	/* 虚点DO */
	struct LogicalPointParameterVirtualDO
	{
		uint8_t   PointType;  //点类型  虚点DO
		uint16_t  ID; //点的ID
		uint8_t   Active;     //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char      Name[32];  //点的名字
		char      Alias[64]; //点的别名
		char      Description[64];//描述
		int32_t   InitValue;//初始值

		/*报警相关*/
		uint8_t AlarmActive;  //报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		uint8_t AlarmType;   //报警类型，不报警、标准报警、加强报警
		uint8_t AlarmPriority;   //报警优先级

		uint8_t AlarmTrigger;//报警触发  开或关
	};
	/* 根据点类型不同，Tab中存放的点信息不同 */
	struct LogicalPointParameter
	{
		uint8_t   PointType;//点类型
		char     Tab[sizeof(struct LogicalPointParameterLAO)];//数组长度是模拟量输出 void      *tab;
	};
	/*获取点的报警信息 */
	struct LogicalPointAlarmInfor
	{
		int32_t  CurrentValue;//获取的当前值
		uint8_t  AlarmState;  //报警状态
		int32_t  AlarmTriggerValue; //首次报警触发值
		int32_t  AlarmMaxValue;     //报警最大值(即发生过的最大一次报警值)
	};
	/*设置点的值*/
	struct LogicalPointvalue
	{
		uint16_t  ID; //点的ID
		uint8_t   WorkMode;  //工作模式，手动模式或自动模式
		int32_t   Value;//初始值
	};
	/********************************** 场景相关 *********************************/
	/* 控制场景 */
	struct Control_Point
	{
		uint8_t  Active;   //使能状态
		uint16_t PointID;	 //计划所对应点的ID号
		int32_t  RunValue; //运行值,数字量0或255，模拟量是个数值
	};
	struct Control_Mode
	{
		uint8_t Active;  //使能状态
		char    Name[32];  //场景模式名称
		uint8_t ID;   //场景模式ID
		Control_Point ControlPoint[100];//模式里点信息
	};
	struct ControlSceneParameter
	{
		uint8_t   SceneType; //场景类型  控制或采集场景，取计划定义的值
		uint16_t  ID;       //控制场景的ID
		uint8_t   Active;  //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char      Name[32];  //场景名字
		char      Alias[64]; //场景别名
		Control_Mode    ControlMode[10]; //场景模式
	};
	/* 采集场景 */
	struct Collect_Point
	{
		uint8_t  Active;   //使能状态
		uint16_t PointID;	 //计划所对应点的ID号
		uint8_t  CollectID;//采集方式，时间间隔采集、值采集
		uint8_t  TimeUnit;//时间间隔单位
		int16_t  Value; //采集时间间隔值、变化量的值
	};
	struct Collect_Mode
	{
		uint8_t  Active;  //使能状态
		char     Name[32];  //场景模式名称
		uint8_t  ID;   //场景模式ID
		Collect_Point CollectPoint[100];//模式里点信息
	};
	struct CollectSceneParameter
	{
		uint8_t  SceneType; //场景类型  控制或采集场景，取计划定义的值
		uint16_t  ID;        //控制场景的ID
		uint8_t   Active;  //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char      Name[32];  //场景名字
		char      Alias[64]; //场景别名
		Collect_Mode    CollectMode[10]; //场景模式
	};
	/* 场景类型不同，Tab中存放的场景信息不同 */
	struct SceneParameter
	{
		uint8_t SceneType; //场景类型  控制或采集场景
		char    Tab[sizeof(struct CollectSceneParameter) > sizeof(struct ControlSceneParameter) ?
			sizeof(struct CollectSceneParameter) : sizeof(struct ControlSceneParameter)];//取控制场景和采集场景较大的值
	};

	/***************************************** 计划相关 *******************************/
	struct SchedulerParameter
	{
		uint8_t  Active;  //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char     Name[32];  //计划的名字
		char     Alias[64];  //别名
		uint8_t  PlanType;  //计划类型  常规计划或节假日计划

		uint8_t  StartDateYear;//开始日期年，如14
		uint8_t  StartDateMonth; //开始日期月，1-12
		uint8_t  StartDateDay; //开始日期日，1-31
		uint8_t  EndDateYear; //结束日期年
		uint8_t  EndDateMonth; //结束日期月
		uint8_t  EndDateDay; //结束日期日

		uint8_t  StartTimeHour;// 开始时间时，0-23
		uint8_t  StartTimeMinute; //开始时间分,0-59
		uint8_t  StartTimeSecond; //开始时间秒,0-59
		uint8_t  EndTimeHour;//结束时间时
		uint8_t  EndTimeMinute;//结束时间分
		uint8_t  EndTimeSecond;//结束时间秒

		uint8_t  ExecutionWay; //执行方式（每天执行、持续执行）
		uint8_t  WeekMask;	 //周掩码
		uint8_t  SceneType; //场景类型，控制场景或采集场景
		uint16_t SceneID;  //场景ID号
		uint8_t  ModeID;  //模式ID号
		uint8_t  DefaultModeID;//默认模式ID
	};



///< beg.h


#define  SENDBUF_SIZE   4096*3
#define  RECVBUF_SIZE   4096*3

#define   SCENE_SENDBUF_SIZE   10240
#define   SCENE_RECVBUF_SIZE   10240

#define   CONTROL_MODE_SIZE    10
#define   COLLECT_MODE_SIZE    10
#define   CONTROL_POINT_SIZE    100
#define   COLLECT_POINT_SIZE    100

/* 定义通信功能码 */
	typedef enum
	{
		/* 和DDC相关 */
		BEGP_FUNCODE_ADD_CONTROLLER_PARAMETER = 0,//新增一个控制器
		BEGP_FUNCODE_SET_CONTROLLER_PARAMETER,    //设置控制器的所有参数
		BEGP_FUNCODE_SET_CONTROLLER_TIME,         //设置控制器的时间
		BEGP_FUNCODE_REBOOT_CONTROLLER,           //重启控制器，相当于复位
		BEGP_FUNCODE_RESET_CONTROLLER,            //重置控制器，恢复出厂设置，并重启
		BEGP_FUNCODE_DELETE_CONTROLLER = 5,           //删除一个控制器，并删除和它相关的模块和点
		BEGP_FUNCODE_GET_CONTROLLER_TIME,        //获取控制器的时间
		BEGP_FUNCODE_GET_CONTROLLER_PARAMETER,    //获取控制器的所有配置参数
		BEGP_FUNCODE_GET_CONTROLLER_INFO,         //通过UDP广播形式获取所有控制器的信息
		BEGP_FUNCODE_SET_CONTROLLER_INFO,         //通过UDP广播形式设置控制器的参数
		/* 模块相关 */
		BEGP_FUNCODE_ADD_MODULE = 10,                 //新增加一个模块
		BEGP_FUNCODE_SET_MODULE_PARAMETER,       //设置一个模块的所有参数
		BEGP_FUNCODE_DELETE_MODULE,              //删除一个模块，并删除和模块相关的点
		BEGP_FUNCODE_GET_MODULE_PARAMETER,       //获取一个模块的所有配置信息
		BEGP_FUNCODE_GET_MODULE_ERROR_CODE,      //获取一个模块的错误代码
		/* 点相关 */
		BEGP_FUNCODE_ADD_LOGICAL_POINT = 15,             //新增加一个逻辑点
		BEGP_FUNCODE_SET_LOGICAL_POINT_PARAMETER,   //设置一个逻辑点的所有参数
		BEGP_FUNCODE_SET_LOGICAL_POINT_VALUE,        //设置一个逻辑点的值
		BEGP_FUNCODE_SET_LOGICAL_POINT_VALUE_BY_NAME,//设置逻辑点的值，根据点的名字
		BEGP_FUNCODE_DELETE_LOGICAL_POINT,          //删除一个逻辑点
		BEGP_FUNCODE_GET_LOGICAL_POINT_PARAMETER = 20,   //获取一个逻辑点的所有配置参数
		BEGP_FUNCODE_GET_LOGICAL_POINT_ALARM_INFOR, //获取一个逻辑点的报警信息
		/* 场景相关 */
		BEGP_FUNCODE_ADD_SCENE,               //增加一个场景
		BEGP_FUNCODE_SET_SCENE_PARAMETER,     //设置一个场景的所有参数
		BEGP_FUNCODE_DELETE_SCENE,            //删除一个场景
		BEGP_FUNCODE_GET_SCENE_PARAMETER = 25,     //获取一个场景的配置信息
		/* 计划相关 */
		BEGP_FUNCODE_ADD_SCHEDULER,           //增加一条计划
		BEGP_FUNCODE_SET_SCHEDULER_PARAMETER, //修改一条计划的所有参数
		BEGP_FUNCODE_DELETE_SCHEDULER,        //删除一条计划
		BEGP_FUNCODE_GET_SCHEDULER_PARAMETER, //获取一条计划的所有参数

		/*远程升级*/
		BEGP_FUNCUDE_IAP, //远程升级
	}BEGP_FUNCODE_TYPE_T;

	typedef enum
	{
		BEGP_PACKETDATA_ECHO_CODE_SUCCESS = 0,
		BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR,
		BEGP_PACKETDATA_ECHO_CODE_INDEXERR,
		BEGP_PACKETDATA_ECHO_CODE_VALUEERR,
		BEGP_PACKETDATA_ECHO_CODE_ERR
	}BEGP_PACKETDATA_ECHO_CODE_TYPE_T;

	/* 请求数据帧头 */
#define BEGP_REQ_HEAD_SIZE	sizeof(struct _BEGP_REQ_HEAD_)
	typedef struct _BEGP_REQ_HEAD_
	{
		uint8_t    SlaveAddress; //DDC用485通信时的地址
		uint8_t	   FunctionCode;  //功能码
		uint16_t   SequenceNumber; //序列号
		uint16_t   DataSize;  //要发送的数据长度
	}BEGP_REQ_HEAD_T;

	/* 响应数据帧头 */
#define BEGP_RSP_HEAD_SIZE   sizeof(struct _BEGP_RSP_HEAD_)
	typedef struct _BEGP_RSP_HEAD_
	{
		uint8_t    SlaveAddress; //DDC用485通信时的地址
		uint8_t	   FunctionCode;  //功能码
		uint16_t   SequenceNumber; //序列号
		uint16_t   DataSize;  //要发送的数据长度
	}BEGP_RSP_HEAD_T;

	/* 设置请求的响应数据正文 */
#define BEGP_RSP_PACKETDATA_ECHO_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_ECHO_)
	typedef struct _BEGP_RSP_PACKETDATA_ECHO_
	{
		uint8_t  Code;
	}BEGP_RSP_PACKETDATA_ECHO_T;

	/********************************** DDC相关 **********************************/
	/* 新增一个控制器 */
#define BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_)
	typedef struct _BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_
	{
		char      Name[32];//控制器的名字
		char      Alias[64];//控制器别名，支持汉字
		char      Description[128];//控制器的描述信息，支持汉字
		char      Location[64];//控制器的安装位置
		char      Zone[64];//区域信息
		uint16_t  ID;//控制器的ID，必须唯一
		uint8_t   Active;//控制器的使能状态，只有使能时才会正常工作，才会与下位机通信
	}BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T;

	/* 设置控制器的所有参数 */
#define  BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_)
	typedef struct  _BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_
	{
		char     Name[32];//控制器的名字
		char     Alias[64];//控制器别名，支持汉字
		char     Description[128];//控制器的描述信息，支持汉字
		char     Location[64];//控制器的安装位置
		char     Zone[64];//区域信息
		uint16_t ID;//控制器的ID，必须唯一
		uint8_t  Active;//控制器的使能状态，只有使能时才会正常工作，才会与下位机通信
	}BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_T;

	/* 设置控制器的时间 */
#define  BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_SIZE    sizeof(struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_)
	typedef struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_
	{
		uint8_t year;//年份表示2000年以后的年数如12代表2012年
		uint8_t month;//1-12
		uint8_t day;  //1-31
		uint8_t wday;  //星期几,取值范围MONDAY-SUNDAY(1-7)
		uint8_t hour; //0-23
		uint8_t minute; //0-59
		uint8_t second; //0-59
	}BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_T;

	/* 获取控制器的时间 */
#define BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_)
	typedef struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_
	{
		uint8_t year;  //年份表示2000年以后的年数如12代表2012年
		uint8_t month; //1-12
		uint8_t day;  //1-31
		uint8_t wday;  //星期几,取值范围MONDAY-SUNDAY(1-7)
		uint8_t hour; //0-23
		uint8_t minute; //0-59
		uint8_t second; //0-59
	}BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_T;

	/* 获取控制器的所有配置参数 */
#define BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_SIZE    sizeof(struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_)
	typedef struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_
	{
		char     Name[32];//控制器的名字
		char     Alias[64];//控制器别名，支持汉字
		char     Description[128];//控制器的描述信息，支持汉字
		char     Location[64];//控制器的安装位置
		char     Zone[64];//区域信息
		uint16_t ID;//控制器的ID，必须唯一
		uint8_t   Active;//控制器的使能状态，只有使能时才会正常工作，才会与下位机通信
	}BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_T;

	/* 获取控制器的所有配置参数 */
#define BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_SIZE    sizeof(struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_)
	typedef struct _BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_
	{
		char	Name[32];//名字
		char	CampanyName[32];//公司名字
		char	Model[12];//型号
		uint8_t	HWVer;//硬件版本号
		uint8_t	FWVer;//软件版本号
		uint8_t	IP[4];//IP地址
		uint8_t Mask[4];//Mask地址
		uint8_t	GateWay[4];//网关
		uint8_t Mac[6];//Mac地址
	}BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_T;

	/* 设置控制器的所有配置参数 */
#define BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_SIZE    sizeof(struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_)
	typedef struct _BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_
	{
		char	Name[32];//名字
		char	CampanyName[32];//公司名字
		char	Model[12];//型号
		uint8_t	HWVer;//硬件版本号
		uint8_t	FWVer;//软件版本号
		uint8_t	IP[4];//IP地址
		uint8_t Mask[4];//Mask地址
		uint8_t	GateWay[4];//网关
		uint8_t Mac[6];//Mac地址
	}BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_T;

	/********************************** 模块相关 **********************************/
	/*增加一个模块*/
#define BEGP_REQ_PACKETDATA_ADD_MODULE_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_ADD_MODULE_)
	typedef struct _BEGP_REQ_PACKETDATA_ADD_MODULE_
	{
		char     Name[32];//控制器的名字
		char     Alias[64];//模块的别名
		char     Description[128];//模块的描述信息
		char     Location[64];//模块的安装位置

		uint8_t  ModelID;//模块型号编码
		uint8_t  FlnID;//模块挂在哪条FLN总线上，可能取值FLN1、FLN2、FLN3、FLN4
		uint8_t  SlaveAddress;//模块的通信地址
		uint8_t  Active;//模块的使能状态 ACTIVE_ENABLE  使能  ACTIVE_DISABLE 禁用
		uint16_t ID;//模块的识别码
	}BEGP_REQ_PACKETDATA_ADD_MODULE_T;

	/* 配置模块的所有配置参数 */
#define BEGP_REQ_PACKETDATA_SET_MODULE_PARAMETER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_SET_MODULE_PARAMETER_)
	typedef struct _BEGP_REQ_PACKETDATA_SET_MODULE_PARAMETER_
	{
		char     Name[32];//控制器的名字
		char     Alias[64];//模块的别名
		char     Description[128];//模块的描述信息
		char     Location[64];//模块的安装位置

		uint8_t  ModelID;//模块型号编码
		uint8_t  FlnID;//模块挂在哪条FLN总线上，可能取值FLN1、FLN2、FLN3、FLN4
		uint8_t  SlaveAddress;//模块的通信地址
		uint8_t  Active;//模块的使能状态 ACTIVE_ENABLE  使能  ACTIVE_DISABLE 禁用
		uint16_t ID;//模块的识别码
	}BEGP_REQ_PACKETDATA_SET_MODULE_PARAMETER_T;

	/* 删除一个模块 */
#define BEGP_REQ_PACKETDATA_DELETE_MODULE_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_DELETE_MODULE_)
	typedef struct _BEGP_REQ_PACKETDATA_DELETE_MODULE_
	{
		uint16_t ID;//模块的识别码
	}BEGP_REQ_PACKETDATA_DELETE_MODULE_T;

	/* 获取模块的所有配置信息 */
	/* 请求帧数据 */
#define BEGP_REQ_PACKETDATA_GET_MODULE_PARAMETER_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_GET_MODULE_PARAMETER_)
	typedef struct _BEGP_REQ_PACKETDATA_GET_MODULE_PARAMETER_
	{
		uint16_t ID;//模块的识别码
	}BEGP_REQ_PACKETDATA_GET_MODULE_PARAMETER_T;
	/* 响应帧数据 */
#define BEGP_RSP_PACKETDATA_GET_MODULE_PARAMETER_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_GET_MODULE_PARAMETER_)
	typedef struct _BEGP_RSP_PACKETDATA_GET_MODULE_PARAMETER_
	{
		char     Name[32];//控制器的名字
		char     Alias[64];//模块的别名
		char     Description[128];//模块的描述信息
		char     Location[64];//模块的安装位置

		uint8_t  ModelID;//模块型号编码
		uint8_t  FlnID;//模块挂在哪条FLN总线上，可能取值FLN1、FLN2、FLN3、FLN4
		uint8_t  SlaveAddress;//模块的通信地址
		uint8_t  Active;//模块的使能状态 ACTIVE_ENABLE  使能  ACTIVE_DISABLE 禁用
		uint16_t ID;//模块的识别码
	}BEGP_RSP_PACKETDATA_GET_MODULE_PARAMETER_T;

	/* 获取模块的错误代码 */
	/*请求帧数据*/
#define BEGP_REQ_PACKETDATA_GET_MODULE_ERROR_CODE_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_GET_MODULE_ERROR_CODE_)
	typedef struct _BEGP_REQ_PACKETDATA_GET_MODULE_ERROR_CODE_
	{
		uint16_t ID;//模块的识别码
	}BEGP_REQ_PACKETDATA_GET_MODULE_ERROR_CODE_T;
	/*返回帧数据*/
#define BEGP_RSP_PACKETDATA_GET_MODULE_ERROR_CODE_SIZE  sizeof(struct _BEGP_RSP_PACKETDATA_GET_MODULE_ERROR_CODE_)
	typedef struct _BEGP_RSP_PACKETDATA_GET_MODULE_ERROR_CODE_
	{
		uint8_t ErrorCode;//模块的错误码
	}BEGP_RSP_PACKETDATA_GET_MODULE_ERROR_CODE_T;

	/********************************** 逻辑点相关 **********************************/
	/* 模拟量输入实点AI */
	typedef struct _BEGP_LOGICAL_POINT_PARAMETER_LAI_
	{
		uint8_t  PointType; //点类型  实点AI
		uint16_t ID; //点的ID
		uint8_t  Active;  //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char     Name[32]; //点的名字
		char     Alias[64]; //点的别名
		char     Description[64]; //描述
		uint16_t ModuleID;  //点所在模块的ID号
		uint8_t  ChannelIndex;  //点所在通道索引
		uint8_t  WorkMode;  //工作模式，手动模式或自动模式
		uint8_t  Polarity;  //极性，正向或反向
		int32_t  InitValue;//值

		/*报警相关*/
		uint8_t  AlarmActive;//报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		uint8_t  AlarmType;  //报警类型，不报警、标准报警、加强报警
		uint8_t  AlarmPriority;  //报警优先级

		uint8_t  LineType;//有效输入类型，包括0-10V、0-20mA、4-20mA
		char     Unit[12]; //工程单位，如℉、℃、KWh
		int32_t  HighRange; //最高阀值
		int32_t  LowRange;  //最低阀值
		uint8_t  Precision;  //精度
		int32_t  AlarmHighValue; //高限报警值
		int32_t  AlarmLowValue;//底限报警值
	}BEGP_LOGICAL_POINT_PARAMETER_LAI_T;
	/* 模拟量输出实点AO */
	typedef struct _BEGP_LOGICAL_POINT_PARAMETER_LAO_
	{
		uint8_t  PointType;  //点类型  实点AO
		uint16_t ID; //点的ID
		uint8_t  Active;     //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char     Name[32];  //点的名字
		char     Alias[64]; //点的别名
		char     Description[64];//描述
		uint16_t ModuleID; //点所在模块的ID号
		uint8_t  ChannelIndex; //点所在通道索引
		uint8_t  WorkMode;  //工作模式，手动模式或自动模式
		uint8_t  Polarity;  //极性，正向或反向
		int32_t  InitValue;//初始值

		/*报警相关*/
		uint8_t AlarmActive;  //报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		uint8_t AlarmType;   //报警类型，不报警、标准报警、加强报警
		uint8_t AlarmPriority;   //报警优先级

		uint8_t LineType;  //有效输出类型，包括0-10V、0-20mA、4-20mA
		char    Unit[12];  //工程单位，如℉、℃、KWh
		int32_t HighRange;  //最高阀值
		int32_t LowRange;   //最低阀值
		uint8_t Precision;   //精度
		int32_t AlarmHighValue;  //高限报警值
		int32_t AlarmLowValue;  //底限报警值
	}BEGP_LOGICAL_POINT_PARAMETER_LAO_T;
	/* 数字量输入实点DI */
	typedef struct _BEGP_LOGICAL_POINT_PARAMETER_LDI_
	{
		uint8_t  PointType;  //点类型    实点DI
		uint16_t ID; //点的ID
		uint8_t  Active;     //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char     Name[32];  //点的名字
		char     Alias[64]; //点的别名
		char     Description[64];//描述
		uint16_t ModuleID; //点所在模块的ID号
		uint8_t  ChannelIndex; //点所在通道索引
		uint8_t  WorkMode;  //工作模式，手动模式或自动模式
		uint8_t  Polarity;  //极性，正向或反向
		int32_t  InitValue;//值

		/*报警相关*/
		uint8_t AlarmActive;  //报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		uint8_t AlarmType;   //报警类型，不报警、标准报警、加强报警
		uint8_t AlarmPriority;   //报警优先级

		uint8_t AlarmTrigger;//报警触发
		uint8_t ActivePassive;//有源无源
	}BEGP_LOGICAL_POINT_PARAMETER_LDI_T;
	/* 数字量输出实点DO */
	typedef struct _BEGP_LOGICAL_POINT_PARAMETER_LDO_
	{
		uint8_t  PointType;  //点类型   实点DO
		uint16_t ID; //点的ID
		uint8_t  Active;     //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char     Name[32];  //点的名字
		char     Alias[64]; //点的别名
		char     Description[64];//描述
		uint16_t ModuleID; //点所在模块的ID号
		uint8_t  ChannelIndex; //点所在通道索引
		uint8_t  WorkMode;  //工作模式，手动模式或自动模式
		uint8_t  Polarity;  //极性，正向或反向
		int32_t  InitValue;//初始值

		/*报警相关*/
		uint8_t AlarmActive;  //报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		uint8_t AlarmType;   //报警类型，不报警、标准报警、加强报警
		uint8_t AlarmPriority;   //报警优先级

		uint8_t AlarmTrigger;//报警触发  开或关
	}BEGP_LOGICAL_POINT_PARAMETER_LDO_T;
	/* 虚点AI */
	typedef struct _BEGP_LOGICAL_POINT_PARAMETER_VAI_
	{
		uint8_t   PointType;  //点类型  虚点AI
		uint16_t  ID; //点的ID
		uint8_t   Active;     //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char      Name[32];  //点的名字
		char      Alias[64]; //点的别名
		char      Description[64];//描述
		int32_t   InitValue;//初始值
		char      Unit[12];  //工程单位，如℉、℃、KWh
		uint8_t   Precision;  //精度

		/*报警相关*/
		uint8_t  AlarmActive;  //报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		uint8_t  AlarmType;   //报警类型，不报警、标准报警、加强报警
		uint8_t  AlarmPriority;   //报警优先级
		int32_t  AlarmHighValue;
		int32_t  AlarmLowValue;
	}BEGP_LOGICAL_POINT_PARAMETER_VAI_T;
	/* 虚点AO */
	typedef struct _BEGP_LOGICAL_POINT_PARAMETER_VAO_
	{
		uint8_t   PointType;  //点类型  虚点AO
		uint16_t  ID; //点的ID
		uint8_t   Active;     //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char      Name[32];  //点的名字
		char      Alias[64]; //点的别名
		char      Description[64];//描述
		int32_t   InitValue;//初始值
		char      Unit[12];  //工程单位，如℉、℃、KWh
		uint8_t   Precision;  //精度

		/*报警相关*/
		uint8_t  AlarmActive;  //报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		uint8_t  AlarmType;   //报警类型，不报警、标准报警、加强报警
		uint8_t  AlarmPriority;   //报警优先级
		int32_t  AlarmHighValue;
		int32_t  AlarmLowValue;
	}BEGP_LOGICAL_POINT_PARAMETER_VAO_T;
	/* 虚点DI */
	typedef struct _BEGP_LOGICAL_POINT_PARAMETER_VDI_
	{
		uint8_t  PointType;  //点类型  虚点DI
		uint16_t ID; //点的ID
		uint8_t  Active;     //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char     Name[32];  //点的名字
		char     Alias[64]; //点的别名
		char     Description[64];//描述
		int32_t  InitValue;//初始值

		/*报警相关*/
		uint8_t AlarmActive;  //报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		uint8_t AlarmType;   //报警类型，不报警、标准报警、加强报警
		uint8_t AlarmPriority;   //报警优先级

		uint8_t AlarmTrigger;//报警触发  开或关
	}BEGP_LOGICAL_POINT_PARAMETER_VDI_T;
	/* 虚点DO */
	typedef struct _BEGP_LOGICAL_POINT_PARAMETER_VDO_
	{
		uint8_t  PointType;  //点类型  虚点DO
		uint16_t ID; //点的ID
		uint8_t  Active;     //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char     Name[32];  //点的名字
		char     Alias[64]; //点的别名
		char     Description[64];//描述
		int32_t  InitValue;//初始值

		/*报警相关*/
		uint8_t AlarmActive;  //报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		uint8_t AlarmType;   //报警类型，不报警、标准报警、加强报警
		uint8_t AlarmPriority;   //报警优先级

		uint8_t AlarmTrigger;//报警触发  开或关
	}BEGP_LOGICAL_POINT_PARAMETER_VDO_T;

	/* 增加一个逻辑点 */
#define BEGP_REQ_PACKETDATA_ADD_LOGICAL_POINT_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_ADD_LOGICAL_POINT_)
	typedef struct _BEGP_REQ_PACKETDATA_ADD_LOGICAL_POINT_
	{
		uint8_t   PointType;//点类型
		char      Tab[sizeof(struct _BEGP_LOGICAL_POINT_PARAMETER_LAO_)];//数组长度是模拟量输出
	}BEGP_REQ_PACKETDATA_ADD_LOGICAL_POINT_T;

	/* 设置一个逻辑点的所有参数 */
#define BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_)
	typedef struct _BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_
	{
		uint8_t   PointType;//点类型
		char      Tab[sizeof(struct _BEGP_LOGICAL_POINT_PARAMETER_LAO_)];//数组长度是模拟量输出
	}BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_T;

	/*设置逻辑点的值*/
#define BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_)
	typedef struct _BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_
	{
		uint16_t  ID; //点的ID
		uint8_t   WorkMode;  //工作模式，手动模式或自动模式
		int32_t   Value;//初始值
	}BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_T;

	/*设置逻辑点的值*/
#define BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_BY_NAME_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_BY_NAME_)
	typedef struct _BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_BY_NAME_
	{
		char Name[64];
		float  Value;//初始值
	}BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_BY_NAME_T;

	/* 删除一个逻辑点 */
#define BEGP_REQ_PACKETDATA_DELETE_LOGICAL_POINT_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_DELETE_LOGICAL_POINT_)
	typedef struct _BEGP_REQ_PACKETDATA_DELETE_LOGICAL_POINT_
	{
		uint16_t  ID; //点的ID
	}BEGP_REQ_PACKETDATA_DELETE_LOGICAL_POINT_T;


	/*获取一个逻辑点的所有参数*/
	/* 请求帧 */
#define BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_)
	typedef struct _BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_
	{
		uint16_t  ID; //点的ID
	}BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_T;
	/* 响应帧 */
#define BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_SIZE  sizeof(struct _BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_)
	typedef struct _BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_
	{
		uint8_t  PointType;//点类型
		char     Tab[sizeof(struct _BEGP_LOGICAL_POINT_PARAMETER_LAO_)];//数组长度是模拟量输出
	}BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_T;

	/*获取一个逻辑点的报警信息*/
	/*请求帧*/
#define BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_)
	typedef struct _BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_
	{
		uint16_t  ID; //点的ID
	}BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T;
	/*响应帧*/
#define  BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_SIZE   sizeof(struct _BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_)
	typedef struct _BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_
	{
		int32_t  CurrentValue;//获取的当前值
		uint8_t  AlarmState;  //报警状态
		int32_t  AlarmTriggerValue; //首次报警触发值
		int32_t  AlarmMaxValue;     //报警最大值(即发生过的最大一次报警值)
	}BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T;

	/********************************** 和场景相关 **********************************/
	/*控制场景*/
	typedef struct _BEGP_CONTROL_POINT_
	{
		uint8_t    Active;   //使能状态
		uint16_t   PointID;	 //计划所对应点的ID号
		int32_t    RunValue; //运行值,数字量0或255，模拟量是个数值
	}BEGP_CONTROL_POINT_T;
	typedef struct _BEGP_CONTROL_MODE_
	{
		uint8_t                Active;  //使能状态
		char                   Name[32];  //场景模式名称
		uint8_t                ID;   //场景模式ID
		BEGP_CONTROL_POINT_T   ControlPoint[CONTROL_POINT_SIZE];//模式里点信息
	}BEGP_CONTROL_MODE_T;
	typedef struct _BEGP_CONTROL_SCENE_PARAMETER_
	{
		uint8_t               SceneType; //场景类型  控制或采集场景，取计划定义的值
		uint16_t              ID;       //控制场景的ID
		uint8_t               Active;  //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char                  Name[32];  //场景名字
		char                  Alias[64]; //场景别名
		BEGP_CONTROL_MODE_T   ControlMode[CONTROL_MODE_SIZE]; //场景模式
	}BEGP_CONTROL_SCENE_PARAMETER_T;

	/* 采集场景 */
	typedef struct _BEGP_COLLECT_POINT_
	{
		uint8_t     Active;   //使能状态
		uint16_t    PointID;	 //计划所对应点的ID号
		uint8_t     CollectID;//采集方式，时间间隔采集、值采集
		uint8_t     TimeUnit;//时间间隔单位
		int16_t     Value; //采集时间间隔值、变化量的值
	}BEGP_COLLECT_POINT_T;
	typedef struct _BEGP_COLLECT_MODE_
	{
		uint8_t                  Active;  //使能状态
		char                     Name[32];  //场景模式名称
		uint8_t                  ID;   //场景模式ID
		BEGP_COLLECT_POINT_T     CollectPoint[COLLECT_POINT_SIZE];//模式里点信息
	}BEGP_COLLECT_MODE_T;
	typedef struct _BEGP_COLLECT_SCENE_PARAMETER_
	{
		uint8_t              SceneType; //场景类型  控制或采集场景，取计划定义的值
		uint16_t             ID;        //控制场景的ID
		uint8_t              Active;  //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char                 Name[32];  //场景名字
		char                 Alias[64]; //场景别名
		BEGP_COLLECT_MODE_T  CollectMode[COLLECT_MODE_SIZE]; //场景模式
	}BEGP_COLLECT_SCENE_PARAMETER_T;

	/* 添加一个场景 */
#define  BEGP_REQ_PACKETDATA_ADD_SCENE_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_ADD_SCENE_)
	typedef struct _BEGP_REQ_PACKETDATA_ADD_SCENE_
	{
		uint8_t SceneType; //场景类型  控制或采集场景
		char    Tab[sizeof(struct _BEGP_COLLECT_SCENE_PARAMETER_) > sizeof(struct _BEGP_CONTROL_SCENE_PARAMETER_) ?
			sizeof(struct _BEGP_COLLECT_SCENE_PARAMETER_) : sizeof(struct _BEGP_CONTROL_SCENE_PARAMETER_)];//取控制场景和采集场景较大的值
	}BEGP_REQ_PACKETDATA_ADD_SCENE_T;

	/*修改一个场景的所有配置参数*/
#define  BEGP_REQ_PACKETDATA_SET_SCENE_PARAMETER_SIZE   sizeof(struct _BEGP_REQ_PACKETDATA_SET_SCENE_PARAMETER_)
	typedef struct _BEGP_REQ_PACKETDATA_SET_SCENE_PARAMETER_
	{
		uint8_t SceneType; //场景类型  控制或采集场景
		char    Tab[sizeof(struct _BEGP_COLLECT_SCENE_PARAMETER_) > sizeof(struct _BEGP_CONTROL_SCENE_PARAMETER_) ?
			sizeof(struct _BEGP_COLLECT_SCENE_PARAMETER_) : sizeof(struct _BEGP_CONTROL_SCENE_PARAMETER_)];//取控制场景和采集场景较大的值
	}BEGP_REQ_PACKETDATA_SET_SCENE_PARAMETER_T;

	/* 删除一个场景 */
#define  BEGP_REQ_PACKETDATA_DELETE_SCENE_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_DELETE_SCENE_)
	typedef struct _BEGP_REQ_PACKETDATA_DELETE_SCENE_
	{
		uint16_t ID;//场景ID	
	}BEGP_REQ_PACKETDATA_DELETE_SCENE_T;

	/* 获取一个场景的配置信息 */
	/* 请求帧 */
#define  BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_) 
	typedef struct _BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_
	{
		uint16_t ID;//场景ID
	}BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_T;
	/* 响应帧 */
#define  BEGP_RSP_PACKETDATA_GET_SCENE_PARAMETER_SIZE  sizeof(struct _BEGP_RSP_PACKETDATA_GET_SCENE_PARAMETER_)
	typedef struct _BEGP_RSP_PACKETDATA_GET_SCENE_PARAMETER_
	{
		uint8_t SceneType; //场景类型  控制或采集场景
		char    Tab[sizeof(struct _BEGP_COLLECT_SCENE_PARAMETER_) > sizeof(struct _BEGP_CONTROL_SCENE_PARAMETER_) ?
			sizeof(struct _BEGP_COLLECT_SCENE_PARAMETER_) : sizeof(struct _BEGP_CONTROL_SCENE_PARAMETER_)];//取控制场景和采集场景较大的值
	}BEGP_RSP_PACKETDATA_GET_SCENE_PARAMETER_T;

	/********************************** 和计划相关 **********************************/
	/* 添加一条计划 */
#define  BEGP_REQ_PACKETDATA_ADD_SCHEDULER_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_ADD_SCHEDULER_)
	typedef struct _BEGP_REQ_PACKETDATA_ADD_SCHEDULER_
	{
		uint16_t ID;//计划的ID号
		uint8_t  Active;  //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char     Name[32];  //计划的名字
		char     Alias[64];  //别名
		uint8_t  PlanType;  //计划类型  常规计划或节假日计划

		uint8_t  StartDateYear;//开始日期年，如14
		uint8_t  StartDateMonth; //开始日期月，1-12
		uint8_t  StartDateDay; //开始日期日，1-31
		uint8_t  EndDateYear; //结束日期年
		uint8_t  EndDateMonth; //结束日期月
		uint8_t  EndDateDay; //结束日期日

		uint8_t  StartTimeHour;// 开始时间时，0-23
		uint8_t  StartTimeMinute; //开始时间分,0-59
		uint8_t  StartTimeSecond; //开始时间秒,0-59
		uint8_t  EndTimeHour;//结束时间时
		uint8_t  EndTimeMinute;//结束时间分
		uint8_t  EndTimeSecond;//结束时间秒

		uint8_t  ExecutionWay; //执行方式（每天执行、持续执行）
		uint8_t  WeekMask;	 //周掩码
		uint8_t  SceneType; //场景类型，控制场景或采集场景
		uint16_t SceneID;  //场景ID号
		uint8_t  ModeID;  //模式ID号
		uint8_t  DefaultModeID;//默认模式ID
	}BEGP_REQ_PACKETDATA_ADD_SCHEDULER_T;

	/* 设置一条计划的所有参数 */
#define  BEGP_REQ_PACKETDATA_SET_SCHEDULER_PARAMETER_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_SET_SCHEDULER_PARAMETER_)
	typedef struct _BEGP_REQ_PACKETDATA_SET_SCHEDULER_PARAMETER_
	{
		uint16_t ID;//计划的ID号
		uint8_t  Active;  //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char     Name[32];  //计划的名字
		char     Alias[64];  //别名
		uint8_t  PlanType;  //计划类型  常规计划或节假日计划

		uint8_t  StartDateYear;//开始日期年，如14
		uint8_t  StartDateMonth; //开始日期月，1-12
		uint8_t  StartDateDay; //开始日期日，1-31
		uint8_t  EndDateYear; //结束日期年
		uint8_t  EndDateMonth; //结束日期月
		uint8_t  EndDateDay; //结束日期日

		uint8_t  StartTimeHour;// 开始时间时，0-23
		uint8_t  StartTimeMinute; //开始时间分,0-59
		uint8_t  StartTimeSecond; //开始时间秒,0-59
		uint8_t  EndTimeHour;//结束时间时
		uint8_t  EndTimeMinute;//结束时间分
		uint8_t  EndTimeSecond;//结束时间秒

		uint8_t  ExecutionWay; //执行方式（每天执行、持续执行）
		uint8_t  WeekMask;	 //周掩码
		uint8_t  SceneType; //场景类型，控制场景或采集场景
		uint16_t SceneID;  //场景ID号
		uint8_t  ModeID;  //模式ID号
		uint8_t  DefaultModeID;//默认模式ID
	}BEGP_REQ_PACKETDATA_SET_SCHEDULER_PARAMETER_T;

	/* 删除一条计划 */
#define BEGP_REQ_PACKETDATA_DELETE_SCHEDULER_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_DELETE_SCHEDULER_)
	typedef struct _BEGP_REQ_PACKETDATA_DELETE_SCHEDULER_
	{
		uint16_t ID;//计划的ID号
	}BEGP_REQ_PACKETDATA_DELETE_SCHEDULER_T;

	/* 获取一条计划的所有参数 */
	/* 请求帧 */
#define BEGP_REQ_PACKETDATA_GET_SCHEDULER_PARAMETER_SIZE  sizeof(struct _BEGP_REQ_PACKETDATA_GET_SCHEDULER_PARAMETER_)
	typedef struct _BEGP_REQ_PACKETDATA_GET_SCHEDULER_PARAMETER_
	{
		uint16_t ID;//计划的ID号
	}BEGP_REQ_PACKETDATA_GET_SCHEDULER_PARAMETER_T;
	/* 响应帧 */
#define BEGP_RSP_PACKETDATA_GET_SCHEDULER_PARAMETER_SIZE  sizeof(struct _BEGP_RSP_PACKETDATA_GET_SCHEDULER_PARAMETER_)
	typedef struct _BEGP_RSP_PACKETDATA_GET_SCHEDULER_PARAMETER_
	{
		uint8_t  Active;  //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
		char     Name[32];  //计划的名字
		char     Alias[64];  //别名
		uint8_t  PlanType;  //计划类型  常规计划或节假日计划

		uint8_t  StartDateYear;//开始日期年，如14
		uint8_t  StartDateMonth; //开始日期月，1-12
		uint8_t  StartDateDay; //开始日期日，1-31
		uint8_t  EndDateYear; //结束日期年
		uint8_t  EndDateMonth; //结束日期月
		uint8_t  EndDateDay; //结束日期日

		uint8_t  StartTimeHour;// 开始时间时，0-23
		uint8_t  StartTimeMinute; //开始时间分,0-59
		uint8_t  StartTimeSecond; //开始时间秒,0-59
		uint8_t  EndTimeHour;//结束时间时
		uint8_t  EndTimeMinute;//结束时间分
		uint8_t  EndTimeSecond;//结束时间秒

		uint8_t  ExecutionWay; //执行方式（每天执行、持续执行）
		uint8_t  WeekMask;	 //周掩码
		uint8_t  SceneType; //场景类型，控制场景或采集场景
		uint16_t SceneID;  //场景ID号
		uint8_t  ModeID;  //模式ID号
		uint8_t  DefaultModeID;//默认模式ID	
	}BEGP_RSP_PACKETDATA_GET_SCHEDULER_PARAMETER_T;

#pragma pack()
}

#endif