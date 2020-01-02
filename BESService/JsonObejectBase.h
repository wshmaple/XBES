#pragma once

#include <string>
#include <vector>
#include "json/json.h"
#include <wtypes.h>
using std::string;
using std::vector;

class CJsonObejectBase
{
public:
	CJsonObejectBase();
	~CJsonObejectBase();


protected:
	enum CEnumJsonTypeMap
	{
		asInt = 1,
		asUInt,
		asString,
		asDouble,
		asBool
	};
public:
//	CJsonObejectBase(void){}
public:
//	virtual ~CJsonObejectBase(void){}
	string Serialize()
	{
		Json::Value new_item;
		int nSize = m_listName.size();
		for (int i = 0; i < nSize; ++i)
		{
			void* pAddr = m_listPropertyAddr[i];
			switch (m_listType[i])
			{
			case asInt:
				new_item[m_listName[i]] = (*(INT*)pAddr);
				break;
			case asUInt:
				new_item[m_listName[i]] = (*(UINT*)pAddr);
				break;
			case asString:
				new_item[m_listName[i]] = (*(string*)pAddr);
			default:
				//我暂时只支持这几种类型，需要的可以自行添加 
				break;
			}
		}
		Json::FastWriter writer;
		std::string out2 = writer.write(new_item);
		return out2;
	}

	bool DeSerialize(const char* str)

	{
		Json::Reader reader;
		Json::Value root;
		if (reader.parse(str, root))
		{
			int nSize = m_listName.size();
			for (int i = 0; i < nSize; ++i)
			{
				void* pAddr = m_listPropertyAddr[i];

				switch (m_listType[i])
				{
				case asInt:
					(*(INT*)pAddr) = root.get(m_listName[i], 0).asInt();
					break;
				case asUInt:
					(*(UINT*)pAddr) = root.get(m_listName[i], 0).asUInt();
					break;
				case  asDouble:
					(*(DOUBLE*)pAddr) = root.get(m_listName[i], 0).asDouble();
					break;
				case  asBool:
					(*(bool*)pAddr) = root.get(m_listName[i], 0).asBool();
					break;
				case asString:
					(*(string*)pAddr) = root.get(m_listName[i], "").asString();
					break;
				default:
					//我暂时只支持这几种类型，需要的可以自行添加 
					break;
				}
			}
			return true;
		}
		return false;
	}
protected:
	void SetProperty(string name, CEnumJsonTypeMap type, void* addr)
	{
		m_listName.push_back(name);
		m_listPropertyAddr.push_back(addr);
		m_listType.push_back(type);
	}
	virtual void SetPropertys() = 0;
	vector<string> m_listName;
	vector<void*>  m_listPropertyAddr;
	vector<CEnumJsonTypeMap>	   m_listType;

};

