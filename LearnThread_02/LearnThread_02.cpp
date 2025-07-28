#include "LearnThread_02.h"
#include <iostream>
#include <thread>

using namespace std;

//使用RAII等待线程结束

void DoSomething()
{
	std::cout << "DoSomething" << std::endl;
}

class DoTask 
{
public:
	//可调用对象
	void operator()() const
	{
		std::cout << "DoTask" << std::endl;
	}

};

class ThreadGuard
{
private:
	thread& _t;
public:
	ThreadGuard(thread& t) :_t(t)
	{
		if (_t.joinable())
		{
			_t.join();
		}
	}

	//禁止拷贝构造和拷贝赋值，防止多个 thread_guard 管理同一个线程，导致重复 join() 的风险。
	ThreadGuard(const ThreadGuard& tg) = delete;
	ThreadGuard& operator=(const ThreadGuard& tg) = delete;
};


//如果 DoSomething() 抛出异常，tg 仍然会被销毁，其析构函数会自动调用 t.join()，防止线程泄漏。
//如果没有 ThreadGuard，必须手动在 main 中写 t.join()，但如果中途抛出异常，线程可能永远不会被 join()，导致程序崩溃或未定义行为。
int main()
{
	thread t1{ DoTask()};

	DoSomething();

	ThreadGuard tg(t1);

	DoSomething();
	return 0;
}