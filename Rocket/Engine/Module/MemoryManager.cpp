#include "Module/MemoryManager.h"

using namespace Rocket;

MemoryManager* Rocket::GetMemoryManager() { return new MemoryManager(); }

size_t* MemoryManager::block_size_lookup_ = nullptr;
BlockAllocator* MemoryManager::allocators_ = nullptr;

std::ostream& Rocket::operator<<(std::ostream& out, MemoryType type)
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
        block_size_lookup_ = new size_t[kMaxBlockSize + 1];
        size_t j = 0;
        for (size_t i = 0; i <= kMaxBlockSize; i++)
        {
            if (i > kBlockSizes[j]) ++j;
            block_size_lookup_[i] = j;
        }

        // initialize the allocators
        allocators_ = new BlockAllocator[kNumBlockSizes];
        for (size_t i = 0; i < kNumBlockSizes; i++)
        {
            allocators_[i].Reset(kBlockSizes[i], kPageSize, kAlignment);
        }

        s_bInitialized = true;
    }

    return 0; 
}

void MemoryManager::Finalize() 
{ 
    delete[] allocators_;
    delete[] block_size_lookup_;
    assert(map_memory_allocation_info_.empty()); 
}

void MemoryManager::Tick(Timestep ts) 
{
#if defined(RK_DEBUG)
    static float count = 0.0f;

    if (count >= 1000.0f)
    {
        //RK_CORE_TRACE("Memory Manager Info:");
        //for (auto info : map_memory_allocation_info_)
        //{
        //    RK_CORE_TRACE("{}\t{}:{}", info.first, info.second.page_memory_type, info.second.page_size);
        //}
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
        return allocators_ + block_size_lookup_[size];
    else
        return nullptr;
}

void* MemoryManager::Allocate(size_t size)
{
    BlockAllocator* pAlloc = LookUpAllocator(size);
    void* ptr = nullptr;
    if (pAlloc)
        ptr = pAlloc->Allocate();
    else
        ptr = malloc(size);
    //RK_CORE_TRACE("Allocate Size {}, {}", ptr, size);
    return ptr;
}

void MemoryManager::Free(void* p, size_t size)
{
    //RK_CORE_TRACE("Free Size {}, {}", p, size);
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
        map_memory_allocation_info_.insert({p, info});
    }

    return static_cast<void*>(p);
}

void MemoryManager::FreePage(void* p)
{
    auto it = map_memory_allocation_info_.find(p);
    if (it != map_memory_allocation_info_.end()) {
        map_memory_allocation_info_.erase(it);
        free(p);
    }
}
