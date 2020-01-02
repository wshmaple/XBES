#include "stdafx.h"
#include "CMsgFactory.h"
#include "CMsgLog.h"

CMsgFactory::CMsgFactory()
{
	m_bShow = false;
}


CMsgFactory::~CMsgFactory()
{
}

void CMsgFactory::Add(string strIP, string strType)
{
	CMsgLog msg;
	int n = DoInNames(strType);
	msg.SetSize(n);
	m_map.insert(pair<string, CMsgLog>(strIP,msg));
}

CMsgLog * CMsgFactory::GetOneMsgLog(string strIP)
{
	std::map<string, CMsgLog>::iterator iter = m_map.find(strIP);
	if (m_map.end() != iter)
		return &(iter->second);
	return NULL ;
}

int CMsgFactory::GetSize()
{
	return m_map.size();
}

void CMsgFactory::DataIN(string strtype, string strIP, int nIndex, string strWebMsg)
{
	CMsgLog *p = GetOneMsgLog(strIP);
	if (p!=NULL)
	{
		p->DataIN(nIndex, strWebMsg);
	}
	else{
		Add(strIP, strtype);
		p = GetOneMsgLog(strIP);
		if (p!=NULL)
		{
			p->DataIN(nIndex, strWebMsg);
		}
	}
}

void CMsgFactory::DataOut(string strtype, string strIP, int nIndex, string strWebMsg, string strMacMsg)
{
	CMsgLog *p = GetOneMsgLog(strIP);
	if (p != NULL)
	{
		p->DataOut(nIndex, strMacMsg,m_mapNames[nIndex]);
	}
	else {
		Add(strIP, strtype);
		p = GetOneMsgLog(strIP);
		if (p != NULL)
		{
			p->DataOut(nIndex, strMacMsg, m_mapNames[nIndex]);
		}
	}
}

void CMsgFactory::DataOut(string strtype, string strIP, int nIndex, string strWebMsg, string strMacMsg, bool bOk)
{
	CMsgLog *p = GetOneMsgLog(strIP);
	if (p != NULL)
	{
		p->DataOut(nIndex, strMacMsg, m_mapNames[nIndex],bOk);
	}
	else {
		Add(strIP, strtype);
		p = GetOneMsgLog(strIP);
		if (p != NULL)
		{
			p->DataOut(nIndex, strMacMsg, m_mapNames[nIndex],bOk);
		}
	}
}

int CMsgFactory::DoInNames(string strType)
{
	if (strType == "cross" || strType == "lamp")
	{
		m_mapNames.insert(pair<int, string>(0, "����һ��������"));
		m_mapNames.insert(pair<int, string>(1, "���ÿ����������в���"));
		m_mapNames.insert(pair<int, string>(2, "���ÿ�������ʱ��"));
		m_mapNames.insert(pair<int, string>(3, "�������������൱�ڸ�λ"));
		m_mapNames.insert(pair<int, string>(4, "���ÿ��������ָ��������ã�������"));
		m_mapNames.insert(pair<int, string>(5, "ɾ��һ������������ɾ��������ص�ģ��͵�"));
		m_mapNames.insert(pair<int, string>(6, "��ȡ��������ʱ��"));
		m_mapNames.insert(pair<int, string>(7, "��ȡ���������������ò���"));
		m_mapNames.insert(pair<int, string>(8, "ͨ��UDP�㲥��ʽ��ȡ���п���������Ϣ"));
		m_mapNames.insert(pair<int, string>(9, "ͨ��UDP�㲥��ʽ���ÿ������Ĳ���"));
		m_mapNames.insert(pair<int, string>(10, "������һ��ģ��"));
		m_mapNames.insert(pair<int, string>(11, "����һ��ģ������в���"));
		m_mapNames.insert(pair<int, string>(12, "ɾ��һ��ģ�飬��ɾ����ģ����صĵ�"));
		m_mapNames.insert(pair<int, string>(13, "��ȡһ��ģ�������������Ϣ"));
		m_mapNames.insert(pair<int, string>(14, "��ȡһ��ģ��Ĵ������"));
		m_mapNames.insert(pair<int, string>(15, "������һ����"));
		m_mapNames.insert(pair<int, string>(16, "����һ��������в���"));
		m_mapNames.insert(pair<int, string>(17, "����һ�����ֵ"));
		m_mapNames.insert(pair<int, string>(18, "���õ��ֵ�����ݵ������"));
		m_mapNames.insert(pair<int, string>(19, "ɾ��һ����"));
		m_mapNames.insert(pair<int, string>(20, "��ȡһ������������ò���"));
		m_mapNames.insert(pair<int, string>(21, "��ȡһ����ı�����Ϣ"));
		m_mapNames.insert(pair<int, string>(22, "����һ������"));
		m_mapNames.insert(pair<int, string>(23, "����һ�����������в���"));
		m_mapNames.insert(pair<int, string>(24, "ɾ��һ������"));
		m_mapNames.insert(pair<int, string>(25, "��ȡһ��������������Ϣ"));
		m_mapNames.insert(pair<int, string>(26, "����һ���ƻ�"));
		m_mapNames.insert(pair<int, string>(27, "�޸�һ���ƻ������в���"));
		m_mapNames.insert(pair<int, string>(28, "ɾ��һ���ƻ�"));
		m_mapNames.insert(pair<int, string>(29, "��ȡһ���ƻ������в���"));
		m_mapNames.insert(pair<int, string>(30, "Զ������"));

	}
	else if (strType == "energy")
	{
		m_mapNames.insert(pair<int, string>(0, "����һ��������"));
		m_mapNames.insert(pair<int, string>(1, "���ÿ����������в���(ͬ��)"));
		m_mapNames.insert(pair<int, string>(2, "����ʱ��"));
		m_mapNames.insert(pair<int, string>(3, "����DDC,�൱�������λ"));
		m_mapNames.insert(pair<int, string>(4, "���ÿ��������ָ��������ã�������"));
		m_mapNames.insert(pair<int, string>(5, "��ȡʱ��"));
		m_mapNames.insert(pair<int, string>(6, "�Ա� ��ȡ���������������ò���"));
		m_mapNames.insert(pair<int, string>(7, "�㲥��ʽ������������ͨ�ţ���ȡ���п���������Ϣ"));
		m_mapNames.insert(pair<int, string>(8, "�㲥��ʽ������������ͨ��"));
		m_mapNames.insert(pair<int, string>(9, "ɾ��һ������������ɾ���͸ÿ�������ص�ģ�顢��"));
		m_mapNames.insert(pair<int, string>(10, "����Զ�����������س���"));
		m_mapNames.insert(pair<int, string>(11, "�ڿ�����������һ��ģ�鲢���ú�ģ�����Ӧ����"));
		m_mapNames.insert(pair<int, string>(12, "����ģ����������ò���"));
		m_mapNames.insert(pair<int, string>(13, "ɾ��һ��ģ�飬��ɾ����ģ����صĵ�"));
		m_mapNames.insert(pair<int, string>(14, "��ȡģ�������������Ϣ"));
		m_mapNames.insert(pair<int, string>(15, "��ȡ�������е����ܲ���"));
		m_mapNames.insert(pair<int, string>(16, "��ȡ������ʷ�����ܲ���"));
		m_mapNames.insert(pair<int, string>(17, "ʵʱ��ȡ�ɼ����ı�����Ϣ"));

	}
	return m_mapNames.size();
}
