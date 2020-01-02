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
		m_mapNames.insert(pair<int, string>(0, "新增一个控制器"));
		m_mapNames.insert(pair<int, string>(1, "设置控制器的所有参数"));
		m_mapNames.insert(pair<int, string>(2, "设置控制器的时间"));
		m_mapNames.insert(pair<int, string>(3, "重启控制器，相当于复位"));
		m_mapNames.insert(pair<int, string>(4, "重置控制器，恢复出厂设置，并重启"));
		m_mapNames.insert(pair<int, string>(5, "删除一个控制器，并删除和它相关的模块和点"));
		m_mapNames.insert(pair<int, string>(6, "获取控制器的时间"));
		m_mapNames.insert(pair<int, string>(7, "获取控制器的所有配置参数"));
		m_mapNames.insert(pair<int, string>(8, "通过UDP广播形式获取所有控制器的信息"));
		m_mapNames.insert(pair<int, string>(9, "通过UDP广播形式设置控制器的参数"));
		m_mapNames.insert(pair<int, string>(10, "新增加一个模块"));
		m_mapNames.insert(pair<int, string>(11, "设置一个模块的所有参数"));
		m_mapNames.insert(pair<int, string>(12, "删除一个模块，并删除和模块相关的点"));
		m_mapNames.insert(pair<int, string>(13, "获取一个模块的所有配置信息"));
		m_mapNames.insert(pair<int, string>(14, "获取一个模块的错误代码"));
		m_mapNames.insert(pair<int, string>(15, "新增加一个点"));
		m_mapNames.insert(pair<int, string>(16, "设置一个点的所有参数"));
		m_mapNames.insert(pair<int, string>(17, "设置一个点的值"));
		m_mapNames.insert(pair<int, string>(18, "设置点的值，根据点的名字"));
		m_mapNames.insert(pair<int, string>(19, "删除一个点"));
		m_mapNames.insert(pair<int, string>(20, "获取一个点的所有配置参数"));
		m_mapNames.insert(pair<int, string>(21, "获取一个点的报警信息"));
		m_mapNames.insert(pair<int, string>(22, "增加一个场景"));
		m_mapNames.insert(pair<int, string>(23, "设置一个场景的所有参数"));
		m_mapNames.insert(pair<int, string>(24, "删除一个场景"));
		m_mapNames.insert(pair<int, string>(25, "获取一个场景的配置信息"));
		m_mapNames.insert(pair<int, string>(26, "增加一条计划"));
		m_mapNames.insert(pair<int, string>(27, "修改一条计划的所有参数"));
		m_mapNames.insert(pair<int, string>(28, "删除一条计划"));
		m_mapNames.insert(pair<int, string>(29, "获取一条计划的所有参数"));
		m_mapNames.insert(pair<int, string>(30, "远程升级"));

	}
	else if (strType == "energy")
	{
		m_mapNames.insert(pair<int, string>(0, "新增一个控制器"));
		m_mapNames.insert(pair<int, string>(1, "设置控制器的所有参数(同步)"));
		m_mapNames.insert(pair<int, string>(2, "设置时间"));
		m_mapNames.insert(pair<int, string>(3, "重启DDC,相当于软件复位"));
		m_mapNames.insert(pair<int, string>(4, "重置控制器，恢复出厂设置，并重启"));
		m_mapNames.insert(pair<int, string>(5, "获取时间"));
		m_mapNames.insert(pair<int, string>(6, "对比 获取控制器的所有配置参数"));
		m_mapNames.insert(pair<int, string>(7, "广播方式与其他控制器通信，获取所有控制器的信息"));
		m_mapNames.insert(pair<int, string>(8, "广播方式与其他控制器通信"));
		m_mapNames.insert(pair<int, string>(9, "删除一个控制器，并删除和该控制器相关的模块、点"));
		m_mapNames.insert(pair<int, string>(10, "进行远程升级，下载程序"));
		m_mapNames.insert(pair<int, string>(11, "在控制器下增加一个模块并配置好模块的相应参数"));
		m_mapNames.insert(pair<int, string>(12, "配置模块的所有配置参数"));
		m_mapNames.insert(pair<int, string>(13, "删除一个模块，并删除和模块相关的点"));
		m_mapNames.insert(pair<int, string>(14, "获取模块的所有配置信息"));
		m_mapNames.insert(pair<int, string>(15, "获取电表的所有电表电能参数"));
		m_mapNames.insert(pair<int, string>(16, "获取电表的历史电表电能参数"));
		m_mapNames.insert(pair<int, string>(17, "实时获取采集器的报警信息"));

	}
	return m_mapNames.size();
}
