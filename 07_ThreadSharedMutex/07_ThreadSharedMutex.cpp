#include "07_ThreadSharedMutex.h"
#include <iostream>
#include <thread>
#include <shared_mutex>
using namespace std;

/**
* std::shared_mutex 是 C++17 引入的读写锁（shared / exclusive lock），用于解决“多读少写”场景下互斥量并发性能不足的问题。
* 比如一个配置文件缓存使用场景 读操作远多于写操作
* 共享锁（读锁）：允许多个线程同时持有
* 独占锁（写锁）：同一时刻只能有一个线程持有
*/

class FileCache
{
public:
	//多个线程可以同时读取文件数据 但是当有线程写数据时 其他线程不能读也不能写
	void Reader()
	{
		while (true)
		{
			{
				//模拟频繁读取文件
				std::shared_lock<std::shared_mutex> readLock(_sharedMutex);  //获取共享锁（读锁）	
				//读取文件操作
				cout << "Reader..." << "_fileData:" << _fileData << endl;
			}
			this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	//同一时间只有一个线程可以写数据 其他线程不能写也不能读
	void Writer()
	{
		int index = 0;
		while (true)
		{
			{
				//模拟写文件 写操作没有那么频繁
				std::unique_lock<std::shared_mutex> writerLock(_sharedMutex); //获取排斥锁（写锁）
				_fileData = index;
				cout << "Writer..." << "_fileData:" << _fileData << endl;
				index++;
			}
			this_thread::sleep_for(std::chrono::seconds(10));
		}
	}

private:
	int _fileData;
	mutable std::shared_mutex _sharedMutex;  //声明为 mutable，是为了允许在 const 成员函数中加锁
};


int main()
{
	FileCache fileCache;
	thread t1(&FileCache::Reader,&fileCache);
	thread t2(&FileCache::Writer,&fileCache);
	
	t1.join();
	t2.join();
	return 0;
}