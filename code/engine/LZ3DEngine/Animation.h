#pragma once

#include "Model.h"

class cSkinMesh;

///动画实例
class LZDLL_API cAnimalModel : public iModel
{
public:
    /** 构造函数。*/
    cAnimalModel(void);

    /** 析构函数。*/
    ~cAnimalModel(void);
    
    /** 是否有骨骼*/
    virtual bool isSkeleton(void) const { return true; }

    virtual bool load(const std::string & name);

    /** 更新*/
    virtual void update(float elapse);

    /** 渲染*/
    virtual void render(IDirect3DDevice9* pDevice);

    ///获得包围盒半径
    virtual float getBBRadius(void) const;

    ///获得包围盒球心
    virtual D3DXVECTOR3 getBBCenter(void) const;

    /** 播放动画*/
    virtual bool playAction(std::string name, bool loop);

    /** 停止动画*/
    virtual void stopAction(void);

    /** 获得动画个数*/
    virtual int getActionCount(void) const;

    /** 获得动画名称*/
    virtual std::string getActionName(int index) const;

    /** 步进动画*/
    virtual void advanceAnimation(void);

    virtual void setAnimSpeed(float s){ m_speed = s; }

    bool playAction(LPD3DXANIMATIONSET pAnimSet);

    LPD3DXANIMATIONSET getAnimSet(int n) const;

    LPD3DXANIMATIONSET getAnimSet(std::string name) const;

    LPD3DXANIMATIONSET findAnimSet(std::string name) const;

    /** 获得动画控制器。*/
    LPD3DXANIMATIONCONTROLLER getAnimalControler(void){ return m_pAnimControler; }

protected:
    LPD3DXANIMATIONCONTROLLER m_pAnimControler;
    cSkinMesh*      m_pSkinMesh;
    float           m_elapse;
    float           m_elapsedTime;
    bool            m_loop;
    float           m_speed;
};