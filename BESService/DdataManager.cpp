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
#include "json.hpp" //  ������
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
		// ��Ϊ��˵�� �����ܺĵ� Ҳ�Ͳ��ü����˰�

		return "";
	}
	
//	LOG_INFO(this_thread::get_id() << "��ʼ�����ܺ����ݣ�");
	switch (nIndex)
	{
	case 0://����һ��������
		jsonResult = AddCollectControllerParameter(root);
		break;
	case 1:// ͬ��
		jsonResult = SetCollectControllerParameter(root);
		//	jsonResult = SetCollectControllerParameter2(root);
		break;
	case 2:	// ����ʱ��
		jsonResult = SetCollectControllerTime(root);
		break;
	case 3://����DDC,�൱�������λ
		jsonResult = RebootCollectController(root);
		break;
	case 4://���ÿ��������ָ��������ã�������
		jsonResult = ResetCollectControler(root);
		break;
	case 5:// ��ȡʱ��
		jsonResult = GetCollectControllerTime(root);
		break;
	case 6:// �Ա� ��ȡ���������������ò���
		jsonResult = GetCollectControllerParameter(root);
		break;
	case 7://�㲥��ʽ������������ͨ�ţ���ȡ���п���������Ϣ
		jsonResult = GetCollectControllerInfoByBroadcast();
		break;
	case 8://�㲥��ʽ������������ͨ��
		jsonResult = SetCollectControllerInfoByBroadcast(root);
		break;
	case 9://ɾ��һ������������ɾ���͸ÿ�������ص�ģ�顢��
		jsonResult = DeleteCollectController(root);
		break;
	case 10://����Զ�����������س���
		jsonResult = FunIAP(root);
		break;
	case 11://�ڿ�����������һ��ģ�鲢���ú�ģ�����Ӧ����
		jsonResult = AddMeter(root);
		break;
	case 12://����ģ����������ò���
		jsonResult = SetMeterParameter(root);
		break;
	case 13://ɾ��һ��ģ�飬��ɾ����ģ����صĵ�
		jsonResult = DeleteMeter(root);
		break;
	case 14://��ȡģ�������������Ϣ
		jsonResult = GetMeterParameter(root);
		break;
	case 15://��ȡ�������е����ܲ���
		jsonResult = GetMeterElectricData(root);
		//GetMeterElectricData2(root, jsonResult);
		break;
	case 16://��ȡ������ʷ�����ܲ���
		jsonResult = GetMeterHisElectricData(root);
		break;
	case 17: //ʵʱ��ȡ�ɼ����ı�����Ϣ
		jsonResult = GetAlarmInfor(root);
		break;
	}
//	LOG_INFO(this_thread::get_id() << "������� JAVA ���ݣ����" << jsonResult);
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
	printf("�����ܺ���Ϣid=%d\n", nIndex);
	switch (nIndex)
	{
	case 0://����һ��������
		jsonResult = AddCollectControllerParameter(root);
		break;
	case 1:// ͬ��
		jsonResult = SetCollectControllerParameter(root);
		//	jsonResult = SetCollectControllerParameter2(root);
		break;
	case 2:	// ����ʱ��
		jsonResult = SetCollectControllerTime(root);
		break;
	case 3://����DDC,�൱�������λ
		jsonResult = RebootCollectController(root);
		break;
	case 4://���ÿ��������ָ��������ã�������
		jsonResult = ResetCollectControler(root);
		break;
	case 5:// ��ȡʱ��
		jsonResult = GetCollectControllerTime(root);
		break;
	case 6:// �Ա� ��ȡ���������������ò���
		jsonResult = GetCollectControllerParameter(root);
		break;
	case 7://�㲥��ʽ������������ͨ�ţ���ȡ���п���������Ϣ
		jsonResult = GetCollectControllerInfoByBroadcast();
		break;
	case 8://�㲥��ʽ������������ͨ��
		jsonResult = SetCollectControllerInfoByBroadcast(root);
		break;
	case 9://ɾ��һ������������ɾ���͸ÿ�������ص�ģ�顢��
		jsonResult = DeleteCollectController(root);
		break;
	case 10://����Զ�����������س���
		jsonResult = FunIAP(root);
		break;
	case 11://�ڿ�����������һ��ģ�鲢���ú�ģ�����Ӧ����
		jsonResult = AddMeter(root);
		break;
	case 12://����ģ����������ò���
		jsonResult = SetMeterParameter(root);
		break;
	case 13://ɾ��һ��ģ�飬��ɾ����ģ����صĵ�
		jsonResult = DeleteMeter(root);
		break;
	case 14://��ȡģ�������������Ϣ
		jsonResult = GetMeterParameter(root);
		break;
	case 15://��ȡ�������е����ܲ���
	//	jsonResult = GetMeterElectricData(root);
		GetMeterElectricData2(root, result);
		break;
	case 16://��ȡ������ʷ�����ܲ���
		jsonResult = GetMeterHisElectricData(root);

		//
		jsonResult["index"] = Json::Value(nIndex);
		//jsonResult["errmsg"] = Json::Value("Ӳ����������");

		break;
	case 17: //ʵʱ��ȡ�ɼ����ı�����Ϣ
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
	//�����ӿ�����Ҫ�Ĳ���
	const char *IPAddr;
	Gloable_Bes::CollectControllerParameter pParameter;
	memset(&pParameter, 0, sizeof(Gloable_Bes::CollectControllerParameter));
	char *errmsg;
	uint32_t ID = atoi(jsonData["CollectControllerParameter"]["ID"].asString().c_str());
	uint8_t Active = atoi(jsonData["CollectControllerParameter"]["Active"].asString().c_str());
	uint8_t SamplePeriod = atoi(jsonData["CollectControllerParameter"]["SamplePeriod"].asString().c_str());
	uint8_t HisdataSavePeriod = atoi(jsonData["CollectControllerParameter"]["HisdataSavePeriod"].asString().c_str());
	//����javaWeb�ͻ��˷�����json����
	string strIP = jsonData["IPAddr"].asString();

	// ��֪�� ��� �ط�Ϊɶ ���� �˿ں� �������紫�ݹ����Ķ˿ں� 

	uint32_t uPort = atoi(jsonData["Port"].asString().c_str());

	IPAddr = strIP.c_str();
	pParameter.ID = ID;
	strcpy_s(pParameter.Name, jsonData["CollectControllerParameter"]["Name"].asString().c_str());//stringתchar����
	strcpy_s(pParameter.Alias, jsonData["CollectControllerParameter"]["Alias"].asString().c_str());
	strcpy_s(pParameter.Description, jsonData["CollectControllerParameter"]["Description"].asString().c_str());
	strcpy_s(pParameter.Location, jsonData["CollectControllerParameter"]["Location"].asString().c_str());
	strcpy_s(pParameter.Zone, jsonData["CollectControllerParameter"]["Zone"].asString().c_str());
	pParameter.Active = Active;
	pParameter.SamplePeriod = SamplePeriod;
	pParameter.HisdataSavePeriod = HisdataSavePeriod;

	std::string strErrmsg;
	//���������ӿڣ��·�����λ�����ж��Ƿ�ɹ�����
	if (m_besManager.BES200_AddCollectControllerParameter(IPAddr, &pParameter, &errmsg) == 0)
	{
//		ctcp.LogPrint("��������", "�����������ɹ���");
		strErrmsg = "�����ɹ�";//���óɹ�����
	}
	else
	{
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
	//	ctcp.LogPrint("��������", "����������ʧ�ܣ�");
	}

	//������ֵ��װ��json����
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//��strErrmsg�ַ������ݷŵ�errmsg�ڵ�
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//����������Ϣ
}

