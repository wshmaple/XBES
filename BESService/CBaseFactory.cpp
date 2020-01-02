#include "stdafx.h"
#include "CBaseFactory.h"


CBaseFactory::CBaseFactory()
{
	m_threadpoolCross.SetPoolSize(20);// 开10个数据处理线程
	m_threadpoolLamp.SetPoolSize(20);// 开10个数据处理线程
	m_threadpoolNengHao.SetPoolSize(20); // 开启10个数据处理线程
}


CBaseFactory::~CBaseFactory()
{ 
	map<string, CCrossDataManager*>::iterator itr =m_mapCross.begin();
	while (itr!= m_mapCross.end())
	{
		delete itr->second;
		itr->second = NULL;
		itr++;
	}
	m_mapCross.clear();

	map<string, CLampDataManager*>::iterator itr2 = m_mapLamp.begin();
	while (itr2 != m_mapLamp.end())
	{
		delete itr2->second;
		itr2->second = NULL;
		itr2++;
	}
	m_mapLamp.clear();

	map<string, CDdataManager*>::iterator itr3 = m_mapNengHao.begin();
	while (itr3 != m_mapNengHao.end())
	{
		delete itr3->second;
		itr3->second = NULL;
		itr3++;
	}
	m_mapNengHao.clear();
}

string CBaseFactory::Run(string strMsg)
{
	return "";
}

// 拆分数据 类型 一条主线程 肯定是不行的 遇上那种 阻塞的 肯定完蛋了
// 耗时操作 解包、发送、回收、封包、
void CBaseFactory::DoParseWebJson(std::string ConfigInfo, std::string& result)
{
/*	LOG_INFO("接收JAVA 数据：" << ConfigInfo.c_str());
	Json::Value jsonResult;
	Json::Reader reader;
	Json::Value root;
	int nIndex = -1;

	if (reader.parse(ConfigInfo, root))
	{
		bool bItem = root["item"].empty();

		if (root["index"].isInt())
		{
			nIndex = root["index"].asUInt();
		}
		else if (root["index"].isString())
		{
			nIndex = atoi(root["index"].asString().c_str());
		}

		if (!bItem)
		{
			printf("bItem %d \n", bItem);
			string strItem = root["item"].asString();
			string strIp = root["IPAddr"].asString();
			if (strItem == "cross")// 楼宇
			{


				m_ui.m_msgLogCross.DataIN(nIndex, ConfigInfo);

				m_ui.m_msgFC.DataIN(strItem, strIp, nIndex, ConfigInfo);

				jsonResult = m_crossDataManager.ComDataManager(root);


				result = jsonResult.toStyledString();
				char cTmpPrint[25500] = { 0 };
				sprintf_s(cTmpPrint, "--index:%d\t\n--%s \t\n webmsg-- %s \t\n --return\t %s \n", nIndex, m_ui.m_msgLogCross.m_mapNames[nIndex].c_str(), ConfigInfo.c_str(), result.c_str());

				LOG_INFO(cTmpPrint);
				
				m_ui.m_msgLogCross.DataOut(nIndex, cTmpPrint);
				m_ui.m_msgFC.DataOut(strItem, strIp, nIndex, cTmpPrint);

				return ;
			}
			else if (strItem == "lamp")//灯光
			{
				
				m_ui.m_msgLogLamp.DataIN(nIndex, ConfigInfo);

				m_ui.m_msgFC.DataIN(strItem, strIp, nIndex, ConfigInfo);

				jsonResult = g_lampDataManager.ComDataManager(root);



				result = jsonResult.toStyledString();
				char cTmpPrint[25500] = { 0 };
				sprintf_s(cTmpPrint, "--index:%d\t\n--%s \t\n webmsg-- %s \t\n --return\t %s \n", nIndex,m_ui.m_msgLogLamp.m_mapNames[nIndex].c_str(), ConfigInfo.c_str(), result.c_str());

				LOG_INFO(cTmpPrint);
				m_ui.m_msgLogLamp.DataOut(nIndex, cTmpPrint);
				m_ui.m_msgFC.DataOut(strItem, strIp, nIndex, cTmpPrint);

				return ;
			}
		}
		else{
			jsonResult = m_dbManager.ComDataManager(root);
			result = jsonResult.toStyledString();
			char cTmpPrint[25500] = { 0 };
			sprintf_s(cTmpPrint, "%d-- \t\n -- %s \t\n --return\t %s \n", nIndex,  ConfigInfo.c_str(), result.c_str());

			LOG_INFO(cTmpPrint);


		}
	}*/
}

