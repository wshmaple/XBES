#pragma once


#include "DdataManager.h"
#include "UIManager.h"

#include "CCrossDataManager.h"
#include "CLampDataManager.h"

#include "CLampType1.h"

// 工厂模式基类
class CBaseFactory
{
public:
	CBaseFactory();
	~CBaseFactory();
	// web json 进这个 函数 处理一圈 出来 就是我们想要的 json 数据
	// 
	string Run(string strMsg);

	// 伪代码
	/*
	long long jiechen(int n)
	{
		long long ret = 1;
		for (int i = 1; i <= n; i++)
		{
			ret += i;
		//	std::cout << n << "->:" << ret << "*" << i <<"\n"<< endl;

		}
	//	chrono::milliseconds sleepDuration(1000);
	//	this_thread::sleep_for(sleepDuration);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "worker thread ID:" << std::this_thread::get_id() << std::endl;
		return ret;
	}

	auto tsPool(ThreadPool &pool, int nData) {
	//	std::cout <<"线程 ID :"<< this_thread::get_id()<<" nData="<< nData << endl;

		return pool.enqueue([](int answer) { return jiechen(answer); }, nData);
	}
	void testPool(){
		ThreadPool pool(4);

		// enqueue and store future
		auto result = pool.enqueue([](int answer) { return jiechen(answer); }, 42);


		// get result from future, print 42
		std::cout << result.get() << std::endl;

		vector<int> data;
		for (int i = 3;i<100;i++)
		{
			data.push_back(i);
		}
		vector<future<long long>> fus;
		for (auto i : data)
		{
			//packaged_task<long long() > pt(bind(jiechen, i));
			//fus.push_back(std::move(pt.get_future()));
			//std::thread(std::move(pt)).detach();
			fus.push_back(tsPool(pool, i));
		}

		cout << "打印结果！\n" << endl;
		int kk = 0;
		for (auto& i : fus)
		{

			i.wait();
			kk++;
			cout << "[" << kk << "]结果:" << i.get() << endl;

		}

	}
	*/

	// 工厂模式中 组装消息 分类一下 
	// 1.能耗
	// 2.Cross
	// 3.灯光系统

	// 4.功能处理
	// 5.统计功能

	// 1.网络系统
	// 2.UI 绘制系统

	// 3.UI 绘制 数据 存储系统

	// 1.交互

	// CMsgLog 保存 要绘制的信息
	// CMsgLog 记录TCP 等 业务逻辑消息


	UIManager m_ui;	// 界面显示 类 UI 系统 
	CDdataManager m_dbManager; // 能耗 系统
	CCrossDataManager m_crossDataManager;// Cross 楼宇控制
	CLampDataManager g_lampDataManager; // 灯光控制系统
	
	//************************************
	// Method:    DoParseWebJson 解析网络消息 网络消息入口函数
	// FullName:  CBaseFactory::DoParseWebJson
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: std::string ConfigInfo
	// Parameter: std::string & result
	//************************************
	void DoParseWebJson(std::string ConfigInfo, std::string& result);

	void DoParseWebJsonNengHao(Json::Value root, std::string& result, CDdataManager*p);//能耗
	void DoParseWebJsonCross(Json::Value root, std::string& result, CCrossDataManager*p);// 楼宇
	void DoParseWebJsonLamp(Json::Value root, std::string& result, CLampDataManager*p);// 灯光

	// 根据 IP 分配不同的线程进行 处理
	// 

	map<string, CDdataManager*> m_mapNengHao;// 能耗 某个IP 地址来处理
	map<string, CCrossDataManager*> m_mapCross;// 楼宇 某个IP 地址线程来处理
	map<string, CLampDataManager*> m_mapLamp;// 灯光 某个地址线程来处理
	map<string, CLampType1*> m_mapLamp1;// 灯光 某个地址线程来处理

	// 获取一个类型的操作吧
	void DoThreadParse(std::string ConfigInfo,std::string &result);

	// 这个 处理线程 多开几个 
	ThreadPool m_threadpoolCross;// 耗时处理用的线程

	ThreadPool m_threadpoolLamp;// 耗时处理用的线程
	ThreadPool m_threadpoolNengHao;// 耗时处理用的线程

	map<string, map<string, CTCP>> m_maptcp;
};

