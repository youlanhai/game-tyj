#include "StdAfx.h"
#include "MemoryCheck.h"
#include "Misc.h"

namespace Lazy
{


//////////////////////////////////////////////////////////////////////////
MemoryCheck* MemoryCheck::s_pInstance = NULL;
static bool s_valid = true;

MemoryCheck* MemoryCheck::instance(void)
{
    if (NULL == s_pInstance)
    {
        s_pInstance = new MemoryCheck();
    }
    return s_pInstance;
}

void MemoryCheck::release(void)
{
    s_valid = false;
    delete s_pInstance;
    s_pInstance = NULL;
}

MemoryCheck::MemoryCheck()
{
    debugMessage(_T("MemoryCheck start ..."));
}

MemoryCheck::~MemoryCheck()
{
    debugMessage(_T("MemoryCheck will exit ..."));

    bool isOK = true;
    for (Iterator it = m_map.begin();
        it != m_map.end();
        ++it)
    {
        if (it->second != 0)
        {
            debugMessageA("!!!!!!error : %s memory leak with [%d]!!!!!!", it->first.c_str(), it->second);
            isOK = false;
        }
    }

    if(isOK)
    {
        debugMessage(_T("MemoryCheck exit with OK."));
    }
}

void MemoryCheck::construct(std::string name, void* ptr)
{
    if (m_map.find(name) == m_map.end())
    {
        m_map[name] = 0;
    }
    ++m_map[name];
}

void MemoryCheck::destruct(std::string name, void* ptr)
{
    if (m_map.find(name) != m_map.end())
    {
        --m_map[name];
    }
}

void debugConstruct(std::string name, void* ptr)
{
#ifdef DEBUG_ALL_MEMORY_CHECK
    debugMessageA("%s construct at 0x%8.8d", name.c_str(), DWORD(ptr));
#endif

    if (s_valid)
    {
        MemoryCheck::instance()->construct(name, ptr);
    }
}

void debugDestruct(std::string name, void* ptr)
{
#ifdef DEBUG_ALL_MEMORY_CHECK
    debugMessageA("%s destruct at 0x%8.8d", name.c_str(), DWORD(ptr));
#endif

    if (s_valid)
    {
        MemoryCheck::instance()->destruct(name, ptr);
    }
}

}//namespace Lazy