#pragma once


#include "DdataManager.h"
#include "UIManager.h"

#include "CCrossDataManager.h"
#include "CLampDataManager.h"

#include "CLampType1.h"

// ����ģʽ����
class CBaseFactory
{
public:
	CBaseFactory();
	~CBaseFactory();
	// web json ����� ���� ����һȦ ���� ����������Ҫ�� json ����
	// 
	string Run(string strMsg);

	// α����
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
	//	std::cout <<"�߳� ID :"<< this_thread::get_id()<<" nData="<< nData << endl;

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

		cout << "��ӡ�����\n" << endl;
		int kk = 0;
		for (auto& i : fus)
		{

			i.wait();
			kk++;
			cout << "[" << kk << "]���:" << i.get() << endl;

		}

	}
	*/

	// ����ģʽ�� ��װ��Ϣ ����һ�� 
	// 1.�ܺ�
	// 2.Cross
	// 3.�ƹ�ϵͳ

	// 4.���ܴ���
	// 5.ͳ�ƹ���

	// 1.����ϵͳ
	// 2.UI ����ϵͳ

	// 3.UI ���� ���� �洢ϵͳ

	// 1.����

	// CMsgLog ���� Ҫ���Ƶ���Ϣ
	// CMsgLog ��¼TCP �� ҵ���߼���Ϣ


	UIManager m_ui;	// ������ʾ �� UI ϵͳ 
	CDdataManager m_dbManager; // �ܺ� ϵͳ
	CCrossDataManager m_crossDataManager;// Cross ¥�����
	CLampDataManager g_lampDataManager; // �ƹ����ϵͳ
	
	//************************************
	// Method:    DoParseWebJson ����������Ϣ ������Ϣ��ں���
	// FullName:  CBaseFactory::DoParseWebJson
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: std::string ConfigInfo
	// Parameter: std::string & result
	//************************************
	void DoParseWebJson(std::string ConfigInfo, std::string& result);

	void DoParseWebJsonNengHao(Json::Value root, std::string& result, CDdataManager*p);//�ܺ�
	void DoParseWebJsonCross(Json::Value root, std::string& result, CCrossDataManager*p);// ¥��
	void DoParseWebJsonLamp(Json::Value root, std::string& result, CLampDataManager*p);// �ƹ�

	// ���� IP ���䲻ͬ���߳̽��� ����
	// 

	map<string, CDdataManager*> m_mapNengHao;// �ܺ� ĳ��IP ��ַ������
	map<string, CCrossDataManager*> m_mapCross;// ¥�� ĳ��IP ��ַ�߳�������
	map<string, CLampDataManager*> m_mapLamp;// �ƹ� ĳ����ַ�߳�������
	map<string, CLampType1*> m_mapLamp1;// �ƹ� ĳ����ַ�߳�������

	// ��ȡһ�����͵Ĳ�����
	void DoThreadParse(std::string ConfigInfo,std::string &result);

	// ��� �����߳� �࿪���� 
	ThreadPool m_threadpoolCross;// ��ʱ�����õ��߳�

	ThreadPool m_threadpoolLamp;// ��ʱ�����õ��߳�
	ThreadPool m_threadpoolNengHao;// ��ʱ�����õ��߳�

	map<string, map<string, CTCP>> m_maptcp;
};

