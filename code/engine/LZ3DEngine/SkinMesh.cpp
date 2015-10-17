//Memory_SkinMesh.cpp 
#include <fstream>
#include "Commen.h"
#include "Utilities.h"
#include "SkinMesh.h"
#include "Res.h"
#include "Log.h"

struct D3DXFRAME_DERIVED: public D3DXFRAME
{
    D3DXMATRIX  CombinedTransformationMatrix;
};

struct D3DXMESHCONTAINER_DERIVED: public D3DXMESHCONTAINER
{
    LPD3DXATTRIBUTERANGE	pAttributeTable;
    LPDIRECT3DTEXTURE9		*ppTextures;		//纹理数组
    LPD3DXMESH				pOrigMesh;		//原始网格模型
    DWORD					NumInfl;		//每个顶点最多受多少骨骼的影响
    DWORD					NumAttributeGroups;		//属性组数量,即子网格的数量
    LPD3DXBUFFER			pBoneCombinationBuf;	//骨骼组合表
    D3DXMATRIX**			ppBoneMatrixPtrs;		//存放骨骼的组合变换矩阵
    D3DXMATRIX*				pBoneOffsetMatrices;	//存放骨骼的初始变换矩阵
    DWORD					NumPaletteEntries;		//骨骼数量上限
    bool					UseSoftwareVP;			//是否使用软件顶点处理
    DWORD					iAttributeSW;     
};

//////////////////////////////////////////////////////////////////////////


class CAllocateHierarchy: public ID3DXAllocateHierarchy
{
public:
    STDMETHOD( CreateFrame )( THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame );
    STDMETHOD(CreateMeshContainer)( 
        THIS_ LPCSTR              Name, 
        CONST D3DXMESHDATA*       pMeshData,
        CONST D3DXMATERIAL*       pMaterials, 
        CONST D3DXEFFECTINSTANCE* pEffectInstances, 
        DWORD                     NumMaterials, 
        CONST DWORD *             pAdjacency, 
        LPD3DXSKININFO			pSkinInfo, 
        LPD3DXMESHCONTAINER		*ppNewMeshContainer);

    STDMETHOD( DestroyFrame )( THIS_ LPD3DXFRAME pFrameToFree);
    STDMETHOD( DestroyMeshContainer )( THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);
    CAllocateHierarchy(cSkinMesh *pSkinMesh) :m_pSkinMesh(pSkinMesh) {}

public:
    cSkinMesh*		m_pSkinMesh;
};

//////////////////////////////////////////////////////////////////////////
HRESULT AllocateName( LPCSTR Name, LPSTR *pNewName )
{
	UINT cbLength;

	if( Name != NULL )
	{
		cbLength = (UINT)strlen(Name) + 1;
		*pNewName = new CHAR[cbLength];
		if (*pNewName == NULL)
			return E_OUTOFMEMORY;
		memcpy( *pNewName, Name, cbLength*sizeof(CHAR) );
	}
	else
	{
		*pNewName = NULL;
	}

	return S_OK;
}
///////////////////////////////////////////////
//			CAllocateHierarchy				///
///////////////////////////////////////////////
HRESULT CAllocateHierarchy::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{
    *ppNewFrame = NULL;
    HRESULT hr = S_OK;
    D3DXFRAME_DERIVED *pFrame = new D3DXFRAME_DERIVED;
	memset(pFrame, 0, sizeof(D3DXFRAME_DERIVED));
	
	hr = AllocateName(Name, (LPSTR*)&pFrame->Name);
	if( FAILED(hr) )
	{
		delete pFrame;
		return hr;
	}
    D3DXMatrixIdentity(&pFrame->TransformationMatrix);
    D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);
    *ppNewFrame = pFrame;
    return hr;
}

LRESULT CAllocateHierarchy::CreateMeshContainer( 
	THIS_ LPCSTR              Name, 
	CONST D3DXMESHDATA*       pMeshData,
	CONST D3DXMATERIAL*       pMaterials, 
	CONST D3DXEFFECTINSTANCE* pEffectInstances, 
	DWORD                     NumMaterials, 
	CONST DWORD *             pAdjacency, 
	LPD3DXSKININFO			pSkinInfo, 
	LPD3DXMESHCONTAINER		*ppNewMeshContainer) 

