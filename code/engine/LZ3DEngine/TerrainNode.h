#pragma once

/**
 *  当地图过大时，将地图分割成若干个小的地图，每次渲染时，仅渲染玩家周围的几个子地图，这样可以得到较高的效率。
 *  MapNode就是这样的子地图。
*/

#include "TerrinData.h"
#include "TerrainObj.h"

#define USE_MESH 1


namespace TerrainConfig
{

    const int MaxMapNodeVertices = 51;
    const int MaxMapNodeVerticesSq = MaxMapNodeVertices * MaxMapNodeVertices;

    const int MaxMapNodeGrid = MaxMapNodeVertices - 1;
    const int MaxMapNodeGridSq = MaxMapNodeGrid * MaxMapNodeGrid;

    const int MapNodeFace = MaxMapNodeGridSq * 2;
    const int MapNodeIndices = MapNodeFace * 3;

    const int MaxMapNodeCacheSize = 64;

    const int NumVertexSharedTex = 4;
}


struct Square2
{
    Square2() : r(0), c(0) {}
    Square2(int r_, int c_) : r(r_), c(c_) { }

    void set(int r_, int c_)
    {
        r = r_; c = c_;
    }
    int r;
    int c;
};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#if USE_MESH

class VIBHolder
{
public:

    VIBHolder() : m_pMesh(NULL) { }

    bool valid() const { return m_pMesh != NULL; }

    bool lockVB(void** pp, DWORD flag=0) const
    {
        return SUCCEEDED(m_pMesh->LockVertexBuffer(flag, pp));
    }

    void unlockVB() const {  m_pMesh->UnlockVertexBuffer(); }

    bool lockIB(void**pp, DWORD flag=0) const
    {
        return SUCCEEDED(m_pMesh->LockIndexBuffer(flag, pp));
    }

    void unlockIB() const { m_pMesh->UnlockIndexBuffer(); }

    bool create(LPDIRECT3DDEVICE9 pDevice, int vertices);

    void optimize();

    void render(LPDIRECT3DDEVICE9 pDevice);

    LPD3DXMESH getMesh() const { return m_pMesh; }
private:

    //释放资源。不允许显示释放。
    void release();
    void clear(){ m_pMesh = NULL; }

    friend class VIBCache;

    LPD3DXMESH m_pMesh; //将高度图生成mesh
};

#else

class VIBHolder
{
public:

    VIBHolder()
        : vertexBuffer(NULL)
        , indexBuffer(NULL)
    { }

    VIBHolder(LPDIRECT3DVERTEXBUFFER9 vb, LPDIRECT3DINDEXBUFFER9 ib )
        : vertexBuffer(vb)
        , indexBuffer(ib)
    { }

    bool valid() const
    {
        return vertexBuffer!=NULL && indexBuffer!=NULL;
    }

    void clear()
    {
        vertexBuffer = NULL;
        indexBuffer = NULL;
    }

    bool lockVB(void** pp, DWORD size, DWORD flag=0)
    {
        return SUCCEEDED(vertexBuffer->Lock(0, size, pp, flag));
    }

    void unlockVB()
    { 
        vertexBuffer->Unlock(); 
    }

    bool lockIB(void**pp, DWORD size, DWORD flag=0)
    {
        return SUCCEEDED(indexBuffer->Lock(0, size, pp, flag));
    }

    void unlockIB()
    {
        indexBuffer->Unlock();
    }

    bool create(LPDIRECT3DDEVICE9 pDevice, int sizeVertex, int sizeIndex);

    void render(LPDIRECT3DDEVICE9 pDevice);
private:
    //释放资源。不允许显示释放。
    void release();


    LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
    LPDIRECT3DINDEXBUFFER9  indexBuffer;

    friend class VIBCache;
};

#endif

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class VIBCache
{
    VIBCache() ;

    ~VIBCache() ;

public:

    size_t size() const { return m_pool.size(); }

    void preLoadOne();

    VIBHolder get();

    void release(VIBHolder holder);

    void clear();

    static VIBCache* instance();

private:
    typedef std::list<VIBHolder> CachePool;

    CachePool   m_pool;
};

typedef std::vector<TerrainObjPtr>  ObjectPool;
typedef ObjectPool::iterator    ObjectIter;
typedef ObjectPool::const_iterator    ObjectConstIter;

//////////////////////////////////////////////////////////////////////////
//地图结点
//////////////////////////////////////////////////////////////////////////
class cMapNode : public IBase
{
public:
    cMapNode(TerrinData* pData, int rID, int cID);
    ~cMapNode(void);

    void render(IDirect3DDevice9* pDevice);
    
    void load();
    void save();

public://地图物体相关

    void renderObj(IDirect3DDevice9* pDevice);

    ///添加一个物体
    void addObj(TerrainObjPtr pObj);

    ///移除一个物体
    void removeObj(TerrainObjPtr pObj);

    ///情况所有物体
    void clearObj();

    ObjectIter begin(){ return m_objPool.begin(); }
    ObjectIter end(){ return m_objPool.end(); }
    ObjectConstIter begin() const{ return m_objPool.begin(); }
    ObjectConstIter end() const { return m_objPool.end(); }

    ///鼠标射线是否与当前地表相交。
    bool intersect(Physics::Vector3 & position) const;
    
    ///aabb是否与地形相交
    bool intersect(const Physics::AABB & aabb) const;

    int rowID() const { return m_rID; }
    int colID() const { return m_cID; }

    float size() const { return m_size; }
    LPD3DXMESH getTerrainMesh() const { return m_vibHolder.getMesh(); }

    ///获得结点在世界坐标下的区域
    const Physics::FRect & getRect() const{ return m_rect; }

protected:

    ///将结点内的x坐标，转换为世界坐标
    float xToMap(float x) const;
    
    ///将结点内的z坐标，转换为世界坐标
    float zToMap(float z) const;

    float x0() const { return m_x0; }
    float z0() const { return m_z0; }

    Square2 squareToMap(int r, int c);

    void release(void);

    void updateNormal();

    void getPos(int index, D3DXVECTOR3 & pos);

protected:
    bool        m_loaded;
    ObjectPool  m_objPool;

    VIBHolder   m_vibHolder;
    TerrinData *m_pTerrin;

    float   m_size;
    float   m_gridSize;
    float   m_x0;       //原点坐标
    float   m_z0;
    int     m_rID;
    int     m_cID;
    Physics::FRect   m_rect;///<地图x，z坐标区域
};

typedef RefPtr<cMapNode> MapNodePtr;