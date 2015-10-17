﻿//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "UtilConfig.h"
#include "RefPtr.h"
#include "ZLock.h"

namespace Lazy
{

//////////////////////////////////////////////////////////////////////////
//线程基类
class ZThreadBase : public IBase
{
public:
    ZThreadBase(bool start=true)
    {
        uint32 flag = 0;
        if (!start)
        {
            flag = CREATE_SUSPENDED;
        }
        m_hThread = (HANDLE)_beginthreadex(NULL, 0, _threadCall, this, flag, NULL);
    }

    void start()
    {
        ResumeThread(m_hThread);
    }
    void pause()
    {
        SuspendThread(m_hThread);
    }

    virtual ~ZThreadBase(void)
    {
        WaitForSingleObject(m_hThread, INFINITE);
        CloseHandle(m_hThread);
    }

    virtual void run() = 0;

private:
    HANDLE          m_hThread;

    static uint32 __stdcall _threadCall(void* param)
    {
        ZThreadBase *pThis = (ZThreadBase*)param;
        pThis->run();
        return 0;
    }
};

typedef RefPtr<ZThreadBase> ZThreadPtr;

typedef void(*ZThreadFunc)(void*);

//普通线程类
class ZThread : public ZThreadBase
{
public:
    ZThread(ZThreadFunc pf, void *param)
        : m_pFunc(pf)
        , m_pParam(param)
    {
        assert(pf && "函数指针不能为空！");
    }

    virtual void run()
    {
        m_pFunc(m_pParam);
    }

private:
    ZThreadFunc m_pFunc;
    void*       m_pParam;
};

//加强线程类.用于调用类成员函数。
template<typename T>
class ZThreadEx : public ZThreadBase
{
public:
    typedef void(T::*ZFuncPtr)(void);

    ZThreadEx(T *pobj, ZFuncPtr func) 
        : m_obj(pobj)
        , m_func(func) 
    {
        assert(pobj && func && "this指针和函数地址都不能为空！");
    }

    virtual void run()
    {
        (m_obj->*m_func)();
    }

private:
    T           *m_obj;
    ZFuncPtr     m_func;
};

}//namespace Lazy