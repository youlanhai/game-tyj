//Mesh.cpp
#include "Commen.h"
#include "Utilities.h"
#include "Mesh.h"
#include <fstream>
#include "Res.h"


///构造函数
cResMesh::cResMesh()
{
    m_pMesh = NULL;
    m_bbCenter = D3DXVECTOR3(0.0f,0.0f,0.0f);
    m_bbRadius = 10.0f;
    //DebugMsg("cResMesh construct 0x%8.8x", DWORD(this));
    MEMORY_CHECK_CONS;
}

cResMesh::~cResMesh()
{
    SafeRelease(m_pMesh);
    MEMORY_CHECK_DEST;

    //DebugMsg("cResMesh destruct 0x%8.8x", DWORD(this));
}

/** 创建自己。*/
IRenderRes* cResMesh::createObj(void)
{
    return new cResMesh();
}

/** 加载资源。*/
bool cResMesh::load(std::string filename)
{
    IDirect3DDevice9 *pDevice = getResMgr()->getDevice();
    //从x文件中加载mesh
    ID3DXBuffer * pBuffer=NULL;
    DWORD numbers = 0;//材质数
    
    HRESULT hr = D3DXLoadMeshFromX(
        Lazy::charToWChar(filename).c_str(),
        D3DXMESH_MANAGED,
        pDevice,
        NULL,
        &pBuffer,
        NULL,
        &numbers,
        &m_pMesh);

    if(FAILED(hr))
    {
        XWRITE_LOGA("load mesh '%s' failed! hr=0x%x, code=0x%x", filename.c_str(), hr, GetLastError());
        return false;
    }

    //获取材质，创建纹理
    D3DXMATERIAL* pMat=(D3DXMATERIAL*)pBuffer->GetBufferPointer();
    m_vecMaterials.resize(numbers);
    m_vecTextures.resize(numbers);
    for(DWORD i=0;i<numbers;++i)
    {
        m_vecMaterials[i]=pMat[i].MatD3D;
        m_vecMaterials[i].Ambient=m_vecMaterials[i].Diffuse;
        m_vecTextures[i]=NULL;
        if(pMat[i].pTextureFilename != NULL)
        {
            std::string fileName;
            if(!LZUtility::searchFile(fileName, pMat[i].pTextureFilename))
            { 
                XWRITE_LOGA("ERROR 缺少纹理%s", pMat[i].pTextureFilename);
            }
            else
            {
                //DebugMsg("加载纹理：%s,old is:%s", fileName.c_str(), destMaterial->pTextureFilename);
                m_vecTextures[i] = getResMgr()->getTextureEx(fileName);
            }
        }
    }
    
//     m_pMesh->OptimizeInplace( D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE, 
//         NULL, NULL, NULL, NULL );
    
    DWORD FVF = m_pMesh->GetFVF();
    if( (FVF & D3DFVF_NORMAL) == 0 )
    {
        D3DXComputeNormals( m_pMesh, NULL );
    }
    pBuffer->Release();

    LPVOID pVertex = NULL;
    if(SUCCEEDED(m_pMesh->LockVertexBuffer(0, &pVertex)))
    {
        D3DXComputeBoundingSphere(
            (D3DXVECTOR3*)pVertex, 
            m_pMesh->GetNumVertices(),
            m_pMesh->GetNumBytesPerVertex(),
            &m_bbCenter, 
            &m_bbRadius);


        D3DXComputeBoundingBox(
            (D3DXVECTOR3*)pVertex, 
            m_pMesh->GetNumVertices(),
            m_pMesh->GetNumBytesPerVertex(),
            &m_aabb.min,
            &m_aabb.max
            );

        m_pMesh->UnlockVertexBuffer();
    }

    return true;
}


void cResMesh::update(float fElapse)
{

}

