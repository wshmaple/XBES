#include "StdAfx.h"
//#include "UserMessage.h"
#include "AsyncSelectCom.h"
//#include "Message.h"
//#include "CommonTool.h"
//#include "Markup.h"

#define		PAIR		std::pair

//CString CAsyncSelectCom::s_strEncryptKey = _T("");
CAsyncSelectCom::CAsyncSelectCom(void)
{
	m_bInited = FALSE;
	m_hWnd = NULL;
	m_socket = INVALID_SOCKET;
}

CAsyncSelectCom::~CAsyncSelectCom(void)
{
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
	}
	if (m_bInited)
	{
		WSACleanup();
	}
}


BOOL CAsyncSelectCom::Init(int nMajorVersion, int nMinorVersion)
{ //初始化指定版本的套接字库
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(nMinorVersion, nMajorVersion);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) 
	{
		TRACE0("Now program could not find a usable WinSock DLL.");
		return m_bInited;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) 
	{
		TRACE0("可用的WindSock DLL版本不是所要求的版本，关闭WindSock库");
		WSACleanup();
		return m_bInited;
	}

	m_bInited = TRUE;
	return m_bInited;
}

BOOL CAsyncSelectCom::LoadConfig(const CString &strXmlFile, const CString &strEncryptKey, 
	const CString &strVersion)
{ //加载配置
	m_mapMessConf.clear();

	//CMarkup dataXml;
	//WCHAR *wChDoc = NULL;
	//if (strEncryptKey == _T(""))
	//{
	//	if (!dataXml.Load(strXmlFile))
	//	{
	//		return FALSE;
	//	}
	//}
	//else
	//{
	//	CString strConfFile;
	//	//CXKEncryptFile xkDecrypt;
	//	CHAR *pDataBuf = NULL;

	//	strConfFile.Format(_T("%sk"), strXmlFile);
	//	//pDataBuf = xkDecrypt.DecryptFileContentEx(strConfFile, strEncryptKey);
	//	if (pDataBuf == NULL)
	//	{
	//		return FALSE;
	//	}
	//	int nlen=MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,(const char *)pDataBuf,-1,NULL,0);
	//	wChDoc=new wchar_t[nlen];
	//	MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,(const char *)pDataBuf,-1,wChDoc,nlen);

	//	if (!dataXml.SetDoc(wChDoc))
	//	{
	//		delete[] wChDoc;
	//		return FALSE;
	//	}
	//}

	//dataXml.ResetMainPos();
	//if(!dataXml.FindElem(_T("MessageConfig")))
	//{
	//	if (wChDoc != NULL)
	//	{
	//		delete[] wChDoc;
	//	}
	//	return FALSE;
	//}

	//if (_ttof(dataXml.GetAttrib(_T("version"))) < _ttof(strVersion))
	//{
	//	if (wChDoc != NULL)
	//	{
	//		delete[] wChDoc;
	//	}
	//	return FALSE;
	//}

	//while(dataXml.FindChildElem(_T("MessageContext")))
	//{
	//	dataXml.IntoElem();
	//	while(dataXml.FindChildElem(_T("Message")))
	//	{
	//		int nID = _ttoi(dataXml.GetChildAttrib(_T("id")));
	//		if (nID != 0)
	//		{
	//			PAIR<MAP<int, CString>::iterator, BOOL> resPair;
	//			resPair = m_mapMessConf.insert(MAP<int, CString>::value_type(nID, dataXml.GetChildData()));
	//			if (resPair.second == FALSE)
	//			{
	//				TRACE1("id=%d 数据插入失败，原因：可能重复定义！", nID);
	//				continue;
	//			}
	//		}
	//	}
	//	dataXml.OutOfElem();
	//}

	//if (wChDoc != NULL)
	//{
	//	delete[] wChDoc;
	//}
	return TRUE;
}

CString CAsyncSelectCom::GetStandardMessage(int nID)
{//根据ID，获取消息内容
	MAP<int, CString>::iterator iterFind;
	iterFind = m_mapMessConf.find(nID);
	if (iterFind != m_mapMessConf.end())
	{
		return iterFind->second;
	}

	return _T("");
}

