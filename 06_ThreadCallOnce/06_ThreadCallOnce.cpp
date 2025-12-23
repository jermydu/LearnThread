#include "06_ThreadCallOnce.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <memory>
using namespace std;

/**
* std::call_once 是 C++ 标准库中用于线程安全的一次性初始化机制，目的是保证某个初始化动作在多线程环境下只执行一次，且由标准库负责同步与可见性。是实现懒初始化和单例的首选工具
* 传入的函数 最多执行一次
* 其他线程在初始化完成前会被阻塞
* 初始化完成后，对所有线程内存可见
*/

std::once_flag g_initFlag;


//1.std::once_flag 必须是同一个实例
//2.InitFunction() 可能被多个线程同时尝试调用，但只会执行一次
//3.其他线程会在 InitFunction() 完成后继续执行
//4.参数在真正执行的那一次中被使用,其余线程传入的参数会被忽略
void InitFunction(int i,string str)
{
	cout << "call InitFunction..." << "i:" << i << "  str:" << str << endl;
	//执行初始化操作
}

void Func()
{
	std::call_once(g_initFlag,InitFunction,100,"hello");
	cout << "call Func" << endl;
}

//典型使用场景  单例懒加载
class Singleton
{
public:
	static Singleton& GetInstance()
	{
		std::call_once(_initFlag, [] {
			_instance.reset(new Singleton);
			});
		return *_instance;
	}
	void DoSomeing()
	{
		cout << "Singleton DoSomeing" << endl;
	}
private:
	Singleton() = default;
	Singleton(const Singleton& singleton) = delete;
	Singleton& operator=(const Singleton& singletion) = delete;

	static std::once_flag _initFlag;
	static std::unique_ptr<Singleton> _instance;
};

//静态成员定义
std::once_flag Singleton::_initFlag;
std::unique_ptr<Singleton> Singleton::_instance;

int main()
{
	thread t1(Func);
	thread t2(Func);

	t1.join();
	t2.join();

	Singleton& s = Singleton::GetInstance();
	s.DoSomeing();
	return 0;
}