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
std::condition_variable g_cvNotEmpty;		//条件变量 不为空
std::condition_variable g_cvNotFull;		//条件变量 不为满
std::queue<int> g_queue;
const size_t MAX_QUEUE_SIZE = 1000;			//队列最大值

void Producer()
{
	for (int i = 0; i < 100000000; i++)
	{
		{
			std::unique_lock<mutex> lock(g_mutex);

			//解决问题2 如果队列满了 则等待消费者消费
			g_cvNotFull.wait(lock, [] {
				return g_queue.size() < MAX_QUEUE_SIZE;
				});
			g_queue.push(i);
			
			lock.unlock();
			cout << "add value:" << i << endl;
		}
		g_cvNotEmpty.notify_one();
	}
}

void Consumer()
{
	while (true)
	{
		std::unique_lock<mutex> lock(g_mutex);
		//1.如果wait第二参数返回true，则继续执行后续代码；如果返回false，则解锁互斥量并阻塞到这里继续等待
		//2.当wait被唤醒后，会重新获取互斥量锁，然后检查第二个参数条件是否满足，满足则继续执行后续代码，否则继续等待（并解锁互斥量）
		g_cvNotEmpty.wait(lock, [] {
			return !g_queue.empty();
			});
		int value = g_queue.front();
		g_queue.pop();
		lock.unlock();

		g_cvNotFull.notify_one();
		//其他处理 不用加锁
		cout << "Consumer one value:" << value << endl;
	}
}

//上诉示例代码存在的问题
//1.notiy_one是用来把wat代码行唤醒,如果当前执行的流程没有停留在wai代码行,那么notify_one的执行就等于啥也没做(没有任何效果) 正常情况下不会有问题
//2.生产者和消费者存在竞争关系，如果生产者执行多消费者执行少 队列中数据会越来越多
//3.Consumer 永远不会退出

int main()
{
	thread t1(Consumer);
	thread t2(Producer);

	t1.join();
	t2.join();

	return 0;
}