#include "LearnThread_05.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>

using namespace std;

//mutex 互斥锁

//注意：常见错误
//不得向锁所在的作用域之外传递指针和引用，指向受保护的共享数据，无论是通过函数返回值将它们保存到对外可见的内存，还是将它们作为参数传递给使用
//者提供的函数。

/**
* 非定时的互斥体类
* std::mutex std::recursive_mutex std::shared_mutex
* 上面三个类都支持的方法
* lock():尝试获取锁 并阻塞知道获得锁
* try_lock():尝试获取锁，如果当前锁被其他线程持有，则立即返回（不阻塞），如果成功获取锁 返回 true 否则返回 false
* unlock():释放锁
*/


int g_index{0};
mutex g_mutex;
void DoSomething()
{
	for (int i = 0; i < 100; i++)
	{
		g_mutex.lock();
		g_index++;
		g_mutex.unlock();
		std::cout << "g_index:" << g_index << std::endl;
	}
}

void DoSomething2()
{
	for (int i = 0; i < 100; i++)
	{
		bool bLock = g_mutex.try_lock();   //不阻塞
		if (bLock)
		{
			std::cout << "get lock operator shared data" << std::endl;
			g_index++;
			g_mutex.unlock();
		}
		else
		{
			std::cout << "...." << std::endl;
		}
		std::cout << "g_index:" << g_index << std::endl;
	}
}

void DoSomething3()
{
	for (int i = 0; i < 100; i++)
	{
		std::lock_guard<mutex> lockGurad(g_mutex);
		g_index++;
		std::cout << "g_index:" << g_index << std::endl;
	}
}

int main()
{
	thread t1(DoSomething3); 
	thread t2(DoSomething3);
	t1.join();
	t2.join();

	return 0;
}