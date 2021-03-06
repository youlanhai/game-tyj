﻿#pragma once

#include "camera.h"

/*  玩家（player）控制相机旋转规则。
    1.第一人称，相机完全有玩家控制。
    2.自由模式，相机完全“自己”控制。
    3.第3人称，根据需要，进行玩家和自己切换。
*/

/** 鼠标控制摄像机。*/
class LZDLL_API CCursorCamera :  public CCamera
{
public:
    CCursorCamera(bool forPy);

    CCursorCamera(IDirect3DDevice9* pDevice, CameraType type = THIRD);

    ~CCursorCamera(void);

    virtual UINT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    virtual void update(float elapse);

    /** 设置鼠标旋转速度。单位为：弧度/像素*/
    void setCurRoSpeed(float speed);
    float getRotateSpeed(){ return m_curSpeedX; }

    void showCursor(bool show);

    virtual void drag(CPoint pt);

    I3DObject* getSource(void){ return m_pSource; }

    bool isDraged(void){ return m_draged; }

    void setHeight(float h){ m_height = h; }

protected:
    bool    m_bMouseDown;
    CPoint  m_ptDown;
    float   m_curSpeedX;
    float   m_curSpeedY;
    bool    m_bCurShow;
    bool    m_draged;
    float   m_height;
    float   m_realDistToPlayer;

};

LZDLL_API CCursorCamera* getCamera(void);