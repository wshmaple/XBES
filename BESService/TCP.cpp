#include "stdafx.h"
#include "tchar.h"
#include "time.h"
#include "io.h"
#include "TCP.h"

/*构造函数*/
CTCP::CTCP()
{
	/*使用Socket的程序在使用Socket之前必须调用WSAStartup函数。该函数的第一个参数指明程序请求使用的Socket版本，
	其中高位字节指明副版本、低位字节指明主版本；操作系统利用第二个参数返回请求的Socket的版本信息。当一个应用
	程序调用WSAStartup函数时，操作系统根据请求的Socket版本来搜索相应的Socket库，然后绑定找到的Socket库到该应
	用程序中。以后应用程序就可以调用所请求的Socket库中的其它Socket函数了。该函数执行成功后返回0.
	*/
	WSADATA		wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) == 0)
	{
		SocketOK = true;
	}
	else
	{
		SocketOK = false;
	}

	for (int i = 0; i<CLIENTSOCKET_MAX; ++i)
	{
		ClientSocket[i].Socketfd = 0;
		memset(ClientSocket[i].ServerIP, 0, sizeof(ClientSocket[i].ServerIP));
		ClientSocket[i].State = E_TCP_CLIENTSOCKET_STATE_IDLE;
		ClientSocket[i].ErrCount = 0;
	}
	InitializeCriticalSection(&cs);
	InitializeCriticalSection(&ss);
}

/*析构函数*/
CTCP::~CTCP()
{
	for (int i = 0; i<CLIENTSOCKET_MAX; ++i)
	{
		if (NULL != ClientSocket[i].State == E_TCP_CLIENTSOCKET_STATE_IDLE)
		{
			DisConnect(i);
		}
	}
	WSACleanup();
	DeleteCriticalSection(&cs);
	DeleteCriticalSection(&ss);
}

/*查找空闲的连接*/
TCP_CLIENTSOCKT_T* CTCP::FindClientSocketIDLE(void)
{
	int i;
	TCP_CLIENTSOCKT_T* pResult = NULL;

	for (i = 0; i < CLIENTSOCKET_MAX; i++)
	{
		if (ClientSocket[i].State == E_TCP_CLIENTSOCKET_STATE_IDLE)
		{
			pResult = &ClientSocket[i];
			break;
		}
	}
	return pResult;
}

/*根据IP查找正在使用的连接*/
TCP_CLIENTSOCKT_T* CTCP::FindClientSocketByIP(const char *ServerIP)
{
	int i;
	TCP_CLIENTSOCKT_T *pResult = NULL;

	for (i = 0; i < CLIENTSOCKET_MAX; i++)
	{
		if (ClientSocket[i].State == E_TCP_CLIENTSOCKET_STATE_CONN&&\
			strcmp(ClientSocket[i].ServerIP, ServerIP) == 0)
		{
			pResult = &ClientSocket[i];
			break;
		}
	}
	return pResult;
}

/*根据ID（下标）来查找TCP连接*/
TCP_CLIENTSOCKT_T* CTCP::FindClientSocketByIndex(int Index)
{
	TCP_CLIENTSOCKT_T *pResult = NULL;

	if (Index>-1 && Index<CLIENTSOCKET_MAX&&\
		ClientSocket[Index].State == E_TCP_CLIENTSOCKET_STATE_CONN)
		pResult = &ClientSocket[Index];

	return pResult;
}

