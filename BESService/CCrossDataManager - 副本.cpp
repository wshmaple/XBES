#include "stdafx.h"
#include "CCrossDataManager.h"
#include "Gloable_Cross.h"

using namespace Gloable_Cross;
#define  get_data_size(structHead,structData)  BES200_htons(sizeof(structHead)+sizeof(structData))
CCrossDataManager::CCrossDataManager()
{
}


CCrossDataManager::~CCrossDataManager()
{
}
uint16_t CCrossDataManager::BES200_htons(uint16_t n)
{
	return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}

Json::Value CCrossDataManager::ComDataManager(Json::Value jsonData)
{
	int nIndex = atoi(jsonData["index"].asString().c_str());
	int nPort = 0;
	nPort = atoi(jsonData["Port"].asString().c_str());
	string strIP = jsonData["IPAddr"].asString();
	const char *IPAddr = NULL;
	IPAddr = strIP.c_str();
	string strItem = jsonData["item"].asString();

	//声明接口所需要的参数	
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };

	int nSendSize = 0;
	switch (nIndex)
	{
	case BEGP_FUNCODE_ADD_CONTROLLER_PARAMETER:							//增加一个控制器
			nSendSize = AddControlParameter(jsonData, SendBuf);
		break;
	case BEGP_FUNCODE_SET_CONTROLLER_PARAMETER:							//设置一个控制器
			nSendSize = SetControlParameter(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_SET_CONTROLLER_TIME	:					// 设置控制器的时间
	case		BEGP_FUNCODE_REBOOT_CONTROLLER		:				    // 重启控制器，相当于复位
	case		BEGP_FUNCODE_RESET_CONTROLLER       :					// 重置控制器，恢复出厂设置，并重启
	case		BEGP_FUNCODE_DELETE_CONTROLLER      :				    // 删除一个控制器，并删除和它相关的模块和点
			nSendSize = OnlySendCmdToHard(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_GET_CONTROLLER_TIME      :					//获取控制器的时间
	case		BEGP_FUNCODE_GET_CONTROLLER_PARAMETER :					//获取控制器的所有配置参数
			nSendSize = OnlySendCmdToHard(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_GET_CONTROLLER_INFO      :					//通过UDP广播形式获取所有控制器的信息
		break;
	case		BEGP_FUNCODE_SET_CONTROLLER_INFO      :					//通过UDP广播形式设置控制器的参数
		break;
	case		BEGP_FUNCODE_ADD_MODULE          :						//新增加一个模块
			nSendSize = AddOrSetControllerModule(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_SET_MODULE_PARAMETER     :					//设置一个模块的所有参数
			nSendSize = AddOrSetControllerModule(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_DELETE_MODULE            :					//删除一个模块，并删除和模块相关的点
	case		BEGP_FUNCODE_GET_MODULE_PARAMETER     :					//获取一个模块的所有配置信息
	case		BEGP_FUNCODE_GET_MODULE_ERROR_CODE    :					//获取一个模块的错误代码
		//	nSendSize = OnlySendCmdToHard(jsonData, SendBuf);
		break;

	case		BEGP_FUNCODE_ADD_LOGICAL_POINT             :			//新增加一个逻辑点
			nSendSize =  AddOrSetPoint(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_SET_LOGICAL_POINT_PARAMETER   :			//设置一个逻辑点的所有参数
			nSendSize =  AddOrSetPoint(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_SET_LOGICAL_POINT_VALUE        :			//设置一个逻辑点的值
			nSendSize = SetPointValue(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_SET_LOGICAL_POINT_VALUE_BY_NAME:			//设置逻辑点的值，根据点的名字
			nSendSize = SetPointValueByName(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_DELETE_LOGICAL_POINT          :			//删除一个逻辑点
			nSendSize = DeletePointByID(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_GET_LOGICAL_POINT_PARAMETER   :			//获取一个逻辑点的所有配置参数
			nSendSize = GetPointParameter(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_GET_LOGICAL_POINT_ALARM_INFOR:				//获取一个逻辑点的报警信息
			nSendSize = GetPointAlarmErrorCode(jsonData, SendBuf);
			
		break;

	case		BEGP_FUNCODE_ADD_SCENE:									//增加一个场景
			
		nSendSize = AddScene(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_SET_SCENE_PARAMETER:						//设置一个场景的所有参数
		nSendSize = SetScene(jsonData, SendBuf);

		break;
	case		BEGP_FUNCODE_DELETE_SCENE:								//删除一个场景
		nSendSize = DelScene(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_GET_SCENE_PARAMETER:						//获取一个场景的配置信息
		nSendSize = GetScene(jsonData, SendBuf);
		break;

	case		BEGP_FUNCODE_ADD_SCHEDULER:								//增加一条计划
		nSendSize = AddScheduler(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_SET_SCHEDULER_PARAMETER:					//修改一条计划的所有参数
		nSendSize = SetScheduler(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_DELETE_SCHEDULER:							//删除一条计划
		nSendSize = DelScheduler(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_GET_SCHEDULER_PARAMETER:					//获取一条计划的所有参数
		nSendSize = GetScheduler(jsonData, SendBuf);
		break;
	
	case		BEGP_FUNCUDE_IAP:										//远程升级
		nSendSize = UpDate(jsonData,SendBuf);
		break;
	}
	int nRecvSize = 0;
	BEGP_REQ_HEAD_T *pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	BEGP_RSP_HEAD_T *pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	int nRes = m_besManager.BES200_Communication2(IPAddr, nPort, SendBuf, nSendSize, RecvBuf, &nRecvSize);

	// 此处的结果是不是要重新组装呢
	// 我觉得是需要组装的呀 RecvBuf
	
	//将返回值组装成json数据
	Json::Value jsonResult;
	jsonResult["code"] = Json::Value(nRes);
	jsonResult["item"] = Json::Value(strItem);
	jsonResult["index"] = Json::Value(nIndex);
	jsonResult["IPAddr"] = Json::Value(strIP);
	jsonResult["Port"] = Json::Value(nPort);

	Json::Value jsonResultData;

	// 处理 返回的结果值
	
	int nRetCode = -1;
	// 检查 头是不是正确的
	
	if (pHead_rsp->FunctionCode == pHead_req->FunctionCode && 
		pHead_rsp->SlaveAddress == pHead_req->SlaveAddress)
	{
		// 具体返回数据的 数据长度 要根据不同的 调用请求来具体的处理 这里要进行分类处理
		
		void *pRecvData =(RecvBuf + BEGP_RSP_HEAD_SIZE);

		switch (nIndex)
		{
		case 0:
		{
			nRetCode = ((BEGP_RSP_PACKETDATA_ECHO_T*)pRecvData)->Code;
			if (nRetCode == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				switch (nRetCode)
				{
				case 0:
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				default:
					break;
				}
			}
		}
			break;
		case BEGP_FUNCODE_GET_CONTROLLER_TIME:// 获取系统时间
			{

				// 源码中
				// (BES200_htons(pHead_rsp->DataSize) >= BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_SIZE))
				int nSize1 = pHead_rsp->DataSize;
				int nSize2 = BES200_htons(sizeof(BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_T));
				if (nSize1 != nSize2)
				{
					// 说明返回的数据 有问题了吧
					// 
				}
				jsonResultData = 	ParseGetTime(pRecvData);
				
			}
			break;
		case BEGP_FUNCODE_GET_CONTROLLER_PARAMETER:// 获取控制器的所有配置参数
		{

			jsonResultData = ParseGetControlPar(pRecvData);
		}
			break;
		case BEGP_FUNCODE_GET_MODULE_PARAMETER: //获取模块的所有配置信息
		{
			jsonResultData = ParseGetModePar(pRecvData);
		}
			break;
		case  BEGP_FUNCODE_GET_MODULE_ERROR_CODE://获取模块的错误代码
		{
			BEGP_RSP_PACKETDATA_GET_MODULE_ERROR_CODE_T *lpVData = (BEGP_RSP_PACKETDATA_GET_MODULE_ERROR_CODE_T*)pRecvData;
			jsonResultData["ErrorCode"] = Json::Value(lpVData->ErrorCode);
		}
		case BEGP_FUNCODE_GET_LOGICAL_POINT_PARAMETER:// 获取一个逻辑点的所有配置参数
		{
			jsonResultData = ParseGetPointPar(pRecvData);

		}
			break;
		case		BEGP_FUNCODE_GET_SCHEDULER_PARAMETER: //获取一条计划的所有参数.
		{
			jsonResultData = ParseGetPlan(pRecvData);
		}
			break;


		}

	}

	////// 明天 具体 弄这一部份 主要是 接收 消息的处理
	
	switch (nIndex)
	{
	case BEGP_FUNCODE_ADD_CONTROLLER_PARAMETER://增加一个控制器
		break;
	case BEGP_FUNCODE_SET_CONTROLLER_PARAMETER://设置一个控制器
		break;
	case		BEGP_FUNCODE_SET_CONTROLLER_TIME:					// 设置控制器的时间
	case		BEGP_FUNCODE_REBOOT_CONTROLLER:				    //重启控制器，相当于复位
	case		BEGP_FUNCODE_RESET_CONTROLLER:					//重置控制器，恢复出厂设置，并重启
	case		BEGP_FUNCODE_DELETE_CONTROLLER:				    //删除一个控制器，并删除和它相关的模块和点
		break;
	case		BEGP_FUNCODE_GET_CONTROLLER_TIME:  //获取控制器的时间
		break;
	case		BEGP_FUNCODE_GET_CONTROLLER_PARAMETER:   //获取控制器的所有配置参数

		break;
	case		BEGP_FUNCODE_GET_CONTROLLER_INFO:   //通过UDP广播形式获取所有控制器的信息
		break;
	case		BEGP_FUNCODE_SET_CONTROLLER_INFO:   //通过UDP广播形式设置控制器的参数
		break;
	case		BEGP_FUNCODE_ADD_MODULE:      //新增加一个模块

		break;
	case		BEGP_FUNCODE_SET_MODULE_PARAMETER:  //设置一个模块的所有参数

		break;
	case		BEGP_FUNCODE_DELETE_MODULE:  //删除一个模块，并删除和模块相关的点
	case		BEGP_FUNCODE_GET_MODULE_PARAMETER:  //获取一个模块的所有配置信息
		break;
	case		BEGP_FUNCODE_GET_MODULE_ERROR_CODE:  //获取一个模块的错误代码
		// 有个 errorcode 的错误代码 需要进行处理
		break;

	case		BEGP_FUNCODE_ADD_LOGICAL_POINT://新增加一个逻辑点

		break;
	case		BEGP_FUNCODE_SET_LOGICAL_POINT_PARAMETER://设置一个逻辑点的所有参数

		break;
	case		BEGP_FUNCODE_SET_LOGICAL_POINT_VALUE://设置一个逻辑点的值

		break;
	case		BEGP_FUNCODE_SET_LOGICAL_POINT_VALUE_BY_NAME://设置逻辑点的值，根据点的名字

		break;
	case		BEGP_FUNCODE_DELETE_LOGICAL_POINT://删除一个逻辑点

		break;
	case		BEGP_FUNCODE_GET_LOGICAL_POINT_PARAMETER://获取一个逻辑点的所有配置参数

		break;
	case		BEGP_FUNCODE_GET_LOGICAL_POINT_ALARM_INFOR: //获取一个逻辑点的报警信息


		break;

	case		BEGP_FUNCODE_ADD_SCENE:              //增加一个场景


		break;
	case		BEGP_FUNCODE_SET_SCENE_PARAMETER:     //设置一个场景的所有参数


		break;
	case		BEGP_FUNCODE_DELETE_SCENE:            //删除一个场景
	
		break;
	case		BEGP_FUNCODE_GET_SCENE_PARAMETER:     //获取一个场景的配置信息

		break;

	case		BEGP_FUNCODE_ADD_SCHEDULER:           //增加一条计划

		break;
	case		BEGP_FUNCODE_SET_SCHEDULER_PARAMETER: //修改一条计划的所有参数

		break;
	case		BEGP_FUNCODE_DELETE_SCHEDULER:        //删除一条计划

		break;
	case		BEGP_FUNCODE_GET_SCHEDULER_PARAMETER: //获取一条计划的所有参数

		break;

	case		BEGP_FUNCUDE_IAP: //远程升级

		break;
	}
 	



	/////
	jsonResult["hcode"] = Json::Value(nRetCode);

	jsonResult["data"] = jsonResultData;
	
	return jsonResult;//返回配置信息
}

int CCrossDataManager::AddControlParameter(Json::Value jsonData, char *sendbuf)
{

	return AddOrSetControllerParameter(jsonData,sendbuf);
}

int CCrossDataManager::SetControlParameter(Json::Value jsonData, char *sendbuf)
{

	return AddOrSetControllerParameter(jsonData, sendbuf);
}

int CCrossDataManager::AddOrSetControllerParameter(Json::Value jsonData, char *sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 1;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T);
	//	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_HEAD_T) + sizeof(BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T));
		// 数据结构体
	BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T tmpAdd;
	tmpAdd.Active = atoi(jsonData["data"]["Active"].asString().c_str());
	tmpAdd.ID = atoi(jsonData["data"]["ID"].asString().c_str());
	strcpy_s(tmpAdd.Alias, jsonData["data"]["Alias"].asString().c_str());
	strcpy_s(tmpAdd.Description, jsonData["data"]["Description"].asString().c_str());
	strcpy_s(tmpAdd.Location, jsonData["data"]["Location"].asString().c_str());
	strcpy_s(tmpAdd.Zone, jsonData["data"]["Zone"].asString().c_str());
	strcpy_s(tmpAdd.Name, jsonData["data"]["Name"].asString().c_str());
	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::SetControllerTime(Json::Value jsonData, char *sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber =0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_T);
	//	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_HEAD_T) + sizeof(BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T));
		// 数据结构体
	BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_T tmpAdd;
	tmpAdd.year = atoi(jsonData["data"]["year"].asString().c_str());
	tmpAdd.month = atoi(jsonData["data"]["month"].asString().c_str());
	tmpAdd.day = atoi(jsonData["data"]["day"].asString().c_str());
	tmpAdd.hour = atoi(jsonData["data"]["hour"].asString().c_str());
	tmpAdd.minute = atoi(jsonData["data"]["minute"].asString().c_str());
	tmpAdd.second = atoi(jsonData["data"]["second"].asString().c_str());

	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::OnlySendCmdToHard(Json::Value jsonData, char *sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons(0);
	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	int nSendSize = sizeof(thead);
	return nSendSize;
}

int CCrossDataManager::AddOrSetControllerModule(Json::Value jsonData, char *sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_REQ_PACKETDATA_ADD_MODULE_T);
	//	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_HEAD_T) + sizeof(BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T));
		// 数据结构体
	BEGP_REQ_PACKETDATA_ADD_MODULE_T tmpAdd;
	tmpAdd.ModelID = atoi(jsonData["data"]["ModelID"].asString().c_str());
	tmpAdd.FlnID = atoi(jsonData["data"]["FlnID"].asString().c_str());
	tmpAdd.SlaveAddress = atoi(jsonData["data"]["SlaveAddress"].asString().c_str());
	tmpAdd.Active = atoi(jsonData["data"]["Active"].asString().c_str());
	tmpAdd.ID = atoi(jsonData["data"]["ID"].asString().c_str());

	strcpy_s(tmpAdd.Name, jsonData["data"]["Name"].asString().c_str());
	strcpy_s(tmpAdd.Alias, jsonData["data"]["Alias"].asString().c_str());
	strcpy_s(tmpAdd.Description, jsonData["data"]["Description"].asString().c_str());
	strcpy_s(tmpAdd.Location, jsonData["data"]["Location"].asString().c_str());

	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::DeleteControllerModule(Json::Value jsonData, char *sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_REQ_PACKETDATA_DELETE_MODULE_T);
	BEGP_REQ_PACKETDATA_DELETE_MODULE_T tmpAdd;
	tmpAdd.ID = BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));

	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::GetControllerModuleData(Json::Value jsonData, char *sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_REQ_PACKETDATA_DELETE_MODULE_T);
	BEGP_REQ_PACKETDATA_DELETE_MODULE_T tmpAdd;
	tmpAdd.ID = BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));

	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::GetControllerModuleErrorCode(Json::Value jsonData, char *sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_REQ_PACKETDATA_DELETE_MODULE_T);
	BEGP_REQ_PACKETDATA_DELETE_MODULE_T tmpAdd;
	tmpAdd.ID = BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));

	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::AddOrSetPoint(Json::Value jsonData, char *sendbuf)
{
	int nType = atoi(jsonData["PointType"].asString().c_str());
	int nSendSize = 0;
	switch (nType)
	{
	case POINT_TYPE_LAI :   //实点AI
		nSendSize = AddPointLAI(jsonData, sendbuf);
		break;
	case POINT_TYPE_LAO :   //实点AO
		nSendSize = AddPointLAO(jsonData, sendbuf);
		break;
	case POINT_TYPE_LDI :  //实点DI
		nSendSize = AddPointLDI(jsonData, sendbuf);
		break;
	case POINT_TYPE_LDO :  //实点DI
		nSendSize = AddPointLDO(jsonData, sendbuf);
		break;
	case POINT_TYPE_VIRTUAL_AI://虚点AI类型
		nSendSize = AddPointVAI(jsonData, sendbuf);
		break;
	case POINT_TYPE_VIRTUAL_AO://虚点AO类型
		nSendSize = AddPointVAO(jsonData, sendbuf);
		break;
	case POINT_TYPE_VIRTUAL_DI://虚点DI类型
		nSendSize = AddPointVDI(jsonData, sendbuf);
		break;
	case POINT_TYPE_VIRTUAL_DO://虚点DO类型
		nSendSize = AddPointVDO(jsonData, sendbuf);
		break;
	default:
		break;
	}
	return nSendSize;
}

int CCrossDataManager::AddPointLAI(Json::Value jsonData, char * sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_LOGICAL_POINT_PARAMETER_LAI_T);
	BEGP_LOGICAL_POINT_PARAMETER_LAI_T tmpAdd;
	tmpAdd.ID = BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));
	tmpAdd.PointType = atoi(jsonData["data"]["PointType"].asString().c_str());
	tmpAdd.Active = atoi(jsonData["data"]["Active"].asString().c_str());

	strcpy_s(tmpAdd.Name, jsonData["data"]["Name"].asString().c_str());
	strcpy_s(tmpAdd.Alias, jsonData["data"]["Alias"].asString().c_str());
	strcpy_s(tmpAdd.Description, jsonData["data"]["Description"].asString().c_str());

	tmpAdd.ModuleID = BES200_htons(atoi(jsonData["data"]["ModuleID"].asString().c_str()));
	tmpAdd.ChannelIndex = atoi(jsonData["data"]["ChannelIndex"].asString().c_str());
	tmpAdd.WorkMode = atoi(jsonData["data"]["WorkMode"].asString().c_str());
	tmpAdd.Polarity = atoi(jsonData["data"]["Polarity"].asString().c_str());
	tmpAdd.InitValue = m_comFun.BES200_htonl(atoi(jsonData["data"]["InitValue"].asString().c_str()));

	tmpAdd.AlarmActive = atoi(jsonData["data"]["AlarmActive"].asString().c_str());
	tmpAdd.AlarmType = atoi(jsonData["data"]["AlarmType"].asString().c_str());
	tmpAdd.AlarmPriority = atoi(jsonData["data"]["AlarmPriority"].asString().c_str());
	tmpAdd.LineType = atoi(jsonData["data"]["LineType"].asString().c_str());

	strcpy_s(tmpAdd.Unit, jsonData["data"]["Unit"].asString().c_str());


	tmpAdd.HighRange = m_comFun.BES200_htonl(atoi(jsonData["data"]["HighRange"].asString().c_str()));
	tmpAdd.LowRange = m_comFun.BES200_htonl(atoi(jsonData["data"]["LowRange"].asString().c_str()));
	tmpAdd.Precision = atoi(jsonData["data"]["Precision"].asString().c_str());
	tmpAdd.AlarmHighValue = m_comFun.BES200_htonl( atoi(jsonData["data"]["AlarmHighValue"].asString().c_str()));
	tmpAdd.AlarmLowValue = m_comFun.BES200_htonl(atoi(jsonData["data"]["AlarmLowValue"].asString().c_str()));

	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::AddPointLAO(Json::Value jsonData, char *sendbuf)
{
	return AddPointLAI(jsonData, sendbuf);
}

int CCrossDataManager::AddPointLDI(Json::Value jsonData, char * sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_LOGICAL_POINT_PARAMETER_LDI_T);
	BEGP_LOGICAL_POINT_PARAMETER_LDI_T tmpAdd;
	tmpAdd.ID = BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));
	tmpAdd.PointType = atoi(jsonData["data"]["PointType"].asString().c_str());
	tmpAdd.Active = atoi(jsonData["data"]["Active"].asString().c_str());

	strcpy_s(tmpAdd.Name, jsonData["data"]["Name"].asString().c_str());
	strcpy_s(tmpAdd.Alias, jsonData["data"]["Alias"].asString().c_str());
	strcpy_s(tmpAdd.Description, jsonData["data"]["Description"].asString().c_str());

	tmpAdd.ModuleID = m_comFun.BES200_htons(atoi(jsonData["data"]["ModuleID"].asString().c_str()));
	tmpAdd.ChannelIndex = atoi(jsonData["data"]["ChannelIndex"].asString().c_str());
	tmpAdd.WorkMode = atoi(jsonData["data"]["WorkMode"].asString().c_str());
	tmpAdd.Polarity = atoi(jsonData["data"]["Polarity"].asString().c_str());
	tmpAdd.InitValue = m_comFun.BES200_htonl (atoi(jsonData["data"]["InitValue"].asString().c_str()));

	tmpAdd.AlarmActive = atoi(jsonData["data"]["AlarmActive"].asString().c_str());
	tmpAdd.AlarmType = atoi(jsonData["data"]["AlarmType"].asString().c_str());
	tmpAdd.AlarmPriority = atoi(jsonData["data"]["AlarmPriority"].asString().c_str());
	tmpAdd.AlarmTrigger = atoi(jsonData["data"]["AlarmTrigger"].asString().c_str());
	tmpAdd.ActivePassive = atoi(jsonData["data"]["ActivePassive"].asString().c_str());

	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::AddPointLDO(Json::Value jsonData, char *sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_LOGICAL_POINT_PARAMETER_LDO_T);
	BEGP_LOGICAL_POINT_PARAMETER_LDO_T tmpAdd;
	tmpAdd.ID = BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));
	tmpAdd.PointType = atoi(jsonData["data"]["PointType"].asString().c_str());
	tmpAdd.Active = atoi(jsonData["data"]["Active"].asString().c_str());

	strcpy_s(tmpAdd.Name, jsonData["data"]["Name"].asString().c_str());
	strcpy_s(tmpAdd.Alias, jsonData["data"]["Alias"].asString().c_str());
	strcpy_s(tmpAdd.Description, jsonData["data"]["Description"].asString().c_str());

	tmpAdd.ModuleID = m_comFun.BES200_htons( atoi(jsonData["data"]["ModuleID"].asString().c_str()));
	tmpAdd.ChannelIndex = atoi(jsonData["data"]["ChannelIndex"].asString().c_str());
	tmpAdd.WorkMode = atoi(jsonData["data"]["WorkMode"].asString().c_str());
	tmpAdd.Polarity = atoi(jsonData["data"]["Polarity"].asString().c_str());
	tmpAdd.InitValue = m_comFun.BES200_htonl( atoi(jsonData["data"]["InitValue"].asString().c_str()));

	tmpAdd.AlarmActive = atoi(jsonData["data"]["AlarmActive"].asString().c_str());
	tmpAdd.AlarmType = atoi(jsonData["data"]["AlarmType"].asString().c_str());
	tmpAdd.AlarmPriority = atoi(jsonData["data"]["AlarmPriority"].asString().c_str());
	tmpAdd.AlarmTrigger = atoi(jsonData["data"]["AlarmTrigger"].asString().c_str());
	
	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::AddPointVAI(Json::Value jsonData, char *sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_LOGICAL_POINT_PARAMETER_VAI_T);
	BEGP_LOGICAL_POINT_PARAMETER_VAI_T tmpAdd;
	tmpAdd.ID = BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));
	tmpAdd.PointType = atoi(jsonData["data"]["PointType"].asString().c_str());
	tmpAdd.Active = atoi(jsonData["data"]["Active"].asString().c_str());

	strcpy_s(tmpAdd.Name, jsonData["data"]["Name"].asString().c_str());
	strcpy_s(tmpAdd.Alias, jsonData["data"]["Alias"].asString().c_str());
	strcpy_s(tmpAdd.Description, jsonData["data"]["Description"].asString().c_str());

	tmpAdd.InitValue = m_comFun.BES200_htonl(atoi(jsonData["data"]["InitValue"].asString().c_str()));

	tmpAdd.AlarmActive = atoi(jsonData["data"]["AlarmActive"].asString().c_str());
	tmpAdd.AlarmType = atoi(jsonData["data"]["AlarmType"].asString().c_str());
	tmpAdd.AlarmPriority = atoi(jsonData["data"]["AlarmPriority"].asString().c_str());

	strcpy_s(tmpAdd.Unit, jsonData["data"]["Unit"].asString().c_str());

	tmpAdd.Precision = atoi(jsonData["data"]["Precision"].asString().c_str());
	tmpAdd.AlarmHighValue = m_comFun.BES200_htonl(atoi(jsonData["data"]["AlarmHighValue"].asString().c_str()));
	tmpAdd.AlarmLowValue = m_comFun.BES200_htonl(atoi(jsonData["data"]["AlarmLowValue"].asString().c_str()));

	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::AddPointVAO(Json::Value jsonData, char *sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_LOGICAL_POINT_PARAMETER_VAO_T);
	BEGP_LOGICAL_POINT_PARAMETER_VAO_T tmpAdd;
	tmpAdd.ID = BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));
	tmpAdd.PointType = atoi(jsonData["data"]["PointType"].asString().c_str());
	tmpAdd.Active = atoi(jsonData["data"]["Active"].asString().c_str());

	strcpy_s(tmpAdd.Name, jsonData["data"]["Name"].asString().c_str());
	strcpy_s(tmpAdd.Alias, jsonData["data"]["Alias"].asString().c_str());
	strcpy_s(tmpAdd.Description, jsonData["data"]["Description"].asString().c_str());
	tmpAdd.InitValue = m_comFun.BES200_htonl(atoi(jsonData["data"]["InitValue"].asString().c_str()));

	tmpAdd.AlarmActive = atoi(jsonData["data"]["AlarmActive"].asString().c_str());
	tmpAdd.AlarmType = atoi(jsonData["data"]["AlarmType"].asString().c_str());
	tmpAdd.AlarmPriority = atoi(jsonData["data"]["AlarmPriority"].asString().c_str());
	strcpy_s(tmpAdd.Unit, jsonData["data"]["Unit"].asString().c_str());

	tmpAdd.Precision = atoi(jsonData["data"]["Precision"].asString().c_str());
	tmpAdd.AlarmHighValue = m_comFun.BES200_htonl(atoi(jsonData["data"]["AlarmHighValue"].asString().c_str()));
	tmpAdd.AlarmLowValue = m_comFun.BES200_htonl(atoi(jsonData["data"]["AlarmLowValue"].asString().c_str()));

	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::AddPointVDI(Json::Value jsonData, char *sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_LOGICAL_POINT_PARAMETER_VDI_T);
	BEGP_LOGICAL_POINT_PARAMETER_VDI_T tmpAdd;
	tmpAdd.ID = BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));
	tmpAdd.PointType = atoi(jsonData["data"]["PointType"].asString().c_str());
	tmpAdd.Active = atoi(jsonData["data"]["Active"].asString().c_str());

	strcpy_s(tmpAdd.Name, jsonData["data"]["Name"].asString().c_str());
	strcpy_s(tmpAdd.Alias, jsonData["data"]["Alias"].asString().c_str());
	strcpy_s(tmpAdd.Description, jsonData["data"]["Description"].asString().c_str());
	tmpAdd.InitValue = m_comFun.BES200_htonl(atoi(jsonData["data"]["InitValue"].asString().c_str()));

	tmpAdd.AlarmActive = atoi(jsonData["data"]["AlarmActive"].asString().c_str());
	tmpAdd.AlarmType = atoi(jsonData["data"]["AlarmType"].asString().c_str());
	tmpAdd.AlarmPriority = atoi(jsonData["data"]["AlarmPriority"].asString().c_str());

	tmpAdd.AlarmTrigger = atoi(jsonData["data"]["AlarmTrigger"].asString().c_str());

	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::AddPointVDO(Json::Value jsonData, char *sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_LOGICAL_POINT_PARAMETER_VDO_T);
	BEGP_LOGICAL_POINT_PARAMETER_VDO_T tmpAdd;
	tmpAdd.ID = BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));
	tmpAdd.PointType = atoi(jsonData["data"]["PointType"].asString().c_str());
	tmpAdd.Active = atoi(jsonData["data"]["Active"].asString().c_str());

	strcpy_s(tmpAdd.Name, jsonData["data"]["Name"].asString().c_str());
	strcpy_s(tmpAdd.Alias, jsonData["data"]["Alias"].asString().c_str());
	strcpy_s(tmpAdd.Description, jsonData["data"]["Description"].asString().c_str());
	tmpAdd.InitValue = m_comFun.BES200_htonl(atoi(jsonData["data"]["InitValue"].asString().c_str()));

	tmpAdd.AlarmActive = atoi(jsonData["data"]["AlarmActive"].asString().c_str());
	tmpAdd.AlarmType = atoi(jsonData["data"]["AlarmType"].asString().c_str());
	tmpAdd.AlarmPriority = atoi(jsonData["data"]["AlarmPriority"].asString().c_str());

	tmpAdd.AlarmTrigger = atoi(jsonData["data"]["AlarmTrigger"].asString().c_str());

	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::SetPointValue(Json::Value jsonData, char *sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_T);
	BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_T tmpAdd;
	tmpAdd.ID = BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));
	tmpAdd.WorkMode = atoi(jsonData["data"]["WorkMode"].asString().c_str());
	tmpAdd.Value = m_comFun.BES200_htonl( atoi(jsonData["data"]["Value"].asString().c_str()));
	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::SetPointValueByName(Json::Value jsonData, char *sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_BY_NAME_T);
	BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_BY_NAME_T tmpAdd;
	strcpy_s(tmpAdd.Name, jsonData["data"]["Name"].asString().c_str());
	tmpAdd.Value = m_comFun.BES200_htonl(atoi(jsonData["data"]["Value"].asString().c_str()));
	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::DeletePointByID(Json::Value jsonData, char *sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_REQ_PACKETDATA_DELETE_LOGICAL_POINT_T);
	BEGP_REQ_PACKETDATA_DELETE_LOGICAL_POINT_T tmpAdd;
	tmpAdd.ID = m_comFun.BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));
	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::GetPointParameter(Json::Value jsonData, char * sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_T);
	BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_T tmpAdd;
	tmpAdd.ID = m_comFun.BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));
	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::GetPointErrorCode(Json::Value jsonData, char * sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T);
	BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T tmpAdd;
	tmpAdd.ID = m_comFun.BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));
	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::GetPointAlarmErrorCode(Json::Value jsonData, char *sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T);
	BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T tmpAdd;
	tmpAdd.ID = m_comFun.BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));
	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::AddScheduler(Json::Value jsonData, char * sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_REQ_PACKETDATA_ADD_SCHEDULER_T);
	BEGP_REQ_PACKETDATA_ADD_SCHEDULER_T tmpAdd;
	tmpAdd.ID = m_comFun.BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));
	tmpAdd.SceneID = m_comFun.BES200_htons(atoi(jsonData["data"]["SceneID"].asString().c_str()));

	tmpAdd.Active = atoi(jsonData["data"]["Active"].asString().c_str());
	strcpy_s(tmpAdd.Name, jsonData["data"]["Name"].asString().c_str());
	strcpy_s(tmpAdd.Alias, jsonData["data"]["Alias"].asString().c_str());

	tmpAdd.PlanType = atoi(jsonData["data"]["PlanType"].asString().c_str());
	tmpAdd.StartDateYear = atoi(jsonData["data"]["StartDateYear"].asString().c_str());
	tmpAdd.StartDateMonth = atoi(jsonData["data"]["StartDateMonth"].asString().c_str());
	tmpAdd.StartDateDay = atoi(jsonData["data"]["StartDateDay"].asString().c_str());
	tmpAdd.EndDateYear = atoi(jsonData["data"]["EndDateYear"].asString().c_str());
	tmpAdd.EndDateMonth = atoi(jsonData["data"]["EndDateMonth"].asString().c_str());
	tmpAdd.EndDateDay = atoi(jsonData["data"]["EndDateDay"].asString().c_str());
	
	tmpAdd.StartTimeHour = atoi(jsonData["data"]["StartTimeHour"].asString().c_str());
	tmpAdd.StartTimeMinute = atoi(jsonData["data"]["StartTimeMinute"].asString().c_str());
	tmpAdd.StartTimeSecond = atoi(jsonData["data"]["StartTimeSecond"].asString().c_str());
	tmpAdd.EndTimeHour = atoi(jsonData["data"]["EndTimeHour"].asString().c_str());
	tmpAdd.EndTimeMinute = atoi(jsonData["data"]["EndTimeMinute"].asString().c_str());
	tmpAdd.EndTimeSecond = atoi(jsonData["data"]["EndTimeSecond"].asString().c_str());
	
	tmpAdd.ExecutionWay = atoi(jsonData["data"]["ExecutionWay"].asString().c_str());
	tmpAdd.WeekMask = atoi(jsonData["data"]["WeekMask"].asString().c_str());
	tmpAdd.SceneType = atoi(jsonData["data"]["SceneType"].asString().c_str());
	tmpAdd.SceneID = atoi(jsonData["data"]["SceneID"].asString().c_str());
	tmpAdd.ModeID = atoi(jsonData["data"]["ModeID"].asString().c_str());
	tmpAdd.DefaultModeID = atoi(jsonData["data"]["DefaultModeID"].asString().c_str());

	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::SetScheduler(Json::Value jsonData, char * sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_REQ_PACKETDATA_SET_SCHEDULER_PARAMETER_T);
	BEGP_REQ_PACKETDATA_SET_SCHEDULER_PARAMETER_T tmpAdd;
	tmpAdd.ID = m_comFun.BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));
	tmpAdd.SceneID = m_comFun.BES200_htons(atoi(jsonData["data"]["SceneID"].asString().c_str()));

	tmpAdd.Active = atoi(jsonData["data"]["Active"].asString().c_str());
	strcpy_s(tmpAdd.Name, jsonData["data"]["Name"].asString().c_str());
	strcpy_s(tmpAdd.Alias, jsonData["data"]["Alias"].asString().c_str());

	tmpAdd.PlanType = atoi(jsonData["data"]["PlanType"].asString().c_str());
	tmpAdd.StartDateYear = atoi(jsonData["data"]["StartDateYear"].asString().c_str());
	tmpAdd.StartDateMonth = atoi(jsonData["data"]["StartDateMonth"].asString().c_str());
	tmpAdd.StartDateDay = atoi(jsonData["data"]["StartDateDay"].asString().c_str());
	tmpAdd.EndDateYear = atoi(jsonData["data"]["EndDateYear"].asString().c_str());
	tmpAdd.EndDateMonth = atoi(jsonData["data"]["EndDateMonth"].asString().c_str());
	tmpAdd.EndDateDay = atoi(jsonData["data"]["EndDateDay"].asString().c_str());

	tmpAdd.StartTimeHour = atoi(jsonData["data"]["StartTimeHour"].asString().c_str());
	tmpAdd.StartTimeMinute = atoi(jsonData["data"]["StartTimeMinute"].asString().c_str());
	tmpAdd.StartTimeSecond = atoi(jsonData["data"]["StartTimeSecond"].asString().c_str());
	tmpAdd.EndTimeHour = atoi(jsonData["data"]["EndTimeHour"].asString().c_str());
	tmpAdd.EndTimeMinute = atoi(jsonData["data"]["EndTimeMinute"].asString().c_str());
	tmpAdd.EndTimeSecond = atoi(jsonData["data"]["EndTimeSecond"].asString().c_str());

	tmpAdd.ExecutionWay = atoi(jsonData["data"]["ExecutionWay"].asString().c_str());
	tmpAdd.WeekMask = atoi(jsonData["data"]["WeekMask"].asString().c_str());
	tmpAdd.SceneType = atoi(jsonData["data"]["SceneType"].asString().c_str());
	tmpAdd.SceneID = atoi(jsonData["data"]["SceneID"].asString().c_str());
	tmpAdd.ModeID = atoi(jsonData["data"]["ModeID"].asString().c_str());
	tmpAdd.DefaultModeID = atoi(jsonData["data"]["DefaultModeID"].asString().c_str());


	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::DelScheduler(Json::Value jsonData, char * sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_REQ_PACKETDATA_DELETE_SCHEDULER_T);
	BEGP_REQ_PACKETDATA_DELETE_SCHEDULER_T tmpAdd;
	tmpAdd.ID = m_comFun.BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));

	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::GetScheduler(Json::Value jsonData, char * sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_REQ_PACKETDATA_GET_SCHEDULER_PARAMETER_T);
	BEGP_REQ_PACKETDATA_GET_SCHEDULER_PARAMETER_T tmpAdd;
	tmpAdd.ID = m_comFun.BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));

	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::AddScene(Json::Value jsonData, char * sendbuf)
{
	//两种场景：采集场景、控制场景
	// 控制场景
	// 数据格式头
	int nSendSize = 0;
	BEGP_REQ_HEAD_T *pthead = (BEGP_REQ_HEAD_T *) sendbuf;
	pthead->FunctionCode = atoi(jsonData["index"].asString().c_str());
	pthead->SequenceNumber = 0;
	pthead->SlaveAddress = 1;

	int nSceneType = atoi(jsonData["data"]["SceneType"].asString().c_str());
	if (nSceneType == 0 )//控制场景
	{
		BEGP_CONTROL_SCENE_PARAMETER_T *lp = (BEGP_CONTROL_SCENE_PARAMETER_T *)(sendbuf + sizeof(BEGP_REQ_HEAD_T));
		pthead->DataSize = sizeof(BEGP_REQ_HEAD_T) + sizeof(BEGP_CONTROL_SCENE_PARAMETER_T);
		nSendSize = sizeof(BEGP_REQ_HEAD_T) + sizeof(BEGP_CONTROL_SCENE_PARAMETER_T);
		lp->SceneType = atoi(jsonData["data"]["data"]["SceneType"].asString().c_str());
		lp->ID = atoi(jsonData["data"]["data"]["ID"].asString().c_str());
		lp->Active = atoi(jsonData["data"]["data"]["Active"].asString().c_str());
		strcpy_s(lp->Name, jsonData["data"]["data"]["Name"].asString().c_str());
		strcpy_s(lp->Alias, jsonData["data"]["data"]["Alias"].asString().c_str());
		int nSizeCollectMode = 0;
		if (jsonData["data"]["data"]["CollectMode"].isArray())
		{
			nSizeCollectMode = jsonData["data"]["data"]["CollectMode"].size();
		}
		for (int i = 0; i< nSizeCollectMode;i++)
		{
			Json::Value vaMode = jsonData["data"]["data"]["CollectMode"];
			Json::Value vaModeItem = vaMode[i];
			//vaMode.get(i, vaModeItem);

			lp->ControlMode[i].Active =atoi(vaModeItem["Active"].asString().c_str());
			lp->ControlMode[i].ID = BES200_htons(atoi(vaModeItem["ID"].asString().c_str()));
			strcpy_s(lp->ControlMode[i].Name ,vaModeItem["Name"].asString().c_str());

			int nSizeCollectPoint = 0;
			if (vaModeItem["CollectPoint"].isArray())
			{
				nSizeCollectPoint = vaModeItem["CollectPoint"].size();
				Json::Value vaPoint = vaModeItem["CollectPoint"];
				for (int j = 0;j < nSizeCollectPoint;j++)
				{
					Json::Value vaPointItem = vaPoint[j]; 
					lp->ControlMode[i].ControlPoint[j].Active = atoi(vaPointItem["Active"].asString().c_str());
					lp->ControlMode[i].ControlPoint[j].PointID = BES200_htons(atoi(vaPointItem["PointID"].asString().c_str()));
					lp->ControlMode[i].ControlPoint[j].RunValue = m_comFun.BES200_htonl( atoi(vaPointItem["RunValue"].asString().c_str()));
				}
			}
		}
		

	}
	else if (nSceneType == 1)// 采集场景
	{
		BEGP_COLLECT_SCENE_PARAMETER_T *lp = (BEGP_COLLECT_SCENE_PARAMETER_T *)(sendbuf + sizeof(BEGP_REQ_HEAD_T));
		pthead->DataSize = sizeof(BEGP_REQ_HEAD_T)+ sizeof(BEGP_COLLECT_SCENE_PARAMETER_T);
		nSendSize = sizeof(BEGP_REQ_HEAD_T) + sizeof(BEGP_COLLECT_SCENE_PARAMETER_T);
		lp->SceneType = atoi(jsonData["data"]["data"]["SceneType"].asString().c_str());
		lp->ID = atoi(jsonData["data"]["data"]["ID"].asString().c_str());
		lp->Active = atoi(jsonData["data"]["data"]["Active"].asString().c_str());
		strcpy_s(lp->Name, jsonData["data"]["data"]["Name"].asString().c_str());
		strcpy_s(lp->Alias, jsonData["data"]["data"]["Alias"].asString().c_str());
		int nSizeCollectMode = 0;
		if (jsonData["data"]["data"]["CollectMode"].isArray())
		{
			nSizeCollectMode = jsonData["data"]["data"]["CollectMode"].size();
		}
		for (int i = 0; i < nSizeCollectMode;i++)
		{
			Json::Value vaMode = jsonData["data"]["data"]["CollectMode"];
			Json::Value vaModeItem;
			vaModeItem = vaMode[i];
			lp->CollectMode[i].Active = atoi(vaModeItem["Active"].asString().c_str());
			lp->CollectMode[i].ID = BES200_htons(atoi(vaModeItem["ID"].asString().c_str()));
			strcpy_s(lp->CollectMode[i].Name, vaModeItem["Active"].asString().c_str());
			int nSizeCollectPoint = 0;
			if (vaModeItem["CollectPoint"].isArray())
			{
				nSizeCollectPoint = vaModeItem["CollectPoint"].size();
				Json::Value vaPoint = vaModeItem["CollectPoint"];
				for (int j = 0;j < nSizeCollectPoint;j++)
				{
					Json::Value vaPointItem;
					vaPointItem = vaPoint[j];
					lp->CollectMode[i].CollectPoint[j].Active = atoi(vaPointItem["Active"].asString().c_str());
					lp->CollectMode[i].CollectPoint[j].CollectID = atoi(vaPointItem["CollectID"].asString().c_str());
					lp->CollectMode[i].CollectPoint[j].TimeUnit = atoi(vaPointItem["TimeUnit"].asString().c_str());
					lp->CollectMode[i].CollectPoint[j].PointID = BES200_htons(atoi(vaPointItem["PointID"].asString().c_str()));
					lp->CollectMode[i].CollectPoint[j].Value = m_comFun.BES200_htonl(atoi(vaPointItem["Value"].asString().c_str()));
				}
			}

		}
	}
	return nSendSize;
}

