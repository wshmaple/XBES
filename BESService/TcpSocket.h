#pragma once

#ifndef _TCPSOCKET_H_
#define _TCPSOCKET_H_

#include "Logs.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <map>
using namespace std;

/*SOCKET连接的参数信息*/
typedef	struct _TCP_SOCK_CLIENTS_{
	char				ServerIP[20];			//字符串类型的IP地址，形式如：“192.168.1.1”
	SOCKET				Socketfd;				//SOCKET的fd号码
	DWORD				TimeOut;				//超时设置时间
	int					State;					//当前的连接状态，可能取值TCP_CLIENTSOCKET_STATE_IDLE、TCP_CLIENTSOCKET_STATE_DISCONN、TCP_CLIENTSOCKET_STATE_CONN
	int					ErrCount;				//错误次数
	struct sockaddr_in	ServerSocketAddr;		//SOCKET连接中使用的标准IP格式
	int					nPort;					//Socket 连接的端口
	int					SocketState;			//当前OCKET的状态
}TCP_CLIENTSOCKT_TS;

//WSAAsyncSelect
//CAsyncSelectCom
class CTcpSocket
{
public:
	CTcpSocket();
	~CTcpSocket();

public:

	int SendMsg(SOCKET socketfd, const char SendBuf[], int SendDataCount);// 发送数据
	int RecvMsg(SOCKET socketfd, char *pRecvBuf, int *pRecvDataCount);// 接收数据



	//************************************
	// Method:    SendMsg
	// FullName:  CTcpSocket::SendMsg
	// Access:    public 
	// Returns:   int						// 返回 标识
	// Qualifier:
	// Parameter: const char ServerIP[]		// IP
	// Parameter: int nPort					// 端口	
	// Parameter: const char * pSendData	// 发送数据内容
	// Parameter: int SendDataSize			// 发送长度
	// Parameter: char * pRecvData			// 接收地址
	// Parameter: int * RecvDataSize		// 接收数据长度
	// Parameter: char * * errmsg			// 错误信息
	//************************************
	int SendMsg(const char ServerIP[], int nPort, const char *pSendData, int nSendDataSize, char *pRecvData, int *nRecvDataSize, char **errmsg);// 实际上我们是这么用的

protected:
	int Connect(const char IP[], int nPort);// 连接

	int ReciveMsg(const char IP[], int nPort);// 接收数据

	void Disconnect(const char IP[], int nPort);// 断开连接


protected:
	//CLogs m_logs; // 日志文件处理

	map <string ,TCP_CLIENTSOCKT_TS > m_mapClients;


	int	SocketState;//本地SOCKET的状态

//	TCP_CLIENTSOCKT_T ClientSocket[CLIENTSOCKET_MAX];//存放SOCKET的连接


	char m_Logbuf[512];//错误信息

	CRITICAL_SECTION cs;
public:
	/*建立TCP的连接*/
	int  TCP_Connect(const char IP[],int nPort, SOCKET *pSocketfd);
	/*断开TCP连接*/
	void TCP_Disconnect(SOCKET Socketfd);
	/*断开TCP连接*/
	void TCP_Disconnect(const char IP[], int nPort);
	/*发送数据*/
	int  TCP_Send(SOCKET Socketfd, const char SendBuf[], int SendDataCount);
	/*接收数据*/
	int  TCP_Recive(SOCKET Socketfd, char *pRecvBuf, int *pRecvDataCount);
	/*生成日志文件*/
	int LogPrint(char *pLogAddress, char *pLogString);
};
#endif
