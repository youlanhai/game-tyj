/************************************************************************/
/*FPS.h                                                            */
/*帧频率计数器：获得游戏的帧刷新率fps。                                       */
/************************************************************************/
#pragma once

#include "RenderObj.h"

class LZDLL_API cFps : public IBase
{
public:
	cFps(void);

	void init( void );

	virtual void updateFPS(void);

	float getCurTime(void);	//获得当前时间

public:

	int getStartTime(void){ return m_nStartTime; }

	int getFrameCount(void){ return m_nFrameCount; }

	float getFPS(void){ return m_fFPS; }

private:
	int m_nStartTime;		//启动时间
    int m_nTotalFrame;      //从游戏启动到现在经过的帧数
	int m_nFrameCount;		//上次更新一来的帧数
	int m_nLastUpdateTime;	//上次更新时的时间
	float m_fFPS;			//帧刷新率
};

class LZDLL_API cFpsRender : public cFps, public IRenderObj
{
public:
    
    virtual void update(float fElapse);

    virtual void render(IDirect3DDevice9 * pDevice);
};