int CCrossDataManager::SetScene(Json::Value jsonData, char * sendbuf)
{

	return AddScene(jsonData, sendbuf);
}

int CCrossDataManager::DelScene(Json::Value jsonData, char * sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_REQ_PACKETDATA_DELETE_SCENE_T);
	BEGP_REQ_PACKETDATA_DELETE_SCENE_T tmpAdd;
	tmpAdd.ID = m_comFun.BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));

	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::GetScene(Json::Value jsonData, char * sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = get_data_size(BEGP_REQ_HEAD_T, BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_T);
	BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_T tmpAdd;
	tmpAdd.ID = m_comFun.BES200_htons(atoi(jsonData["data"]["ID"].asString().c_str()));

	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));
	// 数据
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::UpDate(Json::Value jsonData, char* sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = atoi(jsonData["index"].asString().c_str());
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons(0);	
	// 协议头
	memcpy(sendbuf, &thead, sizeof(thead));

	int nSendSize = sizeof(thead);
	return nSendSize;
}

Json::Value CCrossDataManager::ParseRecvBuf(void *buff)
{
	return "";
}

Json::Value CCrossDataManager::ParseGetTime(void *pRecvData)
{
	Json::Value jsonResultData;
	BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_T *lpVData = (BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_T*)pRecvData;
	jsonResultData["year"] = Json::Value(lpVData->year + 2000);
	jsonResultData["month"] = Json::Value(lpVData->month);
	jsonResultData["day"] = Json::Value(lpVData->day);
	int nWDay = 0;
	nWDay = lpVData->wday;
	if (nWDay == 7)
	{
		nWDay = 0;
	}
	jsonResultData["wday"] = Json::Value(nWDay);
	jsonResultData["hour"] = Json::Value(lpVData->hour);
	jsonResultData["minute"] = Json::Value(lpVData->minute);
	jsonResultData["second"] = Json::Value(lpVData->second);
	return jsonResultData;
}

