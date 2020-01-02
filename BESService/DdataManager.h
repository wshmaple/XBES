#pragma once
#include "json/json.h"
#include <string>
#include "Logs.h"

#include "BES200Manager.h"
using namespace std;
class CDdataManager
{
public:
	CDdataManager();
	~CDdataManager();

	// 统一的消息 入口函数
	Json::Value ComDataManager(Json::Value root);
	void ComDataManager2(Json::Value root, std::string& result);

public:

	/*函数描述：新增一个控制器
	*函数参数：javaWeb客户端发来的jsons数据
	*返回值：函数的错误信息
	*说明：将json数据拆分重组发给下位机
	*/
	Json::Value AddCollectControllerParameter(Json::Value jsonData);


	/*函数描述：设置一个控制器的所有参数
	*函数参数：javaWeb客户端发来的jsons数据
	*返回值：函数的错误信息
	*说明：将json数据拆分重组发给下位机
	*/
	Json::Value SetCollectControllerParameter(Json::Value jsonData);
	Json::Value SetCollectControllerParameter2(Json::Value jsonData);


	/*函数描述：设置控制器的时间
	*函数参数：javaWeb客户端发来的jsons数据
	*返回值：函数的错误信息
	*说明：将json数据拆分重组发给下位机
	*/
	Json::Value SetCollectControllerTime(Json::Value jsonData);


	/*函数描述：重启DDC,相当于软件复位
	*函数参数：javaWeb客户端发来的jsons数据
	*返回值：函数的错误信息
	*说明：将json数据拆分重组发给下位机
	*/
	Json::Value RebootCollectController(Json::Value jsonData);


	/*函数描述：重置控制器，恢复出厂设置，并重启
	*函数参数：javaWeb客户端发来的jsons数据
	*返回值：函数的错误信息
	*说明：将json数据拆分重组发给下位机
	*/
	Json::Value ResetCollectControler(Json::Value jsonData);


	/*函数描述：获取控制器的时间
	*函数参数：javaWeb客户端发来的jsons数据
	*返回值：函数的错误信息及数据
	*说明：将json数据拆分重组发给下位机
	*/
	Json::Value GetCollectControllerTime(Json::Value jsonData);


	/*函数描述：获取控制器的所有配置参数
	*函数参数：javaWeb客户端发来的jsons数据
	*返回值：函数的错误信息及数据
	*说明：将json数据拆分重组发给下位机
	*/
	Json::Value GetCollectControllerParameter(Json::Value jsonData);


	/*函数描述：广播方式与其他控制器通信，获取所有控制器的信息
	*函数参数：javaWeb客户端发来的jsons数据
	*返回值：函数的错误信息及数据
	*说明：将json数据拆分重组发给下位机
	*/
	Json::Value GetCollectControllerInfoByBroadcast();


	/*函数描述：广播方式与其他控制器通信，根据MAC地址来设置其他参数信息，主要是通过MAC修改IP等配置信息
	*函数参数：javaWeb客户端发来的jsons数据
	*返回值：函数的错误信息及数据
	*说明：将json数据拆分重组发给下位机
	*/
	Json::Value SetCollectControllerInfoByBroadcast(Json::Value jsonData);


	/*函数描述：删除一个控制器，并删除和该控制器相关的模块、点
	*函数参数：javaWeb客户端发来的jsons数据
	*返回值：函数的错误信息及数据
	*说明：将json数据拆分重组发给下位机
	*/
	Json::Value DeleteCollectController(Json::Value jsonData);


	/*函数描述：进行远程升级，下载程序
	*函数参数：javaWeb客户端发来的jsons数据
	*返回值：函数的错误信息及数据
	*说明：将json数据拆分重组发给下位机
	*/
	Json::Value  FunIAP(Json::Value jsonData);


	/*函数描述：在控制器下增加一个模块并配置好模块的相应参数
	*函数参数：javaWeb客户端发来的jsons数据
	*返回值：函数的错误信息及数据
	*说明：将json数据拆分重组发给下位机
	*/
	Json::Value AddMeter(Json::Value jsonData);


	/*函数描述：配置模块的所有配置参数
	*函数参数：javaWeb客户端发来的jsons数据
	*返回值：函数的错误信息及数据
	*说明：将json数据拆分重组发给下位机
	*/
	Json::Value SetMeterParameter(Json::Value jsonData);


	/*函数描述：删除一个模块，并删除和模块相关的点
	*函数参数：javaWeb客户端发来的jsons数据
	*返回值：函数的错误信息及数据
	*说明：将json数据拆分重组发给下位机
	*/
	Json::Value DeleteMeter(Json::Value jsonData);


	/*函数描述：获取模块的所有配置信息
	*函数参数：javaWeb客户端发来的jsons数据
	*返回值：函数的错误信息及数据
	*说明：将json数据拆分重组发给下位机
	*/
	Json::Value GetMeterParameter(Json::Value jsonData);


	/*获取电表的所有电表电能参数
	*函数参数：javaWeb客户端发来的jsons数据
	*返回值：函数的错误信息及数据
	*说明：将json数据拆分重组发给下位机
	*/
	Json::Value GetMeterElectricData(Json::Value jsonData);

	void GetMeterElectricData2(Json::Value jsonData, std::string &result);


	/*
	*函数描述：获取电表的历史电表电能参数
	*函数参数：javaWeb客户端发来的jsons数据
	*返回值：函数的错误信息及数据
	*说明：将json数据拆分重组发给下位机
	*/
	Json::Value GetMeterHisElectricData(Json::Value jsonData);


	/*
	*	函数描述：	实时获取采集器的报警信息
	*函数参数：javaWeb客户端发来的jsons数据
	*返回值：函数的错误信息及数据
	*说明：将json数据拆分重组发给下位机
	*/
	Json::Value GetAlarmInfor(Json::Value jsonData);

	//测试接口
	Json::Value Test(Json::Value jsonData);

	//将无符号字符转字符串
	string Getstring(unsigned char number);

	//将无符号字符转字符串
	string Getstring2(unsigned char number);
	//将无符号int转字符串
	string Getstring3(unsigned int number);
	//将十六进制字符串转换为int
	int GetDecimalism(string strNumber);


	//将整数转换为十六进制字符串
	string dec2hex(int number);
	private:
		//CLogs m_logs;

		CBES200Manager m_besManager;

};

