#include "stdafx.h"
#include "DdataManager.h"
//#include "BES200.h"
//#include "begp.h"
#include <time.h>
#include "fstream"
#include <sstream>
#include "gloable.h"
#ifndef _JSON_HPP_H_ 

#define _JSON_HPP_H_ 
#include "json.hpp" //  方法二
#include "csv.hpp"
#endif
using namespace Gloable_Bes;
CDdataManager::CDdataManager()
{
}


CDdataManager::~CDdataManager()
{
}

Json::Value CDdataManager::ComDataManager(Json::Value root)
{
	Json::Value jsonResult;
	int nIndex = -1;
	bool bItem = root["item"].empty();

	if (root["index"].isInt())
	{
		nIndex = root["index"].asUInt();
	}
	else if (root["index"].isString())
	{
		nIndex = atoi(root["index"].asString().c_str());
	}

	if (!bItem){
		// 不为空说明 不是能耗的 也就不用继续了吧

		return "";
	}
	
//	LOG_INFO(this_thread::get_id() << "开始处理能耗数据：");
	switch (nIndex)
	{
	case 0://新增一个控制器
		jsonResult = AddCollectControllerParameter(root);
		break;
	case 1:// 同步
		jsonResult = SetCollectControllerParameter(root);
		//	jsonResult = SetCollectControllerParameter2(root);
		break;
	case 2:	// 设置时间
		jsonResult = SetCollectControllerTime(root);
		break;
	case 3://重启DDC,相当于软件复位
		jsonResult = RebootCollectController(root);
		break;
	case 4://重置控制器，恢复出厂设置，并重启
		jsonResult = ResetCollectControler(root);
		break;
	case 5:// 获取时间
		jsonResult = GetCollectControllerTime(root);
		break;
	case 6:// 对比 获取控制器的所有配置参数
		jsonResult = GetCollectControllerParameter(root);
		break;
	case 7://广播方式与其他控制器通信，获取所有控制器的信息
		jsonResult = GetCollectControllerInfoByBroadcast();
		break;
	case 8://广播方式与其他控制器通信
		jsonResult = SetCollectControllerInfoByBroadcast(root);
		break;
	case 9://删除一个控制器，并删除和该控制器相关的模块、点
		jsonResult = DeleteCollectController(root);
		break;
	case 10://进行远程升级，下载程序
		jsonResult = FunIAP(root);
		break;
	case 11://在控制器下增加一个模块并配置好模块的相应参数
		jsonResult = AddMeter(root);
		break;
	case 12://配置模块的所有配置参数
		jsonResult = SetMeterParameter(root);
		break;
	case 13://删除一个模块，并删除和模块相关的点
		jsonResult = DeleteMeter(root);
		break;
	case 14://获取模块的所有配置信息
		jsonResult = GetMeterParameter(root);
		break;
	case 15://获取电表的所有电表电能参数
		jsonResult = GetMeterElectricData(root);
		//GetMeterElectricData2(root, jsonResult);
		break;
	case 16://获取电表的历史电表电能参数
		jsonResult = GetMeterHisElectricData(root);
		break;
	case 17: //实时获取采集器的报警信息
		jsonResult = GetAlarmInfor(root);
		break;
	}
//	LOG_INFO(this_thread::get_id() << "处理完成 JAVA 数据：结果" << jsonResult);
	return jsonResult;
}


void CDdataManager::ComDataManager2(Json::Value root, std::string& result)
{
	Json::Value jsonResult;
	int nIndex = -1;
	bool bItem = root["item"].empty();

	if (root["index"].isInt())
	{
		nIndex = root["index"].asUInt();
	}
	else if (root["index"].isString())
	{
		nIndex = atoi(root["index"].asString().c_str());
	}
	printf("处理能耗消息id=%d\n", nIndex);
	switch (nIndex)
	{
	case 0://新增一个控制器
		jsonResult = AddCollectControllerParameter(root);
		break;
	case 1:// 同步
		jsonResult = SetCollectControllerParameter(root);
		//	jsonResult = SetCollectControllerParameter2(root);
		break;
	case 2:	// 设置时间
		jsonResult = SetCollectControllerTime(root);
		break;
	case 3://重启DDC,相当于软件复位
		jsonResult = RebootCollectController(root);
		break;
	case 4://重置控制器，恢复出厂设置，并重启
		jsonResult = ResetCollectControler(root);
		break;
	case 5:// 获取时间
		jsonResult = GetCollectControllerTime(root);
		break;
	case 6:// 对比 获取控制器的所有配置参数
		jsonResult = GetCollectControllerParameter(root);
		break;
	case 7://广播方式与其他控制器通信，获取所有控制器的信息
		jsonResult = GetCollectControllerInfoByBroadcast();
		break;
	case 8://广播方式与其他控制器通信
		jsonResult = SetCollectControllerInfoByBroadcast(root);
		break;
	case 9://删除一个控制器，并删除和该控制器相关的模块、点
		jsonResult = DeleteCollectController(root);
		break;
	case 10://进行远程升级，下载程序
		jsonResult = FunIAP(root);
		break;
	case 11://在控制器下增加一个模块并配置好模块的相应参数
		jsonResult = AddMeter(root);
		break;
	case 12://配置模块的所有配置参数
		jsonResult = SetMeterParameter(root);
		break;
	case 13://删除一个模块，并删除和模块相关的点
		jsonResult = DeleteMeter(root);
		break;
	case 14://获取模块的所有配置信息
		jsonResult = GetMeterParameter(root);
		break;
	case 15://获取电表的所有电表电能参数
	//	jsonResult = GetMeterElectricData(root);
		GetMeterElectricData2(root, result);
		break;
	case 16://获取电表的历史电表电能参数
		jsonResult = GetMeterHisElectricData(root);

		//
		jsonResult["index"] = Json::Value(nIndex);
		//jsonResult["errmsg"] = Json::Value("硬件方法错误！");

		break;
	case 17: //实时获取采集器的报警信息
		jsonResult = GetAlarmInfor(root);
		break;
	}

	if (nIndex !=15)
	{
		result = jsonResult.toStyledString();
	}

}

Json::Value CDdataManager::AddCollectControllerParameter(Json::Value jsonData)
{
	//声明接口所需要的参数
	const char *IPAddr;
	Gloable_Bes::CollectControllerParameter pParameter;
	memset(&pParameter, 0, sizeof(Gloable_Bes::CollectControllerParameter));
	char *errmsg;
	uint32_t ID = atoi(jsonData["CollectControllerParameter"]["ID"].asString().c_str());
	uint8_t Active = atoi(jsonData["CollectControllerParameter"]["Active"].asString().c_str());
	uint8_t SamplePeriod = atoi(jsonData["CollectControllerParameter"]["SamplePeriod"].asString().c_str());
	uint8_t HisdataSavePeriod = atoi(jsonData["CollectControllerParameter"]["HisdataSavePeriod"].asString().c_str());
	//解析javaWeb客户端发来的json数据
	string strIP = jsonData["IPAddr"].asString();

	// 不知道 这个 地方为啥 不用 端口号 就是网络传递过来的端口后 

	uint32_t uPort = atoi(jsonData["Port"].asString().c_str());

	IPAddr = strIP.c_str();
	pParameter.ID = ID;
	strcpy_s(pParameter.Name, jsonData["CollectControllerParameter"]["Name"].asString().c_str());//string转char数组
	strcpy_s(pParameter.Alias, jsonData["CollectControllerParameter"]["Alias"].asString().c_str());
	strcpy_s(pParameter.Description, jsonData["CollectControllerParameter"]["Description"].asString().c_str());
	strcpy_s(pParameter.Location, jsonData["CollectControllerParameter"]["Location"].asString().c_str());
	strcpy_s(pParameter.Zone, jsonData["CollectControllerParameter"]["Zone"].asString().c_str());
	pParameter.Active = Active;
	pParameter.SamplePeriod = SamplePeriod;
	pParameter.HisdataSavePeriod = HisdataSavePeriod;

	std::string strErrmsg;
	//调用正晨接口，下发到下位机并判断是否成功返回
	if (m_besManager.BES200_AddCollectControllerParameter(IPAddr, &pParameter, &errmsg) == 0)
	{
//		ctcp.LogPrint("正晨服务", "新增控制器成功！");
		strErrmsg = "操作成功";//配置成功返回
	}
	else
	{
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
	//	ctcp.LogPrint("正晨服务", "新增控制器失败！");
	}

	//将返回值组装成json数据
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//将strErrmsg字符串数据放到errmsg节点
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//返回配置信息
}

Json::Value CDdataManager::SetCollectControllerParameter(Json::Value jsonData)
{
	//声明接口所需要的参数
	const char *IPAddr;
	CollectControllerParameter pParameter;
	memset(&pParameter, 0, sizeof(CollectControllerParameter));
	char *errmsg = "";
	uint32_t ID = atoi(jsonData["CollectControllerParameter"]["ID"].asString().c_str());
	uint8_t Active = atoi(jsonData["CollectControllerParameter"]["Active"].asString().c_str());
	uint8_t SamplePeriod = atoi(jsonData["CollectControllerParameter"]["SamplePeriod"].asString().c_str());
	uint8_t HisdataSavePeriod = atoi(jsonData["CollectControllerParameter"]["HisdataSavePeriod"].asString().c_str());
	//解析javaWeb客户端发来的json数据
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	pParameter.ID = ID;
	strcpy_s(pParameter.Name, jsonData["CollectControllerParameter"]["Name"].asString().c_str());//string转char数组
	strcpy_s(pParameter.Alias, jsonData["CollectControllerParameter"]["Alias"].asString().c_str());
	strcpy_s(pParameter.Description, jsonData["CollectControllerParameter"]["Description"].asString().c_str());
	strcpy_s(pParameter.Location, jsonData["CollectControllerParameter"]["Location"].asString().c_str());
	strcpy_s(pParameter.Zone, jsonData["CollectControllerParameter"]["Zone"].asString().c_str());
	pParameter.Active = Active;
	pParameter.SamplePeriod = SamplePeriod;
	pParameter.HisdataSavePeriod = HisdataSavePeriod;




	std::string strErrmsg;
	//调用正晨接口，下发到下位机并判断是否成功返回
	if (m_besManager.BES200_SetCollectControllerParameter(IPAddr, &pParameter, &errmsg) == 0)
	{
	//	LOG_INFO("正晨服务" << "设置一个控制器的所有参数成功!");
		strErrmsg = "操作成功";//配置成功返回
	}
	else
	{
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
		LOGE("正晨服务" << "设置一个控制器的所有参数失败!");
	}
	//将返回值组装成json数据
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//将strErrmsg字符串数据放到errmsg节点
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//返回配置信息
}