Json::Value CCrossDataManager::ParseGetControlPar(void *pRecvData)
{
	Json::Value jsonResultData;
	BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_T *lpVData = (BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_T*)pRecvData;
	jsonResultData["Name"] = Json::Value(lpVData->Name);
	jsonResultData["Alias"] = Json::Value(lpVData->Alias);
	jsonResultData["Description"] = Json::Value(lpVData->Description);
	jsonResultData["Location"] = Json::Value(lpVData->Location);
	jsonResultData["Zone"] = Json::Value(lpVData->Zone);
	jsonResultData["ID"] = Json::Value(lpVData->ID);
	jsonResultData["Active"] = Json::Value(lpVData->Active);
	return jsonResultData;
}

Json::Value CCrossDataManager::ParseGetModePar(void *pRecvData)
{
	Json::Value jsonResultData;
	BEGP_RSP_PACKETDATA_GET_MODULE_PARAMETER_T *lpVData = (BEGP_RSP_PACKETDATA_GET_MODULE_PARAMETER_T*)pRecvData;
	jsonResultData["Name"] = Json::Value(lpVData->Name);
	jsonResultData["Alias"] = Json::Value(lpVData->Alias);
	jsonResultData["Description"] = Json::Value(lpVData->Description);
	jsonResultData["Location"] = Json::Value(lpVData->Location);
	jsonResultData["ModelID"] = Json::Value(lpVData->ModelID);
	jsonResultData["FlnID"] = Json::Value(lpVData->FlnID);
	jsonResultData["SlaveAddress"] = Json::Value(lpVData->SlaveAddress);
	jsonResultData["Active"] = Json::Value(lpVData->Active);
	jsonResultData["ID"] = Json::Value(lpVData->ID);
	return jsonResultData;
}

