﻿#pragma once

#include "I3DObject.h"
#include "particle.h"

/**雪花类*/
class LZDLL_API cSnow : public cParticleSystem
{
public:
    /**构造函数
    * @param [in] numParticles : 粒子数量
    */
    cSnow(int maxAmount, LPCSTR texture, I3DObject* source);

    ///析构函数
    ~cSnow(void);

    virtual void render(IDirect3DDevice9 *pDevice);

    virtual void update(float fElapse);

    void setSource(I3DObject* source){ m_pSource = source; }

    void setRange(D3DXVECTOR3 min_, D3DXVECTOR3 max_);

    void show(bool s){ m_visible = s; }

    bool visible(){ return m_visible; }

protected:

    virtual void updateParticle(Attribute *attribute, float elapse) ;

    ///重置一个粒子的属性
    virtual void resetParticle(Attribute *attribute) ;

    ///设置渲染状态
    virtual void setRenderState(IDirect3DDevice9 *pDevice);

private:	
    I3DObject*  m_pSource;
    D3DXVECTOR3 m_rangeMin;
    D3DXVECTOR3 m_rangeMax;
    bool        m_visible;
};

