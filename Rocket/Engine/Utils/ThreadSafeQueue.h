#pragma once
#include <mutex>
#include <queue>

template<typename T>
class ThreadSafeQueue
{
private:
    //需要声明为mutable
    mutable std::mutex mutex_;
    std::queue<T> data_queue_;
    std::condition_variable data_cond_;
public:
    ThreadSafeQueue() = default;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    ThreadSafeQueue(ThreadSafeQueue const& other)
    {
        //只需锁定对方的mutex
        std::lock_guard<std::mutex> lk(other.mutex_);
        data_queue_ = other.data_queue_;
    }

    void push(T new_value)
    {
        std::lock_guard<std::mutex> lk(mutex_);
        data_queue_.push(new_value);
        //如果此时队列中已经至少存在一个元素，则函数执行后什么也不影响
        data_cond_.notify_one();
    }

    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lk(mutex_);
        data_cond_.wait(lk, [this] {return !data_queue_.empty(); });
        value = data_queue_.front();
        data_queue_.pop();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mutex_);
        data_cond_.wait(lk, [this] {return !data_queue_.empty(); });
        std::shared_ptr<T> res(std::make_shared<T>(data_queue_.front()));
        data_queue_.pop();
        return res;
    }

    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lk(mutex_);
        if (data_queue_.empty())
            return false;
        value = data_queue_.front();
        data_queue_.pop();
        return true;
    }

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mutex_);
        if (data_queue_.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> res(std::make_shared<T>(data_queue_.front()));
        data_queue_.pop();
        return res;
    }

    bool empty() const
    {
        //需要锁定
        std::lock_guard<std::mutex> lk(mutex_);
        return data_queue_.empty();
    }
};
