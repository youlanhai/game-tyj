#include "StdAfx.h"
#include "Misc.h"
#include "MemoryPool.h"
#include "MemoryCheck.h"
#include <sstream>

namespace Lazy
{
    const size_t MemoryMarkHigh = 0xffff0000;
    const size_t MemoryMarkLow = 0xffff;

    void* allocMemory(size_t n){ return malloc(n); }
    void freeMemory(void* ptr){ return free(ptr); }

    Allocater::Allocater(size_t cellSize)
        : m_pPool(NULL)
        , m_pNext(NULL)
    {
        assert(cellSize <= MaxNodeSize + AllocSizeStep && "The cellSize is big than reserve size!");

        m_free.pNext = NULL;

        changeCellSize(cellSize);

        MEMORY_CHECK_CONS;
    }

    Allocater::~Allocater()
    {
        //TODO: 检查分配出去的内存，是否都已经归还。

        freeMemory(m_pPool);

        if(m_pNext)
        {
            MemoryPool::instance()->releaseAllocater(m_pNext);
        }

        MEMORY_CHECK_DEST;
    }

    //分配内存
    void* Allocater::alloc()
    {
        if(NULL == m_pPool)
        {
            if(!allocPool()) return NULL;
        }

        if(NULL == m_free.pNext)//没有可用内存，则增加一个分配池。
        {
            if(NULL == m_pNext)
            {
                m_pNext = MemoryPool::instance()->newAllocater(m_cellSize);
                if(NULL == m_pNext) return NULL;
            }
            return m_pNext->alloc();
        }

        //使用当前分配池分配内存
        AllocNode* p = m_free.pNext;
        m_free.pNext = p->pNext;
        return p;
    }

    //回收内存
    void Allocater::free(void* ptr)
    {
        assert(ptr != NULL);

        if(isMIn(ptr))
        {
            AllocNode* p = (AllocNode*)ptr;
            p->pNext = m_free.pNext;
            m_free.pNext = p;
        }
        else if(m_pNext)
        {
            m_pNext->free(ptr);
        }
        else
        {
            debugMessage(_T("ERROR: can't find the memory's ownner. 0X%8.8X"), ptr);
        }


        //自动释放
        if(numFreeNode() / float(m_num) >= 0.5f)
        {
            while(m_pNext && m_pNext->empty())
            {
                Allocater* p = m_pNext;
                m_pNext = m_pNext->m_pNext;

                p->m_pNext = NULL;
                MemoryPool::instance()->releaseAllocater(p);
            }
        }

    }

    bool Allocater::allocPool()
    {
        assert(NULL == m_pPool);

        m_pPool = (AllocNode*)allocMemory(MaxPoolSize);
        if(m_pPool == NULL) return false;

        initFreeList();
        return true;
    }

    //初始化自由链表
    void Allocater::initFreeList()
    {
        m_free.pNext = m_pPool;
        if(!m_pPool) return;

        AllocNode* ptr = m_pPool;

        for(size_t i=0; i<m_num-1; ++i)
        {
            ptr->pNext = (AllocNode*)((char*)ptr + m_cellSize);
            ptr = ptr->pNext;
        }

        ptr->pNext = NULL;
    }

    //改变单元大小。只有在未使用的情况下调用。
    void Allocater::changeCellSize(size_t cellSize)
    {
        m_cellSize = max(MinNodeSize, cellSize);//保证最小分配尺寸不会小于一个指针大小
        m_num = MaxPoolSize / m_cellSize;

        initFreeList();
    }

    ///////////////////////////////////////////////////////////////////

    /*static*/ MemoryPool *MemoryPool::s_pInstance = NULL;

    MemoryPool::MemoryPool(void)
        : m_freeList(NULL)
    {
        for(size_t i=0; i<AllocPoolSize; ++i)
        {
            m_pool[i] = newAllocater((i+1) * AllocSizeStep);
        }
    }

    MemoryPool::~MemoryPool(void)
    {
        for(size_t i=0; i<AllocPoolSize; ++i)
        {
            releaseAllocater(m_pool[i]);
        }

        deleteFreeAllocator();
    }

    /*static*/ void MemoryPool::init()
    {
        assert(s_pInstance==NULL && "has been initilized!");

        s_pInstance = new MemoryPool();
    }

