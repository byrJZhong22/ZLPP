//
// Created by jun on 18-2-20.
//

#ifndef ZLPP_THREAD_POOL_H
#define ZLPP_THREAD_POOL_H

#include <stdlib.h>
#include <future>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>

namespace zh {
    class ThreadPool {
    public:
        //Constructor and Destructor
        ThreadPool(size_t);
        ~ThreadPool();

        //Add task to current queue
        template<class F, class... Args>
        auto enqueue(F&& f, Args&&... args)
        -> std::future<decltype(f(args...))>;     //> std::future<typename std::result_of<F(Args...)>::type> would warning

        void Sync(int wait_count);
        size_t ThreadNumber();

    private:
        //need to keep track of threads so we can join them
        std::vector<std::thread> workers;
        //the tasl queue
        std::queue<std::function<void()>> tasks;

        //synchronization
        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop;
        std::atomic_int sync{0};
    };

    ThreadPool::ThreadPool(size_t threads)
        : stop(false) {
        for (size_t i = 0; i < threads; ++i) {
            workers.emplace_back(
                    [this]
                    {
                        for (;;) {
                            std::function<void()> task;
                            {
                                std::unique_lock<std::mutex> lock(this->queue_mutex);
                                this->condition.wait(lock,
                                                    [this]{return this->stop || !this->tasks.empty();});
                                if (this->stop && this->tasks.empty())
                                    return;
                                task = std::move(this->tasks.front());
                                this->tasks.pop();
                            }
                            task();
                            sync++;
                        }
                    }
            );
        }
    }

    void ThreadPool::~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(this->queue_mutex);
            this->stop = true;
        }

        condition.notify_all();
        for (std::thread &t : workers) {
            t.join();
        }
    }

    // Wait all thread to finish their jobs
    inline void ThreadPool::Sync(int wait_count) {
        while (sync != wait_count) {}
        sync = 0;
    }

    inline size_t ThreadPool::ThreadNumber() {
        return workers.size();
    }

}


#endif //ZLPP_THREAD_POOL_H
