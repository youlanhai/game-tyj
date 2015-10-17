#pragma once

#include "Base.h"
#include "RenderObj.h"

class cCallBackPool;
LZDLL_API cCallBackPool* getCallBackPool();
//////////////////////////////////////////////////////////////////////////
///回调对象基类。
class LZDLL_API callObj : public IUpdate, public IBase
{
public:
    callObj() : m_time(0.0f)  { }

    void init(float time);

    void update(float fElapse);

    virtual void onDead(void) = 0;

private:
    float m_time;
};

//////////////////////////////////////////////////////////////////////////
///默认回调函数结构
template<typename Type>
class cDefaultCall : public callObj
{
public:
    cDefaultCall(float time, Type call) 
        : m_pCallBack(call)
    {
        init(time);
    }

    virtual void onDead()
    {
        if (m_pCallBack)
        {
            m_pCallBack();
        }
        getCallBackPool()->remove(this);
    }

protected:
    Type m_pCallBack;
};


template<typename Type>
callObj* callBack(float time, Type call)
{
    return getCallBackPool()->add(new cDefaultCall<Type>(time, call));
}
//////////////////////////////////////////////////////////////////////////

///回调函数池
class LZDLL_API cCallBackPool : public IBase
{
public:
    typedef std::list<RefPtr<callObj>> CallBackPool;
    typedef CallBackPool::iterator      CallBackIter;

    cCallBackPool();

    virtual callObj* add(callObj* pCall);

    virtual void remove(callObj* pCall);

protected:
    CallBackPool m_cbPool;
};