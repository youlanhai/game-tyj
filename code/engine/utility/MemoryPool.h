#pragma once

#include <cassert>

namespace Lazy
{
    const size_t MinNodeSize = sizeof(void*);
    const size_t MaxNodeSize = 1024;
    const size_t MaxPoolSize = 16 * MaxNodeSize;
    const size_t AllocSizeStep = 16;
    const size_t AllocPoolSize = MaxNodeSize/AllocSizeStep + 1;

    void* allocMemory(size_t n);
    void freeMemory(void* ptr);

    inline size_t round(size_t n, size_t base)
    {
        return (n + base - 1) / base * base;
    }

    //内存分配器
    class Allocater
    {
        MAKE_UNCOPY(Allocater)

        struct AllocNode
        {
            AllocNode *pNext;
        };

    public:

        explicit Allocater(size_t cellSize);

        ~Allocater();

        //分配内存
        void* alloc();

        //回收内存
        void free(void* ptr);

        inline bool isMIn(void *ptr) const
        {
            if(NULL == m_pPool) return false;

            return ptr >= m_pPool && ptr <((char*)m_pPool + m_num * m_cellSize);
        }

        size_t cellSize() const { return m_cellSize; }

        bool empty() const { return numFreeNode() == m_num; }

        //占用内存大小
        size_t bytes() const
        {
            size_t size = sizeof(*this);

            if(m_pPool) size += m_cellSize * m_num;
            if(m_pNext) size += m_pNext->bytes();

            return size;
        }

        size_t numFreeNode() const
        {
            size_t n = 0;
            AllocNode* p = m_free.pNext;
            while(p)
            {
                n += 1;
                p = p->pNext;
            }
            return n;
        }

        size_t numEmptyAllocater() const
        {
            size_t n = 0;
            if(empty()) n += 1;
            if(m_pNext) n += m_pNext->numEmptyAllocater();
            return n;
        }

        size_t numAllocater() const
        {
            if(m_pNext) return 1 + m_pNext->numAllocater();
            return 1;
        }

        size_t numMemoryFree() const
        {
            size_t n = 0;

            AllocNode* p = m_free.pNext;
            while(p)
            {
                n += m_cellSize;
                p = p->pNext;
            }

            if(m_pNext) n += m_pNext->numMemoryFree();
            return n;
        }

        size_t numMemoryTotal() const
        {
            size_t n = 0;
            if(m_pPool) n += MaxPoolSize;
            if(m_pNext) n += m_pNext->numMemoryTotal();
            return n;
        }

    protected:

        //分配池
        bool allocPool();

        //初始化自由链表
        void initFreeList();

        //改变单元大小。只有在未使用的情况下调用。
        void changeCellSize(size_t cellSize);

    private:

        size_t      m_num;
        size_t      m_cellSize;
        AllocNode*  m_pPool;
        AllocNode   m_free;
        Allocater*  m_pNext;

        friend class MemoryPool;
    };

    //内存池
    class MemoryPool
    {
    public:
        static void init();
        static void fini();
        static MemoryPool* instance();

        //分配一个分配器
        Allocater* newAllocater(size_t cellSize);

        //回收分配器及其子分配器。
        void releaseAllocater(Allocater *pAlloc);

        //分配内存
        void *alloc(size_t size);

        //释放内存
        void free(void *ptr);

        size_t bytes() const;
        size_t numFreeAllocater() const;
        size_t numTotalAllocater() const;
        size_t numMemoryFree() const;
        size_t numMemoryTotal() const;
        void report(tstring & info) const;
        void report() const;

        //删除不用的分配器，以减轻内存消耗。
        void deleteFreeAllocator();

    private:
        MemoryPool(void);
        ~MemoryPool(void);

        static MemoryPool *s_pInstance;
        Allocater* m_freeList;
        
        Allocater* m_pool[AllocPoolSize];
    };


//内存池分配申明
#define DEC_MEMORY_ALLOCATER()      \
void* operator new (size_t n) { return ::Lazy::MemoryPool::instance()->alloc(n); }    \
void operator delete(void *ptr) { ::Lazy::MemoryPool::instance()->free(ptr); }


}//end namespace Lazy