Json::Value CDdataManager::SetCollectControllerParameter(Json::Value jsonData)
{
	//�����ӿ�����Ҫ�Ĳ���
	const char *IPAddr;
	CollectControllerParameter pParameter;
	memset(&pParameter, 0, sizeof(CollectControllerParameter));
	char *errmsg = "";
	uint32_t ID = atoi(jsonData["CollectControllerParameter"]["ID"].asString().c_str());
	uint8_t Active = atoi(jsonData["CollectControllerParameter"]["Active"].asString().c_str());
	uint8_t SamplePeriod = atoi(jsonData["CollectControllerParameter"]["SamplePeriod"].asString().c_str());
	uint8_t HisdataSavePeriod = atoi(jsonData["CollectControllerParameter"]["HisdataSavePeriod"].asString().c_str());
	//����javaWeb�ͻ��˷�����json����
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	pParameter.ID = ID;
	strcpy_s(pParameter.Name, jsonData["CollectControllerParameter"]["Name"].asString().c_str());//stringתchar����
	strcpy_s(pParameter.Alias, jsonData["CollectControllerParameter"]["Alias"].asString().c_str());
	strcpy_s(pParameter.Description, jsonData["CollectControllerParameter"]["Description"].asString().c_str());
	strcpy_s(pParameter.Location, jsonData["CollectControllerParameter"]["Location"].asString().c_str());
	strcpy_s(pParameter.Zone, jsonData["CollectControllerParameter"]["Zone"].asString().c_str());
	pParameter.Active = Active;
	pParameter.SamplePeriod = SamplePeriod;
	pParameter.HisdataSavePeriod = HisdataSavePeriod;




	std::string strErrmsg;
	//���������ӿڣ��·�����λ�����ж��Ƿ�ɹ�����
	if (m_besManager.BES200_SetCollectControllerParameter(IPAddr, &pParameter, &errmsg) == 0)
	{
	//	LOG_INFO("��������" << "����һ�������������в����ɹ�!");
		strErrmsg = "�����ɹ�";//���óɹ�����
	}
	else
	{
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
		LOGE("��������" << "����һ�������������в���ʧ��!");
	}
	//������ֵ��װ��json����
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//��strErrmsg�ַ������ݷŵ�errmsg�ڵ�
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//����������Ϣ
}

Json::Value CDdataManager::SetCollectControllerParameter2(Json::Value jsonData)
{
	//�����ӿ�����Ҫ�Ĳ���
	const char *IPAddr;
	CollectControllerParameter pParameter;
	memset(&pParameter, 0, sizeof(CollectControllerParameter));
	char *errmsg = "";
	uint32_t ID = atoi(jsonData["CollectControllerParameter"]["ID"].asString().c_str());
	uint8_t Active = atoi(jsonData["CollectControllerParameter"]["Active"].asString().c_str());
	uint8_t SamplePeriod = atoi(jsonData["CollectControllerParameter"]["SamplePeriod"].asString().c_str());
	uint8_t HisdataSavePeriod = atoi(jsonData["CollectControllerParameter"]["HisdataSavePeriod"].asString().c_str());
	//����javaWeb�ͻ��˷�����json����
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	pParameter.ID = ID;
	strcpy_s(pParameter.Name, jsonData["CollectControllerParameter"]["Name"].asString().c_str());//stringתchar����
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
	//strss = "{\"ID\":10000026,\"Name\":\"����1\",\"Alias\":\"����1\",\"Description\":\"����\",\"Location\":\"�з���\",\"Zone\":\"�з���\",\"Active\":1,\"SamplePeriod\":1,\"HisdataSavePeriod\":1}";
	//auto r3 = terra::json::from_json(pParameter2, strss.c_str(), strlen(strss.c_str()));


	/*
	{
	   "Active" : "1",
	   "Alias" : "����1",
	   "Description" : "����1",
	   "HisdataSavePeriod" : "1",
	   "ID" : "10000026",
	   "Location" : "�з���1",
	   "Name" : "����1",
	   "SamplePeriod" : "1",
	   "Zone" : "�з���1"
	}

	*/
	//jsonData.asString();


	std::string strErrmsg;
	//���������ӿڣ��·�����λ�����ж��Ƿ�ɹ�����
	if (m_besManager.BES200_SetCollectControllerParameterTest(IPAddr, &pParameter2, &errmsg) == 0)
	{
		//LOG_INFO("��������" << "����һ�������������в����ɹ�!");
		strErrmsg = "�����ɹ�";//���óɹ�����
	}
	else
	{
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
		LOGE("��������" << "����һ�������������в���ʧ��!");
	}
	//������ֵ��װ��json����
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//��strErrmsg�ַ������ݷŵ�errmsg�ڵ�
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//����������Ϣ
}

Json::Value CDdataManager::SetCollectControllerTime(Json::Value jsonData)
{
	//�����ӿ�����Ҫ�Ĳ���
	const char *IPAddr;
	tm pTime;
	char *errmsg = "";
	//����javaWeb�ͻ��˷�����json����
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	//��ϵͳʱ���ŵ�tm�ṹ����
	time_t now;
	time(&now);
	localtime_s(&pTime,&now);



	std::string strErrmsg;
	////���������ӿڣ��·�����λ�����ж��Ƿ�ɹ�����
	if (m_besManager.BES200_SetCollectControllerTime(IPAddr, &pTime, &errmsg) == 0)
	{
		//LOG_INFO("��������"<<"���ÿ�������ʱ��ɹ���");
		strErrmsg = "�����ɹ�";//���óɹ�����
	}
	else
	{
		LOGE("��������" << "���ÿ�������ʱ��ʧ�ܣ�");
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
	}

	//������ֵ��װ��json����
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//��strErrmsg�ַ������ݷŵ�errmsg�ڵ�

	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//����������Ϣ
}

