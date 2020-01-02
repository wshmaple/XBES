#pragma once
// 消息工厂 管理类
#include "CMsgFactory.h"
class CMsgFactoryControl
{
public:
	CMsgFactoryControl();
	~CMsgFactoryControl();

	map <string ,CMsgFactory> m_map;
	int GetSize();// 获取大小

	bool Add(string strType);// 增加一个类型的消息工厂

	CMsgFactory * Get(string strType);// 获取一个类型的工厂
	bool m_bshow;// 当前类型的设备 是否显示

	// 增加一条消息
	void DataIN(string strtype,string strIP, int nIndex, string strWebMsg);//消息进入
	void DataOut(string strtype, string strIP, int nIndex, string strWebMsg, string strMacMsg);//消息结果反馈
	void DataOut(string strtype, string strIP, int nIndex, string strWebMsg, string strMacMsg,bool bOk);//消息结果反馈
};

