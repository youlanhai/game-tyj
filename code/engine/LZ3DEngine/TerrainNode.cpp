

#include "Commen.h"
#include "Res.h"
#include "Entity.h"
#include "Utilities.h"
#include "TerrainNode.h"
#include "App.h"
#include "TerrinMap.h"

#include "../Physics/Collision.h"

typedef WORD IndexType;    //< 索引顶点类型
DWORD INDEX_TYPE = D3DFMT_INDEX16;

//////////////////////////////////////////////////////////////////////////
#if USE_MESH

template<typename T>
bool updateIndices(VIBHolder * hoder, int vertices, T)
{
    T *pIndex;
    if(!hoder->lockIB((void**)&pIndex)) return false;

    int grid = vertices - 1;
    int i = 0;
    for (int r =0; r<grid; ++r)
    {
        for (int c=0; c<grid; ++c)
        {
            int n = r * vertices + c;
            pIndex[i] = n;
            pIndex[i+1] = n + 1;
            pIndex[i+2] = n + vertices;

            pIndex[i+3] = n + 1;
            pIndex[i+4] = n + vertices + 1;
            pIndex[i+5] = n + vertices;

            i += 6;
        }
    }

    hoder->unlockIB();
    return true;
}

bool VIBHolder::create(LPDIRECT3DDEVICE9 pDevice, int vertices)
{
    const int grid = vertices - 1;
    const int numFace = TerrainConfig::MapNodeFace;
    const int numIndices = numFace * 3;

    if(FAILED(D3DXCreateMeshFVF(numFace, 
        TerrainConfig::MaxMapNodeVerticesSq, 
        D3DXMESH_MANAGED, 
        TerrinVertex::FVF, pDevice, &m_pMesh)))
    {
        debugMessage(_T("ERROR: 创建VIBHolder Mesh失败"));
        return false;
    }

    bool ret;
    if (m_pMesh->GetOptions() & D3DXMESH_32BIT)
        ret = updateIndices(this, vertices, DWORD());
    else
        ret = updateIndices(this, vertices, WORD());

    if(!ret)
    {
        SafeRelease(m_pMesh);
        return false;
    }

    MEMORY_CHECK_CONS;
    return ret;
}

void VIBHolder::optimize()
{
    if(!m_pMesh) return;

    D3DXComputeNormals(m_pMesh, NULL);
}


void VIBHolder::release()
{
    MEMORY_CHECK_DEST;
    SafeRelease(m_pMesh);
}

void VIBHolder::render(LPDIRECT3DDEVICE9 pDevice)
{
    m_pMesh->DrawSubset(0);
}

#else

bool VIBHolder::create(LPDIRECT3DDEVICE9 pDevice, int vertices)
{
    //创建顶点缓冲区
    if(FAILED(pDevice->CreateVertexBuffer(
        TerrinVertex::SIZE*sizeVertex,
        0,
        TerrinVertex::FVF,
        D3DPOOL_MANAGED,
        &vertexBuffer,
        NULL)))
    {
        debugMessage(_T("ERROR: 创建VIBHolder顶点缓冲区失败"));
        return false;
    }

    //创建索引缓冲区
    if(FAILED(pDevice->CreateIndexBuffer(
        sizeof(IndexType)*sizeIndex ,
        0,
        D3DFMT_INDEX16,
        D3DPOOL_MANAGED,
        &indexBuffer,
        NULL)))
    {
        SafeRelease(vertexBuffer);
        debugMessage(_T("ERROR 创建VIBHolder索引缓冲区失败"));
        return false;
    }

    updateIndices(this, vertices, WORD());

    MEMORY_CHECK_CONS;
    return true;
}


void VIBHolder::release()
{
    MEMORY_CHECK_DEST;
    SafeRelease(vertexBuffer);
    SafeRelease(indexBuffer);
}