Json::Value CDdataManager::RebootCollectController(Json::Value jsonData)
{
	//�����ӿ�����Ҫ�Ĳ���
	const char *IPAddr;
	char *errmsg = "";
	//����javaWeb�ͻ��˷�����json����
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	std::string strErrmsg;
	//���������ӿڣ��·�����λ�����ж��Ƿ�ɹ�����
	if (m_besManager.BES200_RebootCollectController(IPAddr, &errmsg) == 0)
	{
		//m_logs.LogPrint("��������", "����DDC�ɹ���");
		//LOG_INFO("��������" << "����DDC�ɹ���");
		strErrmsg = "�����ɹ�";//���óɹ�����
	}
	else
	{
		//m_logs.LogPrint("��������", "����DDCʧ�ܣ�");
		LOGE("��������" << "����DDCʧ�ܣ�");
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
	}
	//������ֵ��װ��json����
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//��strErrmsg�ַ������ݷŵ�errmsg�ڵ�
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//����������Ϣ
}

Json::Value CDdataManager::ResetCollectControler(Json::Value jsonData)
{
	//�����ӿ�����Ҫ�Ĳ���
	const char *IPAddr;
	char *errmsg = "";
	//����javaWeb�ͻ��˷�����json����
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	std::string strErrmsg;
	//���������ӿڣ��·�����λ�����ж��Ƿ�ɹ�����
	if (m_besManager.BES200_ResetCollectControler(IPAddr, &errmsg) == 0)
	{
		//m_logs.LogPrint("��������", "���ÿ������ɹ���");
		//LOG_INFO("��������" << "���ÿ������ɹ�!");
		strErrmsg = "�����ɹ�";//���óɹ�����
	}
	else
	{
		//m_logs.LogPrint("��������", "���ÿ�����ʧ�ܣ�");
		LOGE("��������" << "���ÿ�����ʧ�ܣ�");
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
	}
	//������ֵ��װ��json����
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//��strErrmsg�ַ������ݷŵ�errmsg�ڵ�
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//����������Ϣ
}

Json::Value CDdataManager::GetCollectControllerTime(Json::Value jsonData)
{
	const char *IPAddr;
	char errmsg[256] = {0};
	tm pTime = {0};
	//����javaWeb�ͻ��˷�����json����
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();

	// ��֪�� ��� �ط�Ϊɶ ���� �˿ں� �������紫�ݹ����Ķ˿ں� 

	uint32_t uPort = atoi(jsonData["Port"].asString().c_str());

	std::string strErrmsg;


	int Index;//��ȡ��λ�����ݳɹ���ʧ�ܵ�����
	//Ҫ���ص�json��������
	Json::Value jsonResult;
	//���������ӿڣ��·�����λ�����ж��Ƿ�ɹ�����
	int nRet = m_besManager.BES200_GetCollectControllerTime(IPAddr, &pTime, errmsg);
	if (nRet == 0)
	{
		//m_logs.LogPrint("��������", "��ȡ��������ʱ��ɹ���");
		//LOG_INFO("��������" << "��ȡ��������ʱ��ɹ�!");
		strErrmsg = "�����ɹ�";//���óɹ�����
		Index = 0;
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
		//�ӽڵ㴢��tm�ṹ�������
		Json::Value jsonTime;
		//�ӽڵ�����
		jsonTime["tm_sec"] = Json::Value(pTime.tm_sec);
		jsonTime["tm_min"] = Json::Value(pTime.tm_min);
		jsonTime["tm_hour"] = Json::Value(pTime.tm_hour);
		jsonTime["tm_mday"] = Json::Value(pTime.tm_mday);
		jsonTime["tm_mon"] = Json::Value(pTime.tm_mon);
		jsonTime["tm_year"] = Json::Value(pTime.tm_year);
		jsonTime["tm_wday"] = Json::Value(pTime.tm_wday);
		jsonTime["tm_yday"] = Json::Value(pTime.tm_yday);
		jsonTime["tm_isdst"] = Json::Value(pTime.tm_isdst);
		//���ӽڵ��ֵ�ŵ����ڵ�
		jsonResult["tm"] = Json::Value(jsonTime);
	}
	else
	{
	//	m_logs.LogPrint("��������", "��ȡ��������ʱ��ʧ�ܣ�");
		LOGE("��������" << IPAddr<< "��ȡ��������ʱ��ʧ�ܣ�" << nRet);
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
		Index = 1;
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
	}
//	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//����������Ϣ
}

Json::Value CDdataManager::GetCollectControllerParameter(Json::Value jsonData)
{
	//�����ӿ�����Ҫ�Ĳ���
	const char *IPAddr;
	CollectControllerParameter pParameter;
	char *errmsg = "";
	//����javaWeb�ͻ��˷�����json����
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	std::string strErrmsg;

	int Index;//��ȡ��λ�����ݳɹ���ʧ�ܵ�����
	//Ҫ���ص�json��������
	Json::Value jsonResult;

	if (m_besManager.BES200_GetCollectControllerParameter(IPAddr, &pParameter, &errmsg) == 0)
	{
	//	m_logs.LogPrint("��������", "��ȡ���������������ò����ɹ���");
		LOGE("��������" << "��ȡ���������������ò����ɹ���");
		strErrmsg = "�����ɹ�";//���óɹ�����
		Index = 0;
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
		//�ӽڵ㴢��CollectControllerParameter�ṹ�������
		Json::Value jsonParameter;
		//�ӽڵ�����
		jsonParameter["ID"] = Json::Value(pParameter.ID);
		jsonParameter["Name"] = Json::Value(pParameter.Name);
		jsonParameter["Alias"] = Json::Value(pParameter.Alias);
		jsonParameter["Description"] = Json::Value(pParameter.Description);
		jsonParameter["Location"] = Json::Value(pParameter.Location);
		jsonParameter["Zone"] = Json::Value(pParameter.Zone);
		jsonParameter["Active"] = Json::Value(pParameter.Active);
		jsonParameter["SamplePeriod"] = Json::Value(pParameter.SamplePeriod);
		jsonParameter["HisdataSavePeriod"] = Json::Value(pParameter.HisdataSavePeriod);
		//���ӽڵ��ֵ�ŵ����ڵ�
		jsonResult["CollectControllerParameter"] = Json::Value(jsonParameter);
	}
	else
	{
	//	m_logs.LogPrint("��������", "��ȡ���������������ò���ʧ�ܣ�");
		LOGE("��������" << "��ȡ���������������ò���ʧ�ܣ�");
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
		Index = 1;
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
	}
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;
}

