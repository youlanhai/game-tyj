#pragma once

#include "SmartPtr.h"
#include "ZLock.h"

namespace Lazy
{
    struct TaskInterface : public IBase
    {
        virtual void doTask(void) = 0;
        virtual void onTaskFinish(void) = 0;
        virtual bool isReady(void){ return true; }
    };

    typedef RefPtr<TaskInterface> TaskPtr;
    typedef std::list<TaskPtr> TaskList;

    class TaskMgr : IBase
    {
    public:
        TaskMgr(void);
        ~TaskMgr(void);

        void addTask(TaskPtr task);

        //如果没有任务，此方法会返回NULL。
        TaskPtr getTask();

    private:
        ZCritical   m_locker;
        TaskList    m_tasks;
    };

    typedef RefPtr<TaskMgr> TaskMgrPtr;


    template<typename T>
    class DefTaskObj : public TaskInterface
    {
    public:
        typedef void(T::*ZFuncPtr)(void);

        DefTaskObj(T* pThis, ZFuncPtr pWork, ZFuncPtr pFinish)
            : m_pThis(pThis)
            , m_pFWork(pWork)
            , m_pFFinish(pFinish)
        {}

        virtual void doTask(void)
        {
            (m_pThis->*m_pFWork)();
        }

        virtual void onTaskFinish(void)
        {
            (m_pThis->*m_pFFinish)();
        }

    private:
        T*          m_pThis;
        ZFuncPtr    m_pFWork;
        ZFuncPtr    m_pFFinish;
    };

    typedef void (*TASK_FUN)(void);

    class TaskWithFun : public TaskInterface
    {
    public:
        TaskWithFun(TASK_FUN pWork, TASK_FUN pFinish);

        virtual void doTask(void);
        virtual void onTaskFinish(void);

    private:
        TASK_FUN m_pWork;
        TASK_FUN m_pFinish;
    };
}//namespace Lazy
