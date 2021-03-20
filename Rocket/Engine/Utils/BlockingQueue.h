#pragma once
#include "Core/Core.h"
#include "Utils/Semaphore.h"

template <typename T>
Interface blocking_queue
{
public:
	template <typename Q = T>
	typename std::enable_if<std::is_copy_constructible<Q>::value, void>::type
	push(const T &item)
	{
		{
			std::unique_lock lock(mutex_);
			queue_.push(item);
		}
		ready_.notify_one();
	}

	template <typename Q = T>
	typename std::enable_if<std::is_move_constructible<Q>::value, void>::type
		push(T && item)
	{
		{
			std::unique_lock lock(mutex_);
			queue_.emplace(std::forward<T>(item));
		}
		ready_.notify_one();
	}

	template <typename Q = T>
	typename std::enable_if<std::is_copy_constructible<Q>::value, bool>::type
	try_push(const T &item)
	{
		{
			std::unique_lock lock(mutex_, std::try_to_lock);
			if (!lock)
				return false;
			queue_.push(item);
		}
		ready_.notify_one();
		return true;
	}

	template <typename Q = T>
	typename std::enable_if<std::is_move_constructible<Q>::value, bool>::type
		try_push(T && item)
	{
		{
			std::unique_lock lock(mutex_, std::try_to_lock);
			if (!lock)
				return false;
			queue_.emplace(std::forward<T>(item));
		}
		ready_.notify_one();
		return true;
	}

	template <typename Q = T>
	typename std::enable_if<
		std::is_copy_assignable<Q>::value &&
			!std::is_move_assignable<Q>::value,
		bool>::type
		pop(T & item)
	{
		std::unique_lock lock(mutex_);
		while (queue_.empty() && !done_)
			ready_.wait(lock);
		if (queue_.empty())
			return false;
		item = queue_.front();
		queue_.pop();
		return true;
	}

	template <typename Q = T>
	typename std::enable_if<std::is_move_assignable<Q>::value, bool>::type
		pop(T & item)
	{
		std::unique_lock lock(mutex_);
		while (queue_.empty() && !done_)
			ready_.wait(lock);
		if (queue_.empty())
			return false;
		item = std::move(queue_.front());
		queue_.pop();
		return true;
	}

	template <typename Q = T>
	typename std::enable_if<
		std::is_copy_assignable<Q>::value &&
			!std::is_move_assignable<Q>::value,
		bool>::type
		try_pop(T & item)
	{
		std::unique_lock lock(mutex_, std::try_to_lock);
		if (!lock || queue_.empty())
			return false;
		item = queue_.front();
		queue_.pop();
		return true;
	}

	template <typename Q = T>
	typename std::enable_if<std::is_move_assignable<Q>::value, bool>::type
		try_pop(T & item)
	{
		std::unique_lock lock(mutex_, std::try_to_lock);
		if (!lock || queue_.empty())
			return false;
		item = std::move(queue_.front());
		queue_.pop();
		return true;
	}

	void done() noexcept
	{
		{
			std::unique_lock lock(mutex_);
			done_ = true;
		}
		ready_.notify_all();
	}

	bool empty() const noexcept
	{
		std::scoped_lock lock(mutex_);
		return queue_.empty();
	}

	uint32_t size() const noexcept
	{
		std::scoped_lock lock(mutex_);
		return queue_.size();
	}

private:
	std::queue<T> queue_;
	mutable std::mutex mutex_;
	std::condition_variable ready_;
	bool done_ = false;
};

template <typename T>
class fixed_blocking_queue
{
public:
	explicit fixed_blocking_queue(uint32_t size)
		: size_(size), push_index_(0), pop_index_(0), count_(0),
		  data_((T *)operator new(size * sizeof(T))),
		  open_slots_(size), full_slots_(0)
	{
		if (!size)
			throw std::invalid_argument("Invalid queue size!");
	}

