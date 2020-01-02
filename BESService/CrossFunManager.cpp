#include "stdafx.h"
#include "CrossFunManager.h"
#define  DEF_CROSS_SERVER_PORT 1204
CCrossFunManager::CCrossFunManager()
{
}


CCrossFunManager::~CCrossFunManager()
{
}



#include "stdafx.h"

#include "TCP.h"

// BES200_TEST.cpp : 定义控制台应用程序的入口点。
//

CTCP m_TCP;


#define  my_htons(n) ((n & 0xff) << 8) | ((n & 0xff00) >> 8)
uint16_t  ttBES200_htons(uint16_t n)
{
	return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}
uint16_t CCrossFunManager::BES200_htons(uint16_t n)
{
	return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}

uint16_t CCrossFunManager::BES200_ntohs(uint16_t n)
{
	return BES200_htons(n);
}


uint32_t BES200_htonl(uint32_t n)
{
	return ((n & 0xff) << 24) |
		((n & 0xff00) << 8) |
		((n & 0xff0000UL) >> 8) |
		((n & 0xff000000UL) >> 24);
}

uint32_t BES200_ntohl(uint32_t n)
{
	return BES200_htonl(n);
}

uint16_t BES200_CRC_16(const char *cBuf, int cLen)
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

void CCrossFunManager :: BES200_Free(char **errmsg)
{
	free(*errmsg);
}

static int BES200_RecvAFrame(const char ServerIP[], char *pRecvData)
{
	int DataSize = 0, FrameSize, len;
	BEGP_RSP_HEAD_T *pHead_rsp;

	pHead_rsp = (BEGP_RSP_HEAD_T*)pRecvData;

	/*接收包头*/
	len = m_TCP.Recv(ServerIP, DEF_CROSS_SERVER_PORT,(void*)pRecvData, BEGP_RSP_HEAD_SIZE);
	if (len != BEGP_RSP_HEAD_SIZE)
	{
		return -1;
	}


	DataSize += BEGP_RSP_HEAD_SIZE;
	FrameSize = ttBES200_htons(pHead_rsp->DataSize) + BEGP_CRC_SIZE;

	/*接收正文数据和校验数据*/
	len = m_TCP.Recv(ServerIP, DEF_CROSS_SERVER_PORT, (void*)(pRecvData + BEGP_RSP_HEAD_SIZE), FrameSize);
	if (len != FrameSize)
	{
		return -1;
	}

	DataSize += FrameSize;

	return DataSize;
}

#include <stdio.h>
static int BES200_Communication(const char ServerIP[], const char *pSendData, int SendDataSize, char *pRecvData, int *RecvDataSize)
{
	uint16_t *crc_req, crc1, crc2;
	int size, trytimes = 0;

	crc_req = (uint16_t*)(pSendData + SendDataSize);
	*crc_req = BES200_CRC_16(pSendData, SendDataSize);
	SendDataSize += BEGP_CRC_SIZE;
again:
	if (m_TCP.Connect(ServerIP, DEF_CROSS_SERVER_PORT) == -1)
		return BES200_ERROR_CONNECT;

	if (m_TCP.Send(ServerIP, DEF_CROSS_SERVER_PORT, (void*)pSendData, SendDataSize) != SendDataSize)
	{
		m_TCP.DisConnect(ServerIP, DEF_CROSS_SERVER_PORT);
		if (m_TCP.Connect(ServerIP, DEF_CROSS_SERVER_PORT) == -1)
			return BES200_ERROR_CONNECT;
		if (m_TCP.Send(ServerIP, DEF_CROSS_SERVER_PORT, (void*)pSendData, SendDataSize) != SendDataSize)
		{
			m_TCP.DisConnect(ServerIP, DEF_CROSS_SERVER_PORT);
			return  BES200_ERROR_SEND;
		}
	}

	size = BES200_RecvAFrame(ServerIP, pRecvData);
	if (size == -1)
	{
		trytimes++;
		m_TCP.DisConnect(ServerIP, DEF_CROSS_SERVER_PORT);
		if (trytimes <= 1)
			goto again;
		else
			return BES200_ERROR_RECIVE;
	}
	*RecvDataSize = size;
	/*收到一帧数据，开始处理*/
	crc1 = BES200_CRC_16(pRecvData, (*RecvDataSize) - BEGP_CRC_SIZE);
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
/*新增一个控制器*/
int CCrossFunManager:: BES200_AddControllerParameter(const char *IPAddr, const struct ControllerParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T *pData_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);
	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;



	//正文数据，指针赋值
	pData_req = (BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T*)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_ADD_CONTROLLER_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_req->Active = pParameter->Active;
	memcpy(pData_req->Alias, pParameter->Alias, sizeof(pData_req->Alias));
	memcpy(pData_req->Description, pParameter->Description, sizeof(pData_req->Description));
	pData_req->ID = BES200_htons(pParameter->ID);
	memcpy(pData_req->Location, pParameter->Location, sizeof(pData_req->Location));
	memcpy(pData_req->Name, pParameter->Name, sizeof(pData_req->Name));
	memcpy(pData_req->Zone, pParameter->Zone, sizeof(pData_req->Zone));

	SendDataSize += BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_ADD_CONTROLLER_PARAMETER) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED ;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}
/*修改控制器的所有参数*/
int CCrossFunManager:: BES200_SetControllerParameter(const char *IPAddr, const struct ControllerParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_T *pData_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

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
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_req->Active = pParameter->Active;
	memcpy(pData_req->Alias, pParameter->Alias, sizeof(pData_req->Alias));
	memcpy(pData_req->Description, pParameter->Description, sizeof(pData_req->Description));
	pData_req->ID = BES200_htons(pParameter->ID);
	memcpy(pData_req->Location, pParameter->Location, sizeof(pData_req->Location));
	memcpy(pData_req->Name, pParameter->Name, sizeof(pData_req->Name));
	memcpy(pData_req->Zone, pParameter->Zone, sizeof(pData_req->Zone));

	SendDataSize += BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_SET_CONTROLLER_PARAMETER) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}

/* 设置控制器的时间 */
int CCrossFunManager:: BES200_SetControllerTime(const char *IPAddr, const struct tm *pTime, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_T      *pData_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

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
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_req->year = pTime->tm_year - 2000;//年
	pData_req->month = pTime->tm_mon;  //月
	pData_req->day = pTime->tm_mday;  //日
	pData_req->hour = pTime->tm_hour;  //时
	pData_req->minute = pTime->tm_min; //分
	pData_req->second = pTime->tm_sec;  //秒
	if (pTime->tm_wday == 0)
	{
		pData_req->wday = 7;
	}
	else
	{
		pData_req->wday = pTime->tm_wday;
	}

	SendDataSize += BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_SET_CONTROLLER_TIME) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}

/*重启DDC,相当于软件复位*/
int CCrossFunManager:: BES200_RebootController(const char *IPAddr, char **errmsg)
{
	BEGP_REQ_HEAD_T                           *pHead_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize = 0;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_REBOOT_CONTROLLER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(0);

	SendDataSize += BEGP_REQ_HEAD_SIZE;
	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_REBOOT_CONTROLLER) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}

	return ret;
}
/*重置控制器，恢复出厂设置，并重启*/
int CCrossFunManager:: BES200_ResetControler(const char *IPAddr, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize = 0;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_RESET_CONTROLLER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(0);

	SendDataSize += BEGP_REQ_HEAD_SIZE;
	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_RESET_CONTROLLER) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}

	return ret;
}

/*远程升级*/
int CCrossFunManager:: BES200_FunIAP(const char *IPAddr, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize = 0;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCUDE_IAP;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(0);

	SendDataSize += BEGP_REQ_HEAD_SIZE;
	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);

	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCUDE_IAP) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}

	return ret;

}
/*删除控制器*/
int CCrossFunManager:: BES200_DeleteController(const char *IPAddr, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

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
	pHead_req->DataSize = BES200_htons(0);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_DELETE_CONTROLLER) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}
/*获取控制器的时间*/
int CCrossFunManager:: BES200_GetControllerTime(const char *IPAddr, struct tm *pTime, char **errmsg)
{
	BEGP_REQ_HEAD_T                              *pHead_req;
	BEGP_RSP_HEAD_T                              *pHead_rsp;
	BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_T    *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_CONTROLLER_TIME;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(0);
	SendDataSize += BEGP_REQ_HEAD_SIZE;


	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);

	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_GET_CONTROLLER_TIME) && \
			(BES200_htons(pHead_rsp->DataSize) >= BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_SIZE))
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
			sprintf_s(*errmsg,2048, "err:%d", ret);
		}
		else
		{
			ret = BES200_ERROR_REFUSED;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}

/*获取控制器的所有配置参数*/
int CCrossFunManager:: BES200_GetControllerParameter(const char *IPAddr, struct ControllerParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                                  *pHead_req;
	BEGP_RSP_HEAD_T                                  *pHead_rsp;
	BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_T    *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];


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
	pHead_req->DataSize = BES200_htons(0);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);

	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_GET_CONTROLLER_PARAMETER) && \
			(BES200_htons(pHead_rsp->DataSize) >= BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_SIZE))
		{
			pParameter->Active = pData_rsp->Active;  //是否启用
			memcpy(pParameter->Alias, pData_rsp->Alias, sizeof(pParameter->Alias));  //DDC别名
			memcpy(pParameter->Description, pData_rsp->Description, sizeof(pParameter->Description));  //描述
			pParameter->ID = pData_rsp->ID; //ID号
			memcpy(pParameter->Location, pData_rsp->Location, sizeof(pParameter->Location));  //安装位置
			memcpy(pParameter->Name, pData_rsp->Name, sizeof(pParameter->Name)); //名字
			memcpy(pParameter->Zone, pData_rsp->Zone, sizeof(pParameter->Zone)); //区域
			ret = BES200_ERROR_OK;
			sprintf_s(*errmsg,2048, "err:%d", ret);
		}
		else
		{
			ret = BES200_ERROR_REFUSED;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}
/*广播方式获取所有控制器信息*/
int CCrossFunManager:: BES200_GetControllerInfoByBroadcast(struct ControllerInfo **pInfo, int *size, char **errmsg)
{
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];
	SOCKET m_socket;
	SOCKADDR_IN m_addrfrom, m_addrto;
	uint16_t *pCrcSend, *pCrcRecv, crc;
	int ret, SendDataSize, RecvDataSize;
	BEGP_REQ_HEAD_T                                  *pHead_req;
	BEGP_RSP_HEAD_T                                  *pHead_rsp;
	BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_T        *pData_rsp;


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
	*pInfo = (struct ControllerInfo *)malloc(sizeof(struct ControllerInfo) * 1000);//最多支持1000个设备
	if (*errmsg == NULL)
		return BES200_ERROR_CONNECT;

	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_socket)
	{
		ret = WSAGetLastError();
		sprintf_s(*errmsg,2048, "socket()失败 :%d", ret);
		WSACleanup();
		return BES200_ERROR_CONNECT;
	}

	/*客户端地址*/
	m_addrfrom.sin_family = AF_INET;
	m_addrfrom.sin_port = htons(1204);
	m_addrfrom.sin_addr.S_un.S_addr = INADDR_ANY;

	/*服务端地址*/
	m_addrto.sin_family = AF_INET;
	m_addrto.sin_port = htons(1204);
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
		sprintf_s(*errmsg,2048, "bind()失败 %d!", ret);
		ret = BES200_ERROR_CONNECT;
		goto out;
	}

	int len = sizeof(sockaddr);
	int res = sendto(m_socket, (char*)SendBuf, SendDataSize, 0, (sockaddr*)&m_addrto, len);

	if (res == (BEGP_REQ_HEAD_SIZE + BEGP_CRC_SIZE))
	{
		int index = 0;
		//一直接收，直到接收超时，代表后续没有数据
		while (1)
		{
			RecvDataSize = recvfrom(m_socket, RecvBuf, RECVBUF_SIZE, 0, (sockaddr*)&m_addrfrom, &len);
			if (RecvDataSize == -1 || index >= 1000)
			{
				break;
			}
			else if (RecvDataSize == BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_SIZE + BEGP_CRC_SIZE + BEGP_RSP_HEAD_SIZE)
			{//数据长度正确
				//CRC16校验
				crc = BES200_CRC_16(RecvBuf, RecvDataSize - BEGP_CRC_SIZE);

				if (pHead_rsp->FunctionCode == BEGP_FUNCODE_GET_CONTROLLER_INFO&&\
					pHead_rsp->DataSize == BES200_htons(BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_SIZE) && \
					crc == *pCrcRecv)
				{
					struct ControllerInfo *ptr = *pInfo;
					memcpy(ptr[index].Name, pData_rsp->Name, sizeof(pData_rsp->Name));
					memcpy(ptr[index].Model, pData_rsp->Model, sizeof(pData_rsp->Model));
					memcpy(ptr[index].Mask, pData_rsp->Mask, sizeof(pData_rsp->Mask));
					memcpy(ptr[index].Mac, pData_rsp->Mac, sizeof(pData_rsp->Mac));
					memcpy(ptr[index].IP, pData_rsp->IP, sizeof(pData_rsp->IP));
					ptr[index].HWVer = pData_rsp->HWVer;
					ptr[index].FWVer = pData_rsp->FWVer;
					memcpy(ptr[index].GateWay, pData_rsp->GateWay, sizeof(pData_rsp->GateWay));
					memcpy(ptr[index].CampanyName, pData_rsp->CampanyName, sizeof(pData_rsp->CampanyName));
					*size = ++index;
				}
			}
		}

	}
	else
	{
		sprintf_s(*errmsg,2048, "sendto len = %d失败，res = %d", sizeof(BEGP_REQ_HEAD_T), res);
		ret = BES200_ERROR_SEND;
	}
