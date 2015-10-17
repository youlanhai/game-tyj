
#include "Commen.h"
#include "Utilities.h"
#include "CallBack.h"
#include "App.h"

void callObj::init(float time)
{
    m_time = time;
    getApp()->addUpdater(this);
}
void callObj::update(float fElapse)
{
    m_time -= fElapse;
    if (m_time <= 0.0f)
    {
        getApp()->removeUpdater(this);
        onDead();
    }
}


//////////////////////////////////////////////////////////////////////////
cCallBackPool* g_pCBPool_ = NULL;
LZDLL_API cCallBackPool* getCallBackPool()
{
    return g_pCBPool_;
}

cCallBackPool::cCallBackPool()
{
    g_pCBPool_ = this;
}

callObj* cCallBackPool::add(callObj* pCall)
{
    if (std::find(m_cbPool.begin(), m_cbPool.end(), pCall) == m_cbPool.end())
    {
        m_cbPool.push_back(pCall);
    }
    return pCall;
}

void cCallBackPool::remove(callObj* pCall)
{
    m_cbPool.remove(pCall);
}