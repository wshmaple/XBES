#pragma once
#include <winsock2.h>
class CXTcp
{
public:
	CXTcp();
	~CXTcp();

	int Connect(const char IP[],int nPort=6060, int Timeout = 100);//建立TCP连接，指定超时时间(ms)
	int DisConnect();//根据IP地址断开连接
	

	int ReSend(void *pSendData, int SendDataSize, int Timeout = 100); // 重连后发送
	int ReRecv(void *pRecvData, int RecvDataSize, int Timeout = 100); // 重连后接收
	//


	int Send(void *pSendData, int SendDataSize, int Timeout = 100);
	int Recv(void *pRecvData, int RecvDataSize, int Timeout = 100);


	void XReSend(void *pSendData, int SendDataSize,int &nLen, int Timeout = 100); // 重连后发送
	void XReRecv(void *pRecvData, int RecvDataSize, int &nLen, int Timeout = 100); // 重连后接收

	

private:

	SOCKET m_socket;
	char m_cIP[255];
	int m_nPort;
	
public:
	int XGetLastSendError();
	int XGetLastRecvError();
	bool m_bConnect; // 当前的连接状态
	bool m_bOnline; // 是不是 曾经连接过

	//
private:
	// test
	bool CheckOnline();
	bool m_bckonline;
	int m_nReTryCounts;	
};

