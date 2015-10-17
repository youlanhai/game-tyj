//CParticleGun.cpp 
#include "Commen.h"
#include "ParticleGun.h"

CParticleGun::CParticleGun()
	:m_vPosition(0,0,0),
	m_vDirection(0,0,1)
{
	this->m_Size = 20.5f;
	this->m_vbSize = 2048;
	this->m_vboffset = 0;  
	this->m_vbBatchSize = 512;
}

CParticleGun::~CParticleGun(void)
{

}

void CParticleGun::ResetParticle(Attribute *attribute)
{
	attribute->live = true;
	attribute->position = m_vPosition;
	attribute->position += m_vDirection*10.0f;
	attribute->position.y -= 10.0f;
	attribute->speed = m_vDirection * 4000.0f;
	attribute->color = D3DCOLOR_XRGB(255,0,255);
}

void CParticleGun::Update(float fElapse)
{
    CRect bounds(-5000,-5000,5000,5000);
   
    if (m_pTerrain != NULL)
    {
        bounds = m_pTerrain->getBounds();
    }
    CMList<Attribute>::LPListNode it; 
 	for(it = particles.begin(); it!=particles.end(); it = it->next)
	{
        Attribute *pAttr = &(it->data);
		pAttr->position += pAttr->speed * fElapse ;
		pAttr->age += 1.0f;
        if (pAttr->speed > m_vDirection*100.0f)
        {
            pAttr->speed -= m_vDirection*(10.0f*fElapse);
        }

        if (!bounds.isIn(pAttr->position.x,pAttr->position.z))
        {
            pAttr->live = false;
        }
        else if (pAttr->position.y>3000 || pAttr->position.y<-1000)
        {
            pAttr->live = false;
        }
		else if(m_pTerrain != NULL)
		{
			if(m_pTerrain->getPhysicalHeight(pAttr->position.x, 
                pAttr->position.z) > pAttr->position.y)
			{
				pAttr->live = false;
			}
		}
	}
	RemoveDeadParticle();
}

void CParticleGun::SetRenderState(IDirect3DDevice9 *Device)
{
	CParticle::SetRenderState(Device);
	Device->SetRenderState(D3DRS_POINTSIZE_MAX, FtoD(500.0f));
}

void CParticleGun::updateGunInfo(const D3DXVECTOR3 & direction,
    const D3DXVECTOR3 & position)
{
    m_vDirection = direction;
    m_vPosition = position;
}