/*建立TCP连接，返回连接的下标*/
int CTCP::Connect(const char IP[], int Timeout)
{
	int ret = -1;
	TCP_CLIENTSOCKT_T *pClientSocket = NULL;
//	SOCKET SocketFd;

	/*从连接池中查找，查找到直接返回*/
	pClientSocket = FindClientSocketByIP(IP);
	if (pClientSocket != NULL)
		return (pClientSocket - &ClientSocket[0]);


	/*连接池中没有查找到连接，创建连接*/
	/*SOCKET状态不对，无法使用SOCKET*/
	if (!SocketOK)
		return E_TCP_ERR_SOCKET_ERR;

	EnterCriticalSection(&cs);
	pClientSocket = FindClientSocketIDLE();
	if (pClientSocket != NULL)
		pClientSocket->State = E_TCP_CLIENTSOCKET_STATE_CONN;
	LeaveCriticalSection(&cs);

	if (pClientSocket == NULL)
		return E_TCP_ERR_CONNECT_MAX;

	pClientSocket->Socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == pClientSocket->Socketfd)
	{
		ret = E_TCP_ERR_SOCKET_ERR;
		goto out;
	}

	struct sockaddr_in	ServerSocketAddr;
	strcpy_s(pClientSocket->ServerIP, IP);
	pClientSocket->ErrCount = 0;
	ServerSocketAddr.sin_family = AF_INET;
	ServerSocketAddr.sin_addr.s_addr = inet_addr(IP);
	ServerSocketAddr.sin_port = htons(TCP_SERVER_PORT);

	u_long ul = 1; //设置非阻塞方式连接
	ioctlsocket(pClientSocket->Socketfd, FIONBIO, (unsigned long*)&ul);

	if (connect(pClientSocket->Socketfd, (const struct sockaddr *)&ServerSocketAddr, sizeof(struct sockaddr_in)) != 0)
	{
		struct timeval overtime;
		fd_set writefd;
		FD_ZERO(&writefd);
		FD_SET(pClientSocket->Socketfd, &writefd);
		overtime.tv_sec = 0;
		overtime.tv_usec = Timeout * 1000; //微妙级
		if (select(pClientSocket->Socketfd + 1, 0, &writefd, 0, &overtime) > 0)
		{
			int len = sizeof(int);
			int error = -1;

			if (!FD_ISSET(pClientSocket->Socketfd, &writefd))
			{
				ret = E_TCP_ERR_CONNECT_TIMEOUT;
				goto out;
			}
		}
		else
		{
			ret = E_TCP_ERR_CONNECT_TIMEOUT;
			goto out;
		}
	}

	/*连接成功，将连接设置回阻塞模式*/
	ul = 0; //设置非阻塞方式连接
	ioctlsocket(pClientSocket->Socketfd, FIONBIO, (unsigned long*)&ul);
	return (pClientSocket - &ClientSocket[0]);

out:
	closesocket(pClientSocket->Socketfd);//必须关闭连接
	EnterCriticalSection(&cs);
	memset(pClientSocket, 0, sizeof(TCP_CLIENTSOCKT_T));
	pClientSocket->State = E_TCP_CLIENTSOCKET_STATE_IDLE;
	LeaveCriticalSection(&cs);
	return ret;
}

int CTCP::Connect2(const char IP[], int nPort, int Timeout)
{
	int ret = -1;
	TCP_CLIENTSOCKT_T* pClientSocket = NULL;
	//	SOCKET SocketFd;

		/*从连接池中查找，查找到直接返回*/
	pClientSocket = FindClientSocketByIP(IP);
	if (pClientSocket != NULL)
		return (pClientSocket - &ClientSocket[0]);


	/*连接池中没有查找到连接，创建连接*/
	/*SOCKET状态不对，无法使用SOCKET*/
	if (!SocketOK)
		return E_TCP_ERR_SOCKET_ERR;

	EnterCriticalSection(&cs);
	pClientSocket = FindClientSocketIDLE();
	if (pClientSocket != NULL)
		pClientSocket->State = E_TCP_CLIENTSOCKET_STATE_CONN;
	LeaveCriticalSection(&cs);

	if (pClientSocket == NULL)
		return E_TCP_ERR_CONNECT_MAX;

	pClientSocket->Socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == pClientSocket->Socketfd)
	{
		ret = E_TCP_ERR_SOCKET_ERR;
		goto out;
	}

	struct sockaddr_in	ServerSocketAddr;
	strcpy_s(pClientSocket->ServerIP, IP);
	pClientSocket->ErrCount = 0;
	ServerSocketAddr.sin_family = AF_INET;
	ServerSocketAddr.sin_addr.s_addr = inet_addr(IP);
	ServerSocketAddr.sin_port = htons(nPort);

	u_long ul = 1; //设置非阻塞方式连接
	ioctlsocket(pClientSocket->Socketfd, FIONBIO, (unsigned long*)& ul);

	if (connect(pClientSocket->Socketfd, (const struct sockaddr*) & ServerSocketAddr, sizeof(struct sockaddr_in)) != 0)
	{
		struct timeval overtime;
		fd_set writefd;
		FD_ZERO(&writefd);
		FD_SET(pClientSocket->Socketfd, &writefd);
		overtime.tv_sec = 0;
		overtime.tv_usec = Timeout * 1000; //微妙级
		if (select(pClientSocket->Socketfd + 1, 0, &writefd, 0, &overtime) > 0)
		{
			int len = sizeof(int);
			int error = -1;

			if (!FD_ISSET(pClientSocket->Socketfd, &writefd))
			{
				ret = E_TCP_ERR_CONNECT_TIMEOUT;
				goto out;
			}
		}
		else
		{
			ret = E_TCP_ERR_CONNECT_TIMEOUT;
			goto out;
		}
	}

	/*连接成功，将连接设置回阻塞模式*/
	ul = 0; //设置非阻塞方式连接
	ioctlsocket(pClientSocket->Socketfd, FIONBIO, (unsigned long*)& ul);
	return (pClientSocket - &ClientSocket[0]);

