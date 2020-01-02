#pragma once
#include "ComDef.h"
#include "Gloable_Cross.h"
using namespace Gloable_Cross;
class CCrossFunManager
{
public:
	CCrossFunManager();
	~CCrossFunManager();

	/*************************************原TestDllDemo中链接库函数声明*********************************/
	/*函数描述：新增一个控制器
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          ControllerParameter 需要配置的参数
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_AddControllerParameter(const char *IPAddr, const struct ControllerParameter *pParameter, char **errmsg);

	/*函数描述：设置一个控制器的所有参数
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          ControllerParameter 需要配置的参数
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_SetControllerParameter(const char *IPAddr, const struct ControllerParameter *pParameter, char **errmsg);

	/*函数描述：设置控制器的时间
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          pTime 需要配置的时间
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_SetControllerTime(const char *IPAddr, const struct tm *pTime, char **errmsg);

	/*函数描述：重启DDC,相当于软件复位
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_RebootController(const char *IPAddr, char **errmsg);

	/*函数描述：重置控制器，恢复出厂设置，并重启
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_ResetControler(const char *IPAddr, char **errmsg);


	/*函数描述：进行远程升级，下载程序
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int  BES200_FunIAP(const char *IPAddr, char **errmsg);
	/*函数描述：获取控制器的时间
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          pTime 获取到的时间就存放在此结构体中
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_GetControllerTime(const char *IPAddr, struct tm *pTime, char **errmsg);

	/*函数描述：获取控制器的所有配置参数
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          pParameter 获取到的控制器的配置信息都存放在此结构体中
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_GetControllerParameter(const char *IPAddr, struct ControllerParameter *pParameter, char **errmsg);

	/*函数描述：广播方式与其他控制器通信，获取所有控制器的信息
	*函数参数：pInfo 获取到的控制器的配置信息都存放在此结构体数组中
	*          size pInfo结构体数组的长度
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg和info，即调用BES200_Free(errmsg)和BES200_Free(info)
	*/
	int BES200_GetControllerInfoByBroadcast(struct ControllerInfo **pInfo, int *size, char **errmsg);


	/*函数描述：广播方式与其他控制器通信，根据MAC地址来设置其他参数信息，主要是通过MAC修改IP等配置信息
	*函数参数：pInfo 将需要设置的控制器的信息放在此结构体中，会根据结构体中的MAC地址来设置对应的DDC控制器的其他属性
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg和info，即调用BES200_Free(errmsg)和BES200_Free(info)
	*/
	int BES200_SetControllerInfoByBroadcast(const struct ControllerInfo *pInfo, char **errmsg);

	/*函数描述：删除一个控制器，并删除和该控制器相关的模块、点
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_DeleteController(const char *IPAddr, char **errmsg);

	/*函数描述：在控制器下增加一个模块并配置好模块的相应参数
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          ModuleID 模块的ID识别码，必须唯一，且只有添加时可以指定，以后不允许修改
	*          pParameter 模块的所有配置参数
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_AddModule(const char *IPAddr, uint16_t ModuleID, const struct ModuleParameter *pParameter, char **errmsg);

	/*函数描述：配置模块的所有配置参数
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          ModuleID 模块的ID识别码，必须唯一，且只有添加时可以指定，以后不允许修改
	*          pParameter 模块的所有配置参数
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_SetModuleParameter(const char *IPAddr, uint16_t ModuleID, const struct ModuleParameter *pParameter, char **errmsg);

	/*函数描述：删除一个模块，并删除和模块相关的点
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          ModuleID 模块的ID识别码，必须唯一，且只有添加时可以指定，以后不允许修改
	*          pParameter 模块的所有配置参数
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_DeleteModule(const char *IPAddr, uint16_t ModuleID, char **errmsg);

	/*函数描述：获取模块的所有配置信息
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          ModuleID 模块的ID识别码，必须唯一，且只有添加时可以指定，以后不允许修改
	*          pParameter 模块的所有配置参数都存放在此结构体中
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_GetModuleParameter(const char *IPAddr, uint16_t ModuleID, struct ModuleParameter *pParameter, char **errmsg);

	/*函数描述：获取模块的错误代码
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          ModuleID 模块的ID识别码，必须唯一，且只有添加时可以指定，以后不允许修改
	*          ErrorCode 模块的错误代码，具体定义见***
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_GetModuleErrorCode(const char *IPAddr, uint16_t ModuleID, uint8_t *ErrorCode, char **errmsg);

	/*函数描述：添加一个逻辑点
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          LogicalPointID 点的ID识别码，必须唯一，且只有添加时可以指定，以后不允许修改
	*          LogicalPointParameter 需要配置的参数
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_AddLogicalPoint(const char *IPAddr, uint16_t LogicalPointID, const struct LogicalPointParameter *pParameter, char **errmsg);

	/*函数描述：设置一个逻辑点的所有参数
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	LogicalPointID 点的ID识别码，必须唯一，且只有添加时可以指定，以后不允许修改
	*          LogicalPointParameter 需要配置的参数
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_SetLogicalPointParameter(const char *IPAddr, uint16_t LogicalPointID, const struct LogicalPointParameter *pParameter, char **errmsg);

	/*函数描述：删除一个逻辑点的所有参数
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          LogicalPointID 点的ID识别码，必须唯一，且只有添加时可以指定，以后不允许修改
	*          LogicalPointParameter 需要删除点的参数
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_DeleteLogicalPoint(const char *IPAddr, uint16_t LogicalPointID, char **errmsg);

	/*函数描述：获取一个逻辑点的所有参数
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          LogicalPointID 点的ID识别码，必须唯一，且只有添加时可以指定，以后不允许修改
	*          LogicalPointParameter 需要获取的参数
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_GetLogicalPointParameter(const char *IPAddr, uint16_t LogicalPointID, struct LogicalPointParameter *pParameter, char **errmsg);

	/*函数描述：获取一个逻辑点的报警信息，包括点的当前值、点的报警状态、点的首次报警触发值和点的发生过的最大报警值
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          LogicalPointID 点的ID识别码，必须唯一，且只有添加时可以指定，以后不允许修改
	*          LogicalPointAlarmInfo 要获取的报警信息
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)*/
	int BES200_GetLogicalPointAlarmInfor(const char *IPAddr, uint16_t LogicalPointID, struct LogicalPointAlarmInfor *pParameter, char **errmsg);