void CBaseFactory::DoParseWebJsonNengHao(Json::Value root, std::string& result, CDdataManager*p)
{
//	result = p->ComDataManager(root).toStyledString();

	p->ComDataManager2(root, result);
}



void CBaseFactory::DoParseWebJsonCross(Json::Value root, std::string& result, CCrossDataManager*p)
{
	result = p->ComDataManager(root).toStyledString();
}

void CBaseFactory::DoParseWebJsonLamp(Json::Value root, std::string& result, CLampDataManager*p)
{
	result = p->ComDataManager(root).toStyledString();
}

void CBaseFactory::DoThreadParse(std::string ConfigInfo, std::string &result)
{
//	auto rtBuf = m_threadpool.enqueue([this,&result](std::string ConfigInfo) {
//		DoParseWebJson(ConfigInfo, result);
//		return	0;
//	}, ConfigInfo);
//	int nTTSize = rtBuf.get();
	//return;

//	LOG_INFO("接收JAVA 数据：" << ConfigInfo.c_str());
	Json::Value jsonResult;
	Json::Reader reader;
	Json::Value root;
	int nIndex = -1;

	if (reader.parse(ConfigInfo, root))//
	{
		bool bItem = root["item"].empty();

		if (root["index"].isInt())
		{
			nIndex = root["index"].asUInt();
		}
		else if (root["index"].isString())
		{
			nIndex = atoi(root["index"].asString().c_str());
		}

		if (!bItem)
		{
		//	printf("bItem %d \n", bItem);
			string strItem = root["item"].asString();
			string strIp = root["IPAddr"].asString();
			if (strItem == "cross")// 楼宇
			{		
				//m_ui.m_msgLogCross.DataIN(nIndex, root.toStyledString().c_str());
				m_ui.m_msgFC.DataIN(strItem, strIp, nIndex, root.toStyledString().c_str());
				map<string, CCrossDataManager*> ::iterator itr= m_mapCross.find(strIp);
				if (m_mapCross.end() != itr) {
					CCrossDataManager *p = itr->second;
					//auto rtBuf = m_threadpoolCross.enqueue([this,&result, p](Json::Value root) {
					//	DoParseWebJsonCross(root, result,p);
					//	return	0;
					//}, root)	
					//int nTTSize = rtBuf.get();

					DoParseWebJsonCross(root, result, p);
					bool bOk = false;
					Json::Reader readers;
					Json::Value roots;
					readers.parse(result, roots);
					if (roots["code"].asInt()==0 && roots["hcode"].asInt()==0)
					{
						bOk = true;
					}
					m_ui.m_msgFC.DataOut(strItem, strIp, nIndex, root.toStyledString(), result, bOk);

				}
				else{
					CCrossDataManager *pCross = new CCrossDataManager();
					m_mapCross.insert(make_pair(strIp, pCross));

					map<string, CCrossDataManager*> ::iterator itr = m_mapCross.find(strIp);
					if (m_mapCross.end() != itr) {
						CCrossDataManager *p = itr->second;
						// auto rtBuf = m_threadpoolCross.enqueue([this, &result, p](Json::Value root) {
						// 	DoParseWebJsonCross(root, result, p);
						// 	return	0;
						// }, root);
						// int nTTSize = rtBuf.get();

						DoParseWebJsonCross(root, result, p);
						bool bOk = false;
						Json::Reader readers;
						Json::Value roots;
						readers.parse(result, roots);
						if (roots["code"].asInt() == 0 && roots["hcode"].asInt() == 0)
						{
							bOk = true;
						}
						m_ui.m_msgFC.DataOut(strItem, strIp, nIndex, root.toStyledString(), result, bOk);

					}

					//auto rtBuf = m_threadpoolCross.enqueue([this, &result, pCross](Json::Value root) {
					//	DoParseWebJsonCross(root, result, pCross);
					//	return	0;
					//}, root);
					//int nTTSize = rtBuf.get();
					//m_ui.m_msgFC.DataOut(strItem, strIp, nIndex, root.toStyledString(), result);

				//	result = pCross->ComDataManager(root).toStyledString();
				} 	
			//	jsonResult = m_crossDataManager.ComDataManager(root);
 			//	char cTmpPrint[25500] = { 0 };
 			//	sprintf_s(cTmpPrint, "--index:%d\t\n--%s \t\n webmsg-- %s \t\n --return\t %s \n", nIndex, m_ui.m_msgLogCross.m_mapNames[nIndex].c_str(), root.toStyledString().c_str(), result.c_str());
 			//	LOG_INFO(cTmpPrint);
 			//  m_ui.m_msgLogCross.DataOut(nIndex, cTmpPrint);
				return;
			}
			else if (strItem == "lamp")//灯光
			{
				// 开三条 线程不停的处理
				//m_ui.m_msgLogCross.DataIN(nIndex, root.toStyledString().c_str());
				m_ui.m_msgFC.DataIN(strItem, strIp, nIndex, root.toStyledString().c_str());
				map<string, CLampDataManager*> ::iterator itr = m_mapLamp.find(strIp);
				if (m_mapLamp.end() != itr) {
					CLampDataManager *p = itr->second;
					//result = p->ComDataManager(root).toStyledString();
					auto rtBuf = m_threadpoolLamp.enqueue([this, &result, p](Json::Value root) {
						DoParseWebJsonLamp(root, result, p);
						return	0;
					}, root);
					int nTTSize = rtBuf.get();
					bool bOk = false;
					Json::Reader readers;
					Json::Value roots;
					readers.parse(result, roots);
					if (roots["code"].asInt() == 0 && roots["hcode"].asInt() == 0)
					{
						bOk = true;
					}
					m_ui.m_msgFC.DataOut(strItem, strIp, nIndex, root.toStyledString(), result, bOk);

				}
				else {
					CLampDataManager *pCross = new CLampDataManager();
					m_mapLamp.insert(make_pair(strIp, pCross));

					//auto rtBuf = m_threadpoolLamp.enqueue([this, &result, pCross](Json::Value root) {
					//	DoParseWebJsonLamp(root, result, pCross);
					//	return	0;
					//}, root);
					//int nTTSize = rtBuf.get();
					//m_ui.m_msgFC.DataOut(strItem, strIp, nIndex, root.toStyledString(), result);

					map<string, CLampDataManager*> ::iterator itr = m_mapLamp.find(strIp);
					if (m_mapLamp.end() != itr) {
						CLampDataManager *p = itr->second;
						//result = p->ComDataManager(root).toStyledString();
						auto rtBuf = m_threadpoolLamp.enqueue([this, &result, p](Json::Value root) {
							DoParseWebJsonLamp(root, result, p);
							return	0;
						}, root);
						int nTTSize = rtBuf.get();
						bool bOk = false;
						Json::Reader readers;
						Json::Value roots;
						readers.parse(result, roots);
						if (roots["code"].asInt() == 0 && roots["hcode"].asInt() == 0)
						{
							bOk = true;
						}
						m_ui.m_msgFC.DataOut(strItem, strIp, nIndex, root.toStyledString(), result, bOk);
					}

				}
				//char cTmpPrint[25500] = { 0 };
				//sprintf_s(cTmpPrint, "--index:%d\t\n--%s \t\n webmsg-- %s \t\n --return\t %s \n", nIndex, m_ui.m_msgLogCross.m_mapNames[nIndex].c_str(), root.toStyledString().c_str(), result.c_str());
				//LOG_INFO(cTmpPrint);
				//m_ui.m_msgLogCross.DataOut(nIndex, cTmpPrint);
				

				return;
			}
			else if (strItem == "energy")//能耗
			{
				string strIp = root["IPAddr"].asString();
				m_ui.m_msgFC.DataIN("energy", strIp, nIndex, root.toStyledString().c_str());
				map<string, CDdataManager*> ::iterator itr = m_mapNengHao.find(strIp);
				if (m_mapNengHao.end() != itr) {
					CDdataManager *p = itr->second;
					//result = p->ComDataManager(root).toStyledString();
					auto rtBuf = m_threadpoolNengHao.enqueue([this, &result, p](Json::Value root) {
						//DoParseWebJsonNengHao(root, result, p);
						p->ComDataManager2(root, result);
						return	0;
					}, root);
					int nTTSize = rtBuf.get();
					bool bOk = false;
					Json::Reader readers;
					Json::Value roots;
					readers.parse(result, roots);
					if (roots["errmsg"].asString() == "操作成功")
					{
						bOk = true;
					}
					m_ui.m_msgFC.DataOut("energy", strIp, nIndex, root.toStyledString(), result, bOk);
				//	m_ui.m_msgFC.DataOut("energy", strIp, nIndex, root.toStyledString(), result);
				}
				else {
					CDdataManager *pEnergy = new CDdataManager();
					m_mapNengHao.insert(make_pair(strIp, pEnergy));

					/*
					auto rtBuf = m_threadpoolNengHao.enqueue([this, &result, pEnergy](Json::Value root,string &result) {
						pEnergy->ComDataManager2(root, result);
						return	0;
					}, root, result);
					int nTTSize = rtBuf.get();
					m_ui.m_msgFC.DataOut("energy", strIp, nIndex, root.toStyledString(), result);
					*/
					map<string, CDdataManager*> ::iterator itr = m_mapNengHao.find(strIp);
					if (m_mapNengHao.end() != itr) {
						CDdataManager *p = itr->second;
						auto rtBuf = m_threadpoolNengHao.enqueue([this, &result, p](Json::Value root) {
							p->ComDataManager2(root, result);
							return	0;
						}, root);
						int nTTSize = rtBuf.get();
						bool bOk = false;
						Json::Reader readers;
						Json::Value roots;
						readers.parse(result, roots);
						if (roots["errmsg"].asString() == "操作成功")
						{
							bOk = true;
						}
						m_ui.m_msgFC.DataOut("energy", strIp, nIndex, root.toStyledString(), result, bOk);
						//m_ui.m_msgFC.DataOut("energy", strIp, nIndex, root.toStyledString(), result);
					}


				}
				//	char cTmpPrint[25500] = { 0 };
				//	sprintf_s(cTmpPrint, "--index:%d\t\n--%s \t\n webmsg-- %s \t\n --return\t %s \n", nIndex, m_ui.m_msgLogCross.m_mapNames[nIndex].c_str(), root.toStyledString().c_str(), result.c_str());
				//	LOG_INFO(cTmpPrint);
				return;

			}
		}
		else { // 下面的部分代码 等 energy 确认后 可用删除
// 			jsonResult = m_dbManager.ComDataManager(root);
// 			result = jsonResult.toStyledString();
// 			char cTmpPrint[25500] = { 0 };
// 			sprintf_s(cTmpPrint, "%d-- \t\n -- %s \t\n --return\t %s \n", nIndex, ConfigInfo.c_str(), result.c_str());
// 
// 			LOG_INFO(cTmpPrint);
			string strIp = root["IPAddr"].asString();
			m_ui.m_msgFC.DataIN("energy", strIp, nIndex, root.toStyledString().c_str());
			map<string, CDdataManager*> ::iterator itr = m_mapNengHao.find(strIp);
			if (m_mapNengHao.end() != itr) {
				CDdataManager *p = itr->second;
				//result = p->ComDataManager(root).toStyledString();


				auto rtBuf = m_threadpoolNengHao.enqueue([this, &result, p](Json::Value root) {
					DoParseWebJsonNengHao(root, result, p);
					return	0;
				}, root);
				int nTTSize = rtBuf.get();

				bool bOk = false;
				Json::Reader readers;
				Json::Value roots;
				readers.parse(result, roots);
				if (roots["errmsg"].asString() == "操作成功")
				{
					bOk = true;
				}
				m_ui.m_msgFC.DataOut("energy", strIp, nIndex, root.toStyledString(), result, bOk);

			//	m_ui.m_msgFC.DataOut("energy", strIp, nIndex, root.toStyledString(), result);

			//	DoParseWebJsonNengHao(root, result, p);
			}
			else {
				CDdataManager *pCross = new CDdataManager();
				m_mapNengHao.insert(make_pair(strIp, pCross));

				auto rtBuf = m_threadpoolNengHao.enqueue([this, pCross](Json::Value root, string &result) {
					DoParseWebJsonNengHao(root, result, pCross);
					return	0;
				}, root,result);
				int nTTSize = rtBuf.get();
				bool bOk = false;
				Json::Reader readers;
				Json::Value roots;
				readers.parse(result, roots);
				if (roots["errmsg"].asString() == "操作成功")
				{
					bOk = true;
				}
				m_ui.m_msgFC.DataOut("energy", strIp, nIndex, root.toStyledString(), result, bOk);
			//	m_ui.m_msgFC.DataOut("energy", strIp, nIndex, root.toStyledString(), result);
				
			}
		//	char cTmpPrint[25500] = { 0 };
		//	sprintf_s(cTmpPrint, "--index:%d\t\n--%s \t\n webmsg-- %s \t\n --return\t %s \n", nIndex, m_ui.m_msgLogCross.m_mapNames[nIndex].c_str(), root.toStyledString().c_str(), result.c_str());
		//	LOG_INFO(cTmpPrint);
			

			return;
		}
	}
}
