#pragma once
// ��Ϣ���� ������
#include "CMsgFactory.h"
class CMsgFactoryControl
{
public:
	CMsgFactoryControl();
	~CMsgFactoryControl();

	map <string ,CMsgFactory> m_map;
	int GetSize();// ��ȡ��С

	bool Add(string strType);// ����һ�����͵���Ϣ����

	CMsgFactory * Get(string strType);// ��ȡһ�����͵Ĺ���
	bool m_bshow;// ��ǰ���͵��豸 �Ƿ���ʾ

	// ����һ����Ϣ
	void DataIN(string strtype,string strIP, int nIndex, string strWebMsg);//��Ϣ����
	void DataOut(string strtype, string strIP, int nIndex, string strWebMsg, string strMacMsg);//��Ϣ�������
	void DataOut(string strtype, string strIP, int nIndex, string strWebMsg, string strMacMsg,bool bOk);//��Ϣ�������
};

