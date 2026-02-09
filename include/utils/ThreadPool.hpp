#pragma once 



#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <functional>


class ThreadPool {
    private:
        std::vector<std::thread> workers;
        std::mutex mx;
        std::condition_variable cv;
        std::atomic<bool> stop;
        std::queue<std::function<void()>> tasks;

    public:
        ThreadPool(size_t);
        void submit(std::function<void()>);
        ~ThreadPool();    
};