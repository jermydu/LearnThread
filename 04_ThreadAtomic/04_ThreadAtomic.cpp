#include "04_ThreadAtomic.h"
#include <iostream>
#include <thread>
#include <atomic>

using namespace std;

//原子操作
atomic<int> index{ 0 };

void DoSomething()
{
	for (int i = 0; i < 100; i++)
	{
		index++;
		this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	std::cout << "index:" << index << std::endl;
}


//常用场景 计数类
class Counter
{
	Counter(int i) :m_iCount(i)
	{

	}
	void increment()
	{
		m_iCount.fetch_add(1);
	}
	int get() const
	{
		return m_iCount.load();
	}
private:
	atomic<int> m_iCount;
};

int main()
{
	std::cout << index.is_lock_free() << std::endl;   //is_lock_free  查询是否支持无锁操作
	thread t1(DoSomething);     
	thread t2(DoSomething);

	t1.join();
	t2.join();

	return 0;
}