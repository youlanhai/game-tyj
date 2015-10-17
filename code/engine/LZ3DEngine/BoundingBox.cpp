#include "Commen.h"
#include "BoundingBox.h"
#include "Utilities.h"

const float INFINITY = FLT_MAX;

ID3DXMesh * CBoundingBox::m_pMeshBox = NULL;

bool CBoundingBox::createMeshBox(IDirect3DDevice9 *pDevice)
{
    if (m_pMeshBox != NULL)
    {
        return true;
    }
    debugMessage(_T("创建包围盒立方体..."));
    if(FAILED(D3DXCreateBox(pDevice,1.0f,1.0f,1.0f,&m_pMeshBox,NULL)))
    {
        return false;
    }
    else 
    {
        return true;
    }
}

void CBoundingBox::destoryMeshBox(void)
{
    SafeRelease(m_pMeshBox);
    debugMessage(_T("包围盒立方体已销毁。"));
}

CBoundingBox::CBoundingBox(void)
    : m_vecMin(0.0f,0.0f,0.f),
      m_vecMax(0.0f,0.0f,0.0f)
{
    m_fScale = 1.0f;
}

CBoundingBox::~CBoundingBox(void)
{
}

///创建包围盒
bool CBoundingBox::create(ID3DXMesh *pMesh)
{
    m_vecMin.x = INFINITY;
    m_vecMin.y = INFINITY;
    m_vecMin.z = INFINITY;
    m_vecMax.x = -INFINITY;
    m_vecMax.y = -INFINITY;
    m_vecMax.z = -INFINITY;
    
    BYTE* v = 0;
    if(FAILED(pMesh->LockVertexBuffer(0, (void**)&v)))
    {
        return false;
    }
    HRESULT hr = D3DXComputeBoundingBox(
        (D3DXVECTOR3*)v,
        pMesh->GetNumVertices(),
        D3DXGetFVFVertexSize(pMesh->GetFVF()),
        &m_vecMin,
        &m_vecMax);
    pMesh->UnlockVertexBuffer();
    if( FAILED(hr) )
    {
        return false;
    }

    return true;
}

///渲染包围盒
void CBoundingBox::render(IDirect3DDevice9 *pDevice,D3DXMATRIX * pMat)
{
    if (m_pMeshBox == NULL)
    {
        return ;
    }
    D3DXMATRIX matWord,matScale,matTrans;
    D3DXMatrixScaling(&matScale,
        m_vecMax.x-m_vecMin.x,
        m_vecMax.y-m_vecMin.y,
        m_vecMax.z-m_vecMin.z);

    D3DXVECTOR3 pos = (m_vecMin+m_vecMax) * 0.5f;
    D3DXMatrixTranslation(&matTrans,pos.x,pos.y,pos.z);

    matWord = matScale * matTrans * (*pMat);
    pDevice->SetTransform(D3DTS_WORLD,&matWord);

    pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);

    pDevice->SetTexture(0,NULL);

    m_pMeshBox->DrawSubset(0);

    pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
}

///点是否在包围盒中
bool CBoundingBox::isPointIn(const D3DXVECTOR3 & pos)
{
    if (   pos.x > m_vecMin.x
        && pos.x < m_vecMax.x
        && pos.y > m_vecMin.y
        && pos.y < m_vecMax.y
        && pos.z > m_vecMin.z
        && pos.z < m_vecMax.z)
    {
        return true;
    }
    else
    {
        return false;
    }
}