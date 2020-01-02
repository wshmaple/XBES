#include "stdafx.h"
#include "CMsgLog.h"


CMsgLog::CMsgLog()
{
	m_bShow = false;
	m_nInCount = 0;
	m_nOutCount = 0;
	m_nTotal = 0;
	m_nErrorCount = 0;
	m_nOkCount = 0;
}


CMsgLog::~CMsgLog()
{
}

string CMsgLog::GetNowString()
{
	tm ptminfo;
	time_t now;
	time(&now);
	localtime_s(&ptminfo, &now);
	char ltmweb[256] = { 0 };
	sprintf_s(ltmweb, " %02d-%02d-%02d %02d:%02d:%02d\n",
		ptminfo.tm_year + 1900, ptminfo.tm_mon + 1, ptminfo.tm_mday,
		ptminfo.tm_hour, ptminfo.tm_min, ptminfo.tm_sec);
	string strRt = ltmweb;
	return strRt;
}



void CMsgLog::DataIN(int nIndex, string strWebMsg)
{
	m_mapStates[nIndex] = 1;
	m_mapCounts[nIndex] ++;
	m_strLastMsgFormWebTime = "";
	m_strLastMsgFromMac = "";
	m_strLastMsgFromMacTime = "";
	m_strLastMsgFromWeb = "";

	m_nInCount++;

	m_strLastMsgFromWeb = strWebMsg;
	m_strLastMsgFormWebTime = GetNowString();
	m_mapSecond[nIndex] = GetNowString();

	
}

void CMsgLog::DataOut(int nIndex, string strMacMsg, string strMethodName)
{
	m_strLastMsgFromMacTime = GetNowString();
	m_mapSecond2[nIndex] = GetNowString();
	m_mapStates[nIndex] = 0;
	m_strLastMsgFromMac = strMacMsg;
	m_strMethodName = strMethodName;
	m_nOutCount++;
}

void CMsgLog::DataOut(int nIndex, string strMacMsg, string strMethodName, bool bOk)
{
	m_strLastMsgFromMacTime = GetNowString();
	m_mapSecond2[nIndex] = GetNowString();
	m_mapStates[nIndex] = 0;
	m_strLastMsgFromMac = strMacMsg;
	m_strMethodName = strMethodName;
	m_nOutCount++;
	if (bOk)
	{
		m_nOkCount++;
		m_mapRtOkCount[nIndex]++;
	}
	else{
		m_nErrorCount++;
		m_mapRtErrorCount[nIndex]++;
	}
}

void CMsgLog::SetSize(int nSize)
{
	for (int i = 0; i < nSize; i++)
	{
		m_mapStates.insert(pair<int, int>(i, 0));
		m_mapCounts.insert(pair<int, int>(i, 0));
		m_mapSecond.insert(pair<int, string>(i, ""));
		m_mapSecond2.insert(pair<int, string>(i, ""));
		m_mapRtOkCount.insert(pair<int, int>(i, 0));
		m_mapRtErrorCount.insert(pair<int, int>(i, 0));
	}
}
