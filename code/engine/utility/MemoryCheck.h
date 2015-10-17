#pragma once

#include <map>

namespace Lazy
{


class MemoryCheck
{
public:
    typedef std::map<std::string, int>  Container;
    typedef Container::iterator         Iterator;

    static MemoryCheck* instance(void);

    static void release(void);

    void construct(std::string name, void* ptr);

    void destruct(std::string name, void* ptr);

private: 

    MemoryCheck();

    ~MemoryCheck();

    std::map<std::string, int> m_map;
    static MemoryCheck*     s_pInstance;
};

void debugConstruct(std::string name, void* ptr);
void debugDestruct(std::string name, void* ptr);

#ifndef MEMORY_CHECK_CONS
#   define MEMORY_CHECK_CONS  ::Lazy::debugConstruct(typeid(*this).name(), this)
#   define MEMORY_CHECK_DEST  ::Lazy::debugDestruct(typeid(*this).name(), this)
#endif

}//namespace Lazy