	~fixed_blocking_queue() noexcept
	{
		while (count_--)
		{
			data_[pop_index_].~T();
			pop_index_ = ++pop_index_ % size_;
		}
		operator delete(data_);
	}

	template <typename Q = T>
	typename std::enable_if<
		std::is_copy_constructible<Q>::value &&
			std::is_nothrow_copy_constructible<Q>::value,
		void>::type
	push(const T &item) noexcept
	{
		open_slots_.wait();
		{
			std::scoped_lock lock(mutex_);
			new (data_ + push_index_) T(item);
			push_index_ = ++push_index_ % size_;
			++count_;
		}
		full_slots_.post();
	}

	template <typename Q = T>
	typename std::enable_if<
		std::is_copy_constructible<Q>::value &&
			!std::is_nothrow_copy_constructible<Q>::value,
		void>::type
	push(const T &item)
	{
		open_slots_.wait();
		{
			std::scoped_lock lock(mutex_);
			try
			{
				new (data_ + push_index_) T(item);
			}
			catch (...)
			{
				open_slots_.post();
				throw;
			}
			push_index_ = ++push_index_ % size_;
			++count_;
		}
		full_slots_.post();
	}

	template <typename Q = T>
	typename std::enable_if<
		std::is_move_constructible<Q>::value &&
			std::is_nothrow_move_constructible<Q>::value,
		void>::type
	push(T &&item) noexcept
	{
		open_slots_.wait();
		{
			std::scoped_lock lock(mutex_);
			new (data_ + push_index_) T(std::move(item));
			push_index_ = ++push_index_ % size_;
			++count_;
		}
		full_slots_.post();
	}

	template <typename Q = T>
	typename std::enable_if<
		std::is_move_constructible<Q>::value &&
			!std::is_nothrow_move_constructible<Q>::value,
		void>::type
	push(T &&item)
	{
		open_slots_.wait();
		{
			std::scoped_lock lock(mutex_);
			try
			{
				new (data_ + push_index_) T(std::move(item));
			}
			catch (...)
			{
				open_slots_.post();
				throw;
			}
			push_index_ = ++push_index_ % size_;
			++count_;
		}
		full_slots_.post();
	}

	template <typename Q = T>
	typename std::enable_if<
		std::is_copy_constructible<Q>::value &&
			std::is_nothrow_copy_constructible<Q>::value,
		bool>::type
	try_push(const T &item) noexcept
	{
		auto result = open_slots_.wait_for(std::chrono::seconds(0));
		if (!result)
			return false;
		{
			std::scoped_lock lock(mutex_);
			new (data_ + push_index_) T(item);
			push_index_ = ++push_index_ % size_;
			++count_;
		}
		full_slots_.post();
		return true;
	}

	template <typename Q = T>
	typename std::enable_if<
		std::is_copy_constructible<Q>::value &&
			!std::is_nothrow_copy_constructible<Q>::value,
		bool>::type
	try_push(const T &item)
	{
		auto result = open_slots_.wait_for(std::chrono::seconds(0));
		if (!result)
			return false;
		{
			std::scoped_lock lock(mutex_);
			try
			{
				new (data_ + push_index_) T(item);
			}
			catch (...)
			{
				open_slots_.post();
				throw;
			}
			push_index_ = ++push_index_ % size_;
			++count_;
		}
		full_slots_.post();
		return true;
	}

	template <typename Q = T>
	typename std::enable_if<
		std::is_move_constructible<Q>::value &&
			std::is_nothrow_move_constructible<Q>::value,
		bool>::type
	try_push(T &&item) noexcept
	{
		auto result = open_slots_.wait_for(std::chrono::seconds(0));
		if (!result)
			return false;
		{
			std::scoped_lock lock(mutex_);
			new (data_ + push_index_) T(std::move(item));
			push_index_ = ++push_index_ % size_;
			++count_;
		}
		full_slots_.post();
		return true;
	}

