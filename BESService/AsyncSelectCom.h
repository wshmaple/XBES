#pragma once
#include <winsock2.h>
//#include "BaseType.h"
#include <map>
#define MAP					std::map
#pragma   comment(lib, "ws2_32.lib")

//�������ݴ���������С
#define DEF_SIZE_RECV_BUF	1024 //�ڲ�ʹ�õĽ�������buf��С
#define DEF_SIZE_SEND_BUF	1024 //�ڲ�ʹ�õķ�������buf��С
#define DEF_SIZE_BUF		1024

#ifndef XK_DLL_API_V2_1
#define XK_DLL_API_V2_1	_declspec(dllimport)
#endif

//������Ϣ���첽ѡ��ģ������ͨ��
class XK_DLL_API_V2_1 CAsyncSelectCom
{
public:
	CAsyncSelectCom(void);
	~CAsyncSelectCom(void);
	
	//static void SetEncryptKey(const CString &strEncryptKey); //�������������Կ

	BOOL Init(int nMajorVersion = 2, int nMinorVersion = 2); //��ʼ��ָ���汾���׽��ֿ�

	BOOL Listen(HWND hWnd, UINT uPort); //�������˳�ʼ������������
	
	SOCKET Accept(SOCKADDR *sockAddr); //�������˴���ͻ��˵�����,����ɹ�������ÿͻ���ͨ�ŵ�socket��ʧ�ܷ���INVALID_SOCKET

	BOOL Connet(HWND hWnd, UINT uPort, const CString &strServerIp); //�ͻ��˳�ʼ��������ָ���ķ�����

	void Close(); //�ر��ڲ��׽���m_socket

	void Close(SOCKET &sockClient); //�ر�ָ�����׽���

	BOOL LoadConfig(const CString &strXmlFile, const CString &strEncryptKey = _T(""), 
		const CString &strVersion = _T("2.1")); //������Ϣ�����ļ�

	CString GetStandardMessage(int nID); //����ID����ȡ��Ϣ����

	//Method Function: �����ַ���
	//Parameters: CHAR *bufRecv �������ݵĵ�ַ��
	//			int nLen �������Ĵ�С
	//Recturn: ʵ�ʽ��յ��ֽ���
	int RecvString(CHAR *bufRecv, int nLen);

	int RecvString(CString &strRecvText); //ʹ���ڲ����յĻ���������СΪDEF_SIZE_RECV_BUF
	
	static int RecvString(SOCKET sockClient, CHAR *bufRecv, int nLen); 

	static int RecvString(SOCKET sockClient, CString &strRecvText); //�ڲ��н��ջ���������СΪDEF_SIZE_RECV_BUF
	
	//Method Function: �����ַ���
	//Parameters: CHAR *bufSend �������ݵĵ�ַ��
	//				int nLen ָ�����ʹ�С
	//Recturn: ʵ�ʷ��͵��ֽ���
	int SendString(CHAR *bufSend, int nLen); //�����ַ���

	int SendString(const CString &strSendText); //�ڲ��з��ͻ���������СΪDEF_SIZE_SEND_BUF

	static int SendString(SOCKET sockClient, CHAR *bufSend, int nLen); //�����ַ���

	static int SendString(SOCKET sockClient, const CString &strSendText); //�ڲ��з��ͻ���������СΪDEF_SIZE_SEND_BUF

	//��֤��Ϣ�ķ��ͺͽ���
	BOOL SendIdentifyRequest(const IdentifyRequest &idReq); //������֤��������

	BOOL RecvIdentifyResponse(IdentifyResponse &idRes); //������֤��Ӧ����

	static BOOL SendIdentifyRequest(SOCKET sockClient, const IdentifyRequest &idReq); //������֤��������

	static BOOL RecvIdentifyResponse(SOCKET sockClient, IdentifyResponse &idRes); //������֤��Ӧ����

	static BOOL RecvIdentifyResponse(CHAR *pMsg, int nMsgLen, IdentifyResponse &idRes); //c���ַ����н�����֤��Ӧ����
	
	//��������
	int SendDataWithHead(CHAR *pData, int nLen); //�����ݴ�����Զ�������Ϣͷ�Ͱ��ܳ��ȣ����ͳ�ȥ

	int SendDataWithHead_S(CHAR *pData, int nLen); //�����ݴ�����Զ�������Ϣͷ�Ͱ��ܳ��ȣ��ȼ��ܺ��ͳ�ȥ

	//��ָ������Ϣ���ͺ����ݽ��д����
	//�ټ�����Ϣͷ�Ͱ��ܳ��ȣ����ͳ�ȥ��pDataΪNULL,���ʾ���ݲ���Ϊ�գ�ֻ���������Ͳ���
	//�����͵�����ռ���ֽ���Ϊ5,�̶�λ��
	int SendDataWithType(const CString &strMsgType, CHAR *pData, int nLen); 

