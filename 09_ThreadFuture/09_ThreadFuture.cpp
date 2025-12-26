#include "09_ThreadFuture.h"
#include <iostream>
#include <thread>
#include <future>
using namespace std;

/**
* std::future 和 std::promise 是 C++11 提供的用于线程间异步通信和结果传递的机制
* 它解决的问题是：
	1.异步任务如何返回结果
	2.主线程如何安全等待或查询执行状态
*/
int DoTask(int taskId){
	int restult;
	for (size_t i = 0; i < 10000; i++)
	{
		cout << "taskID:" << taskId << "DoTask...id:" << i << endl;
		restult = i;
	}

	return restult;
}


int main()
{
	std::future<int> fut = std::async(std::launch::async,DoTask,0);
	return 0;
}