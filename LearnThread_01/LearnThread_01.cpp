#include "LearnThread_01.h"
#include <iostream>
#include <thread>

using namespace std;

//创建线程的方式

//传递参数 创建线程
//join() 1、等待线程完成，2、还清理了线程相关的存储部分，
//只能对一个线程使用一次join(); 一旦已经使用过join() std::thread 对象就不能再次加入了，当对其使用joinable()时，将返回否 false
//detach 分离线程

void DoSomething(int id,int iCount)
{
	for (int i = 0; i < iCount; ++i)
	{
		std::cout << "id:" << id << "  value:" << i << std::endl;
	}
}

class DoTask 
{
public:
	DoTask(int id, int iCount) :m_id(id), m_iCount(iCount)
	{

	}

	//可调用对象
	void operator()() const
	{
		for (int i = 0; i < m_iCount; ++i)
		{
			std::cout << "this:" << this << "  " << "DoTask id:" << m_id << "   " << i << std::endl;
		}
	}

	void DoTaskFunc()
	{
		std::cout << "DoTaskFunc" << std::endl;
	}

private:
	int m_id;
	int m_iCount;
};

int main()
{
	//1.通过函数指针创建线程
	thread t1(DoSomething,100,200);     
	t1.join();

	//2.通过可调用对象创建线程
	thread t2(DoTask(200,10));
	t2.join();

	DoTask doTask(5, 10);
	std::cout << "doTask this:" << &doTask << std::endl;
	thread t3(doTask);   //默认拷贝传递
	t3.join();

	thread t4(std::ref(doTask)); //按引用传递
	t4.join();

	//3.通过lambda表达式创建线程
	int id = 10, iCount = 10;
	thread t5([id,iCount] {
		for (int i = 0; i < iCount; i++)
		{
			std::cout << "id:" << id << "  " << "value:" << i << std::endl;
		}
	});
	t5.join();

	//4.通过类成员函数创建线程
	thread t6(&DoTask::DoTaskFunc, &doTask);
	t6.join();
	return 0;
}