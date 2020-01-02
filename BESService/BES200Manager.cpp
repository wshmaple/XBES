#include "stdafx.h"
#include "BES200Manager.h"
#include "tcpSocketHead.h"
#include "gloable.h"
#include "zc_lock.h"
using namespace Gloable_Bes;
#define  DEF_BES_SERVER_PORT 6060
CBES200Manager::CBES200Manager()
{
}


CBES200Manager::~CBES200Manager()
{
}

int CBES200Manager::BES200_RecvAFrame(const char ServerIP[], char *pRecvData)
{
	int DataSize = 0, FrameSize, len;
	BEGP_RSP_HEAD_T *pHead_rsp;
	pHead_rsp = (BEGP_RSP_HEAD_T*)pRecvData;
	/*接收包头*/
	len = m_TCP.Recv(ServerIP, (void*)pRecvData, BEGP_RSP_HEAD_SIZE);
	if (len != BEGP_RSP_HEAD_SIZE)
	{
		//printf("获取数据头失败！%d\n", len);
		return -1;
	}

	DataSize += BEGP_RSP_HEAD_SIZE;
	uint16_t uLenDataSize = BES200_htonl(pHead_rsp->DataSize);
	FrameSize = BES200_htonl(pHead_rsp->DataSize) + BEGP_CRC_SIZE;
	int nLen2 = 0;
	/*接收正文数据和校验数据*/
	nLen2 = m_TCP.Recv(ServerIP, (void*)(pRecvData + BEGP_RSP_HEAD_SIZE), FrameSize);
	if (nLen2 != FrameSize)
	{
		if (nLen2 < FrameSize)
		{
			// 继续接收数据
			int nLen3 = 0;
			nLen3 = m_TCP.Recv(ServerIP, (void*)(pRecvData + BEGP_RSP_HEAD_SIZE+nLen2), FrameSize-nLen2);
			if ((nLen2+nLen3) != FrameSize)
			{
				//二次获取失败
				return - 1;
			}
		}
		else {
		
		}
	}

	DataSize += FrameSize;

	return DataSize;
}

int CBES200Manager::BES200_Communication(const char ServerIP[], const char *pSendData, int SendDataSize, char *pRecvData, int *RecvDataSize)
{
	ZC_LOCK::CSafeLock lock(&m_lock); //加临界区锁
	uint16_t *crc_req, crc1, crc2;
	int size, trytimes = 0;

	crc_req = (uint16_t*)(pSendData + SendDataSize);
	*crc_req = m_comFun.BES200_CRC_16(pSendData, SendDataSize);
	SendDataSize += BEGP_CRC_SIZE;
again:
	if (m_TCP.Connect2(ServerIP, 6060) == -1)
	{
		return BES200_ERROR_CONNECTFAILED;
	}
	if (m_TCP.Send(ServerIP,(void*)pSendData, SendDataSize) != SendDataSize)
	{
		m_TCP.DisConnect(ServerIP);
		if (m_TCP.Connect2(ServerIP, 6060) == -1) {
			m_TCP.DisConnect(ServerIP);
			return BES200_ERROR_CONNECTFAILED;
		}
		if (m_TCP.Send(ServerIP,(void*)pSendData, SendDataSize) != SendDataSize)
		{
			m_TCP.DisConnect(ServerIP);
			return  BES200_ERROR_SENDFAILED;
		}
	}
	else
	{
	}
	size = BES200_RecvAFrame(ServerIP, pRecvData);
	if (size == -1)
	{
		trytimes++;
		m_TCP.DisConnect(ServerIP);
		if (trytimes <= 2)
			goto again;
		else
			return BES200_ERROR_RECEIVETIMEOUT;
	}
	*RecvDataSize = size;
	/*收到一帧数据，开始处理*/
	crc1 = m_comFun.BES200_CRC_16(pRecvData, (*RecvDataSize) - BEGP_CRC_SIZE);
	crc2 = *((uint16_t*)(pRecvData + (*RecvDataSize) - BEGP_CRC_SIZE));
	if (crc1 == crc2)
	{
		return BES200_ERROR_OK;
	}
	else
	{
		return BES200_ERROR_RECEIVETIMEOUT;
	}
	return BES200_ERROR_OK;
}

int CBES200Manager::BES200_Communication2(const char ServerIP[], const char *pSendData, int SendDataSize, char *pRecvData, int *RecvDataSize/*, char **errmsg*/)
{
	return BES200_Communication(ServerIP, pSendData, SendDataSize, pRecvData, RecvDataSize);
}


uint16_t CBES200Manager::BES200_htons(uint16_t n)
{
	return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}

uint16_t CBES200Manager::BES200_ntohs(uint16_t n)
{
	return BES200_htons(n);
}

uint32_t CBES200Manager::BES200_htonl(uint32_t n)
{
	return ((n & 0xff) << 24) |
		((n & 0xff00) << 8) |
		((n & 0xff0000UL) >> 8) |
		((n & 0xff000000UL) >> 24);
}

uint32_t CBES200Manager::BES200_ntohl(uint32_t n)
{
	return BES200_htonl(n);
}

uint16_t CBES200Manager::BES200_CRC_16(const char *cBuf, int cLen)
{
	uint16_t  i, j;
	uint16_t  c, crc = 0xFFFF;
	for (i = 0; i < cLen; i++)
	{
		c = cBuf[i] & 0x00FF;
		crc ^= c;
		for (j = 0; j < 8; j++)
		{
			if (crc & 0x0001)
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else crc >>= 1;
		}
	}
	return(crc);
}

void CBES200Manager::BES200_Free(char **errmsg)
{
	free(*errmsg);
}

void CBES200Manager::BES200Brocast_Free(struct CollectControllerInfo **errmsg)
{
	free(*errmsg);
}

int CBES200Manager::BES200_AddCollectControllerParameter(const char *IPAddr,  struct CollectControllerParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T *pData_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize = 0;
	char SendBuf[SENDBUF_SIZE] = {0};
	char RecvBuf[RECVBUF_SIZE] = { 0 };

	*errmsg = (char *)malloc(2048);
	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_req = (BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T*)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//构造请求包头
	pHead_req->SlaveAddress = 1;			//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_ADD_CONTROLLER_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htonl(BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_req->ID = BES200_htonl(pParameter->ID);
	memcpy(pData_req->Name, pParameter->Name, sizeof(pData_req->Name));
	memcpy(pData_req->Alias, pParameter->Alias, sizeof(pData_req->Alias));
	memcpy(pData_req->Description, pParameter->Description, sizeof(pData_req->Description));
	memcpy(pData_req->Location, pParameter->Location, sizeof(pData_req->Location));
	memcpy(pData_req->Zone, pParameter->Zone, sizeof(pData_req->Zone));
	pData_req->Active = pParameter->Active;
	//新增的
	pData_req->SamplePeriod = pParameter->SamplePeriod;
	pData_req->HisdataSavePeriod = pParameter->HisdataSavePeriod;
	//新增的end
	SendDataSize += BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_SIZE;

	ret = BES200_Communication2(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	//通信正确 
	if (ret == BES200_ERROR_OK){
		//正文是否存在错误
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_ADD_CONTROLLER_PARAMETER) && \
			(pHead_rsp->DataSize) == BES200_htonl(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			//
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS){
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg, 2048, "Communication  OK ");
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR){
				ret = BES200_ERROR_DATASIZEERR;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_METERNOTFOUND){
				ret = BES200_ERROR_METERID_NOTFOUND;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR){
				ret = BES200_ERROR_DATANOTRIGHT;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else{
				ret = BES200_ERROR_REFUSED;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
		}
		else{
			ret = BES200_ERROR_HEAD;
			sprintf_s(*errmsg, 2048, "Communication HEAD Error Num ： %d", ret);
		}
	}
	else{//通信错误

		sprintf_s(*errmsg, 2048, "Communication SYS Error Num ： %d", ret);
	}
	return ret;
}