Json::Value CDdataManager::GetCollectControllerInfoByBroadcast()
{
	//�����ӿ�����Ҫ�Ĳ���
	int size = -1;
	CollectControllerInfo *pInfo = NULL;
	CollectControllerInfo *p = NULL;
	char *errmsg = "";
	//�������ز���
	std::string strErrmsg;
	//��λ�����ݷ��سɹ���ʧ�ܵı�ʶ
	int Index;
	//Ҫ���ص�json��������
	Json::Value jsonResult;
	if (m_besManager.BES200_GetCollectControllerInfoByBroadcast(&pInfo, &size, &errmsg) == 0)
	{
		//m_logs.LogPrint("��������", "�㲥��ʽ������������ͨ�ţ���ȡ���п���������Ϣ�ɹ���");
		//LOG_INFO("��������" << "�㲥��ʽ������������ͨ�ţ���ȡ���п���������Ϣ�ɹ�����");
		p = pInfo;
		strErrmsg = "�����ɹ�";//���óɹ�����																
		Index = 0;
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
		//����jsonResult�ӽڵ㴢��CollectControllerInfo�ṹ���������
		Json::Value jsonCollectControllerInfo;
		//����jsonCollectControllerInfo�ӽڵ��ʶ
		string strIndex;
		for (int i = 0; i < size; i++)
		{
			//�ӽڵ㴢��CollectControllerInfo�ṹ�������																			
			Json::Value jsonInfo;
			//�ӽڵ�����																						
			jsonInfo["Name"] = Json::Value(pInfo->Name);
			jsonInfo["CampanyName"] = Json::Value(pInfo->CampanyName);
			jsonInfo["Model"] = Json::Value(pInfo->Model);
			jsonInfo["HWVer"] = Json::Value(pInfo->HWVer);
			jsonInfo["FWVer"] = Json::Value(pInfo->FWVer);

			//���޷������� ת��Ϊ ����ʽ���ַ�������ӵ�json������
			string strIP = Getstring(pInfo->IP[0]) + '.' + Getstring(pInfo->IP[1]) + '.' + Getstring(pInfo->IP[2]) + '.' + Getstring(pInfo->IP[3]);
			jsonInfo["IP"] = Json::Value(strIP);
			//���޷������� ת��Ϊ ����ʽ���ַ�������ӵ�json������
			string strMask = Getstring(pInfo->Mask[0]) + '.' + Getstring(pInfo->Mask[1]) + '.' + Getstring(pInfo->Mask[2]) + '.' + Getstring(pInfo->Mask[3]);
			jsonInfo["Mask"] = Json::Value(strMask);
			//���޷������� ת��Ϊ ����ʽ���ַ�������ӵ�json������
			string strGateWay = Getstring(pInfo->GateWay[0]) + '.' + Getstring(pInfo->GateWay[1]) + '.' + Getstring(pInfo->GateWay[2]) + '.' + Getstring(pInfo->GateWay[3]);
			jsonInfo["GateWay"] = Json::Value(strGateWay);
			//���޷������� ת��Ϊ ����ʽ���ַ�������ӵ�json������
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

			//��intתΪstring��ȡ�ڵ���
			char cIndex[256];
			string strIndex;
			sprintf_s(cIndex, "%d", i);
			strIndex = cIndex;
			//���ӽڵ��ֵ�ŵ����ڵ�	
			jsonCollectControllerInfo[strIndex] = Json::Value(jsonInfo);
		}
		jsonResult["CollectControllerInfo"] = Json::Value(jsonCollectControllerInfo);
	}
	else
	{
		//m_logs.LogPrint("��������", "�㲥��ʽ������������ͨ�ţ���ȡ���п���������Ϣʧ�ܣ�");
//		LOG_ERROR("��������" << "�㲥��ʽ������������ͨ�ţ���ȡ���п���������Ϣʧ�ܣ�");

		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ														
		Index = 1;
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
	}
	m_besManager.BES200_Free(&errmsg);
	// ת��Ϊ�ַ���������ʽ��
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
	//stringתchar����																								
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
	//���������ӿڣ��·�����λ�����ж��Ƿ�ɹ�����
	if (m_besManager.BES200_SetCollectControllerInfoByBroadcast(&pInfo, &errmsg) == 0)
	{
		//m_logs.LogPrint("��������", "�㲥��ʽ������������ͨ�ųɹ���");
		//LOG_INFO("��������" << "�㲥��ʽ������������ͨ�ųɹ�!");
		strErrmsg = "�����ɹ�";//���óɹ�����
	}
	else
	{
		//m_logs.LogPrint("��������", "�㲥��ʽ������������ͨ��ʧ�ܣ�");
		//LOG_INFO("��������" << "�㲥��ʽ������������ͨ��ʧ��!");
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
	}
	//������ֵ��װ��json����
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//��strErrmsg�ַ������ݷŵ�errmsg�ڵ�
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//����������Ϣ					
}

Json::Value CDdataManager::DeleteCollectController(Json::Value jsonData)
{
	//�����ӿ�����Ҫ�Ĳ���
	const char *IPAddr;
	char *errmsg = "";
	//����javaWeb�ͻ��˷�����json����
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	std::string strErrmsg;
	//���������ӿڣ��·�����λ�����ж��Ƿ�ɹ�����
	if (m_besManager.BES200_DeleteCollectController(IPAddr, &errmsg) == 0)
	{
		//m_logs.LogPrint("��������", "ɾ��һ������������ɾ���͸ÿ�������ص�ģ�顢��ɹ���");
		//LOG_INFO("��������" << "ɾ��һ������������ɾ���͸ÿ�������ص�ģ�顢��ɹ���");
		strErrmsg = "�����ɹ�";//���óɹ�����
	}
	else
	{
		//m_logs.LogPrint("��������", "ɾ��һ������������ɾ���͸ÿ�������ص�ģ�顢��ʧ�ܣ�");
		LOGE("��������" << "ɾ��һ������������ɾ���͸ÿ�������ص�ģ�顢��ʧ�ܣ�");
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
	}
	//������ֵ��װ��json����
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//��strErrmsg�ַ������ݷŵ�errmsg�ڵ�
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//����������Ϣ
}

