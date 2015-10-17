#pragma once
/** 该文件为蒙皮骨骼动画核心实现*/

#include "RenderRes.h"
#include "../Physics/math3d.h"

enum METHOD 
{
	D3DNONINDEXED,
	D3DINDEXED,
	SOFTWARE,
	NONE
};

class cSkinMesh;
typedef RefPtr<cSkinMesh> SkinMeshPtr;

///蒙皮骨骼动画
class LZDLL_API cSkinMesh : public IRenderRes
{
protected:

    cSkinMesh(LPDIRECT3DDEVICE9 pD3DDevice);	//构造函数

public://接口实现

    virtual ~cSkinMesh();

    /** 创建自己。*/
    static IRenderRes* createObj(void);

    /** 加载资源。*/
    virtual bool load(std::string filename);

    /** 获得资源类型*/
    virtual LPCSTR getType(void){ return "skinmesh"; }

    /** 更新*/
    virtual void update(float fElapse);

    /** 渲染*/
    virtual void render(IDirect3DDevice9 * pDevice);

public:

    /** 从X文件中加载骨骼动画。*/
    bool loadFromXFile(std::string fileName);

    /** 生成蒙皮网格*/
    HRESULT GenerateSkinnedMesh(D3DXMESHCONTAINER *pMeshContainer);

    /** 克隆动画控制器。*/
    LPD3DXANIMATIONCONTROLLER cloneAnimaCtrl(void);

    D3DXMATRIX getWorldMatrix( void){ return m_worldMatrix; }

    void setWorldMatrix(D3DXMATRIX mat){ m_worldMatrix = mat;}

	LPD3DXANIMATIONCONTROLLER GetAnimationContrl() { return m_pAnimController; }

	DWORD GetTrangleCnt() { return m_dwTrangleCnt; }

    float getBBRadius(void){ return m_bbRadius; }

    D3DXVECTOR3 getBBCenter(void){ return m_bbCenter; }

    Physics::AABB getAABB(){ return m_aabb; }

protected:
	LPDIRECT3DDEVICE9			m_pd3dDevice;
	LPD3DXFRAME					m_pFrameRoot;		//< 根骨节点
	LPD3DXANIMATIONCONTROLLER	m_pAnimController;  //< 动画控制器
	D3DXMATRIX*		    m_pBoneMatrices;    //< 骨骼矩阵
	UINT				m_NumBoneMatricesMax;//< 骨骼矩阵数量
	METHOD				m_SkinningMethod; //< 蒙皮类型
	D3DCAPS9 			m_d3dCaps;

	DWORD				m_dwTrangleCnt;

    float           m_bbRadius; //< 包围盒半径
    D3DXVECTOR3     m_bbCenter; //< 包围盒中心

    D3DXMATRIX      m_worldMatrix;
    Physics::AABB    m_aabb;
protected:
    void DrawFrame(LPD3DXFRAME pFrame);
    void DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase);

    HRESULT SetupBoneMatrixPointers( LPD3DXFRAME pFrame );
	HRESULT SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase );

    ///更新骨骼框架
    void UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix );

};
