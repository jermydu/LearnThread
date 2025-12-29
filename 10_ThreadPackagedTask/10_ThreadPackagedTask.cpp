#include "10_ThreadPackagedTask.h"
#include <iostream>
#include <thread>
#include <future>
#include <queue>
#include <vector>
#include <functional>
#include <chrono>
#include <mutex>
#include <condition_variable>
using namespace std;

/**
* std::packaged_task 将一个 可调用对象（函数 / lambda / functor）绑定到一个 std::future 
	允许自行决定何时、在哪个线程执行
* std::promise 是“结果的生产者”
它用于在一个线程中 手动设置结果或异常，并在另一个线程中通过 std::future 获取
*   cout << "\n对比总结:" << endl;
    cout << "1. packaged_task 更简单，直接包装函数" << endl;
    cout << "2. promise 更灵活，可以在不同地方设置值" << endl;
    cout << "3. promise 可以设置异常，也可以多次尝试设置值" << endl;
    cout << "4. packaged_task 只能执行一次，promise 可以更精细控制" << endl;
*/

// 全局任务队列和同步工具
std::mutex g_mutex;
std::condition_variable g_cv;
std::queue<std::packaged_task<void()>> g_taskQueue;
bool g_stop = false;

// 任务执行器线程
void DoTask() {
    cout << "Task started, thread id: " << this_thread::get_id() << endl;

    while (true) {
        std::packaged_task<void()> task;

        {
            std::unique_lock<std::mutex> lock(g_mutex);
            // 等待任务或停止信号
            g_cv.wait(lock, []() {
                return !g_taskQueue.empty() || g_stop;
                });

            // 如果停止且队列为空，则退出
            if (g_stop && g_taskQueue.empty()) {
                break;
            }

            // 取出任务
            if (!g_taskQueue.empty()) {
                task = std::move(g_taskQueue.front());
                g_taskQueue.pop();
            }
        }

        // 执行任务
        if (task.valid()) {
            cout << "Executing task in thread: " << this_thread::get_id() << endl;
            task();  // 执行任务
        }
    }

    cout << "Task stopped." << endl;
}

// 具体任务函数示例
int CalculateSum(int a, int b) {
    cout << "Calculating sum of " << a << " + " << b << "..." << endl;
    this_thread::sleep_for(chrono::milliseconds(200));
    return a + b;
}

int CalculateProduct(int a, int b) {
    cout << "Calculating product of " << a << " * " << b << "..." << endl;
    this_thread::sleep_for(chrono::milliseconds(300));
    return a * b;
}

int ComplexCalculation(int x) {
    cout << "Running complex calculation with " << x << "..." << endl;
    this_thread::sleep_for(chrono::milliseconds(500));
    return x * x + 2 * x + 1;
}

// 提交任务到队列
template<typename Func, typename... Args>
auto PostTask(Func&& func, Args&&... args) -> std::future<decltype(func(args...))> {
    // 获取返回类型
    using ReturnType = decltype(func(args...));

    // 创建 packaged_task，绑定函数和参数
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
    );

    // 获取 future
    std::future<ReturnType> result = task->get_future();

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        // 将任务包装成 void() 类型放入队列
        g_taskQueue.emplace([task]() {
            (*task)();
            });
    }

    // 通知任务执行器
    g_cv.notify_one();

    return result;
}

// 测试1: 基本用法示例
void TestBasicUsage() {
    cout << "\n=== Test 1: Basic Usage ===" << endl;

    // 创建 packaged_task
    std::packaged_task<int(int, int)> task(CalculateSum);

    // 获取 future
    std::future<int> result = task.get_future();

    // 在单独线程中执行任务
    thread worker(std::move(task), 10, 20);

    // 主线程可以继续工作
    cout << "Main thread doing other work..." << endl;
    this_thread::sleep_for(chrono::milliseconds(100));

    // 获取结果（会阻塞直到任务完成）
    cout << "Sum result: " << result.get() << endl;

    worker.join();
}

//Promise 与 Package_task 对比
void TestComparisonWithPackagedTask() {
    cout << "\nPromise and Packaged_task compare" << endl;

    cout << "\n1: std::packaged_task" << endl;
    {
        auto task_func = [](int a, int b) -> int {
            cout << "  packaged_task: Calculating..." << endl;
            this_thread::sleep_for(chrono::milliseconds(200));
            return a + b;
            };

        std::packaged_task<int(int, int)> task(task_func);
        std::future<int> future = task.get_future();

        thread t(std::move(task), 10, 20);

        int result = future.get();
        cout << "  packaged_task Result: " << result << endl;

        t.join();
    }

    cout << "\n2: std::promise" << endl;
    {
        std::promise<int> promise;
        std::future<int> future = promise.get_future();

        thread t([&promise]() {
            cout << "  promise: Calculating..." << endl;
            this_thread::sleep_for(chrono::milliseconds(200));
            promise.set_value(10 + 20);
            });

        int result = future.get();
        cout << "  promise Result: " << result << endl;

        t.join();
    }
}

