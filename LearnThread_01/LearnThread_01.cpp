#include "LearnThread_01.h"
#include <iostream>
#include <thread>

using namespace std;

//传递参数 创建线程
//join() 1、等待线程完成，2、还清理了线程相关的存储部分，
//只能对一个线程使用一次join(); 一旦已经使用过join() std::thread 对象就不能再次加入了，当对其使用joinable()时，将返回否 false
//detach 分离线程

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

int main()
{
	thread t1(DoSomething);     //函数指针构造线程
	t1.join();

	//thread t2(DoTask()); //错误  编译器会将其解析为函数声明，而不是类型对象的定义
	DoTask task; //创建一个DoTask对象
	thread t2(task); //使用对象创建线程
	t2.join();
	
	thread t3((DoTask()));	//多组括号 使用临时对象创建线程
	t3.join();

	thread t4{ DoTask() }; //使用花括号创建线程 新的统一的初始化语法
	t4.join();

	thread t5([]{
		DoSomething();
		}); //使用lambda表达式创建线程
	t5.join();

	return 0;
}