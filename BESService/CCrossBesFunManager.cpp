#include "stdafx.h"
#include "CCrossBesFunManager.h"
#include "Gloable_Cross.h"

using namespace Gloable_Cross;

CCrossBesFunManager::CCrossBesFunManager()
{
}


CCrossBesFunManager::~CCrossBesFunManager()
{
}
int CCrossBesFunManager::BES200_Communication2(const char ServerIP[], int nPort, const char *pSendData, int SendDataSize, char *pRecvData, int *RecvDataSize/*, char **errmsg*/)
{
	int ret, res, sendtrytimes = 0, recvtrytimes = 0, receivetimes = 0;
	SOCKET socketfd;
	uint16_t *crc_req, crc1, crc2;
	int FrameSize = 0;
	BEGP_RSP_HEAD_T *pHead_rsp;
	int datasize = 0;
	crc_req = (uint16_t*)(pSendData + SendDataSize);
	*crc_req = m_comFun.BES200_CRC_16(pSendData, SendDataSize);
	SendDataSize += BEGP_CRC_SIZE;
	//
	pHead_rsp = (BEGP_RSP_HEAD_T*)pRecvData;

	uint32_t us = m_comFun.BES200_htonl(pHead_rsp->DataSize) + BEGP_CRC_SIZE + BEGP_RSP_HEAD_SIZE;
	uint32_t us2 = BEGP_RSP_HEAD_SIZE;
	uint32_t us3 = m_comFun.BES200_htonl(pHead_rsp->DataSize);

	do {
		if (m_tcpSocket.TCP_Connect(ServerIP, nPort, &socketfd) != TCP_ERR_OK) {
			ret = BES200_ERROR_CONNECT;
			printf("BES200_ERROR_CONNECTFAILED ����ʧ��\r\n");
			break;
		}
		else {
			//���ӳɹ�����������
			if (m_tcpSocket.TCP_Send(socketfd, pSendData, SendDataSize) != TCP_ERR_OK) {
				//���Ͳ��ɹ�
				//m_tcpSocket.TCP_Disconnect(socketfd);
				//m_tcpSocket.LogPrint("TcpError.log", "���ݷ���ʧ���� ����֣�");
				LOG_ERROR(ServerIP << "���ݷ���ʧ�ܣ�");

				m_tcpSocket.TCP_Disconnect(ServerIP, 6060);

				sendtrytimes++;
				if (sendtrytimes <= TCP_TRYTIMES) {
					continue;
				}
				else {
					ret = BES200_ERROR_SEND;
					//printf("BES200_ERROR_SENDTIMEOUT ���ͳ�ʱ\r\n");
					LOG_WARN(ServerIP << "���ͳ�ʱ��");
					break;
				}

			}
			else {
				//��������
				receivetimes = 0;
				datasize = 0;
				do {
					res = m_tcpSocket.TCP_Recive(socketfd, pRecvData + datasize, RecvDataSize);
					datasize += *RecvDataSize;
					*RecvDataSize = 0;
					receivetimes++;
					Sleep(10);
					//printf("retyr time is %ld", receivetimes);
				} while ((datasize != m_comFun.BES200_htonl(pHead_rsp->DataSize) + BEGP_CRC_SIZE + BEGP_RSP_HEAD_SIZE) && (receivetimes < 100));
				/*printf("receive time is %ld \r\n", receivetimes);
				printf("res is %d \r\n",res);*/
				*RecvDataSize = datasize;

				if (datasize != m_comFun.BES200_htonl(pHead_rsp->DataSize) + BEGP_CRC_SIZE + BEGP_RSP_HEAD_SIZE)
				{
					recvtrytimes++;
					//m_tcpSocket.TCP_Disconnect(socketfd);
					if (recvtrytimes <= TCP_TRYTIMES)
						continue;
					else {
						ret = BES200_ERROR_SEND;
						//printf("timeout error \r\n");
						LOG_WARN("timeout error !");
						break;
					}

				}
				else {
					//printf("�յ������� \n");�ҿ���
					//���ڽ��յ�������,����CRCУ��
					if (*RecvDataSize < BEGP_RSP_HEAD_SIZE) {
						ret = BES200_ERROR_RECIVE;
						//printf("RecvDataSize error \r\n");
						LOG_ERROR("RecvDataSize error !");
						break;
					}
					else {
						crc1 = m_comFun.BES200_CRC_16(pRecvData, (*RecvDataSize) - BEGP_CRC_SIZE);
						crc2 = *((uint16_t*)(pRecvData + (*RecvDataSize) - BEGP_CRC_SIZE));
						if (crc1 == crc2) {
							ret = BES200_ERROR_OK;
							break;
						}
						else {
							ret = BES200_ERROR_RECIVE;
							//printf("crc error \r\n");
							LOG_ERROR("crc error !");
							break;
						}
					}
				}
				datasize = 0;
				receivetimes = 0;
			}//�����Ƿ�ɹ�
		}//�����Ƿ�ɹ�

	} while (1);

	//m_TCP.TCP_Disconnect(socketfd);
	return ret;
}
