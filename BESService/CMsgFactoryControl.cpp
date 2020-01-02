#include "stdafx.h"
#include "CMsgFactoryControl.h"


CMsgFactoryControl::CMsgFactoryControl()
{
	m_bshow = false;
}


CMsgFactoryControl::~CMsgFactoryControl()
{
}

int CMsgFactoryControl::GetSize()
{
	return m_map.size();
}

bool CMsgFactoryControl::Add(string strType)
{
	if (Get(strType)==NULL)
	{
		CMsgFactory cf;
		cf.DoInNames(strType);
		m_map.insert(pair<string, CMsgFactory>(strType, cf));
		return true;
	}
	return false;
}

CMsgFactory * CMsgFactoryControl::Get(string strType)
{
	std::map<string, CMsgFactory>::iterator iter = m_map.find(strType);
	if (m_map.end() != iter)
		return &(iter->second);
	return NULL;
}

void CMsgFactoryControl::DataIN(string strtype, string strIP, int nIndex, string strWebMsg)
{
	if (Get(strtype))
	{
		Get(strtype)->DataIN(strtype,strIP, nIndex, strWebMsg);
	}
	else{
		if (Add(strtype)) {
			Get(strtype)->DataIN(strtype, strIP, nIndex, strWebMsg);
		}
	}
}

void CMsgFactoryControl::DataOut(string strtype, string strIP, int nIndex, string strWebMsg, string strMacMsg)
{
	if (Get(strtype))
	{
		Get(strtype)->DataOut(strtype, strIP, nIndex, strWebMsg, strMacMsg);
	}
	else {
		if (Add(strtype)) {
			Get(strtype)->DataOut(strtype, strIP, nIndex, strWebMsg, strMacMsg);
		}
	}
}

void CMsgFactoryControl::DataOut(string strtype, string strIP, int nIndex, string strWebMsg, string strMacMsg, bool bOk)
{
	if (Get(strtype))
	{
		Get(strtype)->DataOut(strtype, strIP, nIndex, strWebMsg, strMacMsg,bOk);
	}
	else {
		if (Add(strtype)) {
			Get(strtype)->DataOut(strtype, strIP, nIndex, strWebMsg, strMacMsg,bOk);
		}
	}
}