	//���ݼ��ܺ���
	int SendDataWithType_S(const CString &strMsgType, CHAR *pData, int nLen);

	static int SendDataWithHead(SOCKET sockClient, CHAR *pData, int nLen);

	//���ݼ��ܺ���
	static int SendDataWithHead_S(SOCKET sockClient, CHAR *pData, int nLen); //�����ݴ�����Զ�������Ϣͷ�Ͱ��ܳ��ȣ��ȼ��ܺ��ͳ�ȥ

	static int SendDataWithType(SOCKET sockClient, const CString &strMsgType,
		CHAR *pData, int nLen); //������ݻ�����ͨ��Э�鶨�壬��strMsgType������5λ�ַ�����������ʧ��

	//���ݼ��ܺ���
	static int SendDataWithType_S(SOCKET sockClient, const CString &strMsgType,
		CHAR *pData, int nLen);

	//�������յ�����Ϣ����
	//Method Function: ����ͷ����Ϣ�ͳ��Ƚ�����Ϣ,��û���ҵ���Ϣͷ����pMsg���ݿ�����pData��
	//Parameters: CHAR *pMsg [in] ����������Ϣ������, Ҫ��û��������ϻ�����ͨ��Э��, 
	//���û�����һ��ʼ��������Ϣ��ʽ���ݣ�����в���һ����ȷ��Ϣ��ʽ�Ļ��ᣬ��������
	//				int nMsgLen [in] ����������Ϣ�������Ĵ�С, 
	//				CHAR *pData [in/out], ��Ϊ����������ݴ洢������������Ϣͷ���������ܳ���
	//				int nDataLen [int/out] ��Ϊ�������ʱΪ�����������Ĵ�С����Ϊ����ֵΪ��ȷ�����������ݳ���,
	//Return: TRUEΪ���ݽ����ɹ���FALSEΪ���ݽ���ʧ��
	//����ʧ��ԭ��1��pMsg�����ݲ����ϻ�����ͨ��Э���ʽ��2������Ϊ���������ݣ���Ҫƴ�ӣ�
	//				3,���յ����ݴ洢��pData���Ȳ�����
	static BOOL ParseDataByHead(CHAR *pMsg, int nMsgLen, CHAR *pData, int &nDataLen); 

	//Method Function: ����ͷ����Ϣ�ͳ���,������Ϣ����+���ݣ���û��ͷ����Ϣ�������ݵ�ǰ5λ��Ϊ�����ͣ�����������ΪpData������
	//Parameters: CHAR *pMsg [in] ����������Ϣ������, Ҫ��û��������ϻ�����ͨ��Э��, 
	//���û�����һ��ʼ��������Ϣ��ʽ���ݣ�����в���һ����ȷ��Ϣ��ʽ�Ļ��ᣬ��������
	//				int nMsgLen [in] ����������Ϣ�������Ĵ�С, 
	//				CString &strMsgType [out] ���������������ͣ�
	//				CHAR *pData [in/out], ��Ϊ����������ݴ洢����
	//				��������Ϣͷ���������ܳ����Լ���Ϣ����
	//				int nDataLen [int/out] ��Ϊ�������ʱΪ�����������Ĵ�С��
	//				��Ϊ����ֵʱΪ��ȷ�����������ݳ���,�������������ͳ���
	//Return: TRUEΪ���ݽ����ɹ���FALSEΪ���ݽ���ʧ��
	//����ʧ��ԭ��1��pMsg�����ݲ����ϻ�����ͨ��Э���ʽ��2������Ϊ���������ݣ���Ҫƴ�ӣ�
	//				3,���յ����ݴ洢��pData���Ȳ�����
	static BOOL ParseDataByType(CHAR *pMsg, int nMsgLen, CString &strMsgType, 
		CHAR *pData, int &nDataLen);

	static CString GetHostName(); //��ȡ������

	static CString GetHostIp(); //��ȡ����Ip

public:
	void ParseReadMsg(SOCKET sockClient); //���ձ�׼Э�飬ѭ������ָ���׽����Ϸ�������Ϣ

	 //������ParseReadMsgѭ�����������е�һ��������Ϣ��ע������Ϣ�Ѿ�ȥ����ͷ�����ܰ��������ݲ��֣�
	virtual BOOL OnMessageProcessing(SOCKET sockClient, CHAR *bufMsg, int nLen);

protected:
	BOOL ServerInit(); //�������˵��첽��Ϣ�¼�WM_SERVER_ASYCOMM��ע��

	BOOL ClientInit(); //�ͻ��˵��첽��Ϣ�¼�WM_CLIENT_ASYCOMM��ע��

public:
	SOCKET m_socket;
	HWND m_hWnd;
	MAP<int, CString> m_mapMessConf;

protected:
	BOOL m_bInited;

	static CString s_strEncryptKey; //��Կ
};

