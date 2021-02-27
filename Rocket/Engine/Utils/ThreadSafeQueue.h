#pragma once
#include <mutex>
#include <queue>

namespace Rocket
{
    template<typename T>
    class ThreadSafeQueue
    {
    private:
        //需要声明为mutable
        mutable std::mutex mut;
        std::queue<T> data_queue;
        std::condition_variable data_cond;
    public:
        ThreadSafeQueue() = default;
        ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

        ThreadSafeQueue(ThreadSafeQueue const& other)
        {
            //只需锁定对方的mutex
            std::lock_guard<std::mutex> lk(other.mut);
            data_queue = other.data_queue;
        }

        void push(T new_value)
        {
            std::lock_guard<std::mutex> lk(mut);
            data_queue.push(new_value);
            //如果此时队列中已经至少存在一个元素，则函数执行后什么也不影响
            data_cond.notify_one();
        }

        void wait_and_pop(T& value)
        {
            std::unique_lock<std::mutex> lk(mut);
            data_cond.wait(lk, [this] {return !data_queue.empty(); });
            value = data_queue.front();
            data_queue.pop();
        }

        std::shared_ptr<T> wait_and_pop()
        {
            std::unique_lock<std::mutex> lk(mut);
            data_cond.wait(lk, [this] {return !data_queue.empty(); });
            std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
            data_queue.pop();
            return res;
        }

        bool try_pop(T& value)
        {
            std::lock_guard<std::mutex> lk(mut);
            if (data_queue.empty())
                return false;
            value = data_queue.front();
            data_queue.pop();
            return true;
        }

        std::shared_ptr<T> try_pop()
        {
            std::lock_guard<std::mutex> lk(mut);
            if (data_queue.empty())
                return std::shared_ptr<T>();
            std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
            data_queue.pop();
            return res;
        }

        bool empty() const
        {
            //需要锁定
            std::lock_guard<std::mutex> lk(mut);
            return data_queue.empty();
        }
    };
}