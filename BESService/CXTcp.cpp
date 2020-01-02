
#include "stdafx.h"
#include "CXTcp.h"

#include <stdio.h>
//#pragma comment(lib, "ws2_32.lib")

CXTcp::CXTcp()
{
	m_bOnline = false;
	m_bConnect = false;
	m_bckonline = true;
	m_nPort = 6060;
	m_nReTryCounts = 100;
	memset(m_cIP, 0, 255);
}


CXTcp::~CXTcp()
{
	DisConnect();
}
struct TCP_KEEPALIVE {
	unsigned long onoff;
	unsigned long keepalivetime;
	unsigned long keepaliveinterval;
};

#define SIO_KEEPALIVE_VALS _WSAIOW(IOC_VENDOR,4) 
/* KeepAlive实现 */
void set_keepalive(SOCKET s)
{
	BOOL bKeepAlive = TRUE;
	int nRet = ::setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char*)&bKeepAlive, sizeof(bKeepAlive));
	if (nRet == SOCKET_ERROR)
	{
		return;
	}
	/* 输入参数 */
	struct TCP_KEEPALIVE inKeepAlive = { 0 };
	unsigned long ulInLen = sizeof(struct TCP_KEEPALIVE);

	/* 输出参数 */
	struct TCP_KEEPALIVE outKeepAlive = { 0 };
	unsigned long ulOutLen = sizeof(struct TCP_KEEPALIVE);
	unsigned long ulBytesReturn = 0;
	int ret = 0;

	/* 设置keepalive 为5秒，并且发送次数为3次 */
	inKeepAlive.onoff = 1;
	/* 2次keepalive探测间隔时间 */
	inKeepAlive.keepaliveinterval = 500;
	/* 开始首次keepalive探测前的tcp空闲时间 */
	inKeepAlive.keepalivetime = 500;

	ret = WSAIoctl((unsigned int)s,
		SIO_KEEPALIVE_VALS,
		(LPVOID)&inKeepAlive,
		ulInLen,
		(LPVOID)&outKeepAlive,
		ulOutLen,
		&ulBytesReturn,
		NULL,
		NULL);
	if (ret == SOCKET_ERROR)
	{
		printf("error: %d\n", WSAGetLastError());
	}
}

int CXTcp::Connect(const char IP[], int nPort, int Timeout)
{
	m_nPort = nPort;
	m_bOnline = false;
	m_bConnect = false;
	memcpy(m_cIP, IP, strlen(IP));
	int ret = 0;
	struct sockaddr_in	ServerSocketAddr;
	ServerSocketAddr.sin_family = AF_INET;
	ServerSocketAddr.sin_addr.s_addr = inet_addr(IP);
	ServerSocketAddr.sin_port = htons(m_nPort);
	u_long ul = 1; //设置非阻塞方式连接
	//创建套接字
	if ((m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0)
	{
		return -1;
	}
	ioctlsocket(m_socket, FIONBIO, (unsigned long*)&ul);
	if (connect(m_socket, (const struct sockaddr *)&ServerSocketAddr, sizeof(struct sockaddr_in)) != 0)
	{
		struct timeval overtime;
		fd_set writefd;
		FD_ZERO(&writefd);
		FD_SET(m_socket, &writefd);
		overtime.tv_sec = 0;
		overtime.tv_usec = Timeout * 1000; //微妙级
		if (select(m_socket + 1, 0, &writefd, 0, &overtime) > 0)
		{
			int len = sizeof(int);
			int error = -1;
			if (!FD_ISSET(m_socket, &writefd))
			{
				ret = -3;	
				return ret;
			}
		}
		else
		{
			ret = -3;
			return ret;
		}
	}
	/*连接成功，将连接设置回阻塞模式*/
	ul = 0; //设置阻塞方式连接
	ret = ioctlsocket(m_socket, FIONBIO, (unsigned long*)&ul);
	m_bOnline = true;
	m_bConnect = true;
	set_keepalive(m_socket);
	return ret;
}



int CXTcp::DisConnect()
{
	if (m_socket>0)
	{
		shutdown(m_socket, 2);
		closesocket(m_socket);
	}
	else return 0;
	return 1;
}

int CXTcp::ReSend(void *pSendData, int SendDataSize, int Timeout /*= 100*/)
{
	
	int nFlag = 0;
	nFlag = Send(pSendData, SendDataSize, Timeout);
	if (nFlag==0 || nFlag == -1)
	{
		// 没有发送成功啊
		// m_bConnect = false;
		DWORD dwError = GetLastError();
	}
	else{
		return nFlag;
	}
	return nFlag;
}

int CXTcp::ReRecv(void * pRecvData, int RecvDataSize, int Timeout)
{
	int nFlag = 0;
	int nLen;
	nFlag = Recv(pRecvData, RecvDataSize, Timeout);
	nLen = nFlag;
	if (nLen == 0 || nLen == -1)
	{
		// 没有接收成功啊
		DWORD dwError = WSAGetLastError();
		if (EINTR == dwError)
		{
			printf("ReRecv socket error close");
		}
	}else{
		return nLen;
	}
	return nLen;
}

int CXTcp::Send(void *pSendData, int SendDataSize, int Timeout /*= DEFAULT_SEND_TIMEOUT*/)
{
	if (XGetLastSendError()==1)
	{
		int TimeOut = Timeout;
		int ret = 0;
		if (setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&TimeOut, sizeof(TimeOut)) == SOCKET_ERROR)
		{
			ret = -1;
		}
		else
		{
		//	printf("开始发送数据:\n");
			ret = send(m_socket, (const char*)pSendData, SendDataSize, 0);
		}
		return ret;
	}
	else{
		return -1;
	}

	


}

