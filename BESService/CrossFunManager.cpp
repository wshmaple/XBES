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

// BES200_TEST.cpp : �������̨Ӧ�ó������ڵ㡣
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

	/*���հ�ͷ*/
	len = m_TCP.Recv(ServerIP, DEF_CROSS_SERVER_PORT,(void*)pRecvData, BEGP_RSP_HEAD_SIZE);
	if (len != BEGP_RSP_HEAD_SIZE)
	{
		return -1;
	}


	DataSize += BEGP_RSP_HEAD_SIZE;
	FrameSize = ttBES200_htons(pHead_rsp->DataSize) + BEGP_CRC_SIZE;

	/*�����������ݺ�У������*/
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
	/*�յ�һ֡���ݣ���ʼ����*/
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
/*����һ��������*/
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
	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;



	//�������ݣ�ָ�븳ֵ
	pData_req = (BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_T*)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_ADD_CONTROLLER_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_ADD_CONTROLLER_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}
/*�޸Ŀ����������в���*/
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
	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//�������ݣ�ָ�븳ֵ
	pData_req = (BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_T*)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_CONTROLLER_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_CONTROLLER_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}

/* ���ÿ�������ʱ�� */
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
	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//�������ݣ�ָ�븳ֵ
	pData_req = (BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_T*)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_CONTROLLER_TIME;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_CONTROLLER_TIME_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
	pData_req->year = pTime->tm_year - 2000;//��
	pData_req->month = pTime->tm_mon;  //��
	pData_req->day = pTime->tm_mday;  //��
	pData_req->hour = pTime->tm_hour;  //ʱ
	pData_req->minute = pTime->tm_min; //��
	pData_req->second = pTime->tm_sec;  //��
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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}

/*����DDC,�൱�������λ*/
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

	//�������ݣ�ָ�븳ֵ
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}

	return ret;
}
/*���ÿ��������ָ��������ã�������*/
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

	//�������ݣ�ָ�븳ֵ
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}

	return ret;
}

/*Զ������*/
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

	//�������ݣ�ָ�븳ֵ
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}

	return ret;

}
/*ɾ��������*/
int CCrossFunManager:: BES200_DeleteController(const char *IPAddr, char **errmsg)
{
	BEGP_REQ_HEAD_T                                *pHead_req;
	BEGP_RSP_HEAD_T                                *pHead_rsp;
	BEGP_RSP_PACKETDATA_ECHO_T                     *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);
	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//�������ݣ�ָ�븳ֵ
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}
/*��ȡ��������ʱ��*/
int CCrossFunManager:: BES200_GetControllerTime(const char *IPAddr, struct tm *pTime, char **errmsg)
{
	BEGP_REQ_HEAD_T                              *pHead_req;
	BEGP_RSP_HEAD_T                              *pHead_rsp;
	BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_T    *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];

	*errmsg = (char *)malloc(2048);

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//�������ݣ�ָ�븳ֵ
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_CONTROLLER_TIME_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}