BOOL CAsyncSelectCom::ServerInit()
{
	if(WSAAsyncSelect(m_socket, m_hWnd, WM_SERVER_ASYCOMM, 
		FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE) != 0)
	{
		TRACE0("CAsyncSelectCom::ServerInit注册异步事件FD_ACCEPT|FD_READ|FD_WRITE|FD_CLOSE失败！");
		//DebugConsole(_T("CAsyncSelectCom::ServerInit注册异步事件FD_ACCEPT|FD_READ|FD_WRITE|FD_CLOSE失败！"));
		return FALSE;
	}

	return TRUE;
}

BOOL CAsyncSelectCom::Listen(HWND hWnd,UINT uPort)
{
	if (!m_bInited)
	{
		TRACE0("没有使用CAsyncSelectCom::Init()初始化套接字库！");
		return FALSE;
	}
	m_hWnd = hWnd;

	if(m_socket != INVALID_SOCKET)
	{
		//如果已经创建了套接字先关闭原来的
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	//创建新的套接字，这里是创建的流类型的套接字
	m_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(!ServerInit())
	{
		return FALSE;
	}

	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	sockAddr.sin_port = htons(uPort);

	int res = 0;
	//绑定一个套接字到本机的地址
	res = bind(m_socket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr));
	if(res == SOCKET_ERROR)
	{	
		//绑定错误
		TRACE0("CAsyncSelectCom::Listen函数中出现绑定错误！");
		//DebugConsole(_T("CAsyncSelectCom::Listen函数中出现绑定错误！"));
		return FALSE;
	}

	//开始一个侦听过程，等待客户的连接
	res = listen(m_socket, 5);    //这里的第二个参数表示最多支持的客户连接数
	if(res == SOCKET_ERROR)
	{	
		//listen失败
		TRACE0("CAsyncSelectCom::Listen函数中监听失败！");
		//DebugConsole(_T("CAsyncSelectCom::Listen函数中监听失败！"));
		return FALSE;
	}

	return TRUE;
}

SOCKET CAsyncSelectCom::Accept(SOCKADDR *sockAddr)
{
	int nAddrLen = sizeof(*sockAddr);
	SOCKET sockClient = INVALID_SOCKET;

	sockClient = accept(m_socket, sockAddr, &nAddrLen);

	return sockClient;
}

BOOL CAsyncSelectCom::ClientInit()
{
	if(WSAAsyncSelect(m_socket, m_hWnd, WM_CLIENT_ASYCOMM, FD_READ | FD_WRITE | FD_CLOSE | 
		FD_CONNECT) != 0)
	{
		TRACE0("CAsyncSelectCom::ClientInit注册异步事件FD_READ|FD_WRITE|FD_CLOSE|FD_CONNECT失败！");
		//DebugConsole(_T("CAsyncSelectCom::ClientInit注册异步事件FD_READ|FD_WRITE|FD_CLOSE|FD_CONNECT失败！"));
		return FALSE;
	}
	return TRUE;
}

BOOL CAsyncSelectCom::Connet(HWND hWnd, UINT uPort, const CString &strServerIp)
{
	if (!m_bInited)
	{
		TRACE0("没有使用CAsyncSelectCom::Init()初始化套接字库！");
		return FALSE;
	}
	m_hWnd = hWnd;

	if(m_socket != INVALID_SOCKET)
	{
		//如果原来打开这套接字先关闭
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	//创建新的流套接字
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	if(!ClientInit())
	{
		return FALSE;
	}

	//准备服务器的信息，这里需要指定服务器的地址
	char bufIp[256];
	if(!CCommonTool::CStringToChar(strServerIp, bufIp, 256))
	{
		return FALSE;
	}

	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.S_un.S_addr = inet_addr(bufIp);
	sockAddr.sin_port = htons(uPort);   //改变端口号的数据格式

	//这里主动连接服务器，该过程将等待一定时间
	int nRes = 0;
	nRes = connect(m_socket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr));
	if(nRes == SOCKET_ERROR)
	{
		//连接失败
		if(GetLastError() != WSAEWOULDBLOCK)
		{
			TRACE0("CAsyncSelectCom::Connet函数中，发生WSAEWOULDBLOCK错误");
			//DebugConsole(_T("CAsyncSelectCom::Connet函数中，发生WSAEWOULDBLOCK错误"));
			return FALSE;
		}
	}

	return TRUE;
}

