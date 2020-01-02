#include "stdafx.h"
#include "TcpSocket.h"
#include <winsock2.h>

#include "tcpSocketHead.h"

CTcpSocket::CTcpSocket()
{
}


CTcpSocket::~CTcpSocket()
{
	map<string, TCP_CLIENTSOCKT_TS>::reverse_iterator iter;

	for (iter = m_mapClients.rbegin(); iter != m_mapClients.rend(); iter++)
	{
		shutdown(iter->second.Socketfd, 2);
		closesocket(iter->second.Socketfd);
	}
}

int CTcpSocket::SendMsg(const char ServerIP[], int nPort, const char *pSendData, int nSendDataSize, char *pRecvData, int *nRecvDataSize, char **errmsg)
{

	//SOCKET sc = 0;
	//int nIndex = -1;

	//char cFlag[255] = { 0 };
	//sprintf_s(cFlag, 255, "%s:%d", ServerIP, nPort);
	//string strKey = cFlag;

	//map <string, TCP_CLIENTSOCKT_TS >::iterator iter;

	//iter = m_mapClients.find(strKey);

	//if (iter != m_mapClients.end())
	//{
	//	// �ҵ���
	//}
	//else{
	//	// û���ҵ�
	//	if (Connect(ServerIP, nPort) == TCP_ERR_OK){

	//	}
	//}


	//iter = m_mapClients.find(strKey);

	//if (iter!=m_mapClients.end())
	//{
	//	// ��������
	//	int nResSend = SendMsg(iter->second.Socketfd, pSendData, nSendDataSize);

	//	// ��������
	//	int nResRecv = RecvMsg(iter->second.Socketfd, pRecvData, nRecvDataSize);
	//}

return 0;

}

int CTcpSocket::SendMsg(SOCKET socketfd, const char SendBuf[], int SendDataCount)
{
	int           iRes;
	struct timeval overtime;
	fd_set         writefd;// , readfd;
	// 
	//	char RecvBuf[999];

	if (NULL != socketfd)
	{
		//
		sprintf_s(m_Logbuf, "TCP_Send | Socketfd = %d.", socketfd);
		LogPrint(NULL, m_Logbuf);


		FD_ZERO(&writefd);
		FD_SET(socketfd, &writefd);
		overtime.tv_sec = 1;			 //�뼶
		overtime.tv_usec = 3000; //΢�
		iRes = select(socketfd + 1, 0, &writefd, 0, &overtime);
		if (0 < iRes){
			//
			if (FD_ISSET(socketfd, &writefd)){
				iRes = send(socketfd, (const char*)SendBuf, SendDataCount, 0);
				//setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&TimeOut, sizeof(TimeOut)) == SOCKET_ERROR)
				if (0 < iRes){
					//
					iRes = TCP_ERR_OK;

				}
				else{//���Ͳ��ɹ�
					//
					sprintf_s(m_Logbuf, "TCP_Send | Socketfd = %d,Send Finish wait select overtime", socketfd);
					LogPrint(NULL, m_Logbuf);
					//
					iRes = TCP_ERR_SEND;
				}
			}
			else{//û����Ч��socket
				//
				sprintf_s(m_Logbuf, "TCP_Send | Socketfd = %d,Send Finish wait select overtime", socketfd);
				LogPrint(NULL, m_Logbuf);
				iRes = TCP_ERR_SEND_OUTTIME;
			}
		}
		else{//û����Ч��socket
			//
			sprintf_s(m_Logbuf, "TCP_Send | Socketfd = %d,Wait Send State Select overtime.", socketfd);
			LogPrint(NULL, m_Logbuf);
			//			
			iRes = TCP_ERR_SEND_OUTTIME;
		}
	}
	else{//socketΪnull
		//
		sprintf_s(m_Logbuf, "TCP_Send | Socketfd = NULL.");
		LogPrint(NULL, m_Logbuf);
		//
		iRes = TCP_ERR_RECIVE;
	}//

	return iRes;
}