Json::Value CDdataManager::FunIAP(Json::Value jsonData)
{
	//�����ӿ�����Ҫ�Ĳ���
	const char *IPAddr;
	char *errmsg = "";
	//����javaWeb�ͻ��˷�����json����
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	std::string strErrmsg;
	//���������ӿڣ��·�����λ�����ж��Ƿ�ɹ�����
	if (m_besManager.BES200_FunIAP(IPAddr, &errmsg) == 0)
	{
	//	m_logs.LogPrint("��������", "����Զ�����������س���ɹ���");
		//LOG_INFO("��������" << "����Զ�����������س���ɹ���");
		strErrmsg = "�����ɹ�";//���óɹ�����
	}
	else
	{
		//m_logs.LogPrint("��������", "����Զ�����������س���ʧ�ܣ�");
		LOGE("��������" << "����Զ�����������س���ʧ�ܣ�");
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
	}
	//������ֵ��װ��json����
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//��strErrmsg�ַ������ݷŵ�errmsg�ڵ�
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//����������Ϣ
}

Json::Value CDdataManager::AddMeter(Json::Value jsonData)
{
	//�����ӿ�����Ҫ�Ĳ���
	const char *IPAddr;
	unsigned int MeterID;
	MeterParameter pParameter;
	memset(&pParameter, 0, sizeof(MeterParameter));
	char *errmsg = "";
	//����javaWeb�ͻ��˷�����json����
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	MeterID = atoi(jsonData["MeterID"].asString().c_str());
	//ΪMeterParameter�ṹ���������
	pParameter.MeterID = atoi(jsonData["MeterParameter"]["MeterID"].asString().c_str());
	pParameter.MeterType = atoi(jsonData["MeterParameter"]["MeterType"].asString().c_str());
	pParameter.ComRate = atoi(jsonData["MeterParameter"]["ComRate"].asString().c_str());
	pParameter.ComPort = atoi(jsonData["MeterParameter"]["ComPort"].asString().c_str());
	pParameter.ComAgreementType = atoi(jsonData["MeterParameter"]["ComAgreementType"].asString().c_str());
	string strPhyAddr = jsonData["MeterParameter"]["PhyAddr"].asString();//�������ַ���沢���
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
	strcpy_s(pParameter.Alias, jsonData["MeterParameter"]["Alias"].asString().c_str());//stringתchar����
	strcpy_s(pParameter.Description, jsonData["MeterParameter"]["Description"].asString().c_str());
	strcpy_s(pParameter.Location, jsonData["MeterParameter"]["Location"].asString().c_str());
	pParameter.Active = atoi(jsonData["MeterParameter"]["Active"].asString().c_str());
	//ΪMeterParameter�ṹ���е�ElectricDataCollectMethod�ṹ�������
	pParameter.CollectMethod.CollectCount = atoi(jsonData["ElectricDataCollectMethod"]["CollectCount"].asString().c_str());
	pParameter.CollectMethod.CollectMethodID = atoi(jsonData["ElectricDataCollectMethod"]["CollectMethodID"].asString().c_str());

	// ת��Ϊ�ַ���������ʽ��
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
//		ctcp.LogPrint("��������", "�ڿ�����������һ��ģ�鲢���ú�ģ�����Ӧ�����ɹ���");
		strErrmsg = "�����ɹ�";//���óɹ�����
	}
	else
	{
	//	m_logs.LogPrint("��������", "�ڿ�����������һ��ģ�鲢���ú�ģ�����Ӧ����ʧ�ܣ�");
		LOGE("��������" << "�ڿ�����������һ��ģ�鲢���ú�ģ�����Ӧ����ʧ�ܣ�");
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
	}
	//������ֵ��װ��json����
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//��strErrmsg�ַ������ݷŵ�errmsg�ڵ�
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//����������Ϣ
}

Json::Value CDdataManager::SetMeterParameter(Json::Value jsonData)
{
	//�����ӿ�����Ҫ�Ĳ���
	const char *IPAddr;
	unsigned int MeterID;
	MeterParameter pParameter;
	char *errmsg = "";
	//����javaWeb�ͻ��˷�����json����
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	MeterID = atoi(jsonData["MeterID"].asString().c_str());
	//ΪMeterParameter�ṹ���������
	pParameter.MeterID = atoi(jsonData["MeterParameter"]["MeterID"].asString().c_str());
	pParameter.MeterType = atoi(jsonData["MeterParameter"]["MeterType"].asString().c_str());
	pParameter.ComRate = atoi(jsonData["MeterParameter"]["ComRate"].asString().c_str());
	pParameter.ComPort = atoi(jsonData["MeterParameter"]["ComPort"].asString().c_str());
	pParameter.ComAgreementType = atoi(jsonData["MeterParameter"]["ComAgreementType"].asString().c_str());
	string strPhyAddr = jsonData["MeterParameter"]["PhyAddr"].asString();//�������ַ���沢���
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

	strcpy_s(pParameter.Alias, jsonData["MeterParameter"]["Alias"].asString().c_str());//stringתchar����
	strcpy_s(pParameter.Description, jsonData["MeterParameter"]["Description"].asString().c_str());
	strcpy_s(pParameter.Location, jsonData["MeterParameter"]["Location"].asString().c_str());
	pParameter.Active = atoi(jsonData["MeterParameter"]["Active"].asString().c_str());
	//ΪMeterParameter�ṹ���е�ElectricDataCollectMethod�ṹ�������
	pParameter.CollectMethod.CollectCount = atoi(jsonData["ElectricDataCollectMethod"]["CollectCount"].asString().c_str());
	pParameter.CollectMethod.CollectMethodID = atoi(jsonData["ElectricDataCollectMethod"]["CollectMethodID"].asString().c_str());

	// ת��Ϊ�ַ���������ʽ��
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
	//	ctcp.LogPrint("��������", "����ģ����������ò����ɹ���");
		strErrmsg = "�����ɹ�";//���óɹ�����
	}
	else
	{
	//	ctcp.LogPrint("��������", "����ģ����������ò���ʧ�ܣ�");
		LOGE("��������" << "����ģ����������ò���ʧ��");
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
	}
	//������ֵ��װ��json����
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//��strErrmsg�ַ������ݷŵ�errmsg�ڵ�
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//����������Ϣ
}