Json::Value CDdataManager::SetCollectControllerParameter2(Json::Value jsonData)
{
	//声明接口所需要的参数
	const char *IPAddr;
	CollectControllerParameter pParameter;
	memset(&pParameter, 0, sizeof(CollectControllerParameter));
	char *errmsg = "";
	uint32_t ID = atoi(jsonData["CollectControllerParameter"]["ID"].asString().c_str());
	uint8_t Active = atoi(jsonData["CollectControllerParameter"]["Active"].asString().c_str());
	uint8_t SamplePeriod = atoi(jsonData["CollectControllerParameter"]["SamplePeriod"].asString().c_str());
	uint8_t HisdataSavePeriod = atoi(jsonData["CollectControllerParameter"]["HisdataSavePeriod"].asString().c_str());
	//解析javaWeb客户端发来的json数据
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	pParameter.ID = ID;
	strcpy_s(pParameter.Name, jsonData["CollectControllerParameter"]["Name"].asString().c_str());//string转char数组
	strcpy_s(pParameter.Alias, jsonData["CollectControllerParameter"]["Alias"].asString().c_str());
	strcpy_s(pParameter.Description, jsonData["CollectControllerParameter"]["Description"].asString().c_str());
	strcpy_s(pParameter.Location, jsonData["CollectControllerParameter"]["Location"].asString().c_str());
	strcpy_s(pParameter.Zone, jsonData["CollectControllerParameter"]["Zone"].asString().c_str());
	pParameter.Active = Active;
	pParameter.SamplePeriod = SamplePeriod;
	pParameter.HisdataSavePeriod = HisdataSavePeriod;


	CollectControllerParameterTest pParameter2;
	//memset(&pParameter2, 0, sizeof(CollectControllerParameterTest));

	//memcpy(&pParameter2, &pParameter,  sizeof(CollectControllerParameter));

//	int nS1 = sizeof(pParameter);
//	int nS2 = sizeof(pParameter2);
	//Collect_PointS cs3;
	//string strss = jsonData["CollectControllerParameter"].toStyledString();
	//strss = "{\"ID\":10000026,\"Name\":\"测试1\",\"Alias\":\"测试1\",\"Description\":\"测试\",\"Location\":\"研发部\",\"Zone\":\"研发部\",\"Active\":1,\"SamplePeriod\":1,\"HisdataSavePeriod\":1}";
	//auto r3 = terra::json::from_json(pParameter2, strss.c_str(), strlen(strss.c_str()));


	/*
	{
	   "Active" : "1",
	   "Alias" : "测试1",
	   "Description" : "测试1",
	   "HisdataSavePeriod" : "1",
	   "ID" : "10000026",
	   "Location" : "研发部1",
	   "Name" : "测试1",
	   "SamplePeriod" : "1",
	   "Zone" : "研发部1"
	}

	*/
	//jsonData.asString();


	std::string strErrmsg;
	//调用正晨接口，下发到下位机并判断是否成功返回
	if (m_besManager.BES200_SetCollectControllerParameterTest(IPAddr, &pParameter2, &errmsg) == 0)
	{
		//LOG_INFO("正晨服务" << "设置一个控制器的所有参数成功!");
		strErrmsg = "操作成功";//配置成功返回
	}
	else
	{
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
		LOGE("正晨服务" << "设置一个控制器的所有参数失败!");
	}
	//将返回值组装成json数据
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//将strErrmsg字符串数据放到errmsg节点
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//返回配置信息
}

Json::Value CDdataManager::SetCollectControllerTime(Json::Value jsonData)
{
	//声明接口所需要的参数
	const char *IPAddr;
	tm pTime;
	char *errmsg = "";
	//解析javaWeb客户端发来的json数据
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	//将系统时间存放到tm结构体中
	time_t now;
	time(&now);
	localtime_s(&pTime,&now);



	std::string strErrmsg;
	////调用正晨接口，下发到下位机并判断是否成功返回
	if (m_besManager.BES200_SetCollectControllerTime(IPAddr, &pTime, &errmsg) == 0)
	{
		//LOG_INFO("正晨服务"<<"设置控制器的时间成功！");
		strErrmsg = "操作成功";//配置成功返回
	}
	else
	{
		LOGE("正晨服务" << "设置控制器的时间失败！");
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
	}

	//将返回值组装成json数据
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//将strErrmsg字符串数据放到errmsg节点

	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//返回配置信息
}

Json::Value CDdataManager::RebootCollectController(Json::Value jsonData)
{
	//声明接口所需要的参数
	const char *IPAddr;
	char *errmsg = "";
	//解析javaWeb客户端发来的json数据
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	std::string strErrmsg;
	//调用正晨接口，下发到下位机并判断是否成功返回
	if (m_besManager.BES200_RebootCollectController(IPAddr, &errmsg) == 0)
	{
		//m_logs.LogPrint("正晨服务", "重启DDC成功！");
		//LOG_INFO("正晨服务" << "重启DDC成功！");
		strErrmsg = "操作成功";//配置成功返回
	}
	else
	{
		//m_logs.LogPrint("正晨服务", "重启DDC失败！");
		LOGE("正晨服务" << "重启DDC失败！");
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
	}
	//将返回值组装成json数据
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//将strErrmsg字符串数据放到errmsg节点
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//返回配置信息
}

Json::Value CDdataManager::ResetCollectControler(Json::Value jsonData)
{
	//声明接口所需要的参数
	const char *IPAddr;
	char *errmsg = "";
	//解析javaWeb客户端发来的json数据
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	std::string strErrmsg;
	//调用正晨接口，下发到下位机并判断是否成功返回
	if (m_besManager.BES200_ResetCollectControler(IPAddr, &errmsg) == 0)
	{
		//m_logs.LogPrint("正晨服务", "重置控制器成功！");
		//LOG_INFO("正晨服务" << "重置控制器成功!");
		strErrmsg = "操作成功";//配置成功返回
	}
	else
	{
		//m_logs.LogPrint("正晨服务", "重置控制器失败！");
		LOGE("正晨服务" << "重置控制器失败！");
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
	}
	//将返回值组装成json数据
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//将strErrmsg字符串数据放到errmsg节点
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//返回配置信息
}

Json::Value CDdataManager::GetCollectControllerTime(Json::Value jsonData)
{
	const char *IPAddr;
	char errmsg[256] = {0};
	tm pTime = {0};
	//解析javaWeb客户端发来的json数据
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();

	// 不知道 这个 地方为啥 不用 端口号 就是网络传递过来的端口后 

	uint32_t uPort = atoi(jsonData["Port"].asString().c_str());

	std::string strErrmsg;


	int Index;//获取下位机数据成功或失败的声明
	//要返回的json数据声明
	Json::Value jsonResult;
	//调用正晨接口，下发到下位机并判断是否成功返回
	int nRet = m_besManager.BES200_GetCollectControllerTime(IPAddr, &pTime, errmsg);
	if (nRet == 0)
	{
		//m_logs.LogPrint("正晨服务", "获取控制器的时间成功！");
		//LOG_INFO("正晨服务" << "获取控制器的时间成功!");
		strErrmsg = "操作成功";//配置成功返回
		Index = 0;
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
		//子节点储存tm结构体的数据
		Json::Value jsonTime;
		//子节点属性
		jsonTime["tm_sec"] = Json::Value(pTime.tm_sec);
		jsonTime["tm_min"] = Json::Value(pTime.tm_min);
		jsonTime["tm_hour"] = Json::Value(pTime.tm_hour);
		jsonTime["tm_mday"] = Json::Value(pTime.tm_mday);
		jsonTime["tm_mon"] = Json::Value(pTime.tm_mon);
		jsonTime["tm_year"] = Json::Value(pTime.tm_year);
		jsonTime["tm_wday"] = Json::Value(pTime.tm_wday);
		jsonTime["tm_yday"] = Json::Value(pTime.tm_yday);
		jsonTime["tm_isdst"] = Json::Value(pTime.tm_isdst);
		//将子节点的值放到根节点
		jsonResult["tm"] = Json::Value(jsonTime);
	}
	else
	{
	//	m_logs.LogPrint("正晨服务", "获取控制器的时间失败！");
		LOGE("正晨服务" << IPAddr<< "获取控制器的时间失败！" << nRet);
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
		Index = 1;
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
	}
//	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//返回配置信息
}

Json::Value CDdataManager::GetCollectControllerParameter(Json::Value jsonData)
{
	//声明接口所需要的参数
	const char *IPAddr;
	CollectControllerParameter pParameter;
	char *errmsg = "";
	//解析javaWeb客户端发来的json数据
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	std::string strErrmsg;

	int Index;//获取下位机数据成功或失败的声明
	//要返回的json数据声明
	Json::Value jsonResult;

	if (m_besManager.BES200_GetCollectControllerParameter(IPAddr, &pParameter, &errmsg) == 0)
	{
	//	m_logs.LogPrint("正晨服务", "获取控制器的所有配置参数成功！");
		LOGE("正晨服务" << "获取控制器的所有配置参数成功！");
		strErrmsg = "操作成功";//配置成功返回
		Index = 0;
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
		//子节点储存CollectControllerParameter结构体的数据
		Json::Value jsonParameter;
		//子节点属性
		jsonParameter["ID"] = Json::Value(pParameter.ID);
		jsonParameter["Name"] = Json::Value(pParameter.Name);
		jsonParameter["Alias"] = Json::Value(pParameter.Alias);
		jsonParameter["Description"] = Json::Value(pParameter.Description);
		jsonParameter["Location"] = Json::Value(pParameter.Location);
		jsonParameter["Zone"] = Json::Value(pParameter.Zone);
		jsonParameter["Active"] = Json::Value(pParameter.Active);
		jsonParameter["SamplePeriod"] = Json::Value(pParameter.SamplePeriod);
		jsonParameter["HisdataSavePeriod"] = Json::Value(pParameter.HisdataSavePeriod);
		//将子节点的值放到根节点
		jsonResult["CollectControllerParameter"] = Json::Value(jsonParameter);
	}
	else
	{
	//	m_logs.LogPrint("正晨服务", "获取控制器的所有配置参数失败！");
		LOGE("正晨服务" << "获取控制器的所有配置参数失败！");
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
		Index = 1;
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
	}
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;
}