int CBES200Manager::BES200_SetCollectControllerParameter(const char *IPAddr, const struct CollectControllerParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_T *pData_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize = 0;
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };

	*errmsg = (char *)malloc(2048);
	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_req = (BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_T*)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_CONTROLLER_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htonl(BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;


	//构造请求正文数据
	pData_req->ID = BES200_htonl(pParameter->ID);
	memcpy(pData_req->Name, pParameter->Name, sizeof(pData_req->Name));
	memcpy(pData_req->Alias, pParameter->Alias, sizeof(pData_req->Alias));
	memcpy(pData_req->Description, pParameter->Description, sizeof(pData_req->Description));
	memcpy(pData_req->Location, pParameter->Location, sizeof(pData_req->Location));
	memcpy(pData_req->Zone, pParameter->Zone, sizeof(pData_req->Zone));
	pData_req->Active = pParameter->Active;
	//新增的
	pData_req->SamplePeriod = pParameter->SamplePeriod;
	pData_req->HisdataSavePeriod = pParameter->HisdataSavePeriod;
	//新增的end
	SendDataSize += BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_SIZE;

	//int n1 = sizeof(BEGP_REQ_HEAD_T);
	//int n2 = sizeof(BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_T);

	//int n3 = n1 + n2;

	//int n4 = sizeof(SendBuf);
	//int n5 = sizeof(pHead_req);
	//int n6 = sizeof(pData_req);
	//_BEGP_REQ_HEAD_ tagTest;

	//int n6666 = sizeof(tagTest.SlaveAddress); //uint8_t
	//int n7 = sizeof(tagTest.SequenceNumber);//16
	//int n8 = sizeof(tagTest.DataSize);//32
	//tagTest.SlaveAddress = 1;
	//int n9 = sizeof(tagTest.SlaveAddress); //uint8_t 
	//tagTest.SlaveAddress = 10;
	//int n69 = sizeof(tagTest.SlaveAddress); //uint8_t
	//tagTest.SlaveAddress = 100;
	//int n699 = sizeof(tagTest.SlaveAddress); //uint8_t
	//tagTest.SlaveAddress = 1000;
	//int n68 = sizeof(tagTest.SlaveAddress); //uint8_t

	//int n9777 = sizeof(char[20]);

	//terra::string_stream sscs;

	//terra::json::to_json(sscs, *pHead_req);
	//std::cout << ".........................................................................................." << std::endl;
	//std::cout << sscs.str() << std::endl;
	//LOG_WARN("发送pHead_req:" << sscs.str().c_str());
	//terra::json::to_json(sscs, *pData_req);
	//LOG_WARN("发送pData_req:" << sscs.str().c_str());

	//terra::json::to_json(sscs, *pHead_rsp);
	//LOG_WARN("接收 pHead_rsp:" << sscs.str().c_str());

	//terra::json::to_json(sscs, *pData_rsp);
	//LOG_WARN("接收 pData_rsp" << sscs.str().c_str());



	ret = BES200_Communication2(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	//通信正确 
	if (ret == BES200_ERROR_OK){
		//正文是否存在错误
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_SET_CONTROLLER_PARAMETER) && \
			(pHead_rsp->DataSize) == BES200_htonl(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			//
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS){
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg, 2048, "Communication  OK ");
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR){
				ret = BES200_ERROR_DATASIZEERR;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_METERNOTFOUND){
				ret = BES200_ERROR_METERID_NOTFOUND;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR){
				ret = BES200_ERROR_DATANOTRIGHT;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else{
				ret = BES200_ERROR_REFUSED;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
		}
		else{
			ret = BES200_ERROR_HEAD;
			sprintf_s(*errmsg, 2048, "Communication HEAD Error Num ： %d", ret);
		}
	}
	else{//通信错误

		sprintf_s(*errmsg, 2048, "Communication SYS Error Num ： %d", ret);
	}
	return ret;
}

int CBES200Manager::BES200_SetCollectControllerParameterTest(const char * IPAddr, const CollectControllerParameterTest * pParameter, char ** errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	_TAG_CollectControllerParameterTest			   *pData_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize = 0;
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };

	*errmsg = (char *)malloc(2048);
	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_req = (_TAG_CollectControllerParameterTest*)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_CONTROLLER_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htonl(BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_SIZE_TEST);
	SendDataSize += BEGP_REQ_HEAD_SIZE;


	//构造请求正文数据
	pData_req->ID = BES200_htonl(pParameter->ID);


	//memcpy(pData_req->Name, pParameter->Name, sizeof(pData_req->Name));
	//memcpy(pData_req->Alias, pParameter->Alias, sizeof(pData_req->Alias));
	//memcpy(pData_req->Description, pParameter->Description, sizeof(pData_req->Description));
	//memcpy(pData_req->Location, pParameter->Location, sizeof(pData_req->Location));
	//memcpy(pData_req->Zone, pParameter->Zone, sizeof(pData_req->Zone));

	pData_req->Name = pParameter->Name;
	pData_req->Alias = pParameter->Alias;
	pData_req->Description = pParameter->Description;
	pData_req->Location = pParameter->Location;
	pData_req->Zone = pParameter->Zone;
	
	pData_req->Active = pParameter->Active;

	//pData_req = pParameter;

	//新增的
	pData_req->SamplePeriod = pParameter->SamplePeriod;
	pData_req->HisdataSavePeriod = pParameter->HisdataSavePeriod;
	//新增的end
	SendDataSize += BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_SIZE_TEST;


	terra::string_stream sscs;

	//terra::json::to_json(sscs, *pHead_req);
	//std::cout << ".........................................................................................." << std::endl;
	//std::cout << sscs.str() << std::endl;
	//LOG_ERROR("发送pHead_req:" << sscs.str().c_str());
	//terra::json::to_json(sscs, *pData_req);
	//LOG_ERROR("发送pData_req:" << sscs.str().c_str());

	//terra::json::to_json(sscs, *pHead_rsp);
	//LOG_ERROR("接收 pHead_rsp:" << sscs.str().c_str());

	//terra::json::to_json(sscs, *pData_rsp);
	//LOG_ERROR("接收 pData_rsp" << sscs.str().c_str());



	ret = BES200_Communication2(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	//通信正确 
	if (ret == BES200_ERROR_OK) {
		//正文是否存在错误
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_SET_CONTROLLER_PARAMETER) && \
			(pHead_rsp->DataSize) == BES200_htonl(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			//
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS) {
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg, 2048, "Communication  OK ");
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR) {
				ret = BES200_ERROR_DATASIZEERR;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_METERNOTFOUND) {
				ret = BES200_ERROR_METERID_NOTFOUND;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR) {
				ret = BES200_ERROR_DATANOTRIGHT;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else {
				ret = BES200_ERROR_REFUSED;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
		}
		else {
			ret = BES200_ERROR_HEAD;
			sprintf_s(*errmsg, 2048, "Communication HEAD Error Num ： %d", ret);
		}
	}
	else {//通信错误

		sprintf_s(*errmsg, 2048, "Communication SYS Error Num ： %d", ret);
	}
	return ret;
}

