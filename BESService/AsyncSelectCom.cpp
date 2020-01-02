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
{ //��ʼ��ָ���汾���׽��ֿ�
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
		TRACE0("���õ�WindSock DLL�汾������Ҫ��İ汾���ر�WindSock��");
		WSACleanup();
		return m_bInited;
	}

	m_bInited = TRUE;
	return m_bInited;
}

BOOL CAsyncSelectCom::LoadConfig(const CString &strXmlFile, const CString &strEncryptKey, 
	const CString &strVersion)
{ //��������
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
	//				TRACE1("id=%d ���ݲ���ʧ�ܣ�ԭ�򣺿����ظ����壡", nID);
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
{//����ID����ȡ��Ϣ����
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
		TRACE0("CAsyncSelectCom::ServerInitע���첽�¼�FD_ACCEPT|FD_READ|FD_WRITE|FD_CLOSEʧ�ܣ�");
		//DebugConsole(_T("CAsyncSelectCom::ServerInitע���첽�¼�FD_ACCEPT|FD_READ|FD_WRITE|FD_CLOSEʧ�ܣ�"));
		return FALSE;
	}

	return TRUE;
}

BOOL CAsyncSelectCom::Listen(HWND hWnd,UINT uPort)
{
	if (!m_bInited)
	{
		TRACE0("û��ʹ��CAsyncSelectCom::Init()��ʼ���׽��ֿ⣡");
		return FALSE;
	}
	m_hWnd = hWnd;

	if(m_socket != INVALID_SOCKET)
	{
		//����Ѿ��������׽����ȹر�ԭ����
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	//�����µ��׽��֣������Ǵ����������͵��׽���
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
	//��һ���׽��ֵ������ĵ�ַ
	res = bind(m_socket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr));
	if(res == SOCKET_ERROR)
	{	
		//�󶨴���
		TRACE0("CAsyncSelectCom::Listen�����г��ְ󶨴���");
		//DebugConsole(_T("CAsyncSelectCom::Listen�����г��ְ󶨴���"));
		return FALSE;
	}

	//��ʼһ���������̣��ȴ��ͻ�������
	res = listen(m_socket, 5);    //����ĵڶ���������ʾ���֧�ֵĿͻ�������
	if(res == SOCKET_ERROR)
	{	
		//listenʧ��
		TRACE0("CAsyncSelectCom::Listen�����м���ʧ�ܣ�");
		//DebugConsole(_T("CAsyncSelectCom::Listen�����м���ʧ�ܣ�"));
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
		TRACE0("CAsyncSelectCom::ClientInitע���첽�¼�FD_READ|FD_WRITE|FD_CLOSE|FD_CONNECTʧ�ܣ�");
		//DebugConsole(_T("CAsyncSelectCom::ClientInitע���첽�¼�FD_READ|FD_WRITE|FD_CLOSE|FD_CONNECTʧ�ܣ�"));
		return FALSE;
	}
	return TRUE;
}

