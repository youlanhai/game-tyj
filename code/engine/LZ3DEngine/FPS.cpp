//D3DTimer.cpp 
#include "Commen.h"
#include "FPS.h"

#include "Res.h"
#include "Utilities.h"

cFps::cFps(void)
{
    m_fFPS = 0.0f;
    m_nFrameCount = 0;
    m_nTotalFrame = 0;
	m_nLastUpdateTime = timeGetTime();;
	m_nStartTime = timeGetTime();
}

void cFps::init( void )
{
	m_nStartTime = timeGetTime();
}

void cFps::updateFPS( void )
{
	++m_nTotalFrame;
    ++m_nFrameCount;
	int nCurrentTime = timeGetTime();
    float elapse = (nCurrentTime - m_nLastUpdateTime)/1000.0f;
	if(elapse > 1.0f)//每1秒更新一次
	{
		m_fFPS = m_nFrameCount/elapse;
		m_nLastUpdateTime = nCurrentTime;
        m_nFrameCount = 0;
	}
}

float cFps::getCurTime(void)
{
	return (float)timeGetTime();
}

//////////////////////////////////////////////////////////////////////////
void cFpsRender::update(float fElapse)
{
    updateFPS();
}

void cFpsRender::render(IDirect3DDevice9 * pDevice)
{
    char buffer[128];
    sprintf_s(buffer, 128, "fps:%.2f", getFPS());
    CRect rc(0, 0, 500, 30);
    LPD3DXFONT font = getResMgr()->getFontEx("");
    if (font)
    {
        font->DrawText(NULL, Lazy::charToWChar(buffer).c_str(),
            -1, &rc, DT_LEFT|DT_SINGLELINE, 0xffff0000);
    }
}