int CBES200Manager::BES200_SetCollectControllerTime(const char *IPAddr, const struct tm *pTime, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_T      *pData_req;

	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize = 0;
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };

	*errmsg = (char *)malloc(2048);
	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_req = (BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_T*)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_CONTROLLER_TIME;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htonl(BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_req->year = pTime->tm_year - 1988;	//年
	pData_req->month = pTime->tm_mon + 1;			//月
	pData_req->day = pTime->tm_mday;			//日
	pData_req->hour = pTime->tm_hour;			//时
	pData_req->minute = pTime->tm_min;			//分
	pData_req->second = pTime->tm_sec;			//秒


	


	if (pTime->tm_wday == 0)
	{
		pData_req->wday = 7;
	}
	else
	{
		pData_req->wday = pTime->tm_wday;
	}

	SendDataSize += BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_SIZE;



	terra::string_stream sscs;

	//terra::json::to_json(sscs, *pHead_req);
	//std::cout << ".........................................................................................." << std::endl;
	//std::cout << sscs.str() << std::endl;
	//LOG_ERROR("发送pHead_req:"<<sscs.str().c_str());
	//terra::json::to_json(sscs, *pData_req);
	//LOG_ERROR("发送pData_req:" << sscs.str().c_str());

	//terra::json::to_json(sscs, *pHead_rsp);
	//LOG_ERROR("接收 pHead_rsp:" << sscs.str().c_str());

	//terra::json::to_json(sscs, *pData_rsp);
	//LOG_ERROR("接收 pData_rsp" << sscs.str().c_str());



	ret = BES200_Communication2(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	//通信正确 
	if (ret == BES200_ERROR_OK){
		//正文是否存在错误
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_SET_CONTROLLER_TIME) && \
			(pHead_rsp->DataSize) == BES200_htonl(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			//
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS){
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg, 2048, "Communication  OK ");
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR){
				ret = BES200_ERROR_DATASIZEERR;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_METERNOTFOUND){
				ret = BES200_ERROR_METERID_NOTFOUND;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR){
				ret = BES200_ERROR_DATANOTRIGHT;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else{
				ret = BES200_ERROR_REFUSED;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
		}
		else{
			ret = BES200_ERROR_HEAD;
			sprintf_s(*errmsg, 2048, "Communication HEAD Error Num ： %d", ret);
		}
	}
	else{//通信错误

		sprintf_s(*errmsg, 2048, "Communication SYS Error Num ： %d", ret);
	}
	return ret;
}

int CBES200Manager::BES200_RebootCollectController(const char *IPAddr, char **errmsg)
{
	BEGP_REQ_HEAD_T                           *pHead_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize = 0;
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };

	*errmsg = (char *)malloc(2048);

	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_REBOOT_CONTROLLER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htonl(0);

	SendDataSize += BEGP_REQ_HEAD_SIZE;
	ret = BES200_Communication2(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	//通信正确 
	if (ret == BES200_ERROR_OK){
		//正文是否存在错误
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_REBOOT_CONTROLLER) && \
			(pHead_rsp->DataSize) == BES200_htonl(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			//
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS){
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg, 2048, "Communication  OK ");
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR){
				ret = BES200_ERROR_DATASIZEERR;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_METERNOTFOUND){
				ret = BES200_ERROR_METERID_NOTFOUND;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR){
				ret = BES200_ERROR_DATANOTRIGHT;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else{
				ret = BES200_ERROR_REFUSED;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
		}
		else{
			ret = BES200_ERROR_HEAD;
			sprintf_s(*errmsg, 2048, "Communication HEAD Error Num ： %d", ret);
		}
	}
	else{//通信错误

		sprintf_s(*errmsg, 2048, "Communication SYS Error Num ： %d", ret);
	}
	return ret;
}

int CBES200Manager::BES200_FunIAP(const char *IPAddr, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize = 0;
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };

	*errmsg = (char *)malloc(2048);

	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_IAP;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htonl(0);

	SendDataSize += BEGP_REQ_HEAD_SIZE;
	ret = BES200_Communication2(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	//通信正确 
	if (ret == BES200_ERROR_OK){
		//正文是否存在错误
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_REBOOT_CONTROLLER) && \
			(pHead_rsp->DataSize) == BES200_htonl(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			//
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS){
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg, 2048, "Communication  OK ");
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR){
				ret = BES200_ERROR_DATASIZEERR;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_METERNOTFOUND){
				ret = BES200_ERROR_METERID_NOTFOUND;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR){
				ret = BES200_ERROR_DATANOTRIGHT;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else{
				ret = BES200_ERROR_REFUSED;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
		}
		else{
			ret = BES200_ERROR_HEAD;
			sprintf_s(*errmsg, 2048, "Communication HEAD Error Num ： %d", ret);
		}
	}
	else{//通信错误

		sprintf_s(*errmsg, 2048, "Communication SYS Error Num ： %d", ret);
	}
	return ret;
}

int CBES200Manager::BES200_ResetCollectControler(const char *IPAddr, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize = 0;
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };

	*errmsg = (char *)malloc(2048);

	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_RESET_CONTROLLER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htonl(0);

	SendDataSize += BEGP_REQ_HEAD_SIZE;
	ret = BES200_Communication2(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	//通信正确 
	if (ret == BES200_ERROR_OK){
		//正文是否存在错误
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_RESET_CONTROLLER) && \
			(pHead_rsp->DataSize) == BES200_htonl(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			//
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS){
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg, 2048, "Communication  OK ");
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR){
				ret = BES200_ERROR_DATASIZEERR;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_METERNOTFOUND){
				ret = BES200_ERROR_METERID_NOTFOUND;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR){
				ret = BES200_ERROR_DATANOTRIGHT;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else{
				ret = BES200_ERROR_REFUSED;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
		}
		else{
			ret = BES200_ERROR_HEAD;
			sprintf_s(*errmsg, 2048, "Communication HEAD Error Num ： %d", ret);
		}
	}
	else{//通信错误

		sprintf_s(*errmsg, 2048, "Communication SYS Error Num ： %d", ret);
	}
	return ret;
}

