#include "StdAfx.h"
#include "LoadingMgr.h"
#include "Misc.h"

namespace Lazy
{
    /*static*/ LoadingMgr * LoadingMgr::s_pInstance = NULL;

    /*static*/ LoadingMgr * LoadingMgr::instance()
    {
        return s_pInstance;
    }

    /*static*/ void LoadingMgr::init(int numThread/* = 4*/, int maxSend/*=-1*/)
    {
        assert(s_pInstance == NULL && "LoadingMgr has been initialized.");

        s_pInstance = new LoadingMgr(numThread, maxSend);
    }

    /*static*/ void LoadingMgr::fini()
    {
        if(s_pInstance)
        {
            delete s_pInstance;
            s_pInstance = NULL;
        }
    }

    LoadingMgr::LoadingMgr(int numThread /*= 5*/, int maxSend/*=-1*/)
        : m_maxSend(maxSend)
        , m_running(true)
        , m_workStep(0)
    {
        debugMessage(_T("LoadingMgr 0x%8.8x construct."), this);

        for (int i=0; i<numThread; ++i)
        {
            m_threadPool.push_back(new LoadingThread(i, this));
        }
    }


    LoadingMgr::~LoadingMgr(void)
    {
        m_running = false;
        m_threadPool.clear();

        debugMessage(_T("LoadingMgr 0x%8.8x destruct."), this);
    }

    void LoadingMgr::processFinishTask()
    {
        ++m_workStep;

        int maxSend = m_maxSend;
        if (maxSend < 0)
        {
            maxSend = 0x7fffffff;
        }

        for (int i=0; i<maxSend; ++i)
        {
            TaskPtr task = m_finishMgr.getTask();
            if (!task)
            {
                break;
            }
            task->onTaskFinish();
        }
    }

    TaskPtr LoadingMgr::getTask()
    {
        return m_workingMgr.getTask();
    }

    void LoadingMgr::addTask(TaskPtr task)
    {
        m_workingMgr.addTask(task);
    }

    void LoadingMgr::addFinishTask(TaskPtr task)
    {
        m_finishMgr.addTask(task);
    }


    //////////////////////////////////////////////////////////////////////////

    LoadingThread::LoadingThread(int id, LoadingMgr * pMgr)
        : m_id(id)
        , m_pMgr(pMgr)
    {

    }

    LoadingThread::~LoadingThread()
    {

    }

    void LoadingThread::run()
    {
        debugMessage(_T("LoadingThread %d start..."), m_id);

        while(m_pMgr->running())
        {
            TaskPtr task = m_pMgr->getTask();
            if (task)
            {
                task->doTask();
                m_pMgr->addFinishTask(task);
                Sleep(10);
            }
            else
            {
                Sleep(100);
            }
        }

        debugMessage(_T("LoadingThread %d exit."), m_id);
    }

}//namespace Lazy