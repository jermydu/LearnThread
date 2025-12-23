#include "05_ThreadMutex.h"
#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

//mutex 互斥锁

//注意：常见错误
//不得向锁所在的作用域之外传递指针和引用，指向受保护的共享数据，无论是通过函数返回值将它们保存到对外可见的内存，还是将它们作为参数传递给使用者提供的函数。

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

//////////////////////////////////////////////////////////////////////////////////////
//lock 阻塞获取锁 获取后需要手动解说
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

//////////////////////////////////////////////////////////////////////////////////////
//try_lock 非阻塞获取锁  获取后需要手动解锁
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

//////////////////////////////////////////////////////////////////////////////////////
//自动加锁自动解锁
void DoSomething3()
{
	for (int i = 0; i < 100; i++)
	{
		std::lock_guard<mutex> lockGurad(g_mutex);		
		g_index++;
		std::cout << "g_index:" << g_index << std::endl;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//std::lock() 可以同时锁住多个互斥量 解决死锁问题
//一个真实的多个互斥量锁住多个共享数据的场景（银行账户转账引发的死锁问题）
class Account
{
public:
	int m_balace = 100000;	
	mutex mtx;
};

//错误示例 当多个线程调用这个函数时 from 和 to 可能会以不同的顺序加锁 从而引发死锁
void DoSomething4(Account& from,Account& to,int balance)
{
	std::lock_guard<mutex> lock1(from.mtx);
	std::lock_guard<mutex> lock2(to.mtx);

	from.m_balace -= balance;
	to.m_balace += balance;
}

//使用std::lock() 同时锁住两个互斥量 要不全锁住 要不都不锁住 解决死锁问题
void DoSomething5(Account& from, Account& to, int balance)
{
	std::lock(from.mtx, to.mtx);

	//std::adopt_lock  标记锁已经被锁住 不要需要再次加锁
	std::lock_guard<mutex> lock1(from.mtx,std::adopt_lock);			//搭配lock_guard 自动释放锁
	std::lock_guard<mutex> lock2(to.mtx,std::adopt_lock);

	from.m_balace -= balance;
	to.m_balace += balance;
}

//延迟加锁
void DoSomething6(Account& from, Account& to, int balance)
{
	//std::defer_lock 标记锁没有被锁住 需要手动加锁
	std::unique_lock<mutex> lock1(from.mtx, std::defer_lock);		
	std::unique_lock<mutex> lock2(to.mtx, std::defer_lock);
	//一些不需要加锁的操作

	std::lock(from.mtx, to.mtx);
	from.m_balace -= balance;
	to.m_balace += balance;
}

//尝试加锁
void DoSomething7(Account& from)
{
	//std::try_to_lock 尝试加锁 如果没有加锁成功需要手动加锁 不阻塞 必须检查 owns_lock()
	std::unique_lock<mutex> lock(from.mtx, std::try_to_lock);

	if (lock.owns_lock())
	{
		//操作共享数据
	}
	else
	{
		//没有获取到锁
		return;
	}
}

//C++17 提供了 std::scoped_lock 可以同时锁住多个互斥量 并且自动解锁 相当于 std::lock + 多个 lock_gurad 
void DoSomething8(Account& from, Account& to, int balance)
{
	std::scoped_lock<mutex, mutex> lock(from.mtx, to.mtx);

	from.m_balace -= balance;
	to.m_balace += balance;
}

int main()
{
	thread t1(DoSomething3); 
	thread t2(DoSomething3);
	t1.join();
	t2.join();

	Account from;
	Account to;
	thread t3(DoSomething5, std::ref(from),std::ref(to),100);
	t3.join();

	return 0;
}