int CBES200Manager::BES200_DeleteCollectController(const char *IPAddr, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize = 0;
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };

	*errmsg = (char *)malloc(2048);
	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_DELETE_CONTROLLER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htonl(0);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	ret = BES200_Communication2(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	//通信正确 
	if (ret == BES200_ERROR_OK){
		//正文是否存在错误
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_DELETE_CONTROLLER) && \
			(pHead_rsp->DataSize) == BES200_htonl(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			//
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS){
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg, 2048, "Communication  OK ");
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR){
				ret = BES200_ERROR_DATASIZEERR;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_METERNOTFOUND){
				ret = BES200_ERROR_METERID_NOTFOUND;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR){
				ret = BES200_ERROR_DATANOTRIGHT;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else{
				ret = BES200_ERROR_REFUSED;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
		}
		else{
			ret = BES200_ERROR_HEAD;
			sprintf_s(*errmsg, 2048, "Communication HEAD Error Num ： %d", ret);
		}
	}
	else{//通信错误

		sprintf_s(*errmsg, 2048, "Communication SYS Error Num ： %d", ret);
	}
	return ret;
}

int CBES200Manager::BES200_GetCollectControllerTime(const char *IPAddr, struct tm *pTime, char *errmsg)
{
	BEGP_REQ_HEAD_T                              *pHead_req;
	BEGP_RSP_HEAD_T                              *pHead_rsp;
	BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_T    *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize = 0;
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };

	//*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_CONTROLLER_TIME;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htonl(0);


	SendDataSize += BEGP_REQ_HEAD_SIZE;

	ret = BES200_Communication2(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);

	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_GET_CONTROLLER_TIME) && \
			(BES200_htonl(pHead_rsp->DataSize) >= BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_SIZE))
		{
			pTime->tm_year = pData_rsp->year + 2000;
			pTime->tm_mon = pData_rsp->month;
			pTime->tm_mday = pData_rsp->day;
			pTime->tm_hour = pData_rsp->hour;
			pTime->tm_min = pData_rsp->minute;
			pTime->tm_sec = pData_rsp->second;
			if (pData_rsp->wday == 7)
			{
				pData_rsp->wday = 0;
			}
			else
			{
				pTime->tm_wday = pData_rsp->wday;
			}
			ret = BES200_ERROR_OK;
			sprintf_s(errmsg, 256, "Communication  OK ");

		}
		else{
			ret = BES200_ERROR_HEAD;
			sprintf_s(errmsg, 256, "Communication HEAD Error Num ： %d", ret);
		}
	}
	else
	{
		//错误码处理
		sprintf_s(errmsg, 256, "Communication SYS Error Num ： %d", ret);
	}
	return ret;
}

int CBES200Manager::BES200_GetCollectControllerParameter(const char *IPAddr, struct CollectControllerParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                                  *pHead_req;
	BEGP_RSP_HEAD_T                                  *pHead_rsp;
	BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_T   *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize = 0;
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_CONTROLLER_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htonl(0);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	ret = BES200_Communication2(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);

//	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);

	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_GET_CONTROLLER_PARAMETER) && \
			(BES200_htonl(pHead_rsp->DataSize) >= BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_SIZE))
		{
			pParameter->ID = pData_rsp->ID;											//ID号
			memcpy(pParameter->Name, pData_rsp->Name, sizeof(pParameter->Name));		//名字
			memcpy(pParameter->Alias, pData_rsp->Alias, sizeof(pParameter->Alias));			//DDC别名
			memcpy(pParameter->Description, pData_rsp->Description, sizeof(pParameter->Description));	//描述
			memcpy(pParameter->Location, pData_rsp->Location, sizeof(pParameter->Location));		//安装位置
			memcpy(pParameter->Zone, pData_rsp->Zone, sizeof(pParameter->Zone));			//区域
			pParameter->Active = pData_rsp->Active;										//是否启用
			pParameter->SamplePeriod = pData_rsp->SamplePeriod;								//采样周期
			pParameter->HisdataSavePeriod = pData_rsp->HisdataSavePeriod;							//历史数据保存周期
			ret = BES200_ERROR_OK;
			sprintf_s(*errmsg, 2048, "Communication  OK ");

		}
		else{
			ret = BES200_ERROR_HEAD;
			sprintf_s(*errmsg, 2048, "Communication HEAD Error Num ： %d", ret);
		}
	}
	else
	{
		//错误码处理
		sprintf_s(*errmsg, 2048, "Communication SYS Error Num ： %d", ret);
	}
	return ret;
}

