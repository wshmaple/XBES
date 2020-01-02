#pragma once
#include "ComDef.h"
#include "gloable.h"
using namespace Gloable_Bes;
#include "TCP.h"
#include "CComFun.h"

#include "zc_lock.h"
class CBES200Manager
{
public:
	CBES200Manager();
	~CBES200Manager();
public:
	// ���巽������
	//int BES200_Communication(const char ServerIP[], const char *pSendData, int SendDataSize, char *pRecvData, int *RecvDataSize/*, char **errmsg*/);
	int BES200_Communication(const char ServerIP[], const char *pSendData, int SendDataSize, char *pRecvData, int *RecvDataSize);
	int BES200_Communication2(const char ServerIP[], const char *pSendData, int SendDataSize, char *pRecvData, int *RecvDataSize/*, char **errmsg*/);

	uint16_t BES200_htons(uint16_t n);

	uint16_t BES200_ntohs(uint16_t n);

	uint32_t BES200_htonl(uint32_t n);

	uint32_t BES200_ntohl(uint32_t n);

	uint16_t BES200_CRC_16(const char *cBuf, int cLen);

	void BES200_Free(char **errmsg);

	void BES200Brocast_Free(struct CollectControllerInfo **errmsg);

	/*����һ��������*/
	int BES200_AddCollectControllerParameter(const char *IPAddr,  struct Gloable_Bes::CollectControllerParameter *pParameter, char **errmsg);

	/*�޸Ŀ����������в���*/
	int BES200_SetCollectControllerParameter(const char *IPAddr, const struct CollectControllerParameter *pParameter, char **errmsg);

	/*�޸Ŀ����������в���*/
	int BES200_SetCollectControllerParameterTest(const char *IPAddr, const struct CollectControllerParameterTest *pParameter, char **errmsg);

	/* ���ÿ�������ʱ�� */
	int BES200_SetCollectControllerTime(const char *IPAddr, const struct tm *pTime, char **errmsg);

	/*����DDC,�൱�������λ*/
	int BES200_RebootCollectController(const char *IPAddr, char **errmsg);

	/*Զ������*/
	int BES200_FunIAP(const char *IPAddr, char **errmsg);

	/*���ÿ��������ָ��������ã�������*/
	int BES200_ResetCollectControler(const char *IPAddr, char **errmsg);

	/*ɾ��������*/
	int BES200_DeleteCollectController(const char *IPAddr, char **errmsg);

	/*��ȡ��������ʱ��*/
	int BES200_GetCollectControllerTime(const char *IPAddr, struct tm *pTime, char *errmsg);


	/*��ȡ���������������ò���*/
	int BES200_GetCollectControllerParameter(const char *IPAddr, struct CollectControllerParameter *pParameter, char **errmsg);

	/*�㲥��ʽ��ȡ���п�������Ϣ*/
	int BES200_GetCollectControllerInfoByBroadcast(struct CollectControllerInfo **pInfo, int *size, char **errmsg);

	/*�㲥��ʽ����MAC��ַ���ÿ�������IP��ַ����Ϣ*/
	int BES200_SetCollectControllerInfoByBroadcast(const struct CollectControllerInfo *pInfo, char **errmsg);

	/*����һ�����*/
	int BES200_AddMeter(const char *IPAddr, uint32_t MeterID, const struct MeterParameter *pParameter, char **errmsg);

	/*���õ����������ò���*/
	int BES200_SetMeterParameter(const char *IPAddr, uint32_t MeterID, const struct MeterParameter *pParameter, char **errmsg);

	/*ɾ��һ��ģ�飬��ɾ����ģ����صĵ�*/
	int BES200_DeleteMeter(const char *IPAddr, uint32_t MeterID, char **errmsg);

	/* ��ȡģ�������������Ϣ */
	int BES200_GetMeterParameter(const char *IPAddr, uint32_t MeterID, struct MeterParameter *pParameter, char **errmsg);

	int BES200_GetMeterElectricData(const char *IPAddr, uint32_t MeterID, struct MeterElectricData *pElectricData, char **errmsg);


	int BES200_GetMeterHisElectricData(const char *IPAddr, uint32_t MeterID, uint8_t Selectday, struct MeterHisElectricData *pHisElectricData, char **errmsg);

	int BES200_GetAlarmInfor(const char *IPAddr, struct AlarmInfor *AlarmInfor, char **errmsg);

#define  SENDBUF_SIZE   9000
#define  RECVBUF_SIZE   9000
public:
	//char SendBuf[SENDBUF_SIZE];
	//char RecvBuf[RECVBUF_SIZE];
	int BES200_RecvAFrame(const char ServerIP[], char *pRecvData);
public:
	CTCP m_TCP;
	CComFun m_comFun;
private:
	ZC_LOCK::CCriticalSection	m_lock;		// �ٽ�����
};