int CTcpSocket::RecvMsg(SOCKET socketfd, char *pRecvBuf, int *pRecvDataCount)
{
	int            iRes = 0;
	struct timeval overtime;
	fd_set         readfd;
	char   *pRecvBuftemp = pRecvBuf;
	int TimeOut = 2000; //1��

	*pRecvDataCount = 0;
	//
	if (NULL != socketfd){
		int nRecvBufLen = PACKAGE_SIZE_MAX; //����Ϊ32K
		setsockopt(socketfd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBufLen, sizeof(int));

		FD_ZERO(&readfd);
		FD_SET(socketfd, &readfd);
		overtime.tv_sec = 0;			 //�뼶
		overtime.tv_usec = 100; //΢�
		iRes = select(socketfd + 1, &readfd, 0, 0, &overtime);
		if (0 < iRes){
			if (FD_ISSET(socketfd, &readfd)){
				//�������� Ĭ�ϻ�����Ϊ8k ������pRecvBuf���趨Ҫ����8k
				/*if (setsockopt(Socketfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&TimeOut, sizeof(TimeOut)) == SOCKET_ERROR)
				{
				iRes = TCP_ERR_RECIVE;
				}
				else{*/

				iRes = recv(socketfd, pRecvBuf, PACKAGE_SIZE_MAX, 0);
				if (iRes > 0){
					*pRecvDataCount = iRes;
					printf("receive  size  is %ld  \r\n", *pRecvDataCount);
					iRes = TCP_ERR_OK;
				}
				else{
					/*printf("receive error, size long is %ld \r\n", *pRecvDataCount);*/
					sprintf_s(m_Logbuf, "TCP_Recive | Socketfd = %d,Recviv error!", socketfd);
					LogPrint(NULL, m_Logbuf);
					iRes = TCP_ERR_RECIVE;
					//printf("receive error long is %ld \r\n", *pRecvDataCount);
				}
				/*}*/
			}
			else{//ѡ��ʱ
				//
				/*printf("readfd is not the member \r\n" );*/
				sprintf_s(m_Logbuf, "TCP_Recive | Socketfd = %d,Recviv overtime! ", socketfd);
				LogPrint(NULL, m_Logbuf);
				//
				iRes = TCP_ERR_RECIVE_OUTTIME;
			}
		}
		else{//ѡ��ʱ
			//
			sprintf_s(m_Logbuf, "TCP_Recive | Socketfd = %d,Recviv overtime!", socketfd);
			LogPrint(NULL, m_Logbuf);
			/*printf("receive SELECT ��ʱ is %d\r\n", iRes);*/
			//
			iRes = TCP_ERR_RECIVE_OUTTIME;
		}
	}
	else{//soketΪ��
		//
		sprintf_s(m_Logbuf, "TCP_Recive | Socketfd = NULL.");
		LogPrint(NULL, m_Logbuf);

		iRes = TCP_ERR_RECIVE;
		//
	}
	return iRes;
}

