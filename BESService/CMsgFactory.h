#pragma once
// ��Ϣ���� ��
#include "CMsgLog.h"
#include <map>
#include <string>
class CMsgFactory
{
public:
	CMsgFactory();
	~CMsgFactory();
	
	void Add(string strIP,string strType); // ����һ��
	CMsgLog * GetOneMsgLog(string strIP); // ���ض���ָ��
	int  GetSize();// ��ȡ���ȴ�С
	void DataIN(string strtype, string strIP , int nIndex, string strWebMsg);//��Ϣ����
	void DataOut(string strtype, string strIP, int nIndex, string strWebMsg, string strMacMsg);//��Ϣ�������
	void DataOut(string strtype, string strIP, int nIndex, string strWebMsg, string strMacMsg,bool bOk);//��Ϣ�������
	int DoInNames(string strType);

	map<string, CMsgLog> m_map; // ���� һ����Ϣ�ƹ�

	map<int, string> m_mapNames;
	// 
	bool m_bShow;// ��ǰ�豸 �Ƿ���ʾ

private:
	
};