/*��ȡ���������������ò���*/
int CCrossFunManager:: BES200_GetControllerParameter(const char *IPAddr, struct ControllerParameter *pParameter, char **errmsg)
{
	BEGP_REQ_HEAD_T                                  *pHead_req;
	BEGP_RSP_HEAD_T                                  *pHead_rsp;
	BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_T    *pData_rsp;
	int ret, SendDataSize = 0, RecvDataSize;
	char SendBuf[SENDBUF_SIZE];
	char RecvBuf[RECVBUF_SIZE];


	*errmsg = (char *)malloc(2048);

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//�������ݣ�ָ�븳ֵ
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_CONTROLLER_PARAMETER_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
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
			pParameter->Active = pData_rsp->Active;  //�Ƿ�����
			memcpy(pParameter->Alias, pData_rsp->Alias, sizeof(pParameter->Alias));  //DDC����
			memcpy(pParameter->Description, pData_rsp->Description, sizeof(pParameter->Description));  //����
			pParameter->ID = pData_rsp->ID; //ID��
			memcpy(pParameter->Location, pData_rsp->Location, sizeof(pParameter->Location));  //��װλ��
			memcpy(pParameter->Name, pData_rsp->Name, sizeof(pParameter->Name)); //����
			memcpy(pParameter->Zone, pData_rsp->Zone, sizeof(pParameter->Zone)); //����
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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}
/*�㲥��ʽ��ȡ���п�������Ϣ*/
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


	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];
	pCrcSend = (uint16_t*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pCrcRecv = (uint16_t*)(&RecvBuf[BEGP_RSP_HEAD_SIZE + BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_SIZE]);
	//�������ݣ�ָ�븳ֵ
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	/*����ͷ���ݸ�ֵ*/
	pHead_req->SlaveAddress = 1;
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_CONTROLLER_INFO;
	pHead_req->SequenceNumber = 1;
	pHead_req->DataSize = 0;
	SendDataSize = BEGP_REQ_HEAD_SIZE;

	/*����CRC16*/
	*pCrcSend = BES200_CRC_16(SendBuf, SendDataSize);//����CRC16
	SendDataSize += BEGP_CRC_SIZE;

	*errmsg = (char *)malloc(1024);
	*pInfo = (struct ControllerInfo *)malloc(sizeof(struct ControllerInfo) * 1000);//���֧��1000���豸
	if (*errmsg == NULL)
		return BES200_ERROR_CONNECT;

	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_socket)
	{
		ret = WSAGetLastError();
		sprintf_s(*errmsg,2048, "socket()ʧ�� :%d", ret);
		WSACleanup();
		return BES200_ERROR_CONNECT;
	}

	/*�ͻ��˵�ַ*/
	m_addrfrom.sin_family = AF_INET;
	m_addrfrom.sin_port = htons(1204);
	m_addrfrom.sin_addr.S_un.S_addr = INADDR_ANY;

	/*����˵�ַ*/
	m_addrto.sin_family = AF_INET;
	m_addrto.sin_port = htons(1204);
	m_addrto.sin_addr.S_un.S_addr = INADDR_BROADCAST;

	//���ù㲥
	bool bBroadcast = true;
	setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (char FAR *)&bBroadcast, sizeof(bBroadcast));
	//���ó�ʱ
	int nNetTimeout;
	nNetTimeout = 1000;//����
	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char FAR *)&nNetTimeout, sizeof(nNetTimeout));

	ret = ::bind(m_socket, (sockaddr*)&m_addrfrom, sizeof(sockaddr));
	if (ret == SOCKET_ERROR)
	{
		ret = WSAGetLastError();
		sprintf_s(*errmsg,2048, "bind()ʧ�� %d!", ret);
		ret = BES200_ERROR_CONNECT;
		goto out;
	}

	int len = sizeof(sockaddr);
	int res = sendto(m_socket, (char*)SendBuf, SendDataSize, 0, (sockaddr*)&m_addrto, len);

	if (res == (BEGP_REQ_HEAD_SIZE + BEGP_CRC_SIZE))
	{
		int index = 0;
		//һֱ���գ�ֱ�����ճ�ʱ���������û������
		while (1)
		{
			RecvDataSize = recvfrom(m_socket, RecvBuf, RECVBUF_SIZE, 0, (sockaddr*)&m_addrfrom, &len);
			if (RecvDataSize == -1 || index >= 1000)
			{
				break;
			}
			else if (RecvDataSize == BEGP_RSP_PACKETDATA_GET_CONTROLLER_INFO_SIZE + BEGP_CRC_SIZE + BEGP_RSP_HEAD_SIZE)
			{//���ݳ�����ȷ
				//CRC16У��
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
		sprintf_s(*errmsg,2048, "sendto len = %dʧ�ܣ�res = %d", sizeof(BEGP_REQ_HEAD_T), res);
		ret = BES200_ERROR_SEND;
	}
out:
	closesocket(m_socket);
	return ret;
}

/*�㲥��ʽ����MAC��ַ���ÿ�������IP��ַ����Ϣ*/
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//�������ݣ�ָ�븳ֵ
	pData_req = (BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	/*CRC16ָ�븳ֵ*/
	pCrcSend = (uint16_t*)(&SendBuf[BEGP_REQ_HEAD_SIZE + BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_SIZE]);
	pCrcRecv = (uint16_t*)(&RecvBuf[BEGP_RSP_HEAD_SIZE + BEGP_RSP_PACKETDATA_ECHO_SIZE]);

	/*����ͷ��ֵ*/
	pHead_req->SlaveAddress = 1;
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_CONTROLLER_INFO;
	pHead_req->SequenceNumber = 1;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_CONTROLLER_INFO_SIZE);
	SendDataSize = BEGP_REQ_HEAD_SIZE;

	/*�������ݸ�ֵ*/
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

	/*����CRC*/
	*pCrcSend = BES200_CRC_16(SendBuf, SendDataSize);
	SendDataSize += BEGP_CRC_SIZE;

	*errmsg = (char *)malloc(1024);
	if (*errmsg == NULL)
		return BES200_ERROR_CONNECT;

	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_socket)
	{
		ret = WSAGetLastError();
		sprintf_s(*errmsg,2048, "socket()ʧ�� :%d", ret);
		WSACleanup();
		return BES200_ERROR_CONNECT;
	}

	/*�ͻ��˵�ַ*/
	m_addrfrom.sin_family = AF_INET;
	m_addrfrom.sin_port = htons(1204);
	m_addrfrom.sin_addr.S_un.S_addr = INADDR_ANY;

	/*����˵�ַ*/
	m_addrto.sin_family = AF_INET;
	m_addrto.sin_port = htons(1204);
	m_addrto.sin_addr.S_un.S_addr = INADDR_BROADCAST;

	//���ù㲥
	bool bBroadcast = true;
	setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (char FAR *)&bBroadcast, sizeof(bBroadcast));
	//���ó�ʱ
	int nNetTimeout;
	nNetTimeout = 1000;
	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char FAR *)&nNetTimeout, sizeof(nNetTimeout));

	ret = ::bind(m_socket, (sockaddr*)&m_addrfrom, sizeof(sockaddr));
	if (ret == SOCKET_ERROR)
	{
		ret = WSAGetLastError();
		sprintf_s(*errmsg,2048, "bind()ʧ�� %d!", ret);
		ret = BES200_ERROR_CONNECT;
		goto out;
	}

	/*�㲥��������*/
	int len = sizeof(sockaddr);
	int res = sendto(m_socket, (char*)SendBuf, SendDataSize, 0, (sockaddr*)&m_addrto, len);
	if (res == SendDataSize)
	{
		RecvDataSize = recvfrom(m_socket, RecvBuf, RECVBUF_SIZE, 0, (sockaddr*)&m_addrfrom, &len);
		if (RecvDataSize == BEGP_RSP_HEAD_SIZE + BEGP_RSP_PACKETDATA_ECHO_SIZE + BEGP_CRC_SIZE)
		{//���ݳ�����ȷ
			//CRC16У��
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

/*����һ��ģ��*/
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//�������ݣ�ָ�븳ֵ
	pData_rep = (BEGP_REQ_PACKETDATA_ADD_MODULE_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_ADD_MODULE;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_ADD_MODULE_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
	memcpy(pData_rep->Name, pParameter->Name, sizeof(pData_rep->Name));//ģ������
	memcpy(pData_rep->Alias, pParameter->Alias, sizeof(pData_rep->Alias));//ģ�����
	memcpy(pData_rep->Description, pParameter->Description, sizeof(pData_rep->Description));//ģ���������Ϣ
	memcpy(pData_rep->Location, pParameter->Location, sizeof(pData_rep->Location));//ģ��İ�װλ��
	pData_rep->ModelID = pParameter->ModelID; //ģ���ͺű���
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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}
/*����ģ����������ò���*/
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//�������ݣ�ָ�븳ֵ
	pData_rep = (BEGP_REQ_PACKETDATA_SET_MODULE_PARAMETER_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_MODULE_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_MODULE_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
	memcpy(pData_rep->Name, pParameter->Name, sizeof(pData_rep->Name));//ģ������
	memcpy(pData_rep->Alias, pParameter->Alias, sizeof(pData_rep->Alias));//ģ�����
	memcpy(pData_rep->Description, pParameter->Description, sizeof(pData_rep->Description));//ģ���������Ϣ
	memcpy(pData_rep->Location, pParameter->Location, sizeof(pData_rep->Location));//ģ��İ�װλ��
	pData_rep->ModelID = pParameter->ModelID; //ģ���ͺű���
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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}
/*ɾ��һ��ģ�飬��ɾ����ģ����صĵ�*/
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//�������ݣ�ָ�븳ֵ
	pData_rep = (BEGP_REQ_PACKETDATA_DELETE_MODULE_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_DELETE_MODULE;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_DELETE_MODULE_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
	pData_rep->ID = BES200_htons(ModuleID);    //Ҫɾ����ģ��ĵ�ID��

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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}
/* ��ȡģ�������������Ϣ */
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//�������ݣ�ָ�븳ֵ
	pData_rep = (BEGP_REQ_PACKETDATA_GET_MODULE_PARAMETER_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_MODULE_PARAMETER_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_MODULE_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_GET_MODULE_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
	pData_rep->ID = BES200_htons(ModuleID);    //Ҫ��ȡģ������ĵ�ID��

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

			memcpy(pParameter->Name, pData_rsp->Name, sizeof(pParameter->Name));//ģ������
			memcpy(pParameter->Alias, pData_rsp->Alias, sizeof(pParameter->Alias));//ģ�����
			memcpy(pParameter->Description, pData_rsp->Description, sizeof(pParameter->Description));//ģ���������Ϣ
			memcpy(pParameter->Location, pData_rsp->Location, sizeof(pParameter->Location));//ģ��İ�װλ��
			pParameter->ModelID = pData_rsp->ModelID; //ģ���ͺű���
			pParameter->FlnID = pData_rsp->FlnID;//����
			pParameter->SlaveAddress = pData_rsp->SlaveAddress; //485ͨ�ŵ�ַ
			pParameter->Active = pData_rsp->Active;  //�Ƿ�����
			pParameter->ID = BES200_htons(pData_rsp->ID);  //ģ���ID��
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}
/*��ȡģ��Ĵ������*/
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//�������ݣ�ָ�븳ֵ
	pData_rep = (BEGP_REQ_PACKETDATA_GET_MODULE_ERROR_CODE_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_MODULE_ERROR_CODE_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_MODULE_ERROR_CODE;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_GET_MODULE_ERROR_CODE_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
	pData_rep->ID = BES200_htons(ModuleID);    //Ҫ��ȡģ������ĵ�ID��

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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}
/*���һ���߼���*/
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//�������ݣ�ָ�븳ֵ
	pData_req = (BEGP_REQ_PACKETDATA_ADD_LOGICAL_POINT_T*)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_ADD_LOGICAL_POINT;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_ADD_LOGICAL_POINT_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
	if (pParameter->PointType == POINT_TYPE_LAI) //ʵ��AI
	{
		BEGP_LOGICAL_POINT_PARAMETER_LAI_T        *pParameterLAI;
		BEGP_LOGICAL_POINT_PARAMETER_LAI_T        *pDataLAI_req;

		pDataLAI_req = (BEGP_LOGICAL_POINT_PARAMETER_LAI_T*)pData_req;
		pParameterLAI = (BEGP_LOGICAL_POINT_PARAMETER_LAI_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataLAI_req->ID = BES200_htons(LogicalPointID);  //�߼����ID
		pDataLAI_req->PointType = pParameterLAI->PointType;  //�������
		pDataLAI_req->Active = pParameterLAI->Active;//�Ƿ�����
		memcpy(pDataLAI_req->Name, pParameterLAI->Name, sizeof(pDataLAI_req->Name));  //����
		memcpy(pDataLAI_req->Alias, pParameterLAI->Alias, sizeof(pDataLAI_req->Alias)); //����
		memcpy(pDataLAI_req->Description, pParameterLAI->Description, sizeof(pDataLAI_req->Description)); //����
		pDataLAI_req->ModuleID = BES200_htons(pParameterLAI->ModuleID);  //������ģ��ID
		pDataLAI_req->ChannelIndex = pParameterLAI->ChannelIndex;//�����ڵ�ͨ������
		pDataLAI_req->WorkMode = pParameterLAI->WorkMode; //����ģʽ
		pDataLAI_req->Polarity = pParameterLAI->Polarity; //�Ƿ���
		pDataLAI_req->InitValue = BES200_htonl(pParameterLAI->InitValue);//ֵ

		pDataLAI_req->AlarmActive = pParameterLAI->AlarmActive;//�����Ƿ�����
		pDataLAI_req->AlarmType = pParameterLAI->AlarmType; //��������
		pDataLAI_req->AlarmPriority = pParameterLAI->AlarmPriority;//�������ȼ�

		pDataLAI_req->LineType = pParameterLAI->LineType;//ģ������������
		memcpy(pDataLAI_req->Unit, pParameterLAI->Unit, sizeof(pDataLAI_req->Unit)); //���̵�λ
		pDataLAI_req->HighRange = BES200_htonl(pParameterLAI->HighRange);//32λ���ݴ�С��ת��
		pDataLAI_req->LowRange = BES200_htonl(pParameterLAI->LowRange);//32λ���ݴ�С��ת��
		pDataLAI_req->Precision = pParameterLAI->Precision;//����
		pDataLAI_req->AlarmHighValue = BES200_htonl(pParameterLAI->AlarmHighValue);//32λ���ݴ�С��ת��
		pDataLAI_req->AlarmLowValue = BES200_htonl(pParameterLAI->AlarmLowValue);//32λ���ݴ�С��ת��
	}
	//����������������
	else if (pParameter->PointType == POINT_TYPE_LAO) //ʵ��AO
	{
		BEGP_LOGICAL_POINT_PARAMETER_LAO_T        *pParameterLAO;
		BEGP_LOGICAL_POINT_PARAMETER_LAO_T        *pDataLAO_req;

		pDataLAO_req = (BEGP_LOGICAL_POINT_PARAMETER_LAO_T*)pData_req;
		pParameterLAO = (BEGP_LOGICAL_POINT_PARAMETER_LAO_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataLAO_req->ID = BES200_htons(LogicalPointID);  //�߼����ID
		pDataLAO_req->PointType = pParameterLAO->PointType;  //�������
		pDataLAO_req->Active = pParameterLAO->Active;//�Ƿ�����
		memcpy(pDataLAO_req->Name, pParameterLAO->Name, sizeof(pDataLAO_req->Name));  //����
		memcpy(pDataLAO_req->Alias, pParameterLAO->Alias, sizeof(pDataLAO_req->Alias)); //����
		memcpy(pDataLAO_req->Description, pParameterLAO->Description, sizeof(pDataLAO_req->Description)); //����
		pDataLAO_req->ModuleID = BES200_htons(pParameterLAO->ModuleID);  //������ģ��ID
		pDataLAO_req->ChannelIndex = pParameterLAO->ChannelIndex;//�����ڵ�ͨ������
		pDataLAO_req->WorkMode = pParameterLAO->WorkMode; //����ģʽ
		pDataLAO_req->Polarity = pParameterLAO->Polarity; //�Ƿ���
		pDataLAO_req->InitValue = BES200_htonl(pParameterLAO->InitValue); //��ʼֵ

		pDataLAO_req->AlarmActive = pParameterLAO->AlarmActive;//�����Ƿ�����
		pDataLAO_req->AlarmType = pParameterLAO->AlarmType; //��������
		pDataLAO_req->AlarmPriority = pParameterLAO->AlarmPriority;//�������ȼ�

		pDataLAO_req->LineType = pParameterLAO->LineType;//ģ�����������
		memcpy(pDataLAO_req->Unit, pParameterLAO->Unit, sizeof(pDataLAO_req->Unit)); //���̵�λ
		pDataLAO_req->HighRange = BES200_htonl(pParameterLAO->HighRange);//32λ���ݴ�С��ת��
		pDataLAO_req->LowRange = BES200_htonl(pParameterLAO->LowRange);//32λ���ݴ�С��ת��
		pDataLAO_req->Precision = pParameterLAO->Precision;//����
		pDataLAO_req->AlarmHighValue = BES200_htonl(pParameterLAO->AlarmHighValue);//32λ���ݴ�С��ת��
		pDataLAO_req->AlarmLowValue = BES200_htonl(pParameterLAO->AlarmLowValue);//32λ���ݴ�С��ת��
	}
	else if (pParameter->PointType == POINT_TYPE_LDI) //ʵ��DI
	{
		BEGP_LOGICAL_POINT_PARAMETER_LDI_T        *pParameterLDI;
		BEGP_LOGICAL_POINT_PARAMETER_LDI_T        *pDataLDI_req;

		pDataLDI_req = (BEGP_LOGICAL_POINT_PARAMETER_LDI_T*)pData_req;
		pParameterLDI = (BEGP_LOGICAL_POINT_PARAMETER_LDI_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataLDI_req->ID = BES200_htons(LogicalPointID);  //�߼����ID
		pDataLDI_req->PointType = pParameterLDI->PointType;  //�������
		pDataLDI_req->Active = pParameterLDI->Active;//�Ƿ�����
		memcpy(pDataLDI_req->Name, pParameterLDI->Name, sizeof(pDataLDI_req->Name));  //����
		memcpy(pDataLDI_req->Alias, pParameterLDI->Alias, sizeof(pDataLDI_req->Alias)); //����
		memcpy(pDataLDI_req->Description, pParameterLDI->Description, sizeof(pDataLDI_req->Description)); //����
		pDataLDI_req->ModuleID = BES200_htons(pParameterLDI->ModuleID);  //������ģ��ID
		pDataLDI_req->ChannelIndex = pParameterLDI->ChannelIndex;//�����ڵ�ͨ������
		pDataLDI_req->WorkMode = pParameterLDI->WorkMode; //����ģʽ
		pDataLDI_req->Polarity = pParameterLDI->Polarity; //�Ƿ���
		pDataLDI_req->InitValue = BES200_htonl(pParameterLDI->InitValue);//ֵ

		pDataLDI_req->AlarmActive = pParameterLDI->AlarmActive;//�����Ƿ�����
		pDataLDI_req->AlarmType = pParameterLDI->AlarmType; //��������
		pDataLDI_req->AlarmPriority = pParameterLDI->AlarmPriority;//�������ȼ�

		pDataLDI_req->AlarmTrigger = pParameterLDI->AlarmTrigger;//��������
		pDataLDI_req->ActivePassive = pParameterLDI->ActivePassive;//��Դ��Դ
	}
	else if (pParameter->PointType == POINT_TYPE_LDO) //ʵ��DO
	{
		BEGP_LOGICAL_POINT_PARAMETER_LDO_T        *pParameterLDO;
		BEGP_LOGICAL_POINT_PARAMETER_LDO_T        *pDataLDO_req;

		pDataLDO_req = (BEGP_LOGICAL_POINT_PARAMETER_LDO_T*)pData_req;
		pParameterLDO = (BEGP_LOGICAL_POINT_PARAMETER_LDO_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataLDO_req->ID = BES200_htons(LogicalPointID);  //�߼����ID
		pDataLDO_req->PointType = pParameterLDO->PointType;  //�������
		pDataLDO_req->Active = pParameterLDO->Active;//�Ƿ�����
		memcpy(pDataLDO_req->Name, pParameterLDO->Name, sizeof(pDataLDO_req->Name));  //����
		memcpy(pDataLDO_req->Alias, pParameterLDO->Alias, sizeof(pDataLDO_req->Alias)); //����
		memcpy(pDataLDO_req->Description, pParameterLDO->Description, sizeof(pDataLDO_req->Description)); //����
		pDataLDO_req->ModuleID = BES200_htons(pParameterLDO->ModuleID);  //������ģ��ID
		pDataLDO_req->ChannelIndex = pParameterLDO->ChannelIndex;//�����ڵ�ͨ������
		pDataLDO_req->WorkMode = pParameterLDO->WorkMode; //����ģʽ
		pDataLDO_req->Polarity = pParameterLDO->Polarity; //�Ƿ���
		pDataLDO_req->InitValue = BES200_htonl(pParameterLDO->InitValue); //�Ƿ���

		pDataLDO_req->AlarmActive = pParameterLDO->AlarmActive;//�����Ƿ�����
		pDataLDO_req->AlarmType = pParameterLDO->AlarmType; //��������
		pDataLDO_req->AlarmPriority = pParameterLDO->AlarmPriority;//�������ȼ�

		pDataLDO_req->AlarmTrigger = pParameterLDO->AlarmTrigger;//��������
	}
	else if (pParameter->PointType == POINT_TYPE_VIRTUAL_AI) //���AI
	{
		BEGP_LOGICAL_POINT_PARAMETER_VAI_T        *pParameterVAI;
		BEGP_LOGICAL_POINT_PARAMETER_VAI_T        *pDataVAI_req;

		pDataVAI_req = (BEGP_LOGICAL_POINT_PARAMETER_VAI_T*)pData_req;
		pParameterVAI = (BEGP_LOGICAL_POINT_PARAMETER_VAI_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataVAI_req->ID = BES200_htons(LogicalPointID);  //�߼����ID
		pDataVAI_req->PointType = pParameterVAI->PointType;  //�������
		pDataVAI_req->Active = pParameterVAI->Active;//�Ƿ�����
		memcpy(pDataVAI_req->Name, pParameterVAI->Name, sizeof(pDataVAI_req->Name));  //����
		memcpy(pDataVAI_req->Alias, pParameterVAI->Alias, sizeof(pDataVAI_req->Alias)); //����
		memcpy(pDataVAI_req->Description, pParameterVAI->Description, sizeof(pDataVAI_req->Description)); //����
		pDataVAI_req->InitValue = BES200_htonl(pParameterVAI->InitValue);  //��ʼֵ
		memcpy(pDataVAI_req->Unit, pParameterVAI->Unit, sizeof(pDataVAI_req->Unit)); //��λ
		pDataVAI_req->Precision = pParameterVAI->Precision;  //����

		/*�������*/
		pDataVAI_req->AlarmActive = pParameterVAI->AlarmActive;//�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		pDataVAI_req->AlarmType = pParameterVAI->AlarmType;//�������ͣ�����������׼��������ǿ����
		pDataVAI_req->AlarmPriority = pParameterVAI->AlarmPriority;//�������ȼ�
		pDataVAI_req->AlarmHighValue = pParameterVAI->AlarmHighValue;
		pDataVAI_req->AlarmLowValue = pParameterVAI->AlarmLowValue;
	}
	else if (pParameter->PointType == POINT_TYPE_VIRTUAL_AO) //���AO
	{
		BEGP_LOGICAL_POINT_PARAMETER_VAO_T        *pParameterVAO;
		BEGP_LOGICAL_POINT_PARAMETER_VAO_T        *pDataVAO_req;

		pDataVAO_req = (BEGP_LOGICAL_POINT_PARAMETER_VAO_T*)pData_req;
		pParameterVAO = (BEGP_LOGICAL_POINT_PARAMETER_VAO_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataVAO_req->ID = BES200_htons(LogicalPointID);  //�߼����ID
		pDataVAO_req->PointType = pParameterVAO->PointType;  //�������
		pDataVAO_req->Active = pParameterVAO->Active;//�Ƿ�����
		memcpy(pDataVAO_req->Name, pParameterVAO->Name, sizeof(pDataVAO_req->Name));  //����
		memcpy(pDataVAO_req->Alias, pParameterVAO->Alias, sizeof(pDataVAO_req->Alias)); //����
		memcpy(pDataVAO_req->Description, pParameterVAO->Description, sizeof(pDataVAO_req->Description)); //����
		pDataVAO_req->InitValue = BES200_htonl(pParameterVAO->InitValue);  //��ʼֵ
		memcpy(pDataVAO_req->Unit, pParameterVAO->Unit, sizeof(pDataVAO_req->Unit)); //��λ
		pDataVAO_req->Precision = pParameterVAO->Precision;  //����

		/*�������*/
		pDataVAO_req->AlarmActive = pParameterVAO->AlarmActive;//�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		pDataVAO_req->AlarmType = pParameterVAO->AlarmType;//�������ͣ�����������׼��������ǿ����
		pDataVAO_req->AlarmPriority = pParameterVAO->AlarmPriority;//�������ȼ�
		pDataVAO_req->AlarmHighValue = pParameterVAO->AlarmHighValue;
		pDataVAO_req->AlarmLowValue = pParameterVAO->AlarmLowValue;
	}
	else if (pParameter->PointType == POINT_TYPE_VIRTUAL_DI) //���DI
	{
		BEGP_LOGICAL_POINT_PARAMETER_VDI_T        *pParameterVDI;
		BEGP_LOGICAL_POINT_PARAMETER_VDI_T        *pDataVDI_req;

		pDataVDI_req = (BEGP_LOGICAL_POINT_PARAMETER_VDI_T*)pData_req;
		pParameterVDI = (BEGP_LOGICAL_POINT_PARAMETER_VDI_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataVDI_req->ID = BES200_htons(LogicalPointID);  //�߼����ID
		pDataVDI_req->PointType = pParameterVDI->PointType;  //�������
		pDataVDI_req->Active = pParameterVDI->Active;//�Ƿ�����
		memcpy(pDataVDI_req->Name, pParameterVDI->Name, sizeof(pDataVDI_req->Name));  //����
		memcpy(pDataVDI_req->Alias, pParameterVDI->Alias, sizeof(pDataVDI_req->Alias)); //����
		memcpy(pDataVDI_req->Description, pParameterVDI->Description, sizeof(pDataVDI_req->Description)); //����
		pDataVDI_req->InitValue = BES200_htonl(pParameterVDI->InitValue);  //��ʼֵ

		/*�������*/
		pDataVDI_req->AlarmActive = pParameterVDI->AlarmActive;//�����Ƿ�����
		pDataVDI_req->AlarmType = pParameterVDI->AlarmType; //��������
		pDataVDI_req->AlarmPriority = pParameterVDI->AlarmPriority;//�������ȼ�

		pDataVDI_req->AlarmTrigger = pParameterVDI->AlarmTrigger;//��������
	}
	else if (pParameter->PointType == POINT_TYPE_VIRTUAL_DO) //���DO
	{
		BEGP_LOGICAL_POINT_PARAMETER_VDO_T        *pParameterVDO;
		BEGP_LOGICAL_POINT_PARAMETER_VDO_T        *pDataVDO_req;

		pDataVDO_req = (BEGP_LOGICAL_POINT_PARAMETER_VDO_T*)pData_req;
		pParameterVDO = (BEGP_LOGICAL_POINT_PARAMETER_VDO_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataVDO_req->ID = BES200_htons(LogicalPointID);  //�߼����ID
		pDataVDO_req->PointType = pParameterVDO->PointType;  //�������
		pDataVDO_req->Active = pParameterVDO->Active;//�Ƿ�����
		memcpy(pDataVDO_req->Name, pParameterVDO->Name, sizeof(pDataVDO_req->Name));  //����
		memcpy(pDataVDO_req->Alias, pParameterVDO->Alias, sizeof(pDataVDO_req->Alias)); //����
		memcpy(pDataVDO_req->Description, pParameterVDO->Description, sizeof(pDataVDO_req->Description)); //����
		pDataVDO_req->InitValue = BES200_htonl(pParameterVDO->InitValue);  //��ʼֵ

		/*�������*/
		pDataVDO_req->AlarmActive = pParameterVDO->AlarmActive;//�����Ƿ�����
		pDataVDO_req->AlarmType = pParameterVDO->AlarmType; //��������
		pDataVDO_req->AlarmPriority = pParameterVDO->AlarmPriority;//�������ȼ�

		pDataVDO_req->AlarmTrigger = pParameterVDO->AlarmTrigger;//��������
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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}

/*����һ���߼�������в���*/
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//�������ݣ�ָ�븳ֵ
	pData_req = (BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_T *)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_LOGICAL_POINT_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
	if (pParameter->PointType == POINT_TYPE_LAI) //ʵ��AI
	{
		BEGP_LOGICAL_POINT_PARAMETER_LAI_T        *pParameterLAI;
		BEGP_LOGICAL_POINT_PARAMETER_LAI_T        *pDataLAI_req;

		pDataLAI_req = (BEGP_LOGICAL_POINT_PARAMETER_LAI_T*)pData_req;
		pParameterLAI = (BEGP_LOGICAL_POINT_PARAMETER_LAI_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataLAI_req->PointType = pParameterLAI->PointType;  //�������
		pDataLAI_req->ID = BES200_htons(LogicalPointID);  //�߼����ID
		pDataLAI_req->Active = pParameterLAI->Active;//�Ƿ�����
		memcpy(pDataLAI_req->Name, pParameterLAI->Name, sizeof(pDataLAI_req->Name));  //����
		memcpy(pDataLAI_req->Alias, pParameterLAI->Alias, sizeof(pDataLAI_req->Alias)); //����
		memcpy(pDataLAI_req->Description, pParameterLAI->Description, sizeof(pDataLAI_req->Description)); //����
		pDataLAI_req->ModuleID = BES200_htons(pParameterLAI->ModuleID);  //������ģ��ID
		pDataLAI_req->ChannelIndex = pParameterLAI->ChannelIndex;//�����ڵ�ͨ������
		pDataLAI_req->WorkMode = pParameterLAI->WorkMode; //����ģʽ
		pDataLAI_req->Polarity = pParameterLAI->Polarity; //�Ƿ���
		pDataLAI_req->InitValue = BES200_htonl(pParameterLAI->InitValue);//ֵ

		pDataLAI_req->AlarmActive = pParameterLAI->AlarmActive;//�����Ƿ�����
		pDataLAI_req->AlarmType = pParameterLAI->AlarmType; //��������
		pDataLAI_req->AlarmPriority = pParameterLAI->AlarmPriority;//�������ȼ�

		pDataLAI_req->LineType = pParameterLAI->LineType;//ģ������������
		memcpy(pDataLAI_req->Unit, pParameterLAI->Unit, sizeof(pDataLAI_req->Unit)); //���̵�λ
		pDataLAI_req->HighRange = BES200_htonl(pParameterLAI->HighRange);//32λ���ݴ�С��ת��
		pDataLAI_req->LowRange = BES200_htonl(pParameterLAI->LowRange);//32λ���ݴ�С��ת��
		pDataLAI_req->Precision = pParameterLAI->Precision;//����
		pDataLAI_req->AlarmHighValue = BES200_htonl(pParameterLAI->AlarmHighValue);//32λ���ݴ�С��ת��
		pDataLAI_req->AlarmLowValue = BES200_htonl(pParameterLAI->AlarmLowValue);//32λ���ݴ�С��ת��
	}
	//����������������
	else if (pParameter->PointType == POINT_TYPE_LAO) //ʵ��AO
	{
		BEGP_LOGICAL_POINT_PARAMETER_LAO_T        *pParameterLAO;
		BEGP_LOGICAL_POINT_PARAMETER_LAO_T        *pDataLAO_req;

		pDataLAO_req = (BEGP_LOGICAL_POINT_PARAMETER_LAO_T*)pData_req;
		pParameterLAO = (BEGP_LOGICAL_POINT_PARAMETER_LAO_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataLAO_req->PointType = pParameterLAO->PointType;  //�������
		pDataLAO_req->ID = BES200_htons(LogicalPointID);  //�߼����ID
		pDataLAO_req->Active = pParameterLAO->Active;//�Ƿ�����
		memcpy(pDataLAO_req->Name, pParameterLAO->Name, sizeof(pDataLAO_req->Name));  //����
		memcpy(pDataLAO_req->Alias, pParameterLAO->Alias, sizeof(pDataLAO_req->Alias)); //����
		memcpy(pDataLAO_req->Description, pParameterLAO->Description, sizeof(pDataLAO_req->Description)); //����
		pDataLAO_req->ModuleID = BES200_htons(pParameterLAO->ModuleID);  //������ģ��ID
		pDataLAO_req->ChannelIndex = pParameterLAO->ChannelIndex;//�����ڵ�ͨ������
		pDataLAO_req->WorkMode = pParameterLAO->WorkMode; //����ģʽ
		pDataLAO_req->Polarity = pParameterLAO->Polarity; //�Ƿ���
		pDataLAO_req->InitValue = BES200_htonl(pParameterLAO->InitValue); //��ʼֵ

		pDataLAO_req->AlarmActive = pParameterLAO->AlarmActive;//�����Ƿ�����
		pDataLAO_req->AlarmType = pParameterLAO->AlarmType; //��������
		pDataLAO_req->AlarmPriority = pParameterLAO->AlarmPriority;//�������ȼ�

		pDataLAO_req->LineType = pParameterLAO->LineType;//ģ�����������
		memcpy(pDataLAO_req->Unit, pParameterLAO->Unit, sizeof(pDataLAO_req->Unit)); //���̵�λ
		pDataLAO_req->HighRange = BES200_htonl(pParameterLAO->HighRange);//32λ���ݴ�С��ת��
		pDataLAO_req->LowRange = BES200_htonl(pParameterLAO->LowRange);//32λ���ݴ�С��ת��
		pDataLAO_req->Precision = pParameterLAO->Precision;//����
		pDataLAO_req->AlarmHighValue = BES200_htonl(pParameterLAO->AlarmHighValue);//32λ���ݴ�С��ת��
		pDataLAO_req->AlarmLowValue = BES200_htonl(pParameterLAO->AlarmLowValue);//32λ���ݴ�С��ת��
	}
	else if (pParameter->PointType == POINT_TYPE_LDI) //ʵ��DI
	{
		BEGP_LOGICAL_POINT_PARAMETER_LDI_T        *pParameterLDI;
		BEGP_LOGICAL_POINT_PARAMETER_LDI_T        *pDataLDI_req;

		pDataLDI_req = (BEGP_LOGICAL_POINT_PARAMETER_LDI_T*)pData_req;
		pParameterLDI = (BEGP_LOGICAL_POINT_PARAMETER_LDI_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataLDI_req->PointType = pParameterLDI->PointType;  //�������
		pDataLDI_req->ID = BES200_htons(LogicalPointID);  //�߼����ID
		pDataLDI_req->Active = pParameterLDI->Active;//�Ƿ�����
		memcpy(pDataLDI_req->Name, pParameterLDI->Name, sizeof(pDataLDI_req->Name));  //����
		memcpy(pDataLDI_req->Alias, pParameterLDI->Alias, sizeof(pDataLDI_req->Alias)); //����
		memcpy(pDataLDI_req->Description, pParameterLDI->Description, sizeof(pDataLDI_req->Description)); //����
		pDataLDI_req->ModuleID = BES200_htons(pParameterLDI->ModuleID);  //������ģ��ID
		pDataLDI_req->ChannelIndex = pParameterLDI->ChannelIndex;//�����ڵ�ͨ������
		pDataLDI_req->WorkMode = pParameterLDI->WorkMode; //����ģʽ
		pDataLDI_req->Polarity = pParameterLDI->Polarity; //�Ƿ���
		pDataLDI_req->InitValue = BES200_htonl(pParameterLDI->InitValue);//ֵ

		pDataLDI_req->AlarmActive = pParameterLDI->AlarmActive;//�����Ƿ�����
		pDataLDI_req->AlarmType = pParameterLDI->AlarmType; //��������
		pDataLDI_req->AlarmPriority = pParameterLDI->AlarmPriority;//�������ȼ�

		pDataLDI_req->AlarmTrigger = pParameterLDI->AlarmTrigger;//��������
		pDataLDI_req->ActivePassive = pParameterLDI->ActivePassive;//��Դ��Դ
	}
	else if (pParameter->PointType == POINT_TYPE_LDO) //ʵ��DO
	{
		BEGP_LOGICAL_POINT_PARAMETER_LDO_T        *pParameterLDO;
		BEGP_LOGICAL_POINT_PARAMETER_LDO_T        *pDataLDO_req;

		pDataLDO_req = (BEGP_LOGICAL_POINT_PARAMETER_LDO_T*)pData_req;
		pParameterLDO = (BEGP_LOGICAL_POINT_PARAMETER_LDO_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataLDO_req->PointType = pParameterLDO->PointType;  //�������
		pDataLDO_req->ID = BES200_htons(LogicalPointID);  //�߼����ID
		pDataLDO_req->Active = pParameterLDO->Active;//�Ƿ�����
		memcpy(pDataLDO_req->Name, pParameterLDO->Name, sizeof(pDataLDO_req->Name));  //����
		memcpy(pDataLDO_req->Alias, pParameterLDO->Alias, sizeof(pDataLDO_req->Alias)); //����
		memcpy(pDataLDO_req->Description, pParameterLDO->Description, sizeof(pDataLDO_req->Description)); //����
		pDataLDO_req->ModuleID = BES200_htons(pParameterLDO->ModuleID);  //������ģ��ID
		pDataLDO_req->ChannelIndex = pParameterLDO->ChannelIndex;//�����ڵ�ͨ������
		pDataLDO_req->WorkMode = pParameterLDO->WorkMode; //����ģʽ
		pDataLDO_req->Polarity = pParameterLDO->Polarity; //�Ƿ���
		pDataLDO_req->InitValue = BES200_htonl(pParameterLDO->InitValue); //��ʼֵ

		pDataLDO_req->AlarmActive = pParameterLDO->AlarmActive;//�����Ƿ�����
		pDataLDO_req->AlarmType = pParameterLDO->AlarmType; //��������
		pDataLDO_req->AlarmPriority = pParameterLDO->AlarmPriority;//�������ȼ�

		pDataLDO_req->AlarmTrigger = pParameterLDO->AlarmTrigger;//��������
	}
	else if (pParameter->PointType == POINT_TYPE_VIRTUAL_AI) //���AI
	{
		BEGP_LOGICAL_POINT_PARAMETER_VAI_T        *pParameterVAI;
		BEGP_LOGICAL_POINT_PARAMETER_VAI_T        *pDataVAI_req;

		pDataVAI_req = (BEGP_LOGICAL_POINT_PARAMETER_VAI_T*)pData_req;
		pParameterVAI = (BEGP_LOGICAL_POINT_PARAMETER_VAI_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataVAI_req->PointType = pParameterVAI->PointType;  //�������
		pDataVAI_req->ID = BES200_htons(LogicalPointID);  //�߼����ID
		pDataVAI_req->Active = pParameterVAI->Active;//�Ƿ�����
		memcpy(pDataVAI_req->Name, pParameterVAI->Name, sizeof(pDataVAI_req->Name));  //����
		memcpy(pDataVAI_req->Alias, pParameterVAI->Alias, sizeof(pDataVAI_req->Alias)); //����
		memcpy(pDataVAI_req->Description, pParameterVAI->Description, sizeof(pDataVAI_req->Description)); //����
		pDataVAI_req->InitValue = BES200_htonl(pParameterVAI->InitValue);  //��ʼֵ
		memcpy(pDataVAI_req->Unit, pParameterVAI->Unit, sizeof(pDataVAI_req->Unit)); //��λ
		pDataVAI_req->Precision = pParameterVAI->Precision;  //����

		/*�������*/
		pDataVAI_req->AlarmActive = pParameterVAI->AlarmActive;//�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		pDataVAI_req->AlarmType = pParameterVAI->AlarmType;//�������ͣ�����������׼��������ǿ����
		pDataVAI_req->AlarmPriority = pParameterVAI->AlarmPriority;//�������ȼ�
		pDataVAI_req->AlarmHighValue = pParameterVAI->AlarmHighValue;
		pDataVAI_req->AlarmLowValue = pParameterVAI->AlarmLowValue;
	}
	else if (pParameter->PointType == POINT_TYPE_VIRTUAL_AO) //���AO
	{
		BEGP_LOGICAL_POINT_PARAMETER_VAO_T        *pParameterVAO;
		BEGP_LOGICAL_POINT_PARAMETER_VAO_T        *pDataVAO_req;

		pDataVAO_req = (BEGP_LOGICAL_POINT_PARAMETER_VAO_T*)pData_req;
		pParameterVAO = (BEGP_LOGICAL_POINT_PARAMETER_VAO_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataVAO_req->PointType = pParameterVAO->PointType;  //�������
		pDataVAO_req->ID = BES200_htons(LogicalPointID);  //�߼����ID
		pDataVAO_req->Active = pParameterVAO->Active;//�Ƿ�����
		memcpy(pDataVAO_req->Name, pParameterVAO->Name, sizeof(pDataVAO_req->Name));  //����
		memcpy(pDataVAO_req->Alias, pParameterVAO->Alias, sizeof(pDataVAO_req->Alias)); //����
		memcpy(pDataVAO_req->Description, pParameterVAO->Description, sizeof(pDataVAO_req->Description)); //����
		pDataVAO_req->InitValue = BES200_htonl(pParameterVAO->InitValue);  //��ʼֵ
		memcpy(pDataVAO_req->Unit, pParameterVAO->Unit, sizeof(pDataVAO_req->Unit)); //��λ
		pDataVAO_req->Precision = pParameterVAO->Precision;  //����

		/*�������*/
		pDataVAO_req->AlarmActive = pParameterVAO->AlarmActive;//�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
		pDataVAO_req->AlarmType = pParameterVAO->AlarmType;//�������ͣ�����������׼��������ǿ����
		pDataVAO_req->AlarmPriority = pParameterVAO->AlarmPriority;//�������ȼ�
		pDataVAO_req->AlarmHighValue = pParameterVAO->AlarmHighValue;
		pDataVAO_req->AlarmLowValue = pParameterVAO->AlarmLowValue;
	}
	else if (pParameter->PointType == POINT_TYPE_VIRTUAL_DI) //���DI
	{
		BEGP_LOGICAL_POINT_PARAMETER_VDI_T        *pParameterVDI;
		BEGP_LOGICAL_POINT_PARAMETER_VDI_T        *pDataVDI_req;

		pDataVDI_req = (BEGP_LOGICAL_POINT_PARAMETER_VDI_T*)pData_req;
		pParameterVDI = (BEGP_LOGICAL_POINT_PARAMETER_VDI_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataVDI_req->PointType = pParameterVDI->PointType;  //�������
		pDataVDI_req->ID = BES200_htons(LogicalPointID);  //�߼����ID
		pDataVDI_req->Active = pParameterVDI->Active;//�Ƿ�����
		memcpy(pDataVDI_req->Name, pParameterVDI->Name, sizeof(pDataVDI_req->Name));  //����
		memcpy(pDataVDI_req->Alias, pParameterVDI->Alias, sizeof(pDataVDI_req->Alias)); //����
		memcpy(pDataVDI_req->Description, pParameterVDI->Description, sizeof(pDataVDI_req->Description)); //����
		pDataVDI_req->InitValue = BES200_htonl(pParameterVDI->InitValue);  //��ʼֵ

		/*�������*/
		pDataVDI_req->AlarmActive = pParameterVDI->AlarmActive;//�����Ƿ�����
		pDataVDI_req->AlarmType = pParameterVDI->AlarmType; //��������
		pDataVDI_req->AlarmPriority = pParameterVDI->AlarmPriority;//�������ȼ�

		pDataVDI_req->AlarmTrigger = pParameterVDI->AlarmTrigger;//��������
	}
	else if (pParameter->PointType == POINT_TYPE_VIRTUAL_DO) //���DO
	{
		BEGP_LOGICAL_POINT_PARAMETER_VDO_T        *pParameterVDO;
		BEGP_LOGICAL_POINT_PARAMETER_VDO_T        *pDataVDO_req;

		pDataVDO_req = (BEGP_LOGICAL_POINT_PARAMETER_VDO_T*)pData_req;
		pParameterVDO = (BEGP_LOGICAL_POINT_PARAMETER_VDO_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataVDO_req->PointType = pParameterVDO->PointType;  //�������
		pDataVDO_req->ID = BES200_htons(LogicalPointID);  //�߼����ID
		pDataVDO_req->Active = pParameterVDO->Active;//�Ƿ�����
		memcpy(pDataVDO_req->Name, pParameterVDO->Name, sizeof(pDataVDO_req->Name));  //����
		memcpy(pDataVDO_req->Alias, pParameterVDO->Alias, sizeof(pDataVDO_req->Alias)); //����
		memcpy(pDataVDO_req->Description, pParameterVDO->Description, sizeof(pDataVDO_req->Description)); //����
		pDataVDO_req->InitValue = BES200_htonl(pParameterVDO->InitValue);  //��ʼֵ

		/*�������*/
		pDataVDO_req->AlarmActive = pParameterVDO->AlarmActive;//�����Ƿ�����
		pDataVDO_req->AlarmType = pParameterVDO->AlarmType; //��������
		pDataVDO_req->AlarmPriority = pParameterVDO->AlarmPriority;//�������ȼ�

		pDataVDO_req->AlarmTrigger = pParameterVDO->AlarmTrigger;//��������
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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}

/*����һ���߼����ֵ*/
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//�������ݣ�ָ�븳ֵ
	pData_req = (BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_T *)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_LOGICAL_POINT_VALUE;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}


/*����һ���߼����ֵ��ͨ������*/
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//�������ݣ�ָ�븳ֵ
	pData_req = (BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_BY_NAME_T *)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_LOGICAL_POINT_VALUE_BY_NAME;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_LOGICAL_POINT_VALUE_BY_NAME_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}


/*ɾ��һ���߼���*/
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//�������ݣ�ָ�븳ֵ
	pData_rep = (BEGP_REQ_PACKETDATA_DELETE_LOGICAL_POINT_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_DELETE_LOGICAL_POINT;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_DELETE_LOGICAL_POINT_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
	pData_rep->ID = BES200_htons(LogicalPointID);    //Ҫɾ�����ID��

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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}
/*��ȡһ���߼�����������ò���*/
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//�������ݣ�ָ�븳ֵ
	pData_rep = (BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_LOGICAL_POINT_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
	pData_rep->ID = BES200_htons(LogicalPointID);    //Ҫ��ȡ���ID��

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
			if (pData_rsp->PointType == POINT_TYPE_LAI) //ʵ��AI
			{
				BEGP_LOGICAL_POINT_PARAMETER_LAI_T        *pParameterLAI;
				BEGP_LOGICAL_POINT_PARAMETER_LAI_T        *ppData_rsp;

				ppData_rsp = (BEGP_LOGICAL_POINT_PARAMETER_LAI_T*)pData_rsp;
				pParameterLAI = (BEGP_LOGICAL_POINT_PARAMETER_LAI_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

				pParameterLAI->PointType = ppData_rsp->PointType;  //�������
				pParameterLAI->ID = BES200_htons(ppData_rsp->ID);  //�߼����ID
				pParameterLAI->Active = ppData_rsp->Active;//�Ƿ�����
				memcpy(pParameterLAI->Name, ppData_rsp->Name, sizeof(pParameterLAI->Name));//����
				memcpy(pParameterLAI->Alias, ppData_rsp->Alias, sizeof(pParameterLAI->Alias)); //����
				memcpy(pParameterLAI->Description, ppData_rsp->Description, sizeof(pParameterLAI->Description)); //����
				pParameterLAI->ModuleID = BES200_htons(ppData_rsp->ModuleID);  //������ģ��ID
				pParameterLAI->ChannelIndex = ppData_rsp->ChannelIndex;//�����ڵ�ͨ������
				pParameterLAI->WorkMode = ppData_rsp->WorkMode; //����ģʽ
				pParameterLAI->Polarity = ppData_rsp->Polarity; //�Ƿ���
				pParameterLAI->InitValue = BES200_htonl(ppData_rsp->InitValue); //ֵ

				pParameterLAI->AlarmActive = ppData_rsp->AlarmActive;//�����Ƿ�����
				pParameterLAI->AlarmType = ppData_rsp->AlarmType; //��������
				pParameterLAI->AlarmPriority = ppData_rsp->AlarmPriority;//�������ȼ�

				pParameterLAI->LineType = ppData_rsp->LineType;//ģ������������
				memcpy(pParameterLAI->Unit, ppData_rsp->Unit, sizeof(pParameterLAI->Unit)); //���̵�λ
				pParameterLAI->HighRange = BES200_htonl(ppData_rsp->HighRange);//32λ���ݴ�С��ת��
				pParameterLAI->LowRange = BES200_htonl(ppData_rsp->LowRange);//32λ���ݴ�С��ת��
				pParameterLAI->Precision = ppData_rsp->Precision;//����
				pParameterLAI->AlarmHighValue = BES200_htonl(ppData_rsp->AlarmHighValue);//32λ���ݴ�С��ת��
				pParameterLAI->AlarmLowValue = BES200_htonl(ppData_rsp->AlarmLowValue);//32λ���ݴ�С��ת��
			}
			else if (pData_rsp->PointType == POINT_TYPE_LAO) //ʵ��AO
			{
				BEGP_LOGICAL_POINT_PARAMETER_LAO_T        *pParameterLAO;
				BEGP_LOGICAL_POINT_PARAMETER_LAO_T        *pDataLAO_rsp;

				pDataLAO_rsp = (BEGP_LOGICAL_POINT_PARAMETER_LAO_T*)pData_rsp;
				pParameterLAO = (BEGP_LOGICAL_POINT_PARAMETER_LAO_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

				pParameterLAO->PointType = pDataLAO_rsp->PointType;  //�������
				pParameterLAO->ID = BES200_htons(pDataLAO_rsp->ID);  //�߼����ID
				pParameterLAO->Active = pDataLAO_rsp->Active;//�Ƿ�����
				memcpy(pParameterLAO->Name, pDataLAO_rsp->Name, sizeof(pParameterLAO->Name));//����
				memcpy(pParameterLAO->Alias, pDataLAO_rsp->Alias, sizeof(pParameterLAO->Alias)); //����
				memcpy(pParameterLAO->Description, pDataLAO_rsp->Description, sizeof(pParameterLAO->Description)); //����
				pParameterLAO->ModuleID = BES200_htons(pDataLAO_rsp->ModuleID);  //������ģ��ID
				pParameterLAO->ChannelIndex = pDataLAO_rsp->ChannelIndex;//�����ڵ�ͨ������
				pParameterLAO->WorkMode = pDataLAO_rsp->WorkMode; //����ģʽ
				pParameterLAO->Polarity = pDataLAO_rsp->Polarity; //�Ƿ���
				pParameterLAO->InitValue = BES200_htonl(pDataLAO_rsp->InitValue); //��ʼֵ

				pParameterLAO->AlarmActive = pDataLAO_rsp->AlarmActive;//�����Ƿ�����
				pParameterLAO->AlarmType = pDataLAO_rsp->AlarmType; //��������
				pParameterLAO->AlarmPriority = pDataLAO_rsp->AlarmPriority;//�������ȼ�

				pParameterLAO->LineType = pDataLAO_rsp->LineType;//ģ������������
				memcpy(pParameterLAO->Unit, pDataLAO_rsp->Unit, sizeof(pParameterLAO->Unit)); //���̵�λ
				pParameterLAO->HighRange = BES200_htonl(pDataLAO_rsp->HighRange);//32λ���ݴ�С��ת��
				pParameterLAO->LowRange = BES200_htonl(pDataLAO_rsp->LowRange);//32λ���ݴ�С��ת��
				pParameterLAO->Precision = pDataLAO_rsp->Precision;//����
				pParameterLAO->AlarmHighValue = BES200_htonl(pDataLAO_rsp->AlarmHighValue);//32λ���ݴ�С��ת��
				pParameterLAO->AlarmLowValue = BES200_htonl(pDataLAO_rsp->AlarmLowValue);//32λ���ݴ�С��ת��
			}
			else if (pData_rsp->PointType == POINT_TYPE_LDI) //ʵ��DI
			{
				BEGP_LOGICAL_POINT_PARAMETER_LDI_T        *pParameterLDI;
				BEGP_LOGICAL_POINT_PARAMETER_LDI_T        *pDataLDI_rsp;

				pDataLDI_rsp = (BEGP_LOGICAL_POINT_PARAMETER_LDI_T*)pData_rsp;
				pParameterLDI = (BEGP_LOGICAL_POINT_PARAMETER_LDI_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

				pParameterLDI->PointType = pDataLDI_rsp->PointType;  //�������
				pParameterLDI->ID = BES200_htons(pDataLDI_rsp->ID);  //�߼����ID
				pParameterLDI->Active = pDataLDI_rsp->Active;//�Ƿ�����
				memcpy(pParameterLDI->Name, pDataLDI_rsp->Name, sizeof(pParameterLDI->Name));  //����
				memcpy(pParameterLDI->Alias, pDataLDI_rsp->Alias, sizeof(pParameterLDI->Alias)); //����
				memcpy(pParameterLDI->Description, pDataLDI_rsp->Description, sizeof(pParameterLDI->Description)); //����
				pParameterLDI->ModuleID = BES200_htons(pDataLDI_rsp->ModuleID);  //������ģ��ID
				pParameterLDI->ChannelIndex = pDataLDI_rsp->ChannelIndex;//�����ڵ�ͨ������
				pParameterLDI->WorkMode = pDataLDI_rsp->WorkMode; //����ģʽ
				pParameterLDI->Polarity = pDataLDI_rsp->Polarity; //�Ƿ���
				pParameterLDI->InitValue = BES200_htonl(pDataLDI_rsp->InitValue); //ֵ

				pParameterLDI->AlarmActive = pDataLDI_rsp->AlarmActive;//�����Ƿ�����
				pParameterLDI->AlarmType = pDataLDI_rsp->AlarmType; //��������
				pParameterLDI->AlarmPriority = pDataLDI_rsp->AlarmPriority;//�������ȼ�

				pParameterLDI->AlarmTrigger = pDataLDI_rsp->AlarmTrigger;//��������
				pParameterLDI->ActivePassive = pDataLDI_rsp->ActivePassive;//��Դ��Դ
			}
			else if (pData_rsp->PointType == POINT_TYPE_LDO) //ʵ��DO
			{
				BEGP_LOGICAL_POINT_PARAMETER_LDO_T        *pParameterLDO;
				BEGP_LOGICAL_POINT_PARAMETER_LDO_T        *pDataLDO_rsp;

				pDataLDO_rsp = (BEGP_LOGICAL_POINT_PARAMETER_LDO_T*)pData_rsp;
				pParameterLDO = (BEGP_LOGICAL_POINT_PARAMETER_LDO_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

				pParameterLDO->PointType = pDataLDO_rsp->PointType;  //�������
				pParameterLDO->ID = BES200_htons(pDataLDO_rsp->ID);  //�߼����ID
				pParameterLDO->Active = pDataLDO_rsp->Active;//�Ƿ�����
				memcpy(pParameterLDO->Name, pDataLDO_rsp->Name, sizeof(pParameterLDO->Name));  //����
				memcpy(pParameterLDO->Alias, pDataLDO_rsp->Alias, sizeof(pParameterLDO->Alias)); //����
				memcpy(pParameterLDO->Description, pDataLDO_rsp->Description, sizeof(pParameterLDO->Description)); //����
				pParameterLDO->ModuleID = BES200_htons(pDataLDO_rsp->ModuleID);  //������ģ��ID
				pParameterLDO->ChannelIndex = pDataLDO_rsp->ChannelIndex;//�����ڵ�ͨ������
				pParameterLDO->WorkMode = pDataLDO_rsp->WorkMode; //����ģʽ
				pParameterLDO->Polarity = pDataLDO_rsp->Polarity; //�Ƿ���
				pParameterLDO->InitValue = BES200_htonl(pDataLDO_rsp->InitValue); //�Ƿ���

				pParameterLDO->AlarmActive = pDataLDO_rsp->AlarmActive;//�����Ƿ�����
				pParameterLDO->AlarmType = pDataLDO_rsp->AlarmType; //��������
				pParameterLDO->AlarmPriority = pDataLDO_rsp->AlarmPriority;//�������ȼ�

				pParameterLDO->AlarmTrigger = pDataLDO_rsp->AlarmTrigger;//��������
			}
			else if (pData_rsp->PointType == POINT_TYPE_VIRTUAL_AI) //���AI
			{
				BEGP_LOGICAL_POINT_PARAMETER_VAI_T        *pParameterVAI;
				BEGP_LOGICAL_POINT_PARAMETER_VAI_T        *pDataVAI_req;

				pDataVAI_req = (BEGP_LOGICAL_POINT_PARAMETER_VAI_T*)pData_rsp;
				pParameterVAI = (BEGP_LOGICAL_POINT_PARAMETER_VAI_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

				pParameterVAI->PointType = pDataVAI_req->PointType;  //�������
				pParameterVAI->ID = BES200_htons(pDataVAI_req->ID);  //�߼����ID
				pParameterVAI->Active = pDataVAI_req->Active;//�Ƿ�����
				memcpy(pParameterVAI->Name, pDataVAI_req->Name, sizeof(pParameterVAI->Name));  //����
				memcpy(pParameterVAI->Alias, pDataVAI_req->Alias, sizeof(pParameterVAI->Alias)); //����
				memcpy(pParameterVAI->Description, pDataVAI_req->Description, sizeof(pParameterVAI->Description)); //����
				pParameterVAI->InitValue = BES200_htonl(pDataVAI_req->InitValue);  //��ʼֵ
				memcpy(pParameterVAI->Unit, pDataVAI_req->Unit, sizeof(pParameterVAI->Unit)); //��λ
				pParameterVAI->Precision = pDataVAI_req->Precision;  //����

				/*�������*/
				pParameterVAI->AlarmActive = pDataVAI_req->AlarmActive;//�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
				pParameterVAI->AlarmType = pDataVAI_req->AlarmType;//�������ͣ�����������׼��������ǿ����
				pParameterVAI->AlarmPriority = pDataVAI_req->AlarmPriority;//�������ȼ�
				pParameterVAI->AlarmHighValue = pDataVAI_req->AlarmHighValue;
				pParameterVAI->AlarmLowValue = pDataVAI_req->AlarmLowValue;
			}
			else if (pData_rsp->PointType == POINT_TYPE_VIRTUAL_AO) //���AO
			{
				BEGP_LOGICAL_POINT_PARAMETER_VAO_T        *pParameterVAO;
				BEGP_LOGICAL_POINT_PARAMETER_VAO_T        *pDataVAO_req;

				pDataVAO_req = (BEGP_LOGICAL_POINT_PARAMETER_VAO_T*)pData_rsp;
				pParameterVAO = (BEGP_LOGICAL_POINT_PARAMETER_VAO_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

				pParameterVAO->PointType = pDataVAO_req->PointType;  //�������
				pParameterVAO->ID = BES200_htons(pDataVAO_req->ID);  //�߼����ID
				pParameterVAO->Active = pDataVAO_req->Active;//�Ƿ�����
				memcpy(pParameterVAO->Name, pDataVAO_req->Name, sizeof(pParameterVAO->Name));  //����
				memcpy(pParameterVAO->Alias, pDataVAO_req->Alias, sizeof(pParameterVAO->Alias)); //����
				memcpy(pParameterVAO->Description, pDataVAO_req->Description, sizeof(pParameterVAO->Description)); //����
				pParameterVAO->InitValue = BES200_htonl(pDataVAO_req->InitValue);  //��ʼֵ
				memcpy(pParameterVAO->Unit, pDataVAO_req->Unit, sizeof(pParameterVAO->Unit)); //��λ
				pParameterVAO->Precision = pDataVAO_req->Precision;  //����

				/*�������*/
				pParameterVAO->AlarmActive = pDataVAO_req->AlarmActive;//�����Ƿ����ã�ALARM_ACTIVE_ENABLEʹ�ܣ�ALARM_ACTIVE_DISABLE��ֹ
				pParameterVAO->AlarmType = pDataVAO_req->AlarmType;//�������ͣ�����������׼��������ǿ����
				pParameterVAO->AlarmPriority = pDataVAO_req->AlarmPriority;//�������ȼ�
				pParameterVAO->AlarmHighValue = pDataVAO_req->AlarmHighValue;
				pParameterVAO->AlarmLowValue = pDataVAO_req->AlarmLowValue;
			}
			else if (pData_rsp->PointType == POINT_TYPE_VIRTUAL_DI) //���DI
			{
				BEGP_LOGICAL_POINT_PARAMETER_VDI_T        *pParameterVDI;
				BEGP_LOGICAL_POINT_PARAMETER_VDI_T        *pDataVDI_req;

				pDataVDI_req = (BEGP_LOGICAL_POINT_PARAMETER_VDI_T*)pData_rsp;
				pParameterVDI = (BEGP_LOGICAL_POINT_PARAMETER_VDI_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

				pParameterVDI->PointType = pDataVDI_req->PointType;  //�������
				pParameterVDI->ID = BES200_htons(pDataVDI_req->ID);  //�߼����ID
				pParameterVDI->Active = pDataVDI_req->Active;//�Ƿ�����
				memcpy(pParameterVDI->Name, pDataVDI_req->Name, sizeof(pParameterVDI->Name));  //����
				memcpy(pParameterVDI->Alias, pDataVDI_req->Alias, sizeof(pParameterVDI->Alias)); //����
				memcpy(pParameterVDI->Description, pDataVDI_req->Description, sizeof(pParameterVDI->Description)); //����
				pParameterVDI->InitValue = BES200_htonl(pDataVDI_req->InitValue);  //��ʼֵ

				/*�������*/
				pParameterVDI->AlarmActive = pDataVDI_req->AlarmActive;//�����Ƿ�����
				pParameterVDI->AlarmType = pDataVDI_req->AlarmType; //��������
				pParameterVDI->AlarmPriority = pDataVDI_req->AlarmPriority;//�������ȼ�

				pParameterVDI->AlarmTrigger = pDataVDI_req->AlarmTrigger;//��������
			}
			else if (pData_rsp->PointType == POINT_TYPE_VIRTUAL_DO) //���DO
			{
				BEGP_LOGICAL_POINT_PARAMETER_VDO_T        *pParameterVDO;
				BEGP_LOGICAL_POINT_PARAMETER_VDO_T        *pDataVDO_req;

				pDataVDO_req = (BEGP_LOGICAL_POINT_PARAMETER_VDO_T*)pData_rsp;
				pParameterVDO = (BEGP_LOGICAL_POINT_PARAMETER_VDO_T *)pParameter;//�ֽ�˳����Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

				pParameterVDO->PointType = pDataVDO_req->PointType;  //�������
				pParameterVDO->ID = BES200_htons(pDataVDO_req->ID);  //�߼����ID
				pParameterVDO->Active = pDataVDO_req->Active;//�Ƿ�����
				memcpy(pParameterVDO->Name, pDataVDO_req->Name, sizeof(pParameterVDO->Name));  //����
				memcpy(pParameterVDO->Alias, pDataVDO_req->Alias, sizeof(pParameterVDO->Alias)); //����
				memcpy(pParameterVDO->Description, pDataVDO_req->Description, sizeof(pParameterVDO->Description)); //����
				pParameterVDO->InitValue = BES200_htonl(pDataVDO_req->InitValue);  //��ʼֵ

				/*�������*/
				pParameterVDO->AlarmActive = pDataVDO_req->AlarmActive;//�����Ƿ�����
				pParameterVDO->AlarmType = pDataVDO_req->AlarmType; //��������
				pParameterVDO->AlarmPriority = pDataVDO_req->AlarmPriority;//�������ȼ�

				pParameterVDO->AlarmTrigger = pDataVDO_req->AlarmTrigger;//��������
			}
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}
/*��ȡһ���߼���ı�����Ϣ*/
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//�������ݣ�ָ�븳ֵ
	pData_rep = (BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_LOGICAL_POINT_ALARM_INFOR;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_GET_LOGICAL_POINT_ALARM_INFOR_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
	pData_rep->ID = BES200_htons(LogicalPointID);    //Ҫ��ȡ���ID��

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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}

/* ���һ���ƻ� */
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//�������ݣ�ָ�븳ֵ
	pData_rep = (BEGP_REQ_PACKETDATA_ADD_SCHEDULER_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_ADD_SCHEDULER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_ADD_SCHEDULER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
	pData_rep->ID = BES200_htons(SchedulerID);//�ƻ���ID��
	pData_rep->Active = pParameter->Active;//�ƻ��Ƿ�����
	memcpy(pData_rep->Name, pParameter->Name, sizeof(pData_rep->Name));//�ƻ�������
	memcpy(pData_rep->Alias, pParameter->Alias, sizeof(pData_rep->Alias));//�ƻ��ı���
	pData_rep->PlanType = pParameter->PlanType;//�ƻ�������

	pData_rep->StartDateYear = pParameter->StartDateYear;//�ƻ��Ŀ�ʼ���� ��
	pData_rep->StartDateMonth = pParameter->StartDateMonth;//�ƻ��Ŀ�ʼ���� ��
	pData_rep->StartDateDay = pParameter->StartDateDay;//�ƻ��Ŀ�ʼ����  ��
	pData_rep->EndDateYear = pParameter->EndDateYear;//�ƻ��Ľ������� ��
	pData_rep->EndDateMonth = pParameter->EndDateMonth;//�ƻ��Ľ������� ��
	pData_rep->EndDateDay = pParameter->EndDateDay;//�ƻ��Ľ�������  ��

	pData_rep->StartTimeHour = pParameter->StartTimeHour;//�ƻ��Ŀ�ʼʱ��  ʱ
	pData_rep->StartTimeMinute = pParameter->StartTimeMinute;//�ƻ��Ŀ�ʼʱ��  ��
	pData_rep->StartTimeSecond = pParameter->StartTimeSecond;//�ƻ��Ŀ�ʼʱ��  ��
	pData_rep->EndTimeHour = pParameter->EndTimeHour;//�ƻ��Ľ���ʱ��  ʱ
	pData_rep->EndTimeMinute = pParameter->EndTimeMinute;//�ƻ��Ľ���ʱ��  ��
	pData_rep->EndTimeSecond = pParameter->EndTimeSecond;//�ƻ��Ľ���ʱ��  ��

	pData_rep->ExecutionWay = pParameter->ExecutionWay;//�ƻ���ִ�з�ʽ
	pData_rep->WeekMask = pParameter->WeekMask;//�ƻ���������
	pData_rep->SceneType = pParameter->SceneType;//�ƻ��ĳ�������
	pData_rep->SceneID = BES200_htons(pParameter->SceneID);//�ƻ��ĳ���ID
	pData_rep->ModeID = pParameter->ModeID;//�ƻ���ģʽID
	pData_rep->DefaultModeID = pParameter->DefaultModeID;//Ĭ��ģʽID

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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}
/* ����һ���ƻ����������ò��� */
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//�������ݣ�ָ�븳ֵ
	pData_rep = (BEGP_REQ_PACKETDATA_SET_SCHEDULER_PARAMETER_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_SCHEDULER_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_SCHEDULER_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
	pData_rep->ID = BES200_htons(SchedulerID);//�ƻ���ID��
	pData_rep->Active = pParameter->Active;//�ƻ��Ƿ�����
	memcpy(pData_rep->Name, pParameter->Name, sizeof(pData_rep->Name));//�ƻ�������
	memcpy(pData_rep->Alias, pParameter->Alias, sizeof(pData_rep->Alias));//�ƻ��ı���
	pData_rep->PlanType = pParameter->PlanType;//�ƻ�������

	pData_rep->StartDateYear = pParameter->StartDateYear;//�ƻ��Ŀ�ʼ���� ��
	pData_rep->StartDateMonth = pParameter->StartDateMonth;//�ƻ��Ŀ�ʼ���� ��
	pData_rep->StartDateDay = pParameter->StartDateDay;//�ƻ��Ŀ�ʼ����  ��
	pData_rep->EndDateYear = pParameter->EndDateYear;//�ƻ��Ľ������� ��
	pData_rep->EndDateMonth = pParameter->EndDateMonth;//�ƻ��Ľ������� ��
	pData_rep->EndDateDay = pParameter->EndDateDay;//�ƻ��Ľ�������  ��

	pData_rep->StartTimeHour = pParameter->StartTimeHour;//�ƻ��Ŀ�ʼʱ��  ʱ
	pData_rep->StartTimeMinute = pParameter->StartTimeMinute;//�ƻ��Ŀ�ʼʱ��  ��
	pData_rep->StartTimeSecond = pParameter->StartTimeSecond;//�ƻ��Ŀ�ʼʱ��  ��
	pData_rep->EndTimeHour = pParameter->EndTimeHour;//�ƻ��Ľ���ʱ��  ʱ
	pData_rep->EndTimeMinute = pParameter->EndTimeMinute;//�ƻ��Ľ���ʱ��  ��
	pData_rep->EndTimeSecond = pParameter->EndTimeSecond;//�ƻ��Ľ���ʱ��  ��

	pData_rep->ExecutionWay = pParameter->ExecutionWay;//�ƻ���ִ�з�ʽ
	pData_rep->WeekMask = pParameter->WeekMask;//�ƻ���������
	pData_rep->SceneType = pParameter->SceneType;//�ƻ��ĳ�������
	pData_rep->SceneID = BES200_htons(pParameter->SceneID);//�ƻ��ĳ���ID
	pData_rep->ModeID = pParameter->ModeID;//�ƻ���ģʽID
	pData_rep->DefaultModeID = pParameter->DefaultModeID;//Ĭ��ģʽID

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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}

/* ɾ��һ���ƻ� */
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//�������ݣ�ָ�븳ֵ
	pData_rep = (BEGP_REQ_PACKETDATA_DELETE_SCHEDULER_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_DELETE_SCHEDULER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_DELETE_SCHEDULER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
	pData_rep->ID = BES200_htons(SchedulerID);//�ƻ���ID��

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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}

/*��ȡһ���ƻ����������ò���*/
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)&SendBuf[0];
	pHead_rsp = (BEGP_RSP_HEAD_T*)&RecvBuf[0];

	//�������ݣ�ָ�븳ֵ
	pData_rep = (BEGP_REQ_PACKETDATA_GET_SCHEDULER_PARAMETER_T*)(&SendBuf[BEGP_REQ_HEAD_SIZE]);
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_SCHEDULER_PARAMETER_T*)(&RecvBuf[BEGP_RSP_HEAD_SIZE]);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_SCHEDULER_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_GET_SCHEDULER_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
	pData_rep->ID = BES200_htons(SchedulerID);    //Ҫ��ȡ���ID��

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

			pParameter->Active = pData_rsp->Active;//�ƻ��Ƿ�����
			memcpy(pParameter->Name, pData_rsp->Name, sizeof(pParameter->Name));//�ƻ�������
			memcpy(pParameter->Alias, pData_rsp->Alias, sizeof(pParameter->Alias));//�ƻ��ı���
			pParameter->PlanType = pData_rsp->PlanType;//�ƻ�������

			pParameter->StartDateYear = pData_rsp->StartDateYear;//�ƻ��Ŀ�ʼ���� ��
			pParameter->StartDateMonth = pData_rsp->StartDateMonth;//�ƻ��Ŀ�ʼ���� ��
			pParameter->StartDateDay = pData_rsp->StartDateDay;//�ƻ��Ŀ�ʼ����  ��
			pParameter->EndDateYear = pData_rsp->EndDateYear;//�ƻ��Ľ������� ��
			pParameter->EndDateMonth = pData_rsp->EndDateMonth;//�ƻ��Ľ������� ��
			pParameter->EndDateDay = pData_rsp->EndDateDay;//�ƻ��Ľ�������  ��

			pParameter->StartTimeHour = pData_rsp->StartTimeHour;//�ƻ��Ŀ�ʼʱ��  ʱ
			pParameter->StartTimeMinute = pData_rsp->StartTimeMinute;//�ƻ��Ŀ�ʼʱ��  ��
			pParameter->StartTimeSecond = pData_rsp->StartTimeSecond;//�ƻ��Ŀ�ʼʱ��  ��
			pParameter->EndTimeHour = pData_rsp->EndTimeHour;//�ƻ��Ľ���ʱ��  ʱ
			pParameter->EndTimeMinute = pData_rsp->EndTimeMinute;//�ƻ��Ľ���ʱ��  ��
			pParameter->EndTimeSecond = pData_rsp->EndTimeSecond;//�ƻ��Ľ���ʱ��  ��

			pParameter->ExecutionWay = pData_rsp->ExecutionWay;//�ƻ���ִ�з�ʽ
			pParameter->WeekMask = pData_rsp->WeekMask;//�ƻ���������
			pParameter->SceneType = pData_rsp->SceneType;//�ƻ��ĳ�������
			pParameter->SceneID = BES200_htons(pData_rsp->SceneID);//�ƻ��ĳ���ID
			pParameter->ModeID = pData_rsp->ModeID;//�ƻ���ģʽID
			pParameter->DefaultModeID = pData_rsp->DefaultModeID;//Ĭ��ģʽID
		}
		else
		{
			ret = BES200_ERROR_OTHERS;
		}
	}
	else
	{
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}

/*���һ������*/
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//�������ݣ�ָ�븳ֵ
	pData_req = (BEGP_REQ_PACKETDATA_ADD_SCENE_T*)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_ADD_SCENE;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_ADD_SCENE_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
	if (pParameter->SceneType == SCENE_TYPE_CONTROL) //���Ƴ���
	{
		BEGP_CONTROL_SCENE_PARAMETER_T        *pParameterControlScene;
		BEGP_CONTROL_SCENE_PARAMETER_T        *pDataControlScene_req;

		pDataControlScene_req = (BEGP_CONTROL_SCENE_PARAMETER_T*)pData_req;
		pParameterControlScene = (BEGP_CONTROL_SCENE_PARAMETER_T *)pParameter;//�ֽ������Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataControlScene_req->SceneType = pParameterControlScene->SceneType;//��������  
		pDataControlScene_req->ID = BES200_htons(SceneID);//����ID
		pDataControlScene_req->Active = pParameterControlScene->Active;//���Ƴ�������
		memcpy(pDataControlScene_req->Name, pParameterControlScene->Name, sizeof(pDataControlScene_req->Name));//����������
		memcpy(pDataControlScene_req->Alias, pParameterControlScene->Alias, sizeof(pDataControlScene_req->Alias));//����������
		for (i = 0; i < CONTROL_MODE_SIZE; i++)
		{
			pDataControlScene_req->ControlMode[i].Active = pParameterControlScene->ControlMode[i].Active;//ģʽ�Ƿ�����
			memcpy(pDataControlScene_req->ControlMode[i].Name, pParameterControlScene->ControlMode[i].Name, sizeof(pDataControlScene_req->ControlMode[i].Name));//ģʽ������
			pDataControlScene_req->ControlMode[i].ID = pParameterControlScene->ControlMode[i].ID;//ģʽ��ID
			for (j = 0; j < CONTROL_POINT_SIZE; j++)
			{
				pDataControlScene_req->ControlMode[i].ControlPoint[j].Active = pParameterControlScene->ControlMode[i].ControlPoint[j].Active;//���Ƿ�����
				pDataControlScene_req->ControlMode[i].ControlPoint[j].PointID = BES200_htons(pParameterControlScene->ControlMode[i].ControlPoint[j].PointID);//���ID
				pDataControlScene_req->ControlMode[i].ControlPoint[j].RunValue = BES200_htonl(pParameterControlScene->ControlMode[i].ControlPoint[j].RunValue);//�������ֵ
			}
		}
	}
	else if (pParameter->SceneType == SCENE_TYPE_COLLECT) //�ɼ�����
	{
		BEGP_COLLECT_SCENE_PARAMETER_T        *pParameterCollectScene;
		BEGP_COLLECT_SCENE_PARAMETER_T        *pDataCollectScene_req;

		pDataCollectScene_req = (BEGP_COLLECT_SCENE_PARAMETER_T*)pData_req;
		pParameterCollectScene = (BEGP_COLLECT_SCENE_PARAMETER_T *)pParameter;//�ֽ������Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataCollectScene_req->SceneType = pParameterCollectScene->SceneType;//��������  
		pDataCollectScene_req->ID = BES200_htons(SceneID);//����ID
		pDataCollectScene_req->Active = pParameterCollectScene->Active;//���Ƴ�������
		memcpy(pDataCollectScene_req->Name, pParameterCollectScene->Name, sizeof(pDataCollectScene_req->Name));//����������
		memcpy(pDataCollectScene_req->Alias, pParameterCollectScene->Alias, sizeof(pDataCollectScene_req->Alias));//����������
		for (i = 0; i < COLLECT_MODE_SIZE; i++)
		{
			pDataCollectScene_req->CollectMode[i].Active = pParameterCollectScene->CollectMode[i].Active;//ģʽ�Ƿ�����
			memcpy(pDataCollectScene_req->CollectMode[i].Name, pParameterCollectScene->CollectMode[i].Name, sizeof(pDataCollectScene_req->CollectMode[i].Name));//ģʽ������
			pDataCollectScene_req->CollectMode[i].ID = pParameterCollectScene->CollectMode[i].ID;//ģʽ��ID
			for (j = 0; j < CONTROL_POINT_SIZE; j++)
			{
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].Active = pParameterCollectScene->CollectMode[i].CollectPoint[j].Active;//���Ƿ�����
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].PointID = BES200_htons(pParameterCollectScene->CollectMode[i].CollectPoint[j].PointID);//���ID
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].CollectID = pParameterCollectScene->CollectMode[i].CollectPoint[j].CollectID;//��Ĳɼ���ʽ
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].TimeUnit = pParameterCollectScene->CollectMode[i].CollectPoint[j].TimeUnit;//���ʱ�����ɼ��ĵ�λ
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].Value = BES200_htons(pParameterCollectScene->CollectMode[i].CollectPoint[j].Value);//�ɼ�ֵ
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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}

/*����һ���������������ò���*/
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//�������ݣ�ָ�븳ֵ
	pData_req = (BEGP_REQ_PACKETDATA_SET_SCENE_PARAMETER_T*)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_SET_SCENE_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_SET_SCENE_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
	if (pParameter->SceneType == SCENE_TYPE_CONTROL) //���Ƴ���
	{
		BEGP_CONTROL_SCENE_PARAMETER_T        *pParameterControlScene;
		BEGP_CONTROL_SCENE_PARAMETER_T        *pDataControlScene_req;

		pDataControlScene_req = (BEGP_CONTROL_SCENE_PARAMETER_T*)pData_req;
		pParameterControlScene = (BEGP_CONTROL_SCENE_PARAMETER_T *)pParameter;//�ֽ������Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataControlScene_req->SceneType = pParameterControlScene->SceneType;//��������  
		pDataControlScene_req->ID = BES200_htons(SceneID);//����ID
		pDataControlScene_req->Active = pParameterControlScene->Active;//���Ƴ�������
		memcpy(pDataControlScene_req->Name, pParameterControlScene->Name, sizeof(pDataControlScene_req->Name));//����������
		memcpy(pDataControlScene_req->Alias, pParameterControlScene->Alias, sizeof(pDataControlScene_req->Alias));//����������
		for (i = 0; i < CONTROL_MODE_SIZE; i++)
		{
			pDataControlScene_req->ControlMode[i].Active = pParameterControlScene->ControlMode[i].Active;//ģʽ�Ƿ�����
			memcpy(pDataControlScene_req->ControlMode[i].Name, pParameterControlScene->ControlMode[i].Name, sizeof(pDataControlScene_req->ControlMode[i].Name));//ģʽ������
			pDataControlScene_req->ControlMode[i].ID = pParameterControlScene->ControlMode[i].ID;//ģʽ��ID
			for (j = 0; j < CONTROL_POINT_SIZE; j++)
			{
				pDataControlScene_req->ControlMode[i].ControlPoint[j].Active = pParameterControlScene->ControlMode[i].ControlPoint[j].Active;//���Ƿ�����
				pDataControlScene_req->ControlMode[i].ControlPoint[j].PointID = BES200_htons(pParameterControlScene->ControlMode[i].ControlPoint[j].PointID);//���ID
				pDataControlScene_req->ControlMode[i].ControlPoint[j].RunValue = BES200_htonl(pParameterControlScene->ControlMode[i].ControlPoint[j].RunValue);//�������ֵ
			}
		}
	}
	else if (pParameter->SceneType == SCENE_TYPE_COLLECT) //�ɼ�����
	{
		BEGP_COLLECT_SCENE_PARAMETER_T        *pParameterCollectScene;
		BEGP_COLLECT_SCENE_PARAMETER_T        *pDataCollectScene_req;

		pDataCollectScene_req = (BEGP_COLLECT_SCENE_PARAMETER_T*)pData_req;
		pParameterCollectScene = (BEGP_COLLECT_SCENE_PARAMETER_T *)pParameter;//�ֽ������Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

		pDataCollectScene_req->SceneType = pParameterCollectScene->SceneType;//��������  
		pDataCollectScene_req->ID = BES200_htons(SceneID);//����ID
		pDataCollectScene_req->Active = pParameterCollectScene->Active;//���Ƴ�������
		memcpy(pDataCollectScene_req->Name, pParameterCollectScene->Name, sizeof(pDataCollectScene_req->Name));//����������
		memcpy(pDataCollectScene_req->Alias, pParameterCollectScene->Alias, sizeof(pDataCollectScene_req->Alias));//����������
		for (i = 0; i < COLLECT_MODE_SIZE; i++)
		{
			pDataCollectScene_req->CollectMode[i].Active = pParameterCollectScene->CollectMode[i].Active;//ģʽ�Ƿ�����
			memcpy(pDataCollectScene_req->CollectMode[i].Name, pParameterCollectScene->CollectMode[i].Name, sizeof(pDataCollectScene_req->CollectMode[i].Name));//ģʽ������
			pDataCollectScene_req->CollectMode[i].ID = pParameterCollectScene->CollectMode[i].ID;//ģʽ��ID
			for (j = 0; j < CONTROL_POINT_SIZE; j++)
			{
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].Active = pParameterCollectScene->CollectMode[i].CollectPoint[j].Active;//���Ƿ�����
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].PointID = BES200_htons(pParameterCollectScene->CollectMode[i].CollectPoint[j].PointID);//���ID
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].CollectID = pParameterCollectScene->CollectMode[i].CollectPoint[j].CollectID;//��Ĳɼ���ʽ
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].TimeUnit = pParameterCollectScene->CollectMode[i].CollectPoint[j].TimeUnit;//���ʱ�����ɼ��ĵ�λ
				pDataCollectScene_req->CollectMode[i].CollectPoint[j].Value = BES200_htons(pParameterCollectScene->CollectMode[i].CollectPoint[j].Value);//�ɼ�ֵ
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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}
/*ɾ��һ������*/
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//�������ݣ�ָ�븳ֵ
	pData_req = (BEGP_REQ_PACKETDATA_DELETE_SCENE_T*)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_ECHO_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_DELETE_SCENE;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_DELETE_SCENE_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//������������
	pData_req->ID = BES200_htons(SceneID);//Ҫɾ���ĳ�����ID��

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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}
/*��ȡһ���������������ò���*/
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

	//֡ͷ���ݣ�ָ�븳ֵ
	pHead_req = (BEGP_REQ_HEAD_T*)SendBuf;
	pHead_rsp = (BEGP_RSP_HEAD_T*)RecvBuf;

	//�������ݣ�ָ�븳ֵ
	pData_req = (BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_T*)(SendBuf + BEGP_REQ_HEAD_SIZE);
	pData_rsp = (BEGP_RSP_PACKETDATA_GET_SCENE_PARAMETER_T*)(RecvBuf + BEGP_RSP_HEAD_SIZE);

	//���������ͷ
	pHead_req->SlaveAddress = 1;//����DDC��ʶ��ţ�������ͨ��ʱĬ�����ó�1����
	pHead_req->FunctionCode = BEGP_FUNCODE_GET_SCENE_PARAMETER;
	pHead_req->SequenceNumber = 0;
	pHead_req->DataSize = BES200_htons(BEGP_REQ_PACKETDATA_GET_SCENE_PARAMETER_SIZE);
	SendDataSize += BEGP_REQ_HEAD_SIZE;

	//����������������
	pData_req->ID = BES200_htons(SceneID);//Ҫ��ȡ�ĳ���������ID��

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
			if (pData_rsp->SceneType == SCENE_TYPE_CONTROL) //���Ƴ���
			{
				BEGP_CONTROL_SCENE_PARAMETER_T        *pParameterControlScene;
				BEGP_CONTROL_SCENE_PARAMETER_T        *pDataControlScene_rsp;

				pDataControlScene_rsp = (BEGP_CONTROL_SCENE_PARAMETER_T*)pData_rsp;
				pParameterControlScene = (BEGP_CONTROL_SCENE_PARAMETER_T *)pParameter;//�ֽ������Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

				

				pParameterControlScene->SceneType = pDataControlScene_rsp->SceneType;//��������  
				pParameterControlScene->ID = BES200_htons(pDataControlScene_rsp->ID);//����ID
				pParameterControlScene->Active = pDataControlScene_rsp->Active;//���Ƴ�������
				memcpy(pParameterControlScene->Name, pDataControlScene_rsp->Name, sizeof(pParameterControlScene->Name));//����������
				memcpy(pParameterControlScene->Alias, pDataControlScene_rsp->Alias, sizeof(pParameterControlScene->Alias));//����������
				for (i = 0; i < CONTROL_MODE_SIZE; i++)
				{
					pParameterControlScene->ControlMode[i].Active = pDataControlScene_rsp->ControlMode[i].Active;//ģʽ�Ƿ�����
					memcpy(pParameterControlScene->ControlMode[i].Name, pDataControlScene_rsp->ControlMode[i].Name, sizeof(pParameterControlScene->ControlMode[i].Name));//ģʽ������
					pParameterControlScene->ControlMode[i].ID = pDataControlScene_rsp->ControlMode[i].ID;//ģʽ��ID
					for (j = 0; j < CONTROL_POINT_SIZE; j++)
					{
						pParameterControlScene->ControlMode[i].ControlPoint[j].Active = pDataControlScene_rsp->ControlMode[i].ControlPoint[j].Active;//���Ƿ�����
						pParameterControlScene->ControlMode[i].ControlPoint[j].PointID = BES200_htons(pDataControlScene_rsp->ControlMode[i].ControlPoint[j].PointID);//���ID
						pParameterControlScene->ControlMode[i].ControlPoint[j].RunValue = BES200_htonl(pDataControlScene_rsp->ControlMode[i].ControlPoint[j].RunValue);//�������ֵ
					}
				}
			}
			else if (pData_rsp->SceneType == SCENE_TYPE_COLLECT) //�ɼ�����
			{
				BEGP_COLLECT_SCENE_PARAMETER_T        *pParameterCollectScene;
				BEGP_COLLECT_SCENE_PARAMETER_T        *pDataCollectScene_rsp;

				pDataCollectScene_rsp = (BEGP_COLLECT_SCENE_PARAMETER_T*)pData_rsp;
				pParameterCollectScene = (BEGP_COLLECT_SCENE_PARAMETER_T *)pParameter;//�ֽ������Ҫһ�£���BES200.h��Ҫ1�ֽڶ���

				pParameterCollectScene->SceneType = pDataCollectScene_rsp->SceneType;//��������  
				pParameterCollectScene->ID = BES200_htons(pDataCollectScene_rsp->ID);//����ID
				pParameterCollectScene->Active = pDataCollectScene_rsp->Active;//���Ƴ�������
				memcpy(pParameterCollectScene->Name, pDataCollectScene_rsp->Name, sizeof(pParameterCollectScene->Name));//����������
				memcpy(pParameterCollectScene->Alias, pDataCollectScene_rsp->Alias, sizeof(pParameterCollectScene->Alias));//����������
				for (i = 0; i < COLLECT_MODE_SIZE; i++)
				{
					pParameterCollectScene->CollectMode[i].Active = pDataCollectScene_rsp->CollectMode[i].Active;//ģʽ�Ƿ�����
					memcpy(pParameterCollectScene->CollectMode[i].Name, pDataCollectScene_rsp->CollectMode[i].Name, sizeof(pParameterCollectScene->CollectMode[i].Name));//ģʽ������
					pParameterCollectScene->CollectMode[i].ID = pDataCollectScene_rsp->CollectMode[i].ID;//ģʽ��ID
					for (j = 0; j < CONTROL_POINT_SIZE; j++)
					{
						pParameterCollectScene->CollectMode[i].CollectPoint[j].Active = pDataCollectScene_rsp->CollectMode[i].CollectPoint[j].Active;//���Ƿ�����
						pParameterCollectScene->CollectMode[i].CollectPoint[j].PointID = BES200_htons(pDataCollectScene_rsp->CollectMode[i].CollectPoint[j].PointID);//���ID
						pParameterCollectScene->CollectMode[i].CollectPoint[j].CollectID = pDataCollectScene_rsp->CollectMode[i].CollectPoint[j].CollectID;//��Ĳɼ���ʽ
						pParameterCollectScene->CollectMode[i].CollectPoint[j].TimeUnit = pDataCollectScene_rsp->CollectMode[i].CollectPoint[j].TimeUnit;//���ʱ�����ɼ��ĵ�λ
						pParameterCollectScene->CollectMode[i].CollectPoint[j].Value = BES200_htons(pDataCollectScene_rsp->CollectMode[i].CollectPoint[j].Value);//�ɼ�ֵ
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
		//�����봦��
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
			sprintf_s(*errmsg,2048, "������");
		}
	}
	return ret;
}