// 测试2: 使用 lambda 表达式
void TestLambda() {
    cout << "\n=== Test 2: Lambda Expression ===" << endl;

    // 使用 lambda 创建 packaged_task
    std::packaged_task<double()> task([]() -> double {
        cout << "Lambda task executing..." << endl;
        this_thread::sleep_for(chrono::milliseconds(200));
        return 3.14159;
        });

    std::future<double> result = task.get_future();

    // 直接在当前线程执行
    task();

    cout << "Pi value: " << result.get() << endl;
}

// 测试3: 异常处理
void TestExceptionHandling() {
    cout << "\n=== Test 3: Exception Handling ===" << endl;

    std::packaged_task<int(int)> task([](int x) -> int {
        if (x < 0) {
            throw std::runtime_error("Negative value not allowed!");
        }
        return x * 2;
        });

    std::future<int> result = task.get_future();

    thread worker(std::move(task), -5);

    try {
        int value = result.get();
        cout << "Result: " << value << endl;
    }
    catch (const std::exception& e) {
        cout << "Caught exception: " << e.what() << endl;
    }

    worker.join();
}

// 测试4: 任务队列系统
void TestTaskQueue() {
    cout << "\n=== Test 4: Task Queue System ===" << endl;

    // 启动任务执行器线程
    thread executor(DoTask);

    // 提交多个任务
    vector<std::future<int>> futures;

    futures.push_back(PostTask(CalculateSum, 100, 200));
    futures.push_back(PostTask(CalculateProduct, 5, 7));
    futures.push_back(PostTask(ComplexCalculation, 8));
    futures.push_back(PostTask([]() -> int {
        cout << "Inline lambda task..." << endl;
        this_thread::sleep_for(chrono::milliseconds(150));
        return 42;
        }));

    // 再提交几个延迟任务
    this_thread::sleep_for(chrono::milliseconds(500));
    futures.push_back(PostTask(CalculateSum, 30, 40));
    futures.push_back(PostTask(CalculateProduct, 9, 9));

    // 获取所有结果
    for (size_t i = 0; i < futures.size(); ++i) {
        try {
            int result = futures[i].get();
            cout << "Task " << i + 1 << " result: " << result << endl;
        }
        catch (const std::exception& e) {
            cout << "Task " << i + 1 << " failed: " << e.what() << endl;
        }
    }

    // 停止任务执行器
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_stop = true;
    }
    g_cv.notify_all();

    executor.join();
}

// 测试5: 批量任务处理
void TestBatchProcessing() {
    cout << "\n=== Test 5: Batch Processing ===" << endl;

    // 创建多个 packaged_task
    vector<std::packaged_task<int(int)>> tasks;
    vector<std::future<int>> futures;

    for (int i = 0; i < 5; ++i) {
        // 每个任务乘以不同的系数
        tasks.emplace_back([i](int x) -> int {
            cout << "Task " << i << " processing value " << x << endl;
            this_thread::sleep_for(chrono::milliseconds(100));
            return x * (i + 1);
            });

        // 获取 future
        futures.push_back(tasks.back().get_future());
    }

    // 在多个线程中并行执行
    vector<thread> workers;
    for (int i = 0; i < 5; ++i) {
        workers.emplace_back(std::move(tasks[i]), 10);
    }

    // 收集结果
    int total = 0;
    for (int i = 0; i < 5; ++i) {
        total += futures[i].get();
    }

    for (auto& w : workers) {
        w.join();
    }

    cout << "Total result from all tasks: " << total << endl;
}

int main() {
    cout << "Testing std::packaged_task examples..." << endl;

    // 运行各个测试
    TestBasicUsage();
    TestComparisonWithPackagedTask();
    TestLambda();
    TestExceptionHandling();

    // 重置停止标志
    g_stop = false;
    while (!g_taskQueue.empty()) {
        g_taskQueue.pop();
    }

    TestTaskQueue();
    TestBatchProcessing();

    cout << "\nAll tests completed!" << endl;

    return 0;
}