int CTcpSocket::Connect(const char IP[], int nPort)
{
	char cFlag[255] = { 0 };
	sprintf_s(cFlag, 255, "%s:%d", IP, nPort);
	string strKey = cFlag;

	int nRecvBufLen = PACKAGE_SIZE_MAX; //����Ϊ32K
	int	i =0, iRes;
	unsigned long  ul=0;

	TCP_CLIENTSOCKT_TS tcpOne;
	strcpy_s(tcpOne.ServerIP, IP);

	tcpOne.nPort = nPort;

	tcpOne.Socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//��ʹ��send receive�����������ݵķ��ͽ���

	tcpOne.ServerSocketAddr.sin_family = AF_INET;
	
	tcpOne.ServerSocketAddr.sin_addr .s_addr = inet_addr(tcpOne.ServerIP);

	tcpOne.ServerSocketAddr.sin_port = htons(tcpOne.nPort);

	setsockopt(tcpOne.Socketfd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBufLen, sizeof(int));
	struct linger so_linger;
	//�ر�ʱǿ�� �ر�
	so_linger.l_onoff = 1;
	so_linger.l_linger = 1;
	setsockopt(tcpOne.Socketfd, SOL_SOCKET, SO_LINGER, (const char*)&so_linger, sizeof so_linger);

	iRes = ioctlsocket(tcpOne.Socketfd, FIONBIO, (unsigned long*)&ul);

	//
	//�趨�������ɹ�
	if (SOCKET_ERROR != iRes){
		//
		sprintf_s(m_Logbuf, "TCP_Connect | ClientSocket[%d].Socketfd = %d ioctlsocket Success.", i, tcpOne.Socketfd);
		LogPrint(tcpOne.ServerIP, m_Logbuf);
		//
		iRes = connect(tcpOne.Socketfd, (const struct sockaddr *)&(tcpOne.ServerSocketAddr), sizeof(struct sockaddr_in));
		//���ӳɹ�
		if (0 == iRes){
			//
			sprintf_s(m_Logbuf, "TCP_Connect | ClientSocket[%d].Socketfd = %d connect Success .", i, tcpOne.Socketfd);
			LogPrint(tcpOne.ServerIP, m_Logbuf);
			//							
			tcpOne.State = TCP_CLIENTSOCKET_STATE_CONN;
			iRes = TCP_ERR_OK;

			m_mapClients.insert(pair<string, TCP_CLIENTSOCKT_TS>(strKey, tcpOne));
			//break;
		}
		else{
			//���ɹ� ����select�������г�ʱ����
			sprintf_s(m_Logbuf, "TCP_Connect | ClientSocket[%d].Socketfd = %d connect not success so Select...", i, tcpOne.Socketfd);
			LogPrint(tcpOne.ServerIP, m_Logbuf);
			fd_set writefd;
			struct timeval overtime;
			FD_ZERO(&writefd);
			FD_SET(tcpOne.Socketfd, &writefd);
			overtime.tv_sec = 0;//
			overtime.tv_usec = 3000; //΢�
			iRes = select(tcpOne.Socketfd + 1, 0, &writefd, 0, &overtime);
			if (0 < iRes){
				if (FD_ISSET(tcpOne.Socketfd, &writefd)){
					//��Ч
					sprintf_s(m_Logbuf, "TCP_Connect | ClientSocket[%d].Socketfd = %d connect Select Success", i, tcpOne.Socketfd);
					LogPrint(tcpOne.ServerIP, m_Logbuf);
					//�ɹ���������Ϊ����ģʽ
					ul = 0;
					iRes = ioctlsocket(tcpOne.Socketfd, FIONBIO, (unsigned long*)&ul);
					//����״̬
					tcpOne.State = TCP_CLIENTSOCKET_STATE_CONN;
					//*pSocketfd = pClientSocket->Socketfd;
					m_mapClients.insert(pair<string, TCP_CLIENTSOCKT_TS>(strKey, tcpOne));
					iRes = TCP_ERR_OK;
				}
				else{
					//��Ч
					iRes = WSAGetLastError();
					sprintf_s(m_Logbuf, "TCP_Connect | ClientSocket[%d].Socketfd = %d connect Select Failed ErrCode = %d!", i, tcpOne.Socketfd, iRes);
					LogPrint(tcpOne.ServerIP, m_Logbuf);

					//�ر�socket
					closesocket(tcpOne.Socketfd);
					//��socket�ͻ�clientsocket����ȥ�����ͷſռ�

					//memset(pClientSocket, 0, sizeof(TCP_CLIENTSOCKT_T));

					tcpOne.State = TCP_CLIENTSOCKET_STATE_IDLE;
					//����ֵΪ TCP IO����
					iRes = TCP_ERR_CONNECT_IOCTL;
				//	break;
				}
			}
			else{//��ʱ����
				iRes = WSAGetLastError();

				sprintf_s(m_Logbuf, "TCP_Connect | ClientSocket[%d].Socketfd = %d connect Select Overtime ErrCode = %d  ��", i, tcpOne.Socketfd, iRes);
				LogPrint(tcpOne.ServerIP, m_Logbuf);
				//�ر�socket
				closesocket(tcpOne.Socketfd);

				//��socket�ͻ�clientsocket����ȥ�����ͷſռ�
				//memset(pClientSocket, 0, sizeof(TCP_CLIENTSOCKT_T));
				tcpOne.State = TCP_CLIENTSOCKET_STATE_IDLE;

				iRes = TCP_ERR_CONNECT_OUTTIME;
				//break;
			}//select���ɹ�
		}//���Ӳ��ɹ�
	}
	else{//�趨���������ɹ�
		iRes = WSAGetLastError();
		//
		sprintf_s(m_Logbuf, "TCP_Connect | ClientSocket[%d].Socketfd = %d ioctlsocket Failed ErrCode = %d!", i, tcpOne.Socketfd, iRes);
		LogPrint(tcpOne.ServerIP, m_Logbuf);
		//
		closesocket(tcpOne.Socketfd);//ÿ�ε�����socket֮��Ӧ�ý����ͷ�

		//��socket�ͻ�clientsocket����ȥ�����ͷſռ�
	//	memset(pClientSocket, 0, sizeof(TCP_CLIENTSOCKT_T));
		tcpOne.State = TCP_CLIENTSOCKET_STATE_IDLE;

		iRes = TCP_ERR_CONNECT_IOCTL;
	}
	return iRes;
}