void CAsyncSelectCom::Close()
{
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

void CAsyncSelectCom::Close(SOCKET &sockClient)
{ //关闭指定的套接字
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(sockClient);
		sockClient = INVALID_SOCKET;
	}
}

int CAsyncSelectCom::RecvString(CHAR *bufRecv, int nLen)
{
	if (m_socket == INVALID_SOCKET)
	{
		return -1;
	}

	return RecvString(m_socket, bufRecv, nLen);
}

int CAsyncSelectCom::RecvString(CString &strRecvText)
{
	if (m_socket == INVALID_SOCKET)
	{
		return -1;
	}

	return RecvString(m_socket, strRecvText);
}

int CAsyncSelectCom::RecvString(SOCKET sockClient, CHAR *bufRecv, int nLen)
{
	if (sockClient == INVALID_SOCKET)
	{
		TRACE0("CAsyncSelectCom::RecvString中套接字m_socket = INVALID_SOCKET");
		//DebugConsole(_T("CAsyncSelectCom::RecvString中套接字m_socket = INVALID_SOCKET"));
		return 0;
	}
	int nRecv = recv(sockClient, bufRecv, nLen - 1, 0);
	if (nRecv == -1)
	{
		return -1;
	}
	bufRecv[nRecv] = '\0';

	return nRecv;
}

int CAsyncSelectCom::RecvString(SOCKET sockClient, CString &strRecvText)
{ //内部有接收缓冲区，大小为DEF_SIZE_RECV_BUF
	CHAR bufRecv[DEF_SIZE_RECV_BUF];
	int nRecv = RecvString(sockClient, bufRecv, DEF_SIZE_RECV_BUF);
	strRecvText = CString(bufRecv);

	return nRecv;
}

int CAsyncSelectCom::SendString(CHAR *bufSend, int nLen)
{
	if (m_socket == INVALID_SOCKET)
	{
		return -1;
	}

	return SendString(m_socket, bufSend, nLen);
}

int CAsyncSelectCom::SendString(const CString &strSendText)
{
	if (m_socket == INVALID_SOCKET)
	{
		return -1;
	}

	return SendString(m_socket, strSendText);	
}

int CAsyncSelectCom::SendString(SOCKET sockClient, CHAR *bufSend, int nLen)
{ //发送字符串
	if (sockClient == INVALID_SOCKET)
	{
		TRACE0("CAsyncSelectCom::SendString中套接字m_socket = INVALID_SOCKET");
		//DebugConsole(_T("CAsyncSelectCom::SendString中套接字m_socket = INVALID_SOCKET"));
		return 0;
	}

	return send(sockClient, bufSend, nLen, 0);
}

int CAsyncSelectCom::SendString(SOCKET sockClient, const CString &strSendText)
{//内部有发送缓冲区，大小为DEF_SIZE_SEND_BUF
	CHAR bufSend[DEF_SIZE_SEND_BUF];
	if (!CCommonTool::CStringToChar(strSendText, bufSend, DEF_SIZE_SEND_BUF))
	{
		return 0;
	}

	int nSend = strlen(bufSend);
	return SendString(sockClient, bufSend, nSend);
}

BOOL CAsyncSelectCom::SendIdentifyRequest(const IdentifyRequest &idReq)
{ //发送验证请求数据
	if (m_socket == INVALID_SOCKET)
	{
		return -1;
	}

	return SendIdentifyRequest(m_socket, idReq);
}

