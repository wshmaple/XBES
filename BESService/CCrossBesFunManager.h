#pragma once
#include "tcpSocketHead.h"
#include "TcpSocket.h"

#include "CComFun.h"
class CCrossBesFunManager
{
public:
	CCrossBesFunManager();
	~CCrossBesFunManager();
	int BES200_Communication2(const char ServerIP[], int nPort, const char *pSendData, int SendDataSize, char *pRecvData, int *RecvDataSize/*, char **errmsg*/);
	CTcpSocket m_tcpSocket;
	CComFun m_comFun;
};



