#pragma once
class CMsgLog
{
public:
	CMsgLog();
	~CMsgLog();

	map<int, int > m_mapStates;// 状态
	map<int, int > m_mapCounts;// 次数
	map<int, string > m_mapSecond;// 时间戳 接收
	map<int, string > m_mapSecond2;// 时间戳 反馈
	map<int, int > m_mapRtOkCount;// 正确返回次数
	map<int, int > m_mapRtErrorCount;// 错误返回次数 

	// 最近一条 数据的 接收和反馈
	string m_strLastMsgFromWeb; // 最近一条 web 消息

	string m_strLastMsgFromMac; // 最近 反馈 web 消息

	string m_strLastMsgFormWebTime; // 最近 web 消息时间
	string m_strLastMsgFromMacTime; // 最近反馈 web 消息时间

	string m_strMsgType; // 类型描述 灯光/楼宇
	string m_strMsgIP;   // 当前的 IP 信息
	string m_strMsgPort; // 信息的端口号

	string m_strMethodName;// 方法名称

	bool m_bShow;

	int m_nTotal; // 总的调用次数
	int m_nOkCount;    // 调用正确的次数
	int m_nErrorCount; // 调用错误的次数

	int m_nInCount; // 调用次数
	int m_nOutCount; // 反馈次数


//	CTime m_lastTime;
public:
	string GetNowString();
	void DataIN(int nIndex,string strWebMsg);//消息进入

	void DataOut(int nIndex,string strMacMsg,string strMethodName);//消息结果反馈
	void DataOut(int nIndex,string strMacMsg,string strMethodName,bool bOk);//消息结果反馈

	void SetSize(int nSize);

};

