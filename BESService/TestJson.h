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
	//������Ҫʵ�ִ˺��������ҽ���Ӧ��ӳ���ϵ�������� 
	virtual void SetPropertys()
	{
		SetProperty("MsgID", asBool, &MsgID);
		SetProperty("MsgTitle", asString, &MsgTitle);
		SetProperty("MsgContent", asString, &MsgContent);
	}
};