BOOL CAsyncSelectCom::Connet(HWND hWnd, UINT uPort, const CString &strServerIp)
{
	if (!m_bInited)
	{
		TRACE0("û��ʹ��CAsyncSelectCom::Init()��ʼ���׽��ֿ⣡");
		return FALSE;
	}
	m_hWnd = hWnd;

	if(m_socket != INVALID_SOCKET)
	{
		//���ԭ�������׽����ȹر�
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	//�����µ����׽���
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	if(!ClientInit())
	{
		return FALSE;
	}

	//׼������������Ϣ��������Ҫָ���������ĵ�ַ
	char bufIp[256];
	if(!CCommonTool::CStringToChar(strServerIp, bufIp, 256))
	{
		return FALSE;
	}

	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.S_un.S_addr = inet_addr(bufIp);
	sockAddr.sin_port = htons(uPort);   //�ı�˿ںŵ����ݸ�ʽ

	//�����������ӷ��������ù��̽��ȴ�һ��ʱ��
	int nRes = 0;
	nRes = connect(m_socket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr));
	if(nRes == SOCKET_ERROR)
	{
		//����ʧ��
		if(GetLastError() != WSAEWOULDBLOCK)
		{
			TRACE0("CAsyncSelectCom::Connet�����У�����WSAEWOULDBLOCK����");
			//DebugConsole(_T("CAsyncSelectCom::Connet�����У�����WSAEWOULDBLOCK����"));
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
{ //�ر�ָ�����׽���
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
		TRACE0("CAsyncSelectCom::RecvString���׽���m_socket = INVALID_SOCKET");
		//DebugConsole(_T("CAsyncSelectCom::RecvString���׽���m_socket = INVALID_SOCKET"));
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
{ //�ڲ��н��ջ���������СΪDEF_SIZE_RECV_BUF
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
{ //�����ַ���
	if (sockClient == INVALID_SOCKET)
	{
		TRACE0("CAsyncSelectCom::SendString���׽���m_socket = INVALID_SOCKET");
		//DebugConsole(_T("CAsyncSelectCom::SendString���׽���m_socket = INVALID_SOCKET"));
		return 0;
	}

	return send(sockClient, bufSend, nLen, 0);
}

int CAsyncSelectCom::SendString(SOCKET sockClient, const CString &strSendText)
{//�ڲ��з��ͻ���������СΪDEF_SIZE_SEND_BUF
	CHAR bufSend[DEF_SIZE_SEND_BUF];
	if (!CCommonTool::CStringToChar(strSendText, bufSend, DEF_SIZE_SEND_BUF))
	{
		return 0;
	}

	int nSend = strlen(bufSend);
	return SendString(sockClient, bufSend, nSend);
}

BOOL CAsyncSelectCom::SendIdentifyRequest(const IdentifyRequest &idReq)
{ //������֤��������
	if (m_socket == INVALID_SOCKET)
	{
		return -1;
	}

	return SendIdentifyRequest(m_socket, idReq);
}

BOOL CAsyncSelectCom::SendIdentifyRequest(SOCKET sockClient, const IdentifyRequest &idReq)
{ //������֤��������
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
{//������֤��Ӧ����
	if (m_socket == INVALID_SOCKET)
	{
		return -1;
	}

	return RecvIdentifyResponse(m_socket, idRes);
}

BOOL CAsyncSelectCom::RecvIdentifyResponse(CHAR *pMsg, int nMsgLen, IdentifyResponse &idRes)
{//c���ַ����н�����֤��Ӧ����
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
{//������֤��Ӧ����
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

	//���ж���Ϣͷ��
	CString strHead = strMsg.Left(NUM_FIXED_MSGHEAD);
	if (strHead != TEXT_FIXED_MSGHEAD)
	{
		return FALSE;
	}

	//�ж���Ϣ�ܳ���
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

	//�ж���Ϣ����
	CString strMsgType = strMsg.Mid(NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET, NUM_MSG_MAIN_TYPE);
	if (strMsgType != TYPE_MSG_IDENTIFY_RES)
	{
		return FALSE;
	}

	//�������
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
{ //�����ݴ�����Զ�������Ϣͷ�Ͱ��ܳ��ȣ��ȼ��ܺ��ͳ�ȥ
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

//���ݼ��ܺ���
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

//���ݼ��ܺ���
int CAsyncSelectCom::SendDataWithHead_S(SOCKET sockClient, CHAR *pData, int nLen)
{ //�����ݴ�����Զ�������Ϣͷ�Ͱ��ܳ��ȣ����ͳ�ȥ
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

//���ݼ��ܺ���
int CAsyncSelectCom::SendDataWithType_S(SOCKET sockClient, const CString &strMsgType,
	CHAR *pData, int nLen)
{
#ifdef _DEBUG
	if (sockClient == INVALID_SOCKET)
	{
		TRACE0("CXKServer::SendDataWithHead�����sockClient=INVALID_SOCKET");
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
	//��ȡ��Ϣͷ
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

	//�������ݰ��ܳ���
	CHAR bufTotalLen[NUM_TOTAL_LEN_DATAPACKET + 1];
	int nTotalLen = 0;
	if (NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET > nMsgLen)
	{
		return FALSE;
	}
	memcpy(bufTotalLen, pMsg + NUM_FIXED_MSGHEAD, NUM_TOTAL_LEN_DATAPACKET);
	bufTotalLen[NUM_TOTAL_LEN_DATAPACKET] = '\0';
	nTotalLen = atoi(bufTotalLen);

	//������������
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
		TRACE0("CAsyncSelectCom::GetHostName��ȡ������ʧ�ܣ�");
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
		TRACE0("CAsyncSelectCom::GetHostIp��ȡ������ʧ�ܣ�");
		return _T("");
	}

	if((host = gethostbyname(bufName)) == NULL)
	{
		TRACE0("CAsyncSelectCom::GetHostIp��ȡ����Ip����");
		return _T("");
	}

	if (host->h_addr_list[0] == NULL)
	{
		return _T("");
	}

	return CString(inet_ntoa (*(struct in_addr *)host->h_addr_list[0]));
}

void CAsyncSelectCom::ParseReadMsg(SOCKET sockClient)
{//ѭ������ָ���׽����Ϸ�������Ϣ
	CString strHead = _T("");
	CString strFixedHead = _T(""); //������������
	CString strDataLength = _T("0"); //���ݰ�����
	
	int nTotalLen = 0;
	int uDataLength = 0;
	int nRead = -1;
	BOOL bReadLength = FALSE;

	char totalBuf[BUF_TOTAL_SIZE];
	char recvBuf[BUF_RECV_SIZE];
	//BOOL bPauseRead = FALSE; //�ܵĴ���buffer���ˣ���ͣ��ȡ��ʶ
	
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
				{//û�й̶���Ϣͷ,ֱ�ӷ��ز����������ַ�����
					TRACE0("���������У�����һ����Ϣû�й̶���Ϣͷ��");
					return;
				}
				strDataLength = strHead.Right(NUM_TOTAL_LEN_DATAPACKET);
				uDataLength = _ttoi(strDataLength);
				bReadLength = TRUE;
			}

			if (uDataLength <= NUM_FIXED_MSGHEAD + NUM_TOTAL_LEN_DATAPACKET)
			{
				TRACE0("���ݰ�ָ�������ݳ��Ȳ�����Ҫ��");
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
{ //������ParseReadMsgѭ�����������е�һ��������Ϣ
	//TRACE(CString(bufMsg));
	return TRUE;
}