int CBES200Manager::BES200_GetCollectControllerInfoByBroadcast(struct CollectControllerInfo **pInfo, int *size, char **errmsg)
{
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };
	SOCKET m_socket;
	SOCKADDR_IN m_addrfrom, m_addrto;
	uint16_t *pCrcSend, *pCrcRecv, crc;
	int ret = BES200_ERROR_OTHERS, SendDataSize = 0, RecvDataSize = 0;
	BEGP_REQ_HEAD_T                                  *pHead_req;
	BEGP_RSP_HEAD_T                                  *pHead_rsp;
	BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_T        *pData_rsp;
	int index = 0;

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];
	pCrcSend = (uint16_t*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pCrcRecv = (uint16_t*)(&RecvBuf[BEGP_RSP_HEAD_SIZE + BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_SIZE]);
	//正文数据，指针赋值
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	/*请求头数据赋值*/
	pHead_req->SlaveAddress = 1;
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_CONTROLLER_INFO;
	pHead_req->SequenceNumber = 1;
	pHead_req->DataSize = 0;
	SendDataSize = BEGP_REQ_HEAD_SIZE;

	/*计算CRC16*/
	*pCrcSend = BES200_CRC_16(SendBuf, SendDataSize);//计算CRC16
	SendDataSize += BEGP_CRC_SIZE;

	*errmsg = (char *)malloc(1024);
	*pInfo = (struct CollectControllerInfo *)malloc(sizeof(struct CollectControllerInfo) * 1000);//最多支持1000个设备
	if (*errmsg == NULL || *pInfo == NULL)
		return BES200_ERROR_MALLOCERROR;

	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_socket)
	{
		ret = WSAGetLastError();
		sprintf_s(*errmsg, 2048, "socket()失败 :%d", ret);
		WSACleanup();
		return BES200_ERROR_SOCKETFAILED;
	}

	/*客户端地址*/
	m_addrfrom.sin_family = AF_INET;
	m_addrfrom.sin_port = htons(1203);
	m_addrfrom.sin_addr.S_un.S_addr = INADDR_ANY;

	/*服务端地址*/
	m_addrto.sin_family = AF_INET;
	m_addrto.sin_port = htons(1203);
	m_addrto.sin_addr.S_un.S_addr = INADDR_BROADCAST;

	//设置广播
	bool bBroadcast = true;
	setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (char FAR *)&bBroadcast, sizeof(bBroadcast));
	//设置超时
	int nNetTimeout;
	nNetTimeout = 1000;//毫秒
	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char FAR *)&nNetTimeout, sizeof(nNetTimeout));

	ret = ::bind(m_socket, (sockaddr*)&m_addrfrom, sizeof(sockaddr));
	if (ret == SOCKET_ERROR)
	{
		ret = WSAGetLastError();
		sprintf_s(*errmsg, 2048, "bind()失败 %d!", ret);
		ret = BES200_ERROR_SOCKETFAILED;
		closesocket(m_socket);
		return ret;
	}

	int len = sizeof(sockaddr);
	int res = sendto(m_socket, (char*)SendBuf, SendDataSize, 0, (sockaddr*)&m_addrto, len);

	if (res == (BEGP_REQ_HEAD_SIZE + BEGP_CRC_SIZE))
	{

		//一直接收，直到接收超时，代表后续没有数据
		while (1)
		{
			RecvDataSize = recvfrom(m_socket, RecvBuf, RECVBUF_SIZE, 0, (sockaddr*)&m_addrfrom, &len);
			if (RecvDataSize == -1 || index >= 1000)
			{
				if (index > 0)
				{
					ret = BES200_ERROR_OK;
					sprintf_s(*errmsg, 2048, "搜索到 %d 个采集器  ，ret = %d", index, ret);
				}
				else{
					ret = BES200_ERROR_OTHERS;
					sprintf_s(*errmsg, 2048, "搜索到 %d 个采集器  ，ret = %d", index, ret);
				}
				break;
			}
			else if (RecvDataSize == BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_SIZE + BEGP_CRC_SIZE + BEGP_RSP_HEAD_SIZE)
			{//数据长度正确
				//CRC16校验
				crc = BES200_CRC_16(RecvBuf, RecvDataSize - BEGP_CRC_SIZE);

				if (pHead_rsp->FunctionCode == BEGP_FUNCODE_GET_CONTROLLER_INFO&&\
					pHead_rsp->DataSize == BES200_htonl(BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_SIZE) && \
					crc == *pCrcRecv)
				{
					struct CollectControllerInfo *ptr = *pInfo;
					memcpy(ptr[index].Name, pData_rsp->Name, sizeof(pData_rsp->Name));
					memcpy(ptr[index].Model, pData_rsp->Model, sizeof(pData_rsp->Model));
					memcpy(ptr[index].Mask, pData_rsp->Mask, sizeof(pData_rsp->Mask));
					memcpy(ptr[index].Mac, pData_rsp->Mac, sizeof(pData_rsp->Mac));
					memcpy(ptr[index].IP, pData_rsp->IP, sizeof(pData_rsp->IP));
					ptr[index].HWVer = pData_rsp->HWVer;
					ptr[index].FWVer = pData_rsp->FWVer;
					memcpy(ptr[index].GateWay, pData_rsp->GateWay, sizeof(pData_rsp->GateWay));
					memcpy(ptr[index].CampanyName, pData_rsp->CampanyName, sizeof(pData_rsp->CampanyName));

					ptr[index].ID = pData_rsp->ID;
					memcpy(ptr[index].Alias, pData_rsp->Alias, sizeof(pData_rsp->Alias));
					memcpy(ptr[index].Description, pData_rsp->Description, sizeof(pData_rsp->Description));
					memcpy(ptr[index].Location, pData_rsp->Location, sizeof(pData_rsp->Location));
					memcpy(ptr[index].Zone, pData_rsp->Zone, sizeof(pData_rsp->Zone));
					ptr[index].Active = pData_rsp->Active;
					ptr[index].SamplePeriod = pData_rsp->SamplePeriod;
					ptr[index].HisdataSavePeriod = pData_rsp->HisdataSavePeriod;

					*size = ++index;
				}
				else{
					ret = BES200_ERROR_DATASIZEERR;
					sprintf_s(*errmsg, 2048, "搜索到 %d 个采集器  ，ret = %d", index, ret);
					continue;
				}
			}
			else{
				ret = BES200_ERROR_DATASIZEERR;
				sprintf_s(*errmsg, 2048, "搜索到 %d 个采集器  ，ret = %d", index, ret);
				continue;
			}
		}

	}
	else{
		sprintf_s(*errmsg, 2048, "sendto len = %d失败，res = %d", sizeof(BEGP_REQ_HEAD_T), res);
		ret = BES200_ERROR_DATASIZEERR;
	}
	//out:
	closesocket(m_socket);
	return ret;
}