{
	HRESULT hr = S_OK;
	*ppNewMeshContainer = NULL;

	if( pMeshData->Type != D3DXMESHTYPE_MESH)
	{
		return E_FAIL;
	}

	LPD3DXMESH pMesh = pMeshData->pMesh;
	if( pMesh->GetFVF() == 0)
	{
		return E_FAIL;
	}

	//为网格容器分配内存
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = new D3DXMESHCONTAINER_DERIVED();
	ZeroMemory(pMeshContainer, sizeof(D3DXMESHCONTAINER_DERIVED));

	AllocateName(Name, &pMeshContainer->Name);
	
	//复制mesh数据
	if( !(pMesh->GetFVF() & D3DFVF_NORMAL))//没有法线
	{
		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

        LPDIRECT3DDEVICE9 pd3dDevice = NULL;
        pMesh->GetDevice(&pd3dDevice);
		hr = pMesh->CloneMeshFVF( pMesh->GetOptions(), pMesh->GetFVF() | D3DFVF_NORMAL, pd3dDevice, &pMeshContainer->MeshData.pMesh );
        SafeRelease( pd3dDevice );
        if( FAILED(hr))
		{
			DestroyMeshContainer( pMeshContainer );
			return hr;
		}
		pMesh = pMeshContainer->MeshData.pMesh;
		D3DXComputeNormals( pMesh, NULL );
	}
	else
    {
        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
		pMeshContainer->MeshData.pMesh = pMesh;
		pMeshContainer->MeshData.pMesh->AddRef();
    }

    DWORD NumFaces = pMesh->GetNumFaces();
	pMeshContainer->NumMaterials = max(1, NumMaterials);

	pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
	pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
	pMeshContainer->pAdjacency = new DWORD[NumFaces*3];
	
    //复制邻接信息
	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces * 3);

	//复制材质，创建纹理。
	if( NumMaterials > 0)            
	{
		memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials);
    }
    else
    {
        ZeroMemory(&pMeshContainer->pMaterials[0].MatD3D, sizeof(D3DXMATERIAL));
        D3DCOLORVALUE color = {0.5f, 0.5f, 0.5f, 1.0f};
        pMeshContainer->pMaterials[0].MatD3D.Diffuse = color;
    }
	for( DWORD i = 0; i < pMeshContainer->NumMaterials; ++i)
    {
        pMeshContainer->ppTextures[i] = NULL;
        //复制材质
        LPD3DXMATERIAL destMaterial = pMeshContainer->pMaterials + i; 
        destMaterial->MatD3D.Ambient = destMaterial->MatD3D.Diffuse;
        destMaterial->MatD3D.Specular = destMaterial->MatD3D.Diffuse;
        destMaterial->MatD3D.Power = 1.0f;
// 		    destMaterial->MatD3D.Ambient.r *= 0.6f;
// 		    destMaterial->MatD3D.Ambient.g *= 0.6f;
// 		    destMaterial->MatD3D.Ambient.b *= 0.6f;
// 		    destMaterial->MatD3D.Specular.r *= 0.6f;
// 		    destMaterial->MatD3D.Specular.g *= 0.6f;
// 		    destMaterial->MatD3D.Specular.b *= 0.6f;

        //提取纹理
		if( destMaterial->pTextureFilename != NULL)
		{
            std::string fileName;
            if(!LZUtility::searchFile(fileName, destMaterial->pTextureFilename))
            { 
                XWRITE_LOGA("ERROR: texture '%s' not found!", destMaterial->pTextureFilename);
            }
            else
            {
                //DebugMsg("加载纹理：%s,old is:%s", fileName.c_str(), destMaterial->pTextureFilename);
                pMeshContainer->ppTextures[i] = getResMgr()->getTextureEx(fileName);
            }
		}
	}
	
	if( pSkinInfo != NULL)
	{
        //复制蒙皮信息
		pMeshContainer->pSkinInfo = pSkinInfo;
		pMeshContainer->pSkinInfo->AddRef();

        //复制网格信息
		pMeshContainer->pOrigMesh = pMesh;
		pMeshContainer->pOrigMesh->AddRef();

        //复制骨骼信息
		DWORD nBones = pSkinInfo->GetNumBones();
		pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[nBones];
		for( DWORD i = 0; i < nBones; ++i)
		{
			pMeshContainer->pBoneOffsetMatrices[i] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i));
		}

		//生成蒙皮网格模型
		hr = m_pSkinMesh->GenerateSkinnedMesh(pMeshContainer);

		if( FAILED(hr))
		{
			DestroyMeshContainer( pMeshContainer );
			return hr;
		}
	}
	*ppNewMeshContainer = pMeshContainer;
	pMeshContainer = NULL;
	return hr;
}

