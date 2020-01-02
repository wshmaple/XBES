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
	// 定义方法函数
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

	/*新增一个控制器*/
	int BES200_AddCollectControllerParameter(const char *IPAddr,  struct Gloable_Bes::CollectControllerParameter *pParameter, char **errmsg);

	/*修改控制器的所有参数*/
	int BES200_SetCollectControllerParameter(const char *IPAddr, const struct CollectControllerParameter *pParameter, char **errmsg);

	/*修改控制器的所有参数*/
	int BES200_SetCollectControllerParameterTest(const char *IPAddr, const struct CollectControllerParameterTest *pParameter, char **errmsg);

	/* 设置控制器的时间 */
	int BES200_SetCollectControllerTime(const char *IPAddr, const struct tm *pTime, char **errmsg);

	/*重启DDC,相当于软件复位*/
	int BES200_RebootCollectController(const char *IPAddr, char **errmsg);

	/*远程升级*/
	int BES200_FunIAP(const char *IPAddr, char **errmsg);

	/*重置控制器，恢复出厂设置，并重启*/
	int BES200_ResetCollectControler(const char *IPAddr, char **errmsg);

	/*删除控制器*/
	int BES200_DeleteCollectController(const char *IPAddr, char **errmsg);

	/*获取控制器的时间*/
	int BES200_GetCollectControllerTime(const char *IPAddr, struct tm *pTime, char *errmsg);


	/*获取控制器的所有配置参数*/
	int BES200_GetCollectControllerParameter(const char *IPAddr, struct CollectControllerParameter *pParameter, char **errmsg);

	/*广播方式获取所有控制器信息*/
	int BES200_GetCollectControllerInfoByBroadcast(struct CollectControllerInfo **pInfo, int *size, char **errmsg);

	/*广播方式根据MAC地址设置控制器的IP地址等信息*/
	int BES200_SetCollectControllerInfoByBroadcast(const struct CollectControllerInfo *pInfo, char **errmsg);

	/*增加一个电表*/
	int BES200_AddMeter(const char *IPAddr, uint32_t MeterID, const struct MeterParameter *pParameter, char **errmsg);

	/*配置电表的所有配置参数*/
	int BES200_SetMeterParameter(const char *IPAddr, uint32_t MeterID, const struct MeterParameter *pParameter, char **errmsg);

	/*删除一个模块，并删除和模块相关的点*/
	int BES200_DeleteMeter(const char *IPAddr, uint32_t MeterID, char **errmsg);

	/* 获取模块的所有配置信息 */
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
	ZC_LOCK::CCriticalSection	m_lock;		// 临界区锁
};

