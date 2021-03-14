// From : https://zhuanlan.zhihu.com/p/29092979
#pragma once
#include "Interface/IMemoryManager.h"
#include "Common/BlockAllocator.h"
#include "Utils/Hashing.h"
#include "Utils/Portable.h"

#include <functional>

namespace Rocket
{
    ENUM(MemoryType){CPU = "CPU"_i32, GPU = "GPU"_i32};
    std::ostream& operator<<(std::ostream& out, MemoryType type);

    class MemoryManager : implements IMemoryManager
    {
    public:
        template<typename T, typename... Arguments>
        T* New(Arguments... parameters)
        {
            return new (Allocate(sizeof(T))) T(parameters...);
        }

        template<typename T>
        void Delete(T *p)
        {
            reinterpret_cast<T*>(p)->~T();
            Free(p, sizeof(T));
        }

    public:
        RUNTIME_MODULE_TYPE(MemoryManager);
        MemoryManager() = default;
        virtual ~MemoryManager() = default;

        int Initialize() final;
        void Finalize() final;
        void Tick(Timestep ts) final;

        void* Allocate(size_t size);
        void  Free(void* p, size_t size);
        void* AllocatePage(size_t size) final;
        void  FreePage(void* p) final;

    private:
        struct MemoryAllocationInfo 
        {
            size_t PageSize;
            MemoryType PageMemoryType;
        };
        
        Map<void*, MemoryAllocationInfo> m_mapMemoryAllocationInfo;

    private:
        static size_t* m_pBlockSizeLookup;
        static BlockAllocator* m_pAllocators;
    private:
        static BlockAllocator* LookUpAllocator(size_t size);
    };

    MemoryManager* GetMemoryManager();
    extern MemoryManager* g_MemoryManager;

#if defined(USE_MEMORY_MANAGER)
#define RK_ALLOCATE(sz) g_MemoryManager->Allocate(sz)
#define RK_ALLOCATE_CLASS(T) g_MemoryManager->Allocate(sizeof(T))
#define RK_DELETE(T,p) g_MemoryManager->Delete<T>(p)
#define RK_FREE(p,n) g_MemoryManager->Free(p, n)
#else
#define RK_ALLOCATE(sz)
#define RK_ALLOCATE_CLASS(T)
#define RK_DELETE(T,p)
#define RK_FREE(p,n)
#endif

    template<typename T>
    class deleter
    {
    public:
        deleter() = default;
        deleter(const deleter& other) { RK_CORE_TRACE("Copy deleter."); }
        deleter(deleter&& other) { RK_CORE_TRACE("Move deleter."); }
        deleter& operator=(const deleter&) { RK_CORE_TRACE("Copy assign deleter."); return *this; }
        deleter& operator=(deleter&&) { RK_CORE_TRACE("Move assign deleter."); return *this; }

        void operator() (T* ptr)
        {
            RK_CORE_TRACE("Start of deleter function.");
            RK_DELETE(T, ptr);
            ptr = nullptr;
            RK_CORE_TRACE("End of deleter function.");
        }
    };
}