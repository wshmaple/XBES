#pragma once
#include <winsock2.h>
//#include "BaseType.h"
#include <map>
#define MAP					std::map
#pragma   comment(lib, "ws2_32.lib")

//定义数据处理缓冲区大小
#define DEF_SIZE_RECV_BUF	1024 //内部使用的接收数据buf大小
#define DEF_SIZE_SEND_BUF	1024 //内部使用的发送数据buf大小
#define DEF_SIZE_BUF		1024

#ifndef XK_DLL_API_V2_1
#define XK_DLL_API_V2_1	_declspec(dllimport)
#endif

//基于消息的异步选择模型网络通信
class XK_DLL_API_V2_1 CAsyncSelectCom
{
public:
	CAsyncSelectCom(void);
	~CAsyncSelectCom(void);
	
	//static void SetEncryptKey(const CString &strEncryptKey); //设置网络加密密钥

	BOOL Init(int nMajorVersion = 2, int nMinorVersion = 2); //初始化指定版本的套接字库

	BOOL Listen(HWND hWnd, UINT uPort); //服务器端初始化后启动监听
	
	SOCKET Accept(SOCKADDR *sockAddr); //服务器端处理客户端的连接,处理成功返回与该客户端通信的socket，失败返回INVALID_SOCKET

	BOOL Connet(HWND hWnd, UINT uPort, const CString &strServerIp); //客户端初始化后连接指定的服务器

	void Close(); //关闭内部套接字m_socket

	void Close(SOCKET &sockClient); //关闭指定的套接字

	BOOL LoadConfig(const CString &strXmlFile, const CString &strEncryptKey = _T(""), 
		const CString &strVersion = _T("2.1")); //设置消息配置文件

	CString GetStandardMessage(int nID); //根据ID，获取消息内容

	//Method Function: 接收字符串
	//Parameters: CHAR *bufRecv 接收数据的地址，
	//			int nLen 接收区的大小
	//Recturn: 实际接收的字节数
	int RecvString(CHAR *bufRecv, int nLen);

	int RecvString(CString &strRecvText); //使用内部接收的缓冲区，大小为DEF_SIZE_RECV_BUF
	
	static int RecvString(SOCKET sockClient, CHAR *bufRecv, int nLen); 

	static int RecvString(SOCKET sockClient, CString &strRecvText); //内部有接收缓冲区，大小为DEF_SIZE_RECV_BUF
	
	//Method Function: 发送字符串
	//Parameters: CHAR *bufSend 发送数据的地址，
	//				int nLen 指定发送大小
	//Recturn: 实际发送的字节数
	int SendString(CHAR *bufSend, int nLen); //发送字符串

	int SendString(const CString &strSendText); //内部有发送缓冲区，大小为DEF_SIZE_SEND_BUF

	static int SendString(SOCKET sockClient, CHAR *bufSend, int nLen); //发送字符串

	static int SendString(SOCKET sockClient, const CString &strSendText); //内部有发送缓冲区，大小为DEF_SIZE_SEND_BUF

	//验证信息的发送和接收
	BOOL SendIdentifyRequest(const IdentifyRequest &idReq); //发送验证请求数据

	BOOL RecvIdentifyResponse(IdentifyResponse &idRes); //接收验证回应数据

	static BOOL SendIdentifyRequest(SOCKET sockClient, const IdentifyRequest &idReq); //发送验证请求数据

	static BOOL RecvIdentifyResponse(SOCKET sockClient, IdentifyResponse &idRes); //接收验证回应数据

	static BOOL RecvIdentifyResponse(CHAR *pMsg, int nMsgLen, IdentifyResponse &idRes); //c从字符串中解析验证响应内容
	
	//发送数据
	int SendDataWithHead(CHAR *pData, int nLen); //将数据打包，自动加上消息头和包总长度，发送出去

	int SendDataWithHead_S(CHAR *pData, int nLen); //将数据打包，自动加上消息头和包总长度，先加密后发送出去

	//将指定的消息类型和数据进行打包，
	//再加上消息头和包总长度，发送出去，pData为NULL,则表示数据部分为空，只发送主类型部分
	//主类型的数据占的字节数为5,固定位数
	int SendDataWithType(const CString &strMsgType, CHAR *pData, int nLen); 