HRESULT CAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree) 
{
	if( pFrameToFree != NULL )
	{
		SafeDeleteArray( pFrameToFree->Name );
		SafeDelete( pFrameToFree );
	}
    return S_OK; 
}

HRESULT CAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
	if( pMeshContainerBase == NULL )
	{
		return S_OK;
	}
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
    SafeDeleteArray( pMeshContainer->Name );
    SafeDeleteArray( pMeshContainer->pAdjacency );
    SafeDeleteArray( pMeshContainer->pMaterials );
    SafeDeleteArray( pMeshContainer->pBoneOffsetMatrices );
 
    SafeDeleteArray( pMeshContainer->ppTextures );//纹理不需要释放，有纹理管理器管理。
    SafeDeleteArray( pMeshContainer->ppBoneMatrixPtrs );
    SafeRelease( pMeshContainer->pBoneCombinationBuf );
    SafeRelease( pMeshContainer->MeshData.pMesh );
    SafeRelease( pMeshContainer->pSkinInfo );
    SafeRelease( pMeshContainer->pOrigMesh );
    SafeDelete( pMeshContainer );
    return S_OK;
}



//////////////////////////////////////////////////////////////////////
// cSkinMesh
//////////////////////////////////////////////////////////////////////
cSkinMesh::cSkinMesh(LPDIRECT3DDEVICE9 pD3DDevice)
{
    MEMORY_CHECK_CONS;

	m_pd3dDevice = pD3DDevice;
	m_pd3dDevice->GetDeviceCaps( &m_d3dCaps );
	m_pAnimController = NULL;
	m_pFrameRoot = NULL;
	m_SkinningMethod = D3DNONINDEXED;
	m_pBoneMatrices = NULL;
	m_NumBoneMatricesMax = 0;

	m_dwTrangleCnt = 0;
    D3DXMatrixIdentity(&m_worldMatrix);
    m_bbCenter = D3DXVECTOR3(0.0f,0.0f,0.0f);
    m_bbRadius = 10.0f;
}

cSkinMesh::~cSkinMesh()
{
	CAllocateHierarchy Alloc(this);
	D3DXFrameDestroy(m_pFrameRoot, &Alloc);
    SafeRelease(m_pAnimController);
	SafeDeleteArray( m_pBoneMatrices );

    MEMORY_CHECK_DEST;
}

/** 创建自己。*/
IRenderRes* cSkinMesh::createObj(void)
{
    return new cSkinMesh(getResMgr()->getDevice());
}

/** 加载资源。*/
bool cSkinMesh::load(std::string filename)
{
    return loadFromXFile(filename);
}


/** 更新*/
void cSkinMesh::update(float fElapse)
{

}


bool cSkinMesh::loadFromXFile(std::string fileName)
{
    CAllocateHierarchy Alloc(this);
    HRESULT hr = D3DXLoadMeshHierarchyFromX(
        Lazy::charToWChar(fileName).c_str(), 
        D3DXMESH_MANAGED, 
        m_pd3dDevice, 
        &Alloc, 
        NULL, 
        &m_pFrameRoot, 
        &m_pAnimController);

    if(FAILED(hr))
    {
        XWRITE_LOGA("ERROR: load SkinMesh '%s' failed! hr=0x%x, code=0x%x",
            fileName.c_str(), hr, GetLastError());

        return false;
    }

    if(FAILED(SetupBoneMatrixPointers(m_pFrameRoot)))
    {
        return false;
    }

    if(m_pFrameRoot)	
    {
        D3DXMATRIX matIdentity;
        D3DXMatrixIdentity(&matIdentity); 
        UpdateFrameMatrices(m_pFrameRoot, &matIdentity);

        if(FAILED(D3DXFrameCalculateBoundingSphere( m_pFrameRoot, &m_bbCenter, &m_bbRadius )))
        {
            m_bbCenter = D3DXVECTOR3(0.0f,0.0f,0.0f);
            m_bbRadius = 10.0f;
        }

        Physics::Vector3 extend(m_bbRadius, m_bbRadius, m_bbRadius);
        m_aabb.min = m_bbCenter - extend;
        m_aabb.min = m_bbCenter + extend;
    }
    return true;
}

