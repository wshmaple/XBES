#include "stdafx.h"
#include "CCrossDataManager.h"
#include "Gloable_Cross.h"

using namespace Gloable_Cross;
#define  DEF_CROSS_SERVER_PORT 1204

typedef  unsigned       char uint8_t;
typedef  unsigned short int  uint16_t;
typedef  unsigned       int  uint32_t;
typedef    signed       char int8_t;
typedef    signed short int  int16_t;
typedef    signed       int  int32_t;

CCrossDataManager::CCrossDataManager()
{
	m_threadpool.SetPoolSize(5);// ��ʼ�� 5�� ��ʱ�߼� �����߳�
}

CCrossDataManager::~CCrossDataManager()
{
}
uint16_t CCrossDataManager::BES200_htons(uint16_t n)
{
	return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}
int CCrossDataManager::BES200_RecvAFrame(const char ServerIP[], char *pRecvData)
{
	int DataSize = 0, FrameSize, len;
	BEGP_RSP_HEAD_T *pHead_rsp;
	pHead_rsp = (BEGP_RSP_HEAD_T*)pRecvData;
	/*���հ�ͷ*/
	len = m_TCP.Recv(ServerIP, (void*)pRecvData, BEGP_RSP_HEAD_SIZE);
	if (len != BEGP_RSP_HEAD_SIZE)
	{
		return -1;
	}
	DataSize += BEGP_RSP_HEAD_SIZE;
	FrameSize = m_comFun.BES200_htons(pHead_rsp->DataSize) + BEGP_CRC_SIZE;
	/*�����������ݺ�У������*/
	len = m_TCP.Recv(ServerIP, (void*)(pRecvData + BEGP_RSP_HEAD_SIZE), FrameSize);
	if (len != FrameSize)
	{
		return -1;
	}
	DataSize += FrameSize;
	return DataSize;
}

int CCrossDataManager::BES200_Communication(const char ServerIP[], const char *pSendData, int SendDataSize, char *pRecvData, int *RecvDataSize)
{
//	ZC_LOCK::CSafeLock lock(&m_lock); //���ٽ�����
	uint16_t *crc_req, crc1, crc2;
	int size, trytimes = 0;
	
	try
	{
		
		strlen(pSendData);
		crc_req = (uint16_t*)(pSendData + SendDataSize);
	
		*crc_req = m_comFun.BES200_CRC_16(pSendData, SendDataSize);
	


	}
	catch (const std::exception& )
	{
		LOGE("errors:"<<GetLastError());
	}
	

	SendDataSize += BEGP_CRC_SIZE;
again:
	if (m_TCP.Connect(ServerIP) == -1)
		return BES200_ERROR_CONNECT;
	if (m_TCP.Send(ServerIP,(void*)pSendData, SendDataSize) != SendDataSize)
	{
		m_TCP.DisConnect(ServerIP);
		if (m_TCP.Connect(ServerIP) == -1) {
			m_TCP.DisConnect(ServerIP);
			return BES200_ERROR_CONNECT;
		}
		if (m_TCP.Send(ServerIP,(void*)pSendData, SendDataSize) != SendDataSize)
		{
			m_TCP.DisConnect(ServerIP);
			return  BES200_ERROR_SEND;
		}
		
	}
	//Sleep(10);
	size = BES200_RecvAFrame(ServerIP, pRecvData);
	if (size == -1)
	{
		trytimes++;
		m_TCP.DisConnect(ServerIP);
		if (trytimes <= 1)
			goto again;
		else
			return BES200_ERROR_RECIVE;
	}

	*RecvDataSize = size;
	/*�յ�һ֡���ݣ���ʼ����*/
	crc1 = m_comFun.BES200_CRC_16(pRecvData, (*RecvDataSize) - BEGP_CRC_SIZE);
	crc2 = *((uint16_t*)(pRecvData + (*RecvDataSize) - BEGP_CRC_SIZE));
	if (crc1 == crc2)
	{
		return BES200_ERROR_OK;
	}
	else
	{
		return BES200_ERROR_RECIVE;
	}

	return BES200_ERROR_OK;
}