	template <typename Q = T>
	typename std::enable_if<
		std::is_move_constructible<Q>::value &&
			!std::is_nothrow_move_constructible<Q>::value,
		bool>::type
	try_push(T &&item)
	{
		auto result = open_slots_.wait_for(std::chrono::seconds(0));
		if (!result)
			return false;
		{
			std::scoped_lock lock(mutex_);
			try
			{
				new (data_ + push_index_) T(std::move(item));
			}
			catch (...)
			{
				open_slots_.post();
				throw;
			}
			push_index_ = ++push_index_ % size_;
			++count_;
		}
		full_slots_.post();
		return true;
	}

	template <typename Q = T>
	typename std::enable_if<
		!std::is_move_assignable<Q>::value &&
			std::is_nothrow_copy_assignable<Q>::value,
		void>::type
	pop(T &item) noexcept
	{
		full_slots_.wait();
		{
			std::scoped_lock lock(mutex_);
			item = data_[pop_index_];
			data_[pop_index_].~T();
			pop_index_ = ++pop_index_ % size_;
			--count_;
		}
		open_slots_.post();
	}

	template <typename Q = T>
	typename std::enable_if<
		!std::is_move_assignable<Q>::value &&
			!std::is_nothrow_copy_assignable<Q>::value,
		void>::type
	pop(T &item)
	{
		full_slots_.wait();
		{
			std::scoped_lock lock(mutex_);
			try
			{
				item = data_[pop_index_];
			}
			catch (...)
			{
				full_slots_.post();
				throw;
			}
			data_[pop_index_].~T();
			pop_index_ = ++pop_index_ % size_;
			--count_;
		}
		open_slots_.post();
	}

	template <typename Q = T>
	typename std::enable_if<
		std::is_move_assignable<Q>::value &&
			std::is_nothrow_move_assignable<Q>::value,
		void>::type
	pop(T &item) noexcept
	{
		full_slots_.wait();
		{
			std::scoped_lock lock(mutex_);
			item = std::move(data_[pop_index_]);
			data_[pop_index_].~T();
			pop_index_ = ++pop_index_ % size_;
			--count_;
		}
		open_slots_.post();
	}

	template <typename Q = T>
	typename std::enable_if<
		std::is_move_assignable<Q>::value &&
			!std::is_nothrow_move_assignable<Q>::value,
		void>::type
	pop(T &item)
	{
		full_slots_.wait();
		{
			std::scoped_lock lock(mutex_);
			try
			{
				item = std::move(data_[pop_index_]);
			}
			catch (...)
			{
				full_slots_.post();
				throw;
			}
			data_[pop_index_].~T();
			pop_index_ = ++pop_index_ % size_;
			--count_;
		}
		open_slots_.post();
	}

	template <typename Q = T>
	typename std::enable_if<
		!std::is_move_assignable<Q>::value &&
			std::is_nothrow_copy_assignable<Q>::value,
		bool>::type
	try_pop(T &item) noexcept
	{
		auto result = full_slots_.wait_for(std::chrono::seconds(0));
		if (!result)
			return false;
		{
			std::scoped_lock lock(mutex_);
			item = data_[pop_index_];
			data_[pop_index_].~T();
			pop_index_ = ++pop_index_ % size_;
			--count_;
		}
		open_slots_.post();
		return true;
	}

	template <typename Q = T>
	typename std::enable_if<
		!std::is_move_assignable<Q>::value &&
			!std::is_nothrow_copy_assignable<Q>::value,
		bool>::type
	try_pop(T &item)
	{
		auto result = full_slots_.wait_for(std::chrono::seconds(0));
		if (!result)
			return false;
		{
			std::scoped_lock lock(mutex_);
			try
			{
				item = data_[pop_index_];
			}
			catch (...)
			{
				full_slots_.post();
				throw;
			}
			data_[pop_index_].~T();
			pop_index_ = ++pop_index_ % size_;
			--count_;
		}
		open_slots_.post();
		return true;
	}

