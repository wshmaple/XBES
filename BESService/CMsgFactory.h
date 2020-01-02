#pragma once
// 消息管理 类
#include "CMsgLog.h"
#include <map>
#include <string>
class CMsgFactory
{
public:
	CMsgFactory();
	~CMsgFactory();
	
	void Add(string strIP,string strType); // 增加一个
	CMsgLog * GetOneMsgLog(string strIP); // 返回对象指针
	int  GetSize();// 获取长度大小
	void DataIN(string strtype, string strIP , int nIndex, string strWebMsg);//消息进入
	void DataOut(string strtype, string strIP, int nIndex, string strWebMsg, string strMacMsg);//消息结果反馈
	void DataOut(string strtype, string strIP, int nIndex, string strWebMsg, string strMacMsg,bool bOk);//消息结果反馈
	int DoInNames(string strType);

	map<string, CMsgLog> m_map; // 保存 一组信息灯光

	map<int, string> m_mapNames;
	// 
	bool m_bShow;// 当前设备 是否显示

private:
	
};

