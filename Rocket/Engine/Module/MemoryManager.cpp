#include "Module/MemoryManager.h"

namespace Rocket
{
    MemoryManager* GetMemoryManager() { return new MemoryManager(); }

    size_t* MemoryManager::m_pBlockSizeLookup = nullptr;
    BlockAllocator* MemoryManager::m_pAllocators = nullptr;

    std::ostream& operator<<(std::ostream& out, MemoryType type)
    {
        auto n = static_cast<int32_t>(type);
        n = endian_net_unsigned_int<int32_t>(n);
        char* c = reinterpret_cast<char*>(&n);

        for (size_t i = 0; i < sizeof(int32_t); i++)
        {
            out << *c++;
        }

        return out;
    }

    static const uint32_t kBlockSizes[] = {
        // 4-increments
        4,  8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48,
        52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 

        // 32-increments
        128, 160, 192, 224, 256, 288, 320, 352, 384, 
        416, 448, 480, 512, 544, 576, 608, 640, 

        // 64-increments
        704, 768, 832, 896, 960, 1024
    };

    static const uint32_t kPageSize  = 8192;
    static const uint32_t kAlignment = 4;
    // number of elements in the block size array
    static const uint32_t kNumBlockSizes = sizeof(kBlockSizes) / sizeof(kBlockSizes[0]);
    // largest valid block size
    static const uint32_t kMaxBlockSize = kBlockSizes[kNumBlockSizes - 1];

    int MemoryManager::Initialize() 
    {
        // one-time initialization
        static bool s_bInitialized = false;
        if (!s_bInitialized)
        {
            // initialize block size lookup table
            m_pBlockSizeLookup = new size_t[kMaxBlockSize + 1];
            size_t j = 0;
            for (size_t i = 0; i <= kMaxBlockSize; i++)
            {
                if (i > kBlockSizes[j]) ++j;
                m_pBlockSizeLookup[i] = j;
            }

            // initialize the allocators
            m_pAllocators = new BlockAllocator[kNumBlockSizes];
            for (size_t i = 0; i < kNumBlockSizes; i++)
            {
                m_pAllocators[i].Reset(kBlockSizes[i], kPageSize, kAlignment);
            }

            s_bInitialized = true;
        }

        return 0; 
    }

    void MemoryManager::Finalize() 
    { 
        delete[] m_pAllocators;
        delete[] m_pBlockSizeLookup;
        assert(m_mapMemoryAllocationInfo.empty()); 
    }

    void MemoryManager::Tick(Timestep ts) 
    {
    #if defined(RK_DEBUG)
        static float count = 0.0f;

        if (count >= 1000.0f)
        {
            RK_CORE_TRACE("Memory Manager Info:");
            for (auto info : m_mapMemoryAllocationInfo)
            {
                RK_CORE_TRACE("{}\t{}:{}", info.first, info.second.PageMemoryType, info.second.PageSize);
            }
            count = 0.0f;
        }
        else
        {
            count += ts;
        }
    #endif
    }

    BlockAllocator* MemoryManager::LookUpAllocator(size_t size)
    {
        // check eligibility for lookup
        if (size <= kMaxBlockSize)
            return m_pAllocators + m_pBlockSizeLookup[size];
        else
            return nullptr;
    }

    void* MemoryManager::Allocate(size_t size)
    {
        BlockAllocator* pAlloc = LookUpAllocator(size);
        if (pAlloc)
            return pAlloc->Allocate();
        else
            return malloc(size);
    }

    void MemoryManager::Free(void* p, size_t size)
    {
        BlockAllocator* pAlloc = LookUpAllocator(size);
        if (pAlloc)
            pAlloc->Free(p);
        else
            free(p);
    }

    void* MemoryManager::AllocatePage(size_t size)
    {
        uint8_t* p;

        p = static_cast<uint8_t*>(malloc(size));
        if (p) {
            MemoryAllocationInfo info = {size, MemoryType::CPU};
            m_mapMemoryAllocationInfo.insert({p, info});
        }

        return static_cast<void*>(p);
    }

    void MemoryManager::FreePage(void* p)
    {
        auto it = m_mapMemoryAllocationInfo.find(p);
        if (it != m_mapMemoryAllocationInfo.end()) {
            m_mapMemoryAllocationInfo.erase(it);
            free(p);
        }
    }
}