void VIBHolder::render(LPDIRECT3DDEVICE9 pDevice)
{
    pDevice->SetStreamSource(0, m_vibHolder.vertexBuffer, 0, TerrinVertex::SIZE);
    pDevice->SetFVF(TerrinVertex::FVF);//设置灵活顶点格式
    pDevice->SetIndices(m_vibHolder.indexBuffer);
    pDevice->DrawIndexedPrimitive(
        D3DPT_TRIANGLELIST, 0, 0, m_vertexCount, 0, (m_vrows-1)*(m_vcols-1)*2);
}

#endif

//////////////////////////////////////////////////////////////////////////
/*static*/ VIBCache* VIBCache::instance()
{
    static VIBCache s_instance;
    return &s_instance;
}

VIBCache::VIBCache() 
{
    MEMORY_CHECK_CONS;
}

VIBCache::~VIBCache() 
{
    clear();
    MEMORY_CHECK_DEST;
}

void VIBCache::preLoadOne()
{
    m_pool.push_back(VIBHolder());
    m_pool.back().create(getApp()->getDevice(), TerrainConfig::MaxMapNodeVertices);
}

VIBHolder VIBCache::get()
{
    VIBHolder holder;
    if (m_pool.empty())
    {
        holder.create(getApp()->getDevice(), TerrainConfig::MaxMapNodeVertices);
        return holder;
    }

    holder = m_pool.front();
    m_pool.pop_front();
    return holder;
}

void VIBCache::release(VIBHolder holder)
{
    if(!holder.valid()) return ;

    if (m_pool.size() < TerrainConfig::MaxMapNodeCacheSize)
    {
        m_pool.push_back(holder);
        holder.clear();
    }
    else
    {
        holder.release();
    }
}

void VIBCache::clear()
{
    for (CachePool::iterator it = m_pool.begin();
        it != m_pool.end(); ++it)
    {
        it->release();
    }
    m_pool.clear();
}


//////////////////////////////////////////////////////////////////////////

cMapNode::cMapNode(TerrinData* pData, int rID, int cID)
    : m_pTerrin(pData)
    , m_loaded(false)
    , m_rID(rID)
    , m_cID(cID)
{
    MEMORY_CHECK_CONS;

    m_gridSize = pData->squareSize();
    m_size = m_gridSize * TerrainConfig::MaxMapNodeGrid;
    m_z0 = m_rID * m_size;
    m_x0 = m_cID * m_size;

    m_rect.left = xToMap(0);
    m_rect.top = zToMap(0);
    m_rect.right = xToMap(m_size);
    m_rect.bottom = zToMap(m_size);

    m_rect.normalization();
}

cMapNode::~cMapNode(void)
{
    release();

    MEMORY_CHECK_DEST;
}

float cMapNode::xToMap(float x) const
{
    return x + m_x0 - m_pTerrin->width()/2.0f;
}

float cMapNode::zToMap(float z) const
{
    return m_pTerrin->height()/2.0f - (z + m_z0);
}

Square2 cMapNode::squareToMap(int r, int c)
{
    return Square2(r + int(m_z0/m_gridSize), c + int(m_x0/m_gridSize));
}

void cMapNode::getPos(int index, D3DXVECTOR3 & pos)
{
    int c = index % TerrainConfig::MaxMapNodeGrid;
    int r = index / TerrainConfig::MaxMapNodeGrid;

    Square2 sq = squareToMap(r, c);

    if (sq.r < 0 ||
        sq.r >= m_pTerrin->vrows() ||
        sq.c < 0 ||
        sq.c >= m_pTerrin->vcols())
    {
        ZeroMemory(&pos, sizeof(pos));
        return ;
    }

    pos.x = xToMap(c * m_gridSize);
    pos.y = m_pTerrin->getHeight(r, c);
    pos.z = zToMap(r * m_gridSize);
}