	//数据加密后发送
	int SendDataWithType_S(const CString &strMsgType, CHAR *pData, int nLen);

	static int SendDataWithHead(SOCKET sockClient, CHAR *pData, int nLen);

	//数据加密后发送
	static int SendDataWithHead_S(SOCKET sockClient, CHAR *pData, int nLen); //将数据打包，自动加上消息头和包总长度，先加密后发送出去

	static int SendDataWithType(SOCKET sockClient, const CString &strMsgType,
		CHAR *pData, int nLen); //必须根据基本的通信协议定义，如strMsgType必须是5位字符串，否则发送失败

	//数据加密后发送
	static int SendDataWithType_S(SOCKET sockClient, const CString &strMsgType,
		CHAR *pData, int nLen);

	//解析接收到的消息数据
	//Method Function: 根据头部信息和长度解析消息,若没有找到消息头，则将pMsg内容拷贝到pData中
	//Parameters: CHAR *pMsg [in] 待解析的消息缓冲区, 要求该缓冲区符合基本的通信协议, 
	//若该缓冲区一开始不满足消息格式内容，则会有查找一次正确消息格式的机会，继续解析
	//				int nMsgLen [in] 带解析的消息缓冲区的大小, 
	//				CHAR *pData [in/out], 作为解析后的数据存储区，不包括消息头部和数据总长度
	//				int nDataLen [int/out] 作为传入参数时为接收数据区的大小；作为返回值为正确解析到的数据长度,
	//Return: TRUE为数据解析成功，FALSE为数据解析失败
	//解析失败原因：1，pMsg中数据不符合基本的通信协议格式；2，数据为不完整数据，需要拼接；
	//				3,接收的数据存储区pData长度不够；
	static BOOL ParseDataByHead(CHAR *pMsg, int nMsgLen, CHAR *pData, int &nDataLen); 

	//Method Function: 根据头部信息和长度,解析消息类型+数据，若没有头部信息，则将数据的前5位作为主类型，其余数据作为pData的数据
	//Parameters: CHAR *pMsg [in] 待解析的消息缓冲区, 要求该缓冲区符合基本的通信协议, 
	//若该缓冲区一开始不满足消息格式内容，则会有查找一次正确消息格式的机会，继续解析
	//				int nMsgLen [in] 带解析的消息缓冲区的大小, 
	//				CString &strMsgType [out] 解析到的数据类型，
	//				CHAR *pData [in/out], 作为解析后的数据存储区，
	//				不包括消息头部和数据总长度以及消息类型
	//				int nDataLen [int/out] 作为传入参数时为接收数据区的大小；
	//				作为返回值时为正确解析到的数据长度,不包含数据类型长度
	//Return: TRUE为数据解析成功，FALSE为数据解析失败
	//解析失败原因：1，pMsg中数据不符合基本的通信协议格式；2，数据为不完整数据，需要拼接；
	//				3,接收的数据存储区pData长度不够；
	static BOOL ParseDataByType(CHAR *pMsg, int nMsgLen, CString &strMsgType, 
		CHAR *pData, int &nDataLen);

	static CString GetHostName(); //获取主机名

	static CString GetHostIp(); //获取主机Ip

public:
	void ParseReadMsg(SOCKET sockClient); //按照标准协议，循环解析指定套接字上发来的消息

	 //处理由ParseReadMsg循环解析过程中的一条完整消息（注：该消息已经去除了头部和总包长度数据部分）
	virtual BOOL OnMessageProcessing(SOCKET sockClient, CHAR *bufMsg, int nLen);

protected:
	BOOL ServerInit(); //服务器端的异步消息事件WM_SERVER_ASYCOMM的注册

	BOOL ClientInit(); //客户端的异步消息事件WM_CLIENT_ASYCOMM的注册

public:
	SOCKET m_socket;
	HWND m_hWnd;
	MAP<int, CString> m_mapMessConf;

protected:
	BOOL m_bInited;

	static CString s_strEncryptKey; //密钥
};