Json::Value CDdataManager::GetCollectControllerInfoByBroadcast()
{
	//声明接口所需要的参数
	int size = -1;
	CollectControllerInfo *pInfo = NULL;
	CollectControllerInfo *p = NULL;
	char *errmsg = "";
	//声明返回参数
	std::string strErrmsg;
	//下位机数据返回成功或失败的标识
	int Index;
	//要返回的json数据声明
	Json::Value jsonResult;
	if (m_besManager.BES200_GetCollectControllerInfoByBroadcast(&pInfo, &size, &errmsg) == 0)
	{
		//m_logs.LogPrint("正晨服务", "广播方式与其他控制器通信，获取所有控制器的信息成功！");
		//LOG_INFO("正晨服务" << "广播方式与其他控制器通信，获取所有控制器的信息成功！！");
		p = pInfo;
		strErrmsg = "操作成功";//配置成功返回																
		Index = 0;
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
		//定义jsonResult子节点储存CollectControllerInfo结构数组的数据
		Json::Value jsonCollectControllerInfo;
		//定义jsonCollectControllerInfo子节点标识
		string strIndex;
		for (int i = 0; i < size; i++)
		{
			//子节点储存CollectControllerInfo结构体的数据																			
			Json::Value jsonInfo;
			//子节点属性																						
			jsonInfo["Name"] = Json::Value(pInfo->Name);
			jsonInfo["CampanyName"] = Json::Value(pInfo->CampanyName);
			jsonInfo["Model"] = Json::Value(pInfo->Model);
			jsonInfo["HWVer"] = Json::Value(pInfo->HWVer);
			jsonInfo["FWVer"] = Json::Value(pInfo->FWVer);

			//将无符号数字 转换为 带格式的字符串再添加到json数据中
			string strIP = Getstring(pInfo->IP[0]) + '.' + Getstring(pInfo->IP[1]) + '.' + Getstring(pInfo->IP[2]) + '.' + Getstring(pInfo->IP[3]);
			jsonInfo["IP"] = Json::Value(strIP);
			//将无符号数字 转换为 带格式的字符串再添加到json数据中
			string strMask = Getstring(pInfo->Mask[0]) + '.' + Getstring(pInfo->Mask[1]) + '.' + Getstring(pInfo->Mask[2]) + '.' + Getstring(pInfo->Mask[3]);
			jsonInfo["Mask"] = Json::Value(strMask);
			//将无符号数字 转换为 带格式的字符串再添加到json数据中
			string strGateWay = Getstring(pInfo->GateWay[0]) + '.' + Getstring(pInfo->GateWay[1]) + '.' + Getstring(pInfo->GateWay[2]) + '.' + Getstring(pInfo->GateWay[3]);
			jsonInfo["GateWay"] = Json::Value(strGateWay);
			//将无符号数字 转换为 带格式的字符串再添加到json数据中
			string strMac = Getstring(pInfo->Mac[0]) + '.' + Getstring(pInfo->Mac[1]) + '.' + Getstring(pInfo->Mac[2]) + '.' + Getstring(pInfo->Mac[3]) + '.' + Getstring(pInfo->Mac[4]) + '.' + Getstring(pInfo->Mac[5]);
			jsonInfo["Mac"] = Json::Value(strMac);

			jsonInfo["ID"] = Json::Value(pInfo->ID);
			jsonInfo["Alias"] = Json::Value(pInfo->Alias);
			jsonInfo["Description"] = Json::Value(pInfo->Description);
			jsonInfo["Location"] = Json::Value(pInfo->Location);
			jsonInfo["Zone"] = Json::Value(pInfo->Zone);
			jsonInfo["Active"] = Json::Value(pInfo->Active);
			jsonInfo["SamplePeriod"] = Json::Value(pInfo->SamplePeriod);
			jsonInfo["HisdataSavePeriod"] = Json::Value(pInfo->HisdataSavePeriod);
			pInfo++;

			//将int转为string获取节点名
			char cIndex[256];
			string strIndex;
			sprintf_s(cIndex, "%d", i);
			strIndex = cIndex;
			//将子节点的值放到根节点	
			jsonCollectControllerInfo[strIndex] = Json::Value(jsonInfo);
		}
		jsonResult["CollectControllerInfo"] = Json::Value(jsonCollectControllerInfo);
	}
	else
	{
		//m_logs.LogPrint("正晨服务", "广播方式与其他控制器通信，获取所有控制器的信息失败！");
//		LOG_ERROR("正晨服务" << "广播方式与其他控制器通信，获取所有控制器的信息失败！");

		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息														
		Index = 1;
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
	}
	m_besManager.BES200_Free(&errmsg);
	// 转换为字符串（带格式）
	//std::string out = jsonResult.toStyledString();
	//cout <<out<< endl;
	if (p != NULL)
	{
		m_besManager.BES200Brocast_Free(&p);
	}
	return jsonResult;
}

Json::Value CDdataManager::SetCollectControllerInfoByBroadcast(Json::Value jsonData)
{
	CollectControllerInfo pInfo;
	memset(&pInfo, 0, sizeof(CollectControllerInfo));
	char *errmsg;
	//string转char数组																								
	strcpy_s(pInfo.Name, jsonData["CollectControllerInfo"]["Name"].asString().c_str());
	strcpy_s(pInfo.CampanyName, jsonData["CollectControllerInfo"]["CampanyName"].asString().c_str());
	strcpy_s(pInfo.Model, jsonData["CollectControllerInfo"]["Model"].asString().c_str());
	string a = "asxaaa";
	/*	jsonData["CollectControllerInfo"]["IP"].asString().copy(pInfo.IP);
	jsonData["CollectControllerInfo"]["Mask"].asString().copy(pInfo.Mask);
	jsonData["CollectControllerInfo"]["GateWay"].asString(pInfo.GateWay);
	jsonData["CollectControllerInfo"]["Mac"].asString(pInfo.Mac);*/
	strcpy_s(pInfo.Alias, jsonData["CollectControllerInfo"]["Alias"].asString().c_str());
	strcpy_s(pInfo.Description, jsonData["CollectControllerInfo"]["Description"].asString().c_str());
	strcpy_s(pInfo.Location, jsonData["CollectControllerInfo"]["Location"].asString().c_str());
	strcpy_s(pInfo.Zone, jsonData["CollectControllerInfo"]["Zone"].asString().c_str());
	pInfo.HWVer = atoi(jsonData["CollectControllerInfo"]["HWVer"].asString().c_str());
	pInfo.FWVer = atoi(jsonData["CollectControllerInfo"]["FWVer"].asString().c_str());
	pInfo.ID = atoi(jsonData["CollectControllerInfo"]["ID"].asString().c_str());
	pInfo.Active = atoi(jsonData["CollectControllerInfo"]["Active"].asString().c_str());
	pInfo.SamplePeriod = atoi(jsonData["CollectControllerInfo"]["SamplePeriod"].asString().c_str());
	pInfo.HisdataSavePeriod = atoi(jsonData["CollectControllerInfo"]["HisdataSavePeriod"].asString().c_str());
	std::string strErrmsg;
	//调用正晨接口，下发到下位机并判断是否成功返回
	if (m_besManager.BES200_SetCollectControllerInfoByBroadcast(&pInfo, &errmsg) == 0)
	{
		//m_logs.LogPrint("正晨服务", "广播方式与其他控制器通信成功！");
		//LOG_INFO("正晨服务" << "广播方式与其他控制器通信成功!");
		strErrmsg = "操作成功";//配置成功返回
	}
	else
	{
		//m_logs.LogPrint("正晨服务", "广播方式与其他控制器通信失败！");
		//LOG_INFO("正晨服务" << "广播方式与其他控制器通信失败!");
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
	}
	//将返回值组装成json数据
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//将strErrmsg字符串数据放到errmsg节点
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//返回配置信息					
}

Json::Value CDdataManager::DeleteCollectController(Json::Value jsonData)
{
	//声明接口所需要的参数
	const char *IPAddr;
	char *errmsg = "";
	//解析javaWeb客户端发来的json数据
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	std::string strErrmsg;
	//调用正晨接口，下发到下位机并判断是否成功返回
	if (m_besManager.BES200_DeleteCollectController(IPAddr, &errmsg) == 0)
	{
		//m_logs.LogPrint("正晨服务", "删除一个控制器，并删除和该控制器相关的模块、点成功！");
		//LOG_INFO("正晨服务" << "删除一个控制器，并删除和该控制器相关的模块、点成功！");
		strErrmsg = "操作成功";//配置成功返回
	}
	else
	{
		//m_logs.LogPrint("正晨服务", "删除一个控制器，并删除和该控制器相关的模块、点失败！");
		LOGE("正晨服务" << "删除一个控制器，并删除和该控制器相关的模块、点失败！");
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
	}
	//将返回值组装成json数据
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//将strErrmsg字符串数据放到errmsg节点
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//返回配置信息
}

Json::Value CDdataManager::FunIAP(Json::Value jsonData)
{
	//声明接口所需要的参数
	const char *IPAddr;
	char *errmsg = "";
	//解析javaWeb客户端发来的json数据
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	std::string strErrmsg;
	//调用正晨接口，下发到下位机并判断是否成功返回
	if (m_besManager.BES200_FunIAP(IPAddr, &errmsg) == 0)
	{
	//	m_logs.LogPrint("正晨服务", "进行远程升级，下载程序成功！");
		//LOG_INFO("正晨服务" << "进行远程升级，下载程序成功！");
		strErrmsg = "操作成功";//配置成功返回
	}
	else
	{
		//m_logs.LogPrint("正晨服务", "进行远程升级，下载程序失败！");
		LOGE("正晨服务" << "进行远程升级，下载程序失败！");
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
	}
	//将返回值组装成json数据
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//将strErrmsg字符串数据放到errmsg节点
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//返回配置信息
}