	template <typename Q = T>
	typename std::enable_if<
		std::is_move_assignable<Q>::value &&
			std::is_nothrow_move_assignable<Q>::value,
		bool>::type
	try_pop(T &item) noexcept
	{
		auto result = full_slots_.wait_for(std::chrono::seconds(0));
		if (!result)
			return false;
		{
			std::scoped_lock lock(mutex_);
			item = std::move(data_[pop_index_]);
			data_[pop_index_].~T();
			pop_index_ = ++pop_index_ % size_;
			--count_;
		}
		open_slots_.post();
		return true;
	}

	template <typename Q = T>
	typename std::enable_if<
		std::is_move_assignable<Q>::value &&
			!std::is_nothrow_move_assignable<Q>::value,
		bool>::type
	try_pop(T &item)
	{
		auto result = full_slots_.wait_for(std::chrono::seconds(0));
		if (!result)
			return false;
		{
			std::scoped_lock lock(mutex_);
			try
			{
				item = std::move(data_[pop_index_]);
			}
			catch (...)
			{
				full_slots_.post();
				throw;
			}
			data_[pop_index_].~T();
			pop_index_ = ++pop_index_ % size_;
			--count_;
		}
		open_slots_.post();
		return true;
	}

	bool empty() const noexcept
	{
		std::scoped_lock lock(mutex_);
		return count_ == 0;
	}

	bool full() const noexcept
	{
		std::scoped_lock lock(mutex_);
		return count_ == size_;
	}

	uint32_t size() const noexcept
	{
		std::scoped_lock lock(mutex_);
		return count_;
	}

	uint32_t capacity() const noexcept
	{
		return size_;
	}

private:
	const uint32_t size_;
	uint32_t push_index_;
	uint32_t pop_index_;
	uint32_t count_;
	T *data_;

	Semaphore open_slots_;
	Semaphore full_slots_;
	mutable std::mutex mutex_;
};

template <typename T>
class atomic_blocking_queue
{
public:
	explicit atomic_blocking_queue(uint32_t size)
		: size_(size), push_index_(0), pop_index_(0), count_(0),
		  data_((T *)operator new(size * sizeof(T))),
		  open_slots_(size), full_slots_(0)
	{
		if (!size)
			throw std::invalid_argument("Invalid queue size!");
	}

	~atomic_blocking_queue() noexcept
	{
		while (count_--)
		{
			data_[pop_index_].~T();
			pop_index_ = ++pop_index_ % size_;
		}
		operator delete(data_);
	}

	template <typename Q = T>
	typename std::enable_if<std::is_nothrow_copy_constructible<Q>::value, void>::type
	push(const T &item) noexcept
	{
		open_slots_.wait();

		auto pushIndex = push_index_.fetch_add(1);
		new (data_ + (pushIndex % size_)) T(item);
		++count_;

		auto expected = push_index_.load();
		while (!push_index_.compare_exchange_weak(expected, push_index_ % size_))
			expected = push_index_.load();

		full_slots_.post();
	}

	template <typename Q = T>
	typename std::enable_if<std::is_nothrow_move_constructible<Q>::value, void>::type
	push(T &&item) noexcept
	{
		open_slots_.wait();

		auto pushIndex = push_index_.fetch_add(1);
		new (data_ + (pushIndex % size_)) T(std::move(item));
		++count_;

		auto expected = push_index_.load();
		while (!push_index_.compare_exchange_weak(expected, push_index_ % size_))
			expected = push_index_.load();

		full_slots_.post();
	}

	template <typename Q = T>
	typename std::enable_if<std::is_nothrow_copy_constructible<Q>::value, bool>::type
	try_push(const T &item) noexcept
	{
		auto result = open_slots_.wait_for(std::chrono::seconds(0));
		if (!result)
			return false;

		auto pushIndex = push_index_.fetch_add(1);
		new (data_ + (pushIndex % size_)) T(item);
		++count_;

		auto expected = push_index_.load();
		while (!push_index_.compare_exchange_weak(expected, push_index_ % size_))
			expected = push_index_.load();

		full_slots_.post();
		return true;
	}

