#pragma once
// ������Ϣ ����

#include <mutex>
class CXWebMsgQueen
{
public:
	CXWebMsgQueen();
	~CXWebMsgQueen();
	struct MsgItem {
		int x;
		char buf[255];
		MsgItem() {}
		bool operator<(const MsgItem& a)const {
			return x < a.x;// �󶥶�
		}
	};

	priority_queue<MsgItem> m_queMsg;
	std::deque<int> m_data;
	std::mutex  m_mtx; // ������
	std::condition_variable m_cv; // ��������

	int m_nGen;
private:
	void ProductThread() {
		while (true) {
			std::unique_lock <std::mutex> lck(m_mtx);
			m_nGen = ++m_nGen % 1000;
//			printf("%d produce %d -size:%d\n", std::this_thread::get_id(), m_nGen , m_data.size());
			
			m_data.push_back(m_nGen);
			lck.unlock();
			m_cv.notify_all();

			/* �ȴ�1S */
			std::chrono::milliseconds dura(1000);
			std::this_thread::sleep_for(dura);
		}
	}
	void CustomThread() {
		while (true) {
			std::unique_lock <std::mutex> lck(m_mtx);
			while (m_data.empty()) {
				m_cv.wait(lck);
			}
			int nData = m_data.front();
			m_data.pop_front();
//			printf("%d consume %d -size:%d\n", std::this_thread::get_id(), nData, m_data.size());
			lck.unlock();

			/* �ȴ�2S */
			std::chrono::milliseconds dura(2000);
			std::this_thread::sleep_for(dura);
		}

	}
public:
	void Start() {
		std::vector<std::thread> threads;
		threads.clear();
		for (int i = 0; i < 5; i++) {/* �������߳� */
			threads.push_back(std::thread(&CXWebMsgQueen::ProductThread, this));
		}
		for (int i = 5; i < 15; i++) {/* �������߳� */
			threads.push_back(std::thread(&CXWebMsgQueen::CustomThread, this));
		}
		for (auto& t : threads) {/* �ȴ������̵߳��˳� */
			t.join();
		}
	}

	void test() {

		MsgItem msg;
		for (int i = 100; i > 1; i--)
		{
			sprintf_s(msg.buf, "name%d", i);
			msg.x = i;
			m_queMsg.push(msg);
		}
		int k = 0;
		while (!m_queMsg.empty())
		{
			printf_s("%d,%s\n", m_queMsg.top().x, m_queMsg.top().buf);

			m_queMsg.pop();
			k++;
			if (k == 50 || k == 60 || k == 70) {
				printf("���ӣ�%d\n", k);
				for (int i = 200; i > 190; i--)
				{
					sprintf_s(msg.buf, "name%d", i);
					msg.x = i;
					m_queMsg.push(msg);
				}

			}
		}
	}
};