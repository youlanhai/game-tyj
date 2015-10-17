//CMesh.h
#pragma once

#include <vector>
#include "BoundingBox.h"
#include "RenderObj.h"
#include "RenderRes.h"
#include "../Physics/math3d.h"

class LZDLL_API cResMesh : public IRenderRes
{
public:
    ~cResMesh();

    /** 创建自己。*/
    static IRenderRes* createObj(void);

    /** 加载资源。*/
    virtual bool load(std::string filename);

    /** 获得资源类型*/
    virtual LPCSTR getType(void){ return "mesh"; }

    void update(float fElapse);

    void render(IDirect3DDevice9 * pDevice);

public:

    /**获得材质。*/
    std::vector<D3DMATERIAL9>& getMaterials(void){ return m_vecMaterials; }

    /**获得mesh。*/
    ID3DXMesh * getMesh(void){ return m_pMesh; }

    float getBBRadius(void){ return m_bbRadius; }

    D3DXVECTOR3 getBBCenter(void){ return m_bbCenter; }

    const Physics::AABB & getAABB(){ return m_aabb; }

protected:
    ///构造函数
    cResMesh();

protected:
    std::vector<IDirect3DTexture9*>	m_vecTextures;	//< 纹理数组指针
    std::vector<D3DMATERIAL9>		m_vecMaterials;	//< 材质数组
    ID3DXMesh       *m_pMesh;	//< 网格
    float           m_bbRadius; //< 包围盒半径
    D3DXVECTOR3     m_bbCenter; //< 包围盒中心
    Physics::AABB    m_aabb;     //< aabb包围盒
};


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

struct MeshWrap
{
public:
    MeshWrap(ID3DXMesh *mesh);
    ~MeshWrap();

    DWORD getIndex(DWORD i) const;

    void getPosition(DWORD index, D3DXVECTOR3 & p);

    void getTriangle(DWORD iFace, D3DXVECTOR3 &p0, D3DXVECTOR3 &p1, D3DXVECTOR3 &p2);

    void getTriangle(DWORD iFace, Physics::Triangle & tri)
    {
        getTriangle(iFace, tri.a, tri.b, tri.c);
    }

	DWORD getNbVertex() const { return m_numVertex; }
	DWORD getNbFace() const { return m_numFaces; }

	const BYTE * getVertices() const { return m_vBuffer; }
	const BYTE * getIndices() const { return m_iBuffer; }

	DWORD getIndexStride() const { return m_isIdx32 ? 4: 2; }
	DWORD getVertexStride() const { return m_step; }
private:
    bool        m_isIdx32;
    ID3DXMesh   *m_mesh;
    BYTE        *m_vBuffer;
    BYTE        *m_iBuffer;
    DWORD       m_step;
    DWORD       m_numFaces;
    DWORD       m_numVertex;
};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

LPD3DXMESH generateSimpleMesh(LPD3DXMESH srcMesh, float factor);
