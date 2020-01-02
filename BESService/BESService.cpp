// BESService.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "soapH.h"
#include "add.nsmap"
#include "json/json.h"
#include "CBaseFactory.h"
#include "resource.h"
#include "cmdline.h"

//#include "CXWebMsgQueen.h";
CBaseFactory g_baseFactory;
#define  THREAD_GSOAP_COUNT 300 // gsoap 处理线程个数
ThreadPool g_threadsoap;// 耗时处理用的线程
//CXWebMsgQueen g_webmsg;

//server端的实现函数与add.h中声明的函数相同，但是多了一个当前的soap连接的参数
int ns__GetInfo(struct soap *add_soap, std::string ConfigInfo, std::string& result)
{

//	LOG_INFO(this_thread::get_id() << "接收JAVA 数据：");
//	LOG_INFO(this_thread::get_id() << "接收并开始处理JAVA 数据：" );
//	Sleep(4000);
	LOGW(ConfigInfo.c_str());
	DWORD start_time = GetTickCount();

	g_baseFactory.DoThreadParse(ConfigInfo, result);
//	Sleep(4000);
	DWORD end_time = GetTickCount();
	DWORD dw = end_time - start_time;


	LOGI("1、" << "处理完成 JAVA 数据：用时" << dw << "结果" << result);
	LOGI("2、"<< "处理完成 JAVA 数据：结果" << result);

	//LOGI(""  << "处理完成 JAVA 数据：结果" << result);
	return 0;
}

int TestThreadRecv(std::string ConfigInfo, std::string& result){
	DWORD start_time = GetTickCount();
	g_baseFactory.DoThreadParse(ConfigInfo, result);
	DWORD end_time = GetTickCount();
	DWORD dw = end_time - start_time;
//	LOG_INFO(this_thread::get_id() << "用时" << dw);
	return 0;
}

DWORD WINAPI TTT(LPVOID lp){
	string str = " {\"IPAddr\" : \"172.16.14.110\",\"Port\" : \"1204\",\"data\" : {\"ID\" : 5},\"index\" : 13,\"item\" : \"cross\"}";
	string result = "";

	string str2 = "{\"IPAddr\":\"172.16.14.100\",\"Port\":\"6060\",\"index\":\"16\",\"Selectday\":\"3\",\"MeterID\":\"11111\"}";
	DWORD start_time = GetTickCount();
	for (int i = 1;i<1000;i++)
	{
		TestThreadRecv(str2, result);
	}
	DWORD end_time = GetTickCount();
	DWORD dw = end_time - start_time;
//	LOGI(this_thread::get_id() << "1000个数处理完成用时" << dw);
	return 0;
}
DWORD WINAPI TTT2(LPVOID lp){
	string str = " {\"IPAddr\":\"172.16.14.110\",\"index\":18,\"item\":\"cross\",\"data\":{\"Value\":255,\"Name\":\"DDC1Y_M201\"},\"Port\":\"1204\"}";
	string result = "";
	DWORD start_time = GetTickCount();

	TestThreadRecv(str, result);

	string str2 = " {\"IPAddr\" : \"172.16.14.110\",\"Port\" : \"1204\",\"data\" : {\"ID\" : 5},\"index\" : 20,\"item\" : \"cross\"}";

	DWORD end_time = GetTickCount();
	DWORD dw = end_time - start_time;
//	LOG_INFO(this_thread::get_id() << "1000个数处理完成用时" << dw);
	return 0;
}

DWORD WINAPI SoapRun(LPVOID lpParam){
	int m, s;
	int nPort = 7890;//这个是端口号,javaWeb端也要这个端口,与服务端连接
	struct soap platform_soap;
	soap_init(&platform_soap);
	m = soap_bind(&platform_soap, NULL, nPort, 100);
	if (m < 0)
	{
		//ctcp.LogPrint("正晨服务", "gsoap_bind链接异常！");
		soap_print_fault(&platform_soap, stderr);
		fprintf(stderr, "gsoap_bind链接异常！err = %d\n", m);
		exit(-1);
	}

	for (;;)
	{
		s = soap_accept(&platform_soap);
		if (s < 0)
		{
			soap_print_fault(&platform_soap, stderr);
			exit(-1);
		}
		fprintf(stderr, "Socket connection successful: slave socket = %d\n", s);
	//	struct soap *psp2 = NULL;
	//	psp2 = soap_copy(&platform_soap);


		soap_serve(&platform_soap);//该句说明该server的服务 
		soap_end(&platform_soap);
	}

	return 0;
}
//DWORD WINAPI MySoapThread(LPVOID lpParamter){
//	struct soap *psp2 = NULL;
//	psp2 = (struct soap *)lpParamter;
//	if (!soap_valid_socket(psp2->socket))
//	{
//		return 0;
//	}
//	printf("当前运行线程是：%p\n", this_thread::get_id());
//	// 消息处理和销毁
//	soap_serve(psp2);
//	soap_destroy(psp2);
//	soap_end(psp2);
//	return 0;
//}