Json::Value CCrossDataManager::ParseGetPointPar(void *pRecvData)
{
	Json::Value jsonResultData;
	BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_T *lpVData = (BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_T *)pRecvData;

	if (lpVData->PointType == POINT_TYPE_LAI)
	{
		BEGP_LOGICAL_POINT_PARAMETER_LAI_T *lpD = (BEGP_LOGICAL_POINT_PARAMETER_LAI_T*)lpVData;

		jsonResultData["PointType"] = Json::Value(lpD->PointType);
		jsonResultData["ID"] = Json::Value(BES200_htons(lpD->ID));
		jsonResultData["Active"] = Json::Value(lpD->Active);
		jsonResultData["Name"] = Json::Value(lpD->Name);
		jsonResultData["Alias"] = Json::Value(lpD->Alias);
		jsonResultData["Description"] = Json::Value(lpD->Description);
		jsonResultData["ModuleID"] = Json::Value(BES200_htons(lpD->ModuleID));
		jsonResultData["ChannelIndex"] = Json::Value(lpD->ChannelIndex);
		jsonResultData["WorkMode"] = Json::Value(lpD->WorkMode);
		jsonResultData["Polarity"] = Json::Value(lpD->Polarity);
		jsonResultData["InitValue"] = Json::Value(m_comFun.BES200_htonl(lpD->InitValue));
		jsonResultData["AlarmActive"] = Json::Value(lpD->AlarmActive);
		jsonResultData["AlarmType"] = Json::Value(lpD->AlarmType);
		jsonResultData["AlarmPriority"] = Json::Value(lpD->AlarmPriority);
		jsonResultData["LineType"] = Json::Value(lpD->LineType);
		jsonResultData["Unit"] = Json::Value(lpD->Unit);
		jsonResultData["HighRange"] = Json::Value(m_comFun.BES200_htonl(lpD->HighRange));
		jsonResultData["LowRange"] = Json::Value(m_comFun.BES200_htonl(lpD->LowRange));
		jsonResultData["Precision"] = Json::Value(lpD->Precision);
		jsonResultData["AlarmHighValue"] = Json::Value(m_comFun.BES200_htonl(lpD->AlarmHighValue));
		jsonResultData["AlarmLowValue"] = Json::Value(m_comFun.BES200_htonl(lpD->AlarmLowValue));

	}
	else if (lpVData->PointType == POINT_TYPE_LAO) {
		BEGP_LOGICAL_POINT_PARAMETER_LAO_T *lpD = (BEGP_LOGICAL_POINT_PARAMETER_LAO_T*)lpVData;

		jsonResultData["PointType"] = Json::Value(lpD->PointType);
		jsonResultData["ID"] = Json::Value(BES200_htons(lpD->ID));
		jsonResultData["Active"] = Json::Value(lpD->Active);
		jsonResultData["Name"] = Json::Value(lpD->Name);
		jsonResultData["Alias"] = Json::Value(lpD->Alias);
		jsonResultData["Description"] = Json::Value(lpD->Description);
		jsonResultData["ModuleID"] = Json::Value(BES200_htons(lpD->ModuleID));
		jsonResultData["ChannelIndex"] = Json::Value(lpD->ChannelIndex);
		jsonResultData["WorkMode"] = Json::Value(lpD->WorkMode);
		jsonResultData["Polarity"] = Json::Value(lpD->Polarity);
		jsonResultData["InitValue"] = Json::Value(m_comFun.BES200_htonl(lpD->InitValue));
		jsonResultData["AlarmActive"] = Json::Value(lpD->AlarmActive);
		jsonResultData["AlarmType"] = Json::Value(lpD->AlarmType);
		jsonResultData["AlarmPriority"] = Json::Value(lpD->AlarmPriority);
		jsonResultData["LineType"] = Json::Value(lpD->LineType);
		jsonResultData["Unit"] = Json::Value(lpD->Unit);
		jsonResultData["HighRange"] = Json::Value(m_comFun.BES200_htonl(lpD->HighRange));
		jsonResultData["LowRange"] = Json::Value(m_comFun.BES200_htonl(lpD->LowRange));
		jsonResultData["Precision"] = Json::Value(lpD->Precision);
		jsonResultData["AlarmHighValue"] = Json::Value(m_comFun.BES200_htonl(lpD->AlarmHighValue));
		jsonResultData["AlarmLowValue"] = Json::Value(m_comFun.BES200_htonl(lpD->AlarmLowValue));

	}
	else if (lpVData->PointType == POINT_TYPE_LDI) {
		BEGP_LOGICAL_POINT_PARAMETER_LDI_T *lpD = (BEGP_LOGICAL_POINT_PARAMETER_LDI_T*)lpVData;

		jsonResultData["PointType"] = Json::Value(lpD->PointType);
		jsonResultData["ID"] = Json::Value(BES200_htons(lpD->ID));
		jsonResultData["Active"] = Json::Value(lpD->Active);
		jsonResultData["Name"] = Json::Value(lpD->Name);
		jsonResultData["Alias"] = Json::Value(lpD->Alias);
		jsonResultData["Description"] = Json::Value(lpD->Description);
		jsonResultData["ModuleID"] = Json::Value(BES200_htons(lpD->ModuleID));
		jsonResultData["ChannelIndex"] = Json::Value(lpD->ChannelIndex);
		jsonResultData["WorkMode"] = Json::Value(lpD->WorkMode);
		jsonResultData["Polarity"] = Json::Value(lpD->Polarity);
		jsonResultData["InitValue"] = Json::Value(m_comFun.BES200_htonl(lpD->InitValue));
		jsonResultData["AlarmActive"] = Json::Value(lpD->AlarmActive);
		jsonResultData["AlarmType"] = Json::Value(lpD->AlarmType);
		jsonResultData["AlarmPriority"] = Json::Value(lpD->AlarmPriority);

		jsonResultData["AlarmTrigger"] = Json::Value(lpD->AlarmTrigger);
		jsonResultData["ActivePassive"] = Json::Value(lpD->ActivePassive);
	}
	else if (lpVData->PointType == POINT_TYPE_LDO) {
		BEGP_LOGICAL_POINT_PARAMETER_LDO_T *lpD = (BEGP_LOGICAL_POINT_PARAMETER_LDO_T*)lpVData;

		jsonResultData["PointType"] = Json::Value(lpD->PointType);
		jsonResultData["ID"] = Json::Value(BES200_htons(lpD->ID));
		jsonResultData["Active"] = Json::Value(lpD->Active);
		jsonResultData["Name"] = Json::Value(lpD->Name);
		jsonResultData["Alias"] = Json::Value(lpD->Alias);
		jsonResultData["Description"] = Json::Value(lpD->Description);
		jsonResultData["ModuleID"] = Json::Value(BES200_htons(lpD->ModuleID));
		jsonResultData["ChannelIndex"] = Json::Value(lpD->ChannelIndex);
		jsonResultData["WorkMode"] = Json::Value(lpD->WorkMode);
		jsonResultData["Polarity"] = Json::Value(lpD->Polarity);
		jsonResultData["InitValue"] = Json::Value(m_comFun.BES200_htonl(lpD->InitValue));
		jsonResultData["AlarmActive"] = Json::Value(lpD->AlarmActive);
		jsonResultData["AlarmType"] = Json::Value(lpD->AlarmType);
		jsonResultData["AlarmPriority"] = Json::Value(lpD->AlarmPriority);

		jsonResultData["AlarmTrigger"] = Json::Value(lpD->AlarmTrigger);

	}
	else if (lpVData->PointType == POINT_TYPE_VIRTUAL_AI) {
		BEGP_LOGICAL_POINT_PARAMETER_VAI_T *lpD = (BEGP_LOGICAL_POINT_PARAMETER_VAI_T*)lpVData;

		jsonResultData["PointType"] = Json::Value(lpD->PointType);
		jsonResultData["ID"] = Json::Value(BES200_htons(lpD->ID));
		jsonResultData["Active"] = Json::Value(lpD->Active);
		jsonResultData["Name"] = Json::Value(lpD->Name);
		jsonResultData["Alias"] = Json::Value(lpD->Alias);
		jsonResultData["Description"] = Json::Value(lpD->Description);

		jsonResultData["InitValue"] = Json::Value(m_comFun.BES200_htonl(lpD->InitValue));

		jsonResultData["AlarmActive"] = Json::Value(lpD->AlarmActive);
		jsonResultData["AlarmType"] = Json::Value(lpD->AlarmType);
		jsonResultData["AlarmPriority"] = Json::Value(lpD->AlarmPriority);

		jsonResultData["AlarmHighValue"] = Json::Value(lpD->AlarmHighValue);// 警惕 是不是需要BES200_htonl
		jsonResultData["AlarmLowValue"] = Json::Value(lpD->AlarmLowValue);// 警惕 是不是需要BES200_htonl
	}
	else if (lpVData->PointType == POINT_TYPE_VIRTUAL_AO) {
		BEGP_LOGICAL_POINT_PARAMETER_VAO_T *lpD = (BEGP_LOGICAL_POINT_PARAMETER_VAO_T*)lpVData;

		jsonResultData["PointType"] = Json::Value(lpD->PointType);
		jsonResultData["ID"] = Json::Value(BES200_htons(lpD->ID));
		jsonResultData["Active"] = Json::Value(lpD->Active);
		jsonResultData["Name"] = Json::Value(lpD->Name);
		jsonResultData["Alias"] = Json::Value(lpD->Alias);
		jsonResultData["Description"] = Json::Value(lpD->Description);

		jsonResultData["InitValue"] = Json::Value(m_comFun.BES200_htonl(lpD->InitValue));


		jsonResultData["Unit"] = Json::Value(lpD->Unit);
		jsonResultData["Precision"] = Json::Value(lpD->Precision);

		jsonResultData["AlarmActive"] = Json::Value(lpD->AlarmActive);
		jsonResultData["AlarmType"] = Json::Value(lpD->AlarmType);
		jsonResultData["AlarmPriority"] = Json::Value(lpD->AlarmPriority);

		jsonResultData["AlarmHighValue"] = Json::Value(lpD->AlarmHighValue);// 警惕 是不是需要BES200_htonl
		jsonResultData["AlarmLowValue"] = Json::Value(lpD->AlarmLowValue);// 警惕 是不是需要BES200_htonl
	}
	else if (lpVData->PointType == POINT_TYPE_VIRTUAL_AO) {
		BEGP_LOGICAL_POINT_PARAMETER_VDI_T *lpD = (BEGP_LOGICAL_POINT_PARAMETER_VDI_T*)lpVData;

		jsonResultData["PointType"] = Json::Value(lpD->PointType);
		jsonResultData["ID"] = Json::Value(BES200_htons(lpD->ID));
		jsonResultData["Active"] = Json::Value(lpD->Active);
		jsonResultData["Name"] = Json::Value(lpD->Name);
		jsonResultData["Alias"] = Json::Value(lpD->Alias);
		jsonResultData["Description"] = Json::Value(lpD->Description);

		jsonResultData["InitValue"] = Json::Value(m_comFun.BES200_htonl(lpD->InitValue));

		jsonResultData["AlarmActive"] = Json::Value(lpD->AlarmActive);
		jsonResultData["AlarmType"] = Json::Value(lpD->AlarmType);
		jsonResultData["AlarmPriority"] = Json::Value(lpD->AlarmPriority);

		jsonResultData["AlarmTrigger"] = Json::Value(lpD->AlarmPriority);

	}
	else if (lpVData->PointType == POINT_TYPE_VIRTUAL_DO) {
		BEGP_LOGICAL_POINT_PARAMETER_VDI_T *lpD = (BEGP_LOGICAL_POINT_PARAMETER_VDI_T*)lpVData;

		jsonResultData["PointType"] = Json::Value(lpD->PointType);
		jsonResultData["ID"] = Json::Value(BES200_htons(lpD->ID));
		jsonResultData["Active"] = Json::Value(lpD->Active);
		jsonResultData["Name"] = Json::Value(lpD->Name);
		jsonResultData["Alias"] = Json::Value(lpD->Alias);
		jsonResultData["Description"] = Json::Value(lpD->Description);

		jsonResultData["InitValue"] = Json::Value(m_comFun.BES200_htonl(lpD->InitValue));

		jsonResultData["AlarmActive"] = Json::Value(lpD->AlarmActive);
		jsonResultData["AlarmType"] = Json::Value(lpD->AlarmType);
		jsonResultData["AlarmPriority"] = Json::Value(lpD->AlarmPriority);

		jsonResultData["AlarmTrigger"] = Json::Value(lpD->AlarmPriority);

	}
	return jsonResultData;
}

