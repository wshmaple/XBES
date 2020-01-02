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

	// ͳһ����Ϣ ��ں���
	Json::Value ComDataManager(Json::Value root);
	void ComDataManager2(Json::Value root, std::string& result);

public:

	/*��������������һ��������
	*����������javaWeb�ͻ��˷�����jsons����
	*����ֵ�������Ĵ�����Ϣ
	*˵������json���ݲ�����鷢����λ��
	*/
	Json::Value AddCollectControllerParameter(Json::Value jsonData);


	/*��������������һ�������������в���
	*����������javaWeb�ͻ��˷�����jsons����
	*����ֵ�������Ĵ�����Ϣ
	*˵������json���ݲ�����鷢����λ��
	*/
	Json::Value SetCollectControllerParameter(Json::Value jsonData);
	Json::Value SetCollectControllerParameter2(Json::Value jsonData);


	/*�������������ÿ�������ʱ��
	*����������javaWeb�ͻ��˷�����jsons����
	*����ֵ�������Ĵ�����Ϣ
	*˵������json���ݲ�����鷢����λ��
	*/
	Json::Value SetCollectControllerTime(Json::Value jsonData);


	/*��������������DDC,�൱�������λ
	*����������javaWeb�ͻ��˷�����jsons����
	*����ֵ�������Ĵ�����Ϣ
	*˵������json���ݲ�����鷢����λ��
	*/
	Json::Value RebootCollectController(Json::Value jsonData);


	/*�������������ÿ��������ָ��������ã�������
	*����������javaWeb�ͻ��˷�����jsons����
	*����ֵ�������Ĵ�����Ϣ
	*˵������json���ݲ�����鷢����λ��
	*/
	Json::Value ResetCollectControler(Json::Value jsonData);


	/*������������ȡ��������ʱ��
	*����������javaWeb�ͻ��˷�����jsons����
	*����ֵ�������Ĵ�����Ϣ������
	*˵������json���ݲ�����鷢����λ��
	*/
	Json::Value GetCollectControllerTime(Json::Value jsonData);


	/*������������ȡ���������������ò���
	*����������javaWeb�ͻ��˷�����jsons����
	*����ֵ�������Ĵ�����Ϣ������
	*˵������json���ݲ�����鷢����λ��
	*/
	Json::Value GetCollectControllerParameter(Json::Value jsonData);


	/*�����������㲥��ʽ������������ͨ�ţ���ȡ���п���������Ϣ
	*����������javaWeb�ͻ��˷�����jsons����
	*����ֵ�������Ĵ�����Ϣ������
	*˵������json���ݲ�����鷢����λ��
	*/
	Json::Value GetCollectControllerInfoByBroadcast();


	/*�����������㲥��ʽ������������ͨ�ţ�����MAC��ַ����������������Ϣ����Ҫ��ͨ��MAC�޸�IP��������Ϣ
	*����������javaWeb�ͻ��˷�����jsons����
	*����ֵ�������Ĵ�����Ϣ������
	*˵������json���ݲ�����鷢����λ��
	*/
	Json::Value SetCollectControllerInfoByBroadcast(Json::Value jsonData);


	/*����������ɾ��һ������������ɾ���͸ÿ�������ص�ģ�顢��
	*����������javaWeb�ͻ��˷�����jsons����
	*����ֵ�������Ĵ�����Ϣ������
	*˵������json���ݲ�����鷢����λ��
	*/
	Json::Value DeleteCollectController(Json::Value jsonData);


	/*��������������Զ�����������س���
	*����������javaWeb�ͻ��˷�����jsons����
	*����ֵ�������Ĵ�����Ϣ������
	*˵������json���ݲ�����鷢����λ��
	*/
	Json::Value  FunIAP(Json::Value jsonData);


	/*�����������ڿ�����������һ��ģ�鲢���ú�ģ�����Ӧ����
	*����������javaWeb�ͻ��˷�����jsons����
	*����ֵ�������Ĵ�����Ϣ������
	*˵������json���ݲ�����鷢����λ��
	*/
	Json::Value AddMeter(Json::Value jsonData);


	/*��������������ģ����������ò���
	*����������javaWeb�ͻ��˷�����jsons����
	*����ֵ�������Ĵ�����Ϣ������
	*˵������json���ݲ�����鷢����λ��
	*/
	Json::Value SetMeterParameter(Json::Value jsonData);


	/*����������ɾ��һ��ģ�飬��ɾ����ģ����صĵ�
	*����������javaWeb�ͻ��˷�����jsons����
	*����ֵ�������Ĵ�����Ϣ������
	*˵������json���ݲ�����鷢����λ��
	*/
	Json::Value DeleteMeter(Json::Value jsonData);


	/*������������ȡģ�������������Ϣ
	*����������javaWeb�ͻ��˷�����jsons����
	*����ֵ�������Ĵ�����Ϣ������
	*˵������json���ݲ�����鷢����λ��
	*/
	Json::Value GetMeterParameter(Json::Value jsonData);


	/*��ȡ�������е����ܲ���
	*����������javaWeb�ͻ��˷�����jsons����
	*����ֵ�������Ĵ�����Ϣ������
	*˵������json���ݲ�����鷢����λ��
	*/
	Json::Value GetMeterElectricData(Json::Value jsonData);

	void GetMeterElectricData2(Json::Value jsonData, std::string &result);


	/*
	*������������ȡ������ʷ�����ܲ���
	*����������javaWeb�ͻ��˷�����jsons����
	*����ֵ�������Ĵ�����Ϣ������
	*˵������json���ݲ�����鷢����λ��
	*/
	Json::Value GetMeterHisElectricData(Json::Value jsonData);


	/*
	*	����������	ʵʱ��ȡ�ɼ����ı�����Ϣ
	*����������javaWeb�ͻ��˷�����jsons����
	*����ֵ�������Ĵ�����Ϣ������
	*˵������json���ݲ�����鷢����λ��
	*/
	Json::Value GetAlarmInfor(Json::Value jsonData);

	//���Խӿ�
	Json::Value Test(Json::Value jsonData);

	//���޷����ַ�ת�ַ���
	string Getstring(unsigned char number);

	//���޷����ַ�ת�ַ���
	string Getstring2(unsigned char number);
	//���޷���intת�ַ���
	string Getstring3(unsigned int number);
	//��ʮ�������ַ���ת��Ϊint
	int GetDecimalism(string strNumber);


	//������ת��Ϊʮ�������ַ���
	string dec2hex(int number);
	private:
		//CLogs m_logs;

		CBES200Manager m_besManager;

};