out:
	closesocket(pClientSocket->Socketfd);//必须关闭连接
	EnterCriticalSection(&cs);
	memset(pClientSocket, 0, sizeof(TCP_CLIENTSOCKT_T));
	pClientSocket->State = E_TCP_CLIENTSOCKET_STATE_IDLE;
	LeaveCriticalSection(&cs);
	return ret;
}


int CTCP::DisConnect(int Index)
{
	int ret = -1;
	if (Index > -1 && Index < E_TCP_ERR_CONNECT_MAX)
	{
		if (ClientSocket[Index].State != E_TCP_CLIENTSOCKET_STATE_IDLE)
		{
			shutdown(ClientSocket[Index].Socketfd, 2);
			closesocket(ClientSocket[Index].Socketfd);
		}
		memset(&ClientSocket[Index], 0, sizeof(TCP_CLIENTSOCKT_T));
		ClientSocket[Index].State = E_TCP_CLIENTSOCKET_STATE_IDLE;
		ret = 0;
	}
	else
		ret = -1;
	return ret;
}

int CTCP::DisConnect(const char IP[])
{
	TCP_CLIENTSOCKT_T *pClientSocket = NULL;
	int ret = -1;;

	pClientSocket = FindClientSocketByIP(IP);

	LOGI("-----------------------------DisConnect:" << IP);

	if (pClientSocket == NULL)
		ret = -1;
	else
	{
		shutdown(pClientSocket->Socketfd, 2);
		closesocket(pClientSocket->Socketfd);
		memset(pClientSocket, 0, sizeof(TCP_CLIENTSOCKT_T));
		pClientSocket->State = E_TCP_CLIENTSOCKET_STATE_IDLE;
		ret = 0;
	}
	return ret;
}

int CTCP::Send(int Index, void *pSendData, int SendDataSize, int Timeout)
{
	SOCKET s;
	int ret = -1;

	if (Index > -1 && Index < CLIENTSOCKET_MAX&&\
		ClientSocket[Index].State == E_TCP_CLIENTSOCKET_STATE_CONN)
	{
		s = ClientSocket[Index].Socketfd;
		int TimeOut = Timeout;
		if (setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char *)&TimeOut, sizeof(TimeOut)) == SOCKET_ERROR)
		{
			ret = -1;
		}
		else
			ret = send(s, (const char*)pSendData, SendDataSize, 0);
	}
	else
	{
		ret = -1;
	}
	return ret;
}

int CTCP::Send(const char IP[], void *pSendData, int SendDataSize, int Timeout)
{
	SOCKET s;
	TCP_CLIENTSOCKT_T *pClientSocket = NULL;
	int ret = -1;

	pClientSocket = FindClientSocketByIP(IP);
	if (pClientSocket == NULL)
		return -1;

	s = pClientSocket->Socketfd;

	int TimeOut = Timeout;
	if (setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char *)&TimeOut, sizeof(TimeOut)) == SOCKET_ERROR)
	{
		ret = -1;
	}
	else
		ret = send(s, (const char*)pSendData, SendDataSize, 0);

	return ret;
}


int CTCP::Recv(int Index, void *pRecvData, int RecvDataSize, int Timeout /* = DEFAULT_RECV_TIMEOUT */)
{
	SOCKET s;
	int ret = -1;

	if (Index > -1 && Index < CLIENTSOCKET_MAX&&\
		ClientSocket[Index].State == E_TCP_CLIENTSOCKET_STATE_CONN)
	{
		s = ClientSocket[Index].Socketfd;
		int TimeOut = Timeout;
		if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&TimeOut, sizeof(TimeOut)) == SOCKET_ERROR)
		{
			ret = -1;
		}
		else
			ret = recv(s, (char *)pRecvData, RecvDataSize, 0);
	}
	return ret;
}

int CTCP::Recv(const char IP[], void *pRecvData, int RecvDataSize, int Timeout /* = DEFAULT_RECV_TIMEOUT */)
{
	SOCKET s;
	TCP_CLIENTSOCKT_T *pClientSocket = NULL;
	int ret = -1;

	pClientSocket = FindClientSocketByIP(IP);
	if (pClientSocket == NULL)
		return -1;

	s = pClientSocket->Socketfd;

	int TimeOut = Timeout;
	if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&TimeOut, sizeof(TimeOut)) == SOCKET_ERROR)
	{
		ret = -1;
	}
	else
	{
		ret = recv(s, (char *)pRecvData, RecvDataSize, 0);
		if (ret == SOCKET_ERROR)
		{
			/*int err1;
			char cc1[30];
			err1 = WSAGetLastError();
			sprintf(cc1, "D:\\recv%d.txt", err1);
			FILE *fp = fopen(cc1, "a+");
			if (fp != NULL)
			fclose(fp);*/
			//printf("recv err :%d\n", WSAGetLastError());
			return -1;
		}
	}


	return ret;
}



