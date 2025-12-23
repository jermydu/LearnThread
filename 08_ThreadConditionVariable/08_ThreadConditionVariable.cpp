#include "08_ThreadConditionVariable.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
using namespace std;

/**
* std::condition_variable 是 C++11 提供的线程同步原语，用于线程之间的等待-通知机制。它解决的问题不是“互斥”，而是“在满足某个条件之前高效等待”。
* 典型场景：
		1.生产者 / 消费者
		2.任务队列
		3.线程池
		4.等待某个状态变为 true
*/

std::mutex g_mutex;
std::condition_variable g_cv;
std::queue<int> g_queue;

void Producer()
{
	for (int i = 0; i < 100000000; i++)
	{
		{
			std::lock_guard<mutex> lock(g_mutex);
			g_queue.push(i);
		}
		cout << "add value:" << i << endl;
		g_cv.notify_one();
	}
}

void Consumer()
{
	while (true)
	{
		std::unique_lock<mutex> lock(g_mutex);
		g_cv.wait(lock, [] {
			return !g_queue.empty();
			});
		int value = g_queue.front();
		g_queue.pop();
		lock.unlock();

		//其他处理 不用加锁
		cout << "Consumer one value:" << value << endl;
	}
}

int main()
{
	thread t1(Consumer);
	thread t2(Producer);

	t1.join();
	t2.join();

	return 0;
}