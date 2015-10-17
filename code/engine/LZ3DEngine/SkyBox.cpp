//cSkyBox.cpp
#include "Commen.h"
#include "Utilities.h"

#include "Res.h"
#include "SkyBox.h"

struct SkyVertex
{
    SkyVertex(float x, float y, float z, float u, float v)
    {
        this->x = x; this->y = y; this->z = z;
        this->u = u; this->v = v;
    }
    float x,y,z;
    float u,v;
    static  DWORD FVF;
    static  UINT SIZE;
};
DWORD SkyVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;
UINT SkyVertex::SIZE = sizeof(SkyVertex);


//////////////////////////////////////////////////////////////////////////

cSkyBox::cSkyBox()
{
    m_pVertexBuffer = NULL;
    m_pSource = NULL;
    MEMORY_CHECK_CONS;
    m_visible = true;
}


cSkyBox::cSkyBox(IDirect3DDevice9 *pDevice)
{
    m_pVertexBuffer = NULL;
    m_pSource = NULL;
    create(pDevice);
    MEMORY_CHECK_CONS;
    m_visible = true;
}

cSkyBox::~cSkyBox(void)
{
    SafeRelease(m_pVertexBuffer);
    MEMORY_CHECK_DEST;
}

///创建顶点
bool cSkyBox::create(IDirect3DDevice9 *pDevice)
{
    if(FAILED(pDevice->CreateVertexBuffer(
        24* SkyVertex::SIZE, 
        0, 
        SkyVertex::FVF, 
        D3DPOOL_MANAGED,
        &m_pVertexBuffer, 
        0)))
    {
        return false;
    }
    setSkyRange(D3DXVECTOR3(-50000, -20000, -50000), 
        D3DXVECTOR3(50000,30000,50000));
    return true;
}

/** 设置天空数据。
    @param : texNames，纹理名称。格式为name%d.xxx。图片顺序为:前后左右顶底。*/
void cSkyBox::setSkyImage(LPCSTR texNames)
{
    m_texNames = texNames;
}

void cSkyBox::setSkyRange( D3DXVECTOR3 min, D3DXVECTOR3 max)
{
    SkyVertex *skyvertex = NULL;
    if(FAILED(m_pVertexBuffer->Lock(0, 0, (void**)&skyvertex, 0)))
    {
        return;
    }
    //北面
    skyvertex[0] = SkyVertex(min.x, max.y, max.z, 0.0f, 0.0f);
    skyvertex[1] = SkyVertex(max.x, max.y, max.z, 1.0f, 0.0f);
    skyvertex[2] = SkyVertex(min.x, min.y, max.z, 0.0f, 1.0f);
    skyvertex[3] = SkyVertex(max.x, min.y, max.z, 1.0f, 1.0f);
    //东面
    skyvertex[4] = SkyVertex(max.x, max.y, max.z, 0.0f, 0.0f);
    skyvertex[5] = SkyVertex(max.x, max.y, min.z, 1.0f, 0.0f);
    skyvertex[6] = SkyVertex(max.x, min.y, max.z, 0.0f, 1.0f);
    skyvertex[7] = SkyVertex(max.x, min.y, min.z, 1.0f, 1.0f);
    //西面
    skyvertex[8]  = SkyVertex(max.x, max.y, min.z, 0.0f, 0.0f);
    skyvertex[9]  = SkyVertex(min.x, max.y, min.z, 1.0f, 0.0f);
    skyvertex[10] = SkyVertex(max.x, min.y, min.z, 0.0f, 1.0f);
    skyvertex[11] = SkyVertex(min.x, min.y, min.z, 1.0f, 1.0f);
    //南面
    skyvertex[12] = SkyVertex(min.x, max.y, min.z, 0.0f, 0.0f);
    skyvertex[13] = SkyVertex(min.x, max.y, max.z, 1.0f, 0.0f);
    skyvertex[14] = SkyVertex(min.x, min.y, min.z, 0.0f, 1.0f);
    skyvertex[15] = SkyVertex(min.x, min.y, max.z, 1.0f, 1.0f);
    //顶面
    skyvertex[16] = SkyVertex(min.x, max.y, min.z, 0.0f, 0.0f);
    skyvertex[17] = SkyVertex(max.x, max.y, min.z, 1.0f, 0.0f);
    skyvertex[18] = SkyVertex(min.x, max.y, max.z, 0.0f, 1.0f);
    skyvertex[19] = SkyVertex(max.x, max.y, max.z, 1.0f, 1.0f);
    //底面
    skyvertex[20] = SkyVertex(min.x, min.y, max.z, 0.0f, 0.0f);
    skyvertex[21] = SkyVertex(max.x, min.y, max.z, 1.0f, 0.0f);
    skyvertex[22] = SkyVertex(min.x, min.y, min.z, 0.0f, 1.0f);
    skyvertex[23] = SkyVertex(max.x, min.y, min.z, 1.0f, 1.0f);
    m_pVertexBuffer->Unlock();
}

void cSkyBox::render(IDirect3DDevice9 *pDevice)
{
    if (!m_visible || NULL==pDevice || NULL==m_pVertexBuffer)
    {
        return ;
    }
    D3DXMATRIX world;
    if (m_pSource)
    {
        D3DXVECTOR3 pos = m_pSource->getPos();
        D3DXMatrixTranslation(&world, pos.x, 0.0f, pos.z);
    }
    else
    {
        D3DXMatrixIdentity(&world);
    }
    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    pDevice->SetTransform(D3DTS_WORLD, &world);
    pDevice->SetStreamSource(0, m_pVertexBuffer, 0, SkyVertex::SIZE);
    pDevice->SetFVF(SkyVertex::FVF);
    char name[256];
    for(int i = 0; i<6; i++)
    {
        sprintf_s(name, 256, m_texNames.c_str(), i);
        pDevice->SetTexture(0, getResMgr()->getTextureEx(name));
        pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, i*4, 2);
    }
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    
}