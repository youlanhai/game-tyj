#include "StdAfx.h"
#include "TaskMgr.h"
#include "Misc.h"

namespace Lazy
{

TaskMgr::TaskMgr(void)
{
}


TaskMgr::~TaskMgr(void)
{
}

void TaskMgr::addTask(TaskPtr task)
{
    m_locker.lock();

    m_tasks.push_back(task);

    m_locker.unlock();
}

//如果没有任务，此方法会返回NULL。
TaskPtr TaskMgr::getTask()
{
    ZLockHolder holder(&m_locker);

    TaskList::iterator it = m_tasks.begin();
    for (; it != m_tasks.end(); ++it)
    {
        if ((*it)->isReady())
        {
            break;
        }
    }
    if (it == m_tasks.end())
    {
        return NULL;
    }

    TaskPtr task = *it;
    m_tasks.erase(it);

    return task;
}

TaskWithFun::TaskWithFun(TASK_FUN pWork, TASK_FUN pFinish)
    : m_pWork(pWork)
    , m_pFinish(pFinish)
{

}

void TaskWithFun::doTask(void)
{
    if (m_pWork)
    {
        m_pWork();
    }
}
void TaskWithFun::onTaskFinish(void)
{
    if (m_pFinish)
    {
        m_pFinish();
    }
}

}//namespace Lazy