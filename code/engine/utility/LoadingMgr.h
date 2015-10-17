//该模块实现多线程加载资源
//需要被加载的资源，实现TaskInterface接口，然后作为任务添加到LoadingMgr.

#pragma once

#include "ZThread.h"
#include "TaskMgr.h"

namespace Lazy
{
    
    class LoadingMgr : public IBase
    {
    public:
        /* 处理完成任务。此方法调用Task的onTaskFinish方法，以通知调用者。
           注意：此方法请在主线程中调用，以免造成线程不安全。
            */
        void processFinishTask();

        bool running() const { return m_running; }
        int getWorkStep() const { return m_workStep; }

        TaskPtr getTask();
        void addTask(TaskPtr task);

        static void init(int numThread = 4, int maxSend=-1);
        static void fini();
        static LoadingMgr * instance();

    public:

        //添加完成任务，外部请不要调用。
        void addFinishTask(TaskPtr task);

    private:

        LoadingMgr(int numThread, int maxSend);
        ~LoadingMgr(void);

        static LoadingMgr *s_pInstance;
    private:
        bool        m_running;
        int         m_maxSend;
        int         m_workStep;
        TaskMgr     m_workingMgr;
        TaskMgr     m_finishMgr;
        std::list<ZThreadPtr>   m_threadPool;//线程池
    };

    //////////////////////////////////////////////////////////////////////////
    //加载资源线程
    //////////////////////////////////////////////////////////////////////////
    class LoadingThread : public ZThreadBase
    {
    public:
        LoadingThread(int id, LoadingMgr * pMgr);
        ~LoadingThread();

        virtual void run();

    private:
        int          m_id;
        LoadingMgr  *m_pMgr;
    };

}//namespace Lazy