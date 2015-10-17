#pragma once
#include "base.h"

typedef void (*TIMERCALL)(void *pParam);

/*定时器类。定时单位为毫秒。
使用create创建定时器，创建后定时器处于就绪状态，不可用。
调用start启动定时器，如果定时器处于运行状态，则启动无效。
update更新定时器。
调用stop来停止定时器。
reset重置定时器到就绪状态。如果要重头开始计时，则需要调用reset，然后调用start。
*/
class LZDLL_API CTimer : public IBase
{
public:
    CTimer(void);
    ~CTimer(void);
    
    /*创建定时器。*/
    void create(float interval,bool repeat = false, 
        TIMERCALL pCall = NULL,void *pParam = NULL);

    /*开始定时*/
    void start(void);

    /*重置定时器到初始状态。*/
    void reset(void);

    /*停止定时*/
    void stop(void);

    /*更新定时器*/
    void update(float fElapse);

    /*是否计时完成。仅对非循环定时器起作用。*/
    bool isFinished(void){ return m_fElapse >= m_fInterval; };

    /*定时器是否正在运行*/
    bool isRunning(void){ return ((!isFinished()) && m_bEnable); }

    /*获得计时进度*/
    float getPercent(void){ return m_fElapse/m_fInterval; }

public://属性

    /*设置定时时间间隔*/
    float getInterval(void){ return m_fInterval; }
    void setInterval(float time){ m_fInterval = time; }

    /*设置回调函数*/
    void setCallBack(TIMERCALL pCall){ m_pCallBack = pCall; }
    void setCallParam(void *pParam){ m_pCallParam = pParam; }

    /*设置重复*/
    bool getRepeat(void){ return m_bRepeat; }
    void setRepeat(bool r){ m_bRepeat = r; }

private:
    bool    m_bEnable;      //是否可用
    float   m_fInterval;    //时间间隔
    float   m_fElapse;      //已经流逝时间
    bool    m_bRepeat;      //是否循环计时
    TIMERCALL m_pCallBack;  //回调函数
    void      *m_pCallParam;//回调函数参数
};
