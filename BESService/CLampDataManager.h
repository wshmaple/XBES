#pragma once



#include "CCrossDataManager.h"

/**
 * Manager for lamp data. 主要处理灯光的相关操作的
 *
 * @author WSH
 * @date 2018/9/14
 */
#define   LAMP_CONTROL_MODE_SIZE    512
#define   LAMP_COLLECT_MODE_SIZE    512
#define   LAMP_CONTROL_POINT_SIZE    512
#define   LAMP_COLLECT_POINT_SIZE    100
#pragma pack(1)
typedef struct _LAMP_BEGP_REQ_PACKETDATA_ADD_MODULE_
{
	char     Name[32];//模块的名字
	char     Alias[64];//模块的别名
	char     Description[128];//模块的描述信息
	char     Location[64];//模块的安装位置

	uint8_t  ModelID;//模块型号编码
	uint8_t  AreaNum;     //区域号1-255
	uint8_t	 BranchNum;   //支路号1-255
	uint8_t  SlaveAddress;//模块的通信地址
	uint8_t  Active;//模块的使能状态 ACTIVE_ENABLE  使能  ACTIVE_DISABLE 禁用
	uint16_t ID;//模块的识别码
}LAMP_BEGP_REQ_PACKETDATA_ADD_MODULE_T;


/********************************** 和场景相关 **********************************/
/*控制场景*/
typedef struct  _LAMP_BEGP_CONTROL_POINT_
{
	uint8_t    Active;   //使能状态
	uint16_t   PointID;	 //计划所对应点的ID号
	int32_t    RunValue; //运行值,数字量0或255，模拟量是个数值
	uint8_t    DelayTime;//延时执行时间
}LAMP_BEGP_CONTROL_POINT_T;
typedef struct  _LAMP_BEGP_CONTROL_MODE_
{
	uint8_t                Active;  //使能状态
	char                   Name[32];  //场景模式名称
	uint8_t                ID;   //场景模式ID
	LAMP_BEGP_CONTROL_POINT_T   ControlPoint[LAMP_CONTROL_POINT_SIZE];//模式里点信息
}LAMP_BEGP_CONTROL_MODE_T;
typedef struct  _LAMP_BEGP_CONTROL_SCENE_PARAMETER_
{
	uint8_t               SceneType; //场景类型  控制或采集场景，取计划定义的值
	uint16_t              ID;       //控制场景的ID
	uint8_t               Active;  //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
	char                  Name[32];  //场景名字
	char                  Alias[64]; //场景别名
	LAMP_BEGP_CONTROL_MODE_T   ControlMode[LAMP_CONTROL_MODE_SIZE]; //场景模式
}LAMP_BEGP_CONTROL_SCENE_PARAMETER_T;

/* 采集场景 */
typedef struct  _LAMP_BEGP_COLLECT_POINT_
{
	uint8_t     Active;   //使能状态
	uint16_t    PointID;	 //计划所对应点的ID号
	uint8_t     CollectID;//采集方式，时间间隔采集、值采集
	uint8_t     TimeUnit;//时间间隔单位
	int16_t     Value; //采集时间间隔值、变化量的值
}LAMP_BEGP_COLLECT_POINT_T;
typedef struct  _LAMP_BEGP_COLLECT_MODE_
{
	uint8_t                  Active;  //使能状态
	char                     Name[32];  //场景模式名称
	uint8_t                  ID;   //场景模式ID
	LAMP_BEGP_COLLECT_POINT_T     CollectPoint[LAMP_COLLECT_POINT_SIZE];//模式里点信息
}LAMP_BEGP_COLLECT_MODE_T;
typedef struct  _LAMP_BEGP_COLLECT_SCENE_PARAMETER_
{
	uint8_t              SceneType; //场景类型  控制或采集场景，取计划定义的值
	uint16_t             ID;        //控制场景的ID
	uint8_t              Active;  //是否启用，ACTIVE_ENABLE使能，ACTIVE_DISABLE禁止
	char                 Name[32];  //场景名字
	char                 Alias[64]; //场景别名
	LAMP_BEGP_COLLECT_MODE_T  CollectMode[LAMP_COLLECT_MODE_SIZE]; //场景模式
}LAMP_BEGP_COLLECT_SCENE_PARAMETER_T;
#pragma pack()
class CLampDataManager :
	public CCrossDataManager
{
public:
	CLampDataManager();
	~CLampDataManager();

	// 总的控制

	// 模块
	virtual  int   AddOrSetControllerModule(Json::Value jsonData, char *sendbuf);
	virtual  int   AddOrSetControllerModule2(Json::Value jsonData, char *sendbuf);
	virtual  Json::Value ParseGetModePar(void *pRecvData);//获取模块信息
	// 场景
	virtual  int   AddScene(Json::Value jsonData, char * sendbuf); // 增加一个场景
	virtual  int   SetScene(Json::Value jsonData, char * sendbuf); // 设置一个场景
	virtual  Json::Value ParseGetSence(void *pRecvData);	// 获取场景数据
};

