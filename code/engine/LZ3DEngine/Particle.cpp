//Particle.cpp 
#include "Commen.h"
#include "Utilities.h"
#include "Particle.h"

#include "App.h"
#include "RenderState.h"

DWORD ParticleVertex::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE;
DWORD ParticleVertex::SIZE = sizeof(ParticleVertex);

cParticleSystem::cParticleSystem(int maxAmount, LPCSTR texture)
{
    m_pVertexBuffer = NULL;	//顶点缓冲区指针
    m_Size = 1.0f;				//粒子大小
    m_maxAmount = maxAmount;
    m_amountToRender = 0;
    m_textureName = texture;

    if(FAILED(getApp()->getDevice()->CreateVertexBuffer(
        m_maxAmount * ParticleVertex::SIZE, 
        D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY, 
        ParticleVertex::FVF,
        D3DPOOL_MANAGED, 
        &m_pVertexBuffer, 
        NULL)))
    {
        m_pVertexBuffer = NULL;
        debugMessage(_T("error 创建粒子系统顶点缓冲区失败！"));
        return;
    }

    for(int i=0; i<m_maxAmount; ++i)
    {
        addParticle();
    }
    getApp()->addUpdateRender(this);
}

cParticleSystem::~cParticleSystem(void)
{
    getApp()->removeUpdateRender(this);
    SafeRelease(m_pVertexBuffer);
}

void cParticleSystem::setRenderState(IDirect3DDevice9 *pDevice)
{
    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

    pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
    pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
    pDevice->SetRenderState(D3DRS_POINTSIZE, floatToDWORD(m_Size));
    pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, floatToDWORD(0.0f));
    pDevice->SetRenderState(D3DRS_POINTSCALE_A, floatToDWORD(0.0f));
    pDevice->SetRenderState(D3DRS_POINTSCALE_B, floatToDWORD(0.0f));
    pDevice->SetRenderState(D3DRS_POINTSCALE_C, floatToDWORD(1.0f));

    pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
}


void cParticleSystem::recoverRenderState(IDirect3DDevice9 *pDevice)
{
    pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);

    pDevice->SetRenderState(D3DRS_LIGHTING, DefaultRS::LightingEnable);
    pDevice->SetRenderState(D3DRS_ZENABLE, DefaultRS::ZEnable);
}

void cParticleSystem::addParticle()
{
    Attribute attribute;
    resetParticle(&attribute);
    particles.push_front(attribute);
}

void cParticleSystem::reset()
{
    ListNode<Attribute> * pNode = particles.begin();
    for( ; pNode != NULL; pNode = pNode->next)
    {
        resetParticle(&(pNode->data));
    }
}

bool cParticleSystem::isDead()
{
    ListNode<Attribute> *pNode=particles.begin();
    for(; pNode != NULL; pNode = pNode->next)
    {
        if(pNode->data.live)//有活的
        {
            return false;
        }
    }
    return true;
}

void cParticleSystem::removeDeadParticle()
{
    ListNode<Attribute> *pPrev = particles.head();
    ListNode<Attribute> *pNode = particles.begin();
    while(pNode != NULL)
    {
        if(!(pNode->data.live))
        {
            pPrev->next = pNode->next;
            particles.deleteNode(pNode);
            pNode = pPrev->next;
        }
        else
        {
            pPrev = pNode;
            pNode = pNode->next;
        }
    }
}

void cParticleSystem::update(float fElapse)
{
    updateParticles(fElapse);
    removeDeadParticle();
}

void cParticleSystem::updateParticles(float elapse)
{
    m_amountToRender = 0;
    ParticleVertex *pVertex = NULL;
    if( FAILED(m_pVertexBuffer->Lock( 0, m_maxAmount*ParticleVertex::SIZE, (void**)&pVertex, 0)))
    {
        return;
    }

    DWORD numParticles = 0;

    Attribute *pAttr = NULL;
    ListNode<Attribute> *pNode = NULL;
    for( pNode = particles.begin();
        pNode != NULL; 
        pNode = pNode->next)
    {
        pAttr = &(pNode->data);
        updateParticle(pAttr, elapse);
        if(pAttr->live)
        {
            pVertex->color = pAttr->color;
            pVertex->x = pAttr->position.x;
            pVertex->y = pAttr->position.y;
            pVertex->z = pAttr->position.z;
            ++pVertex; //移到下一存贮单元
            ++m_amountToRender;
        }
    }
    m_pVertexBuffer->Unlock();
}

void cParticleSystem::render(IDirect3DDevice9 *pDevice)
{
    if(m_pVertexBuffer == NULL || particles.empty())
    {
        return ;
    }

    D3DXMATRIX temp;
    D3DXMatrixIdentity(&temp);
    pDevice->SetTransform(D3DTS_WORLD, &temp);

    setRenderState(pDevice);
    pDevice->SetTexture(0, getResMgr()->getTextureEx(m_textureName));

    pDevice->SetFVF(ParticleVertex::FVF);
    pDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(ParticleVertex));

    if(m_amountToRender > 0)
    {
        pDevice->DrawPrimitive(D3DPT_POINTLIST, 0, m_amountToRender);
    }

    recoverRenderState(pDevice);
}

float cParticleSystem::getRandomFloat(float lowBound, float highBound)
{
    if( lowBound > highBound )
    {
        float t = lowBound ;
        lowBound = highBound;
        highBound = t;
    }
    float f = (rand() % 10000) * 0.0001f; 

    return (f * (highBound - lowBound)) + lowBound; 
}

D3DXVECTOR3 cParticleSystem::getRandomVector(D3DXVECTOR3* min,D3DXVECTOR3* max)
{
    return D3DXVECTOR3(getRandomFloat(min->x, max->x), 
        getRandomFloat(min->y, max->y),
        getRandomFloat(min->z, max->z));
}
////////////////////////////////////////////
