/**********************************************************
ParticleGun.h（粒子枪类 继承于粒子基类Particle.h）
作用: 创建粒子子弹，动态更新,销毁粒子子弹
**********************************************************/

#pragma once
#include "particle.h"
#include "Terrain.h"
#include "Base.h"

class LZDLL_API CParticleGun : public CParticle
{
public:
	CParticleGun();
	~CParticleGun(void);
	void ResetParticle(Attribute *attribute);
	void Update(float fElapse);
	void SetRenderState(IDirect3DDevice9 *Device);

    void updateGunInfo(const D3DXVECTOR3 & direction,
        const D3DXVECTOR3 & position);
    void setTerrain(const RefPtr<CTerrain> & pTerrain) { m_pTerrain = pTerrain; }
private:
	RefPtr<CTerrain>    m_pTerrain;
	D3DXVECTOR3     m_vDirection;
	D3DXVECTOR3     m_vPosition;
};