BOOL CAsyncSelectCom::SendIdentifyRequest(SOCKET sockClient, const IdentifyRequest &idReq)
{ //发送验证请求数据
	int nSend = SendDataWithType(sockClient, TYPE_MSG_IDENTIFY_REQ, 
		(CHAR *)&idReq, sizeof(idReq));
	if (nSend == NUM_FIXED_MSGHEAD + NUM_MSG_MAIN_TYPE 
		+ NUM_TOTAL_LEN_DATAPACKET + sizeof(idReq))
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CAsyncSelectCom::RecvIdentifyResponse(IdentifyResponse &idRes)
{//接收验证回应数据
	if (m_socket == INVALID_SOCKET)
	{
		return -1;
	}

	return RecvIdentifyResponse(m_socket, idRes);
}

BOOL CAsyncSelectCom::RecvIdentifyResponse(CHAR *pMsg, int nMsgLen, IdentifyResponse &idRes)
{//c从字符串中解析验证响应内容
	CString strMsgType = _T("");
	int nDataLen = sizeof(idRes);
	
	if(!ParseDataByType(pMsg, nMsgLen, strMsgType, (CHAR *)&idRes, nDataLen))
	{
		return FALSE;
	}

	if (strMsgType == TYPE_MSG_IDENTIFY_RES && nDataLen == sizeof(idRes))
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CAsyncSelectCom::RecvIdentifyResponse(SOCKET sockClient, IdentifyResponse &idRes)
{//接收验证回应数据
	int nBufLen = NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET 
		+ NUM_MSG_MAIN_TYPE + sizeof(idRes) + 1;
	int nRead = 0;
	CHAR bufRecv[NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET 
		+ NUM_MSG_MAIN_TYPE + sizeof(idRes) + 1];
	memset(&idRes, 0, sizeof(idRes));

	nRead = recv(sockClient, bufRecv, nBufLen, 0);
	if (nRead != nBufLen)
	{
		return FALSE;
	}

	CString strMsg(bufRecv);

	//先判断消息头部
	CString strHead = strMsg.Left(NUM_FIXED_MSGHEAD);
	if (strHead != TEXT_FIXED_MSGHEAD)
	{
		return FALSE;
	}

	//判断消息总长度
	CString strTotalLen = strMsg.Mid(NUM_FIXED_MSGHEAD, NUM_TOTAL_LEN_DATAPACKET);
	CHAR bufData[NUM_TOTAL_LEN_DATAPACKET + 1];
	if (!CCommonTool::CStringToChar(strTotalLen, bufData, NUM_TOTAL_LEN_DATAPACKET + 1))
	{
		return FALSE;
	}
	int nTotalLen = atoi(bufData);
	if (nTotalLen != NUM_FIXED_MSGHEAD + NUM_MSG_MAIN_TYPE + NUM_TOTAL_LEN_DATAPACKET)
	{
		return FALSE;
	}

	//判断消息类型
	CString strMsgType = strMsg.Mid(NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET, NUM_MSG_MAIN_TYPE);
	if (strMsgType != TYPE_MSG_IDENTIFY_RES)
	{
		return FALSE;
	}

	//填充数据
	memcpy(&idRes, bufRecv + NUM_FIXED_MSGHEAD + NUM_MSG_MAIN_TYPE 
		+ NUM_TOTAL_LEN_DATAPACKET, sizeof(idRes));

	return TRUE;
}

int CAsyncSelectCom::SendDataWithHead(CHAR *pData, int nLen)
{
	if (m_socket == INVALID_SOCKET)
	{
		return -1;
	}

	return SendDataWithHead(m_socket, pData, nLen);
}

int CAsyncSelectCom::SendDataWithHead_S(CHAR *pData, int nLen)
{ //将数据打包，自动加上消息头和包总长度，先加密后发送出去
	if (m_socket == INVALID_SOCKET)
	{
		return -1;
	}

	return SendDataWithHead_S(m_socket, pData, nLen);
}

int CAsyncSelectCom::SendDataWithType(const CString &strMsgType,
	CHAR *pData, int nLen)
{
	if (m_socket == INVALID_SOCKET)
	{
		return -1;
	}

	return SendDataWithType(m_socket, strMsgType, pData, nLen);
}

//数据加密后发送
int CAsyncSelectCom::SendDataWithType_S(const CString &strMsgType, CHAR *pData, int nLen)
{
	if (m_socket == INVALID_SOCKET)
	{
		return -1;
	}

	return SendDataWithType_S(m_socket, strMsgType, pData, nLen);
}

int CAsyncSelectCom::SendDataWithHead(SOCKET sockClient, CHAR *pData, int nLen)
{
// 	if (pData == NULL)
// 	{
// 		nLen = 0;
// 	}
	ASSERT(pData != NULL);
	CHAR *pszMsgBuf = pData;
	int nMsgLen = nLen;
// 	CXKEncryptFile xkEncrypt;
// 	if (s_strEncryptKey != _T("") 
// 		&& !xkEncrypt.EncryptBuffer(pData, nLen, s_strEncryptKey, &pszMsgBuf, nMsgLen))
// 	{
// 		return 0;
// 	}

	CString strHead;
	int nTotalLen = NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET + nMsgLen;
	int nSend = 0;
	CString strFormatHead;

	strFormatHead.Format(_T("%%s%%0%dd"), NUM_TOTAL_LEN_DATAPACKET);
	strHead.Format(strFormatHead, TEXT_FIXED_MSGHEAD, nTotalLen);
	
	char bufHead[NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET + 1];
	memset(bufHead, 0, NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET + 1);
	if(CCommonTool::CStringToChar(strHead, bufHead, 
		NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET + 1))
	{
		char bufSend[BUF_TOTAL_SIZE];
		memcpy(bufSend, bufHead, NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET);
		//if (pData != NULL)
		//{
		memcpy(bufSend + NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET, pszMsgBuf, nMsgLen);
		//}
		
		nSend = send(sockClient, bufSend, nTotalLen, 0);
	}

	return nSend;
}

//数据加密后发送
int CAsyncSelectCom::SendDataWithHead_S(SOCKET sockClient, CHAR *pData, int nLen)
{ //将数据打包，自动加上消息头和包总长度，发送出去
	CHAR *pszMsgBuf = pData;
	int nMsgLen = nLen;


	return SendDataWithHead(sockClient, pszMsgBuf, nMsgLen);
}

int CAsyncSelectCom::SendDataWithType(SOCKET sockClient, const CString &strMsgType,
	CHAR *pData, int nLen)
{
	if (pData == NULL)
	{
		nLen = 0;
	}

	CHAR bufSend[BUF_TOTAL_SIZE];
	if (!CCommonTool::CStringToChar(strMsgType, bufSend, NUM_MSG_MAIN_TYPE + 1))
	{
		return 0;
	}

	if (pData != NULL)
	{
		memcpy(bufSend + NUM_MSG_MAIN_TYPE, pData, nLen);
		nLen += NUM_MSG_MAIN_TYPE;
	}
	else
	{
		nLen = NUM_MSG_MAIN_TYPE;
	}

	return SendDataWithHead(sockClient, bufSend, nLen);


	/*CString strMsg;
	CString strFormat;
	int nSend = 0;
	int nTotalLen = nLen + NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET + NUM_MSG_MAIN_TYPE;

	strFormat.Format(_T("%%s%%0%dd%%s"), NUM_TOTAL_LEN_DATAPACKET);
	strMsg.Format(strFormat, TEXT_FIXED_MSGHEAD, nTotalLen, strMsgType);

	char bufHeadEx[NUM_MSG_MAIN_TYPE + NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET + 1];
	memset(bufHeadEx, 0, NUM_MSG_MAIN_TYPE + NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET + 1);
	if(CCommonTool::CStringToChar(strMsg, bufHeadEx, 
		NUM_MSG_MAIN_TYPE + NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET + 1))
	{
		char bufSend[BUF_TOTAL_SIZE];
		memcpy(bufSend, bufHeadEx, NUM_MSG_MAIN_TYPE + NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET);
		if (pData != NULL)
		{
			memcpy(bufSend + NUM_MSG_MAIN_TYPE + NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET, 
				pData, nLen);
		}
		nSend = send(sockClient, bufSend, nTotalLen, 0);
	}
	
	return nSend;*/
}

//数据加密后发送
int CAsyncSelectCom::SendDataWithType_S(SOCKET sockClient, const CString &strMsgType,
	CHAR *pData, int nLen)
{
#ifdef _DEBUG
	if (sockClient == INVALID_SOCKET)
	{
		TRACE0("CXKServer::SendDataWithHead传入的sockClient=INVALID_SOCKET");
		return -1;
	}
#endif
	ASSERT(sockClient != INVALID_SOCKET);

	CHAR bufSend[BUF_TOTAL_SIZE];
	if (!CCommonTool::CStringToChar(strMsgType, bufSend, NUM_MSG_MAIN_TYPE + 1))
	{
		return 0;
	}

	if (pData != NULL)
	{
		memcpy(bufSend + NUM_MSG_MAIN_TYPE, pData, nLen);
		nLen += NUM_MSG_MAIN_TYPE;
	}
	else
	{
		nLen = NUM_MSG_MAIN_TYPE;
	}

	return SendDataWithHead_S(sockClient, bufSend, nLen);
}

BOOL CAsyncSelectCom::ParseDataByHead(CHAR *pMsg, int nMsgLen, CHAR *pData, int &nDataLen)
{
	//提取消息头
	CHAR bufHead[NUM_FIXED_MSGHEAD + 1];
	if (nMsgLen < NUM_FIXED_MSGHEAD)
	{
		return FALSE;
	}
	memcpy(bufHead, pMsg, NUM_FIXED_MSGHEAD);
	bufHead[NUM_FIXED_MSGHEAD] = '\0';
	
	CString strHead(bufHead);
	memset(pData, 0, nDataLen);
	if (strHead != TEXT_FIXED_MSGHEAD)
	{
		if (nMsgLen <= nDataLen)
		{
			memcpy(pData, pMsg, nMsgLen);
			nDataLen = nMsgLen;
		}
		
		return FALSE;
	}

	//解析数据包总长度
	CHAR bufTotalLen[NUM_TOTAL_LEN_DATAPACKET + 1];
	int nTotalLen = 0;
	if (NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET > nMsgLen)
	{
		return FALSE;
	}
	memcpy(bufTotalLen, pMsg + NUM_FIXED_MSGHEAD, NUM_TOTAL_LEN_DATAPACKET);
	bufTotalLen[NUM_TOTAL_LEN_DATAPACKET] = '\0';
	nTotalLen = atoi(bufTotalLen);

	//解析真正数据
	if (nTotalLen > nMsgLen)
	{
		return FALSE;
	}

	int nTmpLen = nTotalLen - NUM_FIXED_MSGHEAD - NUM_TOTAL_LEN_DATAPACKET;
	
	if (nTmpLen < 0 || nTmpLen > nDataLen)
	{
		return FALSE;
	}

	nDataLen = nTmpLen;
	memcpy(pData, pMsg + NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET, nDataLen);
	//pData[nDataLen] = '\0';
	return TRUE;
}

BOOL CAsyncSelectCom::ParseDataByType(CHAR *pMsg, int nMsgLen, CString &strMsgType, 
	CHAR *pData, int &nDataLen)
{
	BOOL bRes = ParseDataByHead(pMsg, nMsgLen, pData, nDataLen);
	if(nDataLen > NUM_MSG_MAIN_TYPE)
	{
		CHAR bufMsgType[NUM_MSG_MAIN_TYPE + 1];
		memcpy(bufMsgType, pData, NUM_MSG_MAIN_TYPE);
		bufMsgType[NUM_MSG_MAIN_TYPE] = '\0';
		strMsgType = CString(bufMsgType);
		nDataLen -= NUM_MSG_MAIN_TYPE;
		memcpy(pData, pData + NUM_MSG_MAIN_TYPE, nDataLen);
		
		return bRes;
	}

	strMsgType = _T("");
	return bRes;
}

CString CAsyncSelectCom::GetHostName()
{
	CHAR bufName[DEF_SIZE_BUF];
	if (gethostname(bufName, DEF_SIZE_BUF) != 0)
	{
		TRACE0("CAsyncSelectCom::GetHostName获取主机名失败！");
		return _T("");
	}

	CString strHostName(bufName);

	return strHostName;
}

CString CAsyncSelectCom::GetHostIp()
{
	HOSTENT *host = NULL;
	CHAR bufName[DEF_SIZE_BUF];

	if (gethostname(bufName, DEF_SIZE_BUF) != 0)
	{
		TRACE0("CAsyncSelectCom::GetHostIp获取主机名失败！");
		return _T("");
	}

	if((host = gethostbyname(bufName)) == NULL)
	{
		TRACE0("CAsyncSelectCom::GetHostIp获取主机Ip错误！");
		return _T("");
	}

	if (host->h_addr_list[0] == NULL)
	{
		return _T("");
	}

	return CString(inet_ntoa (*(struct in_addr *)host->h_addr_list[0]));
}

void CAsyncSelectCom::ParseReadMsg(SOCKET sockClient)
{//循环解析指定套接字上发来的消息
	CString strHead = _T("");
	CString strFixedHead = _T(""); //发送内容类型
	CString strDataLength = _T("0"); //数据包长度
	
	int nTotalLen = 0;
	int uDataLength = 0;
	int nRead = -1;
	BOOL bReadLength = FALSE;

	char totalBuf[BUF_TOTAL_SIZE];
	char recvBuf[BUF_RECV_SIZE];
	//BOOL bPauseRead = FALSE; //总的处理buffer满了，暂停读取标识
	
	memset(totalBuf, '\0', BUF_TOTAL_SIZE);
	while(nTotalLen > -1 && nTotalLen < BUF_TOTAL_SIZE)
	{
		nRead = -1;
		memset(recvBuf, '\0', BUF_RECV_SIZE);
		if((nRead = recv(sockClient, recvBuf, BUF_RECV_SIZE - 1, 0)) == -1 
			|| nTotalLen + nRead > BUF_TOTAL_SIZE)
		{
			break;
		}

		if (nRead > 0)
		{
			memcpy(totalBuf + nTotalLen, recvBuf, nRead);
			nTotalLen += nRead;
		}

		while(nTotalLen > 0 && nTotalLen < BUF_TOTAL_SIZE)
		{
			if (!bReadLength)
			{
				if (nTotalLen < NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET)
				{
					break;
				}

				char headBuf[NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET + 1];
				memcpy(headBuf, totalBuf, NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET);
				headBuf[NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET] = '\0';
				strHead = headBuf;
				strFixedHead = strHead.Left(NUM_FIXED_MSGHEAD);
				if (strFixedHead != TEXT_FIXED_MSGHEAD)
				{//没有固定信息头,直接返回不继续解析字符串了
					TRACE0("接收数据中，发现一条消息没有固定消息头！");
					return;
				}
				strDataLength = strHead.Right(NUM_TOTAL_LEN_DATAPACKET);
				uDataLength = _ttoi(strDataLength);
				bReadLength = TRUE;
			}

			if (uDataLength <= NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET)
			{
				TRACE0("数据包指定的数据长度不符合要求！");
				nTotalLen -= uDataLength;
				if (nTotalLen < 0)
				{
					nTotalLen = 0;
				}
				else
				{
					memcpy(totalBuf, totalBuf + uDataLength, nTotalLen);
				}
				continue;
			}
			if(nTotalLen < uDataLength)
			{
				break;
			}

			char msgBuf[BUF_TOTAL_SIZE];
			memset(msgBuf, '\0', BUF_TOTAL_SIZE);
			memcpy(msgBuf, totalBuf + NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET, 
				uDataLength - NUM_FIXED_MSGHEAD - NUM_TOTAL_LEN_DATAPACKET);
			nTotalLen -= uDataLength;
			if (nTotalLen > 0)
			{
				memcpy(totalBuf, totalBuf + uDataLength, nTotalLen);
				//totalBuf[nTotalLen] = '\0';
			}
			else
			{
				nTotalLen = 0;
			}


			CHAR *pszMsgBuf = msgBuf;
			int nMsgLen = uDataLength - NUM_FIXED_MSGHEAD - NUM_TOTAL_LEN_DATAPACKET;
	
			if(!OnMessageProcessing(sockClient, pszMsgBuf, nMsgLen))
			{
				closesocket(sockClient);
				sockClient = INVALID_SOCKET;
				return;
			}
			bReadLength = FALSE;
		}
	}
}

BOOL CAsyncSelectCom::OnMessageProcessing(SOCKET sockClient, CHAR *bufMsg, int nLen)
{ //处理由ParseReadMsg循环解析过程中的一条完整消息
	//TRACE(CString(bufMsg));
	return TRUE;
}