int CTcpSocket::TCP_Connect(const char IP[], int nPort, SOCKET *pSocketfd)
{
	int nRt=-1;
	SOCKET sc = 0;
	int nIndex = -1;

	char cFlag[255] = { 0 };
	sprintf_s(cFlag, 255, "%s:%d", IP, nPort);

	string strKey = cFlag;

	map <string, TCP_CLIENTSOCKT_TS >::iterator iter;

	iter = m_mapClients.find(strKey);

	if (iter != m_mapClients.end())
	{
		// �ҵ���
		*pSocketfd = iter->second.Socketfd;
	}
	else{
		// û���ҵ�
		if (Connect(IP, nPort) == TCP_ERR_OK){
			//
			printf("............................����µĿͻ���..................");
			nRt = TCP_ERR_OK;
		}
	}


	iter = m_mapClients.find(strKey);

	if (iter != m_mapClients.end())
	{
		*pSocketfd = iter->second.Socketfd;
		nRt = TCP_ERR_OK;

	}
	return 0;
}

void CTcpSocket::TCP_Disconnect(SOCKET Socketfd)
{
	
}

void CTcpSocket::TCP_Disconnect(const char IP[], int nPort)
{
	char cFlag[255] = { 0 };
	sprintf_s(cFlag, 255, "%s:%d", IP, nPort);

	string strKey = cFlag;

	map <string, TCP_CLIENTSOCKT_TS >::iterator iter;

	iter = m_mapClients.find(strKey);

	if (iter != m_mapClients.end())
	{
		// �ҵ���
		shutdown(iter->second.Socketfd, 2);
		closesocket(iter->second.Socketfd);

		m_mapClients.erase(iter);

	}
}

int CTcpSocket::TCP_Send(SOCKET Socketfd, const char SendBuf[], int SendDataCount)
{
	int           iRes;
	struct timeval overtime;
	fd_set         writefd;// , readfd;
	// 
	//	char RecvBuf[999];

	if (NULL != Socketfd)
	{
		//
		sprintf_s(m_Logbuf, "TCP_Send | Socketfd = %d.", Socketfd);
		LogPrint(NULL, m_Logbuf);


		FD_ZERO(&writefd);
		FD_SET(Socketfd, &writefd);
		overtime.tv_sec = 1;			 //�뼶
		overtime.tv_usec = 3000; //΢�
		iRes = select(Socketfd + 1, 0, &writefd, 0, &overtime);
		if (0 < iRes){
			//
			if (FD_ISSET(Socketfd, &writefd)){
				iRes = send(Socketfd, (const char*)SendBuf, SendDataCount, 0);
				//setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&TimeOut, sizeof(TimeOut)) == SOCKET_ERROR)
				if (0 < iRes){
					//
					iRes = TCP_ERR_OK;

				}
				else{//���Ͳ��ɹ�
					//
					sprintf_s(m_Logbuf, "TCP_Send | Socketfd = %d,Send Finish wait select overtime", Socketfd);
					LogPrint(NULL, m_Logbuf);
					//
					iRes = TCP_ERR_SEND;
				}
			}
			else{//û����Ч��socket
				//
				sprintf_s(m_Logbuf, "TCP_Send | Socketfd = %d,Send Finish wait select overtime", Socketfd);
				LogPrint(NULL, m_Logbuf);
				iRes = TCP_ERR_SEND_OUTTIME;
			}
		}
		else{//û����Ч��socket
			//
			sprintf_s(m_Logbuf, "TCP_Send | Socketfd = %d,Wait Send State Select overtime.", Socketfd);
			LogPrint(NULL, m_Logbuf);
			//			
			iRes = TCP_ERR_SEND_OUTTIME;
		}
	}
	else{//socketΪnull
		//
		sprintf_s(m_Logbuf, "TCP_Send | Socketfd = NULL.");
		LogPrint(NULL, m_Logbuf);
		//
		iRes = TCP_ERR_RECIVE;
	}//

	return iRes;
}