void cResMesh::render(IDirect3DDevice9 * pDevice)
{
    if (m_pMesh == NULL) return ;

    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR );
    pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR );
    pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

    pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
    pDevice->SetRenderState( D3DRS_ALPHAREF, 10);
    pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

    pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1); 
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

    pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE); 
    pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE); 
    pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE); 

    for(DWORD i=0;i<m_vecMaterials.size();++i)
    {
        pDevice->SetMaterial(&m_vecMaterials[i]);
        pDevice->SetTexture(0,m_vecTextures[i]);
        m_pMesh->DrawSubset(i);//绘制
    }

    pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}


//////////////////////////////////////////////////////////////////////////
///MeshWraper
//////////////////////////////////////////////////////////////////////////
MeshWrap::MeshWrap(ID3DXMesh *mesh)
: m_mesh(mesh)
, m_vBuffer(NULL)
, m_iBuffer(NULL)
, m_step(0)
{
    assert(mesh != NULL);

    m_mesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&m_vBuffer);
    m_mesh->LockIndexBuffer(D3DLOCK_READONLY, (void**)&m_iBuffer);
    m_step = m_mesh->GetNumBytesPerVertex();
    m_isIdx32 = m_mesh->GetOptions() & D3DXMESH_32BIT;
    m_numFaces = m_mesh->GetNumFaces();
    m_numVertex = m_mesh->GetNumVertices();
}

MeshWrap::~MeshWrap()
{
    m_mesh->UnlockIndexBuffer();
    m_mesh->UnlockVertexBuffer();
}

DWORD MeshWrap::getIndex(DWORD i) const
{
    assert(i>=0 && i<m_numFaces*3);

    if (m_isIdx32)
    {
        DWORD* pi = (DWORD*)m_iBuffer;
        return pi[i];
    }
    else
    {
        WORD* pi = (WORD*)m_iBuffer;
        return pi[i];
    }
}

void MeshWrap::getPosition(DWORD index, D3DXVECTOR3 & p)
{
    assert(index>=0 && index<m_numVertex && "mesh index out of range");

    BYTE* pp = m_vBuffer + m_step*index;
    p = *((D3DXVECTOR3*)(pp));
}

void MeshWrap::getTriangle(DWORD iFace, D3DXVECTOR3 &p0, D3DXVECTOR3 &p1, D3DXVECTOR3 &p2)
{
    DWORD i = iFace * 3;
    DWORD i0 = getIndex(i++);
    DWORD i1 = getIndex(i++);
    DWORD i2 = getIndex(i);
    getPosition(i0, p0);
    getPosition(i1, p1);
    getPosition(i2, p2);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

LPD3DXMESH generateSimpleMesh(LPD3DXMESH srcMesh, float factor)
{
    if (!srcMesh)
    {
        return NULL;
    }

    HRESULT hr;
    DWORD face = srcMesh->GetNumFaces();
    DWORD destFaces = DWORD(factor * face);

    XWRITE_LOGA("srcMesh faces are: %d. destMeshFace are: %d", face, destFaces);

    std::vector<DWORD> buffer(3*face);
    if(FAILED(hr = srcMesh->GenerateAdjacency(1, &buffer[0])))
    {
        XWRITE_LOGA("GenerateAdjacency faild: %d, %d", hr, HRESULT_CODE(hr));
        return NULL;
    }

    LPD3DXMESH tempMesh = NULL;
    std::vector<DWORD> buffer2(3*face);
    if(FAILED(hr = D3DXCleanMesh(
        D3DXCLEAN_SIMPLIFICATION, 
        srcMesh, 
        &buffer[0], 
        &tempMesh,
        &buffer2[0], 
        NULL)))
    {
        XWRITE_LOGA("D3DXCleanMesh faild: %d, %d", hr, HRESULT_CODE(hr));
        return NULL;
    }

    LPD3DXMESH destMesh = NULL;
    if (FAILED(hr = D3DXSimplifyMesh(tempMesh,
        &buffer2[0],
        NULL,
        NULL,
        destFaces,
        D3DXMESHSIMP_FACE,
        &destMesh)))
    {
        XWRITE_LOGA("D3DXSimplifyMesh faild: %d, %d", hr, HRESULT_CODE(hr));
    }
    SafeRelease(tempMesh);

    return destMesh;
}