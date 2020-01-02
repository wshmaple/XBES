#pragma once
#include "json/json.h"
//#include "CCrossBesFunManager.h"
#include "CComFun.h"

#include "TCP.h"

#include "ThreadPool.h"
#include "zc_lock.h"

class CCrossDataManager
{
public:
	CCrossDataManager();
	~CCrossDataManager();
	Json::Value ComDataManager(Json::Value jsonData);
	 int BES200_RecvAFrame(const char ServerIP[], char *pRecvData);
	 int BES200_Communication(const char ServerIP[], const char *pSendData, int SendDataSize, char *pRecvData, int *RecvDataSize);

	 virtual  int   DoPackSendBuff(Json::Value jsonData, char *sendbuf); // ��� TCP ���ͱ���
	 virtual  Json::Value   DoPackRecvBuffToJson(char *pSendData , int nIndex, Json::Value &vrt); // ��� Ӳ�� ���� Ϊ ���籨��
	// �����������
	// 1.�������ݸ�Ӳ����Ӳ�����ص��Ǵ������
	// 2. ����Ӳ�����ݣ�Ӳ�����ض�Ӧ����
	// 3. ��Ӳ�������� ��Ӳ������ִ�д������
	
	virtual  int   AddControlParameter(Json::Value jsonData, char *sendbuf);
	virtual  int   SetControlParameter(Json::Value jsonData, char *sendbuf);
	
	virtual  int   AddOrSetControllerParameter(Json::Value jsonData,char *sendbuf);

	virtual  int   SetControllerTime(Json::Value jsonData, char  *sendbuf);

	virtual  int   OnlySendCmdToHard(Json::Value jsonData, char *sendbuf);//����ֻ�Ƿ���һ����Ϣ��Ӳ��



	// ģ��
	virtual  int   AddOrSetControllerModule(Json::Value jsonData, char *sendbuf);
	virtual  int   DeleteControllerModule(Json::Value jsonData, char *sendbuf);
	virtual  int   GetControllerModuleData(Json::Value jsonData, char *sendbuf);
	virtual  int   GetControllerModuleData2(Json::Value jsonData, char *sendbuf);



	virtual  int   GetControllerModuleErrorCode(Json::Value jsonData, char *sendbuf);

	// �߼���
	virtual  int   AddOrSetPoint(Json::Value jsonData, char *sendbuf);

	virtual  int   AddPointLAI(Json::Value jsonData, char *sendbuf);
	virtual  int   AddPointLAO(Json::Value jsonData, char *sendbuf);
	virtual  int   AddPointLDI(Json::Value jsonData, char *sendbuf);
	virtual  int   AddPointLDO(Json::Value jsonData, char *sendbuf);
	virtual  int   AddPointVAI(Json::Value jsonData, char *sendbuf);
	virtual  int   AddPointVAO(Json::Value jsonData, char *sendbuf);
	virtual  int   AddPointVDI(Json::Value jsonData, char *sendbuf);
	virtual  int   AddPointVDO(Json::Value jsonData, char *sendbuf);

	// �����߼����ֵ
	virtual  int   SetPointValue(Json::Value jsonData, char *sendbuf);
	virtual  int   SetPointValueByName(Json::Value jsonData, char *sendbuf);
	virtual  int   DeletePointByID(Json::Value jsonData, char *sendbuf);
	virtual  int   GetPointParameter(Json::Value jsonData, char * sendbuf);
	virtual  int   GetPointErrorCode(Json::Value jsonData, char * sendbuf);

	//����
	virtual  int   GetPointAlarmErrorCode(Json::Value jsonData, char *sendbuf);

	//�ƻ�
	virtual  int   AddScheduler(Json::Value jsonData, char * sendbuf);
	virtual  int   SetScheduler(Json::Value jsonData, char * sendbuf);
	virtual  int   DelScheduler(Json::Value jsonData, char * sendbuf);
	virtual  int   GetScheduler(Json::Value jsonData, char * sendbuf);

	//����
	virtual  int   AddScene(Json::Value jsonData, char * sendbuf);
	virtual  int   SetScene(Json::Value jsonData, char * sendbuf);
	virtual  int   DelScene(Json::Value jsonData, char * sendbuf);
	virtual  int   GetScene(Json::Value jsonData, char * sendbuf);

	//����
	virtual  int   UpDate(Json::Value jsonData, char* sendbuf);


	// ����������ʱ��
	virtual Json::Value ParseRecvBuf(void *buff); // ��������ֵ

	virtual Json::Value ParseGetTime(void *pRecvData);
	virtual Json::Value ParseGetControlPar(void *pRecvData);
	virtual Json::Value ParseGetModePar(void *pRecvData);
	virtual Json::Value ParseGetPointPar(void *pRecvData);
	virtual Json::Value ParseGetPointAlarm(void *pRecvData);
	virtual Json::Value ParseGetSence(void *pRecvData);
	virtual Json::Value ParseGetPlan(void *pRecvData);

	int32_t Getint32(string strKey, Json::Value jsonData);
	uint32_t GetUint32(string strKey, Json::Value jsonData);
	uint16_t GetUint16(string strKey,Json::Value jsonData);
	uint8_t GetUint8(string strKey, Json::Value jsonData);


public:
//	CCrossBesFunManager m_besManager;
	uint16_t BES200_htons(uint16_t n);
	CComFun m_comFun;

	CTCP m_TCP;

	ThreadPool m_threadpool;// ��ʱ�����õ��߳�

private:
	ZC_LOCK::CCriticalSection	m_lock;		// �ٽ�����
};