int CTcpSocket::TCP_Recive(SOCKET Socketfd, char *pRecvBuf, int *pRecvDataCount)
{
	int            iRes = 0;
	struct timeval overtime;
	fd_set         readfd;
	char   *pRecvBuftemp = pRecvBuf;
	int TimeOut = 2000; //1��

	*pRecvDataCount = 0;
	//
	if (NULL != Socketfd){
	//	int nRecvBufLen = PACKAGE_SIZE_MAX; //����Ϊ32K
		int nRecvBufLen = PACKAGE_SIZE_MAX; //����Ϊ32K
		setsockopt(Socketfd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBufLen, sizeof(int));

		FD_ZERO(&readfd);
		FD_SET(Socketfd, &readfd);
		overtime.tv_sec = 0;			 //�뼶
		overtime.tv_usec = 100; //΢�
		iRes = select(Socketfd + 1, &readfd, 0, 0, &overtime);
		if (0 < iRes){
			if (FD_ISSET(Socketfd, &readfd)){
				//�������� Ĭ�ϻ�����Ϊ8k ������pRecvBuf���趨Ҫ����8k
				/*if (setsockopt(Socketfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&TimeOut, sizeof(TimeOut)) == SOCKET_ERROR)
				{
				iRes = TCP_ERR_RECIVE;
				}
				else{*/

				iRes = recv(Socketfd, pRecvBuf, PACKAGE_SIZE_MAX, 0);
				//LOG_INFO(iRes);
				if (iRes > 0){
					*pRecvDataCount = iRes;
					printf("receive  size  is %ld  \r\n", *pRecvDataCount);
					iRes = TCP_ERR_OK;
				}
				else{
					/*printf("receive error, size long is %ld \r\n", *pRecvDataCount);*/
					sprintf_s(m_Logbuf, "TCP_Recive | Socketfd = %d,Recviv iRes error!", Socketfd);
					LogPrint(NULL, m_Logbuf);
					iRes = TCP_ERR_RECIVE;
					//printf("receive error long is %ld \r\n", *pRecvDataCount);
				}
				/*}*/
			}
			else{//ѡ��ʱ
				//
				/*printf("readfd is not the member \r\n" );*/
				sprintf_s(m_Logbuf, "TCP_Recive | Socketfd = %d,Recviv Select overtime! ", Socketfd);
				LogPrint(NULL, m_Logbuf);
				//
				iRes = TCP_ERR_RECIVE_OUTTIME;
			}
		}
		else{//ѡ��ʱ
			//
			sprintf_s(m_Logbuf, "TCP_Recive | Socketfd = %d,Recviv overtime!", Socketfd);
			LogPrint(NULL, m_Logbuf);
			/*printf("receive SELECT ��ʱ is %d\r\n", iRes);*/
			//
			iRes = TCP_ERR_RECIVE_OUTTIME;
		}
	}
	else{//soketΪ��
		//
		sprintf_s(m_Logbuf, "TCP_Recive | Socketfd = NULL.");
		LogPrint(NULL, m_Logbuf);

		iRes = TCP_ERR_RECIVE;
		//
	}
	return iRes;
}

int CTcpSocket::LogPrint(char *pLogAddress, char *pLogString)
{
	LOGD(pLogAddress<< pLogString);
	return 0;
}
