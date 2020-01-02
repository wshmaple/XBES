#include "stdafx.h"
#include "CLampDataManager.h"
#include "Gloable_Cross.h"

using namespace Gloable_Cross;
#define  get_data_size(structHead,structData)  BES200_htons(sizeof(structHead)+sizeof(structData))
CLampDataManager::CLampDataManager()
{
}


CLampDataManager::~CLampDataManager()
{
}

int CLampDataManager::AddOrSetControllerModule(Json::Value jsonData, char *sendbuf)
{
	// 数据格式头
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode =  jsonData["index"].asUInt();
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize =m_comFun.BES200_htons( sizeof( LAMP_BEGP_REQ_PACKETDATA_ADD_MODULE_T));
	//	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_HEAD_T) + sizeof(BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T));
		// 数据结构体
	LAMP_BEGP_REQ_PACKETDATA_ADD_MODULE_T tmpAdd;
	if (jsonData["data"]["ModelID"].isString())
	{
		tmpAdd.ModelID = atoi( jsonData["data"]["ModelID"].asString().c_str());
	}
	else {
		tmpAdd.ModelID = jsonData["data"]["ModelID"].asUInt();
	}
	
	tmpAdd.AreaNum =  jsonData["data"]["FlnID"].asUInt();
	tmpAdd.BranchNum =  jsonData["data"]["FlnID"].asUInt();

	if (jsonData["data"]["SlaveAddress"].isString())
	{
		tmpAdd.SlaveAddress = atoi(jsonData["data"]["SlaveAddress"].asString().c_str());
	}
	else {
		tmpAdd.SlaveAddress = jsonData["data"]["SlaveAddress"].asUInt();
	}

	if (jsonData["data"]["Active"].isString())
	{
		tmpAdd.Active = atoi(jsonData["data"]["Active"].asString().c_str());
	}
	else {
		tmpAdd.Active = jsonData["data"]["Active"].asUInt();
	}

	if (jsonData["data"]["ID"].isString())
	{
		tmpAdd.ID = m_comFun.BES200_htons( atoi(jsonData["data"]["ID"].asString().c_str()));
	}
	else {
		tmpAdd.ID = m_comFun.BES200_htons(jsonData["data"]["ID"].asUInt());
	}

	if (jsonData["data"]["AreaNum"].isString())
	{
		tmpAdd.AreaNum = (atoi(jsonData["data"]["AreaNum"].asString().c_str()));
	}
	else {
		tmpAdd.AreaNum = (jsonData["data"]["AreaNum"].asUInt());
	}

	if (jsonData["data"]["BranchNum"].isString())
	{
		tmpAdd.BranchNum = (atoi(jsonData["data"]["BranchNum"].asString().c_str()));
	}
	else {
		tmpAdd.BranchNum = (jsonData["data"]["BranchNum"].asUInt());
	}

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

int CLampDataManager::AddOrSetControllerModule2(Json::Value jsonData, char *sendbuf)
{
	BEGP_REQ_HEAD_T                               *pHead_req;
	BEGP_RSP_HEAD_T                               *pHead_rsp;
	LAMP_BEGP_REQ_PACKETDATA_ADD_MODULE_T    *pData_rep;
	BEGP_RSP_PACKETDATA_ECHO_T                    *pData_rsp;
	int  SendDataSize = 0;
	//char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&sendbuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (LAMP_BEGP_REQ_PACKETDATA_ADD_MODULE_T*)(&sendbuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是IP路由器识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_MODULE_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(sizeof(LAMP_BEGP_REQ_PACKETDATA_ADD_MODULE_T));
	SendDataSize += BEGP_REQ_HEAD_SIZE;




	//构造请求正文数据
	memcpy(pData_rep->Name, jsonData["data"]["Name"].asString().c_str(), sizeof(pData_rep->Name));//模块名字
	memcpy(pData_rep->Alias, jsonData["data"]["Alias"].asString().c_str(), sizeof(pData_rep->Alias));//模块别名
	memcpy(pData_rep->Description, jsonData["data"]["Description"].asString().c_str(), sizeof(pData_rep->Description));//模块的描述信息
	memcpy(pData_rep->Location, jsonData["data"]["Location"].asString().c_str(), sizeof(pData_rep->Location));//模块的安装位置

	uint8_t ukey = 0;
	if (jsonData["data"]["ModelID"].isString())
	{
		ukey = atoi(jsonData["data"]["ModelID"].asString().c_str());
	}
	else {
		ukey = jsonData["data"]["ModelID"].asUInt();
	}
	pData_rep->ModelID = ukey; //模块型号编码


	if (jsonData["data"]["AreaNum"].isString())
	{
		ukey = atoi(jsonData["data"]["AreaNum"].asString().c_str());
	}
	else {
		ukey = jsonData["data"]["AreaNum"].asUInt();
	}

	pData_rep->AreaNum = ukey==0?1:ukey;//区域号1-255

	if (jsonData["data"]["BranchNum"].isString())
	{
		ukey = atoi(jsonData["data"]["BranchNum"].asString().c_str());
	}
	else {
		ukey = jsonData["data"]["BranchNum"].asUInt();
	}
	pData_rep->BranchNum = ukey == 0 ? 1 : ukey;//支路号1-255
	if (jsonData["data"]["SlaveAddress"].isString())
	{
		ukey = atoi(jsonData["data"]["SlaveAddress"].asString().c_str());
	}
	else {
		ukey = jsonData["data"]["SlaveAddress"].asUInt();
	}
	pData_rep->SlaveAddress = ukey;//模块的通信地址

	if (jsonData["data"]["Active"].isString())
	{
		ukey = atoi(jsonData["data"]["Active"].asString().c_str());
	}
	else {
		ukey = jsonData["data"]["Active"].asUInt();
	}
	pData_rep->Active = ukey;

	uint16_t ukeyID = 0;
	if (jsonData["data"]["ID"].isString())
	{
		ukeyID = atoi(jsonData["data"]["ID"].asString().c_str());
	}
	else {
		ukeyID = jsonData["data"]["ID"].asUInt();
	}
	pData_rep->ID = BES200_htons(ukeyID);





	SendDataSize += sizeof(LAMP_BEGP_REQ_PACKETDATA_ADD_MODULE_T);

//	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	return  SendDataSize;
	return 0;
}

Json::Value CLampDataManager::ParseGetModePar(void *pRecvData)
{
	Json::Value jsonResultData;
	LAMP_BEGP_REQ_PACKETDATA_ADD_MODULE_T *lpVData = (LAMP_BEGP_REQ_PACKETDATA_ADD_MODULE_T*)pRecvData;
	jsonResultData["Name"] = Json::Value(lpVData->Name);
	jsonResultData["Alias"] = Json::Value(lpVData->Alias);
	jsonResultData["Description"] = Json::Value(lpVData->Description);
	jsonResultData["Location"] = Json::Value(lpVData->Location);
	jsonResultData["ModelID"] = Json::Value(lpVData->ModelID);
	jsonResultData["AreaNum"] = Json::Value(lpVData->AreaNum);
	jsonResultData["BranchNum"] = Json::Value(lpVData->BranchNum);
	jsonResultData["SlaveAddress"] = Json::Value(lpVData->SlaveAddress);
	jsonResultData["Active"] = Json::Value(lpVData->Active);
	uint16_t uid = lpVData->ID;
	uint16_t uuid = BES200_htons(uid);

	//uint16_t uu =  ntohs(uid);
	//uint16_t uu2 = htons(uid);

	//uint16_t uu3 = ntohl(uid);

	//uint16_t uu4 = htonl(uid);

	//uint16_t uuuid = m_comFun.BES200_ntohs(uid);
	
	jsonResultData["ID"] = Json::Value(BES200_htons(lpVData->ID));
	return jsonResultData;
}

int CLampDataManager::AddScene(Json::Value jsonData, char * sendbuf)
{
	int nSendSize = 0;
	BEGP_REQ_HEAD_T *pthead = (BEGP_REQ_HEAD_T *)sendbuf;
	pthead->FunctionCode =  jsonData["index"].asUInt();
	pthead->SequenceNumber = 0;
	pthead->SlaveAddress = 1;

	int nSceneType =  jsonData["data"]["SceneType"].asUInt();
	if (nSceneType == 0)//控制场景
	{
		LAMP_BEGP_CONTROL_SCENE_PARAMETER_T *lp = (LAMP_BEGP_CONTROL_SCENE_PARAMETER_T *)(sendbuf + sizeof(BEGP_REQ_HEAD_T));
		pthead->DataSize = sizeof(BEGP_REQ_HEAD_T) + sizeof(LAMP_BEGP_CONTROL_SCENE_PARAMETER_T);
		nSendSize = sizeof(BEGP_REQ_HEAD_T) + sizeof(LAMP_BEGP_CONTROL_SCENE_PARAMETER_T);
		lp->SceneType =  jsonData["data"]["data"]["SceneType"].asUInt();
		lp->ID =  jsonData["data"]["data"]["ID"].asUInt();
		lp->Active =  jsonData["data"]["data"]["Active"].asUInt();
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
			Json::Value vaModeItem = vaMode[i];
			//vaMode.get(i, vaModeItem);

			lp->ControlMode[i].Active =  vaModeItem["Active"].asUInt();
			lp->ControlMode[i].ID = BES200_htons( vaModeItem["ID"].asUInt());
			strcpy_s(lp->ControlMode[i].Name, vaModeItem["Name"].asString().c_str());
			int nSizeCollectPoint = 0;
			if (vaModeItem["CollectPoint"].isArray())
			{
				nSizeCollectPoint = vaModeItem["CollectPoint"].size();
				Json::Value vaPoint = vaModeItem["CollectPoint"];
				for (int j = 0;j < nSizeCollectPoint;j++)
				{
					Json::Value vaPointItem = vaPoint[j];
					lp->ControlMode[i].ControlPoint[j].Active =  vaPointItem["Active"].asUInt();
					lp->ControlMode[i].ControlPoint[j].PointID = BES200_htons( vaPointItem["PointID"].asUInt());
					lp->ControlMode[i].ControlPoint[j].RunValue = m_comFun.BES200_htonl( vaPointItem["RunValue"].asUInt());
					lp->ControlMode[i].ControlPoint[j].DelayTime = vaPointItem["RunValue"].asUInt();
				}
			}
		}


	}
	else if (nSceneType == 1)// 采集场景
	{
		LAMP_BEGP_COLLECT_SCENE_PARAMETER_T *lp = (LAMP_BEGP_COLLECT_SCENE_PARAMETER_T *)(sendbuf + sizeof(BEGP_REQ_HEAD_T));
		pthead->DataSize = sizeof(BEGP_REQ_HEAD_T) + sizeof(LAMP_BEGP_COLLECT_SCENE_PARAMETER_T);
		nSendSize = sizeof(BEGP_REQ_HEAD_T) + sizeof(LAMP_BEGP_COLLECT_SCENE_PARAMETER_T);
		lp->SceneType =  jsonData["data"]["data"]["SceneType"].asUInt();
		lp->ID =  jsonData["data"]["data"]["ID"].asUInt();
		lp->Active =  jsonData["data"]["data"]["Active"].asUInt();
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
			lp->CollectMode[i].Active =  vaModeItem["Active"].asUInt();
			lp->CollectMode[i].ID = BES200_htons( vaModeItem["ID"].asUInt());
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
					lp->CollectMode[i].CollectPoint[j].Active =  vaPointItem["Active"].asUInt();
					lp->CollectMode[i].CollectPoint[j].CollectID =  vaPointItem["CollectID"].asUInt();
					lp->CollectMode[i].CollectPoint[j].TimeUnit =  vaPointItem["TimeUnit"].asUInt();
					lp->CollectMode[i].CollectPoint[j].PointID = BES200_htons( vaPointItem["PointID"].asUInt());
					lp->CollectMode[i].CollectPoint[j].Value = m_comFun.BES200_htonl( vaPointItem["Value"].asUInt());
				}
			}

		}
	}
	return nSendSize;
}

int CLampDataManager::SetScene(Json::Value jsonData, char * sendbuf)
{
	return AddScene(jsonData, sendbuf);
}

Json::Value CLampDataManager::ParseGetSence(void *pRecvData)
{
	Json::Value jsonResultData;
	BEGP_RSP_PACKETDATA_GET_SCENE_PARAMETER_T *lpVData = (BEGP_RSP_PACKETDATA_GET_SCENE_PARAMETER_T*)pRecvData;
	int nType = lpVData->SceneType;
	if (nType == 0)// 控制场景
	{
		LAMP_BEGP_CONTROL_SCENE_PARAMETER_T* pData = (LAMP_BEGP_CONTROL_SCENE_PARAMETER_T *)pRecvData;
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
				jsonResultData["CollectMode"][i]["CollectPoint"][j]["DelayTime"] = Json::Value(pData->ControlMode[i].ControlPoint[j].DelayTime);
			}
		}
	}
	else if (nType == 1) // 采集场景
	{
		LAMP_BEGP_COLLECT_SCENE_PARAMETER_T* pData = (LAMP_BEGP_COLLECT_SCENE_PARAMETER_T *)pRecvData;
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