Json::Value CDdataManager::DeleteMeter(Json::Value jsonData)
{
	//�����ӿ�����Ҫ�Ĳ���
	const char *IPAddr;
	unsigned int MeterID;
	char *errmsg = "";
	//����javaWeb�ͻ��˷�����json����
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	MeterID = atoi(jsonData["MeterID"].asString().c_str());

	std::string strErrmsg;
	if (m_besManager.BES200_DeleteMeter(IPAddr, MeterID, &errmsg) == 0)
	{
	//	ctcp.LogPrint("��������", "ɾ��һ��ģ�飬��ɾ����ģ����صĵ�ɹ���");
		strErrmsg = "�����ɹ�";//���óɹ�����
	}
	else
	{
	//	ctcp.LogPrint("��������", "ɾ��һ��ģ�飬��ɾ����ģ����صĵ�ʧ�ܣ�");
		LOGE("��������" << "ɾ��һ��ģ�飬��ɾ����ģ����صĵ�ʧ�ܣ�");
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
	}
	//������ֵ��װ��json����
	Json::Value jsonResult;
	jsonResult["errmsg"] = Json::Value(strErrmsg);//��strErrmsg�ַ������ݷŵ�errmsg�ڵ�
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//����������Ϣ
}

Json::Value CDdataManager::GetMeterParameter(Json::Value jsonData)
{
	//�����ӿ�����Ҫ�Ĳ���
	const char *IPAddr;
	unsigned int MeterID;
	MeterParameter pParameter;
	memset(&pParameter, 0, sizeof(MeterParameter));
	char *errmsg = "";
	//����javaWeb�ͻ��˷�����json����
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	MeterID = atoi(jsonData["MeterID"].asString().c_str());

	//��λ�����ݷ��سɹ���ʧ�ܵı�ʶ
	int Index;
	Json::Value jsonResult;
	std::string strErrmsg;
	if (m_besManager.BES200_GetMeterParameter(IPAddr, MeterID, &pParameter, &errmsg) == 0)
	{
	//	ctcp.LogPrint("��������", "����һ�������������в�����");
		Index = 0;
		Json::Value jsonMeterParameter;
		Json::Value jsonElectricDataCollectMethod;
		Json::Value jsonElectricDataInfo;
		strErrmsg = "�����ɹ�";
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);

		//��ȡMeterParameter�ṹ������
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
		//��jsonMeterParameter�ӽڵ���䵽jsonresult�ڵ�
		jsonResult["MeterParameter"] = Json::Value(jsonMeterParameter);

		//��ȡMeterParameter�ṹ������
		jsonElectricDataCollectMethod["CollectCount"] = Json::Value(pParameter.CollectMethod.CollectCount);
		jsonElectricDataCollectMethod["CollectMethodID"] = Json::Value(pParameter.CollectMethod.CollectMethodID);
		//��jsonElectricDataCollectMethod�ӽڵ���䵽jsonresult�ڵ�
		jsonResult["ElectricDataCollectMethod"] = Json::Value(jsonElectricDataCollectMethod);

		//��ȡElectricDataInfo�ṹ������
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
			//intתstring��ȡ���ܲ���keyֵ
			char cIndex[256];
			string strIndex;
			sprintf_s(cIndex, "%d", i);
			strIndex = cIndex;

			//����strIndex�����ܲ����������
			jsonElectricDataInfo[strIndex] = Json::Value(jsonIndex);
		}
		//��jsonElectricDataInfo�ӽڵ���䵽jsonresult�ڵ�
		jsonResult["ElectricDataInfo"] = Json::Value(jsonElectricDataInfo);
	}
	else
	{
//		ctcp.LogPrint("��������", "����һ�������������в���ʧ�ܣ�");
		LOGE("��������" << "����һ�������������в���ʧ�ܣ�");
		Index = 1;
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
	}

	// ת��Ϊ�ַ���������ʽ��
	//std::string out = jsonResult.toStyledString();
	//cout <<out<< endl;


	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//����������Ϣ
}

Json::Value CDdataManager::GetMeterElectricData(Json::Value jsonData)
{
	//�����ӿ�����Ҫ�Ĳ���																			
	const char *IPAddr;
	unsigned int MeterID;
	MeterElectricData pElectricData;
	char *errmsg = "";
	//����javaWeb�ͻ��˷�����json����																
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	MeterID = atoi(jsonData["MeterID"].asString().c_str());
	//LOG_INFO("GetMeterElectricData��");
	//��λ�����ݷ��سɹ���ʧ�ܵı�ʶ
	int Index;
	Json::Value jsonResult;
	std::string strErrmsg;
	if (m_besManager.BES200_GetMeterElectricData(IPAddr, MeterID, &pElectricData, &errmsg) == 0)
	{
	//	ctcp.LogPrint("��������", "��ȡ�������е����ܲ����ɹ���");
		Index = 0;
		Json::Value jsonMeterElectricData;
		strErrmsg = "�����ɹ�";
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
		//ΪMeterElectricData�ڵ��������
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
		//��MeterElectricData�ӽڵ���ӵ����ڵ�
		jsonResult["MeterElectricData"] = Json::Value(jsonMeterElectricData);
	}
	else
	{
	//	ctcp.LogPrint("��������", "��ȡ�������е����ܲ���ʧ�ܣ�");
		LOGE("��������" << "��ȡ�������е����ܲ���ʧ�ܣ�");
		Index = 1;
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
	}
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//����������Ϣ
}



void CDdataManager::GetMeterElectricData2(Json::Value jsonData, std::string &result)
{
	//�����ӿ�����Ҫ�Ĳ���																			
	const char *IPAddr;
	unsigned int MeterID;
	MeterElectricData pElectricData;
	char *errmsg = "";
	//����javaWeb�ͻ��˷�����json����																
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	MeterID = atoi(jsonData["MeterID"].asString().c_str());
	//LOG_INFO("GetMeterElectricData��");
	//��λ�����ݷ��سɹ���ʧ�ܵı�ʶ
	int Index;
	Json::Value jsonResult;
	std::string strErrmsg;
	if (m_besManager.BES200_GetMeterElectricData(IPAddr, MeterID, &pElectricData, &errmsg) == 0)
	{
		//	ctcp.LogPrint("��������", "��ȡ�������е����ܲ����ɹ���");
		Index = 0;
		Json::Value jsonMeterElectricData;
		strErrmsg = "�����ɹ�";
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
		//ΪMeterElectricData�ڵ��������
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
		//��MeterElectricData�ӽڵ���ӵ����ڵ�
		jsonResult["MeterElectricData"] = Json::Value(jsonMeterElectricData);
		string ss = jsonResult.toStyledString();
		//result = jsonResult.toStyledString();
		result = ss;
	}
	else
	{
		//	ctcp.LogPrint("��������", "��ȡ�������е����ܲ���ʧ�ܣ�");
		LOGE("��������" << "��ȡ�������е����ܲ���ʧ�ܣ�");
		Index = 1;
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);

		result = jsonResult.toStyledString();
	}
	m_besManager.BES200_Free(&errmsg);
}