DWORD WINAPI SoapRun2(LPVOID lpParam) {
	int m, s;
	int nPort = 7890;//这个是端口号,javaWeb端也要这个端口,与服务端连接
	struct soap platform_soap;
	soap_init(&platform_soap);
	platform_soap.accept_timeout = 10000;
	platform_soap.send_timeout = 10000;
	m = soap_bind(&platform_soap, NULL, nPort, 10);
	if (m < 0)
	{
		//ctcp.LogPrint("正晨服务", "gsoap_bind链接异常！");
		soap_print_fault(&platform_soap, stderr);
		fprintf(stderr, "gsoap_bind链接异常！err = %d\n", m);
		exit(-1);
	}
	for (;;)
	{
		s = soap_accept(&platform_soap);
		if (!soap_valid_socket(s))
		{
			if (platform_soap.errnum)
			{
				soap_print_fault(&platform_soap, stderr);
				continue;
			}
			fprintf(stderr, "server timed out\n");
			continue;
		}
		// 拷贝一份
		struct soap *psp2 = NULL;
		psp2 = soap_copy(&platform_soap); //make a safe copy
	
		if (psp2!=NULL)
		{
			// 针对不同连接
			psp2->socket = s;
			auto rtBuf = g_threadsoap.enqueue([psp2] {
				if (!soap_valid_socket(psp2->socket))
				{
					LOGE("soap_valid_socket(psp2->socket)");
					return 0;
				}
				soap_serve(psp2);  // 自动调用接口函数
				soap_destroy(psp2); // dealloc c++ data
				soap_end(psp2); // Dealloc data and clean up
				soap_done(psp2); //Detach soap Struct
				free(psp2); // 释放指针
				return 0;
			});
		}
		else {
			soap_destroy(psp2); // dealloc c++ data
			soap_end(psp2); // Dealloc data and clean up
			soap_done(psp2); //Detach soap Struct
			free(psp2); // 释放指针
			LOGE("platform_soap error！"<< platform_soap.host);
		}	
	}
	return 0;
}