void cSkinMesh::render(IDirect3DDevice9* pDevice)
{
    m_dwTrangleCnt = 0;
    UpdateFrameMatrices(m_pFrameRoot, &m_worldMatrix);
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	DrawFrame(m_pFrameRoot);
}

/** 克隆动画控制器。*/
LPD3DXANIMATIONCONTROLLER cSkinMesh::cloneAnimaCtrl(void)
{
    /*  克隆原始AC。克隆对象用来操作这个mesh，原始AC除了用来克隆外不做其余用途。*/
    LPD3DXANIMATIONCONTROLLER pControl = NULL;
    if(FAILED(m_pAnimController->CloneAnimationController( 
        m_pAnimController->GetMaxNumAnimationOutputs(),
        m_pAnimController->GetMaxNumAnimationSets(),
        m_pAnimController->GetMaxNumTracks(),
        m_pAnimController->GetMaxNumEvents(),
        &pControl )))
    {
        return NULL;
    }
    return pControl;
}

//////底层实现

HRESULT cSkinMesh::GenerateSkinnedMesh(D3DXMESHCONTAINER *pMeshContainerBase)
{
    HRESULT hr = S_OK;
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
    if( pMeshContainer->pSkinInfo == NULL)
        return hr;

    SafeRelease( pMeshContainer->MeshData.pMesh );
    SafeRelease( pMeshContainer->pBoneCombinationBuf );

    //如果不使用索引的蒙皮动画，就把网格转换成可绘制的网格
    if( m_SkinningMethod == D3DNONINDEXED)
    {
        hr = pMeshContainer->pSkinInfo->ConvertToBlendedMesh( 
            pMeshContainer->pOrigMesh,
            D3DXMESH_MANAGED|D3DXMESHOPT_VERTEXCACHE, 
            pMeshContainer->pAdjacency, 
            NULL, NULL, NULL, 
            &pMeshContainer->NumInfl,
            &pMeshContainer->NumAttributeGroups, 
            &pMeshContainer->pBoneCombinationBuf, 
            &pMeshContainer->MeshData.pMesh );
        if( FAILED(hr))
        {
            return hr;
        }

        //如果设备只支持2个顶点混和，ConverteToBlendMesh不能近似地转换所有的网格。这时需要把网格转换成两部分，
        //只使用2个矩阵部分和其他部分。前者使用设备的顶点处理，其他部分使用软件方式处理
        LPD3DXBONECOMBINATION rgBoneCombinations  = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
        for( pMeshContainer->iAttributeSW = 0; pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups; pMeshContainer->iAttributeSW++ )
        {
            DWORD cInfl   = 0;
            for( DWORD iInfl = 0; iInfl < pMeshContainer->NumInfl; iInfl++ )
            {
                if( rgBoneCombinations[pMeshContainer->iAttributeSW].BoneId[iInfl] != UINT_MAX )
                {
                    ++cInfl;
                }
            }
            if( cInfl > m_d3dCaps.MaxVertexBlendMatrices )
            {
                break;
            }
        }

        if( pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups )
        {
            LPD3DXMESH pMeshTmp;
            hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF( D3DXMESH_SOFTWAREPROCESSING|pMeshContainer->MeshData.pMesh->GetOptions(), 
                pMeshContainer->MeshData.pMesh->GetFVF(),
                m_pd3dDevice, &pMeshTmp );
            if( FAILED(hr))
            {
                return hr;
            }

            pMeshContainer->MeshData.pMesh->Release();
            pMeshContainer->MeshData.pMesh = pMeshTmp;
            pMeshTmp = NULL;
        }
    }
    else if( m_SkinningMethod == D3DINDEXED)	//索引缓冲区模式的
    {
        DWORD NumMaxFaceInfl;
        DWORD Flags = D3DXMESHOPT_VERTEXCACHE;
        //获取网格模型的索引缓冲区
        LPDIRECT3DINDEXBUFFER9 pIB;
        hr = pMeshContainer->pOrigMesh->GetIndexBuffer(&pIB);
        if( FAILED(hr))
        {
            return hr;
        }
        //影响一个面的矩阵不会超过12个，自己画就晓得了
        hr = pMeshContainer->pSkinInfo->GetMaxFaceInfluences(pIB, pMeshContainer->pOrigMesh->GetNumFaces(), 
            &NumMaxFaceInfl);
        pIB->Release();
        if( FAILED(hr) )
        {
            return hr;
        }
        NumMaxFaceInfl = min( NumMaxFaceInfl, 12 );
        if( m_d3dCaps.MaxVertexBlendMatrixIndex + 1 < NumMaxFaceInfl )
        {	
            pMeshContainer->NumPaletteEntries = min(256, pMeshContainer->pSkinInfo->GetNumBones());
            pMeshContainer->UseSoftwareVP = true;
            Flags |= D3DXMESH_SYSTEMMEM;
        }
        else
        {
            pMeshContainer->NumPaletteEntries = min( ( m_d3dCaps.MaxVertexBlendMatrixIndex + 1 ) / 2, pMeshContainer->pSkinInfo->GetNumBones() );
            pMeshContainer->UseSoftwareVP = false;
            Flags |= D3DXMESH_MANAGED;
        }
        //生成蒙皮网格模型
        hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh( pMeshContainer->pOrigMesh,
            Flags, 
            pMeshContainer->NumPaletteEntries, 
            pMeshContainer->pAdjacency, 
            NULL, NULL, NULL, 
            &pMeshContainer->NumInfl,
            &pMeshContainer->NumAttributeGroups, 
            &pMeshContainer->pBoneCombinationBuf, 
            &pMeshContainer->MeshData.pMesh );
        if( FAILED(hr) )
        {
            return hr;
        }
    }
    else if( m_SkinningMethod == SOFTWARE)
    {
        hr = pMeshContainer->pOrigMesh->CloneMeshFVF(D3DXMESH_MANAGED, pMeshContainer->pOrigMesh->GetFVF(),
            m_pd3dDevice, &pMeshContainer->MeshData.pMesh);
        if( FAILED(hr))
        {
            return hr;
        }
        hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(NULL, &pMeshContainer->NumAttributeGroups);
        if( FAILED(hr))
        {
            return hr;
        }
        delete[] pMeshContainer->pAttributeTable;
        pMeshContainer->pAttributeTable  = new D3DXATTRIBUTERANGE[pMeshContainer->NumAttributeGroups];
        if( pMeshContainer->pAttributeTable == NULL)
        {
            hr = E_OUTOFMEMORY;
            return hr;
        }
        hr = pMeshContainer->MeshData.pMesh->GetAttributeTable(pMeshContainer->pAttributeTable, NULL);
        if( FAILED(hr))
        {
            return hr;
        }
        if( m_NumBoneMatricesMax < pMeshContainer->pSkinInfo->GetNumBones())
        {
            m_NumBoneMatricesMax = pMeshContainer->pSkinInfo->GetNumBones();

            delete []m_pBoneMatrices; 
            m_pBoneMatrices  = new D3DXMATRIX[m_NumBoneMatricesMax];
            if( m_pBoneMatrices == NULL)
            {
                hr = E_OUTOFMEMORY;
                return hr;
            }
        }
    }
    else
    {        
        hr = E_INVALIDARG;
        return hr;
    }
    return hr;
}