int CCrossDataManager::DoPackSendBuff(Json::Value jsonData, char * SendBuf)
{
	int nIndex = jsonData["index"].asUInt();
	int nSendSize = 0;
	switch (nIndex)
	{
	case BEGP_FUNCODE_ADD_CONTROLLER_PARAMETER:							//����һ��������
		nSendSize = AddControlParameter(jsonData, SendBuf);
		break;
	case BEGP_FUNCODE_SET_CONTROLLER_PARAMETER:							//����һ��������
		nSendSize = SetControlParameter(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_SET_CONTROLLER_TIME:					// ���ÿ�������ʱ��
		nSendSize = SetControllerTime(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_REBOOT_CONTROLLER:				    // �������������൱�ڸ�λ
	case		BEGP_FUNCODE_RESET_CONTROLLER:					// ���ÿ��������ָ��������ã�������
	case		BEGP_FUNCODE_DELETE_CONTROLLER:				    // ɾ��һ������������ɾ��������ص�ģ��͵�
		nSendSize = OnlySendCmdToHard(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_GET_CONTROLLER_TIME:					//��ȡ��������ʱ��
	case		BEGP_FUNCODE_GET_CONTROLLER_PARAMETER:					//��ȡ���������������ò���
		nSendSize = OnlySendCmdToHard(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_GET_CONTROLLER_INFO:					//ͨ��UDP�㲥��ʽ��ȡ���п���������Ϣ
		break;
	case		BEGP_FUNCODE_SET_CONTROLLER_INFO:					//ͨ��UDP�㲥��ʽ���ÿ������Ĳ���
		break;
	case		BEGP_FUNCODE_ADD_MODULE:						//������һ��ģ��
		nSendSize = AddOrSetControllerModule(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_SET_MODULE_PARAMETER:					//����һ��ģ������в���
		nSendSize = AddOrSetControllerModule(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_DELETE_MODULE:					//ɾ��һ��ģ�飬��ɾ����ģ����صĵ�
		nSendSize = DeleteControllerModule(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_GET_MODULE_PARAMETER:					//��ȡһ��ģ�������������Ϣ
		nSendSize = GetControllerModuleData2(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_GET_MODULE_ERROR_CODE:					//��ȡһ��ģ��Ĵ������
		nSendSize = GetControllerModuleErrorCode(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_ADD_LOGICAL_POINT:			//������һ���߼���
		nSendSize = AddOrSetPoint(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_SET_LOGICAL_POINT_PARAMETER:			//����һ���߼�������в���
		nSendSize = AddOrSetPoint(jsonData, SendBuf);
		break;
		break;
	case		BEGP_FUNCODE_SET_LOGICAL_POINT_VALUE:			//����һ���߼����ֵ
		nSendSize = SetPointValue(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_SET_LOGICAL_POINT_VALUE_BY_NAME:			//�����߼����ֵ�����ݵ������
		nSendSize = SetPointValueByName(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_DELETE_LOGICAL_POINT:			//ɾ��һ���߼���
		nSendSize = DeletePointByID(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_GET_LOGICAL_POINT_PARAMETER:			//��ȡһ���߼�����������ò���
		nSendSize = GetPointParameter(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_GET_LOGICAL_POINT_ALARM_INFOR:				//��ȡһ���߼���ı�����Ϣ
		nSendSize = GetPointAlarmErrorCode(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_ADD_SCENE:									//����һ������
		nSendSize = AddScene(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_SET_SCENE_PARAMETER:						//����һ�����������в���
		nSendSize = SetScene(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_DELETE_SCENE:								//ɾ��һ������
		nSendSize = DelScene(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_GET_SCENE_PARAMETER:						//��ȡһ��������������Ϣ
		nSendSize = GetScene(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_ADD_SCHEDULER:								//����һ���ƻ�
		nSendSize = AddScheduler(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_SET_SCHEDULER_PARAMETER:					//�޸�һ���ƻ������в���
		nSendSize = SetScheduler(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_DELETE_SCHEDULER:							//ɾ��һ���ƻ�
		nSendSize = DelScheduler(jsonData, SendBuf);
		break;
	case		BEGP_FUNCODE_GET_SCHEDULER_PARAMETER:					//��ȡһ���ƻ������в���
		nSendSize = GetScheduler(jsonData, SendBuf);
		break;
	case		BEGP_FUNCUDE_IAP:										//Զ������
		nSendSize = UpDate(jsonData, SendBuf);
		break;
	}

	return nSendSize;
}

Json::Value CCrossDataManager::DoPackRecvBuffToJson(char * RecvBuf , int nIndex, Json::Value &vrt)
{
	Json::Value jsonResultData;
	int nRetCode = -1;
	// ��� ͷ�ǲ�����ȷ��
	{
		void *pRecvData = (RecvBuf + BEGP_RSP_HEAD_SIZE);
		BEGP_RSP_PACKETDATA_ECHO_T * pp = (BEGP_RSP_PACKETDATA_ECHO_T*)pRecvData;
		nRetCode = ((BEGP_RSP_PACKETDATA_ECHO_T*)pRecvData)->Code;
		switch (nIndex)
		{
		case BEGP_FUNCODE_ADD_CONTROLLER_PARAMETER:
		{
			nRetCode = ((BEGP_RSP_PACKETDATA_ECHO_T*)pRecvData)->Code;
		}
		break;
		case BEGP_FUNCODE_GET_CONTROLLER_TIME:// ��ȡϵͳʱ��
		{
			int nSize2 = BES200_htons(sizeof(BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_T));
			jsonResultData = ParseGetTime(pRecvData);
			nRetCode = 0;
		}
		break;
		case BEGP_FUNCODE_GET_CONTROLLER_PARAMETER:// ��ȡ���������������ò���
		{
			jsonResultData = ParseGetControlPar(pRecvData);
			nRetCode = 0;
		}
		break;
		case BEGP_FUNCODE_GET_MODULE_PARAMETER: //��ȡģ�������������Ϣ
		{
			jsonResultData = ParseGetModePar(pRecvData);
			nRetCode = 0;
		}
		break;
		case  BEGP_FUNCODE_GET_MODULE_ERROR_CODE://��ȡģ��Ĵ������
		{
			BEGP_RSP_PACKETDATA_GET_MODULE_ERROR_CODE_T *lpVData = (BEGP_RSP_PACKETDATA_GET_MODULE_ERROR_CODE_T*)pRecvData;
			jsonResultData["ErrorCode"] = Json::Value(lpVData->ErrorCode);
		}
		break;
		case BEGP_FUNCODE_GET_LOGICAL_POINT_PARAMETER:// ��ȡһ���߼�����������ò���
		{
			jsonResultData = ParseGetPointPar(pRecvData);
			nRetCode = 0;
		}
		break;
		case BEGP_FUNCODE_GET_LOGICAL_POINT_ALARM_INFOR:
		{
			jsonResultData = ParseGetPointAlarm(pRecvData);
			nRetCode = 0;
		}
		break;
		case BEGP_FUNCODE_DELETE_SCENE:
		{
			
		}
			break;
		case BEGP_FUNCODE_GET_SCENE_PARAMETER:
		{
			jsonResultData = ParseGetSence(pRecvData);
			nRetCode = 0;
		}
		break;
		case BEGP_FUNCODE_GET_SCHEDULER_PARAMETER: //��ȡһ���ƻ������в���.
		{
			jsonResultData = ParseGetPlan(pRecvData);
			nRetCode = 0;
		}
		break;
		}

	}

	switch (nIndex)
	{
	case BEGP_FUNCODE_ADD_CONTROLLER_PARAMETER://����һ��������
		break;
	case BEGP_FUNCODE_SET_CONTROLLER_PARAMETER://����һ��������
		break;
	case		BEGP_FUNCODE_SET_CONTROLLER_TIME:					// ���ÿ�������ʱ��
	case		BEGP_FUNCODE_REBOOT_CONTROLLER:				    //�������������൱�ڸ�λ
	case		BEGP_FUNCODE_RESET_CONTROLLER:					//���ÿ��������ָ��������ã�������
	case		BEGP_FUNCODE_DELETE_CONTROLLER:				    //ɾ��һ������������ɾ��������ص�ģ��͵�
		break;
	case		BEGP_FUNCODE_GET_CONTROLLER_TIME:  //��ȡ��������ʱ��
		break;
	case		BEGP_FUNCODE_GET_CONTROLLER_PARAMETER:   //��ȡ���������������ò���

		break;
	case		BEGP_FUNCODE_GET_CONTROLLER_INFO:   //ͨ��UDP�㲥��ʽ��ȡ���п���������Ϣ
		break;
	case		BEGP_FUNCODE_SET_CONTROLLER_INFO:   //ͨ��UDP�㲥��ʽ���ÿ������Ĳ���
		break;
	case		BEGP_FUNCODE_ADD_MODULE:      //������һ��ģ��

		break;
	case		BEGP_FUNCODE_SET_MODULE_PARAMETER:  //����һ��ģ������в���

		break;
	case		BEGP_FUNCODE_DELETE_MODULE:  //ɾ��һ��ģ�飬��ɾ����ģ����صĵ�
	case		BEGP_FUNCODE_GET_MODULE_PARAMETER:  //��ȡһ��ģ�������������Ϣ
		break;
	case		BEGP_FUNCODE_GET_MODULE_ERROR_CODE:  //��ȡһ��ģ��Ĵ������
		// �и� errorcode �Ĵ������ ��Ҫ���д���
		break;

	case		BEGP_FUNCODE_ADD_LOGICAL_POINT://������һ���߼���

		break;
	case		BEGP_FUNCODE_SET_LOGICAL_POINT_PARAMETER://����һ���߼�������в���

		break;
	case		BEGP_FUNCODE_SET_LOGICAL_POINT_VALUE://����һ���߼����ֵ

		break;
	case		BEGP_FUNCODE_SET_LOGICAL_POINT_VALUE_BY_NAME://�����߼����ֵ�����ݵ������

		break;
	case		BEGP_FUNCODE_DELETE_LOGICAL_POINT://ɾ��һ���߼���

		break;
	case		BEGP_FUNCODE_GET_LOGICAL_POINT_PARAMETER://��ȡһ���߼�����������ò���

		break;
	case		BEGP_FUNCODE_GET_LOGICAL_POINT_ALARM_INFOR: //��ȡһ���߼���ı�����Ϣ


		break;

	case		BEGP_FUNCODE_ADD_SCENE:              //����һ������


		break;
	case		BEGP_FUNCODE_SET_SCENE_PARAMETER:     //����һ�����������в���


		break;
	case		BEGP_FUNCODE_DELETE_SCENE:            //ɾ��һ������

		break;
	case		BEGP_FUNCODE_GET_SCENE_PARAMETER:     //��ȡһ��������������Ϣ

		break;

	case		BEGP_FUNCODE_ADD_SCHEDULER:           //����һ���ƻ�

		break;
	case		BEGP_FUNCODE_SET_SCHEDULER_PARAMETER: //�޸�һ���ƻ������в���

		break;
	case		BEGP_FUNCODE_DELETE_SCHEDULER:        //ɾ��һ���ƻ�

		break;
	case		BEGP_FUNCODE_GET_SCHEDULER_PARAMETER: //��ȡһ���ƻ������в���

		break;

	case		BEGP_FUNCUDE_IAP: //Զ������

		break;
	}

	vrt["hcode"] = Json::Value(nRetCode);
	
	return jsonResultData;
}
int TT(int nV) {
	return 0;
}
Json::Value CCrossDataManager::ComDataManager(Json::Value jsonData)
{
	int nIndex = jsonData["index"].asUInt();
 	int nPort = 0;
	nPort = atoi( jsonData["Port"].asString().c_str());
	string strIP = jsonData["IPAddr"].asString();
	const char *IPAddr = NULL;
	IPAddr = strIP.c_str();
	string strItem = jsonData["item"].asString();
	//�����ӿ�����Ҫ�Ĳ���	
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };
	memset(SendBuf, 0, SENDBUF_SIZE);
	memset(RecvBuf, 0, RECVBUF_SIZE);
	int nSendSize = 0;
//	nSendSize = DoPackSendBuff(jsonData, SendBuf);
	//auto rtBuf = m_threadpool.enqueue([this](Json::Value jsvalue,char *SendBuf) {
	//	return	DoPackSendBuff(jsvalue, SendBuf);
	//}, jsonData, SendBuf);
	//int nTTSize = 	rtBuf.get();
	//nSendSize = nTTSize;
	nSendSize = DoPackSendBuff(jsonData, SendBuf);
	int nRecvSize = 0;
	BEGP_REQ_HEAD_T *pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;


	DWORD start_time = GetTickCount();

	int	nRes = BES200_Communication(IPAddr, SendBuf, nSendSize, RecvBuf, &nRecvSize);

	DWORD end_time = GetTickCount();
	DWORD dw = end_time - start_time;
	//LOG_INFO(this_thread::get_id() << "TCP������ʱ��" << dw);

	BEGP_RSP_HEAD_T *pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;
	Json::Value jsonResult;
	Json::Value jsonResultData;
	if (pHead_rsp->FunctionCode == pHead_req->FunctionCode &&
		pHead_rsp->SlaveAddress == pHead_req->SlaveAddress )
	{
		jsonResultData= DoPackRecvBuffToJson(RecvBuf,nIndex,jsonResult);
	}
	//������ֵ��װ��json����
	jsonResult["code"] = Json::Value(nRes);
	jsonResult["item"] = Json::Value(strItem);
	jsonResult["index"] = Json::Value(nIndex);
	jsonResult["IPAddr"] = Json::Value(strIP);
	jsonResult["Port"] = Json::Value(nPort);
	// ���� ���صĽ��ֵ
	jsonResult["data"] = jsonResultData;

	return jsonResult;//����������Ϣ
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
	// ���ݸ�ʽͷ
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = jsonData["index"].asUInt();
	thead.SequenceNumber = 1;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T));
	//	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_HEAD_T) + sizeof(BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T));
		// ���ݽṹ��
	BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T tmpAdd;
	memset(&tmpAdd, 0, sizeof(tmpAdd));
	tmpAdd.Active = GetUint8("Active", jsonData);
	//tmpAdd.Active = jsonData["data"]["Active"].asUInt();
	//tmpAdd.ID = jsonData["data"]["ID"].asUInt();
	tmpAdd.ID = GetUint16("ID",jsonData);

	string strAlias = jsonData["data"]["Alias"].asString();
	int nLen = strAlias.length();
	int nLen2 = strlen(strAlias.c_str());

	memcpy_s(tmpAdd.Name, 32, jsonData["data"]["Name"].asString().c_str(), strlen(jsonData["data"]["Name"].asString().c_str()));
	memcpy_s(tmpAdd.Alias, 64, jsonData["data"]["Alias"].asString().c_str(), strlen(jsonData["data"]["Alias"].asString().c_str()));
	memcpy_s(tmpAdd.Description, 128, jsonData["data"]["Description"].asString().c_str(), strlen(jsonData["data"]["Description"].asString().c_str()));
	memcpy_s(tmpAdd.Location, 64, jsonData["data"]["Location"].asString().c_str(), strlen(jsonData["data"]["Location"].asString().c_str()));
	memcpy_s(tmpAdd.Zone, 64, jsonData["data"]["Zone"].asString().c_str(), strlen(jsonData["data"]["Zone"].asString().c_str()));

//	strcpy_s(tmpAdd.Alias, strAlias.c_str());
//	strcpy_s(tmpAdd.Description, jsonData["data"]["Description"].asString().c_str());
//	strcpy_s(tmpAdd.Location, jsonData["data"]["Location"].asString().c_str());
//	strcpy_s(tmpAdd.Zone, jsonData["data"]["Zone"].asString().c_str());
//	strcpy_s(tmpAdd.Name, jsonData["data"]["Name"].asString().c_str());

	memset(sendbuf, 0, SENDBUF_SIZE);
	// Э��ͷ
	memcpy(sendbuf, &thead, sizeof(thead));
	// ����
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::SetControllerTime(Json::Value jsonData, char *sendbuf)
{
	// ���ݸ�ʽͷ
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = jsonData["index"].asUInt();
	thead.SequenceNumber =0;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_T));
	//	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_HEAD_T) + sizeof(BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T));
		// ���ݽṹ��
	BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_T tmpAdd;
	tmpAdd.year = jsonData["data"]["year"].asUInt()-2000;
	tmpAdd.month =jsonData["data"]["month"].asUInt();
	tmpAdd.day = jsonData["data"]["day"].asUInt();
	tmpAdd.wday = jsonData["data"]["wday"].asUInt();
	tmpAdd.hour = jsonData["data"]["hour"].asUInt();
	tmpAdd.minute = jsonData["data"]["minute"].asUInt();
	tmpAdd.second = jsonData["data"]["second"].asUInt();

	memset(sendbuf, 0, SENDBUF_SIZE);
	// Э��ͷ
	memcpy(sendbuf, &thead, sizeof(thead));
	// ����
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::OnlySendCmdToHard(Json::Value jsonData, char *sendbuf)
{
	// ���ݸ�ʽͷ
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = jsonData["index"].asUInt();
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons(0);

	memset(sendbuf, 0, SENDBUF_SIZE);
	// Э��ͷ
	memcpy(sendbuf, &thead, sizeof(thead));
	// ����
	int nSendSize = sizeof(thead);
	return nSendSize;
}

int CCrossDataManager::AddOrSetControllerModule(Json::Value jsonData, char *sendbuf)
{
	// ���ݸ�ʽͷ
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = jsonData["index"].asUInt();
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_PACKETDATA_ADD_MODULE_T));
	//	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_HEAD_T) + sizeof(BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T));
		// ���ݽṹ��
	BEGP_REQ_PACKETDATA_ADD_MODULE_T tmpAdd;
	memset(&tmpAdd, 0, sizeof(tmpAdd));
	tmpAdd.ModelID = GetUint8("ModelID", jsonData);
	uint8_t ufinid = GetUint8("FlnID", jsonData);
	tmpAdd.FlnID = ufinid>0?ufinid:1;// GetUint8("FlnID", jsonData);
	tmpAdd.SlaveAddress = GetUint8("SlaveAddress", jsonData);
	tmpAdd.Active = GetUint8("Active",jsonData);

	tmpAdd.ID = BES200_htons(GetUint16("ID", jsonData));

//	strcpy_s(tmpAdd.Name, jsonData["data"]["Name"].asString().c_str());
//	strcpy_s(tmpAdd.Alias, jsonData["data"]["Alias"].asString().c_str());
//	strcpy_s(tmpAdd.Description, jsonData["data"]["Description"].asString().c_str());
//	strcpy_s(tmpAdd.Location, jsonData["data"]["Location"].asString().c_str());

	memcpy_s(tmpAdd.Name, 32, jsonData["data"]["Name"].asString().c_str(), strlen(jsonData["data"]["Name"].asString().c_str()));
	memcpy_s(tmpAdd.Alias, 64, jsonData["data"]["Alias"].asString().c_str(), strlen(jsonData["data"]["Alias"].asString().c_str()));
	memcpy_s(tmpAdd.Description, 128, jsonData["data"]["Description"].asString().c_str(), strlen(jsonData["data"]["Description"].asString().c_str()));
	memcpy_s(tmpAdd.Location, 64, jsonData["data"]["Location"].asString().c_str(), strlen(jsonData["data"]["Location"].asString().c_str()));

	memset(sendbuf, 0, RECVBUF_SIZE);
	// Э��ͷ
	memcpy(sendbuf, &thead, sizeof(thead));
	// ����
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::DeleteControllerModule(Json::Value jsonData, char *sendbuf)
{
	// ���ݸ�ʽͷ
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = jsonData["index"].asUInt();
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_PACKETDATA_DELETE_MODULE_T));
	BEGP_REQ_PACKETDATA_DELETE_MODULE_T tmpAdd;
	//;
	tmpAdd.ID = BES200_htons(GetUint8("ID", jsonData));

	memset(sendbuf, 0, SENDBUF_SIZE);
	// Э��ͷ
	memcpy(sendbuf, &thead, sizeof(thead));
	// ����
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::GetControllerModuleData(Json::Value jsonData, char *sendbuf)
{
	// ���ݸ�ʽͷ
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = jsonData["index"].asUInt();
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons( sizeof(BEGP_REQ_PACKETDATA_DELETE_MODULE_T));
	BEGP_REQ_PACKETDATA_DELETE_MODULE_T tmpAdd;
	uint16_t uid = jsonData["data"]["ID"].asUInt();
	BEGP_REQ_PACKETDATA_DELETE_MODULE_T *p = (BEGP_REQ_PACKETDATA_DELETE_MODULE_T *)(&sendbuf[BEGP_RSP_HEAD_SIZE]);

	tmpAdd.ID = BES200_htons(uid);

	memset(sendbuf, 0, SENDBUF_SIZE);
	// Э��ͷ
	memcpy(sendbuf, &thead, sizeof(thead));
	// ����
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::GetControllerModuleData2(Json::Value jsonData, char *sendbuf)
{
	BEGP_REQ_HEAD_T                               *pHead_req;
	BEGP_RSP_HEAD_T                               *pHead_rsp;
	BEGP_REQ_PACKETDATA_GET_MODULE_PARAMETER_T    *pData_rep;
	BEGP_RSP_PACKETDATA_GET_MODULE_PARAMETER_T    *pData_rsp;
	int  SendDataSize = 0;
	//char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)&sendbuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//�������ݣ�ָ�븳ֵ
	pData_rep = (BEGP_REQ_PACKETDATA_GET_MODULE_PARAMETER_T*)(&sendbuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_MODULE_PARAMETER_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);


	//���������ͷ
 	pHead_req->SlaveAddress = 1;//����IP·������ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_MODULE_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_GET_MODULE_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;
	uint16_t ModuleID;
	if (jsonData["data"]["ID"].isInt())
	{
		ModuleID = jsonData["data"]["ID"].asUInt();
	}
	else {
		ModuleID = atoi(jsonData["data"]["ID"].asString().c_str());
	}
	
	int nL = BES200_htons(ModuleID);
	int nL1 = m_comFun.BES200_htons(ModuleID);
	int nL2 = m_comFun.BES200_htonl(ModuleID);
	//����������������
	pData_rep->ID = BES200_htons(ModuleID);    //Ҫ��ȡģ������ĵ�ID��

	SendDataSize += BEGP_REQ_PACKETDATA_GET_MODULE_PARAMETER_SIZE;

	return SendDataSize;
}

int CCrossDataManager::GetControllerModuleErrorCode(Json::Value jsonData, char *sendbuf)
{
	// ���ݸ�ʽͷ
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode = jsonData["index"].asUInt();
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_PACKETDATA_DELETE_MODULE_T));
	BEGP_REQ_PACKETDATA_DELETE_MODULE_T tmpAdd;
	tmpAdd.ID = BES200_htons(jsonData["data"]["ID"].asUInt());

	memset(sendbuf, 0, SENDBUF_SIZE);
	// Э��ͷ
	memcpy(sendbuf, &thead, sizeof(thead));
	// ����
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::AddOrSetPoint(Json::Value jsonData, char *sendbuf)
{
	int nType = jsonData["data"]["PointType"].asUInt();
	int nSendSize = 0;
//	nType = 3;
	switch (nType)
	{
	case POINT_TYPE_LAI :   //ʵ��AI
		nSendSize = AddPointLAI(jsonData, sendbuf);
		break;
	case POINT_TYPE_LAO :   //ʵ��AO
		nSendSize = AddPointLAO(jsonData, sendbuf);
		break;
	case POINT_TYPE_LDI :  //ʵ��DI
		nSendSize = AddPointLDI(jsonData, sendbuf);
		break;
	case POINT_TYPE_LDO :  //ʵ��DO �������
		nSendSize = AddPointLDO(jsonData, sendbuf);
		break;
	case POINT_TYPE_VIRTUAL_AI://���AI����
		nSendSize = AddPointVAI(jsonData, sendbuf);
		break;
	case POINT_TYPE_VIRTUAL_AO://���AO����
		nSendSize = AddPointVAO(jsonData, sendbuf);
		break;
	case POINT_TYPE_VIRTUAL_DI://���DI����
		nSendSize = AddPointVDI(jsonData, sendbuf);
		break;
	case POINT_TYPE_VIRTUAL_DO://���DO����
		nSendSize = AddPointVDO(jsonData, sendbuf);
		break;
	default:
		break;
	}
	return nSendSize;
}

int CCrossDataManager::AddPointLAI(Json::Value jsonData, char * sendbuf)
{
	BEGP_REQ_HEAD_T                                    *pHead_req;
	BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_T  *pData_req;
	int SendDataSize = 0;
	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)sendbuf;
	//�������ݣ�ָ�븳ֵ
	pData_req = (BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_T *)(sendbuf + BEGP_REQ_HEAD_SIZE);


	//���������ͷ
	pHead_req->SlaveAddress = 1;//����IP·������ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = jsonData["index"].asUInt();
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	BEGP_LOGICAL_POINT_PARAMETER_LAI_T        *pDataLAI_req;
	pDataLAI_req = (BEGP_LOGICAL_POINT_PARAMETER_LAI_T*)pData_req;
	pDataLAI_req->PointType = GetUint8("PointType", jsonData);  //�������
	pDataLAI_req->ID = BES200_htons(GetUint16("ID", jsonData));  //�߼����ID
	pDataLAI_req->Active = GetUint8("Active", jsonData);//�Ƿ�����

	int nLen = strlen(jsonData["data"]["Name"].asString().c_str());
	memcpy_s(pDataLAI_req->Name, nLen, jsonData["data"]["Name"].asString().c_str(), nLen);

	nLen = strlen(jsonData["data"]["Alias"].asString().c_str());
	memcpy_s(pDataLAI_req->Alias, nLen, jsonData["data"]["Alias"].asString().c_str(), nLen); //����

	nLen = strlen(jsonData["data"]["Description"].asString().c_str());
	memcpy_s(pDataLAI_req->Description, nLen, jsonData["data"]["Description"].asString().c_str(), nLen); //����


	pDataLAI_req->ModuleID = BES200_htons(GetUint16("ModuleID", jsonData));  //������ģ��ID
	pDataLAI_req->ChannelIndex = GetUint8("ChannelIndex", jsonData);//�����ڵ�ͨ������
	pDataLAI_req->WorkMode = GetUint8("WorkMode", jsonData); //����ģʽ
	pDataLAI_req->Polarity = GetUint8("Polarity", jsonData); //�Ƿ���
	int32_t uInitValue = Getint32("InitValue", jsonData);
	pDataLAI_req->InitValue =m_comFun.BES200_htonl(uInitValue);//ֵ
	pDataLAI_req->AlarmActive = GetUint8("AlarmActive", jsonData);//�����Ƿ�����
	pDataLAI_req->AlarmType = GetUint8("AlarmType", jsonData); //��������
	pDataLAI_req->AlarmPriority = GetUint8("AlarmPriority", jsonData);//�������ȼ�
	pDataLAI_req->LineType = GetUint8("LineType", jsonData);//ģ������������

	nLen = strlen(jsonData["data"]["Unit"].asString().c_str());
	memcpy_s(pDataLAI_req->Unit, nLen, jsonData["data"]["Unit"].asString().c_str(), nLen); //���̵�λ


	int32_t uHighRange = Getint32("HighRange", jsonData);
	pDataLAI_req->HighRange = m_comFun.BES200_htonl(uHighRange);//32λ���ݴ�С��ת��
	pDataLAI_req->LowRange = m_comFun.BES200_htonl(Getint32("LowRange", jsonData));//32λ���ݴ�С��ת��
	pDataLAI_req->Precision = GetUint8("Precision", jsonData);//����
	pDataLAI_req->AlarmHighValue = m_comFun.BES200_htonl(Getint32("AlarmHighValue", jsonData));//32λ���ݴ�С��ת��
	pDataLAI_req->AlarmLowValue = m_comFun.BES200_htonl(Getint32("AlarmLowValue", jsonData));//32λ���ݴ�С��ת��

	SendDataSize += BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE;

	return SendDataSize;
}

int CCrossDataManager::AddPointLAO(Json::Value jsonData, char *sendbuf)
{
	return AddPointLAI(jsonData, sendbuf);
}

int CCrossDataManager::AddPointLDI(Json::Value jsonData, char * sendbuf)
{
	BEGP_REQ_HEAD_T                                    *pHead_req;
	BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_T  *pData_req;
	int SendDataSize = 0;
	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)sendbuf;
	//�������ݣ�ָ�븳ֵ
	pData_req = (BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_T *)(sendbuf + BEGP_REQ_HEAD_SIZE);


	//���������ͷ
	pHead_req->SlaveAddress = 1;//����IP·������ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = jsonData["index"].asUInt();
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	BEGP_LOGICAL_POINT_PARAMETER_LDI_T        *pDataLDI_req;

	pDataLDI_req = (BEGP_LOGICAL_POINT_PARAMETER_LDI_T*)pData_req;
	pDataLDI_req->PointType = GetUint8("PointType", jsonData);  //�������
	pDataLDI_req->ID = BES200_htons(GetUint16("ID", jsonData));  //�߼����ID
	pDataLDI_req->Active = GetUint8("Active", jsonData);//�Ƿ�����




	int nLen = strlen(jsonData["data"]["Name"].asString().c_str());
	memcpy_s(pDataLDI_req->Name, nLen, jsonData["data"]["Name"].asString().c_str(), nLen);

	nLen = strlen(jsonData["data"]["Alias"].asString().c_str());
	memcpy_s(pDataLDI_req->Alias, nLen, jsonData["data"]["Alias"].asString().c_str(), nLen); //����

	nLen = strlen(jsonData["data"]["Description"].asString().c_str());
	memcpy_s(pDataLDI_req->Description, nLen, jsonData["data"]["Description"].asString().c_str(), nLen); //����


	pDataLDI_req->ModuleID = BES200_htons(GetUint16("ModuleID", jsonData));  //������ģ��ID

	pDataLDI_req->ChannelIndex = GetUint8("ChannelIndex", jsonData);//�����ڵ�ͨ������

	pDataLDI_req->WorkMode = GetUint8("WorkMode", jsonData); //����ģʽ
	pDataLDI_req->Polarity = GetUint8("Polarity", jsonData); //�Ƿ���
	pDataLDI_req->InitValue =m_comFun.BES200_htonl(Getint32("InitValue", jsonData));//ֵ

	pDataLDI_req->AlarmActive = GetUint8("AlarmActive", jsonData);//�����Ƿ�����
	pDataLDI_req->AlarmType = GetUint8("AlarmType", jsonData); //��������
	pDataLDI_req->AlarmPriority = GetUint8("AlarmPriority", jsonData);//�������ȼ�

	pDataLDI_req->AlarmTrigger = GetUint8("AlarmTrigger", jsonData);//��������
	pDataLDI_req->ActivePassive = GetUint8("ActivePassive", jsonData);//��Դ��Դ


	SendDataSize += BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE;

	return SendDataSize;
}

int CCrossDataManager::AddPointLDO(Json::Value jsonData, char *sendbuf)
{
	uint8_t nPointType = jsonData["data"]["PointType"].asUInt();
	if (nPointType == POINT_TYPE_LDO)
	{
		BEGP_REQ_HEAD_T                                    *pHead_req;
		int  SendDataSize = 0;
		//���������ͷ
		pHead_req = (BEGP_REQ_HEAD_T*)sendbuf;

		pHead_req->SlaveAddress = 1;//����IP·������ʶ��ţ�������ͨ��ʱĬ�����ó�1����
		pHead_req->FunctionCode = jsonData["index"].asUInt();
		pHead_req->SequenceNumber = 0;
		pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE);
		int nSize2 = BES200_htons(BEGP_REQ_PACKETDATA_ADD_LOGICAL_POINT_SIZE);

		SendDataSize += BEGP_REQ_HEAD_SIZE;
		BEGP_LOGICAL_POINT_PARAMETER_LDO_T        *pDataLDO_req;
		BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_T  *pData_req;

		pData_req = (BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_T *)(sendbuf + BEGP_REQ_HEAD_SIZE);

		pDataLDO_req = (BEGP_LOGICAL_POINT_PARAMETER_LDO_T*)pData_req;

		pDataLDO_req->PointType = (jsonData["data"]["PointType"].asUInt());  //�������

		pDataLDO_req->ID = BES200_htons(jsonData["data"]["ID"].asUInt());  //�߼����ID
		pDataLDO_req->Active = jsonData["data"]["Active"].asUInt();//�Ƿ�����.

		int nLen = strlen(jsonData["data"]["Name"].asString().c_str());
		memcpy_s(pDataLDO_req->Name, nLen, jsonData["data"]["Name"].asString().c_str(), nLen);


		nLen = strlen(jsonData["data"]["Alias"].asString().c_str());
		memcpy_s(pDataLDO_req->Alias, nLen, jsonData["data"]["Alias"].asString().c_str(), nLen);

		nLen = strlen(jsonData["data"]["Description"].asString().c_str());
		memcpy_s(pDataLDO_req->Description, nLen, jsonData["data"]["Description"].asString().c_str(), nLen);


		pDataLDO_req->ModuleID = BES200_htons(jsonData["data"]["ModuleID"].asUInt());  //������ģ��ID
		pDataLDO_req->ChannelIndex = jsonData["data"]["ChannelIndex"].asUInt();//�����ڵ�ͨ������
		pDataLDO_req->WorkMode = jsonData["data"]["WorkMode"].asUInt(); //����ģʽ
		pDataLDO_req->Polarity = jsonData["data"]["Polarity"].asUInt(); //�Ƿ���
		int nValue = jsonData["data"]["InitValue"].asInt();
		pDataLDO_req->InitValue = m_comFun.BES200_htonl(nValue); //��ʼֵ
		pDataLDO_req->AlarmActive = jsonData["data"]["AlarmActive"].asUInt();//�����Ƿ�����
		pDataLDO_req->AlarmType = jsonData["data"]["AlarmType"].asUInt(); //��������
		pDataLDO_req->AlarmPriority = jsonData["data"]["AlarmPriority"].asUInt();//�������ȼ�
		pDataLDO_req->AlarmTrigger = jsonData["data"]["AlarmTrigger"].asUInt();//��������
		SendDataSize += BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE;
		return SendDataSize;
	}
	return 0;
}

int CCrossDataManager::AddPointVAI(Json::Value jsonData, char *sendbuf)
{
	BEGP_REQ_HEAD_T                                    *pHead_req;
	int  SendDataSize = 0;
	//���������ͷ
	pHead_req = (BEGP_REQ_HEAD_T*)sendbuf;
	pHead_req->SlaveAddress = 1;//����IP·������ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = jsonData["index"].asUInt();
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	BEGP_LOGICAL_POINT_PARAMETER_VAI_T        *pDataVAI_req;

	pDataVAI_req = (BEGP_LOGICAL_POINT_PARAMETER_VAI_T*)(sendbuf + BEGP_REQ_HEAD_SIZE);

	pDataVAI_req->PointType = (GetUint8("PointType", jsonData));  //�������
	pDataVAI_req->ID = BES200_htons(GetUint16("ID", jsonData));  //�߼����ID
	pDataVAI_req->Active = GetUint8("Active", jsonData);//�Ƿ�����

	int nLen = strlen(jsonData["data"]["Name"].asString().c_str());
	memcpy_s(pDataVAI_req->Name, nLen, jsonData["data"]["Name"].asString().c_str(), nLen);


	nLen = strlen(jsonData["data"]["Alias"].asString().c_str());
	memcpy_s(pDataVAI_req->Alias, nLen, jsonData["data"]["Alias"].asString().c_str(), nLen);

	nLen = strlen(jsonData["data"]["Description"].asString().c_str());
	memcpy_s(pDataVAI_req->Description, nLen, jsonData["data"]["Description"].asString().c_str(), nLen);

	pDataVAI_req->InitValue = m_comFun.BES200_htonl(Getint32("InitValue", jsonData));  //��ʼֵ

	nLen = strlen(jsonData["data"]["Unit"].asString().c_str());
	memcpy_s(pDataVAI_req->Unit, nLen, jsonData["data"]["Unit"].asString().c_str(), nLen);//��λ

	pDataVAI_req->Precision = GetUint8("Precision", jsonData);  //����

	/*�������*/
	pDataVAI_req->AlarmActive = GetUint8("AlarmActive", jsonData);//�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
	pDataVAI_req->AlarmType = GetUint8("AlarmType", jsonData);//�������ͣ�����������׼��������ǿ����
	pDataVAI_req->AlarmPriority = GetUint8("AlarmPriority", jsonData);//�������ȼ�

	pDataVAI_req->AlarmHighValue = Getint32("AlarmHighValue", jsonData);
	pDataVAI_req->AlarmLowValue = Getint32("AlarmLowValue", jsonData);

	SendDataSize += BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE;

	return SendDataSize;
}

int CCrossDataManager::AddPointVAO(Json::Value jsonData, char *sendbuf)
{
	BEGP_REQ_HEAD_T                                    *pHead_req;
	int  SendDataSize = 0;
	//���������ͷ
	pHead_req = (BEGP_REQ_HEAD_T*)sendbuf;
	pHead_req->SlaveAddress = 1;//����IP·������ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = jsonData["index"].asUInt();
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	BEGP_LOGICAL_POINT_PARAMETER_VAO_T        *pDataVAO_req;
	pDataVAO_req = (BEGP_LOGICAL_POINT_PARAMETER_VAO_T*)(sendbuf + BEGP_REQ_HEAD_SIZE);

	pDataVAO_req->PointType = (GetUint8("PointType", jsonData));  //�������
	pDataVAO_req->ID = BES200_htons(GetUint16("ID", jsonData));  //�߼����ID
	pDataVAO_req->Active = GetUint8("Active", jsonData);//�Ƿ�����


	int nLen = strlen(jsonData["data"]["Name"].asString().c_str());
	memcpy_s(pDataVAO_req->Name, nLen, jsonData["data"]["Name"].asString().c_str(), nLen);//����


	nLen = strlen(jsonData["data"]["Alias"].asString().c_str());
	memcpy_s(pDataVAO_req->Alias, nLen, jsonData["data"]["Alias"].asString().c_str(), nLen); //����

	nLen = strlen(jsonData["data"]["Description"].asString().c_str());
	memcpy_s(pDataVAO_req->Description, nLen, jsonData["data"]["Description"].asString().c_str(), nLen);//����

	pDataVAO_req->InitValue = m_comFun.BES200_htonl(Getint32("InitValue", jsonData));  //��ʼֵ

	nLen = strlen(jsonData["data"]["Unit"].asString().c_str());
	memcpy_s(pDataVAO_req->Unit, nLen, jsonData["data"]["Unit"].asString().c_str(), nLen);//��λ

	pDataVAO_req->Precision = GetUint8("Precision", jsonData);  //����

	/*�������*/
	pDataVAO_req->AlarmActive = GetUint8("AlarmActive", jsonData);//�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
	pDataVAO_req->AlarmType = GetUint8("AlarmType", jsonData);//�������ͣ�����������׼��������ǿ����
	pDataVAO_req->AlarmPriority = GetUint8("AlarmPriority", jsonData);//�������ȼ�

	pDataVAO_req->AlarmHighValue = Getint32("AlarmHighValue", jsonData);
	pDataVAO_req->AlarmLowValue = Getint32("AlarmLowValue", jsonData);



	SendDataSize += BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE;

	return SendDataSize;
}

int CCrossDataManager::AddPointVDI(Json::Value jsonData, char *sendbuf)
{
	BEGP_REQ_HEAD_T                                    *pHead_req;
	int  SendDataSize = 0;
	//���������ͷ
	pHead_req = (BEGP_REQ_HEAD_T*)sendbuf;
	pHead_req->SlaveAddress = 1;//����IP·������ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = jsonData["index"].asUInt();
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	BEGP_LOGICAL_POINT_PARAMETER_VDI_T        *pDataVDI_req;
	pDataVDI_req = (BEGP_LOGICAL_POINT_PARAMETER_VDI_T*)(sendbuf + BEGP_REQ_HEAD_SIZE);

	pDataVDI_req->PointType = (GetUint8("PointType", jsonData));  //�������
	pDataVDI_req->ID = BES200_htons(GetUint16("ID", jsonData));  //�߼����ID
	pDataVDI_req->Active = GetUint8("Active", jsonData);//�Ƿ�����


	int nLen = strlen(jsonData["data"]["Name"].asString().c_str());
	memcpy_s(pDataVDI_req->Name, nLen, jsonData["data"]["Name"].asString().c_str(), nLen);//����


	nLen = strlen(jsonData["data"]["Alias"].asString().c_str());
	memcpy_s(pDataVDI_req->Alias, nLen, jsonData["data"]["Alias"].asString().c_str(), nLen); //����

	nLen = strlen(jsonData["data"]["Description"].asString().c_str());
	memcpy_s(pDataVDI_req->Description, nLen, jsonData["data"]["Description"].asString().c_str(), nLen);//����

	pDataVDI_req->InitValue = m_comFun.BES200_htonl(Getint32("InitValue", jsonData));  //��ʼֵ


	/*�������*/
	pDataVDI_req->AlarmActive = GetUint8("AlarmActive", jsonData);//�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
	pDataVDI_req->AlarmType = GetUint8("AlarmType", jsonData);//�������ͣ�����������׼��������ǿ����
	pDataVDI_req->AlarmPriority = GetUint8("AlarmPriority", jsonData);//�������ȼ�

	pDataVDI_req->AlarmTrigger = GetUint8("AlarmTrigger", jsonData);

	SendDataSize += BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE;

	return SendDataSize;
}

int CCrossDataManager::AddPointVDO(Json::Value jsonData, char *sendbuf)
{
	BEGP_REQ_HEAD_T                                    *pHead_req;
	int  SendDataSize = 0;
	//���������ͷ
	pHead_req = (BEGP_REQ_HEAD_T*)sendbuf;
	pHead_req->SlaveAddress = 1;//����IP·������ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = jsonData["index"].asUInt();
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	BEGP_LOGICAL_POINT_PARAMETER_VDO_T        *pDataVDO_req;
	pDataVDO_req = (BEGP_LOGICAL_POINT_PARAMETER_VDO_T*)(sendbuf + BEGP_REQ_HEAD_SIZE);

	pDataVDO_req->PointType = (GetUint8("PointType", jsonData));  //�������
	pDataVDO_req->ID = BES200_htons(GetUint16("ID", jsonData));  //�߼����ID
	pDataVDO_req->Active = GetUint8("Active", jsonData);//�Ƿ�����


	int nLen = strlen(jsonData["data"]["Name"].asString().c_str());
	memcpy_s(pDataVDO_req->Name, nLen, jsonData["data"]["Name"].asString().c_str(), nLen);//����


	nLen = strlen(jsonData["data"]["Alias"].asString().c_str());
	memcpy_s(pDataVDO_req->Alias, nLen, jsonData["data"]["Alias"].asString().c_str(), nLen); //����

	nLen = strlen(jsonData["data"]["Description"].asString().c_str());
	memcpy_s(pDataVDO_req->Description, nLen, jsonData["data"]["Description"].asString().c_str(), nLen);//����

	pDataVDO_req->InitValue = m_comFun.BES200_htonl(Getint32("InitValue", jsonData));  //��ʼֵ


	/*�������*/
	pDataVDO_req->AlarmActive = GetUint8("AlarmActive", jsonData);//�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
	pDataVDO_req->AlarmType = GetUint8("AlarmType", jsonData);//�������ͣ�����������׼��������ǿ����
	pDataVDO_req->AlarmPriority = GetUint8("AlarmPriority", jsonData);//�������ȼ�

	pDataVDO_req->AlarmTrigger = GetUint8("AlarmTrigger", jsonData);

	SendDataSize += BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE;

	return SendDataSize;
}

int CCrossDataManager::SetPointValue(Json::Value jsonData, char *sendbuf)
{
	// ���ݸ�ʽͷ
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode =  jsonData["index"].asUInt();
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_T));

	BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_T tmpAdd;
	tmpAdd.ID = BES200_htons( jsonData["data"]["ID"].asUInt());
	tmpAdd.WorkMode =  jsonData["data"]["WorkMode"].asUInt();
	uint16_t uValue = jsonData["data"]["Value"].asUInt();
	tmpAdd.Value = m_comFun.BES200_htonl(uValue);
	// Э��ͷ
	memcpy(sendbuf, &thead, sizeof(thead));
	// ����
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::SetPointValueByName(Json::Value jsonData, char *sendbuf)
{
	// ���ݸ�ʽͷ
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode =  jsonData["index"].asUInt();
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_BY_NAME_T));
	BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_BY_NAME_T tmpAdd;
	strcpy_s(tmpAdd.Name, jsonData["data"]["Name"].asString().c_str());
	
	tmpAdd.Value = m_comFun.BES200_htonl( jsonData["data"]["Value"].asUInt());
	// Э��ͷ
	memcpy(sendbuf, &thead, sizeof(thead));
	// ����
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::DeletePointByID(Json::Value jsonData, char *sendbuf)
{
	// ���ݸ�ʽͷ
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode =  jsonData["index"].asUInt();
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_PACKETDATA_DELETE_LOGICAL_POINT_T));
	BEGP_REQ_PACKETDATA_DELETE_LOGICAL_POINT_T tmpAdd;
	tmpAdd.ID = m_comFun.BES200_htons( jsonData["data"]["ID"].asUInt());
	// Э��ͷ
	memcpy(sendbuf, &thead, sizeof(thead));
	// ����
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::GetPointParameter(Json::Value jsonData, char * sendbuf)
{
	// ���ݸ�ʽͷ
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode =  jsonData["index"].asUInt();
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_T));
	BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_T tmpAdd;
	tmpAdd.ID = m_comFun.BES200_htons( jsonData["data"]["ID"].asUInt());
	// Э��ͷ
	memcpy(sendbuf, &thead, sizeof(thead));
	// ����
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::GetPointErrorCode(Json::Value jsonData, char * sendbuf)
{
	// ���ݸ�ʽͷ
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode =  jsonData["index"].asUInt();
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T));
	BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T tmpAdd;
	tmpAdd.ID = m_comFun.BES200_htons( jsonData["data"]["ID"].asUInt());
	// Э��ͷ
	memcpy(sendbuf, &thead, sizeof(thead));
	// ����
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::GetPointAlarmErrorCode(Json::Value jsonData, char *sendbuf)
{
	// ���ݸ�ʽͷ
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode =  jsonData["index"].asUInt();
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T));
	BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T tmpAdd;
	tmpAdd.ID = m_comFun.BES200_htons( jsonData["data"]["ID"].asUInt());
	// Э��ͷ
	memcpy(sendbuf, &thead, sizeof(thead));
	// ����
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::AddScheduler(Json::Value jsonData, char * sendbuf)
{
	memset(sendbuf, strlen(sendbuf), 0);
	BEGP_REQ_HEAD_T* pHead_req;
	int  SendDataSize = 0;
	//���������ͷ
	pHead_req = (BEGP_REQ_HEAD_T*)sendbuf;
	pHead_req->SlaveAddress = 1;//����IP·������ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = jsonData["index"].asUInt();
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(sizeof(BEGP_REQ_PACKETDATA_ADD_SCHEDULER_T));
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	BEGP_REQ_PACKETDATA_ADD_SCHEDULER_T* pDataVDO_req;
	pDataVDO_req = (BEGP_REQ_PACKETDATA_ADD_SCHEDULER_T*)(sendbuf + BEGP_REQ_HEAD_SIZE);

	SendDataSize += sizeof(BEGP_REQ_PACKETDATA_ADD_SCHEDULER_T);


	// ���ݸ�ʽͷ
	// BEGP_REQ_HEAD_T thead;
	// thead.FunctionCode =  jsonData["index"].asUInt();
	// thead.SequenceNumber = 0;
	// thead.SlaveAddress = 1;
	// thead.DataSize = BES200_htons(sizeof(BEGP_REQ_PACKETDATA_ADD_SCHEDULER_T));
	// BEGP_REQ_PACKETDATA_ADD_SCHEDULER_T tmpAdd;


	pDataVDO_req->ID = BES200_htons( jsonData["data"]["ID"].asUInt());


	pDataVDO_req->Active =  jsonData["data"]["Active"].asUInt();
	//strcpy_s(tmpAdd.Name, jsonData["data"]["Name"].asString().c_str());
	//strcpy_s(tmpAdd.Alias, jsonData["data"]["Alias"].asString().c_str());


	memcpy_s(pDataVDO_req->Name, 32, jsonData["data"]["Name"].asString().c_str(), strlen(jsonData["data"]["Name"].asString().c_str()));
	memcpy_s(pDataVDO_req->Alias, 64, jsonData["data"]["Alias"].asString().c_str(), strlen(jsonData["data"]["Alias"].asString().c_str()));

	pDataVDO_req->PlanType =  jsonData["data"]["PlanType"].asUInt();
	pDataVDO_req->StartDateYear =  jsonData["data"]["StartDateYear"].asUInt();
	pDataVDO_req->StartDateMonth =  jsonData["data"]["StartDateMonth"].asUInt();
	pDataVDO_req->StartDateDay =  jsonData["data"]["StartDateDay"].asUInt();
	pDataVDO_req->EndDateYear =  jsonData["data"]["EndDateYear"].asUInt();
	pDataVDO_req->EndDateMonth =  jsonData["data"]["EndDateMonth"].asUInt();
	pDataVDO_req->EndDateDay =  jsonData["data"]["EndDateDay"].asUInt();

	pDataVDO_req->StartTimeHour =  jsonData["data"]["StartTimeHour"].asUInt();
	pDataVDO_req->StartTimeMinute =  jsonData["data"]["StartTimeMinute"].asUInt();
	pDataVDO_req->StartTimeSecond =  jsonData["data"]["StartTimeSecond"].asUInt();
	pDataVDO_req->EndTimeHour =  jsonData["data"]["EndTimeHour"].asUInt();
	pDataVDO_req->EndTimeMinute =  jsonData["data"]["EndTimeMinute"].asUInt();
	pDataVDO_req->EndTimeSecond =  jsonData["data"]["EndTimeSecond"].asUInt();

	pDataVDO_req->ExecutionWay =  jsonData["data"]["ExecutionWay"].asUInt();
	pDataVDO_req->WeekMask =  jsonData["data"]["WeekMask"].asUInt();
	pDataVDO_req->SceneType =  jsonData["data"]["SceneType"].asUInt();
	pDataVDO_req->SceneID = BES200_htons(jsonData["data"]["SceneID"].asUInt());
	pDataVDO_req->ModeID =  jsonData["data"]["ModeID"].asUInt();
	pDataVDO_req->DefaultModeID =  jsonData["data"]["DefaultModeID"].asUInt();

	// Э��ͷ
//	memcpy(sendbuf, &thead, sizeof(thead));
//	// ����
//	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
//	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return SendDataSize;
}

int CCrossDataManager::SetScheduler(Json::Value jsonData, char * sendbuf)
{
	return AddScheduler(jsonData, sendbuf);
	// ���ݸ�ʽͷ
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode =  jsonData["index"].asUInt();
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_PACKETDATA_SET_SCHEDULER_PARAMETER_T));
	BEGP_REQ_PACKETDATA_SET_SCHEDULER_PARAMETER_T tmpAdd;
	tmpAdd.ID = m_comFun.BES200_htons( jsonData["data"]["ID"].asUInt());
	tmpAdd.SceneID = m_comFun.BES200_htons( jsonData["data"]["SceneID"].asUInt());

	tmpAdd.Active =  jsonData["data"]["Active"].asUInt();
	//strcpy_s(tmpAdd.Name, jsonData["data"]["Name"].asString().c_str());
	//strcpy_s(tmpAdd.Alias, jsonData["data"]["Alias"].asString().c_str());

	memcpy_s(tmpAdd.Name, 32, jsonData["data"]["Name"].asString().c_str(), strlen(jsonData["data"]["Name"].asString().c_str()));
	memcpy_s(tmpAdd.Alias, 64, jsonData["data"]["Alias"].asString().c_str(), strlen(jsonData["data"]["Alias"].asString().c_str()));


	tmpAdd.PlanType =  jsonData["data"]["PlanType"].asUInt();
	tmpAdd.StartDateYear =  jsonData["data"]["StartDateYear"].asUInt();
	tmpAdd.StartDateMonth =  jsonData["data"]["StartDateMonth"].asUInt();
	tmpAdd.StartDateDay =  jsonData["data"]["StartDateDay"].asUInt();
	tmpAdd.EndDateYear =  jsonData["data"]["EndDateYear"].asUInt();
	tmpAdd.EndDateMonth =  jsonData["data"]["EndDateMonth"].asUInt();
	tmpAdd.EndDateDay =  jsonData["data"]["EndDateDay"].asUInt();

	tmpAdd.StartTimeHour =  jsonData["data"]["StartTimeHour"].asUInt();
	tmpAdd.StartTimeMinute =  jsonData["data"]["StartTimeMinute"].asUInt();
	tmpAdd.StartTimeSecond =  jsonData["data"]["StartTimeSecond"].asUInt();
	tmpAdd.EndTimeHour =  jsonData["data"]["EndTimeHour"].asUInt();
	tmpAdd.EndTimeMinute =  jsonData["data"]["EndTimeMinute"].asUInt();
	tmpAdd.EndTimeSecond =  jsonData["data"]["EndTimeSecond"].asUInt();

	tmpAdd.ExecutionWay =  jsonData["data"]["ExecutionWay"].asUInt();
	tmpAdd.WeekMask =  jsonData["data"]["WeekMask"].asUInt();
	tmpAdd.SceneType =  jsonData["data"]["SceneType"].asUInt();
	tmpAdd.SceneID =  jsonData["data"]["SceneID"].asUInt();
	tmpAdd.ModeID =  jsonData["data"]["ModeID"].asUInt();
	tmpAdd.DefaultModeID =  jsonData["data"]["DefaultModeID"].asUInt();


	// Э��ͷ
	memcpy(sendbuf, &thead, sizeof(thead));
	// ����
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::DelScheduler(Json::Value jsonData, char * sendbuf)
{
	// ���ݸ�ʽͷ
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode =  jsonData["index"].asUInt();
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_PACKETDATA_DELETE_SCHEDULER_T));
	BEGP_REQ_PACKETDATA_DELETE_SCHEDULER_T tmpAdd;
	tmpAdd.ID = m_comFun.BES200_htons( jsonData["data"]["ID"].asUInt());

	// Э��ͷ
	memcpy(sendbuf, &thead, sizeof(thead));
	// ����
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::GetScheduler(Json::Value jsonData, char * sendbuf)
{
	// ���ݸ�ʽͷ
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode =  jsonData["index"].asUInt();
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons(sizeof(BEGP_REQ_PACKETDATA_GET_SCHEDULER_PARAMETER_T));
	BEGP_REQ_PACKETDATA_GET_SCHEDULER_PARAMETER_T tmpAdd;
	tmpAdd.ID = m_comFun.BES200_htons( jsonData["data"]["ID"].asUInt());
	// Э��ͷ
	memcpy(sendbuf, &thead, sizeof(thead));
	// ����
	memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return nSendSize;
}

int CCrossDataManager::AddScene(Json::Value jsonData, char * sendbuf)
{
	//���ֳ������ɼ����������Ƴ���
	// ���Ƴ���
	// ���ݸ�ʽͷ

	uint8_t uIndex = 0;
	if (jsonData["index"].isString())
	{
		uIndex = atoi(jsonData["index"].asString().c_str());
	}
	else {
		uIndex = jsonData["index"].asUInt();
		//	uT = jsonData["data"][strKey].asInt();
	}


	int nSendSize = 0;
	BEGP_REQ_HEAD_T *pthead = (BEGP_REQ_HEAD_T *) sendbuf;
	pthead->FunctionCode = uIndex;
	pthead->SequenceNumber = 0;
	pthead->SlaveAddress = 1;
	pthead->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_ADD_SCENE_SIZE);
	nSendSize = sizeof(BEGP_REQ_HEAD_T) + BEGP_REQ_PACKETDATA_ADD_SCENE_SIZE;
	int nSceneType = GetUint8("SceneType", jsonData);// jsonData["data"]["SceneType"].asUInt();

	

	if (nSceneType == 0 )//���Ƴ���
	{
		BEGP_CONTROL_SCENE_PARAMETER_T *lp = (BEGP_CONTROL_SCENE_PARAMETER_T *)(sendbuf + sizeof(BEGP_REQ_HEAD_T));
		int nskkk = sizeof(BEGP_REQ_HEAD_T);
	//	pthead->DataSize = BES200_htons(sizeof(BEGP_CONTROL_SCENE_PARAMETER_T));
		
	//	string strs = jsonData["data"]["data"]["SceneType"].asString();
	//	int skkk = jsonData["data"]["data"]["SceneType"].asUInt();
		uint8_t uSceneType = 0;
		if (jsonData["data"]["data"]["SceneType"].isString())
		{
			uSceneType = atoi(jsonData["data"]["data"]["SceneType"].asString().c_str());
		}
		else {
			uSceneType = jsonData["data"]["data"]["SceneType"].asUInt();
			//	uT = jsonData["data"][strKey].asInt();
		}


		lp->SceneType = uSceneType;
		int uID = 0;
		if (jsonData["data"]["data"]["ID"].isString())
		{
			uID = atoi(jsonData["data"]["data"]["ID"].asString().c_str());
		}
		else {
			uID = jsonData["data"]["data"]["ID"].asUInt();
			//	uT = jsonData["data"][strKey].asInt();
		}

		lp->ID = BES200_htons(uID);

		uint8_t uActive = 0;
		if (jsonData["data"]["data"]["Active"].isString())
		{
			uActive = atoi(jsonData["data"]["data"]["Active"].asString().c_str());
		}
		else {
			uActive = jsonData["data"]["data"]["Active"].asUInt();
			//	uT = jsonData["data"][strKey].asInt();
		}

		lp->Active = uActive;
	/*	strcpy_s(lp->Name, jsonData["data"]["data"]["Name"].asString().c_str());
		strcpy_s(lp->Alias, jsonData["data"]["data"]["Alias"].asString().c_str());
*/
		memcpy_s(lp->Name, 32, jsonData["data"]["data"]["Name"].asString().c_str(), strlen(jsonData["data"]["data"]["Name"].asString().c_str()));
		memcpy_s(lp->Alias, 64, jsonData["data"]["data"]["Alias"].asString().c_str(), strlen(jsonData["data"]["data"]["Alias"].asString().c_str()));


		int nSizeCollectMode = 0;
		if (jsonData["data"]["data"]["CollectMode"].isArray())
		{
			nSizeCollectMode = jsonData["data"]["data"]["CollectMode"].size();
		}

	//	memset(lp->ControlMode, 0, sizeof(lp->ControlMode));
		for (int i = 0; i< nSizeCollectMode;i++)
		{
			Json::Value vaMode = jsonData["data"]["data"]["CollectMode"];
			Json::Value vaModeItem = vaMode[i];
			//vaMode.get(i, vaModeItem);

			lp->ControlMode[i].Active = vaModeItem["Active"].asUInt();
			uint8_t uMID = 0;
			if (jsonData["data"]["data"]["CollectMode"][i]["ID"].isString())
			{
				uMID = atoi(jsonData["data"]["data"]["CollectMode"][i]["ID"].asString().c_str());
			}
			else {
				uMID = jsonData["data"]["data"]["CollectMode"][i]["ID"].asUInt();
				//	uT = jsonData["data"][strKey].asInt();
			}


		//	int nID = jsonData["data"]["data"]["CollectMode"][i]["ID"].asUInt();


			lp->ControlMode[i].ID = uMID;

		

			memcpy_s(lp->ControlMode[i].Name, 32, vaModeItem["Name"].asString().c_str(), strlen(vaModeItem["Name"].asString().c_str()));

		

			int nSizeCollectPoint = 0;
			if (vaModeItem["CollectPoint"].isArray())
			{
				nSizeCollectPoint = vaModeItem["CollectPoint"].size();
				Json::Value vaPoint = vaModeItem["CollectPoint"];
				for (int j = 0;j < nSizeCollectPoint;j++)
				{
					Json::Value vaPointItem = vaPoint[j]; 
					uint8_t uMActive = 0;
					if (vaPointItem["Active"].isString())
					{
						uMActive = atoi(vaPointItem["Active"].asString().c_str());
					}
					else {
						uMActive = vaPointItem["Active"].asUInt();
						//	uT = jsonData["data"][strKey].asInt();
					}


					lp->ControlMode[i].ControlPoint[j].Active = uMActive;// vaPointItem["Active"].asUInt();

					uint8_t uMPointID = 0;
					if (vaPointItem["PointID"].isString())
					{
						uMPointID = atoi(vaPointItem["PointID"].asString().c_str());
					}
					else {
						uMPointID = vaPointItem["PointID"].asUInt();
						//	uT = jsonData["data"][strKey].asInt();
					}


					lp->ControlMode[i].ControlPoint[j].PointID = BES200_htons(uMPointID);//; BES200_htons( vaPointItem["PointID"].asUInt());


					uint8_t uMRunValue = 0;
					if (vaPointItem["PointID"].isString())
					{
						uMRunValue = atoi(vaPointItem["RunValue"].asString().c_str());
					}
					else {
						uMRunValue = vaPointItem["RunValue"].asUInt();
						//	uT = jsonData["data"][strKey].asInt();
					}


					lp->ControlMode[i].ControlPoint[j].RunValue = m_comFun.BES200_htonl(uMRunValue);
		
				}
			}
		}
	}
	else if (nSceneType == 1)// �ɼ�����
	{
		BEGP_COLLECT_SCENE_PARAMETER_T *lp = (BEGP_COLLECT_SCENE_PARAMETER_T *)(sendbuf + sizeof(BEGP_REQ_HEAD_T));
		//pthead->DataSize = BES200_htons(sizeof(BEGP_COLLECT_SCENE_PARAMETER_T));
	
		//nSendSize = sizeof(BEGP_REQ_HEAD_T) + sizeof(BEGP_COLLECT_SCENE_PARAMETER_T);
		lp->SceneType =  jsonData["data"]["data"]["SceneType"].asUInt();
		lp->ID = BES200_htons(jsonData["data"]["data"]["ID"].asUInt());
		lp->Active =  jsonData["data"]["data"]["Active"].asUInt();
		//strcpy_s(lp->Name, jsonData["data"]["data"]["Name"].asString().c_str());
		//strcpy_s(lp->Alias, jsonData["data"]["data"]["Alias"].asString().c_str());

		memcpy_s(lp->Name, 32, jsonData["data"]["Name"].asString().c_str(), strlen(jsonData["data"]["Name"].asString().c_str()));
		memcpy_s(lp->Alias, 64, jsonData["data"]["Alias"].asString().c_str(), strlen(jsonData["data"]["Alias"].asString().c_str()));


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
			int nID = jsonData["data"]["data"]["CollectMode"][i]["ID"].asUInt();
			lp->CollectMode[i].ID = nID;
		//	strcpy_s(lp->CollectMode[i].Name, vaModeItem["Active"].asString().c_str());


			memcpy_s(lp->CollectMode[i].Name, 32, vaModeItem["Active"].asString().c_str(), strlen(vaModeItem["Active"].asString().c_str()));


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

int CCrossDataManager::SetScene(Json::Value jsonData, char * sendbuf)
{
	return AddScene(jsonData, sendbuf);
}

int CCrossDataManager::DelScene(Json::Value jsonData, char * sendbuf)
{
	BEGP_REQ_HEAD_T* pHead_req;
	BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_T* pData_req;
	int SendDataSize = 0;
	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)sendbuf;
	//�������ݣ�ָ�븳ֵ
	pData_req = (BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_T*)(sendbuf + BEGP_REQ_HEAD_SIZE);


	//���������ͷ
	pHead_req->SlaveAddress = 1;//����IP·������ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = jsonData["index"].asUInt();
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_DELETE_SCENE_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	pData_req->ID = BES200_htons(jsonData["data"]["ID"].asUInt());

	SendDataSize += BEGP_REQ_PACKETDATA_DELETE_SCENE_SIZE;



	//// ���ݸ�ʽͷ
	//BEGP_REQ_HEAD_T thead;
	//thead.FunctionCode =  jsonData["index"].asUInt();
	//thead.SequenceNumber = 0;
	//thead.SlaveAddress = 1;
	//thead.DataSize = BES200_htons(BEGP_REQ_PACKETDATA_DELETE_SCENE_SIZE);
	//BEGP_REQ_PACKETDATA_DELETE_SCENE_T tmpAdd;
	//tmpAdd.ID = m_comFun.BES200_htons( jsonData["data"]["ID"].asUInt());

	//// Э��ͷ
	//memcpy(sendbuf, &thead, sizeof(thead));
	//// ����
	//memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	//int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return SendDataSize;
}

int CCrossDataManager::GetScene(Json::Value jsonData, char * sendbuf)
{
	BEGP_REQ_HEAD_T* pHead_req;
	BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_T* pData_req;
	int SendDataSize = 0;
	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)sendbuf;
	//�������ݣ�ָ�븳ֵ
	pData_req = (BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_T*)(sendbuf + BEGP_REQ_HEAD_SIZE);


	//���������ͷ
	pHead_req->SlaveAddress = 1;//����IP·������ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = jsonData["index"].asUInt();
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	pData_req->ID = BES200_htons(jsonData["data"]["ID"].asUInt());

	SendDataSize += BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_SIZE;

	
	//// ���ݸ�ʽͷ
	//BEGP_REQ_HEAD_T thead;
	//thead.FunctionCode =  jsonData["index"].asUInt();
	//thead.SequenceNumber = 0;
	//thead.SlaveAddress = 1;
	//thead.DataSize = BES200_htons(sizeof(BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_T));
	//BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_T tmpAdd;
	//tmpAdd.ID = BES200_htons( jsonData["data"]["ID"].asUInt());

	//// Э��ͷ
	//memcpy(sendbuf, &thead, sizeof(thead));
	//// ����
	//memcpy(sendbuf + sizeof(thead), &tmpAdd, sizeof(tmpAdd));
	//int nSendSize = sizeof(thead) + sizeof(tmpAdd);
	return SendDataSize;
}

int CCrossDataManager::UpDate(Json::Value jsonData, char* sendbuf)
{
	// ���ݸ�ʽͷ
	BEGP_REQ_HEAD_T thead;
	thead.FunctionCode =  jsonData["index"].asUInt();
	thead.SequenceNumber = 0;
	thead.SlaveAddress = 1;
	thead.DataSize = BES200_htons(0);	
	// Э��ͷ
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
	uint16_t nID = m_comFun.BES200_htons(lpVData->ID);
	jsonResultData["ID"] = Json::Value(nID);
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
	uint16_t nID = m_comFun.BES200_htons(lpVData->ID);
	jsonResultData["ID"] = Json::Value(nID);
	return jsonResultData;
}

Json::Value CCrossDataManager::ParseGetPointPar(void *pRecvData)
{
	Json::Value jsonResultData;
	BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_T *lpVData = (BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_T *)pRecvData;
	int nType = lpVData->PointType;
	uint8_t uType = lpVData->PointType;
	int nType2 = BES200_htons(nType);
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

		//int32_t uvalue = m_comFun.BES200_htonl2(lpD->InitValue);

		//int32_t uvalue2 = m_comFun.BES200_htonl2(lpD->HighRange);
		//int32_t uvalue3 = m_comFun.BES200_htonl2(lpD->LowRange);
		int uV = m_comFun.BES200_htonl(lpD->InitValue);
		if (uV > 900000 || uV < -900000)
		{
			uV = 0;
		}
		jsonResultData["InitValue"] = Json::Value(uV);
		jsonResultData["AlarmActive"] = Json::Value(lpD->AlarmActive);
		jsonResultData["AlarmType"] = Json::Value(lpD->AlarmType);
		jsonResultData["AlarmPriority"] = Json::Value(lpD->AlarmPriority);
		jsonResultData["LineType"] = Json::Value(lpD->LineType);
		jsonResultData["Unit"] = Json::Value(lpD->Unit);
		jsonResultData["HighRange"] = Json::Value(m_comFun.BES200_htonl2(lpD->HighRange));
		jsonResultData["LowRange"] = Json::Value(m_comFun.BES200_htonl2(lpD->LowRange));
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

		int uV = m_comFun.BES200_htonl(lpD->InitValue);
		if (uV > 900000 || uV < -900000)
		{
			uV = 0;
		}
		jsonResultData["InitValue"] = Json::Value(uV);

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

		int uV = m_comFun.BES200_htonl(lpD->InitValue);
		if (uV > 900000 || uV < -900000)
		{
			uV = 0;
		}
		jsonResultData["InitValue"] = Json::Value(uV);

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

		int uV = m_comFun.BES200_htonl(lpD->InitValue);
		if (uV > 900000 || uV < -900000)
		{
			uV = 0;
		}
		jsonResultData["InitValue"] = Json::Value(uV);

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


		int uV = m_comFun.BES200_htonl(lpD->InitValue);
		if (uV > 900000 || uV < -900000)
		{
			uV = 0;
		}
		jsonResultData["InitValue"] = Json::Value(uV);


		jsonResultData["AlarmActive"] = Json::Value(lpD->AlarmActive);
		jsonResultData["AlarmType"] = Json::Value(lpD->AlarmType);
		jsonResultData["AlarmPriority"] = Json::Value(lpD->AlarmPriority);

		jsonResultData["AlarmHighValue"] = Json::Value(lpD->AlarmHighValue);// ���� �ǲ�����ҪBES200_htonl
		jsonResultData["AlarmLowValue"] = Json::Value(lpD->AlarmLowValue);// ���� �ǲ�����ҪBES200_htonl
	}
	else if (lpVData->PointType == POINT_TYPE_VIRTUAL_AO) {
		BEGP_LOGICAL_POINT_PARAMETER_VAO_T *lpD = (BEGP_LOGICAL_POINT_PARAMETER_VAO_T*)lpVData;

		jsonResultData["PointType"] = Json::Value(lpD->PointType);
		jsonResultData["ID"] = Json::Value(BES200_htons(lpD->ID));
		jsonResultData["Active"] = Json::Value(lpD->Active);
		jsonResultData["Name"] = Json::Value(lpD->Name);
		jsonResultData["Alias"] = Json::Value(lpD->Alias);
		jsonResultData["Description"] = Json::Value(lpD->Description);


		int uV = m_comFun.BES200_htonl(lpD->InitValue);
		if (uV > 900000 || uV < -900000)
		{
			uV = 0;
		}
		jsonResultData["InitValue"] = Json::Value(uV);



		jsonResultData["Unit"] = Json::Value(lpD->Unit);
		jsonResultData["Precision"] = Json::Value(lpD->Precision);

		jsonResultData["AlarmActive"] = Json::Value(lpD->AlarmActive);
		jsonResultData["AlarmType"] = Json::Value(lpD->AlarmType);
		jsonResultData["AlarmPriority"] = Json::Value(lpD->AlarmPriority);

		jsonResultData["AlarmHighValue"] = Json::Value(lpD->AlarmHighValue);// ���� �ǲ�����ҪBES200_htonl
		jsonResultData["AlarmLowValue"] = Json::Value(lpD->AlarmLowValue);// ���� �ǲ�����ҪBES200_htonl
	}
	else if (lpVData->PointType == POINT_TYPE_VIRTUAL_AO) {
		BEGP_LOGICAL_POINT_PARAMETER_VDI_T *lpD = (BEGP_LOGICAL_POINT_PARAMETER_VDI_T*)lpVData;

		jsonResultData["PointType"] = Json::Value(lpD->PointType);
		jsonResultData["ID"] = Json::Value(BES200_htons(lpD->ID));
		jsonResultData["Active"] = Json::Value(lpD->Active);
		jsonResultData["Name"] = Json::Value(lpD->Name);
		jsonResultData["Alias"] = Json::Value(lpD->Alias);
		jsonResultData["Description"] = Json::Value(lpD->Description);


		int uV = m_comFun.BES200_htonl(lpD->InitValue);
		if (uV > 900000 || uV < -900000)
		{
			uV = 0;
		}
		jsonResultData["InitValue"] = Json::Value(uV);


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


		int uV = m_comFun.BES200_htonl(lpD->InitValue);
		if (uV > 900000 || uV < -900000)
		{
			uV = 0;
		}
		jsonResultData["InitValue"] = Json::Value(uV);


		jsonResultData["AlarmActive"] = Json::Value(lpD->AlarmActive);
		jsonResultData["AlarmType"] = Json::Value(lpD->AlarmType);
		jsonResultData["AlarmPriority"] = Json::Value(lpD->AlarmPriority);

		jsonResultData["AlarmTrigger"] = Json::Value(lpD->AlarmPriority);

	}
	else if (lpVData->PointType == POINT_TYPE_VIRTUAL_DI) {
		BEGP_LOGICAL_POINT_PARAMETER_VDI_T *lpD = (BEGP_LOGICAL_POINT_PARAMETER_VDI_T*)lpVData;

		jsonResultData["PointType"] = Json::Value(lpD->PointType);
		jsonResultData["ID"] = Json::Value(BES200_htons(lpD->ID));
		jsonResultData["Active"] = Json::Value(lpD->Active);
		jsonResultData["Name"] = Json::Value(lpD->Name);
		jsonResultData["Alias"] = Json::Value(lpD->Alias);
		jsonResultData["Description"] = Json::Value(lpD->Description);


		int uV = m_comFun.BES200_htonl(lpD->InitValue);
		if (uV > 900000 || uV < -900000)
		{
			uV = 0;
		}
		jsonResultData["InitValue"] = Json::Value(uV);


		jsonResultData["AlarmActive"] = Json::Value(lpD->AlarmActive);
		jsonResultData["AlarmType"] = Json::Value(lpD->AlarmType);
		jsonResultData["AlarmPriority"] = Json::Value(lpD->AlarmPriority);

		jsonResultData["AlarmTrigger"] = Json::Value(lpD->AlarmPriority);

	}
	else {
	// ��������
	}
	return jsonResultData;
}

Json::Value CCrossDataManager::ParseGetPointAlarm(void *pRecvData)
{
	Json::Value jsonResultData;
	BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T *lpVData = (BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T*)pRecvData;
	jsonResultData["CurrentValue"] = Json::Value(m_comFun.BES200_htonl(lpVData->CurrentValue));
	jsonResultData["AlarmState"] = Json::Value(lpVData->AlarmState);
	jsonResultData["AlarmTriggerValue"] = Json::Value(m_comFun.BES200_htonl(lpVData->AlarmTriggerValue));
	jsonResultData["AlarmMaxValue"] = Json::Value(m_comFun.BES200_htonl(lpVData->AlarmMaxValue));
	return jsonResultData;
}

Json::Value CCrossDataManager::ParseGetSence(void *pRecvData)
{
	Json::Value jsonResultData;
	BEGP_RSP_PACKETDATA_GET_SCENE_PARAMETER_T *lpVData = (BEGP_RSP_PACKETDATA_GET_SCENE_PARAMETER_T*)pRecvData;
	int nType = lpVData->SceneType;
	if (nType == 0 )// ���Ƴ���
	{
		BEGP_CONTROL_SCENE_PARAMETER_T* pData = (BEGP_CONTROL_SCENE_PARAMETER_T *)pRecvData;
		jsonResultData["SceneType"] = Json::Value(pData->SceneType);
		uint16_t nID = m_comFun.BES200_htons(pData->ID);
		jsonResultData["ID"] = Json::Value(nID);

		//jsonResultData["ID"] = Json::Value(pData->ID);
		jsonResultData["Active"] = Json::Value(pData->Active);
		jsonResultData["Name"] = Json::Value(pData->Name);
		jsonResultData["Alias"] = Json::Value(pData->Alias);
		for (int i = 0;i < CONTROL_MODE_SIZE; i++) {
			jsonResultData["CollectMode"][i]["Active"] = Json::Value(pData->ControlMode[i].Active);
			jsonResultData["CollectMode"][i]["Name"] = Json::Value(pData->ControlMode[i].Name);
			
			uint16_t nID = pData->ControlMode[i].ID;
			jsonResultData["CollectMode"][i]["ID"] = Json::Value(nID);

			for (int j = 0; j < CONTROL_POINT_SIZE; j++)
			{
				jsonResultData["CollectMode"][i]["CollectPoint"][j]["Active"] = Json::Value(pData->ControlMode[i].ControlPoint[j].Active);
				jsonResultData["CollectMode"][i]["CollectPoint"][j]["PointID"] = Json::Value(BES200_htons(pData->ControlMode[i].ControlPoint[j].PointID));
				jsonResultData["CollectMode"][i]["CollectPoint"][j]["RunValue"] = Json::Value(pData->ControlMode[i].ControlPoint[j].RunValue);
			}
		}
	}
	else if (nType == 1) // �ɼ�����
	{
		BEGP_COLLECT_SCENE_PARAMETER_T* pData = (BEGP_COLLECT_SCENE_PARAMETER_T *)pRecvData;
		jsonResultData["SceneType"] = Json::Value(pData->SceneType);

		uint16_t nID = m_comFun.BES200_htons(pData->ID);
	
		jsonResultData["ID"] = Json::Value(nID);
		jsonResultData["Active"] = Json::Value(pData->Active);
		jsonResultData["Name"] = Json::Value(pData->Name);
		jsonResultData["Alias"] = Json::Value(pData->Alias);

		for (int i = 0;i < CONTROL_MODE_SIZE; i++) {

			jsonResultData["CollectMode"][i]["Active"] = Json::Value(pData->CollectMode[i].Active);
			jsonResultData["CollectMode"][i]["Name"] = Json::Value(pData->CollectMode[i].Name);
			uint16_t nID = pData->CollectMode[i].ID;
			jsonResultData["CollectMode"][i]["ID"] = Json::Value(nID);
			for (int j = 0; j < CONTROL_POINT_SIZE; j++)
			{
				jsonResultData["CollectMode"][i]["CollectPoint"][j]["Active"] = Json::Value(pData->CollectMode[i].CollectPoint[j].Active);
				jsonResultData["CollectMode"][i]["CollectPoint"][j]["PointID"] = Json::Value(BES200_htons(pData->CollectMode[i].CollectPoint[j].PointID));
				jsonResultData["CollectMode"][i]["CollectPoint"][j]["CollectID"] = Json::Value(pData->CollectMode[i].CollectPoint[j].CollectID);
				jsonResultData["CollectMode"][i]["CollectPoint"][j]["TimeUnit"] = Json::Value(pData->CollectMode[i].CollectPoint[j].TimeUnit);
				jsonResultData["CollectMode"][i]["CollectPoint"][j]["Value"] = Json::Value(pData->CollectMode[i].CollectPoint[j].Value);
			}
		}
	}
	else{
		// ��������
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
//	jsonResultData["ID"] = Json::Value(lpVData->);
	return jsonResultData;
}

uint8_t CCrossDataManager::GetUint8(string strKey, Json::Value jsonData)
{
	uint8_t uT = 0;
	if (jsonData["data"][strKey].isString())
	{
		uT = atoi(jsonData["data"][strKey].asString().c_str());
	}
	else {
		uT = jsonData["data"][strKey].asUInt();
	//	uT = jsonData["data"][strKey].asInt();
	}
	return uT;
}

int32_t CCrossDataManager::Getint32(string strKey, Json::Value jsonData)
{
	int32_t uT = 0;
	if (jsonData["data"][strKey].isString())
	{
		uT = atoi(jsonData["data"][strKey].asString().c_str());
	}
	else {
		uT = jsonData["data"][strKey].asInt();
		//	uT = jsonData["data"][strKey].asInt();
	}
	return uT;
}

uint32_t CCrossDataManager::GetUint32(string strKey, Json::Value jsonData)
{
	uint32_t uT = 0;
	if (jsonData["data"][strKey].isString())
	{
		uT = atoi(jsonData["data"][strKey].asString().c_str());
	}
	else {
		uT = jsonData["data"][strKey].asUInt();
	//	uT = jsonData["data"][strKey].asInt();
	}
	return uT;
}

uint16_t CCrossDataManager::GetUint16(string strKey, Json::Value jsonData)
{
	uint16_t uT = 0;
	if (jsonData["data"][strKey].isString())
	{
		uT = atoi(jsonData["data"][strKey].asString().c_str());	
	}
	else {
		uT = jsonData["data"][strKey].asUInt();
	//	uT = jsonData["data"][strKey].asInt();
	}
	return uT;
}
