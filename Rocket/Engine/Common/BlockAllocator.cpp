#include "Common/BlockAllocator.h"
#include "Module/MemoryManager.h"

#include <cassert>
#include <cstdlib>
#include <cstring>

using namespace Rocket;

BlockAllocator::BlockAllocator(size_t data_size, size_t page_size, size_t alignment)
    : page_list_(nullptr), free_list_(nullptr)
{
    Reset(data_size, page_size, alignment);
}

void BlockAllocator::Reset(size_t data_size, size_t page_size, size_t alignment)
{
    FreeAll();

    page_size_ = page_size;

    size_t minimal_size = (sizeof(BlockHeader) > data_size) ? sizeof(BlockHeader) : data_size;
    // this magic only works when alignment is 2^n, which should general be the
    // case because most CPU/GPU also requires the aligment be in 2^n but still
    // we use a assert to guarantee it
#if defined(RK_DEBUG)
    RK_CORE_ASSERT(alignment > 0 && ((alignment & (alignment - 1))) == 0, "Alignment Error");
#endif
    block_size_ = RK_ALIGN(minimal_size, alignment);

    alignment_size_ = block_size_ - minimal_size;

    blocks_per_page_ = (page_size_ - sizeof(PageHeader)) / block_size_;
}

void* BlockAllocator::Allocate(size_t size)
{
    assert(size <= block_size_);
    return Allocate();
}

void* BlockAllocator::Allocate()
{
    if (!free_list_)
    {
        // allocate a new page
        //PageHeader* pNewPage = reinterpret_cast<PageHeader*>(new uint8_t[page_size_]);
        auto* pNewPage = reinterpret_cast<PageHeader*>(g_MemoryManager->AllocatePage(page_size_));
        ++pages_;
        blocks_ += blocks_per_page_;
        free_blocks_ += blocks_per_page_;

#if defined(RK_DEBUG)
        FillFreePage(pNewPage);
#endif

        if (page_list_)
            pNewPage->next = page_list_;
        else
            pNewPage->next = nullptr;

        page_list_ = pNewPage;

        BlockHeader* block = pNewPage->Blocks();
        // link each block in the page
        for (uint32_t i = 0; i < blocks_per_page_ - 1; i++)
        {
            block->next = NextBlock(block);
            block = NextBlock(block);
        }
        block->next = nullptr;

        free_list_ = pNewPage->Blocks();
    }

    BlockHeader* freeBlock = free_list_;
    free_list_ = free_list_->next;
    --free_blocks_;

#if defined(RK_DEBUG)
    FillAllocatedBlock(freeBlock);
#endif

    return reinterpret_cast<void*>(freeBlock);
}

void BlockAllocator::Free(void* p)
{
    auto* block = reinterpret_cast<BlockHeader*>(p);

#if defined(RK_DEBUG)
    FillFreeBlock(block);
#endif

    block->next = free_list_;
    free_list_ = block;
    ++free_blocks_;
}

void BlockAllocator::FreeAll()
{
    PageHeader* pPage = page_list_;
    while (pPage)
    {
        PageHeader* _p = pPage;
        pPage = pPage->next;
        g_MemoryManager->FreePage(reinterpret_cast<void*>(_p));
        //delete[] reinterpret_cast<uint8_t*>(_p);
    }

    page_list_ = nullptr;
    free_list_ = nullptr;

    pages_ = 0;
    blocks_ = 0;
    free_blocks_ = 0;
}

#if defined(RK_DEBUG)
void BlockAllocator::FillFreePage(PageHeader* page)
{
    // page header
    page->next = nullptr;

    // blocks
    BlockHeader* block = page->Blocks();
    for (uint32_t i = 0; i < blocks_per_page_; i++)
    {
        FillFreeBlock(block);
        block = NextBlock(block);
    }
}

void BlockAllocator::FillFreeBlock(BlockHeader* block)
{
    // block header + data
    memset(block, PATTERN_FREE, block_size_ - alignment_size_);

    // alignment
    memset(reinterpret_cast<uint8_t*>(block) + block_size_ - alignment_size_, PATTERN_ALIGN, alignment_size_);
}

void BlockAllocator::FillAllocatedBlock(BlockHeader* block) {
    // block header + data
    memset(block, PATTERN_ALLOC, block_size_ - alignment_size_);

    // alignment
    memset(reinterpret_cast<uint8_t*>(block) + block_size_ - alignment_size_, PATTERN_ALIGN, alignment_size_);
}
#endif

BlockHeader* BlockAllocator::NextBlock(BlockHeader* block)
{
    return reinterpret_cast<BlockHeader*>(reinterpret_cast<uint8_t*>(block) + block_size_);
}
