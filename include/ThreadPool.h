#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <atomic>
#include <stdexcept>
#include <memory>

class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::condition_variable finished;

    std::atomic<bool> stop;
    std::atomic<int> activeTasks;

public:
    ThreadPool(size_t numThreads);

    template<class F>
    auto enqueue(F&& f) -> std::future<decltype(f())>;

    // Espera hasta que no haya tareas pendientes ni activas
    void wait();

    ~ThreadPool();
};

template<class F>
auto ThreadPool::enqueue(F&& f) -> std::future<decltype(f())> {
    using return_type = decltype(f());

    auto taskPtr = std::make_shared<std::packaged_task<return_type()>>(std::forward<F>(f));
    std::future<return_type> res = taskPtr->get_future();

    {
        std::unique_lock<std::mutex> lock(queueMutex);
        if (stop) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }

        tasks.emplace([taskPtr, this]() {
            activeTasks.fetch_add(1, std::memory_order_relaxed);
            try {
                (*taskPtr)();
            } catch (...) {
                activeTasks.fetch_sub(1, std::memory_order_relaxed);
                finished.notify_all();
                throw;
            }
            activeTasks.fetch_sub(1, std::memory_order_relaxed);
            finished.notify_all();
        });
    }

    condition.notify_one();
    return res;
}

#endif // THREADPOOL_H