Json::Value CCrossDataManager::ParseGetPointAlarm(void *pRecvData)
{
	Json::Value jsonResultData;
	BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T *lpVData = (BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T*)pRecvData;
	jsonResultData["CurrentValue"] = Json::Value(lpVData->CurrentValue);
	jsonResultData["AlarmState"] = Json::Value(lpVData->AlarmState);
	jsonResultData["AlarmTriggerValue"] = Json::Value(lpVData->AlarmTriggerValue);
	jsonResultData["AlarmMaxValue"] = Json::Value(lpVData->AlarmMaxValue);
	return jsonResultData;
}

Json::Value CCrossDataManager::ParseGetSence(void *pRecvData)
{
	Json::Value jsonResultData;
	BEGP_RSP_PACKETDATA_GET_SCENE_PARAMETER_T *lpVData = (BEGP_RSP_PACKETDATA_GET_SCENE_PARAMETER_T*)pRecvData;
	int nType = lpVData->SceneType;
	if (nType == 0 )// 控制场景
	{
		BEGP_CONTROL_SCENE_PARAMETER_T* pData = (BEGP_CONTROL_SCENE_PARAMETER_T *)pRecvData;
		jsonResultData["SceneType"] = Json::Value(pData->SceneType);
		jsonResultData["ID"] = Json::Value(pData->ID);
		jsonResultData["Active"] = Json::Value(pData->Active);
		jsonResultData["Name"] = Json::Value(pData->Name);
		jsonResultData["Alias"] = Json::Value(pData->Alias);
		for (int i = 0;i < CONTROL_MODE_SIZE; i++) {
			jsonResultData["CollectMode"][i]["Active"] = Json::Value(pData->ControlMode[i].Active);
			jsonResultData["CollectMode"][i]["Name"] = Json::Value(pData->ControlMode[i].Active);
			jsonResultData["CollectMode"][i]["ID"] = Json::Value(pData->ControlMode[i].Active);
			for (int j = 0; j < CONTROL_POINT_SIZE; j++)
			{
				jsonResultData["CollectMode"][i]["CollectPoint"][j]["Active"] = Json::Value(pData->ControlMode[i].ControlPoint[j].Active);
				jsonResultData["CollectMode"][i]["CollectPoint"][j]["PointID"] = Json::Value(pData->ControlMode[i].ControlPoint[j].PointID);
				jsonResultData["CollectMode"][i]["CollectPoint"][j]["RunValue"] = Json::Value(pData->ControlMode[i].ControlPoint[j].RunValue);
			}
		}
	}
	else if (nType == 1) // 采集场景
	{
		BEGP_COLLECT_SCENE_PARAMETER_T* pData = (BEGP_COLLECT_SCENE_PARAMETER_T *)pRecvData;
		jsonResultData["SceneType"] = Json::Value(pData->SceneType);
		jsonResultData["ID"] = Json::Value(pData->ID);
		jsonResultData["Active"] = Json::Value(pData->Active);
		jsonResultData["Name"] = Json::Value(pData->Name);
		jsonResultData["Alias"] = Json::Value(pData->Alias);

		for (int i = 0;i < CONTROL_MODE_SIZE; i++) {

			jsonResultData["CollectMode"][i]["Active"] = Json::Value(pData->CollectMode[i].Active);
			jsonResultData["CollectMode"][i]["Name"] = Json::Value(pData->CollectMode[i].Active);
			jsonResultData["CollectMode"][i]["ID"] = Json::Value(pData->CollectMode[i].Active);
			for (int j = 0; j < CONTROL_POINT_SIZE; j++)
			{
				jsonResultData["CollectMode"][i]["CollectPoint"][j]["Active"] = Json::Value(pData->CollectMode[i].CollectPoint[j].Active);
				jsonResultData["CollectMode"][i]["CollectPoint"][j]["PointID"] = Json::Value(pData->CollectMode[i].CollectPoint[j].PointID);
				jsonResultData["CollectMode"][i]["CollectPoint"][j]["CollectID"] = Json::Value(pData->CollectMode[i].CollectPoint[j].CollectID);
				jsonResultData["CollectMode"][i]["CollectPoint"][j]["TimeUnit"] = Json::Value(pData->CollectMode[i].CollectPoint[j].TimeUnit);
				jsonResultData["CollectMode"][i]["CollectPoint"][j]["Value"] = Json::Value(pData->CollectMode[i].CollectPoint[j].Value);
			}
		}
	}
	return jsonResultData;
}

