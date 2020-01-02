#pragma once

#ifndef _TCPSOCKET_H_
#define _TCPSOCKET_H_

#include "Logs.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <map>
using namespace std;

/*SOCKET���ӵĲ�����Ϣ*/
typedef	struct _TCP_SOCK_CLIENTS_{
	char				ServerIP[20];			//�ַ������͵�IP��ַ����ʽ�磺��192.168.1.1��
	SOCKET				Socketfd;				//SOCKET��fd����
	DWORD				TimeOut;				//��ʱ����ʱ��
	int					State;					//��ǰ������״̬������ȡֵTCP_CLIENTSOCKET_STATE_IDLE��TCP_CLIENTSOCKET_STATE_DISCONN��TCP_CLIENTSOCKET_STATE_CONN
	int					ErrCount;				//�������
	struct sockaddr_in	ServerSocketAddr;		//SOCKET������ʹ�õı�׼IP��ʽ
	int					nPort;					//Socket ���ӵĶ˿�
	int					SocketState;			//��ǰOCKET��״̬
}TCP_CLIENTSOCKT_TS;

//WSAAsyncSelect
//CAsyncSelectCom
class CTcpSocket
{
public:
	CTcpSocket();
	~CTcpSocket();

public:

	int SendMsg(SOCKET socketfd, const char SendBuf[], int SendDataCount);// ��������
	int RecvMsg(SOCKET socketfd, char *pRecvBuf, int *pRecvDataCount);// ��������



	//************************************
	// Method:    SendMsg
	// FullName:  CTcpSocket::SendMsg
	// Access:    public 
	// Returns:   int						// ���� ��ʶ
	// Qualifier:
	// Parameter: const char ServerIP[]		// IP
	// Parameter: int nPort					// �˿�	
	// Parameter: const char * pSendData	// ������������
	// Parameter: int SendDataSize			// ���ͳ���
	// Parameter: char * pRecvData			// ���յ�ַ
	// Parameter: int * RecvDataSize		// �������ݳ���
	// Parameter: char * * errmsg			// ������Ϣ
	//************************************
	int SendMsg(const char ServerIP[], int nPort, const char *pSendData, int nSendDataSize, char *pRecvData, int *nRecvDataSize, char **errmsg);// ʵ������������ô�õ�

protected:
	int Connect(const char IP[], int nPort);// ����

	int ReciveMsg(const char IP[], int nPort);// ��������

	void Disconnect(const char IP[], int nPort);// �Ͽ�����


protected:
	//CLogs m_logs; // ��־�ļ�����

	map <string ,TCP_CLIENTSOCKT_TS > m_mapClients;


	int	SocketState;//����SOCKET��״̬

//	TCP_CLIENTSOCKT_T ClientSocket[CLIENTSOCKET_MAX];//���SOCKET������


	char m_Logbuf[512];//������Ϣ

	CRITICAL_SECTION cs;
public:
	/*����TCP������*/
	int  TCP_Connect(const char IP[],int nPort, SOCKET *pSocketfd);
	/*�Ͽ�TCP����*/
	void TCP_Disconnect(SOCKET Socketfd);
	/*�Ͽ�TCP����*/
	void TCP_Disconnect(const char IP[], int nPort);
	/*��������*/
	int  TCP_Send(SOCKET Socketfd, const char SendBuf[], int SendDataCount);
	/*��������*/
	int  TCP_Recive(SOCKET Socketfd, char *pRecvBuf, int *pRecvDataCount);
	/*������־�ļ�*/
	int LogPrint(char *pLogAddress, char *pLogString);
};
#endif
