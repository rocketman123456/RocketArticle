#pragma once
#include "Core/Core.h"
#include "Module/MemoryManager.h"

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
            if (m_pData != nullptr)
                m_pData.reset();
            m_pData = nullptr;
        }

        explicit Buffer(size_t size, size_t alignment = 4) : m_szSize(size)
        {
            m_pData = Ref<uint8_t>(new uint8_t[size], [](uint8_t* v){ delete[]v; });
        }

        Buffer(Buffer&& rhs) noexcept
        {
            m_pData = rhs.m_pData;
            m_szSize = rhs.m_szSize;
            rhs.m_pData = nullptr;
            rhs.m_szSize = 0;
        }

        Buffer& operator=(Buffer&& rhs) noexcept
        {
            m_pData.reset();
            m_pData = rhs.m_pData;
            m_szSize = rhs.m_szSize;
            rhs.m_pData = nullptr;
            rhs.m_szSize = 0;
            return *this;
        }

        [[nodiscard]] Ref<uint8_t> GetData() { return m_pData; };
        [[nodiscard]] const Ref<uint8_t> GetData() const { return m_pData; };
        [[nodiscard]] size_t GetDataSize() const { return m_szSize; };

        uint8_t* MoveData()
        {
            uint8_t* tmp = m_pData.get();
            m_pData.reset();
            m_szSize = 0;
            return tmp;
        }

        void SetData(Ref<uint8_t> data, size_t size)
        {
            if (m_pData != nullptr) 
                m_pData.reset();
            m_pData = data;
            m_szSize = size;
        }

    protected:
        Ref<uint8_t> m_pData{nullptr};
        size_t m_szSize{0};
    };
}