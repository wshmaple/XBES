#ifndef _TCP_H_
#define _TCP_H_

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define TCP_TRYTIMES 2
/*TCP通信端口定义*/
#define TCP_SERVER_PORT  1204

/*默认TCP连接超时等待时间*/
#define DEFAULT_CONNECT_TIMEOUT 3000

/*默认TCP发送超时等待时间*/
#define DEFAULT_SEND_TIMEOUT 3000

/*默认TCP接收超时等待时间*/
#define DEFAULT_RECV_TIMEOUT 3000

/*允许同时存在的SOCKET连接数*/
#define CLIENTSOCKET_MAX  5

#define DEFAULT_RECVBUF_SIZE 10240

/*SOCKET通信中可能出现的错误代码*/
typedef enum
{
	E_TCP_ERR_OK = 0,//无错误
	E_TCP_ERR_SOCKET_ERR = -1,//建立SOCKET错误
	E_TCP_ERR_CONNECT = -2,//连接失败
	E_TCP_ERR_CONNECT_TIMEOUT = -3,//连接失败,超时
	E_TCP_ERR_SEND = -4,//发送失败
	E_TCP_ERR_RECIVE = -5,//接收失败
	E_TCP_ERR_SEND_OUTTIME = -6,//发送超时
	E_TCP_ERR_RECIVE_OUTTIME = -7,//接收超时
	E_TCP_ERR_SEND_EXCEPT = -8,//发送失败，其他未知错误
	E_TCP_ERR_RECIVE_EXCEPT = -9,//接收失败，其他未知错误
	E_TCP_ERR_CONNECT_MAX = -10,//同时连接数达到最大
}E_TCP_ERR_T;

/*SOCKET应用的状态*/
typedef enum
{
	E_TCP_SOCKET_STATUS_ERR = 0,//错误
	E_TCP_SOCKET_STATUS_OK,//正常
}E_TCP_SOCKET_STATE_T;

/*SOCKET连接的状态*/
typedef enum
{
	E_TCP_CLIENTSOCKET_STATE_IDLE = 0,//空闲
	E_TCP_CLIENTSOCKET_STATE_DISCONN,//连接断开
	E_TCP_CLIENTSOCKET_STATE_CONN,//正在连接
}E_TCP_CLIENTSOCKET_STATE_T;

/*SOCKET连接的参数信息*/
typedef	struct _TCP_SOCK_CLIENT_{
	char				ServerIP[20];//字符串类型的IP地址，形式如：“192.168.1.1”
	SOCKET				Socketfd;//SOCKET的fd号码
	DWORD				TimeOut;//超时设置时间
	int					State;//当前的连接状态，可能取值E_TCP_CLIENTSOCKET_STATE_IDLE、E_TCP_CLIENTSOCKET_STATE_DISCONN、E_TCP_CLIENTSOCKET_STATE_CONN
	int					ErrCount;//错误次数
	int                 nPort; // 端口号
}TCP_CLIENTSOCKT_T;


class CTCP
{
public:
	CTCP();
	~CTCP();
	bool	SocketOK;//SOCKET的状态

	/*查找已经建立好的连接*/
	TCP_CLIENTSOCKT_T* FindClientSocketByIP(const char *ServerIP);//通过IP查找TCP连接
	TCP_CLIENTSOCKT_T* FindClientSocketByIndex(int Index);

	/*查找空闲的连接*/
	TCP_CLIENTSOCKT_T* FindClientSocketIDLE(void);

	int Connect(const char IP[], int Timeout = DEFAULT_CONNECT_TIMEOUT);//建立TCP连接，指定超时时间(ms)
	int Connect2(const char IP[],int nPort, int Timeout = DEFAULT_CONNECT_TIMEOUT);//建立TCP连接，指定超时时间(ms)
	int DisConnect(const char IP[]);//根据IP地址断开连接
	int DisConnect(int Index);//根据连接的序列号断开连接

	int Send(const char IP[], void *pSendData, int SendDataSize, int Timeout = DEFAULT_SEND_TIMEOUT);
	int Send(int Index, void *pSendData, int SendDataSize, int Timeout = DEFAULT_SEND_TIMEOUT);
	int Recv(const char IP[], void *pRecvData, int RecvDataSize, int Timeout = DEFAULT_RECV_TIMEOUT);
	int Recv(int Index, void *pRecvData, int RecvDataSize, int Timeout = DEFAULT_RECV_TIMEOUT);

	CRITICAL_SECTION cs;
	TCP_CLIENTSOCKT_T	ClientSocket[CLIENTSOCKET_MAX];//存放SOCKET的连接
	CRITICAL_SECTION ss;
};

#endif