//------------------------------------------------------------------
//desc	调用SetupBoneMatrixPointersOnMesh递归为各框架安置组合变换矩阵
HRESULT cSkinMesh::SetupBoneMatrixPointers( LPD3DXFRAME pFrame )
{
    HRESULT hr;
    if( pFrame->pMeshContainer != NULL )
    {
        hr = SetupBoneMatrixPointersOnMesh( pFrame->pMeshContainer );
        if( FAILED(hr))
        {
            return hr;
        }
    }
    //如果存在兄弟骨骼
    if( pFrame->pFrameSibling != NULL )
    {
        hr = SetupBoneMatrixPointers( pFrame->pFrameSibling );
        if( FAILED(hr))
        {
            return hr;
        }
    }
    //如果存在子骨骼
    if( pFrame->pFrameFirstChild != NULL )
    {
        hr = SetupBoneMatrixPointers( pFrame->pFrameFirstChild );
        if( FAILED(hr))
        {
            return hr;
        }
    }
    return S_OK;
}

//-----------------------------------------------------------
//desc: 设置每个骨骼的组合变换矩阵
HRESULT cSkinMesh::SetupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainerBase)
{
    //先转换成新的类
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
    LPD3DXSKININFO pSkinInfo = pMeshContainer->pSkinInfo;
    //只有蒙皮网格模型才有骨骼矩阵
    if( NULL == pSkinInfo)
    {
        return S_OK;
    }
    //得到骨骼数量
    DWORD nBones = pSkinInfo->GetNumBones();
    pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[nBones];
    
    for(DWORD i = 0; i < nBones; ++i )
    {
        D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind( m_pFrameRoot, pSkinInfo->GetBoneName(i) );
        if( pFrame != NULL)
        {
            pMeshContainer->ppBoneMatrixPtrs[i] = &pFrame->CombinedTransformationMatrix;
        }
        else
        {
            pMeshContainer->ppBoneMatrixPtrs[i] = NULL;
        }
    }
    return S_OK;
}