DWORD WINAPI UiRun(LPVOID lp) {
	g_baseFactory.m_ui.UIInit();
	PostQuitMessage(WM_QUIT);
	return 0;
}
/*
void testJsonArray() {
	const char* pstr = "{\"MsgContent\":\"哈哈22\",\"MsgID\":true,\"MsgTitle\":\"黑黑22\",\"MsgTitle3\":\"黑黑22\",\"ids\":[{\"id\":11},{\"id\":12},{\"id\":13},{\"id\":14},{\"id\":15}],\"num\":222}";
	Json::Value va;
	Json::Reader reader;
	reader.parse(pstr, va);


	if (va["ids"].isArray())
	{
		int nSize = va["ids"].size();
		Json::Value vids = va["ids"];

		int ssn = va["num"].asUInt();
		int ssd = va["num"].asUInt();

		int nIdsSize = vids.size();

		cout << vids.toStyledString() << endl;

		

		for (int j =0;j<nIdsSize;j++)
		{
		//	cout << vids[i] << endl;
			Json::Value vt;
			vt = vids[j];
			cout << vt["id"] << endl;

		}

		for (int i = 0;i<nSize;i++)
		{
			Json::Value vv;
			vids.get(i, vv);

		//	int iii = vv.asUInt();
		//	int s = va["ids"][i].asUInt();
		//	cout << s << endl;
		}
	}

}


void TestLamp() {
//	string str = "{ \"IPAddr\": \"172.16.12.10\",\"index\": 6,\"item\": \"cross\",\"data\": {},\"Port\": \"1204\"}";  //获取时间 室内照明 133
	string str = "{ \"IPAddr\": \"172.16.14.9\",\"index\": 7,\"item\": \"cross\",\"data\": {},\"Port\": \"1204\"}";  //获取IP路由器的所有配置参数 测试 OK 了
//	string str = "{ \"IPAddr\": \"172.16.12.134\",\"index\": 13,\"item\": \"cross\",\"data\": {\"ID\": 50},\"Port\": \"1204\"}";  //获取模块的所有信息 ID 50 测试 OK
	//string str = "{\"IPAddr\":\"172.16.14.135\",\"index\":2,\"item\":\"cross\",\"data\":{\"wday\":4,\"month\":9,\"hour\":8,\"year\":2018,\"day\":20,\"minute\":36,\"second\":13},\"Port\":\"1204\"}";  //设置时间

//	string str = "{ \"IPAddr\": \"172.16.12.134\",\"index\": 17,\"item\": \"cross\",\"data\": {\"Value\": 255,\"WorkMode\": 0, \"ID\": 660},\"Port\": \"1204\"}";
	
	// 通过 名字设置 逻辑点的值
	// 设置DO 点 1 的值"ID": 123, //点的ID
	//	"WorkMode": 2, //工作模式，手动模式或自动模式
	//		"Value" : 3, //初始值172.16.14.133
	//string str = "{ \"IPAddr\": \"172.16.14.9\",\"index\": 18,\"item\": \"cross\",\"data\": {\"ID\":1,\"WorkMode\":1,\"Value\":0,\"Name\":\"M81908_1\"},\"Port\": \"1204\"}";  //获取时间 室内照明 133

	Json::Value jsonResult;
	Json::Reader reader;
	Json::Value root;
	int nIndex = -1; 

	if (reader.parse(str, root)) {
		jsonResult =	g_lampDataManager.ComDataManager(root);

		string ss = jsonResult.toStyledString();

		LOG_INFO(ss.c_str());
	}

	//string str2 = "{ \"IPAddr\": \"127.0.0.1\",\"index\": 4,\"item\": \"cross\",\"data\": {},\"Port\": \"1205\"}";
	//if (reader.parse(str2, root)) {
	//	jsonResult = m_dbManager.GetCollectControllerTime(root);
	//	string ss = jsonResult.toStyledString();

	//	LOG_INFO(ss.c_str());
	//}
	
}

void TestLampPoint(string str) {
	Json::Value jsonResult;
	Json::Reader reader;
	Json::Value root;
	int nIndex = -1;

	if (reader.parse(str, root)) {

		nIndex = root["index"].asUInt();
		g_ui.m_msgLogLamp.m_mapStates[nIndex] = 1;
		g_ui.m_msgLogLamp.m_mapCounts[nIndex] ++;

		jsonResult = g_lampDataManager.ComDataManager(root);

		g_ui.m_msgLogLamp.m_mapStates[nIndex] = 0;

		string ss = jsonResult.toStyledString();

		LOG_INFO(ss.c_str());
	}
}
void TestCrossPoint(string str) {
	Json::Value jsonResult;
	Json::Reader reader;
	Json::Value root;
	int nIndex = -1;

	if (reader.parse(str, root)) {

		nIndex = root["index"].asUInt();
		g_ui.m_msgLogCross.m_mapStates[nIndex] = 1;
		g_ui.m_msgLogCross.m_mapCounts[nIndex] ++;

		jsonResult = g_crossDataManager.ComDataManager(root);

		g_ui.m_msgLogCross.m_mapStates[nIndex] = 0;

		string ss = jsonResult.toStyledString();

		LOG_INFO(ss.c_str());
	}
}
#include<mutex>
bool bT = true;
std::mutex g_lock;
std::recursive_mutex mutex;

std::timed_mutex mutex1; // 超时 互斥锁
chrono::milliseconds timeout(10000);// 互斥锁超时时间

DWORD WINAPI  LP111mutex(LPVOID l) {

	int nCount = 0;
	while (nCount<2000)
	{
		nCount++;
	//	printf("LP1 第 %d 条 数据", nCount);
	//	LOG_INFO(this_thread::get_id() <<"第"<< nCount<<"条命令:do work without the mutex");
		if (mutex1.try_lock_for(timeout))
		{

			//cout << this_thread::get_id() << ":do work with the mutex" << endl;

			//LOG_INFO(this_thread::get_id() << ":do work with the mutex");
			LOG_INFO(this_thread::get_id() << "第" << nCount << "条命令:do work with the mutex");

			if (bT)
			{
				string str = "{ \"IPAddr\": \"172.16.13.134\",\"index\": 17,\"item\": \"lamp\",\"data\": {\"Value\": 255,\"WorkMode\": 0, \"ID\": 6},\"Port\": \"1204\"}";
				TestLampPoint(str);
			}
			else {
				string str = "{ \"IPAddr\": \"172.16.13.134\",\"index\": 17,\"item\": \"lamp\",\"data\": {\"Value\": 0,\"WorkMode\": 0, \"ID\": 6},\"Port\": \"1204\"}";
				TestLampPoint(str);
			}
			bT = !bT;

		//	chrono::milliseconds sleepDuration(250);
		//	this_thread::sleep_for(sleepDuration);

			mutex1.unlock();
		//	this_thread::sleep_for(sleepDuration);
		}
		else
		{

		//	cout << this_thread::get_id() << ": do work ------without------- mutex" << endl;
		//	LOG_INFO(this_thread::get_id() << ":do work --------------without--------------- the mutex");
			LOG_INFO(this_thread::get_id() << "第" << nCount << "条命令:do work --------------without--------------- the mutex");

		//	chrono::milliseconds sleepDuration(100);
		//	this_thread::sleep_for(sleepDuration);
		}
	}
	
	
	//for (int i = 0; i < 20000; i++)
	//{
	//	//Sleep(1000);
	//	printf("LP1 第 %d 条 数据", i);
	//	if (bT)
	//	{
	//		string str = "{ \"IPAddr\": \"172.16.13.134\",\"index\": 17,\"item\": \"lamp\",\"data\": {\"Value\": 255,\"WorkMode\": 0, \"ID\": 6},\"Port\": \"1204\"}";
	//		TestLampPoint(str);
	//	}
	//	else {
	//		string str = "{ \"IPAddr\": \"172.16.13.134\",\"index\": 17,\"item\": \"lamp\",\"data\": {\"Value\": 0,\"WorkMode\": 0, \"ID\": 6},\"Port\": \"1204\"}";
	//		TestLampPoint(str);
	//	}
	//	bT = !bT;
	//}
	return 0;
}

DWORD WINAPI  LP111(LPVOID l) {
	
	for (int i = 0; i < 20000; i++)
	{
		//Sleep(1000);
		printf("LP1 第 %d 条 数据", i);
		if (bT)
		{
			string str = "{ \"IPAddr\": \"172.16.13.134\",\"index\": 17,\"item\": \"lamp\",\"data\": {\"Value\": 255,\"WorkMode\": 0, \"ID\": 6},\"Port\": \"1204\"}";
			TestLampPoint(str);
		}
		else {
			string str = "{ \"IPAddr\": \"172.16.13.134\",\"index\": 17,\"item\": \"lamp\",\"data\": {\"Value\": 0,\"WorkMode\": 0, \"ID\": 6},\"Port\": \"1204\"}";
			TestLampPoint(str);
		}
		bT = !bT;
	}
	return 0;
}
DWORD WINAPI LP222(LPVOID l) {
	//bool bT = true;
	for (int i = 0; i < 20000; i++)
	{
		printf("LP2 第 %d 条 数据", i);
		if (bT)
		{
			string str = "{ \"IPAddr\": \"172.16.14.130\",\"index\": 17,\"item\": \"lamp\",\"data\": {\"Value\": 255,\"WorkMode\": 0, \"ID\": 663},\"Port\": \"1204\"}";
		//	TestLampPoint(str);
		}
		else {
			string str = "{ \"IPAddr\": \"172.16.13.134\",\"index\": 17,\"item\": \"lamp\",\"data\": {\"Value\": 0,\"WorkMode\": 0, \"ID\": 6},\"Port\": \"1204\"}";
			TestLampPoint(str);
		}
		bT = !bT;
	}
	return 0;
}
*/
LoggerId logid_fromfile;
LoggerId logid_dynamic;
int _tmainasdf(int argc, char* argv[])
{
	//增加 命令行参数
	cmdline::parser a;
//	a.add<string>("host", 'h', "host name", true, "");
//	a.add<string>("psw", 'w', "password", true, "");
//	a.add<int>("port", 'p', "port number", false, 80, cmdline::range(1, 65535));
//	a.add<string>("type", 't', "protocol type", false, "http", cmdline::oneof<string>("http", "https", "ssh", "ftp"));
//	a.add("gzip", '\0', "gzip when transfer");

	a.add<string>("name", 'n', "", false, "zc");
	a.add<string>("password", 'p', "", false, "zc");

	a.add<bool>("showui", 'u', "", false, false);
	a.add<bool>("showconsole", 'c', "", false, false);
	bool bShowConsole = false;
	bool bshowui = false;
	if (argc > 2)
	{
		a.parse_check(argc, argv);
		bShowConsole = a.get<bool>("showconsole");
		bshowui = a.get<bool>("showui");
	}

	//cout << a.get<string>("name") << ":"
	//	<< a.get<string>("password") << ""
	//	<< a.get<bool>("showui") 
	//	<< a.get<bool>("showconsole")
	//	<< endl;
//	if (a.exist("gzip")) cout << "gzip" << endl;



	if (bShowConsole)
	{
		::ShowWindow(::GetConsoleWindow(), SW_SHOW);
	}
	else{
		::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	}
	
	//设置线程
	g_threadsoap.SetPoolSize(THREAD_GSOAP_COUNT);// 开启1000个线程用来处理消息

	DWORD threadID;
	HANDLE hThread;
	hThread = CreateThread(NULL, 0, SoapRun2, NULL, 0, &threadID);
	//if (CLog4Manager::instance().open_log())
	//{
	//	LOG_INFO("BESService Starting...");
	//}
	ILog4zManager::getRef().config("config.cfg");
//	logid_dynamic = ILog4zManager::getRef().createLogger("dynamic");
//	logid_fromfile = ILog4zManager::getRef().findLogger("fromfile");
	//start log4z
	ILog4zManager::getRef().start();

	//configure the output behaviour

//	ILog4zManager::getRef().setLoggerDisplay(LOG4Z_MAIN_LOGGER_ID, true);
//	ILog4zManager::getRef().setLoggerDisplay(logid_dynamic, true);
//	ILog4zManager::getRef().setLoggerLevel(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_DEBUG);
//	ILog4zManager::getRef().setLoggerLevel(logid_dynamic, LOG_LEVEL_DEBUG);
//	ILog4zManager::getRef().setLoggerMonthdir(logid_dynamic, true);
//	ILog4zManager::getRef().setLoggerFileLine(LOG4Z_MAIN_LOGGER_ID, false);
//	ILog4zManager::getRef().setLoggerName(LOG4Z_MAIN_LOGGER_ID, "main");
//	ILog4zManager::getRef().setLoggerPath(LOG4Z_MAIN_LOGGER_ID, "./");
//	ILog4zManager::getRef().setLoggerLimitsize(logid_dynamic, 1);
//	ILog4zManager::getRef().setLoggerOutFile(LOG4Z_MAIN_LOGGER_ID, true);


//	ILog4zManager::getRef().setLoggerLevel(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_TRACE);
	//LOGT("set LOG_LEVEL_TRACE ----------------------------");
//	ILog4zManager::getRef().setLoggerLevel(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_DEBUG);
	//LOGT("set LOG_LEVEL_TRACE ============================");
	LOGI("BESService Starting...");

	//HANDLE hThread3;
	//DWORD threadID3;
	//hThread3 = CreateThread(NULL, 0, TTT, NULL, 0, &threadID3);
	//CloseHandle(hThread3);
#pragma region UI  

	if (bshowui)
	{
		// 界面测试
		DWORD threadID2;
		HANDLE hThread2;
		hThread2 = CreateThread(NULL, 0, UiRun, NULL, 0, &threadID2);
		WaitForSingleObject(hThread2, INFINITE); // 等待，直到线程被激发
		CloseHandle(hThread2);
	}

#pragma endregion
	//getchar();
//	TTT();
//	TTT2(NULL);
//	TTT2(NULL);

	if (!bshowui)
	{
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}
	
	return 0;
}

int main(int argc, char* argv[]) {
	//g_webmsg.Start();
	_tmainasdf(argc, argv);
}