	/*函数描述：添加一个场景
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          SceneID 场景的ID识别码，必须唯一，且只有添加时可以指定，以后不允许修改
	*          SceneParameter 需要配置的场景的所有参数
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_AddScene(const char *IPAddr, uint16_t SceneID, const struct SceneParameter *pParameter, char **errmsg);

	/*函数描述：修改一个场景
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          SceneID 场景的ID识别码，必须唯一，且只有添加时可以指定，以后不允许修改
	*          SceneParameter 需要修改的场景的所有参数
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_SetSceneParameter(const char *IPAddr, uint16_t SceneID, const struct SceneParameter *pParameter, char **errmsg);

	/*函数描述：删除一个场景
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          SceneID 场景的ID识别码，必须唯一，且只有添加时可以指定，以后不允许修改
	*          SceneType 需要删除的场景的类型
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_DeleteScene(const char *IPAddr, uint16_t SceneID, char **errmsg);

	/*函数描述：获取一个场景的配置信息
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          SceneID 场景的ID识别码，必须唯一，且只有添加时可以指定，以后不允许修改
	*          SceneType 需要获取的场景的类型
	*          SceneParameter 需要获取的场景的所有参数
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_GetSceneParameter(const char *IPAddr, uint16_t SceneID, struct SceneParameter *pParameter, char **errmsg);

	/*函数描述：添加一条计划
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          SchedulerID 计划的ID识别码，必须唯一，且只有添加时可以指定，以后不允许修改
	*          SchedulerParameter 需要配置的计划的所有参数
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_AddScheduler(const char *IPAddr, uint16_t SchedulerID, const struct SchedulerParameter *pParameter, char **errmsg);

	/*函数描述：设置一条计划的所有参数
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          SchedulerID 计划的ID识别码，必须唯一，且只有添加时可以指定，以后不允许修改
	*          SchedulerParameter 需要设置的计划的所有参数
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_SetSchedulerParameter(const char *IPAddr, uint16_t SchedulerID, const struct SchedulerParameter *pParameter, char **errmsg);

	/*函数描述：删除一条计划
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          SchedulerID 计划的ID识别码，必须唯一，且只有添加时可以指定，以后不允许修改
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_DeleteScheduler(const char *IPAddr, uint16_t SchedulerID, char **errmsg);

	/*函数描述：获取一条计划的所有参数
	*函数参数：IPAddr 控制器的IP地址，这个地址一般是通过串口连接的方式配置上的
	*          SchedulerID 计划的ID识别码，必须唯一，且只有添加时可以指定，以后不允许修改
	*          SchedulerParameter 需要设置的计划的所有参数
	*          errmsg 如果函数调用出错，会存储着错误的详细信息，函数调用完毕之后必须释放此空间
	*返回值：函数的错误代码 BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*注意：调用此函数之后一定要释放errmsg，即调用BES200_Free(errmsg)
	*/
	int BES200_GetSchedulerParameter(const char *IPAddr, uint16_t SchedulerID, struct SchedulerParameter *pParameter, char **errmsg);


	void BES200_Free(char **errmsg);


	int BES200_SetLogicalPointValue(const char *IPAddr, uint16_t LogicalPointID, const struct LogicalPointvalue *pParameter, char **errmsg);

	int BES200_SetLogicalPointValueByName(const char *IPAddr, const char * LogicalPointName, float Value, char **errmsg);


	uint16_t BES200_htons(uint16_t n);
	  

	uint16_t BES200_ntohs(uint16_t n);
	
};