out:
	closesocket(m_socket);
	return ret;
}

/*广播方式根据MAC地址设置控制器的IP地址等信息*/
int CCrossFunManager:: BES200_SetControllerInfoByBroadcast(const struct ControllerInfo *pInfo, char **errmsg)
{
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];
	uint16_t *pCrcSend, *pCrcRecv, crc;
	SOCKET m_socket;
	SOCKADDR_IN m_addrfrom, m_addrto;
	int ret, SendDataSize, RecvDataSize;
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
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_SIZE);
	SendDataSize = BEGP_REQ_HEAD_SIZE;

	/*正文数据赋值*/
	memcpy(pData_req->Name, pInfo->Name, sizeof(pInfo->Name));
	memcpy(pData_req->Model, pInfo->Model, sizeof(pInfo->Model));
	memcpy(pData_req->Mask, pInfo->Mask, sizeof(pInfo->Mask));
	memcpy(pData_req->Mac, pInfo->Mac, sizeof(pInfo->Mac));
	memcpy(pData_req->IP, pInfo->IP, sizeof(pInfo->IP));
	pData_req->HWVer = pInfo->HWVer;
	pData_req->FWVer = pInfo->FWVer;
	memcpy(pData_req->GateWay, pInfo->GateWay, sizeof(pInfo->GateWay));
	memcpy(pData_req->CampanyName, pInfo->CampanyName, sizeof(pInfo->CampanyName));
	SendDataSize += BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_SIZE;

	/*计算CRC*/
	*pCrcSend = BES200_CRC_16(SendBuf, SendDataSize);
	SendDataSize += BEGP_CRC_SIZE;

	*errmsg = (char *)malloc(1024);
	if (*errmsg == NULL)
		return BES200_ERROR_CONNECT;

	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_socket)
	{
		ret = WSAGetLastError();
		sprintf_s(*errmsg,2048, "socket()失败 :%d", ret);
		WSACleanup();
		return BES200_ERROR_CONNECT;
	}

	/*客户端地址*/
	m_addrfrom.sin_family = AF_INET;
	m_addrfrom.sin_port = htons(1204);
	m_addrfrom.sin_addr.S_un.S_addr = INADDR_ANY;

	/*服务端地址*/
	m_addrto.sin_family = AF_INET;
	m_addrto.sin_port = htons(1204);
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
		sprintf_s(*errmsg,2048, "bind()失败 %d!", ret);
		ret = BES200_ERROR_CONNECT;
		goto out;
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
				pHead_rsp->DataSize == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE) && \
				crc == *pCrcRecv)
			{
				if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
				{
					ret = BES200_ERROR_OK;
					goto out;
				}

			}
		}
	}
	ret = BES200_ERROR_OTHERS;
out:
	closesocket(m_socket);
	return ret;
}

/*增加一个模块*/
int CCrossFunManager:: BES200_AddModule(const char *IPAddr, uint16_t ModuleID, const struct ModuleParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                            *pHead_req;
	BEGP_RSP_HEAD_T                            *pHead_rsp;
	BEGP_REQ_PACKETDATA_ADD_MODULE_T           *pData_rep;
	BEGP_RSP_PACKETDATA_ECHO_T                 *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (BEGP_REQ_PACKETDATA_ADD_MODULE_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_ADD_MODULE;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_ADD_MODULE_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	memcpy(pData_rep->Name, pParameter->Name, sizeof(pData_rep->Name));//模块名字
	memcpy(pData_rep->Alias, pParameter->Alias, sizeof(pData_rep->Alias));//模块别名
	memcpy(pData_rep->Description, pParameter->Description, sizeof(pData_rep->Description));//模块的描述信息
	memcpy(pData_rep->Location, pParameter->Location, sizeof(pData_rep->Location));//模块的安装位置
	pData_rep->ModelID = pParameter->ModelID; //模块型号编码
	pData_rep->FlnID = pParameter->FlnID;
	pData_rep->SlaveAddress = pParameter->SlaveAddress;
	pData_rep->Active = pParameter->Active;
	pData_rep->ID = BES200_htons(ModuleID);

	SendDataSize += BEGP_REQ_PACKETDATA_ADD_MODULE_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);

	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_ADD_MODULE) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}
/*配置模块的所有配置参数*/
int CCrossFunManager:: BES200_SetModuleParameter(const char *IPAddr, uint16_t ModuleID, const struct ModuleParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                               *pHead_req;
	BEGP_RSP_HEAD_T                               *pHead_rsp;
	BEGP_REQ_PACKETDATA_SET_MODULE_PARAMETER_T    *pData_rep;
	BEGP_RSP_PACKETDATA_ECHO_T                    *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (BEGP_REQ_PACKETDATA_SET_MODULE_PARAMETER_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_MODULE_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_MODULE_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	memcpy(pData_rep->Name, pParameter->Name, sizeof(pData_rep->Name));//模块名字
	memcpy(pData_rep->Alias, pParameter->Alias, sizeof(pData_rep->Alias));//模块别名
	memcpy(pData_rep->Description, pParameter->Description, sizeof(pData_rep->Description));//模块的描述信息
	memcpy(pData_rep->Location, pParameter->Location, sizeof(pData_rep->Location));//模块的安装位置
	pData_rep->ModelID = pParameter->ModelID; //模块型号编码
	pData_rep->FlnID = pParameter->FlnID;
	pData_rep->SlaveAddress = pParameter->SlaveAddress;
	pData_rep->Active = pParameter->Active;
	pData_rep->ID = BES200_htons(ModuleID);

	SendDataSize += BEGP_REQ_PACKETDATA_SET_MODULE_PARAMETER_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_SET_MODULE_PARAMETER) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}
/*删除一个模块，并删除和模块相关的点*/
int CCrossFunManager:: BES200_DeleteModule(const char *IPAddr, uint16_t ModuleID, char **errmsg)
{
	BEGP_REQ_HEAD_T                               *pHead_req;
	BEGP_RSP_HEAD_T                               *pHead_rsp;
	BEGP_REQ_PACKETDATA_DELETE_MODULE_T           *pData_rep;
	BEGP_RSP_PACKETDATA_ECHO_T                    *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (BEGP_REQ_PACKETDATA_DELETE_MODULE_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_DELETE_MODULE;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_DELETE_MODULE_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_rep->ID = BES200_htons(ModuleID);    //要删除的模块的的ID号

	SendDataSize += BEGP_REQ_PACKETDATA_DELETE_MODULE_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_DELETE_MODULE) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}
/* 获取模块的所有配置信息 */
int CCrossFunManager:: BES200_GetModuleParameter(const char *IPAddr, uint16_t ModuleID, struct ModuleParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                               *pHead_req;
	BEGP_RSP_HEAD_T                               *pHead_rsp;
	BEGP_REQ_PACKETDATA_GET_MODULE_PARAMETER_T    *pData_rep;
	BEGP_RSP_PACKETDATA_GET_MODULE_PARAMETER_T    *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (BEGP_REQ_PACKETDATA_GET_MODULE_PARAMETER_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_MODULE_PARAMETER_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_MODULE_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_GET_MODULE_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_rep->ID = BES200_htons(ModuleID);    //要获取模块参数的的ID号

	SendDataSize += BEGP_REQ_PACKETDATA_GET_MODULE_PARAMETER_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_GET_MODULE_PARAMETER) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_GET_MODULE_PARAMETER_SIZE))
		{

			ret = BES200_ERROR_OK;
			sprintf_s(*errmsg,2048, "err:%d", ret);

			memcpy(pParameter->Name, pData_rsp->Name, sizeof(pParameter->Name));//模块名字
			memcpy(pParameter->Alias, pData_rsp->Alias, sizeof(pParameter->Alias));//模块别名
			memcpy(pParameter->Description, pData_rsp->Description, sizeof(pParameter->Description));//模块的描述信息
			memcpy(pParameter->Location, pData_rsp->Location, sizeof(pParameter->Location));//模块的安装位置
			pParameter->ModelID = pData_rsp->ModelID; //模块型号编码
			pParameter->FlnID = pData_rsp->FlnID;//总线
			pParameter->SlaveAddress = pData_rsp->SlaveAddress; //485通信地址
			pParameter->Active = pData_rsp->Active;  //是否启用
			pParameter->ID = BES200_htons(pData_rsp->ID);  //模块的ID号
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}
/*获取模块的错误代码*/
int CCrossFunManager:: BES200_GetModuleErrorCode(const char *IPAddr, uint16_t ModuleID, uint8_t *ErrorCode, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_REQ_PACKETDATA_GET_MODULE_ERROR_CODE_T    *pData_rep;
	BEGP_RSP_PACKETDATA_GET_MODULE_ERROR_CODE_T    *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (BEGP_REQ_PACKETDATA_GET_MODULE_ERROR_CODE_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_MODULE_ERROR_CODE_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_MODULE_ERROR_CODE;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_GET_MODULE_ERROR_CODE_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_rep->ID = BES200_htons(ModuleID);    //要获取模块参数的的ID号

	SendDataSize += BEGP_REQ_PACKETDATA_GET_MODULE_ERROR_CODE_SIZE;
	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_GET_MODULE_ERROR_CODE) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_GET_MODULE_ERROR_CODE_SIZE))
		{

			ret = BES200_ERROR_OK;
			sprintf_s(*errmsg,2048, "err:%d", ret);

			*ErrorCode = pData_rsp->ErrorCode;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}
