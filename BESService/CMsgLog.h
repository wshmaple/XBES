#pragma once
class CMsgLog
{
public:
	CMsgLog();
	~CMsgLog();

	map<int, int > m_mapStates;// ״̬
	map<int, int > m_mapCounts;// ����
	map<int, string > m_mapSecond;// ʱ��� ����
	map<int, string > m_mapSecond2;// ʱ��� ����
	map<int, int > m_mapRtOkCount;// ��ȷ���ش���
	map<int, int > m_mapRtErrorCount;// ���󷵻ش��� 

	// ���һ�� ���ݵ� ���պͷ���
	string m_strLastMsgFromWeb; // ���һ�� web ��Ϣ

	string m_strLastMsgFromMac; // ��� ���� web ��Ϣ

	string m_strLastMsgFormWebTime; // ��� web ��Ϣʱ��
	string m_strLastMsgFromMacTime; // ������� web ��Ϣʱ��

	string m_strMsgType; // �������� �ƹ�/¥��
	string m_strMsgIP;   // ��ǰ�� IP ��Ϣ
	string m_strMsgPort; // ��Ϣ�Ķ˿ں�

	string m_strMethodName;// ��������

	bool m_bShow;

	int m_nTotal; // �ܵĵ��ô���
	int m_nOkCount;    // ������ȷ�Ĵ���
	int m_nErrorCount; // ���ô���Ĵ���

	int m_nInCount; // ���ô���
	int m_nOutCount; // ��������


//	CTime m_lastTime;
public:
	string GetNowString();
	void DataIN(int nIndex,string strWebMsg);//��Ϣ����

	void DataOut(int nIndex,string strMacMsg,string strMethodName);//��Ϣ�������
	void DataOut(int nIndex,string strMacMsg,string strMethodName,bool bOk);//��Ϣ�������

	void SetSize(int nSize);

};

