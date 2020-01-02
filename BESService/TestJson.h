#pragma once
#include "JsonObejectBase.h"
class CTestJson :
	public CJsonObejectBase
{
public:
	CTestJson();
	~CTestJson();

	bool MsgID;
	string MsgTitle;
	string MsgContent;
	string ss;
protected:
	//子类需要实现此函数，并且将相应的映射关系进行设置 
	virtual void SetPropertys()
	{
		SetProperty("MsgID", asBool, &MsgID);
		SetProperty("MsgTitle", asString, &MsgTitle);
		SetProperty("MsgContent", asString, &MsgContent);
	}
};