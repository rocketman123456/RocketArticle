#pragma once
#include "Core/Core.h"

namespace Rocket
{
    class Buffer
    {
        public:
        Buffer() = default;
        Buffer(const Buffer& rhs) = delete;
        Buffer& operator=(const Buffer& rhs) = delete;
        ~Buffer()
        {
            if (data_ != nullptr)
                data_.reset();
            data_ = nullptr;
        }

        explicit Buffer(size_t size, size_t alignment = 4) : size_(size)
        {
            data_ = Ref<uint8_t>(new uint8_t[size], [](uint8_t* v){ delete[]v; });
        }

        Buffer(Buffer&& rhs) noexcept
        {
            data_ = rhs.data_;
            size_ = rhs.size_;
            rhs.data_ = nullptr;
            rhs.size_ = 0;
        }

        Buffer& operator=(Buffer&& rhs) noexcept
        {
            data_.reset();
            data_ = rhs.data_;
            size_ = rhs.size_;
            rhs.data_ = nullptr;
            rhs.size_ = 0;
            return *this;
        }

        [[nodiscard]] Ref<uint8_t> GetData() { return data_; };
        [[nodiscard]] const Ref<uint8_t> GetData() const { return data_; };
        [[nodiscard]] size_t GetDataSize() const { return size_; };

        uint8_t* MoveData()
        {
            uint8_t* tmp = data_.get();
            data_.reset();
            size_ = 0;
            return tmp;
        }

        void SetData(Ref<uint8_t> data, size_t size)
        {
            if (data_ != nullptr) 
                data_.reset();
            data_ = data;
            size_ = size;
        }

    protected:
        Ref<uint8_t> data_{nullptr};
        size_t size_{0};
    };
}