//-----------------------------------------------------
// desc: 绘制框架: 还是和安置矩阵一样递归渲染各框架
void cSkinMesh::DrawFrame(LPD3DXFRAME pFrame)
{
    LPD3DXMESHCONTAINER pMeshContainer;

    pMeshContainer = pFrame->pMeshContainer;
    while( pMeshContainer != NULL )
    {
        DrawMeshContainer( pMeshContainer, pFrame );
        pMeshContainer = pMeshContainer->pNextMeshContainer;
    }
    if( pFrame->pFrameSibling != NULL )
    {
        DrawFrame( pFrame->pFrameSibling );
    }
    if( pFrame->pFrameFirstChild != NULL )
    {
        DrawFrame( pFrame->pFrameFirstChild );
    }
}

void cSkinMesh::DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase)
{
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
    D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
    UINT iMaterial;
    UINT NumBlend;
    UINT iAttrib;
    DWORD AttribIdPrev;
    LPD3DXBONECOMBINATION pBoneComb;

    UINT iMatrixIndex;
    UINT iPaletteEntry;
    D3DXMATRIX matTemp;

    //普通的mesh
    if( pMeshContainer->pSkinInfo == NULL)
    {
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &pFrame->CombinedTransformationMatrix );

        for( iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++ )
        {
            m_pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D );
            m_pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[iMaterial] );
            pMeshContainer->MeshData.pMesh->DrawSubset( iMaterial );
        }
        m_dwTrangleCnt += pMeshContainer->MeshData.pMesh->GetNumFaces();
        return ;
    }

    if( m_SkinningMethod == D3DNONINDEXED)
    {
        AttribIdPrev = UNUSED32; 
        pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
        for( iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++ )
        {
            NumBlend = 0;
            for( DWORD i = 0; i < pMeshContainer->NumInfl; ++i )
            {
                if( pBoneComb[iAttrib].BoneId[i] != UINT_MAX )
                {
                    NumBlend = i;
                }
            }
            if( m_d3dCaps.MaxVertexBlendMatrices >= NumBlend + 1)
            {
                for( DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
                {
                    iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
                    if( iMatrixIndex != UINT_MAX)
                    {
                        D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
                        m_pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp );
                    }
                }
                m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, NumBlend );
                if( (AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32) )
                {
                    m_pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D );
                    m_pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] );
                    AttribIdPrev = pBoneComb[iAttrib].AttribId;
                }
                m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
                m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
                m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
                pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
            }
        }
        if( pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups)
        {
            AttribIdPrev = UNUSED32; 
            m_pd3dDevice->SetSoftwareVertexProcessing( true );
            for( iAttrib = pMeshContainer->iAttributeSW; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++ )
            {
                NumBlend = 0;
                for( DWORD i = 0; i < pMeshContainer->NumInfl; ++i )
                {
                    if( pBoneComb[iAttrib].BoneId[i] != UINT_MAX )
                    {
                        NumBlend = i;
                    }
                }
                if( m_d3dCaps.MaxVertexBlendMatrices < NumBlend + 1 )
                {
                    for( DWORD i = 0; i < pMeshContainer->NumInfl; ++i)
                    {
                        iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
                        if( iMatrixIndex != UINT_MAX)
                        {
                            D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
                            m_pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp );
                        }
                    }
                    m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);
                    if( (AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32))
                    {
                        m_pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D );
                        m_pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] );
                        AttribIdPrev = pBoneComb[iAttrib].AttribId;
                    }
                    pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
                }
            }
            m_pd3dDevice->SetSoftwareVertexProcessing( false );
        }
        m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, 0 );
    }
    else if( m_SkinningMethod == D3DINDEXED )
    {
        if( pMeshContainer->UseSoftwareVP )
        {
            m_pd3dDevice->SetSoftwareVertexProcessing( true );
        }
        if( pMeshContainer->NumInfl == 1 )
            m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_0WEIGHTS );
        else
            m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, pMeshContainer->NumInfl - 1 );

        if( pMeshContainer->NumInfl )
        {
            m_pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, true );
        }
        pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
        for(DWORD iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++ )
        {
            for( iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry )
            {
                iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
                if( iMatrixIndex != UINT_MAX)
                {
                    D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], 
                        pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );	
                    m_pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( iPaletteEntry ), &matTemp );
                }
            }
            m_pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D );
            m_pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] );
            pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib );
        }
        m_pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
        m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);
        if( pMeshContainer->UseSoftwareVP)
        {
            m_pd3dDevice->SetSoftwareVertexProcessing( false );
        }
    }
    else if( m_SkinningMethod == SOFTWARE )
    {
        DWORD cBones  = pMeshContainer->pSkinInfo->GetNumBones();
        for(DWORD i = 0; i < cBones; ++i )
        {
            D3DXMatrixMultiply( &m_pBoneMatrices[i], &pMeshContainer->pBoneOffsetMatrices[i], pMeshContainer->ppBoneMatrixPtrs[i] );
        }

        D3DXMATRIX  Identity;
        D3DXMatrixIdentity( &Identity );
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &Identity );

        PBYTE pbVerticesSrc;
        PBYTE pbVerticesDest;
        pMeshContainer->pOrigMesh->LockVertexBuffer( D3DLOCK_READONLY, (LPVOID*)&pbVerticesSrc );
        pMeshContainer->MeshData.pMesh->LockVertexBuffer( 0, (LPVOID*)&pbVerticesDest );
        
        pMeshContainer->pSkinInfo->UpdateSkinnedMesh( m_pBoneMatrices, NULL, pbVerticesSrc, pbVerticesDest );
        
        pMeshContainer->pOrigMesh->UnlockVertexBuffer();
        pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();

        for(DWORD iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++ )
        {
            m_pd3dDevice->SetMaterial( &(pMeshContainer->pMaterials[pMeshContainer->pAttributeTable[iAttrib].AttribId].MatD3D) );
            m_pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[pMeshContainer->pAttributeTable[iAttrib].AttribId] );
            pMeshContainer->MeshData.pMesh->DrawSubset(pMeshContainer->pAttributeTable[iAttrib].AttribId );
        }
    }
    m_dwTrangleCnt += pMeshContainer->MeshData.pMesh->GetNumFaces();
}

void cSkinMesh::UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix)
{
    D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
    if( pParentMatrix != NULL)
    {
        D3DXMatrixMultiply( &pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix );
    }
    else
    {
        pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;
    }
    if( pFrame->pFrameSibling != NULL )
    {
        UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);
    }
    if( pFrame->pFrameFirstChild != NULL )
    {
        UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);
    }
}