int CBES200Manager::BES200_SetCollectControllerInfoByBroadcast(const struct CollectControllerInfo *pInfo, char **errmsg)
{
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };
	uint16_t *pCrcSend, *pCrcRecv, crc;
	SOCKET m_socket;
	SOCKADDR_IN m_addrfrom, m_addrto;
	int ret = BES200_ERROR_OTHERS, SendDataSize, RecvDataSize;
	BEGP_REQ_HEAD_T                                  *pHead_req;
	BEGP_RSP_HEAD_T                                  *pHead_rsp;
	BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_T        *pData_req;
	BEGP_RSP_PACKETDATA_ECHO_T						 *pData_rsp;

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_req = (BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	/*CRC16指针赋值*/
	pCrcSend = (uint16_t*)(&SendBuf[BEGP_REQ_HEAD_SIZE + BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_SIZE]);
	pCrcRecv = (uint16_t*)(&RecvBuf[BEGP_RSP_HEAD_SIZE + BEGP_RSP_PACKETDATA_ECHO_SIZE]);

	/*请求头赋值*/
	pHead_req->SlaveAddress = 1;
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_CONTROLLER_INFO;
	pHead_req->SequenceNumber = 1;
	pHead_req->DataSize = BES200_htonl(BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_SIZE);
	SendDataSize = BEGP_REQ_HEAD_SIZE;

	/*正文数据赋值*/
	memcpy(pData_req->Name, pInfo->Name, sizeof(pInfo->Name));
	memcpy(pData_req->CampanyName, pInfo->CampanyName, sizeof(pInfo->CampanyName));
	memcpy(pData_req->Model, pInfo->Model, sizeof(pInfo->Model));
	pData_req->HWVer = pInfo->HWVer;
	pData_req->FWVer = pInfo->FWVer;
	memcpy(pData_req->IP, pInfo->IP, sizeof(pInfo->IP));
	memcpy(pData_req->Mask, pInfo->Mask, sizeof(pInfo->Mask));
	memcpy(pData_req->GateWay, pInfo->GateWay, sizeof(pInfo->GateWay));
	memcpy(pData_req->Mac, pInfo->Mac, sizeof(pInfo->Mac));

	SendDataSize += BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_SIZE;

	/*计算CRC*/
	*pCrcSend = BES200_CRC_16(SendBuf, SendDataSize);
	SendDataSize += BEGP_CRC_SIZE;

	*errmsg = (char *)malloc(1024);
	if (*errmsg == NULL)
		return BES200_ERROR_MALLOCERROR;

	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_socket)
	{
		ret = WSAGetLastError();
		sprintf_s(*errmsg, 2048, "socket()失败 :%d", ret);
		WSACleanup();
		return BES200_ERROR_SOCKETFAILED;
	}

	/*客户端地址*/
	m_addrfrom.sin_family = AF_INET;
	m_addrfrom.sin_port = 1204;
	m_addrfrom.sin_addr.S_un.S_addr = INADDR_ANY;

	/*服务端地址*/
	m_addrto.sin_family = AF_INET;
	m_addrto.sin_port = 1204;
	m_addrto.sin_addr.S_un.S_addr = INADDR_BROADCAST;

	//设置广播
	bool bBroadcast = true;
	setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (char FAR *)&bBroadcast, sizeof(bBroadcast));
	//设置超时
	int nNetTimeout;
	nNetTimeout = 1000;
	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char FAR *)&nNetTimeout, sizeof(nNetTimeout));

	ret = ::bind(m_socket, (sockaddr*)&m_addrfrom, sizeof(sockaddr));
	if (ret == SOCKET_ERROR)
	{
		ret = WSAGetLastError();
		sprintf_s(*errmsg, 2048, "bind()失败 %d!", ret);
		ret = BES200_ERROR_SOCKETFAILED;
		closesocket(m_socket);
		return ret;
	}

	/*广播发送数据*/
	int len = sizeof(sockaddr);
	int res = sendto(m_socket, (char*)SendBuf, SendDataSize, 0, (sockaddr*)&m_addrto, len);
	if (res == SendDataSize)
	{
		RecvDataSize = recvfrom(m_socket, RecvBuf, RECVBUF_SIZE, 0, (sockaddr*)&m_addrfrom, &len);
		if (RecvDataSize == BEGP_RSP_HEAD_SIZE + BEGP_RSP_PACKETDATA_ECHO_SIZE + BEGP_CRC_SIZE)
		{//数据长度正确
			//CRC16校验
			crc = BES200_CRC_16(RecvBuf, RecvDataSize - BEGP_CRC_SIZE);

			if (pHead_rsp->FunctionCode == BEGP_FUNCODE_SET_CONTROLLER_INFO&&\
				pHead_rsp->DataSize == BES200_htonl(BEGP_RSP_PACKETDATA_ECHO_SIZE) && \
				crc == *pCrcRecv)
			{
				if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
				{
					ret = BES200_ERROR_OK;
					sprintf_s(*errmsg, 2048, "发送成功 ret=%d!", ret);

				}

			}
			else{
				res = BES200_ERROR_DATASIZEERR;
				sprintf_s(*errmsg, 2048, " 失败，res = %d", res);

			}
		}
		else{
			res = BES200_ERROR_DATASIZEERR;
			sprintf_s(*errmsg, 2048, " 失败，res = %d", res);

		}
	}
	else{
		res = BES200_ERROR_DATASIZEERR;
		sprintf_s(*errmsg, 2048, " 失败，res = %d", res);

	}
	/*ret = BES200_ERROR_OTHERS;*/

	closesocket(m_socket);
	return ret;
}