Json::Value CDdataManager::GetMeterHisElectricData(Json::Value jsonData)
{
	//BES200_GetMeterHisElectricData(const char *IPAddr, uint32_t MeterID, uint8_t Selectday, struct MeterHisElectricData *pHisElectricData, char **errmsg);
	//�����ӿ�����Ҫ�Ĳ���																			
	const char *IPAddr;
	unsigned int MeterID;
	uint8_t Selectday;
	MeterHisElectricData pHisElectricData;
	char *errmsg = "";
	//����javaWeb�ͻ��˷�����json����																
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();
	MeterID = atoi(jsonData["MeterID"].asString().c_str());
	Selectday = atoi(jsonData["Selectday"].asString().c_str());
	//��λ�����ݷ��سɹ���ʧ�ܵı�ʶ
	int Index;
	Json::Value jsonResult;
	std::string strErrmsg;
	if (m_besManager.BES200_GetMeterHisElectricData(IPAddr, MeterID, Selectday, &pHisElectricData, &errmsg) == 0)
	{
//		ctcp.LogPrint("��������", "��ȡ������ʷ�����ܲ����ɹ���");
		Index = 0;
		Json::Value jsonMeterHisElectricData;
		Json::Value jsonMeterElectricHisData;
		strErrmsg = "�����ɹ�";
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
		//ΪMeterHisElectricData�ڵ��������
		jsonMeterHisElectricData["HisDataHourCount"] = Json::Value(pHisElectricData.HisDataHourCount);
		jsonMeterHisElectricData["MeterID"] = Json::Value(pHisElectricData.MeterID);
		int Count = pHisElectricData.HisDataHourCount;
		//ΪMeterElectricHisData�ڵ��������
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

			//intתstring��ȡ���ܲ���keyֵ
			char cIndex[256];
			string strIndex;
			sprintf_s(cIndex, "%d", i);
			strIndex = cIndex;
			//����strIndex�����ܲ����������
			jsonMeterElectricHisData[strIndex] = Json::Value(jsonIndex);

		}
		//jsonResult����ӽڵ�
		jsonResult["MeterHisElectricData"] = Json::Value(jsonMeterHisElectricData);
		jsonResult["MeterElectricHisData"] = Json::Value(jsonMeterElectricHisData);

	}
	else
	{
	//	ctcp.LogPrint("��������", "��ȡ������ʷ�����ܲ���ʧ�ܣ�");
		LOGE("��������" << "��ȡ������ʷ�����ܲ���ʧ�ܣ�");
		Index = 1;
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
	}
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//����������Ϣ
}

Json::Value CDdataManager::GetAlarmInfor(Json::Value jsonData)
{
	//�����ӿ�����Ҫ�Ĳ���																			
	const char *IPAddr;
	AlarmInfor AlarmInfor;
	char *errmsg = "";
	//����javaWeb�ͻ��˷�����json����																
	string strIP = jsonData["IPAddr"].asString();
	IPAddr = strIP.c_str();

	int Index;
	Json::Value jsonResult;
	std::string strErrmsg;
	if (m_besManager.BES200_GetAlarmInfor(IPAddr, &AlarmInfor, &errmsg) == 0)
	{
//		ctcp.LogPrint("��������", "ʵʱ��ȡ�ɼ����ı�����Ϣ�ɹ���");
		Index = 0;
		Json::Value jsonAlarmInfor;
		strErrmsg = "�����ɹ�";
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
		//jsonResult����ӽڵ�
		jsonResult["AlarmInfor"] = Json::Value(jsonAlarmInfor);
	}
	else
	{
//		ctcp.LogPrint("��������", "ʵʱ��ȡ�ɼ����ı�����Ϣʧ�ܣ�");
		LOGE("��������" << "ʵʱ��ȡ�ɼ����ı�����Ϣʧ�ܣ�");
		Index = 1;
		strErrmsg = (std::string)errmsg;//���ò��ɹ�����ȡ������Ϣ
		jsonResult["index"] = Json::Value(Index);
		jsonResult["errmsg"] = Json::Value(strErrmsg);
	}
	m_besManager.BES200_Free(&errmsg);
	return jsonResult;//����������Ϣ
}