Json::Value CCrossDataManager::ParseGetPlan(void *pRecvData)
{
	Json::Value jsonResultData;
	BEGP_RSP_PACKETDATA_GET_SCHEDULER_PARAMETER_T *lpVData = (BEGP_RSP_PACKETDATA_GET_SCHEDULER_PARAMETER_T*)pRecvData;
	jsonResultData["Active"] = Json::Value(lpVData->Active);
	jsonResultData["Name"] = Json::Value(lpVData->Name);
	jsonResultData["Alias"] = Json::Value(lpVData->Alias);
	jsonResultData["PlanType"] = Json::Value(lpVData->PlanType);

	jsonResultData["StartDateYear"] = Json::Value(lpVData->StartDateYear);
	jsonResultData["StartDateMonth"] = Json::Value(lpVData->StartDateMonth);
	jsonResultData["StartDateDay"] = Json::Value(lpVData->StartDateDay);
	jsonResultData["EndDateYear"] = Json::Value(lpVData->EndDateYear);
	jsonResultData["EndDateMonth"] = Json::Value(lpVData->EndDateMonth);
	jsonResultData["EndDateDay"] = Json::Value(lpVData->EndDateDay);


	jsonResultData["StartTimeHour"] = Json::Value(lpVData->StartTimeHour);
	jsonResultData["StartTimeMinute"] = Json::Value(lpVData->StartTimeMinute);
	jsonResultData["StartTimeSecond"] = Json::Value(lpVData->StartTimeSecond);
	jsonResultData["EndTimeHour"] = Json::Value(lpVData->EndTimeHour);
	jsonResultData["EndTimeMinute"] = Json::Value(lpVData->EndTimeMinute);
	jsonResultData["EndTimeSecond"] = Json::Value(lpVData->EndTimeSecond);

	jsonResultData["ExecutionWay"] = Json::Value(lpVData->ExecutionWay);
	jsonResultData["WeekMask"] = Json::Value(lpVData->WeekMask);
	jsonResultData["SceneType"] = Json::Value(lpVData->SceneType);
	jsonResultData["SceneID"] = Json::Value(BES200_htons(lpVData->SceneID));
	jsonResultData["ModeID"] = Json::Value(lpVData->ModeID);
	jsonResultData["DefaultModeID"] = Json::Value(lpVData->DefaultModeID);
	return jsonResultData;
}