Json::Value CDdataManager::AddMeter(Json::Value jsonData)
{
	//声明接口所需要的参数
	const char *IPAddr;
	unsigned int MeterID;
	MeterParameter pParameter;
	memset(&pParameter, 0, sizeof(MeterParameter));
	char *errmsg = "";
	//解析javaWeb客户端发来的json数据
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	MeterID = atoi(jsonData["MeterID"].asString().c_str());
	//为MeterParameter结构体添加数据
	pParameter.MeterID = atoi(jsonData["MeterParameter"]["MeterID"].asString().c_str());
	pParameter.MeterType = atoi(jsonData["MeterParameter"]["MeterType"].asString().c_str());
	pParameter.ComRate = atoi(jsonData["MeterParameter"]["ComRate"].asString().c_str());
	pParameter.ComPort = atoi(jsonData["MeterParameter"]["ComPort"].asString().c_str());
	pParameter.ComAgreementType = atoi(jsonData["MeterParameter"]["ComAgreementType"].asString().c_str());
	string strPhyAddr = jsonData["MeterParameter"]["PhyAddr"].asString();//将物理地址保存并拆分
	//if (pParameter.MeterType == METER_TYPE_WATER){
	//	pParameter.PhyAddr[6] = (atoi(strPhyAddr.substr(0, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(1, 1).c_str()));
	//	pParameter.PhyAddr[5] = (atoi(strPhyAddr.substr(2, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(3, 1).c_str()));
	//	pParameter.PhyAddr[4] = (atoi(strPhyAddr.substr(4, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(5, 1).c_str()));
	//	pParameter.PhyAddr[3] = (atoi(strPhyAddr.substr(6, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(7, 1).c_str()));
	//	pParameter.PhyAddr[3] = (atoi(strPhyAddr.substr(8, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(9, 1).c_str()));
	//	pParameter.PhyAddr[1] = (atoi(strPhyAddr.substr(10, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(11, 1).c_str()));
	//	pParameter.PhyAddr[0] = (atoi(strPhyAddr.substr(12, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(13, 1).c_str()));
	//}
	//else{
	//	pParameter.PhyAddr[5] = (atoi(strPhyAddr.substr(0, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(1, 1).c_str()));
	//	pParameter.PhyAddr[4] = (atoi(strPhyAddr.substr(2, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(3, 1).c_str()));
	//	pParameter.PhyAddr[3] = (atoi(strPhyAddr.substr(4, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(5, 1).c_str()));
	//	pParameter.PhyAddr[2] = (atoi(strPhyAddr.substr(6, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(7, 1).c_str()));
	//	pParameter.PhyAddr[1] = (atoi(strPhyAddr.substr(8, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(9, 1).c_str()));
	//	pParameter.PhyAddr[0] = (atoi(strPhyAddr.substr(10, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(11, 1).c_str()));
	//}

//	if ((pParameter.ComAgreementType == COMAGREEMENTTYPE_CJT188) || 
//		(pParameter.ComAgreementType == COMAGREEMENTTYPE_SIMPLE_CJT188) 
//		|| (pParameter.ComAgreementType == COMAGREEMENTTYPE_W645)|| (pParameter.ComAgreementType == COMAGREEMENTTYPE_SDJYL_WaterMeter)) {
//
//
//		pParameter.PhyAddr[6] = (atoi(strPhyAddr.substr(0, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(1, 1).c_str()));
//		pParameter.PhyAddr[5] = (atoi(strPhyAddr.substr(2, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(3, 1).c_str()));
//		pParameter.PhyAddr[4] = (atoi(strPhyAddr.substr(4, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(5, 1).c_str()));
//		pParameter.PhyAddr[3] = (atoi(strPhyAddr.substr(6, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(7, 1).c_str()));
//		pParameter.PhyAddr[3] = (atoi(strPhyAddr.substr(8, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(9, 1).c_str()));
//		pParameter.PhyAddr[1] = (atoi(strPhyAddr.substr(10, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(11, 1).c_str()));
//		pParameter.PhyAddr[0] = (atoi(strPhyAddr.substr(12, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(13, 1).c_str()));
//	}
//	else {
//		pParameter.PhyAddr[5] = (atoi(strPhyAddr.substr(0, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(1, 1).c_str()));
//		pParameter.PhyAddr[4] = (atoi(strPhyAddr.substr(2, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(3, 1).c_str()));
//		pParameter.PhyAddr[3] = (atoi(strPhyAddr.substr(4, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(5, 1).c_str()));
//		pParameter.PhyAddr[2] = (atoi(strPhyAddr.substr(6, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(7, 1).c_str()));
//		pParameter.PhyAddr[1] = (atoi(strPhyAddr.substr(8, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(9, 1).c_str()));
//		pParameter.PhyAddr[0] = (atoi(strPhyAddr.substr(10, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(11, 1).c_str()));
//	}
std::string strAddr = strPhyAddr;

	if ((pParameter.ComAgreementType == COMAGREEMENTTYPE_CJT188) ||
		(pParameter.ComAgreementType == COMAGREEMENTTYPE_SIMPLE_CJT188) ||
		(pParameter.ComAgreementType == COMAGREEMENTTYPE_W645)
		|| (pParameter.ComAgreementType == COMAGREEMENTTYPE_SDJYL_WaterMeter)) {
		char cDataMsg[255] = { 0 };
		int nLen = strAddr.length();
		int nIndex = 0;
		std::string strNode;
		for (int i = 0; i < nLen;)
		{
			strNode = strAddr.substr(i, 2);
			int nC = atoi(strNode.c_str());
			char* end;
			uint8_t result = static_cast<uint8_t>(strtol(strNode.c_str(), &end, 16));
			cDataMsg[nIndex] = result;
			nIndex++;
			i = i + 2;
		}




		// pParameter.PhyAddr[6] = (atoi(strPhyAddr.substr(0, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(1, 1).c_str()));
		// pParameter.PhyAddr[5] = (atoi(strPhyAddr.substr(2, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(3, 1).c_str()));
		// pParameter.PhyAddr[4] = (atoi(strPhyAddr.substr(4, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(5, 1).c_str()));
		// pParameter.PhyAddr[3] = (atoi(strPhyAddr.substr(6, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(7, 1).c_str()));
		// pParameter.PhyAddr[2] = (atoi(strPhyAddr.substr(8, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(9, 1).c_str()));
		// pParameter.PhyAddr[1] = (atoi(strPhyAddr.substr(10, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(11, 1).c_str()));
		// pParameter.PhyAddr[0] = (atoi(strPhyAddr.substr(12, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(13, 1).c_str()));


		pParameter.PhyAddr[6] = cDataMsg[0];
		pParameter.PhyAddr[5] = cDataMsg[1];
		pParameter.PhyAddr[4] = cDataMsg[2];
		pParameter.PhyAddr[3] = cDataMsg[3];
		pParameter.PhyAddr[2] = cDataMsg[4];
		pParameter.PhyAddr[1] = cDataMsg[5];
		pParameter.PhyAddr[0] = cDataMsg[6];



	}
	else {
		char cDataMsg[255] = { 0 };
		int nLen = strAddr.length();
		int nIndex = 0;
		std::string strNode;
		for (int i = 0; i < nLen;)
		{
			strNode = strAddr.substr(i, 2);
			int nC = atoi(strNode.c_str());
			char* end;
			uint8_t result = static_cast<uint8_t>(strtol(strNode.c_str(), &end, 16));
			cDataMsg[nIndex] = result;
			nIndex++;
			i = i + 2;
		}
		//	pParameter.PhyAddr[6] = cDataMsg[0];
		pParameter.PhyAddr[5] = cDataMsg[0];
		pParameter.PhyAddr[4] = cDataMsg[1];
		pParameter.PhyAddr[3] = cDataMsg[2];
		pParameter.PhyAddr[2] = cDataMsg[3];
		pParameter.PhyAddr[1] = cDataMsg[4];
		pParameter.PhyAddr[0] = cDataMsg[5];


		//	pParameter.PhyAddr[5] = (atoi(strPhyAddr.substr(0, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(1, 1).c_str()));
		//	pParameter.PhyAddr[4] = (atoi(strPhyAddr.substr(2, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(3, 1).c_str()));
		//	pParameter.PhyAddr[3] = (atoi(strPhyAddr.substr(4, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(5, 1).c_str()));
		//	pParameter.PhyAddr[2] = (atoi(strPhyAddr.substr(6, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(7, 1).c_str()));
		//	pParameter.PhyAddr[1] = (atoi(strPhyAddr.substr(8, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(9, 1).c_str()));
		//	pParameter.PhyAddr[0] = (atoi(strPhyAddr.substr(10, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(11, 1).c_str()));
	}
	strcpy_s(pParameter.Alias, jsonData["MeterParameter"]["Alias"].asString().c_str());//string转char数组
	strcpy_s(pParameter.Description, jsonData["MeterParameter"]["Description"].asString().c_str());
	strcpy_s(pParameter.Location, jsonData["MeterParameter"]["Location"].asString().c_str());
	pParameter.Active = atoi(jsonData["MeterParameter"]["Active"].asString().c_str());
	//为MeterParameter结构体中的ElectricDataCollectMethod结构添加数据
	pParameter.CollectMethod.CollectCount = atoi(jsonData["ElectricDataCollectMethod"]["CollectCount"].asString().c_str());
	pParameter.CollectMethod.CollectMethodID = atoi(jsonData["ElectricDataCollectMethod"]["CollectMethodID"].asString().c_str());

	// 转换为字符串（带格式）
	//std::string out = jsonData.toStyledString();
	//cout <<out<< endl;

	int nCount = (int)pParameter.CollectMethod.CollectCount;
	for (int i = 0; i < nCount; i++)
	{
		char cIndex[256];
		string strIndex;
		sprintf_s(cIndex, "%d", i);
		strIndex = cIndex;

		pParameter.CollectMethod.ElectricDataInfo[i].ElectricID = atoi(jsonData["ElectricDataInfo"][strIndex]["ElectricID"].asString().c_str());
		strcpy_s(pParameter.CollectMethod.ElectricDataInfo[i].ElectricName, jsonData["ElectricDataInfo"][strIndex]["ElectricName"].asString().c_str());
		pParameter.CollectMethod.ElectricDataInfo[i].OffsetAddr = GetDecimalism(jsonData["ElectricDataInfo"][strIndex]["OffsetAddr"].asString());
		pParameter.CollectMethod.ElectricDataInfo[i].DataLength = atoi(jsonData["ElectricDataInfo"][strIndex]["DataLength"].asString().c_str());
		pParameter.CollectMethod.ElectricDataInfo[i].DataEncodeType = atoi(jsonData["ElectricDataInfo"][strIndex]["DataEncodeType"].asString().c_str());
		strcpy_s(pParameter.CollectMethod.ElectricDataInfo[i].UintType, jsonData["ElectricDataInfo"][strIndex]["UintType"].asString().c_str());
		pParameter.CollectMethod.ElectricDataInfo[i].PointLocation = atoi(jsonData["ElectricDataInfo"][strIndex]["PointLocation"].asString().c_str());
	}
	std::string strErrmsg;
	if (m_besManager.BES200_AddMeter(IPAddr, MeterID, &pParameter, &errmsg) == 0)
	{
//		ctcp.LogPrint("正晨服务", "在控制器下增加一个模块并配置好模块的相应参数成功！");
		strErrmsg = "操作成功";//配置成功返回
	}
	else
	{
	//	m_logs.LogPrint("正晨服务", "在控制器下增加一个模块并配置好模块的相应参数失败！");
		LOGE("正晨服务" << "在控制器下增加一个模块并配置好模块的相应参数失败！");
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
	}
	//将返回值组装成json数据
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//将strErrmsg字符串数据放到errmsg节点
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//返回配置信息
}

