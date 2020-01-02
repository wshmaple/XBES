#ifndef _TCP_H_
#define _TCP_H_

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define TCP_TRYTIMES 2
/*TCPͨ�Ŷ˿ڶ���*/
#define TCP_SERVER_PORT  1204

/*Ĭ��TCP���ӳ�ʱ�ȴ�ʱ��*/
#define DEFAULT_CONNECT_TIMEOUT 3000

/*Ĭ��TCP���ͳ�ʱ�ȴ�ʱ��*/
#define DEFAULT_SEND_TIMEOUT 3000

/*Ĭ��TCP���ճ�ʱ�ȴ�ʱ��*/
#define DEFAULT_RECV_TIMEOUT 3000

/*����ͬʱ���ڵ�SOCKET������*/
#define CLIENTSOCKET_MAX  5

#define DEFAULT_RECVBUF_SIZE 10240

/*SOCKETͨ���п��ܳ��ֵĴ������*/
typedef enum
{
	E_TCP_ERR_OK = 0,//�޴���
	E_TCP_ERR_SOCKET_ERR = -1,//����SOCKET����
	E_TCP_ERR_CONNECT = -2,//����ʧ��
	E_TCP_ERR_CONNECT_TIMEOUT = -3,//����ʧ��,��ʱ
	E_TCP_ERR_SEND = -4,//����ʧ��
	E_TCP_ERR_RECIVE = -5,//����ʧ��
	E_TCP_ERR_SEND_OUTTIME = -6,//���ͳ�ʱ
	E_TCP_ERR_RECIVE_OUTTIME = -7,//���ճ�ʱ
	E_TCP_ERR_SEND_EXCEPT = -8,//����ʧ�ܣ�����δ֪����
	E_TCP_ERR_RECIVE_EXCEPT = -9,//����ʧ�ܣ�����δ֪����
	E_TCP_ERR_CONNECT_MAX = -10,//ͬʱ�������ﵽ���
}E_TCP_ERR_T;

/*SOCKETӦ�õ�״̬*/
typedef enum
{
	E_TCP_SOCKET_STATUS_ERR = 0,//����
	E_TCP_SOCKET_STATUS_OK,//����
}E_TCP_SOCKET_STATE_T;

/*SOCKET���ӵ�״̬*/
typedef enum
{
	E_TCP_CLIENTSOCKET_STATE_IDLE = 0,//����
	E_TCP_CLIENTSOCKET_STATE_DISCONN,//���ӶϿ�
	E_TCP_CLIENTSOCKET_STATE_CONN,//��������
}E_TCP_CLIENTSOCKET_STATE_T;

/*SOCKET���ӵĲ�����Ϣ*/
typedef	struct _TCP_SOCK_CLIENT_{
	char				ServerIP[20];//�ַ������͵�IP��ַ����ʽ�磺��192.168.1.1��
	SOCKET				Socketfd;//SOCKET��fd����
	DWORD				TimeOut;//��ʱ����ʱ��
	int					State;//��ǰ������״̬������ȡֵE_TCP_CLIENTSOCKET_STATE_IDLE��E_TCP_CLIENTSOCKET_STATE_DISCONN��E_TCP_CLIENTSOCKET_STATE_CONN
	int					ErrCount;//�������
	int                 nPort; // �˿ں�
}TCP_CLIENTSOCKT_T;


class CTCP
{
public:
	CTCP();
	~CTCP();
	bool	SocketOK;//SOCKET��״̬

	/*�����Ѿ������õ�����*/
	TCP_CLIENTSOCKT_T* FindClientSocketByIP(const char *ServerIP);//ͨ��IP����TCP����
	TCP_CLIENTSOCKT_T* FindClientSocketByIndex(int Index);

	/*���ҿ��е�����*/
	TCP_CLIENTSOCKT_T* FindClientSocketIDLE(void);

	int Connect(const char IP[], int Timeout = DEFAULT_CONNECT_TIMEOUT);//����TCP���ӣ�ָ����ʱʱ��(ms)
	int Connect2(const char IP[],int nPort, int Timeout = DEFAULT_CONNECT_TIMEOUT);//����TCP���ӣ�ָ����ʱʱ��(ms)
	int DisConnect(const char IP[]);//����IP��ַ�Ͽ�����
	int DisConnect(int Index);//�������ӵ����кŶϿ�����

	int Send(const char IP[], void *pSendData, int SendDataSize, int Timeout = DEFAULT_SEND_TIMEOUT);
	int Send(int Index, void *pSendData, int SendDataSize, int Timeout = DEFAULT_SEND_TIMEOUT);
	int Recv(const char IP[], void *pRecvData, int RecvDataSize, int Timeout = DEFAULT_RECV_TIMEOUT);
	int Recv(int Index, void *pRecvData, int RecvDataSize, int Timeout = DEFAULT_RECV_TIMEOUT);

	CRITICAL_SECTION cs;
	TCP_CLIENTSOCKT_T	ClientSocket[CLIENTSOCKET_MAX];//���SOCKET������
	CRITICAL_SECTION ss;
};

#endif
