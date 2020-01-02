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
			printf("BES200_ERROR_CONNECTFAILED 连接失败\r\n");
			break;
		}
		else {
			//连接成功，发送数据
			if (m_tcpSocket.TCP_Send(socketfd, pSendData, SendDataSize) != TCP_ERR_OK) {
				//发送不成功
				//m_tcpSocket.TCP_Disconnect(socketfd);
				//m_tcpSocket.LogPrint("TcpError.log", "数据发送失败了 好奇怪！");
				LOG_ERROR(ServerIP << "数据发送失败！");

				m_tcpSocket.TCP_Disconnect(ServerIP, 6060);

				sendtrytimes++;
				if (sendtrytimes <= TCP_TRYTIMES) {
					continue;
				}
				else {
					ret = BES200_ERROR_SEND;
					//printf("BES200_ERROR_SENDTIMEOUT 发送超时\r\n");
					LOG_WARN(ServerIP << "发送超时！");
					break;
				}

			}
			else {
				//接收数据
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
					//printf("收到数据了 \n");我看下
					//终于接收到数据了,进行CRC校验
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
			}//发送是否成功
		}//连接是否成功

	} while (1);

	//m_TCP.TCP_Disconnect(socketfd);
	return ret;
}
