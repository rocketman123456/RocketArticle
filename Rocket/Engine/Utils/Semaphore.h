#pragma once
#include "Core/Core.h"
#include "Utils/Timer.h"

class Semaphore
{
public:
	explicit Semaphore(uint32_t count = 0)
		: count_(count) {}

	void post()
	{
		{
			std::unique_lock lock(mutex_);
			++count_;
		}
		cv_.notify_one();
	}

	void post(uint32_t count)
	{
		{
			std::unique_lock lock(mutex_);
			count_ += count;
		}
		cv_.notify_all();
	}

	void wait()
	{
		std::unique_lock lock(mutex_);
		cv_.wait(lock, [this]() { return count_ != 0; });
		--count_;
	}

	template <typename Rep, typename Period>
	bool wait_for(const std::chrono::duration<Rep, Period> &t)
	{
		std::unique_lock lock(mutex_);
		if (!cv_.wait_for(lock, t, [this]() { return count_ != 0; }))
			return false;
		--count_;
		return true;
	}

	template <typename Clock, typename Duration>
	bool wait_until(const std::chrono::time_point<Clock, Duration> &t)
	{
		std::unique_lock lock(mutex_);
		if (!cv_.wait_until(lock, t, [this]() { return count_ != 0; }))
			return false;
		--count_;
		return true;
	}

private:
	uint32_t count_;
	std::mutex mutex_;
	std::condition_variable cv_;
};

class FastSemaphore
{
public:
	explicit FastSemaphore(uint32_t count = 0)
		: count_(count), semaphore_(0) {}

	void post()
	{
		int count = count_.fetch_add(1, std::memory_order_release);
		if (count < 0)
			semaphore_.post();
	}

	void wait()
	{
		int count = count_.fetch_sub(1, std::memory_order_acquire);
		if (count < 1)
			semaphore_.wait();
	}

private:
	std::atomic_int count_;
	Semaphore semaphore_;
};