int CBES200Manager::BES200_AddMeter(const char *IPAddr, uint32_t MeterID, const struct MeterParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                            *pHead_req;
	BEGP_RSP_HEAD_T                            *pHead_rsp;
	BEGP_REQ_PACKETDATA_ADD_METER_T           *pData_rep;
	BEGP_RSP_PACKETDATA_ECHO_T                 *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize = 0;
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (BEGP_REQ_PACKETDATA_ADD_METER_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_ADD_METER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htonl(BEGP_REQ_PACKETDATA_ADD_METER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_rep->MeterID = BES200_htonl(pParameter->MeterID);
	pData_rep->MeterType = pParameter->MeterType;
	pData_rep->ComRate = pParameter->ComRate;
	pData_rep->ComPort = pParameter->ComPort;
	pData_rep->ComAgreementType = pParameter->ComAgreementType;
	memcpy(pData_rep->PhyAddr, (char *)pParameter->PhyAddr, sizeof(pData_rep->PhyAddr));			//物理地址
	memcpy(pData_rep->Alias, pParameter->Alias, sizeof(pData_rep->Alias));				//模块别名
	memcpy(pData_rep->Description, pParameter->Description, sizeof(pData_rep->Description));		//电表的描述信息
	memcpy(pData_rep->Location, pParameter->Location, sizeof(pData_rep->Location));			//电表的安装位置
	pData_rep->Active = pParameter->Active;//电表的安装位置
	//pParameter->CollectMethod;
	memcpy((void *)&(pData_rep->CollectMethod), (void *)&(pParameter->CollectMethod), sizeof(struct ElectricDataCollectMethod));		//电表的采集方案

	SendDataSize += BEGP_REQ_PACKETDATA_ADD_METER_SIZE;

	ret = BES200_Communication2(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	//通信正确 
	if (ret == BES200_ERROR_OK){
		//正文是否存在错误
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_ADD_METER) && \
			(pHead_rsp->DataSize) == BES200_htonl(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			//
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS){
				sprintf_s(*errmsg, 2048, "Communication OK back Num ： %d", ret);
				ret = BES200_ERROR_OK;
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR){
				ret = BES200_ERROR_DATASIZEERR;
				sprintf_s(*errmsg, 2048, "addmeter datasize Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_MAXMETERCOUNT){
				ret = BES200_ERROR_MAXMETERCOUNT;
				sprintf_s(*errmsg, 2048, "max meter count  Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR){
				ret = BES200_ERROR_DATANOTRIGHT;
				sprintf_s(*errmsg, 2048, "datavalue error, Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_METERALREADYEXIST){
				ret = BES200_ERROR_METERIDEXISTED;
				sprintf_s(*errmsg, 2048, "meter already existed,  Error Num ： %d", ret);
			}
			else{
				ret = BES200_ERROR_REFUSED;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
		}
		else{
			ret = BES200_ERROR_HEAD;
			sprintf_s(*errmsg, 2048, "Communication HEAD Error Num ： %d", ret);
		}
	}
	else{//通信错误

		sprintf_s(*errmsg, 2048, "Communication SYS Error Num ： %d", ret);
	}
	return ret;
}

int CBES200Manager::BES200_SetMeterParameter(const char *IPAddr, uint32_t MeterID, const struct MeterParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                               *pHead_req;
	BEGP_RSP_HEAD_T                               *pHead_rsp;
	BEGP_REQ_PACKETDATA_SET_METER_PARAMETER_T    *pData_rep;
	BEGP_RSP_PACKETDATA_ECHO_T                    *pData_rsp;
	int ret = 0, SendDataSize = 0, RecvDataSize = 0;
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (BEGP_REQ_PACKETDATA_SET_METER_PARAMETER_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_METER_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htonl(BEGP_REQ_PACKETDATA_SET_METER_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_rep->MeterID = BES200_htonl(pParameter->MeterID);
	pData_rep->MeterType = pParameter->MeterType;
	pData_rep->ComRate = pParameter->ComRate;
	pData_rep->ComPort = pParameter->ComPort;
	pData_rep->ComAgreementType = pParameter->ComAgreementType;
	memcpy(pData_rep->PhyAddr, (char *)pParameter->PhyAddr, sizeof(pData_rep->PhyAddr));			//物理地址
	memcpy(pData_rep->Alias, pParameter->Alias, sizeof(pData_rep->Alias));				//模块别名
	memcpy(pData_rep->Description, pParameter->Description, sizeof(pData_rep->Description));		//电表的描述信息
	memcpy(pData_rep->Location, pParameter->Location, sizeof(pData_rep->Location));			//电表的安装位置
	pData_rep->Active = pParameter->Active;														//电表的安装位置
	memcpy((void *)&(pData_rep->CollectMethod), (void *)&(pParameter->CollectMethod), sizeof(struct ElectricDataCollectMethod));		//电表的采集方案

	SendDataSize += BEGP_REQ_PACKETDATA_SET_METER_PARAMETER_SIZE;

	ret = BES200_Communication2(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	//通信正确 
	if (ret == BES200_ERROR_OK){
		//正文是否存在错误
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_SET_METER_PARAMETER) && \
			(pHead_rsp->DataSize) == BES200_htonl(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			//
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS){
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg, 2048, "Communication OK back Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR){
				ret = BES200_ERROR_DATASIZEERR;
				sprintf_s(*errmsg, 2048, "datasize err   Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_METERNOTFOUND){
				ret = BES200_ERROR_METERID_NOTFOUND;
				sprintf_s(*errmsg, 2048, "meter not found   Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR){
				ret = BES200_ERROR_DATANOTRIGHT;
				sprintf_s(*errmsg, 2048, "datavalue error  Error Num ： %d", ret);
			}
			else{
				ret = BES200_ERROR_REFUSED;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
		}
		else{
			ret = BES200_ERROR_HEAD;
			sprintf_s(*errmsg, 2048, "Communication HEAD Error Num ： %d", ret);
		}
	}
	else{//通信错误

		sprintf_s(*errmsg, 2048, "Communication SYS Error Num ： %d", ret);
	}
	return ret;
}

int CBES200Manager::BES200_DeleteMeter(const char *IPAddr, uint32_t MeterID, char **errmsg)
{
	BEGP_REQ_HEAD_T                               *pHead_req;
	BEGP_RSP_HEAD_T                               *pHead_rsp;
	BEGP_REQ_PACKETDATA_DELETE_METER_T            *pData_rep;
	BEGP_RSP_PACKETDATA_ECHO_T                    *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize = 0;
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (BEGP_REQ_PACKETDATA_DELETE_METER_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_DELETE_METER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htonl(BEGP_REQ_PACKETDATA_DELETE_METER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_rep->MeterID = BES200_htonl(MeterID);    //要删除的模块的的ID号

	SendDataSize += BEGP_REQ_PACKETDATA_DELETE_METER_SIZE;

	ret = BES200_Communication2(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	//通信正确 
	if (ret == BES200_ERROR_OK){
		//正文是否存在错误
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_DELETE_METER) && \
			(pHead_rsp->DataSize) == BES200_htonl(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			//
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS){
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg, 2048, "delete ok： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR){
				ret = BES200_ERROR_DATASIZEERR;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_METERNOTFOUND){
				ret = BES200_ERROR_METERID_NOTFOUND;
				sprintf_s(*errmsg, 2048, "Communication meter not found CONTENT Error Num ： %d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR){
				ret = BES200_ERROR_DATANOTRIGHT;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
			else{
				ret = BES200_ERROR_REFUSED;
				sprintf_s(*errmsg, 2048, "Communication CONTENT Error Num ： %d", ret);
			}
		}
		else{
			ret = BES200_ERROR_HEAD;
			sprintf_s(*errmsg, 2048, "Communication HEAD Error Num ： %d", ret);
		}
	}
	else{//通信错误

		sprintf_s(*errmsg, 2048, "Communication SYS Error Num ： %d", ret);
	}
	return ret;
}

int CBES200Manager::BES200_GetMeterParameter(const char *IPAddr, uint32_t MeterID, struct MeterParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                               *pHead_req;
	BEGP_RSP_HEAD_T                               *pHead_rsp;
	BEGP_REQ_PACKETDATA_GET_METER_PARAMETER_T    *pData_rep;
	BEGP_RSP_PACKETDATA_GET_METER_PARAMETER_T    *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize = 0;
	int temp = 0;
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (BEGP_REQ_PACKETDATA_GET_METER_PARAMETER_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_METER_PARAMETER_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_METER_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htonl(BEGP_REQ_PACKETDATA_GET_METER_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_rep->MeterID = BES200_htonl(MeterID);    //要获取模块参数的的ID号

	SendDataSize += BEGP_REQ_PACKETDATA_GET_METER_PARAMETER_SIZE;

	ret = BES200_Communication2(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		temp = BEGP_RSP_PACKETDATA_GET_METER_PARAMETER_SIZE;
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_GET_METER_PARAMETER))//&& \
									//(pHead_rsp->DataSize) == BES200_htonl(BEGP_RSP_PACKETDATA_GET_METER_PARAMETER_SIZE))
		{

			//构造请求正文数据
			pParameter->MeterID = BES200_htonl(pData_rsp->MeterID);
			pParameter->MeterType = pData_rsp->MeterType;
			pParameter->ComRate = pData_rsp->ComRate;
			pParameter->ComPort = pData_rsp->ComPort;
			pParameter->ComAgreementType = pData_rsp->ComAgreementType;
			memcpy((void *)&pParameter->PhyAddr, (void *)&pData_rsp->PhyAddr, sizeof(pData_rsp->PhyAddr));			//物理地址
			memcpy(pParameter->Alias, pData_rsp->Alias, sizeof(pData_rsp->Alias));				//模块别名
			memcpy(pParameter->Description, pData_rsp->Description, sizeof(pData_rsp->Description));		//电表的描述信息
			memcpy(pParameter->Location, pData_rsp->Location, sizeof(pData_rsp->Location));			//电表的安装位置
			pParameter->Active = pData_rsp->Active;														//电表的安装位置
			memcpy((void *)&(pParameter->CollectMethod), (void *)&(pData_rsp->CollectMethod), sizeof(struct ElectricDataCollectMethod));		//电表的采集方案

			ret = BES200_ERROR_OK;
			sprintf_s(*errmsg, 2048, "Communication OK back Num ： %d", ret);

		}
		else{
			ret = BES200_ERROR_HEAD;
			sprintf_s(*errmsg, 2048, "Communication HEAD Error Num ： %d", ret);
		}
	}
	else{
		//错误码处理
		sprintf_s(*errmsg, 2048, "Communication SYS Error Num ： %d", ret);
	}
	return ret;
}

int CBES200Manager::BES200_GetMeterElectricData(const char *IPAddr, uint32_t MeterID, struct MeterElectricData *pElectricData, char **errmsg)
{
	BEGP_REQ_HEAD_T                               *pHead_req;
	BEGP_RSP_HEAD_T                               *pHead_rsp;
	BEGP_REQ_PACKETDATA_GET_METER_RTDATA_T    *pData_rep;
	BEGP_RSP_PACKETDATA_GET_METER_RTDATA_T    *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (BEGP_REQ_PACKETDATA_GET_METER_RTDATA_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_METER_RTDATA_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_METER_RTDATA;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htonl(BEGP_REQ_PACKETDATA_GET_METER_RTDATA_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_rep->MeterID = BES200_htonl(MeterID);    //要获取模块参数的的ID号

	SendDataSize += BEGP_REQ_PACKETDATA_GET_METER_RTDATA_SIZE;

	ret = BES200_Communication2(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_GET_METER_RTDATA) && \
			(pHead_rsp->DataSize) == BES200_htonl(BEGP_RSP_PACKETDATA_GET_METER_RTDATA_SIZE))
		{

			//构造请求正文数据
			pElectricData->MeterID = BES200_htonl(pData_rsp->MeterID);
			int nSize1 = sizeof(pData_rsp->ElectricData);
			int nSize2 = sizeof(pElectricData->ElectricData);
			//下面的这个拷贝 导致 数组越界，因为 最大能耗个数 从100个 修改为10 个
			//memcpy((void *)&pElectricData->ElectricData, (void *)&pData_rsp->ElectricData, sizeof(pData_rsp->ElectricData));			//能耗数据
			memcpy((void *)&pElectricData->ElectricData, (void *)&pData_rsp->ElectricData, sizeof(pElectricData->ElectricData));			//能耗数据

			pElectricData->CollectCount = pData_rsp->CollectCount;
			pElectricData->DateYear = pData_rsp->DateYear;
			pElectricData->DateMonth = pData_rsp->DateMonth;
			pElectricData->DateDay = pData_rsp->DateDay;
			pElectricData->TimeHour = pData_rsp->TimeHour;
			pElectricData->TimeMinute = pData_rsp->TimeMinute;
			pElectricData->TimeSecond = pData_rsp->TimeSecond;//实际能耗个数

			ret = BES200_ERROR_OK;
			sprintf_s(*errmsg, 2048, "Communication OK back Num ： %d", ret);

		}
		else{
			ret = BES200_ERROR_HEAD;
			sprintf_s(*errmsg, 2048, "Communication HEAD Error Num ： %d", ret);
		}
	}
	else{
		//错误码处理
		sprintf_s(*errmsg, 2048, "Communication SYS Error Num ： %d", ret);
	}
	return ret;
}

int CBES200Manager::BES200_GetMeterHisElectricData(const char *IPAddr, uint32_t MeterID, uint8_t Selectday, struct MeterHisElectricData *pHisElectricData, char **errmsg)
{
	BEGP_REQ_HEAD_T                               *pHead_req;
	BEGP_RSP_HEAD_T                               *pHead_rsp;
	BEGP_REQ_PACKETDATA_GET_METER_HISDATA_T    *pData_rep;
	BEGP_RSP_PACKETDATA_GET_METER_HISDATA_T    *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (BEGP_REQ_PACKETDATA_GET_METER_HISDATA_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_METER_HISDATA_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_METER_HISDATA;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htonl(BEGP_REQ_PACKETDATA_GET_METER_HISDATA_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_rep->MeterID = BES200_htonl(MeterID);			//要获取模块参数的的ID号
	pData_rep->SeletDay = Selectday;		//要获取模块参数的的TIAN

	SendDataSize += BEGP_REQ_PACKETDATA_GET_METER_HISDATA_SIZE;

	ret = BES200_Communication2(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_GET_METER_HISDATA) && \
			(pHead_rsp->DataSize) == BES200_htonl(BEGP_RSP_PACKETDATA_GET_METER_HISDATA_SIZE))
		{

			//构造请求正文数据
			pHisElectricData->MeterID = BES200_htonl(pData_rsp->MeterID);
			memcpy((void *)&pHisElectricData->MeterElectricData, (void *)&pData_rsp->MeterElectricData, sizeof(pData_rsp->MeterElectricData));			//能耗数据
			pHisElectricData->HisDataHourCount = pData_rsp->HisDataHourCount;																		  //实际能耗小时个数

			ret = BES200_ERROR_OK;
			sprintf_s(*errmsg, 2048, "Communication OK     Num is ： %d", ret);
		}
		else{
			ret = BES200_ERROR_HEAD;
			sprintf_s(*errmsg, 2048, "Communication HEAD Error Num ： %d", ret);
		}
	}
	else{
		//错误码处理
		sprintf_s(*errmsg, 2048, "Communication SYS Error Num ： %d", ret);
	}
	return ret;
}

int CBES200Manager::BES200_GetAlarmInfor(const char *IPAddr, struct AlarmInfor *AlarmInfor, char **errmsg)
{
	BEGP_REQ_HEAD_T                               *pHead_req;
	BEGP_RSP_HEAD_T                               *pHead_rsp;
	BEGP_RSP_PACKETDATA_GET_ALARMINFO_T           *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE] = { 0 };
	char RecvBuf[RECVBUF_SIZE] = { 0 };

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_ALARMINFO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_ALARMINFO;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htonl(0);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据


	ret = BES200_Communication2(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_GET_ALARMINFO) && \
			(pHead_rsp->DataSize) == BES200_htonl(BEGP_RSP_PACKETDATA_GET_ALARMINFO_SIZE))
		{

			//构造请求正文数据
			AlarmInfor->AlarmInforID = BES200_htonl(pData_rsp->AlarmInforID);
			AlarmInfor->AlarmInforDateYear = pData_rsp->AlarmInforDateYear;
			AlarmInfor->AlarmInforDateMonth = pData_rsp->AlarmInforDateMonth;
			AlarmInfor->AlarmInforDateDay = pData_rsp->AlarmInforDateDay;
			AlarmInfor->AlarmInforTimeHour = pData_rsp->AlarmInforTimeHour;
			AlarmInfor->AlarmInforTimeMinute = pData_rsp->AlarmInforTimeMinute;
			AlarmInfor->AlarmInforTimeSecond = pData_rsp->AlarmInforTimeSecond;
			AlarmInfor->AlarmInforCategory = pData_rsp->AlarmInforCategory;
			AlarmInfor->AlarmInforSeqNum = BES200_htons(pData_rsp->AlarmInforSeqNum);
			AlarmInfor->AlarmInforType = pData_rsp->AlarmInforType;

			ret = BES200_ERROR_OK;

		}
		else{
			ret = BES200_ERROR_HEAD;
			sprintf_s(*errmsg, 2048, "Communication HEAD Error Num ： %d", ret);
		}
	}
	else{
		//错误码处理
		sprintf_s(*errmsg, 2048, "Communication SYS Error Num ： %d", ret);
	}
	return ret;
}