int CXTcp::Recv(void *pRecvData, int RecvDataSize, int Timeout /*= DEFAULT_RECV_TIMEOUT*/)
{
	if (XGetLastRecvError() == 1)
	{
		int TimeOut = Timeout;
		int ret = 0;
		if (setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&TimeOut, sizeof(TimeOut)) == SOCKET_ERROR)
		{
			ret = -1;
		}
		else
		{
		//	printf("开始接收数据");
			ret = recv(m_socket, (char *)pRecvData, RecvDataSize, 0);
			if (ret < 0) {
				int nErrorCode = WSAGetLastError();
				if (nErrorCode == EINTR) {
					printf("接收为 0 ，但是依然保持连接\n");
				}
			}
		}
			
		return ret;
	}
	else{
		return -1;
	}
	
}



void CXTcp::XReSend(void *pSendData, int SendDataSize, int &nLen, int Timeout /*= 100*/)
{
	nLen = ReSend(pSendData, SendDataSize, Timeout);
}

void CXTcp::XReRecv(void *pRecvData, int RecvDataSize, int &nLen, int Timeout /*= 100*/)
{
	nLen = ReRecv(pRecvData, RecvDataSize, Timeout);
}

int CXTcp::XGetLastSendError()
{
	fd_set fds;
	int nReady;
	timeval timeout;

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	int nErrorCode = 0;
	FD_ZERO(&fds);
	FD_SET(m_socket, &fds);
	nReady = select(0, NULL, &fds, NULL, &timeout);

	if(nReady > 0) {
		return 1;
	}	
	else if	(0 == nReady) {
	//	printf("发送-select等待超时\n");
		return 2;
	}
	else if (nReady < 0) {
		// 风雪是你，感动是你，这世界我来了，任凭风霜，学我，爱是你我。
	//	printf("发送-select错误\n"); 
		return 0;
	}
	//8700
	//1+1+2.5+3 = 7.5
	//1200
	return 0;
}

int CXTcp::XGetLastRecvError()
{
	fd_set fds;
	int nReady;
	timeval timeout;

	timeout.tv_sec = 4;
	timeout.tv_usec = 0;
	int nErrorCode = 0;
	FD_ZERO(&fds);
	FD_SET(m_socket, &fds);
	nReady = select(0, &fds, NULL, NULL, &timeout);
	if (0 == nReady) {
		//printf("接收-select等待超时\n");
		return 2;
	}
	else if (nReady < 0) {
		//printf("接收-select错误\n");
		return 0;
	}
	else if (nReady > 0) {
		return 1;
	}
	return 0;
}

bool CXTcp::CheckOnline()
{
	while (m_bConnect)
	{
		Sleep(1000);
	}
	return 0;
}