Json::Value CDdataManager::SetMeterParameter(Json::Value jsonData)
{
	//声明接口所需要的参数
	const char *IPAddr;
	unsigned int MeterID;
	MeterParameter pParameter;
	char *errmsg = "";
	//解析javaWeb客户端发来的json数据
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	MeterID = atoi(jsonData["MeterID"].asString().c_str());
	//为MeterParameter结构体添加数据
	pParameter.MeterID = atoi(jsonData["MeterParameter"]["MeterID"].asString().c_str());
	pParameter.MeterType = atoi(jsonData["MeterParameter"]["MeterType"].asString().c_str());
	pParameter.ComRate = atoi(jsonData["MeterParameter"]["ComRate"].asString().c_str());
	pParameter.ComPort = atoi(jsonData["MeterParameter"]["ComPort"].asString().c_str());
	pParameter.ComAgreementType = atoi(jsonData["MeterParameter"]["ComAgreementType"].asString().c_str());
	string strPhyAddr = jsonData["MeterParameter"]["PhyAddr"].asString();//将物理地址保存并拆分
	//if (pParameter.MeterType == METER_TYPE_WATER){
	//	pParameter.PhyAddr[6] = (atoi(strPhyAddr.substr(0, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(1, 1).c_str()));
	//	pParameter.PhyAddr[5] = (atoi(strPhyAddr.substr(2, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(3, 1).c_str()));
	//	pParameter.PhyAddr[4] = (atoi(strPhyAddr.substr(4, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(5, 1).c_str()));
	//	pParameter.PhyAddr[3] = (atoi(strPhyAddr.substr(6, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(7, 1).c_str()));
	//	pParameter.PhyAddr[2] = (atoi(strPhyAddr.substr(8, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(9, 1).c_str()));
	//	pParameter.PhyAddr[1] = (atoi(strPhyAddr.substr(10, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(11, 1).c_str()));
	//	pParameter.PhyAddr[0] = (atoi(strPhyAddr.substr(12, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(13, 1).c_str()));
	//}
	//else{
	//	pParameter.PhyAddr[5] = (atoi(strPhyAddr.substr(0, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(1, 1).c_str()));
	//	pParameter.PhyAddr[4] = (atoi(strPhyAddr.substr(2, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(3, 1).c_str()));
	//	pParameter.PhyAddr[3] = (atoi(strPhyAddr.substr(4, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(5, 1).c_str()));
	//	pParameter.PhyAddr[2] = (atoi(strPhyAddr.substr(6, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(7, 1).c_str()));
	//	pParameter.PhyAddr[1] = (atoi(strPhyAddr.substr(8, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(9, 1).c_str()));
	//	pParameter.PhyAddr[0] = (atoi(strPhyAddr.substr(10, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(11, 1).c_str()));
	//}
	std::string strAddr = strPhyAddr;

	if ((pParameter.ComAgreementType == COMAGREEMENTTYPE_CJT188) || 
		(pParameter.ComAgreementType == COMAGREEMENTTYPE_SIMPLE_CJT188) || 
		(pParameter.ComAgreementType == COMAGREEMENTTYPE_W645)
		|| (pParameter.ComAgreementType == COMAGREEMENTTYPE_SDJYL_WaterMeter)) {
		char cDataMsg[255] = { 0 };
		int nLen = strAddr.length();
		int nIndex = 0;
		std::string strNode;
		for (int i = 0; i < nLen;)
		{
			strNode = strAddr.substr(i, 2);
			int nC = atoi(strNode.c_str());
			char* end;
			uint8_t result = static_cast<uint8_t>(strtol(strNode.c_str(), &end, 16));
			cDataMsg[nIndex] = result;
			nIndex++;
			i = i + 2;
		}
	



		// pParameter.PhyAddr[6] = (atoi(strPhyAddr.substr(0, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(1, 1).c_str()));
		// pParameter.PhyAddr[5] = (atoi(strPhyAddr.substr(2, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(3, 1).c_str()));
		// pParameter.PhyAddr[4] = (atoi(strPhyAddr.substr(4, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(5, 1).c_str()));
		// pParameter.PhyAddr[3] = (atoi(strPhyAddr.substr(6, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(7, 1).c_str()));
		// pParameter.PhyAddr[2] = (atoi(strPhyAddr.substr(8, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(9, 1).c_str()));
		// pParameter.PhyAddr[1] = (atoi(strPhyAddr.substr(10, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(11, 1).c_str()));
		// pParameter.PhyAddr[0] = (atoi(strPhyAddr.substr(12, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(13, 1).c_str()));


		pParameter.PhyAddr[6] = cDataMsg[0];
		pParameter.PhyAddr[5] = cDataMsg[1];
		pParameter.PhyAddr[4] = cDataMsg[2];
		pParameter.PhyAddr[3] = cDataMsg[3];
		pParameter.PhyAddr[2] = cDataMsg[4];
		pParameter.PhyAddr[1] = cDataMsg[5];
		pParameter.PhyAddr[0] = cDataMsg[6];



	}
	else {
		char cDataMsg[255] = { 0 };
		int nLen = strAddr.length();
		int nIndex = 0;
		std::string strNode;
		for (int i = 0; i < nLen;)
		{
			strNode = strAddr.substr(i, 2);
			int nC = atoi(strNode.c_str());
			char* end;
			uint8_t result = static_cast<uint8_t>(strtol(strNode.c_str(), &end, 16));
			cDataMsg[nIndex] = result;
			nIndex++;
			i = i + 2;
		}
	//	pParameter.PhyAddr[6] = cDataMsg[0];
		pParameter.PhyAddr[5] = cDataMsg[0];
		pParameter.PhyAddr[4] = cDataMsg[1];
		pParameter.PhyAddr[3] = cDataMsg[2];
		pParameter.PhyAddr[2] = cDataMsg[3];
		pParameter.PhyAddr[1] = cDataMsg[4];
		pParameter.PhyAddr[0] = cDataMsg[5];


	//	pParameter.PhyAddr[5] = (atoi(strPhyAddr.substr(0, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(1, 1).c_str()));
	//	pParameter.PhyAddr[4] = (atoi(strPhyAddr.substr(2, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(3, 1).c_str()));
	//	pParameter.PhyAddr[3] = (atoi(strPhyAddr.substr(4, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(5, 1).c_str()));
	//	pParameter.PhyAddr[2] = (atoi(strPhyAddr.substr(6, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(7, 1).c_str()));
	//	pParameter.PhyAddr[1] = (atoi(strPhyAddr.substr(8, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(9, 1).c_str()));
	//	pParameter.PhyAddr[0] = (atoi(strPhyAddr.substr(10, 1).c_str())) * 16 + (atoi(strPhyAddr.substr(11, 1).c_str()));
	}

	strcpy_s(pParameter.Alias, jsonData["MeterParameter"]["Alias"].asString().c_str());//string转char数组
	strcpy_s(pParameter.Description, jsonData["MeterParameter"]["Description"].asString().c_str());
	strcpy_s(pParameter.Location, jsonData["MeterParameter"]["Location"].asString().c_str());
	pParameter.Active = atoi(jsonData["MeterParameter"]["Active"].asString().c_str());
	//为MeterParameter结构体中的ElectricDataCollectMethod结构添加数据
	pParameter.CollectMethod.CollectCount = atoi(jsonData["ElectricDataCollectMethod"]["CollectCount"].asString().c_str());
	pParameter.CollectMethod.CollectMethodID = atoi(jsonData["ElectricDataCollectMethod"]["CollectMethodID"].asString().c_str());

	// 转换为字符串（带格式）
	//std::string out = jsonData.toStyledString();
	//cout <<out<< endl;

	int nCount = (int)pParameter.CollectMethod.CollectCount;
	for (int i = 0; i < nCount; i++)
	{
		char cIndex[256];
		string strIndex;
		sprintf_s(cIndex, "%d", i);
		strIndex = cIndex;

// 		string sstest = jsonData["ElectricDataInfo"][strIndex]["OffsetAddr"].asString();
// 
// 		int n1 = GetDecimalism("0100");
// 		int n2 = GetDecimalism("100");

		pParameter.CollectMethod.ElectricDataInfo[i].ElectricID = atoi(jsonData["ElectricDataInfo"][strIndex]["ElectricID"].asString().c_str());
		strcpy_s(pParameter.CollectMethod.ElectricDataInfo[i].ElectricName, jsonData["ElectricDataInfo"][strIndex]["ElectricName"].asString().c_str());
		pParameter.CollectMethod.ElectricDataInfo[i].OffsetAddr = GetDecimalism(jsonData["ElectricDataInfo"][strIndex]["OffsetAddr"].asString());
		pParameter.CollectMethod.ElectricDataInfo[i].DataLength = atoi(jsonData["ElectricDataInfo"][strIndex]["DataLength"].asString().c_str());
		pParameter.CollectMethod.ElectricDataInfo[i].DataEncodeType = atoi(jsonData["ElectricDataInfo"][strIndex]["DataEncodeType"].asString().c_str());
		strcpy_s(pParameter.CollectMethod.ElectricDataInfo[i].UintType, jsonData["ElectricDataInfo"][strIndex]["UintType"].asString().c_str());
		pParameter.CollectMethod.ElectricDataInfo[i].PointLocation = atoi(jsonData["ElectricDataInfo"][strIndex]["PointLocation"].asString().c_str());
	}
	std::string strErrmsg;
	if (m_besManager.BES200_SetMeterParameter(IPAddr, MeterID, &pParameter, &errmsg) == 0)
	{
	//	ctcp.LogPrint("正晨服务", "配置模块的所有配置参数成功！");
		strErrmsg = "操作成功";//配置成功返回
	}
	else
	{
	//	ctcp.LogPrint("正晨服务", "配置模块的所有配置参数失败！");
		LOGE("正晨服务" << "配置模块的所有配置参数失败");
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
	}
	//将返回值组装成json数据
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//将strErrmsg字符串数据放到errmsg节点
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//返回配置信息
}

