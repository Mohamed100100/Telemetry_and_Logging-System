


#include "utils/ThreadPool.hpp"


ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this] {
            std::unique_lock<std::mutex> lock(this->mx,std::defer_lock);
            std::function<void()> task;
            while (true) {

                lock.lock();
                // block until: task pushed into the queue OR stopping
                this->cv.wait(lock, [this] { 
                    return this->stop.load() || !this->tasks.empty(); 
                });
                if (this->stop.load() && this->tasks.empty()){return;}

                task = std::move(this->tasks.front());
                this->tasks.pop();

                lock.unlock();
                task();
            }
        });
    }
}

void ThreadPool::submit(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(mx);
        // push task into the queue
        tasks.emplace(std::move(task));
    }
    cv.notify_one();
}

ThreadPool::~ThreadPool() {
    stop = true;
    cv.notify_all();
    for (std::thread &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

