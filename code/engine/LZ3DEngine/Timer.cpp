#include "Timer.h"

CTimer::CTimer(void)
{
    m_fInterval = 1000000.0f;   //时间间隔
    m_fElapse = 0.0f;           //已经流逝时间
    m_bRepeat = false;          //是否循环计时
    m_pCallBack = NULL;         //回调函数
    m_pCallParam = NULL;
    m_bEnable = false;
}

CTimer::~CTimer(void)
{
}

void CTimer::create(float interval,bool repeat, TIMERCALL pCall,void *pParam)
{
    m_fInterval = interval;
    m_bRepeat = repeat;
    m_pCallBack = pCall;
    m_pCallParam = pParam;
    //初始状态
    m_bEnable = false;
    m_fElapse = 0.0f;
}

/*更新定时器*/
void CTimer::update(float fElapse)
{
    if (!m_bEnable)
    {
        return ;
    }
    m_fElapse += fElapse;
    if (m_fElapse >= m_fInterval)//一次计时完成
    {
        if(NULL != m_pCallBack)
        {
            m_pCallBack(m_pCallParam);
        }
        if (m_bRepeat)
        {
            m_fElapse = 0.0f;
        }
        else
        {
            m_fElapse = m_fInterval;
        }
    }
}

/*重置定时器到初始状态。*/
void CTimer::reset(void)
{
    m_bEnable = false;
    m_fElapse = 0.0f;
}

/*开始定时*/
void CTimer::start(void)
{
    if (isFinished())
    {
        m_bEnable = true;
        m_fElapse = 0.0f;
    }
    else if(!m_bEnable)
    {
        m_bEnable = true;
    }
}

/*停止定时*/
void CTimer::stop(void)
{
    m_bEnable = false;
}