    /*static*/ void MemoryPool::fini()
    {
        if(s_pInstance)
        {
            delete s_pInstance;
            s_pInstance = NULL;
        }
    }

    /*static*/ MemoryPool* MemoryPool::instance()
    {
        assert(s_pInstance && "the MemoryPool doesn't initialized or has been freed!");
        return s_pInstance;
    }

    Allocater* MemoryPool::newAllocater(size_t cellSize)
    {
        if(m_freeList)
        {
            Allocater* p = m_freeList;
            m_freeList = m_freeList->m_pNext;

            p->m_pNext = NULL;
            p->changeCellSize(cellSize);
            return p;
        }
        else
        {
            return new Allocater(cellSize);
        }
    }

    void MemoryPool::releaseAllocater(Allocater *pAlloc)
    {
        if(!m_freeList)
        {
            m_freeList = pAlloc;
        }
        else
        {
            Allocater *pEnd = m_freeList;
            while(pEnd->m_pNext)
            {
                pEnd = pEnd->m_pNext;
            }

            pEnd->m_pNext = pAlloc;
        }
    }
    
    void* MemoryPool::alloc(size_t size)
    {
        size += sizeof(size_t);

        void *ptr;
        if(size > MaxNodeSize)
        {
            ptr = allocMemory(size);
        }
        else
        {
            size = round(size, AllocSizeStep);
            ptr = m_pool[size / AllocSizeStep]->alloc();
        }
        *((size_t*)ptr) = MemoryMarkHigh | size;//用开头4个字节，记录分配内存大小
        ptr = (size_t*)ptr + 1;

        return ptr;
    }

    void MemoryPool::free(void *ptr)
    {
        assert(ptr != NULL);

        ptr = (size_t*)ptr - 1;
        size_t header = *((size_t*)ptr);
        if((header & MemoryMarkHigh) != MemoryMarkHigh)//无效的内存标记
        {
            throw(std::runtime_error("invalid memory!"));
        }

        size_t size = header & MemoryMarkLow;
        if(size > MaxNodeSize)
        {
            freeMemory(ptr);
        }
        else
        {
            m_pool[size / AllocSizeStep]->free(ptr);
        }
    }

    size_t MemoryPool::bytes() const
    {
        size_t n = sizeof(*this);
        for(size_t i=0; i<AllocPoolSize; ++i)
        {
            n += m_pool[i]->bytes();
        }
        if(m_freeList) n += m_freeList->bytes();
        return n;
    }

    size_t MemoryPool::numFreeAllocater() const
    {
        if(m_freeList) return m_freeList->numAllocater();
        return 0;
    }

    size_t MemoryPool::numTotalAllocater() const
    {
        size_t n = numFreeAllocater();
        for(size_t i=0; i<AllocPoolSize; ++i)
        {
            n += m_pool[i]->numAllocater();
        }
        return n;
    }

    size_t MemoryPool::numMemoryFree() const
    {
        size_t n = 0;
        for(size_t i=0; i<AllocPoolSize; ++i)
        {
            n += m_pool[i]->numMemoryFree();
        }
        if(m_freeList) n += m_freeList->numMemoryFree();
        return n;
    }

    size_t MemoryPool::numMemoryTotal() const
    {
        size_t n = 0;
        for(size_t i=0; i<AllocPoolSize; ++i)
        {
            n += m_pool[i]->numMemoryTotal();
        }
        if(m_freeList) n += m_freeList->numMemoryTotal();
        return n;
    }

    //删除不用的分配器。可以减轻内存消耗。
    void MemoryPool::deleteFreeAllocator()
    {
        while(m_freeList)
        {
            Allocater *p = m_freeList;
            m_freeList = m_freeList->m_pNext;

            p->m_pNext = NULL;//不让其子分配器调用releaseAllocater
            delete p;
        }
    }

    void MemoryPool::report(tstring & info) const
    {
        std::wostringstream stream;

        stream<<L"total bytes: "<<bytes()<<std::endl;
        stream<<L"allocater: "<<numFreeAllocater()<<L"/"<<numTotalAllocater()<<std::endl;
        stream<<L"memory: "<<numMemoryFree()<<L"/"<<numMemoryTotal()<<std::endl;

        info = stream.str();
    }

    void MemoryPool::report() const
    {
        tstring info;
        report(info);
        debugMessage(info.c_str());
    }

}//end namespace Lazy