Json::Value CDdataManager::DeleteMeter(Json::Value jsonData)
{
	//声明接口所需要的参数
	const char *IPAddr;
	unsigned int MeterID;
	char *errmsg = "";
	//解析javaWeb客户端发来的json数据
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	MeterID = atoi(jsonData["MeterID"].asString().c_str());

	std::string strErrmsg;
	if (m_besManager.BES200_DeleteMeter(IPAddr, MeterID, &errmsg) == 0)
	{
	//	ctcp.LogPrint("正晨服务", "删除一个模块，并删除和模块相关的点成功！");
		strErrmsg = "操作成功";//配置成功返回
	}
	else
	{
	//	ctcp.LogPrint("正晨服务", "删除一个模块，并删除和模块相关的点失败！");
		LOGE("正晨服务" << "删除一个模块，并删除和模块相关的点失败！");
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
	}
	//将返回值组装成json数据
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//将strErrmsg字符串数据放到errmsg节点
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//返回配置信息
}

Json::Value CDdataManager::GetMeterParameter(Json::Value jsonData)
{
	//声明接口所需要的参数
	const char *IPAddr;
	unsigned int MeterID;
	MeterParameter pParameter;
	memset(&pParameter, 0, sizeof(MeterParameter));
	char *errmsg = "";
	//解析javaWeb客户端发来的json数据
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	MeterID = atoi(jsonData["MeterID"].asString().c_str());

	//下位机数据返回成功或失败的标识
	int Index;
	Json::Value jsonResult;
	std::string strErrmsg;
	if (m_besManager.BES200_GetMeterParameter(IPAddr, MeterID, &pParameter, &errmsg) == 0)
	{
	//	ctcp.LogPrint("正晨服务", "设置一个控制器的所有参数！");
		Index = 0;
		Json::Value jsonMeterParameter;
		Json::Value jsonElectricDataCollectMethod;
		Json::Value jsonElectricDataInfo;
		strErrmsg = "操作成功";
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);

		//获取MeterParameter结构的数据
		jsonMeterParameter["MeterID"] = Json::Value(pParameter.MeterID);
		jsonMeterParameter["MeterType"] = Json::Value(pParameter.MeterType);
		jsonMeterParameter["ComRate"] = Json::Value(pParameter.ComRate);
		jsonMeterParameter["ComPort"] = Json::Value(pParameter.ComPort);
		jsonMeterParameter["ComAgreementType"] = Json::Value(pParameter.ComAgreementType);
		string strPhyAddr = "";
		//if (pParameter.MeterType == METER_TYPE_WATER){
		//	strPhyAddr = Getstring2(pParameter.PhyAddr[6]) + Getstring2(pParameter.PhyAddr[5]) + Getstring2(pParameter.PhyAddr[4]) + Getstring2(pParameter.PhyAddr[3]) + Getstring2(pParameter.PhyAddr[2]) + Getstring2(pParameter.PhyAddr[1]) + Getstring2(pParameter.PhyAddr[0]);
		//}
		//else{
		//	strPhyAddr = Getstring2(pParameter.PhyAddr[5]) + Getstring2(pParameter.PhyAddr[4]) + Getstring2(pParameter.PhyAddr[3]) + Getstring2(pParameter.PhyAddr[2]) + Getstring2(pParameter.PhyAddr[1]) + Getstring2(pParameter.PhyAddr[0]);
		//}
		


		if ((pParameter.ComAgreementType == COMAGREEMENTTYPE_CJT188) || 
			(pParameter.ComAgreementType == COMAGREEMENTTYPE_SIMPLE_CJT188) || 
			(pParameter.ComAgreementType == COMAGREEMENTTYPE_W645) || 
			(pParameter.ComAgreementType == COMAGREEMENTTYPE_SDJYL_WaterMeter)) {

			char cData[24] = { 0 };
			uint8_t sk8 = 0;
			int nPos = 0;
			for (int i = 6; i >=0; i--)
			{
				sk8 = pParameter.PhyAddr[i];
				sprintf(cData + nPos, "%02X", sk8);
				nPos += 2;
			}
			strPhyAddr = cData;
			//strPhyAddr = Getstring2(pParameter.PhyAddr[6]) + Getstring2(pParameter.PhyAddr[5]) + Getstring2(pParameter.PhyAddr[4]) + Getstring2(pParameter.PhyAddr[3]) + Getstring2(pParameter.PhyAddr[2]) + Getstring2(pParameter.PhyAddr[1]) + Getstring2(pParameter.PhyAddr[0]);
		}
		else {
			char cData[24] = { 0 };
			uint8_t sk8 = 0;
			int nPos = 0;
			for (int i = 5; i >= 0; i--)
			{
				sk8 = pParameter.PhyAddr[i];
				sprintf(cData + nPos, "%02X", sk8);
				nPos += 2;
			}
			strPhyAddr = cData;
			//strPhyAddr = Getstring2(pParameter.PhyAddr[5]) + Getstring2(pParameter.PhyAddr[4]) + Getstring2(pParameter.PhyAddr[3]) + Getstring2(pParameter.PhyAddr[2]) + Getstring2(pParameter.PhyAddr[1]) + Getstring2(pParameter.PhyAddr[0]);
		}

		jsonMeterParameter["PhyAddr"] = Json::Value(strPhyAddr);
		jsonMeterParameter["Alias"] = Json::Value(pParameter.Alias);
		jsonMeterParameter["Description"] = Json::Value(pParameter.Description);
		jsonMeterParameter["Location"] = Json::Value(pParameter.Location);
		jsonMeterParameter["Active"] = Json::Value(pParameter.Active);
		//将jsonMeterParameter子节点填充到jsonresult节点
		jsonResult["MeterParameter"] = Json::Value(jsonMeterParameter);

		//获取MeterParameter结构的数据
		jsonElectricDataCollectMethod["CollectCount"] = Json::Value(pParameter.CollectMethod.CollectCount);
		jsonElectricDataCollectMethod["CollectMethodID"] = Json::Value(pParameter.CollectMethod.CollectMethodID);
		//将jsonElectricDataCollectMethod子节点填充到jsonresult节点
		jsonResult["ElectricDataCollectMethod"] = Json::Value(jsonElectricDataCollectMethod);

		//获取ElectricDataInfo结构的数据
		int nCount = pParameter.CollectMethod.CollectCount;
		for (int i = 0; i < nCount; i++)
		{
			Json::Value jsonIndex;
			jsonIndex["DataEncodeType"] = Json::Value(pParameter.CollectMethod.ElectricDataInfo[i].DataEncodeType);
			jsonIndex["DataLength"] = Json::Value(pParameter.CollectMethod.ElectricDataInfo[i].DataLength);
			jsonIndex["ElectricID"] = Json::Value(pParameter.CollectMethod.ElectricDataInfo[i].ElectricID);
			jsonIndex["ElectricName"] = Json::Value(pParameter.CollectMethod.ElectricDataInfo[i].ElectricName);
			jsonIndex["OffsetAddr"] = Json::Value(dec2hex(pParameter.CollectMethod.ElectricDataInfo[i].OffsetAddr));
			jsonIndex["PointLocation"] = Json::Value(pParameter.CollectMethod.ElectricDataInfo[i].PointLocation);
			jsonIndex["UintType"] = Json::Value(pParameter.CollectMethod.ElectricDataInfo[i].UintType);
			//int转string获取电能参数key值
			char cIndex[256];
			string strIndex;
			sprintf_s(cIndex, "%d", i);
			strIndex = cIndex;

			//给第strIndex个电能参数添加数据
			jsonElectricDataInfo[strIndex] = Json::Value(jsonIndex);
		}
		//将jsonElectricDataInfo子节点填充到jsonresult节点
		jsonResult["ElectricDataInfo"] = Json::Value(jsonElectricDataInfo);
	}
	else
	{
//		ctcp.LogPrint("正晨服务", "设置一个控制器的所有参数失败！");
		LOGE("正晨服务" << "设置一个控制器的所有参数失败！");
		Index = 1;
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
	}

	// 转换为字符串（带格式）
	//std::string out = jsonResult.toStyledString();
	//cout <<out<< endl;


	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//返回配置信息
}

Json::Value CDdataManager::GetMeterElectricData(Json::Value jsonData)
{
	//声明接口所需要的参数																			
	const char *IPAddr;
	unsigned int MeterID;
	MeterElectricData pElectricData;
	char *errmsg = "";
	//解析javaWeb客户端发来的json数据																
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	MeterID = atoi(jsonData["MeterID"].asString().c_str());
	//LOG_INFO("GetMeterElectricData！");
	//下位机数据返回成功或失败的标识
	int Index;
	Json::Value jsonResult;
	std::string strErrmsg;
	if (m_besManager.BES200_GetMeterElectricData(IPAddr, MeterID, &pElectricData, &errmsg) == 0)
	{
	//	ctcp.LogPrint("正晨服务", "获取电表的所有电表电能参数成功！");
		Index = 0;
		Json::Value jsonMeterElectricData;
		strErrmsg = "操作成功";
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
		//为MeterElectricData节点添加数据
		jsonMeterElectricData["MeterID"] = Json::Value(pElectricData.MeterID);
		jsonMeterElectricData["CollectCount"] = Json::Value(pElectricData.CollectCount);
		string strElectricData;
		for (int i = 0; i < pElectricData.CollectCount; i++)
		{
			strElectricData = strElectricData + ',' + Getstring3(pElectricData.ElectricData[i]);
		}
		jsonMeterElectricData["ElectricData"] = Json::Value(strElectricData);
		jsonMeterElectricData["DateYear"] = Json::Value(pElectricData.DateYear);
		jsonMeterElectricData["DateMonth"] = Json::Value(pElectricData.DateMonth);
		jsonMeterElectricData["DateDay"] = Json::Value(pElectricData.DateDay);
		jsonMeterElectricData["TimeHour"] = Json::Value(pElectricData.TimeHour);
		jsonMeterElectricData["TimeMinute"] = Json::Value(pElectricData.TimeMinute);
		jsonMeterElectricData["TimeSecond"] = Json::Value(pElectricData.TimeSecond);
		//将MeterElectricData子节点添加到根节点
		jsonResult["MeterElectricData"] = Json::Value(jsonMeterElectricData);
	}
	else
	{
	//	ctcp.LogPrint("正晨服务", "获取电表的所有电表电能参数失败！");
		LOGE("正晨服务" << "获取电表的所有电表电能参数失败！");
		Index = 1;
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
	}
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//返回配置信息
}



