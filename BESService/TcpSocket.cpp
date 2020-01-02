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
	//	// 找到了
	//}
	//else{
	//	// 没有找到
	//	if (Connect(ServerIP, nPort) == TCP_ERR_OK){

	//	}
	//}


	//iter = m_mapClients.find(strKey);

	//if (iter!=m_mapClients.end())
	//{
	//	// 发送数据
	//	int nResSend = SendMsg(iter->second.Socketfd, pSendData, nSendDataSize);

	//	// 接收数据
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
		overtime.tv_sec = 1;			 //秒级
		overtime.tv_usec = 3000; //微妙级
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
				else{//发送不成功
					//
					sprintf_s(m_Logbuf, "TCP_Send | Socketfd = %d,Send Finish wait select overtime", socketfd);
					LogPrint(NULL, m_Logbuf);
					//
					iRes = TCP_ERR_SEND;
				}
			}
			else{//没有有效的socket
				//
				sprintf_s(m_Logbuf, "TCP_Send | Socketfd = %d,Send Finish wait select overtime", socketfd);
				LogPrint(NULL, m_Logbuf);
				iRes = TCP_ERR_SEND_OUTTIME;
			}
		}
		else{//没有有效的socket
			//
			sprintf_s(m_Logbuf, "TCP_Send | Socketfd = %d,Wait Send State Select overtime.", socketfd);
			LogPrint(NULL, m_Logbuf);
			//			
			iRes = TCP_ERR_SEND_OUTTIME;
		}
	}
	else{//socket为null
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
	int TimeOut = 2000; //1秒

	*pRecvDataCount = 0;
	//
	if (NULL != socketfd){
		int nRecvBufLen = PACKAGE_SIZE_MAX; //设置为32K
		setsockopt(socketfd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBufLen, sizeof(int));

		FD_ZERO(&readfd);
		FD_SET(socketfd, &readfd);
		overtime.tv_sec = 0;			 //秒级
		overtime.tv_usec = 100; //微妙级
		iRes = select(socketfd + 1, &readfd, 0, 0, &overtime);
		if (0 < iRes){
			if (FD_ISSET(socketfd, &readfd)){
				//接收数据 默认缓冲区为8k 缓冲区pRecvBuf的设定要大于8k
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
			else{//选择超时
				//
				/*printf("readfd is not the member \r\n" );*/
				sprintf_s(m_Logbuf, "TCP_Recive | Socketfd = %d,Recviv overtime! ", socketfd);
				LogPrint(NULL, m_Logbuf);
				//
				iRes = TCP_ERR_RECIVE_OUTTIME;
			}
		}
		else{//选择超时
			//
			sprintf_s(m_Logbuf, "TCP_Recive | Socketfd = %d,Recviv overtime!", socketfd);
			LogPrint(NULL, m_Logbuf);
			/*printf("receive SELECT 超时 is %d\r\n", iRes);*/
			//
			iRes = TCP_ERR_RECIVE_OUTTIME;
		}
	}
	else{//soket为空
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

	int nRecvBufLen = PACKAGE_SIZE_MAX; //设置为32K
	int	i =0, iRes;
	unsigned long  ul=0;

	TCP_CLIENTSOCKT_TS tcpOne;
	strcpy_s(tcpOne.ServerIP, IP);

	tcpOne.nPort = nPort;

	tcpOne.Socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//可使用send receive函数进行数据的发送接收

	tcpOne.ServerSocketAddr.sin_family = AF_INET;
	
	tcpOne.ServerSocketAddr.sin_addr .s_addr = inet_addr(tcpOne.ServerIP);

	tcpOne.ServerSocketAddr.sin_port = htons(tcpOne.nPort);

	setsockopt(tcpOne.Socketfd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBufLen, sizeof(int));
	struct linger so_linger;
	//关闭时强制 关闭
	so_linger.l_onoff = 1;
	so_linger.l_linger = 1;
	setsockopt(tcpOne.Socketfd, SOL_SOCKET, SO_LINGER, (const char*)&so_linger, sizeof so_linger);

	iRes = ioctlsocket(tcpOne.Socketfd, FIONBIO, (unsigned long*)&ul);

	//
	//设定非阻塞成功
	if (SOCKET_ERROR != iRes){
		//
		sprintf_s(m_Logbuf, "TCP_Connect | ClientSocket[%d].Socketfd = %d ioctlsocket Success.", i, tcpOne.Socketfd);
		LogPrint(tcpOne.ServerIP, m_Logbuf);
		//
		iRes = connect(tcpOne.Socketfd, (const struct sockaddr *)&(tcpOne.ServerSocketAddr), sizeof(struct sockaddr_in));
		//连接成功
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
			//不成功 调用select函数进行超时处理
			sprintf_s(m_Logbuf, "TCP_Connect | ClientSocket[%d].Socketfd = %d connect not success so Select...", i, tcpOne.Socketfd);
			LogPrint(tcpOne.ServerIP, m_Logbuf);
			fd_set writefd;
			struct timeval overtime;
			FD_ZERO(&writefd);
			FD_SET(tcpOne.Socketfd, &writefd);
			overtime.tv_sec = 0;//
			overtime.tv_usec = 3000; //微妙级
			iRes = select(tcpOne.Socketfd + 1, 0, &writefd, 0, &overtime);
			if (0 < iRes){
				if (FD_ISSET(tcpOne.Socketfd, &writefd)){
					//有效
					sprintf_s(m_Logbuf, "TCP_Connect | ClientSocket[%d].Socketfd = %d connect Select Success", i, tcpOne.Socketfd);
					LogPrint(tcpOne.ServerIP, m_Logbuf);
					//成功后将其设置为阻塞模式
					ul = 0;
					iRes = ioctlsocket(tcpOne.Socketfd, FIONBIO, (unsigned long*)&ul);
					//更该状态
					tcpOne.State = TCP_CLIENTSOCKET_STATE_CONN;
					//*pSocketfd = pClientSocket->Socketfd;
					m_mapClients.insert(pair<string, TCP_CLIENTSOCKT_TS>(strKey, tcpOne));
					iRes = TCP_ERR_OK;
				}
				else{
					//无效
					iRes = WSAGetLastError();
					sprintf_s(m_Logbuf, "TCP_Connect | ClientSocket[%d].Socketfd = %d connect Select Failed ErrCode = %d!", i, tcpOne.Socketfd, iRes);
					LogPrint(tcpOne.ServerIP, m_Logbuf);

					//关闭socket
					closesocket(tcpOne.Socketfd);
					//将socket送回clientsocket池中去，即释放空间

					//memset(pClientSocket, 0, sizeof(TCP_CLIENTSOCKT_T));

					tcpOne.State = TCP_CLIENTSOCKET_STATE_IDLE;
					//返回值为 TCP IO错误
					iRes = TCP_ERR_CONNECT_IOCTL;
				//	break;
				}
			}
			else{//超时处理
				iRes = WSAGetLastError();

				sprintf_s(m_Logbuf, "TCP_Connect | ClientSocket[%d].Socketfd = %d connect Select Overtime ErrCode = %d  ！", i, tcpOne.Socketfd, iRes);
				LogPrint(tcpOne.ServerIP, m_Logbuf);
				//关闭socket
				closesocket(tcpOne.Socketfd);

				//将socket送回clientsocket池中去，即释放空间
				//memset(pClientSocket, 0, sizeof(TCP_CLIENTSOCKT_T));
				tcpOne.State = TCP_CLIENTSOCKET_STATE_IDLE;

				iRes = TCP_ERR_CONNECT_OUTTIME;
				//break;
			}//select不成功
		}//连接不成功
	}
	else{//设定非阻塞不成功
		iRes = WSAGetLastError();
		//
		sprintf_s(m_Logbuf, "TCP_Connect | ClientSocket[%d].Socketfd = %d ioctlsocket Failed ErrCode = %d!", i, tcpOne.Socketfd, iRes);
		LogPrint(tcpOne.ServerIP, m_Logbuf);
		//
		closesocket(tcpOne.Socketfd);//每次调用完socket之后都应该进行释放

		//将socket送回clientsocket池中去，即释放空间
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
		// 找到了
		*pSocketfd = iter->second.Socketfd;
	}
	else{
		// 没有找到
		if (Connect(IP, nPort) == TCP_ERR_OK){
			//
			printf("............................添加新的客户端..................");
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
		// 找到了
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
		overtime.tv_sec = 1;			 //秒级
		overtime.tv_usec = 3000; //微妙级
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
				else{//发送不成功
					//
					sprintf_s(m_Logbuf, "TCP_Send | Socketfd = %d,Send Finish wait select overtime", Socketfd);
					LogPrint(NULL, m_Logbuf);
					//
					iRes = TCP_ERR_SEND;
				}
			}
			else{//没有有效的socket
				//
				sprintf_s(m_Logbuf, "TCP_Send | Socketfd = %d,Send Finish wait select overtime", Socketfd);
				LogPrint(NULL, m_Logbuf);
				iRes = TCP_ERR_SEND_OUTTIME;
			}
		}
		else{//没有有效的socket
			//
			sprintf_s(m_Logbuf, "TCP_Send | Socketfd = %d,Wait Send State Select overtime.", Socketfd);
			LogPrint(NULL, m_Logbuf);
			//			
			iRes = TCP_ERR_SEND_OUTTIME;
		}
	}
	else{//socket为null
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
	int TimeOut = 2000; //1秒

	*pRecvDataCount = 0;
	//
	if (NULL != Socketfd){
	//	int nRecvBufLen = PACKAGE_SIZE_MAX; //设置为32K
		int nRecvBufLen = PACKAGE_SIZE_MAX; //设置为32K
		setsockopt(Socketfd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBufLen, sizeof(int));

		FD_ZERO(&readfd);
		FD_SET(Socketfd, &readfd);
		overtime.tv_sec = 0;			 //秒级
		overtime.tv_usec = 100; //微妙级
		iRes = select(Socketfd + 1, &readfd, 0, 0, &overtime);
		if (0 < iRes){
			if (FD_ISSET(Socketfd, &readfd)){
				//接收数据 默认缓冲区为8k 缓冲区pRecvBuf的设定要大于8k
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
			else{//选择超时
				//
				/*printf("readfd is not the member \r\n" );*/
				sprintf_s(m_Logbuf, "TCP_Recive | Socketfd = %d,Recviv Select overtime! ", Socketfd);
				LogPrint(NULL, m_Logbuf);
				//
				iRes = TCP_ERR_RECIVE_OUTTIME;
			}
		}
		else{//选择超时
			//
			sprintf_s(m_Logbuf, "TCP_Recive | Socketfd = %d,Recviv overtime!", Socketfd);
			LogPrint(NULL, m_Logbuf);
			/*printf("receive SELECT 超时 is %d\r\n", iRes);*/
			//
			iRes = TCP_ERR_RECIVE_OUTTIME;
		}
	}
	else{//soket为空
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
