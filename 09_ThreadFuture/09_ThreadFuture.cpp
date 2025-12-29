#include "09_ThreadFuture.h"
#include <iostream>
#include <thread>
#include <future>
using namespace std;

/**
* std::future 是 C++11 提供的用于线程间异步通信和结果传递的机制
* 它解决的问题是：
	1.异步任务如何返回结果
	2.主线程如何安全等待或查询执行状态
*/
int DoTask(int taskId) {
	int restult;
	for (size_t i = 0; i < 10000; i++)
	{
		cout << "taskID:" << taskId << "DoTask...id:" << i << endl;
		restult = i;
	}

	return restult;
}


int main() {
	//std::launch::async 在新线程中异步执行任务
	//std::launch::deferred 任务延迟执行，直到调用 get() 或 wait() 时才执行（和调用者在同一个线程中执行）
	std::future<int> fut1 = std::async(std::launch::async, DoTask, 1);
	std::future<int> fut2 = std::async(std::launch::async, DoTask, 2);

	//其他操作

	//get() 阻塞获取结果 返回执行结果 只能调用一次
	cout << "result:" << fut1.get() << endl;
	//wait() 阻塞直到 DoTask 完成为止 不返回结果  可以调用多次
	fut2.wait();
	cout << "result:" << fut2.get() << endl;

	std::future<int> fut3 = std::async(std::launch::async, DoTask, 3);
	//wait_for()：等待指定时间段 wait_until()：等待直到指定时间点
	std::future_status status = fut3.wait_for(std::chrono::milliseconds(1000));
	//std::future_status status = fut3.wait_until(std::chrono::steady_clock::now() + std::chrono::seconds(10));
	if (status == std::future_status::ready) {
		//任务已完成  可以立即调用 get()，不会阻塞
		cout << "done..." << "result:" << fut3.get() << endl;
	}
	else if (status == std::future_status::deferred) {
		//任务被延迟执行 尚未在任何线程中开始运行 使用 std::launch::deferred 时会触发这种情况
		cout << "deferred..." << endl;
	}
	else if (status == std::future_status::timeout) {
		//超时 在指定时间内任务尚未完成 future 仍处于等待状态
		cout << "timeout..." << endl;
	}
	return 0;
}