Json::Value CDdataManager::Test(Json::Value jsonData)
{
	std::string strIndex = jsonData["index"].asString();
	int	nIndex = atoi(strIndex.c_str());

	//��ȡʱ��
	if (nIndex == 20)
	{
		Json::Value jsonResult;
		jsonResult["errmsg"] = Json::Value("�����ɹ�");//��strErrmsg�ַ������ݷŵ�errmsg�ڵ�

		tm pTime;//����ʱ��ṹ��
		//Ϊʱ��ṹ���������
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
	//��ȡ���������������ò���
	if (nIndex == 21)
	{
		Json::Value jsonResult;
		jsonResult["errmsg"] = Json::Value("�����ɹ�");
		//�ӽڵ㴢��CollectControllerParameter�ṹ�������
		Json::Value jsonParameter;
		//�ӽڵ�����
		jsonParameter["ID"] = Json::Value("111");
		jsonParameter["Name"] = Json::Value("����");
		jsonParameter["Alias"] = Json::Value("aa");
		jsonParameter["Description"] = Json::Value("aaa");
		jsonParameter["Location"] = Json::Value("aaaaa");
		jsonParameter["Zone"] = Json::Value("aaaaaa");
		jsonParameter["Active"] = Json::Value("aasax");
		jsonParameter["SamplePeriod"] = Json::Value("sas");
		jsonParameter["HisdataSavePeriod"] = Json::Value("his");
		//���ӽڵ��ֵ�ŵ����ڵ�
		jsonResult["CollectControllerParameter"] = Json::Value(jsonParameter);
		return jsonResult;
	}
	//�㲥��ʽ������������ͨ�ţ���ȡ���п���������Ϣ
	if (nIndex == 22)
	{
		Json::Value jsonResult;
		jsonResult["errmsg"] = Json::Value("�����ɹ�");
		//����jsonResult�ӽڵ㴢��CollectControllerInfo�ṹ���������
		Json::Value jsonCollectControllerInfo;

		int size = 2;
		for (int i = 0; i < size; i++)
		{
			//�ӽڵ㴢��CollectControllerInfo�ṹ�������																			
			Json::Value jsonInfo;
			//�ӽڵ�����																						 
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

			//��intתΪstring��ȡ�ڵ���
			char cIndex[256];
			string strIndex;
			sprintf_s(cIndex, "%d", i);
			strIndex = cIndex;

			//���ӽڵ��ֵ�ŵ����ڵ�	
			jsonCollectControllerInfo[strIndex] = Json::Value(jsonInfo);
		}
		jsonResult["CollectControllerInfo"] = Json::Value(jsonCollectControllerInfo);
		return jsonResult;
	}
	//��ȡģ�������������Ϣ
	if (nIndex == 23)
	{
		Json::Value jsonResult;
		jsonResult["errmsg"] = Json::Value("�����ɹ�");
		//����jsonResult�ӽڵ㴢��CollectControllerInfo�ṹ���������

		Json::Value jsonMeterParameter;
		Json::Value jsonElectricDataCollectMethod;
		Json::Value jsonElectricDataInfo;
		//��ȡMeterParameter�ṹ������
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
		//��jsonMeterParameter�ӽڵ���䵽jsonresult�ڵ�
		jsonResult["MeterParameter"] = Json::Value("jsonMeterParameter");

		//��ȡMeterParameter�ṹ������
		jsonElectricDataCollectMethod["CollectCount"] = Json::Value("2");
		jsonElectricDataCollectMethod["CollectMethodID"] = Json::Value("collectmethodid");
		//��jsonElectricDataCollectMethod�ӽڵ���䵽jsonresult�ڵ�
		jsonResult["ElectricDataCollectMethod"] = Json::Value(jsonElectricDataCollectMethod);

		//��ȡElectricDataInfo�ṹ������
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
			//intתstring��ȡ���ܲ���keyֵ
			char cIndex[256];
			string strIndex;
			sprintf_s(cIndex, "%d", i);
			strIndex = cIndex;

			//����strIndex�����ܲ����������
			jsonElectricDataInfo[strIndex] = Json::Value(jsonIndex);
		}
		//��jsonElectricDataInfo�ӽڵ���䵽jsonresult�ڵ�
		jsonResult = Json::Value(jsonElectricDataInfo);
		return jsonResult;
	}
	// ��ȡ�������е����ܲ���
	if (nIndex == 24)
	{
		Json::Value jsonResult;
		jsonResult["errmsg"] = Json::Value("�����ɹ�");
		Json::Value jsonMeterElectricData;

		//ΪMeterElectricData�ڵ��������
		jsonMeterElectricData["MeterID"] = Json::Value("meterid");
		jsonMeterElectricData["ElectricData"] = Json::Value("electricdata");
		jsonMeterElectricData["CollectCount"] = Json::Value("collectcount");
		jsonMeterElectricData["DateYear"] = Json::Value("dateyear");
		jsonMeterElectricData["DateMonth"] = Json::Value("datemon");
		jsonMeterElectricData["DateDay"] = Json::Value("DateDay");
		jsonMeterElectricData["TimeHour"] = Json::Value("timehour");
		jsonMeterElectricData["TimeMinute"] = Json::Value("timeminute");
		jsonMeterElectricData["TimeSecond"] = Json::Value("timesecond");
		//��MeterElectricData�ӽڵ���ӵ����ڵ�
		jsonResult["MeterElectricData"] = Json::Value(jsonMeterElectricData);
		return jsonResult;
	}
	//��ȡ������ʷ�����ܲ���
	if (nIndex == 25)
	{
		Json::Value jsonResult;
		jsonResult["errmsg"] = Json::Value("�����ɹ�");

		Json::Value jsonMeterHisElectricData;
		Json::Value jsonMeterElectricHisData;
		//ΪMeterHisElectricData�ڵ��������
		jsonMeterHisElectricData["HisDataHourCount"] = Json::Value("hisdatahourcount");
		jsonMeterHisElectricData["MeterID"] = Json::Value("meterid");
		int Count = 2;
		//ΪMeterElectricHisData�ڵ��������
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

			//intתstring��ȡ���ܲ���keyֵ
			char cIndex[256];
			string strIndex;
			sprintf_s(cIndex, "%d", i);
			strIndex = cIndex;
			//����strIndex�����ܲ����������
			jsonMeterElectricHisData[strIndex] = Json::Value(jsonIndex);

		}
		//jsonResult����ӽڵ�
		jsonResult["MeterHisElectricData"] = Json::Value(jsonMeterHisElectricData);
		jsonResult["MeterElectricHisData"] = Json::Value(jsonMeterElectricHisData);
		return jsonResult;
	}
	//ʵʱ��ȡ�ɼ����ı�����Ϣ
	if (nIndex == 26)
	{
		Json::Value jsonResult;
		Json::Value jsonAlarmInfor;
		jsonResult["errmsg"] = Json::Value("�����ɹ�");
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
		//jsonResult����ӽڵ�
		jsonResult["AlarmInfor"] = Json::Value(jsonAlarmInfor);
		return jsonResult;
	}
	return "";
}

string CDdataManager::Getstring(unsigned char number)
{
	string strNumber;//����ת�����ݵ��ַ���
	char cIndex[256];//ת��ý��
	sprintf_s(cIndex, "%d", number);
	strNumber = cIndex;
	return strNumber;
}

string CDdataManager::Getstring2(unsigned char number)
{
	int n = (number / 16 * 10) + number % 16;
	string strNumber;//����ת�����ݵ��ַ���
	char cIndex[256];//ת��ý��
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
	string strNumber;//����ת�����ݵ��ַ���
	char cIndex[256];//ת��ý��
	sprintf_s(cIndex, "%d", number);
	strNumber = cIndex;
	return strNumber;
}

int CDdataManager::GetDecimalism(string strNumber)
{
	//��һ�ַ���
	//char *str; 
	//int i = (int)strtol(strNumber.c_str(), &str, 16);//ʮ������   
	//return i;  

	//�ڶ��ַ���
	int nValude = 0;
	sscanf_s(strNumber.c_str(), "%x", &nValude);
	return nValude;
}

string CDdataManager::dec2hex(int number)
{
	stringstream ioss; //�����ַ�����
	string strNumber; //���ת�����ַ�
	ioss << std::hex << number; //��ʮ������ʽ���
	ioss >> strNumber;
	//std::string s(width - s_temp.size(), '0'); //��0
	//s += s_temp; //�ϲ�
	return strNumber;
}