void CDdataManager::GetMeterElectricData2(Json::Value jsonData, std::string &result)
{
	//声明接口所需要的参数																			
	const char *IPAddr;
	unsigned int MeterID;
	MeterElectricData pElectricData;
	char *errmsg = "";
	//解析javaWeb客户端发来的json数据																
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	MeterID = atoi(jsonData["MeterID"].asString().c_str());
	//LOG_INFO("GetMeterElectricData！");
	//下位机数据返回成功或失败的标识
	int Index;
	Json::Value jsonResult;
	std::string strErrmsg;
	if (m_besManager.BES200_GetMeterElectricData(IPAddr, MeterID, &pElectricData, &errmsg) == 0)
	{
		//	ctcp.LogPrint("正晨服务", "获取电表的所有电表电能参数成功！");
		Index = 0;
		Json::Value jsonMeterElectricData;
		strErrmsg = "操作成功";
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
		//为MeterElectricData节点添加数据
		jsonMeterElectricData["MeterID"] = Json::Value(pElectricData.MeterID);
		jsonMeterElectricData["CollectCount"] = Json::Value(pElectricData.CollectCount);
		string strElectricData;
		for (int i = 0; i < pElectricData.CollectCount; i++)
		{
			strElectricData = strElectricData + ',' + Getstring3(pElectricData.ElectricData[i]);
		}
		jsonMeterElectricData["ElectricData"] = Json::Value(strElectricData);
		jsonMeterElectricData["DateYear"] = Json::Value(pElectricData.DateYear);
		jsonMeterElectricData["DateMonth"] = Json::Value(pElectricData.DateMonth);
		jsonMeterElectricData["DateDay"] = Json::Value(pElectricData.DateDay);
		jsonMeterElectricData["TimeHour"] = Json::Value(pElectricData.TimeHour);
		jsonMeterElectricData["TimeMinute"] = Json::Value(pElectricData.TimeMinute);
		jsonMeterElectricData["TimeSecond"] = Json::Value(pElectricData.TimeSecond);
		//将MeterElectricData子节点添加到根节点
		jsonResult["MeterElectricData"] = Json::Value(jsonMeterElectricData);
		string ss = jsonResult.toStyledString();
		//result = jsonResult.toStyledString();
		result = ss;
	}
	else
	{
		//	ctcp.LogPrint("正晨服务", "获取电表的所有电表电能参数失败！");
		LOGE("正晨服务" << "获取电表的所有电表电能参数失败！");
		Index = 1;
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);

		result = jsonResult.toStyledString();
	}
	m_besManager.BES200_Free(&errmsg);
}

Json::Value CDdataManager::GetMeterHisElectricData(Json::Value jsonData)
{
	//BES200_GetMeterHisElectricData(const char *IPAddr, uint32_t MeterID, uint8_t Selectday, struct MeterHisElectricData *pHisElectricData, char **errmsg);
	//声明接口所需要的参数																			
	const char *IPAddr;
	unsigned int MeterID;
	uint8_t Selectday;
	MeterHisElectricData pHisElectricData;
	char *errmsg = "";
	//解析javaWeb客户端发来的json数据																
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	MeterID = atoi(jsonData["MeterID"].asString().c_str());
	Selectday = atoi(jsonData["Selectday"].asString().c_str());
	//下位机数据返回成功或失败的标识
	int Index;
	Json::Value jsonResult;
	std::string strErrmsg;
	if (m_besManager.BES200_GetMeterHisElectricData(IPAddr, MeterID, Selectday, &pHisElectricData, &errmsg) == 0)
	{
//		ctcp.LogPrint("正晨服务", "获取电表的历史电表电能参数成功！");
		Index = 0;
		Json::Value jsonMeterHisElectricData;
		Json::Value jsonMeterElectricHisData;
		strErrmsg = "操作成功";
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
		//为MeterHisElectricData节点添加数据
		jsonMeterHisElectricData["HisDataHourCount"] = Json::Value(pHisElectricData.HisDataHourCount);
		jsonMeterHisElectricData["MeterID"] = Json::Value(pHisElectricData.MeterID);
		int Count = pHisElectricData.HisDataHourCount;
		//为MeterElectricHisData节点添加数据
		for (int i = 0; i < Count; i++)
		{

			Json::Value jsonIndex;
			jsonIndex["CollectCount"] = Json::Value(pHisElectricData.MeterElectricData[i].CollectCount);
			jsonIndex["DateDay"] = Json::Value(pHisElectricData.MeterElectricData[i].DateDay);
			jsonIndex["DateMonth"] = Json::Value(pHisElectricData.MeterElectricData[i].DateMonth);
			jsonIndex["DateYear"] = Json::Value(pHisElectricData.MeterElectricData[i].DateYear);
			string strElectricData;
			for (int j = 0; j < pHisElectricData.MeterElectricData[i].CollectCount; j++)
			{
				strElectricData = strElectricData + ',' + Getstring3(pHisElectricData.MeterElectricData[i].ElectricData[j]);
			}
			jsonIndex["ElectricData"] = Json::Value(strElectricData);
			jsonIndex["MeterID"] = Json::Value(pHisElectricData.MeterElectricData[i].MeterID);
			jsonIndex["TimeHour"] = Json::Value(pHisElectricData.MeterElectricData[i].TimeHour);
			jsonIndex["TimeMinute"] = Json::Value(pHisElectricData.MeterElectricData[i].TimeMinute);
			jsonIndex["TimeSecond"] = Json::Value(pHisElectricData.MeterElectricData[i].TimeSecond);

			//int转string获取电能参数key值
			char cIndex[256];
			string strIndex;
			sprintf_s(cIndex, "%d", i);
			strIndex = cIndex;
			//给第strIndex个电能参数添加数据
			jsonMeterElectricHisData[strIndex] = Json::Value(jsonIndex);

		}
		//jsonResult添加子节点
		jsonResult["MeterHisElectricData"] = Json::Value(jsonMeterHisElectricData);
		jsonResult["MeterElectricHisData"] = Json::Value(jsonMeterElectricHisData);

	}
	else
	{
	//	ctcp.LogPrint("正晨服务", "获取电表的历史电表电能参数失败！");
		LOGE("正晨服务" << "获取电表的历史电表电能参数失败！");
		Index = 1;
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
	}
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//返回配置信息
}

Json::Value CDdataManager::GetAlarmInfor(Json::Value jsonData)
{
	//声明接口所需要的参数																			
	const char *IPAddr;
	AlarmInfor AlarmInfor;
	char *errmsg = "";
	//解析javaWeb客户端发来的json数据																
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();

	int Index;
	Json::Value jsonResult;
	std::string strErrmsg;
	if (m_besManager.BES200_GetAlarmInfor(IPAddr, &AlarmInfor, &errmsg) == 0)
	{
//		ctcp.LogPrint("正晨服务", "实时获取采集器的报警信息成功！");
		Index = 0;
		Json::Value jsonAlarmInfor;
		strErrmsg = "操作成功";
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
		jsonAlarmInfor["AlarmInforCategory"] = Json::Value(AlarmInfor.AlarmInforCategory);
		jsonAlarmInfor["AlarmInforDateDay"] = Json::Value(AlarmInfor.AlarmInforDateDay);
		jsonAlarmInfor["AlarmInforDateMonth"] = Json::Value(AlarmInfor.AlarmInforDateMonth);
		jsonAlarmInfor["AlarmInforDateYear"] = Json::Value(AlarmInfor.AlarmInforDateYear);
		jsonAlarmInfor["AlarmInforID"] = Json::Value(AlarmInfor.AlarmInforID);
		jsonAlarmInfor["AlarmInforSeqNum"] = Json::Value(AlarmInfor.AlarmInforSeqNum);
		jsonAlarmInfor["AlarmInforTimeHour"] = Json::Value(AlarmInfor.AlarmInforTimeHour);
		jsonAlarmInfor["AlarmInforTimeMinute"] = Json::Value(AlarmInfor.AlarmInforTimeMinute);
		jsonAlarmInfor["AlarmInforTimeSecond"] = Json::Value(AlarmInfor.AlarmInforTimeSecond);
		jsonAlarmInfor["AlarmInforType"] = Json::Value(AlarmInfor.AlarmInforType);
		//jsonResult添加子节点
		jsonResult["AlarmInfor"] = Json::Value(jsonAlarmInfor);
	}
	else
	{
//		ctcp.LogPrint("正晨服务", "实时获取采集器的报警信息失败！");
		LOGE("正晨服务" << "实时获取采集器的报警信息失败！");
		Index = 1;
		strErrmsg = (std::string)errmsg;//配置不成功，获取错误信息
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
	}
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//返回配置信息
}

