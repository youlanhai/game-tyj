#include "Commen.h"
#include "Utilities.h"
#include "CursorCamera.h"
#include "App.h"
#include "TerrinMap.h"
#include "Collision.h"

CCursorCamera* g_pCamera_ = NULL;
LZDLL_API CCursorCamera* getCamera(void)
{
    return g_pCamera_;
}
//////////////////////////////////////////////////////////////////////////
CCursorCamera::CCursorCamera(bool forPy)
: CCamera(getApp()->getDevice())
{
    g_pCamera_ = this;
    m_bMouseDown = false;
    m_bCurShow = true;
    m_draged = false;
    m_height = 30.0f;

    m_realDistToPlayer = m_fDistToPlayer;

    float angleX = D3DX_PI/getApp()->getWidth()*0.5f;
    setCurRoSpeed(angleX);
}

CCursorCamera::CCursorCamera(IDirect3DDevice9* pDevice, CameraType type /*= THIRD*/)
    : CCamera(pDevice, type)
{
    g_pCamera_ = this;
    m_bMouseDown = false;
    m_bCurShow = true;
    m_draged = false;
    m_height = 60.0f;
    m_realDistToPlayer = m_fDistToPlayer;

    float angleX = D3DX_PI/getApp()->getWidth()*0.5f;
    setCurRoSpeed(angleX);
}


CCursorCamera::~CCursorCamera(void)
{
}


UINT CCursorCamera::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg==WM_LBUTTONDOWN || msg==WM_RBUTTONDOWN)
    {
        m_bMouseDown = true;
        m_ptDown.formatLParam(lParam);
        m_draged = false;
    }
    else if(msg==WM_LBUTTONUP || msg==WM_RBUTTONUP)
    {
        m_bMouseDown = false;
        if (!m_bCurShow)
        {
            showCursor(true);
            return 1;
        }
    }
    else if (msg == WM_MOUSEMOVE)
    {
        if(m_bMouseDown)
        {
            drag(CPoint(lParam));   
            return 1;
        }
    }
    else if (msg == WM_MOUSELEAVE)
    {
        m_bMouseDown = false;
    }
    else if (msg == WM_MOUSEWHEEL)
    {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (NULL == m_pSource)
        {
            if (zDelta < 0)
            {
                moveLook(false);
            }
            else if(zDelta > 0)
            {
                moveLook(true);
            }
        }
        else
        {
            float dt = (m_distMax - m_distMin)/40.0f;
            if (zDelta < 0)
            {
                m_fDistToPlayer += dt;
            }
            else if(zDelta > 0)
            {
                m_fDistToPlayer -= dt;
            }
            correctDist();
        }

        return 1;
    }
    return 0;
}

void CCursorCamera::drag(CPoint pt)
{
    if (pt == m_ptDown)
    {
        return;
    }
    CPoint dp = pt - m_ptDown;

    rotYaw( dp.x*m_curSpeedX );
    rotPitch( dp.y*m_curSpeedY);

#if 1
    showCursor(false);
    getApp()->setCursorPos(m_ptDown);
#else
    m_ptDown = pt;
#endif
    m_draged = true;
}


void CCursorCamera::update(float fElapse)
{
    __super::update(fElapse);

    if (m_realDistToPlayer > 2*m_distMax)
    {
        m_realDistToPlayer = 2*m_distMax;
    }
    
    if (NULL==m_pSource || getCameraType()==FREE)
    {
        RefPtr<CKeyboard> keyboard = getApp()->getKeyboard();
        //左右旋转
        if(keyboard->isKeyPress('A'))
        {
            //rotYaw(-3.14f*fElapse);
            moveRight(false);
        }
        else if(keyboard->isKeyPress('D'))
        {
            //rotYaw(3.14f*fElapse);
            moveRight(true);
        }
        //前进后退
        if(keyboard->isKeyPress('W'))
        {
            moveLook(true);
        }
        else if(keyboard->isKeyPress('S'))
        {
            moveLook(false);
        }
        return ;
    }

    //摄像机跟随
    D3DXVECTOR3 vecPos = m_pSource->getPos();
    vecPos.y += m_height;
    if (getCameraType() == CCamera::FIRST)
    {
        m_pSource->setLook(m_look);
        m_pSource->setUp(m_up);
        m_pSource->setRight(m_right);
        m_position = vecPos;
    }
    else if (getCameraType() == CCamera::THIRD)
    {
        //镜头距离变化缓冲模式
        float factor = 0.8f;
        float delta = m_realDistToPlayer - m_fDistToPlayer;
        float decay = delta*factor*fElapse;
        if (fabs(decay) > fabs(delta))
        {
            decay = delta;
        }

        if (fabs(decay) < 0.00001f)
        {
            decay = 0.0f;
        }

        m_realDistToPlayer -= decay;

        m_position = vecPos - m_look * m_realDistToPlayer;

        //反向射线拾取，避免有物体格挡在相机与玩家之间。
        Physics::Vector3 start = vecPos;
        Physics::Vector3 end = m_position;
        Physics::Vector3 dir = end - start;
        float distance = dir.length() + 10.0f;
        dir.normalize();

        Physics::RayCollision rc(start, dir, distance);
        if (Physics::pickRay(rc))
        {
            distance = min(distance-10.0f, rc.m_hitDistance);
            end = start + dir * distance;
            end.toD3DXVec3(m_position);

            m_realDistToPlayer = distance;
        }

    }

    if (true)
    {
        //贴地处理
        cMap* pMap = getMap();
        if (pMap)
        {
            float h = pMap->getHeight(m_position.x, m_position.z) + 10.0f;
            if ( m_position.y < h)
            {
                m_position.y = h;
            }
        }
    }
}


void CCursorCamera::setCurRoSpeed(float speed)
{
    m_curSpeedX = speed;
    m_curSpeedY = speed;

}

void CCursorCamera::showCursor(bool show)
{
    if (show == m_bCurShow)
    {
        return;
    }
    m_bCurShow = show;
    ShowCursor(show);
}
