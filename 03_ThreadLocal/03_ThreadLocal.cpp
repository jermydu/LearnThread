#include "03_ThreadLocal.h"
#include <iostream>
#include <thread>

using namespace std;

//线程本地存储

int i = 0;			//所有线程共享这个一个变量
thread_local int k = 0;   //标记这个变量为线程本地数据，每个线程都有这个变量的独立副本

void DoSomething()
{
	thread_local int m = 100;  //在函数内部这个变量的行为和声明为静态变量一样，只不过每个线程有独立的副本，不论这个函数在线程中调用几次这个变量也只初始化一次
	std::cout << "i:" << i << "  k:" << k << std::endl;
	++i;
	++k;

	std::cout << "m:" << m << "  &m:" << &m << std::endl;
	++m;
}

int main()
{
	thread t1(DoSomething);     
	t1.join();

	thread t2(DoSomething);
	t2.join();
	return 0;
}