void cMapNode::load()
{
    if(m_loaded) return;
    m_loaded = true;

    m_vibHolder = VIBCache::instance()->get();
    if( !m_vibHolder.valid() )
    {
        debugMessage(_T("ERROR: cMapNode::load vib failed!"));
        return;
    }

    float su = 1.0f/m_size;
    float sv = 1.0f/m_size;
    bool onTex = m_pTerrin->useOneTex();

    TerrinVertex* pVertex = NULL;
    if(!m_vibHolder.lockVB((void**)&pVertex, 0))
    {
        debugMessage(_T("ERROR: cMapNode::load lock vertexbuffer faild"));
        return;
    }



    //以下是创建网格数据
    const int vertices = TerrainConfig::MaxMapNodeVertices;
    for(int z=0; z<vertices; ++z)
    {
        for(int x=0; x<vertices; ++x)
        {
            int i = z * vertices + x;//顶点索引
            float mx = xToMap(x * m_gridSize);
            float mz = zToMap(z * m_gridSize);
            pVertex[i].pos.x = mx;
            pVertex[i].pos.z = mz;
            pVertex[i].pos.y = m_pTerrin->getPhysicalHeight(mx, mz);
            if (onTex)
            {
                pVertex[i].u = x * m_gridSize * su * 4;
                pVertex[i].v = z * m_gridSize * sv * 4;
            }
            else
            {
                pVertex[i].u = float(z%2);
                pVertex[i].v = float(x%2);
            }
        }
    }
    m_vibHolder.unlockVB();

    updateNormal();

    if(!useChunkData) return;
    
    Lazy::tstring name;
    Lazy::charToWChar(name, cMap::instance()->getMapName());
    Lazy::tstring path = Lazy::getFilePath(name);

    formatString(name, _T("%4.4d%4.4d.lzd"), m_rID, m_cID);
    path += name;

    debugMessage(_T("TRACE: load node(%d %d) path='%s'"), m_rID, m_cID, path.c_str());

    Lazy::LZDataPtr root = new Lazy::lzd();
    if(!root->load(path))
    {
        debugMessage(_T("ERROR: load failed! node(%d %d) path='%s'"), m_rID, m_cID, path.c_str());
        return;
    }

    root = root->read(_T("models"));

    m_objPool.clear();
    Lazy::LZDataBase::DataConstIterator it;
    FOR_EACH((*root), it)
    {
        if((*it)->tag() == _T("model"))
        {
            TerrainObjPtr pObj = new cTerrainObj();
            pObj->load(*it);
            addObj(pObj);
        }
    }
}

void cMapNode::save()
{
    if(!m_loaded)
    {
        //debugMessage(_T("ERROR: save failed! the node(%d %d) doesn't loaded!"), m_rID, m_cID);
        //return;
    }

    Lazy::tstring name;
    Lazy::charToWChar(name, cMap::instance()->getMapName());
    Lazy::tstring path = Lazy::getFilePath(name);

    formatString(name, _T("%4.4d%4.4d.lzd"), m_rID, m_cID);
    path += name;

    debugMessage(_T("TRACE: save node(%d %d) path='%s'"), m_rID, m_cID, path.c_str());

    Lazy::LZDataPtr root = new Lazy::lzd();

    {//save time
        Lazy::tstring strTime;
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);
    
        formatString(strTime, _T("[%d.%d.%d][%d.%d.%d][%d]"), 
            sysTime.wYear, sysTime.wMonth, sysTime.wDay,
            sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
        root->writeString(_T("savetime"), strTime);
    }

    Lazy::LZDataPtr models = new Lazy::lzd(_T("models"));
    root->addChild(models);

    ObjectIter it;
    FOR_EACH(m_objPool, it)
    {
        Lazy::LZDataPtr modelInfo = new Lazy::lzd(_T("model"));
        models->addChild(modelInfo);
        (*it)->save(modelInfo);
    }

    if(!root->save(path))
    {
        debugMessage(_T("ERROR: save failed! node(%d %d) path='%s'"), m_rID, m_cID, path.c_str());
    }
}

void cMapNode::render(IDirect3DDevice9* pDevice)
{
    if(!m_loaded) load();
    if (!m_vibHolder.valid()) return ;

    D3DXMATRIX matWord;
    D3DXMatrixIdentity(&matWord);
    pDevice->SetTransform(D3DTS_WORLD, &matWord);
    m_vibHolder.render(pDevice);
}