	template <typename Q = T>
	typename std::enable_if<std::is_nothrow_move_constructible<Q>::value, bool>::type
	try_push(T &&item) noexcept
	{
		auto result = open_slots_.wait_for(std::chrono::seconds(0));
		if (!result)
			return false;

		auto pushIndex = push_index_.fetch_add(1);
		new (data_ + (pushIndex % size_)) T(std::move(item));
		++count_;

		auto expected = push_index_.load();
		while (!push_index_.compare_exchange_weak(expected, push_index_ % size_))
			expected = push_index_.load();

		full_slots_.post();
		return true;
	}

	template <typename Q = T>
	typename std::enable_if<
		!std::is_move_assignable<Q>::value &&
			std::is_nothrow_copy_assignable<Q>::value,
		void>::type
	pop(T &item) noexcept
	{
		full_slots_.wait();

		auto popIndex = pop_index_.fetch_add(1);
		item = data_[popIndex % size_];
		data_[popIndex % size_].~T();
		--count_;

		auto expected = pop_index_.load();
		while (!pop_index_.compare_exchange_weak(expected, pop_index_ % size_))
			expected = pop_index_.load();

		open_slots_.post();
	}

	template <typename Q = T>
	typename std::enable_if<
		std::is_move_assignable<Q>::value &&
			std::is_nothrow_move_assignable<Q>::value,
		void>::type
	pop(T &item) noexcept
	{
		full_slots_.wait();

		auto popIndex = pop_index_.fetch_add(1);
		item = std::move(data_[popIndex % size_]);
		data_[popIndex % size_].~T();
		--count_;

		auto expected = pop_index_.load();
		while (!pop_index_.compare_exchange_weak(expected, pop_index_ % size_))
			expected = pop_index_.load();

		open_slots_.post();
	}

	template <typename Q = T>
	typename std::enable_if<
		!std::is_move_assignable<Q>::value &&
			std::is_nothrow_copy_assignable<Q>::value,
		bool>::type
	try_pop(T &item) noexcept
	{
		auto result = full_slots_.wait_for(std::chrono::seconds(0));
		if (!result)
			return false;

		auto popIndex = pop_index_.fetch_add(1);
		item = data_[popIndex % size_];
		data_[popIndex % size_].~T();
		--count_;

		auto expected = pop_index_.load();
		while (!pop_index_.compare_exchange_weak(expected, pop_index_ % size_))
			expected = pop_index_.load();

		open_slots_.post();
		return true;
	}

	template <typename Q = T>
	typename std::enable_if<
		std::is_move_assignable<Q>::value &&
			std::is_nothrow_move_assignable<Q>::value,
		bool>::type
	try_pop(T &item) noexcept
	{
		auto result = full_slots_.wait_for(std::chrono::seconds(0));
		if (!result)
			return false;

		auto popIndex = pop_index_.fetch_add(1);
		item = std::move(data_[popIndex % size_]);
		data_[popIndex % size_].~T();
		--count_;

		auto expected = pop_index_.load();
		while (!pop_index_.compare_exchange_weak(expected, pop_index_ % size_))
			expected = pop_index_.load();

		open_slots_.post();
		return true;
	}

	bool empty() const noexcept
	{
		return count_ == 0;
	}

	bool full() const noexcept
	{
		return count_ == size_;
	}

	uint32_t size() const noexcept
	{
		return count_;
	}

	uint32_t capacity() const noexcept
	{
		return size_;
	}

private:
	const uint32_t size_;
	std::atomic_uint push_index_;
	std::atomic_uint pop_index_;
	std::atomic_uint count_;
	T *data_;

	Semaphore open_slots_;
	Semaphore full_slots_;
};