/*添加一个逻辑点*/
int CCrossFunManager:: BES200_AddLogicalPoint(const char *IPAddr, uint16_t LogicalPointID, const struct LogicalPointParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_REQ_PACKETDATA_ADD_LOGICAL_POINT_T        *pData_req;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_req = (BEGP_REQ_PACKETDATA_ADD_LOGICAL_POINT_T*)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_ADD_LOGICAL_POINT;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_ADD_LOGICAL_POINT_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	if (pParameter->PointType == POINT_TYPE_LAI) //实点AI
	{
		BEGP_LOGICAL_POINT_PARAMETER_LAI_T        *pParameterLAI;
		BEGP_LOGICAL_POINT_PARAMETER_LAI_T        *pDataLAI_req;

		pDataLAI_req = (BEGP_LOGICAL_POINT_PARAMETER_LAI_T*)pData_req;
		pParameterLAI = (BEGP_LOGICAL_POINT_PARAMETER_LAI_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

		pDataLAI_req->ID = BES200_htons(LogicalPointID);  //逻辑点的ID
		pDataLAI_req->PointType = pParameterLAI->PointType;  //点的类型
		pDataLAI_req->Active = pParameterLAI->Active;//是否启用
		memcpy(pDataLAI_req->Name, pParameterLAI->Name, sizeof(pDataLAI_req->Name));  //名字
		memcpy(pDataLAI_req->Alias, pParameterLAI->Alias, sizeof(pDataLAI_req->Alias)); //别名
		memcpy(pDataLAI_req->Description, pParameterLAI->Description, sizeof(pDataLAI_req->Description)); //描述
		pDataLAI_req->ModuleID = BES200_htons(pParameterLAI->ModuleID);  //点所在模块ID
		pDataLAI_req->ChannelIndex = pParameterLAI->ChannelIndex;//点所在的通道索引
		pDataLAI_req->WorkMode = pParameterLAI->WorkMode; //工作模式
		pDataLAI_req->Polarity = pParameterLAI->Polarity; //是否反向
		pDataLAI_req->InitValue = BES200_htonl(pParameterLAI->InitValue);//值

		pDataLAI_req->AlarmActive = pParameterLAI->AlarmActive;//报警是否启用
		pDataLAI_req->AlarmType = pParameterLAI->AlarmType; //报警类型
		pDataLAI_req->AlarmPriority = pParameterLAI->AlarmPriority;//报警优先级

		pDataLAI_req->LineType = pParameterLAI->LineType;//模拟量输入类型
		memcpy(pDataLAI_req->Unit, pParameterLAI->Unit, sizeof(pDataLAI_req->Unit)); //工程单位
		pDataLAI_req->HighRange = BES200_htonl(pParameterLAI->HighRange);//32位数据大小端转换
		pDataLAI_req->LowRange = BES200_htonl(pParameterLAI->LowRange);//32位数据大小端转换
		pDataLAI_req->Precision = pParameterLAI->Precision;//精度
		pDataLAI_req->AlarmHighValue = BES200_htonl(pParameterLAI->AlarmHighValue);//32位数据大小端转换
		pDataLAI_req->AlarmLowValue = BES200_htonl(pParameterLAI->AlarmLowValue);//32位数据大小端转换
	}
	//构造请求正文数据
	else if (pParameter->PointType == POINT_TYPE_LAO) //实点AO
	{
		BEGP_LOGICAL_POINT_PARAMETER_LAO_T        *pParameterLAO;
		BEGP_LOGICAL_POINT_PARAMETER_LAO_T        *pDataLAO_req;

		pDataLAO_req = (BEGP_LOGICAL_POINT_PARAMETER_LAO_T*)pData_req;
		pParameterLAO = (BEGP_LOGICAL_POINT_PARAMETER_LAO_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

		pDataLAO_req->ID = BES200_htons(LogicalPointID);  //逻辑点的ID
		pDataLAO_req->PointType = pParameterLAO->PointType;  //点的类型
		pDataLAO_req->Active = pParameterLAO->Active;//是否启用
		memcpy(pDataLAO_req->Name, pParameterLAO->Name, sizeof(pDataLAO_req->Name));  //名字
		memcpy(pDataLAO_req->Alias, pParameterLAO->Alias, sizeof(pDataLAO_req->Alias)); //别名
		memcpy(pDataLAO_req->Description, pParameterLAO->Description, sizeof(pDataLAO_req->Description)); //描述
		pDataLAO_req->ModuleID = BES200_htons(pParameterLAO->ModuleID);  //点所在模块ID
		pDataLAO_req->ChannelIndex = pParameterLAO->ChannelIndex;//点所在的通道索引
		pDataLAO_req->WorkMode = pParameterLAO->WorkMode; //工作模式
		pDataLAO_req->Polarity = pParameterLAO->Polarity; //是否反向
		pDataLAO_req->InitValue = BES200_htonl(pParameterLAO->InitValue); //初始值

		pDataLAO_req->AlarmActive = pParameterLAO->AlarmActive;//报警是否启用
		pDataLAO_req->AlarmType = pParameterLAO->AlarmType; //报警类型
		pDataLAO_req->AlarmPriority = pParameterLAO->AlarmPriority;//报警优先级

		pDataLAO_req->LineType = pParameterLAO->LineType;//模拟量输出类型
		memcpy(pDataLAO_req->Unit, pParameterLAO->Unit, sizeof(pDataLAO_req->Unit)); //工程单位
		pDataLAO_req->HighRange = BES200_htonl(pParameterLAO->HighRange);//32位数据大小端转换
		pDataLAO_req->LowRange = BES200_htonl(pParameterLAO->LowRange);//32位数据大小端转换
		pDataLAO_req->Precision = pParameterLAO->Precision;//精度
		pDataLAO_req->AlarmHighValue = BES200_htonl(pParameterLAO->AlarmHighValue);//32位数据大小端转换
		pDataLAO_req->AlarmLowValue = BES200_htonl(pParameterLAO->AlarmLowValue);//32位数据大小端转换
	}
	else if (pParameter->PointType == POINT_TYPE_LDI) //实点DI
	{
		BEGP_LOGICAL_POINT_PARAMETER_LDI_T        *pParameterLDI;
		BEGP_LOGICAL_POINT_PARAMETER_LDI_T        *pDataLDI_req;

		pDataLDI_req = (BEGP_LOGICAL_POINT_PARAMETER_LDI_T*)pData_req;
		pParameterLDI = (BEGP_LOGICAL_POINT_PARAMETER_LDI_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

		pDataLDI_req->ID = BES200_htons(LogicalPointID);  //逻辑点的ID
		pDataLDI_req->PointType = pParameterLDI->PointType;  //点的类型
		pDataLDI_req->Active = pParameterLDI->Active;//是否启用
		memcpy(pDataLDI_req->Name, pParameterLDI->Name, sizeof(pDataLDI_req->Name));  //名字
		memcpy(pDataLDI_req->Alias, pParameterLDI->Alias, sizeof(pDataLDI_req->Alias)); //别名
		memcpy(pDataLDI_req->Description, pParameterLDI->Description, sizeof(pDataLDI_req->Description)); //描述
		pDataLDI_req->ModuleID = BES200_htons(pParameterLDI->ModuleID);  //点所在模块ID
		pDataLDI_req->ChannelIndex = pParameterLDI->ChannelIndex;//点所在的通道索引
		pDataLDI_req->WorkMode = pParameterLDI->WorkMode; //工作模式
		pDataLDI_req->Polarity = pParameterLDI->Polarity; //是否反向
		pDataLDI_req->InitValue = BES200_htonl(pParameterLDI->InitValue);//值

		pDataLDI_req->AlarmActive = pParameterLDI->AlarmActive;//报警是否启用
		pDataLDI_req->AlarmType = pParameterLDI->AlarmType; //报警类型
		pDataLDI_req->AlarmPriority = pParameterLDI->AlarmPriority;//报警优先级

		pDataLDI_req->AlarmTrigger = pParameterLDI->AlarmTrigger;//报警触发
		pDataLDI_req->ActivePassive = pParameterLDI->ActivePassive;//有源无源
	}
	else if (pParameter->PointType == POINT_TYPE_LDO) //实点DO
	{
		BEGP_LOGICAL_POINT_PARAMETER_LDO_T        *pParameterLDO;
		BEGP_LOGICAL_POINT_PARAMETER_LDO_T        *pDataLDO_req;

		pDataLDO_req = (BEGP_LOGICAL_POINT_PARAMETER_LDO_T*)pData_req;
		pParameterLDO = (BEGP_LOGICAL_POINT_PARAMETER_LDO_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

		pDataLDO_req->ID = BES200_htons(LogicalPointID);  //逻辑点的ID
		pDataLDO_req->PointType = pParameterLDO->PointType;  //点的类型
		pDataLDO_req->Active = pParameterLDO->Active;//是否启用
		memcpy(pDataLDO_req->Name, pParameterLDO->Name, sizeof(pDataLDO_req->Name));  //名字
		memcpy(pDataLDO_req->Alias, pParameterLDO->Alias, sizeof(pDataLDO_req->Alias)); //别名
		memcpy(pDataLDO_req->Description, pParameterLDO->Description, sizeof(pDataLDO_req->Description)); //描述
		pDataLDO_req->ModuleID = BES200_htons(pParameterLDO->ModuleID);  //点所在模块ID
		pDataLDO_req->ChannelIndex = pParameterLDO->ChannelIndex;//点所在的通道索引
		pDataLDO_req->WorkMode = pParameterLDO->WorkMode; //工作模式
		pDataLDO_req->Polarity = pParameterLDO->Polarity; //是否反向
		pDataLDO_req->InitValue = BES200_htonl(pParameterLDO->InitValue); //是否反向

		pDataLDO_req->AlarmActive = pParameterLDO->AlarmActive;//报警是否启用
		pDataLDO_req->AlarmType = pParameterLDO->AlarmType; //报警类型
		pDataLDO_req->AlarmPriority = pParameterLDO->AlarmPriority;//报警优先级

		pDataLDO_req->AlarmTrigger = pParameterLDO->AlarmTrigger;//报警触发
	}
	else if (pParameter->PointType == POINT_TYPE_VIRTUAL_AI) //虚点AI
	{
		BEGP_LOGICAL_POINT_PARAMETER_VAI_T        *pParameterVAI;
		BEGP_LOGICAL_POINT_PARAMETER_VAI_T        *pDataVAI_req;

		pDataVAI_req = (BEGP_LOGICAL_POINT_PARAMETER_VAI_T*)pData_req;
		pParameterVAI = (BEGP_LOGICAL_POINT_PARAMETER_VAI_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

		pDataVAI_req->ID = BES200_htons(LogicalPointID);  //逻辑点的ID
		pDataVAI_req->PointType = pParameterVAI->PointType;  //点的类型
		pDataVAI_req->Active = pParameterVAI->Active;//是否启用
		memcpy(pDataVAI_req->Name, pParameterVAI->Name, sizeof(pDataVAI_req->Name));  //名字
		memcpy(pDataVAI_req->Alias, pParameterVAI->Alias, sizeof(pDataVAI_req->Alias)); //别名
		memcpy(pDataVAI_req->Description, pParameterVAI->Description, sizeof(pDataVAI_req->Description)); //描述
		pDataVAI_req->InitValue = BES200_htonl(pParameterVAI->InitValue);  //初始值
		memcpy(pDataVAI_req->Unit, pParameterVAI->Unit, sizeof(pDataVAI_req->Unit)); //单位
		pDataVAI_req->Precision = pParameterVAI->Precision;  //精度

		/*报警相关*/
		pDataVAI_req->AlarmActive = pParameterVAI->AlarmActive;//报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		pDataVAI_req->AlarmType = pParameterVAI->AlarmType;//报警类型，不报警、标准报警、加强报警
		pDataVAI_req->AlarmPriority = pParameterVAI->AlarmPriority;//报警优先级
		pDataVAI_req->AlarmHighValue = pParameterVAI->AlarmHighValue;
		pDataVAI_req->AlarmLowValue = pParameterVAI->AlarmLowValue;
	}
	else if (pParameter->PointType == POINT_TYPE_VIRTUAL_AO) //虚点AO
	{
		BEGP_LOGICAL_POINT_PARAMETER_VAO_T        *pParameterVAO;
		BEGP_LOGICAL_POINT_PARAMETER_VAO_T        *pDataVAO_req;

		pDataVAO_req = (BEGP_LOGICAL_POINT_PARAMETER_VAO_T*)pData_req;
		pParameterVAO = (BEGP_LOGICAL_POINT_PARAMETER_VAO_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

		pDataVAO_req->ID = BES200_htons(LogicalPointID);  //逻辑点的ID
		pDataVAO_req->PointType = pParameterVAO->PointType;  //点的类型
		pDataVAO_req->Active = pParameterVAO->Active;//是否启用
		memcpy(pDataVAO_req->Name, pParameterVAO->Name, sizeof(pDataVAO_req->Name));  //名字
		memcpy(pDataVAO_req->Alias, pParameterVAO->Alias, sizeof(pDataVAO_req->Alias)); //别名
		memcpy(pDataVAO_req->Description, pParameterVAO->Description, sizeof(pDataVAO_req->Description)); //描述
		pDataVAO_req->InitValue = BES200_htonl(pParameterVAO->InitValue);  //初始值
		memcpy(pDataVAO_req->Unit, pParameterVAO->Unit, sizeof(pDataVAO_req->Unit)); //单位
		pDataVAO_req->Precision = pParameterVAO->Precision;  //精度

		/*报警相关*/
		pDataVAO_req->AlarmActive = pParameterVAO->AlarmActive;//报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		pDataVAO_req->AlarmType = pParameterVAO->AlarmType;//报警类型，不报警、标准报警、加强报警
		pDataVAO_req->AlarmPriority = pParameterVAO->AlarmPriority;//报警优先级
		pDataVAO_req->AlarmHighValue = pParameterVAO->AlarmHighValue;
		pDataVAO_req->AlarmLowValue = pParameterVAO->AlarmLowValue;
	}
	else if (pParameter->PointType == POINT_TYPE_VIRTUAL_DI) //虚点DI
	{
		BEGP_LOGICAL_POINT_PARAMETER_VDI_T        *pParameterVDI;
		BEGP_LOGICAL_POINT_PARAMETER_VDI_T        *pDataVDI_req;

		pDataVDI_req = (BEGP_LOGICAL_POINT_PARAMETER_VDI_T*)pData_req;
		pParameterVDI = (BEGP_LOGICAL_POINT_PARAMETER_VDI_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

		pDataVDI_req->ID = BES200_htons(LogicalPointID);  //逻辑点的ID
		pDataVDI_req->PointType = pParameterVDI->PointType;  //点的类型
		pDataVDI_req->Active = pParameterVDI->Active;//是否启用
		memcpy(pDataVDI_req->Name, pParameterVDI->Name, sizeof(pDataVDI_req->Name));  //名字
		memcpy(pDataVDI_req->Alias, pParameterVDI->Alias, sizeof(pDataVDI_req->Alias)); //别名
		memcpy(pDataVDI_req->Description, pParameterVDI->Description, sizeof(pDataVDI_req->Description)); //描述
		pDataVDI_req->InitValue = BES200_htonl(pParameterVDI->InitValue);  //初始值

		/*报警相关*/
		pDataVDI_req->AlarmActive = pParameterVDI->AlarmActive;//报警是否启用
		pDataVDI_req->AlarmType = pParameterVDI->AlarmType; //报警类型
		pDataVDI_req->AlarmPriority = pParameterVDI->AlarmPriority;//报警优先级

		pDataVDI_req->AlarmTrigger = pParameterVDI->AlarmTrigger;//报警触发
	}
	else if (pParameter->PointType == POINT_TYPE_VIRTUAL_DO) //虚点DO
	{
		BEGP_LOGICAL_POINT_PARAMETER_VDO_T        *pParameterVDO;
		BEGP_LOGICAL_POINT_PARAMETER_VDO_T        *pDataVDO_req;

		pDataVDO_req = (BEGP_LOGICAL_POINT_PARAMETER_VDO_T*)pData_req;
		pParameterVDO = (BEGP_LOGICAL_POINT_PARAMETER_VDO_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

		pDataVDO_req->ID = BES200_htons(LogicalPointID);  //逻辑点的ID
		pDataVDO_req->PointType = pParameterVDO->PointType;  //点的类型
		pDataVDO_req->Active = pParameterVDO->Active;//是否启用
		memcpy(pDataVDO_req->Name, pParameterVDO->Name, sizeof(pDataVDO_req->Name));  //名字
		memcpy(pDataVDO_req->Alias, pParameterVDO->Alias, sizeof(pDataVDO_req->Alias)); //别名
		memcpy(pDataVDO_req->Description, pParameterVDO->Description, sizeof(pDataVDO_req->Description)); //描述
		pDataVDO_req->InitValue = BES200_htonl(pParameterVDO->InitValue);  //初始值

		/*报警相关*/
		pDataVDO_req->AlarmActive = pParameterVDO->AlarmActive;//报警是否启用
		pDataVDO_req->AlarmType = pParameterVDO->AlarmType; //报警类型
		pDataVDO_req->AlarmPriority = pParameterVDO->AlarmPriority;//报警优先级

		pDataVDO_req->AlarmTrigger = pParameterVDO->AlarmTrigger;//报警触发
	}

	SendDataSize += BEGP_REQ_PACKETDATA_ADD_LOGICAL_POINT_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_ADD_LOGICAL_POINT) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}

/*设置一个逻辑点的所有参数*/
int CCrossFunManager:: BES200_SetLogicalPointParameter(const char *IPAddr, uint16_t LogicalPointID, const struct LogicalPointParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                                    *pHead_req;
	BEGP_RSP_HEAD_T                                    *pHead_rsp;
	BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_T  *pData_req;
	BEGP_RSP_PACKETDATA_ECHO_T                         *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_req = (BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_T *)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_LOGICAL_POINT_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	if (pParameter->PointType == POINT_TYPE_LAI) //实点AI
	{
		BEGP_LOGICAL_POINT_PARAMETER_LAI_T        *pParameterLAI;
		BEGP_LOGICAL_POINT_PARAMETER_LAI_T        *pDataLAI_req;

		pDataLAI_req = (BEGP_LOGICAL_POINT_PARAMETER_LAI_T*)pData_req;
		pParameterLAI = (BEGP_LOGICAL_POINT_PARAMETER_LAI_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

		pDataLAI_req->PointType = pParameterLAI->PointType;  //点的类型
		pDataLAI_req->ID = BES200_htons(LogicalPointID);  //逻辑点的ID
		pDataLAI_req->Active = pParameterLAI->Active;//是否启用
		memcpy(pDataLAI_req->Name, pParameterLAI->Name, sizeof(pDataLAI_req->Name));  //名字
		memcpy(pDataLAI_req->Alias, pParameterLAI->Alias, sizeof(pDataLAI_req->Alias)); //别名
		memcpy(pDataLAI_req->Description, pParameterLAI->Description, sizeof(pDataLAI_req->Description)); //描述
		pDataLAI_req->ModuleID = BES200_htons(pParameterLAI->ModuleID);  //点所在模块ID
		pDataLAI_req->ChannelIndex = pParameterLAI->ChannelIndex;//点所在的通道索引
		pDataLAI_req->WorkMode = pParameterLAI->WorkMode; //工作模式
		pDataLAI_req->Polarity = pParameterLAI->Polarity; //是否反向
		pDataLAI_req->InitValue = BES200_htonl(pParameterLAI->InitValue);//值

		pDataLAI_req->AlarmActive = pParameterLAI->AlarmActive;//报警是否启用
		pDataLAI_req->AlarmType = pParameterLAI->AlarmType; //报警类型
		pDataLAI_req->AlarmPriority = pParameterLAI->AlarmPriority;//报警优先级

		pDataLAI_req->LineType = pParameterLAI->LineType;//模拟量输入类型
		memcpy(pDataLAI_req->Unit, pParameterLAI->Unit, sizeof(pDataLAI_req->Unit)); //工程单位
		pDataLAI_req->HighRange = BES200_htonl(pParameterLAI->HighRange);//32位数据大小端转换
		pDataLAI_req->LowRange = BES200_htonl(pParameterLAI->LowRange);//32位数据大小端转换
		pDataLAI_req->Precision = pParameterLAI->Precision;//精度
		pDataLAI_req->AlarmHighValue = BES200_htonl(pParameterLAI->AlarmHighValue);//32位数据大小端转换
		pDataLAI_req->AlarmLowValue = BES200_htonl(pParameterLAI->AlarmLowValue);//32位数据大小端转换
	}
	//构造请求正文数据
	else if (pParameter->PointType == POINT_TYPE_LAO) //实点AO
	{
		BEGP_LOGICAL_POINT_PARAMETER_LAO_T        *pParameterLAO;
		BEGP_LOGICAL_POINT_PARAMETER_LAO_T        *pDataLAO_req;

		pDataLAO_req = (BEGP_LOGICAL_POINT_PARAMETER_LAO_T*)pData_req;
		pParameterLAO = (BEGP_LOGICAL_POINT_PARAMETER_LAO_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

		pDataLAO_req->PointType = pParameterLAO->PointType;  //点的类型
		pDataLAO_req->ID = BES200_htons(LogicalPointID);  //逻辑点的ID
		pDataLAO_req->Active = pParameterLAO->Active;//是否启用
		memcpy(pDataLAO_req->Name, pParameterLAO->Name, sizeof(pDataLAO_req->Name));  //名字
		memcpy(pDataLAO_req->Alias, pParameterLAO->Alias, sizeof(pDataLAO_req->Alias)); //别名
		memcpy(pDataLAO_req->Description, pParameterLAO->Description, sizeof(pDataLAO_req->Description)); //描述
		pDataLAO_req->ModuleID = BES200_htons(pParameterLAO->ModuleID);  //点所在模块ID
		pDataLAO_req->ChannelIndex = pParameterLAO->ChannelIndex;//点所在的通道索引
		pDataLAO_req->WorkMode = pParameterLAO->WorkMode; //工作模式
		pDataLAO_req->Polarity = pParameterLAO->Polarity; //是否反向
		pDataLAO_req->InitValue = BES200_htonl(pParameterLAO->InitValue); //初始值

		pDataLAO_req->AlarmActive = pParameterLAO->AlarmActive;//报警是否启用
		pDataLAO_req->AlarmType = pParameterLAO->AlarmType; //报警类型
		pDataLAO_req->AlarmPriority = pParameterLAO->AlarmPriority;//报警优先级

		pDataLAO_req->LineType = pParameterLAO->LineType;//模拟量输出类型
		memcpy(pDataLAO_req->Unit, pParameterLAO->Unit, sizeof(pDataLAO_req->Unit)); //工程单位
		pDataLAO_req->HighRange = BES200_htonl(pParameterLAO->HighRange);//32位数据大小端转换
		pDataLAO_req->LowRange = BES200_htonl(pParameterLAO->LowRange);//32位数据大小端转换
		pDataLAO_req->Precision = pParameterLAO->Precision;//精度
		pDataLAO_req->AlarmHighValue = BES200_htonl(pParameterLAO->AlarmHighValue);//32位数据大小端转换
		pDataLAO_req->AlarmLowValue = BES200_htonl(pParameterLAO->AlarmLowValue);//32位数据大小端转换
	}
	else if (pParameter->PointType == POINT_TYPE_LDI) //实点DI
	{
		BEGP_LOGICAL_POINT_PARAMETER_LDI_T        *pParameterLDI;
		BEGP_LOGICAL_POINT_PARAMETER_LDI_T        *pDataLDI_req;

		pDataLDI_req = (BEGP_LOGICAL_POINT_PARAMETER_LDI_T*)pData_req;
		pParameterLDI = (BEGP_LOGICAL_POINT_PARAMETER_LDI_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

		pDataLDI_req->PointType = pParameterLDI->PointType;  //点的类型
		pDataLDI_req->ID = BES200_htons(LogicalPointID);  //逻辑点的ID
		pDataLDI_req->Active = pParameterLDI->Active;//是否启用
		memcpy(pDataLDI_req->Name, pParameterLDI->Name, sizeof(pDataLDI_req->Name));  //名字
		memcpy(pDataLDI_req->Alias, pParameterLDI->Alias, sizeof(pDataLDI_req->Alias)); //别名
		memcpy(pDataLDI_req->Description, pParameterLDI->Description, sizeof(pDataLDI_req->Description)); //描述
		pDataLDI_req->ModuleID = BES200_htons(pParameterLDI->ModuleID);  //点所在模块ID
		pDataLDI_req->ChannelIndex = pParameterLDI->ChannelIndex;//点所在的通道索引
		pDataLDI_req->WorkMode = pParameterLDI->WorkMode; //工作模式
		pDataLDI_req->Polarity = pParameterLDI->Polarity; //是否反向
		pDataLDI_req->InitValue = BES200_htonl(pParameterLDI->InitValue);//值

		pDataLDI_req->AlarmActive = pParameterLDI->AlarmActive;//报警是否启用
		pDataLDI_req->AlarmType = pParameterLDI->AlarmType; //报警类型
		pDataLDI_req->AlarmPriority = pParameterLDI->AlarmPriority;//报警优先级

		pDataLDI_req->AlarmTrigger = pParameterLDI->AlarmTrigger;//报警触发
		pDataLDI_req->ActivePassive = pParameterLDI->ActivePassive;//有源无源
	}
	else if (pParameter->PointType == POINT_TYPE_LDO) //实点DO
	{
		BEGP_LOGICAL_POINT_PARAMETER_LDO_T        *pParameterLDO;
		BEGP_LOGICAL_POINT_PARAMETER_LDO_T        *pDataLDO_req;

		pDataLDO_req = (BEGP_LOGICAL_POINT_PARAMETER_LDO_T*)pData_req;
		pParameterLDO = (BEGP_LOGICAL_POINT_PARAMETER_LDO_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

		pDataLDO_req->PointType = pParameterLDO->PointType;  //点的类型
		pDataLDO_req->ID = BES200_htons(LogicalPointID);  //逻辑点的ID
		pDataLDO_req->Active = pParameterLDO->Active;//是否启用
		memcpy(pDataLDO_req->Name, pParameterLDO->Name, sizeof(pDataLDO_req->Name));  //名字
		memcpy(pDataLDO_req->Alias, pParameterLDO->Alias, sizeof(pDataLDO_req->Alias)); //别名
		memcpy(pDataLDO_req->Description, pParameterLDO->Description, sizeof(pDataLDO_req->Description)); //描述
		pDataLDO_req->ModuleID = BES200_htons(pParameterLDO->ModuleID);  //点所在模块ID
		pDataLDO_req->ChannelIndex = pParameterLDO->ChannelIndex;//点所在的通道索引
		pDataLDO_req->WorkMode = pParameterLDO->WorkMode; //工作模式
		pDataLDO_req->Polarity = pParameterLDO->Polarity; //是否反向
		pDataLDO_req->InitValue = BES200_htonl(pParameterLDO->InitValue); //初始值

		pDataLDO_req->AlarmActive = pParameterLDO->AlarmActive;//报警是否启用
		pDataLDO_req->AlarmType = pParameterLDO->AlarmType; //报警类型
		pDataLDO_req->AlarmPriority = pParameterLDO->AlarmPriority;//报警优先级

		pDataLDO_req->AlarmTrigger = pParameterLDO->AlarmTrigger;//报警触发
	}
	else if (pParameter->PointType == POINT_TYPE_VIRTUAL_AI) //虚点AI
	{
		BEGP_LOGICAL_POINT_PARAMETER_VAI_T        *pParameterVAI;
		BEGP_LOGICAL_POINT_PARAMETER_VAI_T        *pDataVAI_req;

		pDataVAI_req = (BEGP_LOGICAL_POINT_PARAMETER_VAI_T*)pData_req;
		pParameterVAI = (BEGP_LOGICAL_POINT_PARAMETER_VAI_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

		pDataVAI_req->PointType = pParameterVAI->PointType;  //点的类型
		pDataVAI_req->ID = BES200_htons(LogicalPointID);  //逻辑点的ID
		pDataVAI_req->Active = pParameterVAI->Active;//是否启用
		memcpy(pDataVAI_req->Name, pParameterVAI->Name, sizeof(pDataVAI_req->Name));  //名字
		memcpy(pDataVAI_req->Alias, pParameterVAI->Alias, sizeof(pDataVAI_req->Alias)); //别名
		memcpy(pDataVAI_req->Description, pParameterVAI->Description, sizeof(pDataVAI_req->Description)); //描述
		pDataVAI_req->InitValue = BES200_htonl(pParameterVAI->InitValue);  //初始值
		memcpy(pDataVAI_req->Unit, pParameterVAI->Unit, sizeof(pDataVAI_req->Unit)); //单位
		pDataVAI_req->Precision = pParameterVAI->Precision;  //精度

		/*报警相关*/
		pDataVAI_req->AlarmActive = pParameterVAI->AlarmActive;//报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		pDataVAI_req->AlarmType = pParameterVAI->AlarmType;//报警类型，不报警、标准报警、加强报警
		pDataVAI_req->AlarmPriority = pParameterVAI->AlarmPriority;//报警优先级
		pDataVAI_req->AlarmHighValue = pParameterVAI->AlarmHighValue;
		pDataVAI_req->AlarmLowValue = pParameterVAI->AlarmLowValue;
	}
	else if (pParameter->PointType == POINT_TYPE_VIRTUAL_AO) //虚点AO
	{
		BEGP_LOGICAL_POINT_PARAMETER_VAO_T        *pParameterVAO;
		BEGP_LOGICAL_POINT_PARAMETER_VAO_T        *pDataVAO_req;

		pDataVAO_req = (BEGP_LOGICAL_POINT_PARAMETER_VAO_T*)pData_req;
		pParameterVAO = (BEGP_LOGICAL_POINT_PARAMETER_VAO_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

		pDataVAO_req->PointType = pParameterVAO->PointType;  //点的类型
		pDataVAO_req->ID = BES200_htons(LogicalPointID);  //逻辑点的ID
		pDataVAO_req->Active = pParameterVAO->Active;//是否启用
		memcpy(pDataVAO_req->Name, pParameterVAO->Name, sizeof(pDataVAO_req->Name));  //名字
		memcpy(pDataVAO_req->Alias, pParameterVAO->Alias, sizeof(pDataVAO_req->Alias)); //别名
		memcpy(pDataVAO_req->Description, pParameterVAO->Description, sizeof(pDataVAO_req->Description)); //描述
		pDataVAO_req->InitValue = BES200_htonl(pParameterVAO->InitValue);  //初始值
		memcpy(pDataVAO_req->Unit, pParameterVAO->Unit, sizeof(pDataVAO_req->Unit)); //单位
		pDataVAO_req->Precision = pParameterVAO->Precision;  //精度

		/*报警相关*/
		pDataVAO_req->AlarmActive = pParameterVAO->AlarmActive;//报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
		pDataVAO_req->AlarmType = pParameterVAO->AlarmType;//报警类型，不报警、标准报警、加强报警
		pDataVAO_req->AlarmPriority = pParameterVAO->AlarmPriority;//报警优先级
		pDataVAO_req->AlarmHighValue = pParameterVAO->AlarmHighValue;
		pDataVAO_req->AlarmLowValue = pParameterVAO->AlarmLowValue;
	}
	else if (pParameter->PointType == POINT_TYPE_VIRTUAL_DI) //虚点DI
	{
		BEGP_LOGICAL_POINT_PARAMETER_VDI_T        *pParameterVDI;
		BEGP_LOGICAL_POINT_PARAMETER_VDI_T        *pDataVDI_req;

		pDataVDI_req = (BEGP_LOGICAL_POINT_PARAMETER_VDI_T*)pData_req;
		pParameterVDI = (BEGP_LOGICAL_POINT_PARAMETER_VDI_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

		pDataVDI_req->PointType = pParameterVDI->PointType;  //点的类型
		pDataVDI_req->ID = BES200_htons(LogicalPointID);  //逻辑点的ID
		pDataVDI_req->Active = pParameterVDI->Active;//是否启用
		memcpy(pDataVDI_req->Name, pParameterVDI->Name, sizeof(pDataVDI_req->Name));  //名字
		memcpy(pDataVDI_req->Alias, pParameterVDI->Alias, sizeof(pDataVDI_req->Alias)); //别名
		memcpy(pDataVDI_req->Description, pParameterVDI->Description, sizeof(pDataVDI_req->Description)); //描述
		pDataVDI_req->InitValue = BES200_htonl(pParameterVDI->InitValue);  //初始值

		/*报警相关*/
		pDataVDI_req->AlarmActive = pParameterVDI->AlarmActive;//报警是否启用
		pDataVDI_req->AlarmType = pParameterVDI->AlarmType; //报警类型
		pDataVDI_req->AlarmPriority = pParameterVDI->AlarmPriority;//报警优先级

		pDataVDI_req->AlarmTrigger = pParameterVDI->AlarmTrigger;//报警触发
	}
	else if (pParameter->PointType == POINT_TYPE_VIRTUAL_DO) //虚点DO
	{
		BEGP_LOGICAL_POINT_PARAMETER_VDO_T        *pParameterVDO;
		BEGP_LOGICAL_POINT_PARAMETER_VDO_T        *pDataVDO_req;

		pDataVDO_req = (BEGP_LOGICAL_POINT_PARAMETER_VDO_T*)pData_req;
		pParameterVDO = (BEGP_LOGICAL_POINT_PARAMETER_VDO_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

		pDataVDO_req->PointType = pParameterVDO->PointType;  //点的类型
		pDataVDO_req->ID = BES200_htons(LogicalPointID);  //逻辑点的ID
		pDataVDO_req->Active = pParameterVDO->Active;//是否启用
		memcpy(pDataVDO_req->Name, pParameterVDO->Name, sizeof(pDataVDO_req->Name));  //名字
		memcpy(pDataVDO_req->Alias, pParameterVDO->Alias, sizeof(pDataVDO_req->Alias)); //别名
		memcpy(pDataVDO_req->Description, pParameterVDO->Description, sizeof(pDataVDO_req->Description)); //描述
		pDataVDO_req->InitValue = BES200_htonl(pParameterVDO->InitValue);  //初始值

		/*报警相关*/
		pDataVDO_req->AlarmActive = pParameterVDO->AlarmActive;//报警是否启用
		pDataVDO_req->AlarmType = pParameterVDO->AlarmType; //报警类型
		pDataVDO_req->AlarmPriority = pParameterVDO->AlarmPriority;//报警优先级

		pDataVDO_req->AlarmTrigger = pParameterVDO->AlarmTrigger;//报警触发
	}

	SendDataSize += BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_SET_LOGICAL_POINT_PARAMETER) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}

/*设置一个逻辑点的值*/
int CCrossFunManager:: BES200_SetLogicalPointValue(const char *IPAddr, uint16_t LogicalPointID, const struct LogicalPointvalue *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                                    *pHead_req;
	BEGP_RSP_HEAD_T                                    *pHead_rsp;
	BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_T      *pData_req;
	BEGP_RSP_PACKETDATA_ECHO_T                         *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_req = (BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_T *)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_LOGICAL_POINT_VALUE;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_req->ID = BES200_htons(LogicalPointID);
	pData_req->WorkMode = pParameter->WorkMode;
	pData_req->Value = BES200_htonl(pParameter->Value);

	SendDataSize += BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_SET_LOGICAL_POINT_VALUE) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}


/*设置一个逻辑点的值，通过名字*/
int CCrossFunManager:: BES200_SetLogicalPointValueByName(const char *IPAddr, const char * LogicalPointName, float Value, char **errmsg)
{
	BEGP_REQ_HEAD_T												*pHead_req;
	BEGP_RSP_HEAD_T												*pHead_rsp;
	BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_BY_NAME_T		*pData_req;
	BEGP_RSP_PACKETDATA_ECHO_T									*pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_req = (BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_BY_NAME_T *)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_LOGICAL_POINT_VALUE_BY_NAME;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_BY_NAME_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_req->Value = BES200_htonl(*((uint32_t*)&Value));
	strcpy_s(pData_req->Name, LogicalPointName);
	SendDataSize += BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_BY_NAME_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_SET_LOGICAL_POINT_VALUE_BY_NAME) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}


/*删除一个逻辑点*/
int CCrossFunManager:: BES200_DeleteLogicalPoint(const char *IPAddr, uint16_t LogicalPointID, char **errmsg)
{
	BEGP_REQ_HEAD_T                               *pHead_req;
	BEGP_RSP_HEAD_T                               *pHead_rsp;
	BEGP_REQ_PACKETDATA_DELETE_LOGICAL_POINT_T    *pData_rep;
	BEGP_RSP_PACKETDATA_ECHO_T                    *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (BEGP_REQ_PACKETDATA_DELETE_LOGICAL_POINT_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_DELETE_LOGICAL_POINT;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_DELETE_LOGICAL_POINT_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_rep->ID = BES200_htons(LogicalPointID);    //要删除点的ID号

	SendDataSize += BEGP_REQ_PACKETDATA_DELETE_LOGICAL_POINT_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_DELETE_LOGICAL_POINT) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}
/*获取一个逻辑点的所有配置参数*/
int CCrossFunManager:: BES200_GetLogicalPointParameter(const char *IPAddr, uint16_t LogicalPointID, struct LogicalPointParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                                      *pHead_req;
	BEGP_RSP_HEAD_T                                      *pHead_rsp;
	BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_T    *pData_rep;
	BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_T    *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_LOGICAL_POINT_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_rep->ID = BES200_htons(LogicalPointID);    //要获取点的ID号

	SendDataSize += BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_GET_LOGICAL_POINT_PARAMETER) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_SIZE))
		{
			ret = BES200_ERROR_OK;
			sprintf_s(*errmsg,2048, "err:%d", ret);
			if (pData_rsp->PointType == POINT_TYPE_LAI) //实点AI
			{
				BEGP_LOGICAL_POINT_PARAMETER_LAI_T        *pParameterLAI;
				BEGP_LOGICAL_POINT_PARAMETER_LAI_T        *ppData_rsp;

				ppData_rsp = (BEGP_LOGICAL_POINT_PARAMETER_LAI_T*)pData_rsp;
				pParameterLAI = (BEGP_LOGICAL_POINT_PARAMETER_LAI_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

				pParameterLAI->PointType = ppData_rsp->PointType;  //点的类型
				pParameterLAI->ID = BES200_htons(ppData_rsp->ID);  //逻辑点的ID
				pParameterLAI->Active = ppData_rsp->Active;//是否启用
				memcpy(pParameterLAI->Name, ppData_rsp->Name, sizeof(pParameterLAI->Name));//名字
				memcpy(pParameterLAI->Alias, ppData_rsp->Alias, sizeof(pParameterLAI->Alias)); //别名
				memcpy(pParameterLAI->Description, ppData_rsp->Description, sizeof(pParameterLAI->Description)); //描述
				pParameterLAI->ModuleID = BES200_htons(ppData_rsp->ModuleID);  //点所在模块ID
				pParameterLAI->ChannelIndex = ppData_rsp->ChannelIndex;//点所在的通道索引
				pParameterLAI->WorkMode = ppData_rsp->WorkMode; //工作模式
				pParameterLAI->Polarity = ppData_rsp->Polarity; //是否反向
				pParameterLAI->InitValue = BES200_htonl(ppData_rsp->InitValue); //值

				pParameterLAI->AlarmActive = ppData_rsp->AlarmActive;//报警是否启用
				pParameterLAI->AlarmType = ppData_rsp->AlarmType; //报警类型
				pParameterLAI->AlarmPriority = ppData_rsp->AlarmPriority;//报警优先级

				pParameterLAI->LineType = ppData_rsp->LineType;//模拟量输入类型
				memcpy(pParameterLAI->Unit, ppData_rsp->Unit, sizeof(pParameterLAI->Unit)); //工程单位
				pParameterLAI->HighRange = BES200_htonl(ppData_rsp->HighRange);//32位数据大小端转换
				pParameterLAI->LowRange = BES200_htonl(ppData_rsp->LowRange);//32位数据大小端转换
				pParameterLAI->Precision = ppData_rsp->Precision;//精度
				pParameterLAI->AlarmHighValue = BES200_htonl(ppData_rsp->AlarmHighValue);//32位数据大小端转换
				pParameterLAI->AlarmLowValue = BES200_htonl(ppData_rsp->AlarmLowValue);//32位数据大小端转换
			}
			else if (pData_rsp->PointType == POINT_TYPE_LAO) //实点AO
			{
				BEGP_LOGICAL_POINT_PARAMETER_LAO_T        *pParameterLAO;
				BEGP_LOGICAL_POINT_PARAMETER_LAO_T        *pDataLAO_rsp;

				pDataLAO_rsp = (BEGP_LOGICAL_POINT_PARAMETER_LAO_T*)pData_rsp;
				pParameterLAO = (BEGP_LOGICAL_POINT_PARAMETER_LAO_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

				pParameterLAO->PointType = pDataLAO_rsp->PointType;  //点的类型
				pParameterLAO->ID = BES200_htons(pDataLAO_rsp->ID);  //逻辑点的ID
				pParameterLAO->Active = pDataLAO_rsp->Active;//是否启用
				memcpy(pParameterLAO->Name, pDataLAO_rsp->Name, sizeof(pParameterLAO->Name));//名字
				memcpy(pParameterLAO->Alias, pDataLAO_rsp->Alias, sizeof(pParameterLAO->Alias)); //别名
				memcpy(pParameterLAO->Description, pDataLAO_rsp->Description, sizeof(pParameterLAO->Description)); //描述
				pParameterLAO->ModuleID = BES200_htons(pDataLAO_rsp->ModuleID);  //点所在模块ID
				pParameterLAO->ChannelIndex = pDataLAO_rsp->ChannelIndex;//点所在的通道索引
				pParameterLAO->WorkMode = pDataLAO_rsp->WorkMode; //工作模式
				pParameterLAO->Polarity = pDataLAO_rsp->Polarity; //是否反向
				pParameterLAO->InitValue = BES200_htonl(pDataLAO_rsp->InitValue); //初始值

				pParameterLAO->AlarmActive = pDataLAO_rsp->AlarmActive;//报警是否启用
				pParameterLAO->AlarmType = pDataLAO_rsp->AlarmType; //报警类型
				pParameterLAO->AlarmPriority = pDataLAO_rsp->AlarmPriority;//报警优先级

				pParameterLAO->LineType = pDataLAO_rsp->LineType;//模拟量输入类型
				memcpy(pParameterLAO->Unit, pDataLAO_rsp->Unit, sizeof(pParameterLAO->Unit)); //工程单位
				pParameterLAO->HighRange = BES200_htonl(pDataLAO_rsp->HighRange);//32位数据大小端转换
				pParameterLAO->LowRange = BES200_htonl(pDataLAO_rsp->LowRange);//32位数据大小端转换
				pParameterLAO->Precision = pDataLAO_rsp->Precision;//精度
				pParameterLAO->AlarmHighValue = BES200_htonl(pDataLAO_rsp->AlarmHighValue);//32位数据大小端转换
				pParameterLAO->AlarmLowValue = BES200_htonl(pDataLAO_rsp->AlarmLowValue);//32位数据大小端转换
			}
			else if (pData_rsp->PointType == POINT_TYPE_LDI) //实点DI
			{
				BEGP_LOGICAL_POINT_PARAMETER_LDI_T        *pParameterLDI;
				BEGP_LOGICAL_POINT_PARAMETER_LDI_T        *pDataLDI_rsp;

				pDataLDI_rsp = (BEGP_LOGICAL_POINT_PARAMETER_LDI_T*)pData_rsp;
				pParameterLDI = (BEGP_LOGICAL_POINT_PARAMETER_LDI_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

				pParameterLDI->PointType = pDataLDI_rsp->PointType;  //点的类型
				pParameterLDI->ID = BES200_htons(pDataLDI_rsp->ID);  //逻辑点的ID
				pParameterLDI->Active = pDataLDI_rsp->Active;//是否启用
				memcpy(pParameterLDI->Name, pDataLDI_rsp->Name, sizeof(pParameterLDI->Name));  //名字
				memcpy(pParameterLDI->Alias, pDataLDI_rsp->Alias, sizeof(pParameterLDI->Alias)); //别名
				memcpy(pParameterLDI->Description, pDataLDI_rsp->Description, sizeof(pParameterLDI->Description)); //描述
				pParameterLDI->ModuleID = BES200_htons(pDataLDI_rsp->ModuleID);  //点所在模块ID
				pParameterLDI->ChannelIndex = pDataLDI_rsp->ChannelIndex;//点所在的通道索引
				pParameterLDI->WorkMode = pDataLDI_rsp->WorkMode; //工作模式
				pParameterLDI->Polarity = pDataLDI_rsp->Polarity; //是否反向
				pParameterLDI->InitValue = BES200_htonl(pDataLDI_rsp->InitValue); //值

				pParameterLDI->AlarmActive = pDataLDI_rsp->AlarmActive;//报警是否启用
				pParameterLDI->AlarmType = pDataLDI_rsp->AlarmType; //报警类型
				pParameterLDI->AlarmPriority = pDataLDI_rsp->AlarmPriority;//报警优先级

				pParameterLDI->AlarmTrigger = pDataLDI_rsp->AlarmTrigger;//报警触发
				pParameterLDI->ActivePassive = pDataLDI_rsp->ActivePassive;//有源无源
			}
			else if (pData_rsp->PointType == POINT_TYPE_LDO) //实点DO
			{
				BEGP_LOGICAL_POINT_PARAMETER_LDO_T        *pParameterLDO;
				BEGP_LOGICAL_POINT_PARAMETER_LDO_T        *pDataLDO_rsp;

				pDataLDO_rsp = (BEGP_LOGICAL_POINT_PARAMETER_LDO_T*)pData_rsp;
				pParameterLDO = (BEGP_LOGICAL_POINT_PARAMETER_LDO_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

				pParameterLDO->PointType = pDataLDO_rsp->PointType;  //点的类型
				pParameterLDO->ID = BES200_htons(pDataLDO_rsp->ID);  //逻辑点的ID
				pParameterLDO->Active = pDataLDO_rsp->Active;//是否启用
				memcpy(pParameterLDO->Name, pDataLDO_rsp->Name, sizeof(pParameterLDO->Name));  //名字
				memcpy(pParameterLDO->Alias, pDataLDO_rsp->Alias, sizeof(pParameterLDO->Alias)); //别名
				memcpy(pParameterLDO->Description, pDataLDO_rsp->Description, sizeof(pParameterLDO->Description)); //描述
				pParameterLDO->ModuleID = BES200_htons(pDataLDO_rsp->ModuleID);  //点所在模块ID
				pParameterLDO->ChannelIndex = pDataLDO_rsp->ChannelIndex;//点所在的通道索引
				pParameterLDO->WorkMode = pDataLDO_rsp->WorkMode; //工作模式
				pParameterLDO->Polarity = pDataLDO_rsp->Polarity; //是否反向
				pParameterLDO->InitValue = BES200_htonl(pDataLDO_rsp->InitValue); //是否反向

				pParameterLDO->AlarmActive = pDataLDO_rsp->AlarmActive;//报警是否启用
				pParameterLDO->AlarmType = pDataLDO_rsp->AlarmType; //报警类型
				pParameterLDO->AlarmPriority = pDataLDO_rsp->AlarmPriority;//报警优先级

				pParameterLDO->AlarmTrigger = pDataLDO_rsp->AlarmTrigger;//报警触发
			}
			else if (pData_rsp->PointType == POINT_TYPE_VIRTUAL_AI) //虚点AI
			{
				BEGP_LOGICAL_POINT_PARAMETER_VAI_T        *pParameterVAI;
				BEGP_LOGICAL_POINT_PARAMETER_VAI_T        *pDataVAI_req;

				pDataVAI_req = (BEGP_LOGICAL_POINT_PARAMETER_VAI_T*)pData_rsp;
				pParameterVAI = (BEGP_LOGICAL_POINT_PARAMETER_VAI_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

				pParameterVAI->PointType = pDataVAI_req->PointType;  //点的类型
				pParameterVAI->ID = BES200_htons(pDataVAI_req->ID);  //逻辑点的ID
				pParameterVAI->Active = pDataVAI_req->Active;//是否启用
				memcpy(pParameterVAI->Name, pDataVAI_req->Name, sizeof(pParameterVAI->Name));  //名字
				memcpy(pParameterVAI->Alias, pDataVAI_req->Alias, sizeof(pParameterVAI->Alias)); //别名
				memcpy(pParameterVAI->Description, pDataVAI_req->Description, sizeof(pParameterVAI->Description)); //描述
				pParameterVAI->InitValue = BES200_htonl(pDataVAI_req->InitValue);  //初始值
				memcpy(pParameterVAI->Unit, pDataVAI_req->Unit, sizeof(pParameterVAI->Unit)); //单位
				pParameterVAI->Precision = pDataVAI_req->Precision;  //精度

				/*报警相关*/
				pParameterVAI->AlarmActive = pDataVAI_req->AlarmActive;//报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
				pParameterVAI->AlarmType = pDataVAI_req->AlarmType;//报警类型，不报警、标准报警、加强报警
				pParameterVAI->AlarmPriority = pDataVAI_req->AlarmPriority;//报警优先级
				pParameterVAI->AlarmHighValue = pDataVAI_req->AlarmHighValue;
				pParameterVAI->AlarmLowValue = pDataVAI_req->AlarmLowValue;
			}
			else if (pData_rsp->PointType == POINT_TYPE_VIRTUAL_AO) //虚点AO
			{
				BEGP_LOGICAL_POINT_PARAMETER_VAO_T        *pParameterVAO;
				BEGP_LOGICAL_POINT_PARAMETER_VAO_T        *pDataVAO_req;

				pDataVAO_req = (BEGP_LOGICAL_POINT_PARAMETER_VAO_T*)pData_rsp;
				pParameterVAO = (BEGP_LOGICAL_POINT_PARAMETER_VAO_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

				pParameterVAO->PointType = pDataVAO_req->PointType;  //点的类型
				pParameterVAO->ID = BES200_htons(pDataVAO_req->ID);  //逻辑点的ID
				pParameterVAO->Active = pDataVAO_req->Active;//是否启用
				memcpy(pParameterVAO->Name, pDataVAO_req->Name, sizeof(pParameterVAO->Name));  //名字
				memcpy(pParameterVAO->Alias, pDataVAO_req->Alias, sizeof(pParameterVAO->Alias)); //别名
				memcpy(pParameterVAO->Description, pDataVAO_req->Description, sizeof(pParameterVAO->Description)); //描述
				pParameterVAO->InitValue = BES200_htonl(pDataVAO_req->InitValue);  //初始值
				memcpy(pParameterVAO->Unit, pDataVAO_req->Unit, sizeof(pParameterVAO->Unit)); //单位
				pParameterVAO->Precision = pDataVAO_req->Precision;  //精度

				/*报警相关*/
				pParameterVAO->AlarmActive = pDataVAO_req->AlarmActive;//报警是否启用，ALARM_ACTIVE_ENABLE使能，ALARM_ACTIVE_DISABLE禁止
				pParameterVAO->AlarmType = pDataVAO_req->AlarmType;//报警类型，不报警、标准报警、加强报警
				pParameterVAO->AlarmPriority = pDataVAO_req->AlarmPriority;//报警优先级
				pParameterVAO->AlarmHighValue = pDataVAO_req->AlarmHighValue;
				pParameterVAO->AlarmLowValue = pDataVAO_req->AlarmLowValue;
			}
			else if (pData_rsp->PointType == POINT_TYPE_VIRTUAL_DI) //虚点DI
			{
				BEGP_LOGICAL_POINT_PARAMETER_VDI_T        *pParameterVDI;
				BEGP_LOGICAL_POINT_PARAMETER_VDI_T        *pDataVDI_req;

				pDataVDI_req = (BEGP_LOGICAL_POINT_PARAMETER_VDI_T*)pData_rsp;
				pParameterVDI = (BEGP_LOGICAL_POINT_PARAMETER_VDI_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

				pParameterVDI->PointType = pDataVDI_req->PointType;  //点的类型
				pParameterVDI->ID = BES200_htons(pDataVDI_req->ID);  //逻辑点的ID
				pParameterVDI->Active = pDataVDI_req->Active;//是否启用
				memcpy(pParameterVDI->Name, pDataVDI_req->Name, sizeof(pParameterVDI->Name));  //名字
				memcpy(pParameterVDI->Alias, pDataVDI_req->Alias, sizeof(pParameterVDI->Alias)); //别名
				memcpy(pParameterVDI->Description, pDataVDI_req->Description, sizeof(pParameterVDI->Description)); //描述
				pParameterVDI->InitValue = BES200_htonl(pDataVDI_req->InitValue);  //初始值

				/*报警相关*/
				pParameterVDI->AlarmActive = pDataVDI_req->AlarmActive;//报警是否启用
				pParameterVDI->AlarmType = pDataVDI_req->AlarmType; //报警类型
				pParameterVDI->AlarmPriority = pDataVDI_req->AlarmPriority;//报警优先级

				pParameterVDI->AlarmTrigger = pDataVDI_req->AlarmTrigger;//报警触发
			}
			else if (pData_rsp->PointType == POINT_TYPE_VIRTUAL_DO) //虚点DO
			{
				BEGP_LOGICAL_POINT_PARAMETER_VDO_T        *pParameterVDO;
				BEGP_LOGICAL_POINT_PARAMETER_VDO_T        *pDataVDO_req;

				pDataVDO_req = (BEGP_LOGICAL_POINT_PARAMETER_VDO_T*)pData_rsp;
				pParameterVDO = (BEGP_LOGICAL_POINT_PARAMETER_VDO_T *)pParameter;//字节顺必须要一致，在BES200.h中要1字节对齐

				pParameterVDO->PointType = pDataVDO_req->PointType;  //点的类型
				pParameterVDO->ID = BES200_htons(pDataVDO_req->ID);  //逻辑点的ID
				pParameterVDO->Active = pDataVDO_req->Active;//是否启用
				memcpy(pParameterVDO->Name, pDataVDO_req->Name, sizeof(pParameterVDO->Name));  //名字
				memcpy(pParameterVDO->Alias, pDataVDO_req->Alias, sizeof(pParameterVDO->Alias)); //别名
				memcpy(pParameterVDO->Description, pDataVDO_req->Description, sizeof(pParameterVDO->Description)); //描述
				pParameterVDO->InitValue = BES200_htonl(pDataVDO_req->InitValue);  //初始值

				/*报警相关*/
				pParameterVDO->AlarmActive = pDataVDO_req->AlarmActive;//报警是否启用
				pParameterVDO->AlarmType = pDataVDO_req->AlarmType; //报警类型
				pParameterVDO->AlarmPriority = pDataVDO_req->AlarmPriority;//报警优先级

				pParameterVDO->AlarmTrigger = pDataVDO_req->AlarmTrigger;//报警触发
			}
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}
/*获取一个逻辑点的报警信息*/
int CCrossFunManager:: BES200_GetLogicalPointAlarmInfor(const char *IPAddr, uint16_t LogicalPointID, struct LogicalPointAlarmInfor *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                                          *pHead_req;
	BEGP_RSP_HEAD_T                                          *pHead_rsp;
	BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T      *pData_rep;
	BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T      *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_LOGICAL_POINT_ALARM_INFOR;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_rep->ID = BES200_htons(LogicalPointID);    //要获取点的ID号

	SendDataSize += BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_GET_LOGICAL_POINT_ALARM_INFOR) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_SIZE))
		{
			ret = BES200_ERROR_OK;
			sprintf_s(*errmsg,2048, "err:%d", ret);

			pParameter->CurrentValue = BES200_htonl(pData_rsp->CurrentValue);
			pParameter->AlarmState = pData_rsp->AlarmState;
			pParameter->AlarmTriggerValue = BES200_htonl(pData_rsp->AlarmTriggerValue);
			pParameter->AlarmMaxValue = BES200_htonl(pData_rsp->AlarmMaxValue);
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}

/* 添加一条计划 */
int CCrossFunManager:: BES200_AddScheduler(const char *IPAddr, uint16_t SchedulerID, const struct SchedulerParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                            *pHead_req;
	BEGP_RSP_HEAD_T                            *pHead_rsp;
	BEGP_REQ_PACKETDATA_ADD_SCHEDULER_T        *pData_rep;
	BEGP_RSP_PACKETDATA_ECHO_T                 *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (BEGP_REQ_PACKETDATA_ADD_SCHEDULER_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_ADD_SCHEDULER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_ADD_SCHEDULER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_rep->ID = BES200_htons(SchedulerID);//计划的ID号
	pData_rep->Active = pParameter->Active;//计划是否启用
	memcpy(pData_rep->Name, pParameter->Name, sizeof(pData_rep->Name));//计划的名字
	memcpy(pData_rep->Alias, pParameter->Alias, sizeof(pData_rep->Alias));//计划的别名
	pData_rep->PlanType = pParameter->PlanType;//计划的类型

	pData_rep->StartDateYear = pParameter->StartDateYear;//计划的开始日期 年
	pData_rep->StartDateMonth = pParameter->StartDateMonth;//计划的开始日期 月
	pData_rep->StartDateDay = pParameter->StartDateDay;//计划的开始日期  天
	pData_rep->EndDateYear = pParameter->EndDateYear;//计划的结束日期 年
	pData_rep->EndDateMonth = pParameter->EndDateMonth;//计划的结束日期 月
	pData_rep->EndDateDay = pParameter->EndDateDay;//计划的结束日期  天

	pData_rep->StartTimeHour = pParameter->StartTimeHour;//计划的开始时间  时
	pData_rep->StartTimeMinute = pParameter->StartTimeMinute;//计划的开始时间  分
	pData_rep->StartTimeSecond = pParameter->StartTimeSecond;//计划的开始时间  秒
	pData_rep->EndTimeHour = pParameter->EndTimeHour;//计划的结束时间  时
	pData_rep->EndTimeMinute = pParameter->EndTimeMinute;//计划的结束时间  分
	pData_rep->EndTimeSecond = pParameter->EndTimeSecond;//计划的结束时间  秒

	pData_rep->ExecutionWay = pParameter->ExecutionWay;//计划的执行方式
	pData_rep->WeekMask = pParameter->WeekMask;//计划的周掩码
	pData_rep->SceneType = pParameter->SceneType;//计划的场景类型
	pData_rep->SceneID = BES200_htons(pParameter->SceneID);//计划的场景ID
	pData_rep->ModeID = pParameter->ModeID;//计划的模式ID
	pData_rep->DefaultModeID = pParameter->DefaultModeID;//默认模式ID

	SendDataSize += BEGP_REQ_PACKETDATA_ADD_SCHEDULER_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);

	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_ADD_SCHEDULER) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}
/* 设置一条计划的所有配置参数 */
int CCrossFunManager:: BES200_SetSchedulerParameter(const char *IPAddr, uint16_t SchedulerID, const struct SchedulerParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                                    *pHead_req;
	BEGP_RSP_HEAD_T                                    *pHead_rsp;
	BEGP_REQ_PACKETDATA_SET_SCHEDULER_PARAMETER_T      *pData_rep;
	BEGP_RSP_PACKETDATA_ECHO_T                         *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (BEGP_REQ_PACKETDATA_SET_SCHEDULER_PARAMETER_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_SCHEDULER_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_SCHEDULER_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_rep->ID = BES200_htons(SchedulerID);//计划的ID号
	pData_rep->Active = pParameter->Active;//计划是否启用
	memcpy(pData_rep->Name, pParameter->Name, sizeof(pData_rep->Name));//计划的名字
	memcpy(pData_rep->Alias, pParameter->Alias, sizeof(pData_rep->Alias));//计划的别名
	pData_rep->PlanType = pParameter->PlanType;//计划的类型

	pData_rep->StartDateYear = pParameter->StartDateYear;//计划的开始日期 年
	pData_rep->StartDateMonth = pParameter->StartDateMonth;//计划的开始日期 月
	pData_rep->StartDateDay = pParameter->StartDateDay;//计划的开始日期  天
	pData_rep->EndDateYear = pParameter->EndDateYear;//计划的结束日期 年
	pData_rep->EndDateMonth = pParameter->EndDateMonth;//计划的结束日期 月
	pData_rep->EndDateDay = pParameter->EndDateDay;//计划的结束日期  天

	pData_rep->StartTimeHour = pParameter->StartTimeHour;//计划的开始时间  时
	pData_rep->StartTimeMinute = pParameter->StartTimeMinute;//计划的开始时间  分
	pData_rep->StartTimeSecond = pParameter->StartTimeSecond;//计划的开始时间  秒
	pData_rep->EndTimeHour = pParameter->EndTimeHour;//计划的结束时间  时
	pData_rep->EndTimeMinute = pParameter->EndTimeMinute;//计划的结束时间  分
	pData_rep->EndTimeSecond = pParameter->EndTimeSecond;//计划的结束时间  秒

	pData_rep->ExecutionWay = pParameter->ExecutionWay;//计划的执行方式
	pData_rep->WeekMask = pParameter->WeekMask;//计划的周掩码
	pData_rep->SceneType = pParameter->SceneType;//计划的场景类型
	pData_rep->SceneID = BES200_htons(pParameter->SceneID);//计划的场景ID
	pData_rep->ModeID = pParameter->ModeID;//计划的模式ID
	pData_rep->DefaultModeID = pParameter->DefaultModeID;//默认模式ID

	SendDataSize += BEGP_REQ_PACKETDATA_SET_SCHEDULER_PARAMETER_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);

	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_SET_SCHEDULER_PARAMETER) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}

/* 删除一条计划 */
int CCrossFunManager:: BES200_DeleteScheduler(const char *IPAddr, uint16_t SchedulerID, char **errmsg)
{
	BEGP_REQ_HEAD_T                                    *pHead_req;
	BEGP_RSP_HEAD_T                                    *pHead_rsp;
	BEGP_REQ_PACKETDATA_DELETE_SCHEDULER_T             *pData_rep;
	BEGP_RSP_PACKETDATA_ECHO_T                         *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (BEGP_REQ_PACKETDATA_DELETE_SCHEDULER_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_DELETE_SCHEDULER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_DELETE_SCHEDULER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_rep->ID = BES200_htons(SchedulerID);//计划的ID号

	SendDataSize += BEGP_REQ_PACKETDATA_DELETE_SCHEDULER_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);

	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_DELETE_SCHEDULER) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}

/*获取一条计划的所有配置参数*/
int CCrossFunManager:: BES200_GetSchedulerParameter(const char *IPAddr, uint16_t SchedulerID, struct SchedulerParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                                      *pHead_req;
	BEGP_RSP_HEAD_T                                      *pHead_rsp;
	BEGP_REQ_PACKETDATA_GET_SCHEDULER_PARAMETER_T        *pData_rep;
	BEGP_RSP_PACKETDATA_GET_SCHEDULER_PARAMETER_T        *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//正文数据，指针赋值
	pData_rep = (BEGP_REQ_PACKETDATA_GET_SCHEDULER_PARAMETER_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_SCHEDULER_PARAMETER_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_SCHEDULER_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_GET_SCHEDULER_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_rep->ID = BES200_htons(SchedulerID);    //要获取点的ID号

	SendDataSize += BEGP_REQ_PACKETDATA_GET_SCHEDULER_PARAMETER_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_GET_SCHEDULER_PARAMETER) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_GET_SCHEDULER_PARAMETER_SIZE))
		{
			ret = BES200_ERROR_OK;
			sprintf_s(*errmsg,2048, "err:%d", ret);

			pParameter->Active = pData_rsp->Active;//计划是否启用
			memcpy(pParameter->Name, pData_rsp->Name, sizeof(pParameter->Name));//计划的名字
			memcpy(pParameter->Alias, pData_rsp->Alias, sizeof(pParameter->Alias));//计划的别名
			pParameter->PlanType = pData_rsp->PlanType;//计划的类型

			pParameter->StartDateYear = pData_rsp->StartDateYear;//计划的开始日期 年
			pParameter->StartDateMonth = pData_rsp->StartDateMonth;//计划的开始日期 月
			pParameter->StartDateDay = pData_rsp->StartDateDay;//计划的开始日期  天
			pParameter->EndDateYear = pData_rsp->EndDateYear;//计划的结束日期 年
			pParameter->EndDateMonth = pData_rsp->EndDateMonth;//计划的结束日期 月
			pParameter->EndDateDay = pData_rsp->EndDateDay;//计划的结束日期  天

			pParameter->StartTimeHour = pData_rsp->StartTimeHour;//计划的开始时间  时
			pParameter->StartTimeMinute = pData_rsp->StartTimeMinute;//计划的开始时间  分
			pParameter->StartTimeSecond = pData_rsp->StartTimeSecond;//计划的开始时间  秒
			pParameter->EndTimeHour = pData_rsp->EndTimeHour;//计划的结束时间  时
			pParameter->EndTimeMinute = pData_rsp->EndTimeMinute;//计划的结束时间  分
			pParameter->EndTimeSecond = pData_rsp->EndTimeSecond;//计划的结束时间  秒

			pParameter->ExecutionWay = pData_rsp->ExecutionWay;//计划的执行方式
			pParameter->WeekMask = pData_rsp->WeekMask;//计划的周掩码
			pParameter->SceneType = pData_rsp->SceneType;//计划的场景类型
			pParameter->SceneID = BES200_htons(pData_rsp->SceneID);//计划的场景ID
			pParameter->ModeID = pData_rsp->ModeID;//计划的模式ID
			pParameter->DefaultModeID = pData_rsp->DefaultModeID;//默认模式ID
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}

/*添加一个场景*/
int CCrossFunManager:: BES200_AddScene(const char *IPAddr, uint16_t SceneID, const struct SceneParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_REQ_PACKETDATA_ADD_SCENE_T                *pData_req;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize, i, j;
	char SendBuf[SCENE_SENDBUF_SIZE];
	char RecvBuf[SCENE_RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_req = (BEGP_REQ_PACKETDATA_ADD_SCENE_T*)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_ADD_SCENE;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_ADD_SCENE_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	if (pParameter->SceneType == SCENE_TYPE_CONTROL) //控制场景
	{
		BEGP_CONTROL_SCENE_PARAMETER_T        *pParameterControlScene;
		BEGP_CONTROL_SCENE_PARAMETER_T        *pDataControlScene_req;

		pDataControlScene_req = (BEGP_CONTROL_SCENE_PARAMETER_T*)pData_req;
		pParameterControlScene = (BEGP_CONTROL_SCENE_PARAMETER_T *)pParameter;//字节序必须要一致，在BES200.h中要1字节对齐

		pDataControlScene_req->SceneType = pParameterControlScene->SceneType;//场景类型  
		pDataControlScene_req->ID = BES200_htons(SceneID);//场景ID
		pDataControlScene_req->Active = pParameterControlScene->Active;//控制场景启用
		memcpy(pDataControlScene_req->Name, pParameterControlScene->Name, sizeof(pDataControlScene_req->Name));//场景的名字
		memcpy(pDataControlScene_req->Alias, pParameterControlScene->Alias, sizeof(pDataControlScene_req->Alias));//场景的名字
		for (i = 0; i < CONTROL_MODE_SIZE; i++)
		{
			pDataControlScene_req->ControlMode[i].Active = pParameterControlScene->ControlMode[i].Active;//模式是否启用
			memcpy(pDataControlScene_req->ControlMode[i].Name, pParameterControlScene->ControlMode[i].Name, sizeof(pDataControlScene_req->ControlMode[i].Name));//模式的名字
			pDataControlScene_req->ControlMode[i].ID = pParameterControlScene->ControlMode[i].ID;//模式的ID
			for (j = 0; j < CONTROL_POINT_SIZE; j++)
			{
				pDataControlScene_req->ControlMode[i].ControlPoint[j].Active = pParameterControlScene->ControlMode[i].ControlPoint[j].Active;//点是否启用
				pDataControlScene_req->ControlMode[i].ControlPoint[j].PointID = BES200_htons(pParameterControlScene->ControlMode[i].ControlPoint[j].PointID);//点的ID
				pDataControlScene_req->ControlMode[i].ControlPoint[j].RunValue = BES200_htonl(pParameterControlScene->ControlMode[i].ControlPoint[j].RunValue);//点的运行值
			}
		}
	}
	else if (pParameter->SceneType == SCENE_TYPE_COLLECT) //采集场景
	{
		BEGP_COLLECT_SCENE_PARAMETER_T        *pParameterCollectScene;
		BEGP_COLLECT_SCENE_PARAMETER_T        *pDataCollectScene_req;

		pDataCollectScene_req = (BEGP_COLLECT_SCENE_PARAMETER_T*)pData_req;
		pParameterCollectScene = (BEGP_COLLECT_SCENE_PARAMETER_T *)pParameter;//字节序必须要一致，在BES200.h中要1字节对齐

		pDataCollectScene_req->SceneType = pParameterCollectScene->SceneType;//场景类型  
		pDataCollectScene_req->ID = BES200_htons(SceneID);//场景ID
		pDataCollectScene_req->Active = pParameterCollectScene->Active;//控制场景启用
		memcpy(pDataCollectScene_req->Name, pParameterCollectScene->Name, sizeof(pDataCollectScene_req->Name));//场景的名字
		memcpy(pDataCollectScene_req->Alias, pParameterCollectScene->Alias, sizeof(pDataCollectScene_req->Alias));//场景的名字
		for (i = 0; i < COLLECT_MODE_SIZE; i++)
		{
			pDataCollectScene_req->CollectMode[i].Active = pParameterCollectScene->CollectMode[i].Active;//模式是否启用
			memcpy(pDataCollectScene_req->CollectMode[i].Name, pParameterCollectScene->CollectMode[i].Name, sizeof(pDataCollectScene_req->CollectMode[i].Name));//模式的名字
			pDataCollectScene_req->CollectMode[i].ID = pParameterCollectScene->CollectMode[i].ID;//模式的ID
			for (j = 0; j < CONTROL_POINT_SIZE; j++)
			{
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].Active = pParameterCollectScene->CollectMode[i].CollectPoint[j].Active;//点是否启用
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].PointID = BES200_htons(pParameterCollectScene->CollectMode[i].CollectPoint[j].PointID);//点的ID
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].CollectID = pParameterCollectScene->CollectMode[i].CollectPoint[j].CollectID;//点的采集方式
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].TimeUnit = pParameterCollectScene->CollectMode[i].CollectPoint[j].TimeUnit;//点的时间间隔采集的单位
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].Value = BES200_htons(pParameterCollectScene->CollectMode[i].CollectPoint[j].Value);//采集值
			}
		}
	}

	SendDataSize += BEGP_REQ_PACKETDATA_ADD_SCENE_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_ADD_SCENE) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}

/*设置一个场景的所有配置参数*/
int CCrossFunManager:: BES200_SetSceneParameter(const char *IPAddr, uint16_t SceneID, const struct SceneParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_REQ_PACKETDATA_SET_SCENE_PARAMETER_T      *pData_req;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize, i, j;
	char SendBuf[SCENE_SENDBUF_SIZE];
	char RecvBuf[SCENE_RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_req = (BEGP_REQ_PACKETDATA_SET_SCENE_PARAMETER_T*)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_SCENE_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_SCENE_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	if (pParameter->SceneType == SCENE_TYPE_CONTROL) //控制场景
	{
		BEGP_CONTROL_SCENE_PARAMETER_T        *pParameterControlScene;
		BEGP_CONTROL_SCENE_PARAMETER_T        *pDataControlScene_req;

		pDataControlScene_req = (BEGP_CONTROL_SCENE_PARAMETER_T*)pData_req;
		pParameterControlScene = (BEGP_CONTROL_SCENE_PARAMETER_T *)pParameter;//字节序必须要一致，在BES200.h中要1字节对齐

		pDataControlScene_req->SceneType = pParameterControlScene->SceneType;//场景类型  
		pDataControlScene_req->ID = BES200_htons(SceneID);//场景ID
		pDataControlScene_req->Active = pParameterControlScene->Active;//控制场景启用
		memcpy(pDataControlScene_req->Name, pParameterControlScene->Name, sizeof(pDataControlScene_req->Name));//场景的名字
		memcpy(pDataControlScene_req->Alias, pParameterControlScene->Alias, sizeof(pDataControlScene_req->Alias));//场景的名字
		for (i = 0; i < CONTROL_MODE_SIZE; i++)
		{
			pDataControlScene_req->ControlMode[i].Active = pParameterControlScene->ControlMode[i].Active;//模式是否启用
			memcpy(pDataControlScene_req->ControlMode[i].Name, pParameterControlScene->ControlMode[i].Name, sizeof(pDataControlScene_req->ControlMode[i].Name));//模式的名字
			pDataControlScene_req->ControlMode[i].ID = pParameterControlScene->ControlMode[i].ID;//模式的ID
			for (j = 0; j < CONTROL_POINT_SIZE; j++)
			{
				pDataControlScene_req->ControlMode[i].ControlPoint[j].Active = pParameterControlScene->ControlMode[i].ControlPoint[j].Active;//点是否启用
				pDataControlScene_req->ControlMode[i].ControlPoint[j].PointID = BES200_htons(pParameterControlScene->ControlMode[i].ControlPoint[j].PointID);//点的ID
				pDataControlScene_req->ControlMode[i].ControlPoint[j].RunValue = BES200_htonl(pParameterControlScene->ControlMode[i].ControlPoint[j].RunValue);//点的运行值
			}
		}
	}
	else if (pParameter->SceneType == SCENE_TYPE_COLLECT) //采集场景
	{
		BEGP_COLLECT_SCENE_PARAMETER_T        *pParameterCollectScene;
		BEGP_COLLECT_SCENE_PARAMETER_T        *pDataCollectScene_req;

		pDataCollectScene_req = (BEGP_COLLECT_SCENE_PARAMETER_T*)pData_req;
		pParameterCollectScene = (BEGP_COLLECT_SCENE_PARAMETER_T *)pParameter;//字节序必须要一致，在BES200.h中要1字节对齐

		pDataCollectScene_req->SceneType = pParameterCollectScene->SceneType;//场景类型  
		pDataCollectScene_req->ID = BES200_htons(SceneID);//场景ID
		pDataCollectScene_req->Active = pParameterCollectScene->Active;//控制场景启用
		memcpy(pDataCollectScene_req->Name, pParameterCollectScene->Name, sizeof(pDataCollectScene_req->Name));//场景的名字
		memcpy(pDataCollectScene_req->Alias, pParameterCollectScene->Alias, sizeof(pDataCollectScene_req->Alias));//场景的名字
		for (i = 0; i < COLLECT_MODE_SIZE; i++)
		{
			pDataCollectScene_req->CollectMode[i].Active = pParameterCollectScene->CollectMode[i].Active;//模式是否启用
			memcpy(pDataCollectScene_req->CollectMode[i].Name, pParameterCollectScene->CollectMode[i].Name, sizeof(pDataCollectScene_req->CollectMode[i].Name));//模式的名字
			pDataCollectScene_req->CollectMode[i].ID = pParameterCollectScene->CollectMode[i].ID;//模式的ID
			for (j = 0; j < CONTROL_POINT_SIZE; j++)
			{
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].Active = pParameterCollectScene->CollectMode[i].CollectPoint[j].Active;//点是否启用
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].PointID = BES200_htons(pParameterCollectScene->CollectMode[i].CollectPoint[j].PointID);//点的ID
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].CollectID = pParameterCollectScene->CollectMode[i].CollectPoint[j].CollectID;//点的采集方式
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].TimeUnit = pParameterCollectScene->CollectMode[i].CollectPoint[j].TimeUnit;//点的时间间隔采集的单位
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].Value = BES200_htons(pParameterCollectScene->CollectMode[i].CollectPoint[j].Value);//采集值
			}
		}
	}

	SendDataSize += BEGP_REQ_PACKETDATA_SET_SCENE_PARAMETER_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_SET_SCENE_PARAMETER) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}
/*删除一个场景*/
int CCrossFunManager:: BES200_DeleteScene(const char *IPAddr, uint16_t SceneID, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_REQ_PACKETDATA_DELETE_SCENE_T             *pData_req;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SCENE_SENDBUF_SIZE];
	char RecvBuf[SCENE_RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_req = (BEGP_REQ_PACKETDATA_DELETE_SCENE_T*)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_DELETE_SCENE;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_DELETE_SCENE_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造正文数据
	pData_req->ID = BES200_htons(SceneID);//要删除的场景的ID号

	SendDataSize += BEGP_REQ_PACKETDATA_DELETE_SCENE_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_DELETE_SCENE) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_ECHO_SIZE))
		{
			if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_SUCCESS)
			{
				ret = BES200_ERROR_OK;
				sprintf_s(*errmsg,2048, "err:%d", ret);
			}
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_DATASIZEERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_INDEXERR)
				ret = BES200_ERROR_REFUSED;
			else if (pData_rsp->Code == BEGP_PACKETDATA_ECHO_CODE_VALUEERR)
				ret = BES200_ERROR_REFUSED;
			else
				ret = BES200_ERROR_REFUSED;
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}
/*获取一个场景的所有配置参数*/
int CCrossFunManager:: BES200_GetSceneParameter(const char *IPAddr, uint16_t SceneID, struct SceneParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_T      *pData_req;
	BEGP_RSP_PACKETDATA_GET_SCENE_PARAMETER_T      *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize, i, j;
	char SendBuf[SCENE_SENDBUF_SIZE];
	char RecvBuf[SCENE_RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//帧头数据，指针赋值
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//正文数据，指针赋值
	pData_req = (BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_T*)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_SCENE_PARAMETER_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//构造请求包头
	pHead_req->SlaveAddress = 1;//这是DDC的识别号，用网络通信时默认设置成1即可
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_SCENE_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//构造请求正文数据
	pData_req->ID = BES200_htons(SceneID);//要获取的场景参数的ID号

	SendDataSize += BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_SIZE;

	ret = BES200_Communication(IPAddr, SendBuf, SendDataSize, RecvBuf, &RecvDataSize);
	if (ret == BES200_ERROR_OK)
	{
		if ((pHead_rsp->SlaveAddress == pHead_req->SlaveAddress) && \
			(pHead_rsp->FunctionCode == BEGP_FUNCODE_GET_SCENE_PARAMETER) && \
			(pHead_rsp->DataSize) == BES200_htons(BEGP_RSP_PACKETDATA_GET_SCENE_PARAMETER_SIZE))
		{

			ret = BES200_ERROR_OK;
			sprintf_s(*errmsg,2048, "err:%d", ret);
			if (pData_rsp->SceneType == SCENE_TYPE_CONTROL) //控制场景
			{
				BEGP_CONTROL_SCENE_PARAMETER_T        *pParameterControlScene;
				BEGP_CONTROL_SCENE_PARAMETER_T        *pDataControlScene_rsp;

				pDataControlScene_rsp = (BEGP_CONTROL_SCENE_PARAMETER_T*)pData_rsp;
				pParameterControlScene = (BEGP_CONTROL_SCENE_PARAMETER_T *)pParameter;//字节序必须要一致，在BES200.h中要1字节对齐

				

				pParameterControlScene->SceneType = pDataControlScene_rsp->SceneType;//场景类型  
				pParameterControlScene->ID = BES200_htons(pDataControlScene_rsp->ID);//场景ID
				pParameterControlScene->Active = pDataControlScene_rsp->Active;//控制场景启用
				memcpy(pParameterControlScene->Name, pDataControlScene_rsp->Name, sizeof(pParameterControlScene->Name));//场景的名字
				memcpy(pParameterControlScene->Alias, pDataControlScene_rsp->Alias, sizeof(pParameterControlScene->Alias));//场景的名字
				for (i = 0; i < CONTROL_MODE_SIZE; i++)
				{
					pParameterControlScene->ControlMode[i].Active = pDataControlScene_rsp->ControlMode[i].Active;//模式是否启用
					memcpy(pParameterControlScene->ControlMode[i].Name, pDataControlScene_rsp->ControlMode[i].Name, sizeof(pParameterControlScene->ControlMode[i].Name));//模式的名字
					pParameterControlScene->ControlMode[i].ID = pDataControlScene_rsp->ControlMode[i].ID;//模式的ID
					for (j = 0; j < CONTROL_POINT_SIZE; j++)
					{
						pParameterControlScene->ControlMode[i].ControlPoint[j].Active = pDataControlScene_rsp->ControlMode[i].ControlPoint[j].Active;//点是否启用
						pParameterControlScene->ControlMode[i].ControlPoint[j].PointID = BES200_htons(pDataControlScene_rsp->ControlMode[i].ControlPoint[j].PointID);//点的ID
						pParameterControlScene->ControlMode[i].ControlPoint[j].RunValue = BES200_htonl(pDataControlScene_rsp->ControlMode[i].ControlPoint[j].RunValue);//点的运行值
					}
				}
			}
			else if (pData_rsp->SceneType == SCENE_TYPE_COLLECT) //采集场景
			{
				BEGP_COLLECT_SCENE_PARAMETER_T        *pParameterCollectScene;
				BEGP_COLLECT_SCENE_PARAMETER_T        *pDataCollectScene_rsp;

				pDataCollectScene_rsp = (BEGP_COLLECT_SCENE_PARAMETER_T*)pData_rsp;
				pParameterCollectScene = (BEGP_COLLECT_SCENE_PARAMETER_T *)pParameter;//字节序必须要一致，在BES200.h中要1字节对齐

				pParameterCollectScene->SceneType = pDataCollectScene_rsp->SceneType;//场景类型  
				pParameterCollectScene->ID = BES200_htons(pDataCollectScene_rsp->ID);//场景ID
				pParameterCollectScene->Active = pDataCollectScene_rsp->Active;//控制场景启用
				memcpy(pParameterCollectScene->Name, pDataCollectScene_rsp->Name, sizeof(pParameterCollectScene->Name));//场景的名字
				memcpy(pParameterCollectScene->Alias, pDataCollectScene_rsp->Alias, sizeof(pParameterCollectScene->Alias));//场景的名字
				for (i = 0; i < COLLECT_MODE_SIZE; i++)
				{
					pParameterCollectScene->CollectMode[i].Active = pDataCollectScene_rsp->CollectMode[i].Active;//模式是否启用
					memcpy(pParameterCollectScene->CollectMode[i].Name, pDataCollectScene_rsp->CollectMode[i].Name, sizeof(pParameterCollectScene->CollectMode[i].Name));//模式的名字
					pParameterCollectScene->CollectMode[i].ID = pDataCollectScene_rsp->CollectMode[i].ID;//模式的ID
					for (j = 0; j < CONTROL_POINT_SIZE; j++)
					{
						pParameterCollectScene->CollectMode[i].CollectPoint[j].Active = pDataCollectScene_rsp->CollectMode[i].CollectPoint[j].Active;//点是否启用
						pParameterCollectScene->CollectMode[i].CollectPoint[j].PointID = BES200_htons(pDataCollectScene_rsp->CollectMode[i].CollectPoint[j].PointID);//点的ID
						pParameterCollectScene->CollectMode[i].CollectPoint[j].CollectID = pDataCollectScene_rsp->CollectMode[i].CollectPoint[j].CollectID;//点的采集方式
						pParameterCollectScene->CollectMode[i].CollectPoint[j].TimeUnit = pDataCollectScene_rsp->CollectMode[i].CollectPoint[j].TimeUnit;//点的时间间隔采集的单位
						pParameterCollectScene->CollectMode[i].CollectPoint[j].Value = BES200_htons(pDataCollectScene_rsp->CollectMode[i].CollectPoint[j].Value);//采集值
					}
				}
			}
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//错误码处理
		if (ret == BES200_ERROR_SEND)
		{
			sprintf_s(*errmsg,2048, "Send Error %d", ret);
		}
		else if (ret == BES200_ERROR_CONNECT)
		{
			sprintf_s(*errmsg,2048, "Connect error %d", ret);
		}
		else if (ret == BES200_ERROR_RECIVE)
		{
			sprintf_s(*errmsg,2048, "Recive Error:%d", ret);
		}
		else
		{
			sprintf_s(*errmsg,2048, "不可能");
		}
	}
	return ret;
}