void cMapNode::renderObj(IDirect3DDevice9* pDevice)
{
    ObjectPool::const_iterator it;
    FOR_EACH(m_objPool, it)
    {
        (*it)->render(pDevice);
    }

    //debugMessage(_T("INFO: map(%d, %d) render obj. num:%d"), m_rID, m_cID, m_objPool.size());
}

void cMapNode::release(void)
{
    clearObj();
    if (m_loaded)
    {
        VIBCache::instance()->release(m_vibHolder);
    }
}

void cMapNode::addObj(TerrainObjPtr pObj)
{
    m_objPool.push_back(pObj);
}

void cMapNode::removeObj(TerrainObjPtr pObj)
{
    ObjectIter it = m_objPool.begin();
    while(it != m_objPool.end())
    {
        if(*it == pObj || (*it)->uuid() == pObj->uuid())
            it = m_objPool.erase(it);
        else ++it;
    }
}

void cMapNode::clearObj()
{
    m_objPool.clear();
}

//鼠标是否与当前地形相交。
bool cMapNode::intersect(Physics::Vector3 & position) const
{
    if(!m_loaded) return false;
    if (!m_vibHolder.valid()) return false;

    cPick *pick = getPick();
    if (NULL == pick) return false;

    Physics::Matrix4x4 mat;
    D3DXMatrixIdentity(&mat);
    Physics::RayCollision collider(pick->rayPos(), pick->rayDir());

    if(!collider.pick(m_vibHolder.getMesh(), mat)) return false;

    collider.getCollidPos(position);
    return true;
}

bool cMapNode::intersect(const Physics::AABB & aabb) const
{
    if(m_x0 > aabb.max.x) return false;
    if(m_x0 + m_size < aabb.min.x) return false;
    if(m_z0 > aabb.max.z) return false;
    if(m_z0 + m_size < aabb.min.z) return false;

    return true;
}

void cMapNode::updateNormal(void)
{
#if USE_MESH

    m_vibHolder.optimize();

#else
    //锁定顶点缓冲区
    TerrinVertex* pVertex = NULL;
    if(!m_vibHolder.lockVB((void**)&pVertex, 0))
    {
        return /*false*/;
    }

    const D3DXVECTOR3 zero(0, 0, 0);
    const int vertices = TerrainConfig::MaxMapNodeVertices;

    D3DXVECTOR3 normal;
    for(int y=1; y<vertices-1; ++y)
    {
        for(int x=1;x<vertices-1; ++x)
        {
            int i = y * vertices + x;//顶点索引

            D3DXVECTOR3 cur, left, right, up, down;
            getPos(i, cur);
            getPos(i-1, left);
            getPos(i+1, right);
            getPos(i-vertices, up);
            getPos(i+vertices, down);

            left -= cur;
            right -= cur;
            up -= - cur;
            down -= cur;

            D3DXVECTOR3 lu, ru, ld, rd;

            D3DXVec3Cross(&lu,&left,&up);
            D3DXVec3Cross(&ru,&up,&right);
            D3DXVec3Cross(&ld,&down,&left);
            D3DXVec3Cross(&rd,&right,&down);

            D3DXVec3Normalize(&ld,&ld); 
            D3DXVec3Normalize(&lu,&lu);
            D3DXVec3Normalize(&rd,&rd);
            D3DXVec3Normalize(&ru,&ru);

            normal = (lu + ru + ld + rd) / 4.0f;
            D3DXVec3Normalize(&pVertex[i].nml, &normal);
        }
    }

    for (int y=0; y<vertices; ++y)
    {
        int i = y*vertices;
        pVertex[i].nml = pVertex[i+1].nml;

        i += vertices - 1;
        pVertex[i].nml = pVertex[i-1].nml;
    }

    for (int x=0; x<vertices; ++x)
    {
        pVertex[x].nml = pVertex[x+vertices].nml;

        int i = (vertices-1)*vertices + x;
        pVertex[i].nml = pVertex[i-vertices].nml;
    }
    m_vibHolder.unlockVB();

#endif

}