Json::Value CDdataManager::Test(Json::Value jsonData)
{
	std::string strIndex = jsonData["index"].asString();
	int	nIndex = atoi(strIndex.c_str());

	//获取时间
	if (nIndex == 20)
	{
		Json::Value jsonResult;
		jsonResult["errmsg"] = Json::Value("操作成功");//将strErrmsg字符串数据放到errmsg节点

		tm pTime;//声明时间结构体
		//为时间结构体添加数据
		time_t now;
		time(&now);
		localtime_s(&pTime,&now);

		Json::Value jsonTime;
		jsonTime["tm_sec"] = Json::Value(pTime.tm_sec);
		jsonTime["tm_min"] = Json::Value(pTime.tm_min);
		jsonTime["tm_hour"] = Json::Value(pTime.tm_hour);
		jsonTime["tm_mday"] = Json::Value(pTime.tm_mday);
		jsonTime["tm_mon"] = Json::Value(pTime.tm_mon + 1);
		jsonTime["tm_year"] = Json::Value(pTime.tm_year + 1900);
		jsonTime["tm_wday"] = Json::Value(pTime.tm_wday);
		jsonTime["tm_yday"] = Json::Value(pTime.tm_yday);
		jsonTime["tm_isdst"] = Json::Value(pTime.tm_isdst);
		jsonResult["tm"] = Json::Value(jsonTime);

		return jsonResult;
	}
	//获取控制器的所有配置参数
	if (nIndex == 21)
	{
		Json::Value jsonResult;
		jsonResult["errmsg"] = Json::Value("操作成功");
		//子节点储存CollectControllerParameter结构体的数据
		Json::Value jsonParameter;
		//子节点属性
		jsonParameter["ID"] = Json::Value("111");
		jsonParameter["Name"] = Json::Value("名字");
		jsonParameter["Alias"] = Json::Value("aa");
		jsonParameter["Description"] = Json::Value("aaa");
		jsonParameter["Location"] = Json::Value("aaaaa");
		jsonParameter["Zone"] = Json::Value("aaaaaa");
		jsonParameter["Active"] = Json::Value("aasax");
		jsonParameter["SamplePeriod"] = Json::Value("sas");
		jsonParameter["HisdataSavePeriod"] = Json::Value("his");
		//将子节点的值放到根节点
		jsonResult["CollectControllerParameter"] = Json::Value(jsonParameter);
		return jsonResult;
	}
	//广播方式与其他控制器通信，获取所有控制器的信息
	if (nIndex == 22)
	{
		Json::Value jsonResult;
		jsonResult["errmsg"] = Json::Value("操作成功");
		//定义jsonResult子节点储存CollectControllerInfo结构数组的数据
		Json::Value jsonCollectControllerInfo;

		int size = 2;
		for (int i = 0; i < size; i++)
		{
			//子节点储存CollectControllerInfo结构体的数据																			
			Json::Value jsonInfo;
			//子节点属性																						 
			jsonInfo["Name"] = Json::Value("name");
			jsonInfo["CampanyName"] = Json::Value("campany");
			jsonInfo["Model"] = Json::Value("model");
			jsonInfo["HWVer"] = Json::Value("hwver");
			jsonInfo["FWVer"] = Json::Value("fwver");
			jsonInfo["IP"] = Json::Value("ip");
			jsonInfo["Mask"] = Json::Value("mask");
			jsonInfo["GateWay"] = Json::Value("gateway");
			jsonInfo["Mac"] = Json::Value("mac");
			jsonInfo["ID"] = Json::Value("id");
			jsonInfo["Alias"] = Json::Value("alias");
			jsonInfo["Description"] = Json::Value("desc");
			jsonInfo["Location"] = Json::Value("location");
			jsonInfo["Zone"] = Json::Value("zone");
			jsonInfo["Active"] = Json::Value("active");
			jsonInfo["SamplePeriod"] = Json::Value("sample");
			jsonInfo["HisdataSavePeriod"] = Json::Value("hisdata");

			//将int转为string获取节点名
			char cIndex[256];
			string strIndex;
			sprintf_s(cIndex, "%d", i);
			strIndex = cIndex;

			//将子节点的值放到根节点	
			jsonCollectControllerInfo[strIndex] = Json::Value(jsonInfo);
		}
		jsonResult["CollectControllerInfo"] = Json::Value(jsonCollectControllerInfo);
		return jsonResult;
	}
	//获取模块的所有配置信息
	if (nIndex == 23)
	{
		Json::Value jsonResult;
		jsonResult["errmsg"] = Json::Value("操作成功");
		//定义jsonResult子节点储存CollectControllerInfo结构数组的数据

		Json::Value jsonMeterParameter;
		Json::Value jsonElectricDataCollectMethod;
		Json::Value jsonElectricDataInfo;
		//获取MeterParameter结构的数据
		jsonMeterParameter["MeterID"] = Json::Value("meterid");
		jsonMeterParameter["MeterType"] = Json::Value("metertype");
		jsonMeterParameter["ComRate"] = Json::Value("comrate");
		jsonMeterParameter["ComPort"] = Json::Value("comport");
		jsonMeterParameter["ComAgreementType"] = Json::Value("comAgreementtype");
		jsonMeterParameter["PhyAddr"] = Json::Value("phyaddr");
		jsonMeterParameter["Alias"] = Json::Value("alias");
		jsonMeterParameter["Description"] = Json::Value("description");
		jsonMeterParameter["Location"] = Json::Value("location");
		jsonMeterParameter["Active"] = Json::Value("active");
		//将jsonMeterParameter子节点填充到jsonresult节点
		jsonResult["MeterParameter"] = Json::Value("jsonMeterParameter");

		//获取MeterParameter结构的数据
		jsonElectricDataCollectMethod["CollectCount"] = Json::Value("2");
		jsonElectricDataCollectMethod["CollectMethodID"] = Json::Value("collectmethodid");
		//将jsonElectricDataCollectMethod子节点填充到jsonresult节点
		jsonResult["ElectricDataCollectMethod"] = Json::Value(jsonElectricDataCollectMethod);

		//获取ElectricDataInfo结构的数据
		int nCount = 2;
		for (int i = 0; i < nCount; i++)
		{
			Json::Value jsonIndex;
			jsonIndex["DataEncodeType"] = Json::Value("dataencode");
			jsonIndex["DataLength"] = Json::Value("datalength");
			jsonIndex["ElectricID"] = Json::Value("electricid");
			jsonIndex["ElectricName"] = Json::Value("electricid");
			jsonIndex["OffsetAddr"] = Json::Value("offsetaddr");
			jsonIndex["PointLocation"] = Json::Value("pointlocation");
			jsonIndex["UintType"] = Json::Value("uinttype");
			//int转string获取电能参数key值
			char cIndex[256];
			string strIndex;
			sprintf_s(cIndex, "%d", i);
			strIndex = cIndex;

			//给第strIndex个电能参数添加数据
			jsonElectricDataInfo[strIndex] = Json::Value(jsonIndex);
		}
		//将jsonElectricDataInfo子节点填充到jsonresult节点
		jsonResult = Json::Value(jsonElectricDataInfo);
		return jsonResult;
	}
	// 获取电表的所有电表电能参数
	if (nIndex == 24)
	{
		Json::Value jsonResult;
		jsonResult["errmsg"] = Json::Value("操作成功");
		Json::Value jsonMeterElectricData;

		//为MeterElectricData节点添加数据
		jsonMeterElectricData["MeterID"] = Json::Value("meterid");
		jsonMeterElectricData["ElectricData"] = Json::Value("electricdata");
		jsonMeterElectricData["CollectCount"] = Json::Value("collectcount");
		jsonMeterElectricData["DateYear"] = Json::Value("dateyear");
		jsonMeterElectricData["DateMonth"] = Json::Value("datemon");
		jsonMeterElectricData["DateDay"] = Json::Value("DateDay");
		jsonMeterElectricData["TimeHour"] = Json::Value("timehour");
		jsonMeterElectricData["TimeMinute"] = Json::Value("timeminute");
		jsonMeterElectricData["TimeSecond"] = Json::Value("timesecond");
		//将MeterElectricData子节点添加到根节点
		jsonResult["MeterElectricData"] = Json::Value(jsonMeterElectricData);
		return jsonResult;
	}
	//获取电表的历史电表电能参数
	if (nIndex == 25)
	{
		Json::Value jsonResult;
		jsonResult["errmsg"] = Json::Value("操作成功");

		Json::Value jsonMeterHisElectricData;
		Json::Value jsonMeterElectricHisData;
		//为MeterHisElectricData节点添加数据
		jsonMeterHisElectricData["HisDataHourCount"] = Json::Value("hisdatahourcount");
		jsonMeterHisElectricData["MeterID"] = Json::Value("meterid");
		int Count = 2;
		//为MeterElectricHisData节点添加数据
		for (int i = 0; i < Count; i++)
		{
			Json::Value jsonIndex;
			jsonIndex["CollectCount"] = Json::Value("collectcount");
			jsonIndex["DateDay"] = Json::Value("dateday");
			jsonIndex["DateMonth"] = Json::Value("datemonth");
			jsonIndex["DateYear"] = Json::Value("dateyear");
			jsonIndex["ElectricData"] = Json::Value("electricdata");
			jsonIndex["MeterID"] = Json::Value("meterid");
			jsonIndex["TimeHour"] = Json::Value("timehour");
			jsonIndex["TimeMinute"] = Json::Value("timeminute");
			jsonIndex["TimeSecond"] = Json::Value("timesecond");

			//int转string获取电能参数key值
			char cIndex[256];
			string strIndex;
			sprintf_s(cIndex, "%d", i);
			strIndex = cIndex;
			//给第strIndex个电能参数添加数据
			jsonMeterElectricHisData[strIndex] = Json::Value(jsonIndex);

		}
		//jsonResult添加子节点
		jsonResult["MeterHisElectricData"] = Json::Value(jsonMeterHisElectricData);
		jsonResult["MeterElectricHisData"] = Json::Value(jsonMeterElectricHisData);
		return jsonResult;
	}
	//实时获取采集器的报警信息
	if (nIndex == 26)
	{
		Json::Value jsonResult;
		Json::Value jsonAlarmInfor;
		jsonResult["errmsg"] = Json::Value("操作成功");
		jsonAlarmInfor["AlarmInforCategory"] = Json::Value("alarminforca");
		jsonAlarmInfor["AlarmInforDateDay"] = Json::Value("AlarmInforDateDay");
		jsonAlarmInfor["AlarmInforDateMonth"] = Json::Value("AlarmInforDateMonth");
		jsonAlarmInfor["AlarmInforDateYear"] = Json::Value("AlarmInforDateYear");
		jsonAlarmInfor["AlarmInforID"] = Json::Value("AlarmInforID");
		jsonAlarmInfor["AlarmInforSeqNum"] = Json::Value("AlarmInforSeqNum");
		jsonAlarmInfor["AlarmInforTimeHour"] = Json::Value("AlarmInforTimeHour");
		jsonAlarmInfor["AlarmInforTimeMinute"] = Json::Value("AlarmInforTimeMinute");
		jsonAlarmInfor["AlarmInforTimeSecond"] = Json::Value("AlarmInforTimeSecond");
		jsonAlarmInfor["AlarmInforType"] = Json::Value("AlarmInforType");
		//jsonResult添加子节点
		jsonResult["AlarmInfor"] = Json::Value(jsonAlarmInfor);
		return jsonResult;
	}
	return "";
}

string CDdataManager::Getstring(unsigned char number)
{
	string strNumber;//接收转换数据的字符串
	char cIndex[256];//转换媒介
	sprintf_s(cIndex, "%d", number);
	strNumber = cIndex;
	return strNumber;
}

string CDdataManager::Getstring2(unsigned char number)
{
	int n = (number / 16 * 10) + number % 16;
	string strNumber;//接收转换数据的字符串
	char cIndex[256];//转换媒介
	sprintf_s(cIndex, "%d", n);
	strNumber = cIndex;
	if (number < 10)
	{
		return '0' + strNumber;
	}
	return strNumber;
}

string CDdataManager::Getstring3(unsigned int number)
{
	string strNumber;//接收转换数据的字符串
	char cIndex[256];//转换媒介
	sprintf_s(cIndex, "%d", number);
	strNumber = cIndex;
	return strNumber;
}

int CDdataManager::GetDecimalism(string strNumber)
{
	//第一种方法
	//char *str; 
	//int i = (int)strtol(strNumber.c_str(), &str, 16);//十六进制   
	//return i;  

	//第二种方法
	int nValude = 0;
	sscanf_s(strNumber.c_str(), "%x", &nValude);
	return nValude;
}

string CDdataManager::dec2hex(int number)
{
	stringstream ioss; //定义字符串流
	string strNumber; //存放转化后字符
	ioss << std::hex << number; //以十六制形式输出
	ioss >> strNumber;
	//std::string s(width - s_temp.size(), '0'); //补0
	//s += s_temp; //合并
	return strNumber;
}
