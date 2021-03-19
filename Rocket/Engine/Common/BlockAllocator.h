#pragma once
#include "Interface/IAllocator.h"

namespace Rocket
{
    struct BlockHeader
    {
        // union-ed with data
        BlockHeader* next;
    };

    struct PageHeader
    {
        PageHeader* next;
        BlockHeader* Blocks() { return reinterpret_cast<BlockHeader*>(this + 1); }
    };

    class BlockAllocator : implements IAllocator
    {
    public:
        BlockAllocator() = default;
        BlockAllocator(size_t data_size, size_t page_size, size_t alignment);
        virtual ~BlockAllocator() { FreeAll(); }
        // disable copy & assignment
        BlockAllocator(const BlockAllocator& clone) = delete;
        BlockAllocator& operator=(const BlockAllocator& rhs) = delete;

        // resets the allocator to a new configuration
        void Reset(size_t data_size, size_t page_size, size_t alignment);

        // alloc and free blocks
        void* Allocate() final;
        void* Allocate(size_t size) final;
        void Free(void* p) final;
        void FreeAll() final;

    private:
#if defined(RK_DEBUG)
        // fill a free page with debug patterns
        void FillFreePage(PageHeader* page);
        // fill a block with debug patterns
        void FillFreeBlock(BlockHeader* block);
        // fill an allocated block with debug patterns
        void FillAllocatedBlock(BlockHeader* block);
#endif

        // gets the next block
        BlockHeader* NextBlock(BlockHeader* block);

        // the page list
        PageHeader* page_list_ = nullptr;

        // the free block list
        BlockHeader* free_list_ = nullptr;

        size_t page_size_;
        size_t alignment_size_;
        size_t block_size_;
        size_t blocks_per_page_;

        // statistics
        size_t pages_;
        size_t blocks_;
        size_t free_blocks_;
    };
}