#pragma once
#include <winsock2.h>
class CXTcp
{
public:
	CXTcp();
	~CXTcp();

	int Connect(const char IP[],int nPort=6060, int Timeout = 100);//����TCP���ӣ�ָ����ʱʱ��(ms)
	int DisConnect();//����IP��ַ�Ͽ�����
	

	int ReSend(void *pSendData, int SendDataSize, int Timeout = 100); // ��������
	int ReRecv(void *pRecvData, int RecvDataSize, int Timeout = 100); // ���������
	//


	int Send(void *pSendData, int SendDataSize, int Timeout = 100);
	int Recv(void *pRecvData, int RecvDataSize, int Timeout = 100);


	void XReSend(void *pSendData, int SendDataSize,int &nLen, int Timeout = 100); // ��������
	void XReRecv(void *pRecvData, int RecvDataSize, int &nLen, int Timeout = 100); // ���������

	

private:

	SOCKET m_socket;
	char m_cIP[255];
	int m_nPort;
	
public:
	int XGetLastSendError();
	int XGetLastRecvError();
	bool m_bConnect; // ��ǰ������״̬
	bool m_bOnline; // �ǲ��� �������ӹ�

	//
private:
	// test
	bool CheckOnline();
	bool m_bckonline;
	int m_nReTryCounts;	
};

