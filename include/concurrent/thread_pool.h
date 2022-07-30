#ifndef CONCURRENTHUFFMAN_THREAD_POOL_H
#define CONCURRENTHUFFMAN_THREAD_POOL_H
#include <atomic>
#include <thread>
#include <cassert>
#include <future>
#include "queue.h"
#include "thread_joiner.h"
#include "task.h"

namespace Concurrent {
class ThreadPool
{
public:
    explicit ThreadPool(uint32_t num_threads_ = std::thread::hardware_concurrency())
        : num_threads(num_threads_)
        , running(true)
        , thread_joiner(threads)
    {
        assert(num_threads >= 1 && "Thread pool requires at least 1 thread!");
        threads.reserve(num_threads);
        try
        {
            for (uint32_t i = 0; i < num_threads; ++i)
                threads.emplace_back(&ThreadPool::workerThread, this);
        }
        catch (...)
        {
            running = false;
            throw;
        }
    }

    template<typename Function>
    std::future<typename std::result_of<Function()>::type> submitTask(Function f)
    {
        using result_type = typename std::result_of<Function()>::type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> result(task.get_future());
        task_queue.push(std::move(task));
        return result;
    }

    uint8_t numberOfWorkers() const
    {
        return num_threads;
    }

    ~ThreadPool()
    {
        running = false;
    }

private:
    uint32_t num_threads;
    std::atomic_bool running;
    Queue<Task> task_queue;
    std::vector<std::thread> threads;
    ThreadJoiner thread_joiner;

    void workerThread()
    {
        while (running)
        {
            Task task;
            if (task_queue.tryPop(task))
                task();
            else
                std::this_thread::yield();
        }
    }
};
} // namespace Concurrent
#endif // CONCURRENTHUFFMAN_THREAD_POOL_H
