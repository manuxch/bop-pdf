#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads) : stop(false), activeTasks(0) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queueMutex);
                    this->condition.wait(lock, [this] {
                        return this->stop.load(std::memory_order_relaxed) || !this->tasks.empty();
                    });
                    if (this->stop.load(std::memory_order_relaxed) && this->tasks.empty())
                        return;
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                // Ejecutar la tarea (esta aumentará/disminuirá activeTasks)
                task();
            }
        });
    }
}

void ThreadPool::wait() {
    std::unique_lock<std::mutex> lock(queueMutex);
    finished.wait(lock, [this] {
        return tasks.empty() && (activeTasks.load(std::memory_order_relaxed) == 0);
    });
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop.store(true, std::memory_order_relaxed);
    }
    condition.notify_all();
    for (std::thread &worker : workers) {
        if (worker.joinable())
            worker.join();
    }
}
