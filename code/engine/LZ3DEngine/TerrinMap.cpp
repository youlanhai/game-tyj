
#include "Commen.h"
#include "LZData.h"
#include "Pick.h"
#include "Res.h"
#include "Entity.h"

#include "TerrinMap.h"
#include "Utilities.h"
#include "../Physics/Physics.h"
#include "Material.h"

/**
 *  使用四叉树管理子地图（chunk）。可以很方便查找出位于矩形范围内的chunk。
 *  渲染
 *      以玩家位置为中心，其可见区域构成一个矩形，矩形覆盖到的chunk都需要渲染出来。
 *  碰撞检测
 *      玩家碰撞体构成的aabb所覆盖的chunk。
 *  射线拾取
 */

const int direct8[][2] = {{-1,-1},{1,-1},{1,1},{-1,1},{0,-1},{-1,0},{0,1},{1,0}};


LZDLL_API cMap* getMap(void)
{
    return cMap::instance();
}

//////////////////////////////////////////////////////////////////////////
/*static*/ cMap * cMap::instance()
{
    static cMap s_map;
    return &s_map;
}

bool cMap::init(IDirect3DDevice9* pDevice)
{
    assert(m_pd3dDevice==NULL && "cMap::init erorr: has been initialized!");

    XWRITE_LOG(_T("INFO: cMap::init."));

    m_pd3dDevice = pDevice;
    m_pd3dDevice->AddRef();

    return true;
}

void cMap::fini()
{
    if(!m_pd3dDevice) return;

    release();
    VIBCache::instance()->clear();

    m_pTData = NULL;
    SafeRelease(m_pd3dDevice);

    XWRITE_LOG(_T("INFO: cMap::fini."));
}

cMap::cMap()
    : m_pd3dDevice(0)
{
    m_pTData = new TerrinData();

    m_nodeSize = 0;         //< 每个格子的尺寸。
    m_nodeR = 0;        //< 结点行数
    m_nodeC = 0;        //< 结点列数
    
    m_showLevel = 1;
    m_usefull = false;

    m_pSource = NULL;
    m_pActiveObj = NULL;
    m_pSelectObj = NULL;
    m_loadingProgress = 0.0f;
}

cMap::~cMap(void)
{
    //assert(m_pd3dDevice==NULL && "cMap not fini!");
}


/** 释放资源。*/
void cMap::release(void)
{
    m_mapNodes.clear();
    m_usefull = false;
    m_renderNodes.clear();

    m_pActiveObj = NULL;
    m_pSelectObj = NULL;
}


/** 根据地图文件名，加载地图。地图文件包含地图的高度图，行列等信息。*/
bool cMap::loadMap(std::string mapName)
{
    m_loadingProgress = 0.0f;

    XWRITE_LOGA("INFO: cMap::loadMap %s", mapName.c_str());
    release();

    m_mapName = mapName;

    LZDataPtr ptr = createLzdFromFile(LZD_LZD, mapName);
    if (!ptr)
    {
        XWRITE_LOGA("ERROR: mapp '%s' doesn't exist!", mapName.c_str());
        return false;
    }

    if (!m_pTData->loadHeightMap(
        ptr->readString("mapName"), 
        ptr->readInt("vrows"), 
        ptr->readInt("vcols"),
        ptr->readFloat("squareSize"),
        ptr->readFloat("heightScale"),
        ptr->readBool("useOneTex")))
    {
        XWRITE_LOGA("ERROR: load the height data failed!");
        return false;
    }


    m_objOnGround = ptr->readBool("onGround", true);
    m_nodeSize = m_pTData->squareSize() * TerrainConfig::MaxMapNodeGrid;
    m_nodeR = (int)ceil(m_pTData->height()/(float)m_nodeSize);
    m_nodeC = (int)ceil(m_pTData->width()/(float)m_nodeSize);

    debugMessage(_T("INFO: map rows=%d, cols=%d, node size=%f"), 
        m_nodeR, m_nodeC, m_nodeSize);

    m_textureName = ptr->readString("texture");

    createNode();

    m_usefull = true;
    
    if(!useChunkData)
    {
        std::string name = ptr->readString("resource");
        if(!cTerrainRes::instance()->load(name))
        {
            XWRITE_LOGA("WARNING: load res config file '%s' failed!", name.c_str());
        }

        name = ptr->readString("object");
        if(!loadObj(name))
        {
            XWRITE_LOGA("WARNING: load obj config file '%s' failed!", name.c_str());
        }

    }

    
    //加载路点
    Lazy::tstring wpPath;
    if(!Physics::mapNameToWpPath(wpPath, Lazy::charToWChar(mapName)))
    {
        WRITE_LOG(_T("ERROR: Parse waypoint path failed!"));
    }
    else
    {
        if(!Physics::WayChunkMgr::instance()->load(wpPath))
        {
            WRITE_LOG(_T("ERROR: load way point failed!"));
        }
    }

    m_loadingProgress = 1.0f;
    WRITE_LOG(_T("INFO: map loaded."));
    return true;
}

void cMap::saveMap(const std::string & nameName)
{
    doDataConvert();
}

//数据类型转换
void cMap::doDataConvert()
{
    NodeIterator it;
    FOR_EACH(m_mapNodes, it)
    {
        (*it)->load();
        (*it)->save();
    }
}

void cMap::createNode(void)//创建顶点索引
{
    debugMessage(_T("INFO: cMap::createNodes..."));

    float ssize = (float)m_pTData->squareSize();
    RefPtr<cMapNode> pNode = NULL;

    float totalNode = float(m_nodeR * m_nodeC);

    //将地图分成m_nodeR*m_nodeC块
    for (int r=0; r<m_nodeR; ++r)
    {
        for (int c=0; c<m_nodeC; ++c)
        {
            //debugMessage(_T("INFO : create map node (row %d, col %d)"), r, c);

            m_loadingProgress = (r*m_nodeC + c)/totalNode * 0.5f;
            m_mapNodes.push_back(new cMapNode(m_pTData.get(), r, c));
        }
    }

    debugMessage(_T("INFO: preload vib cache..."));
    for (size_t i=VIBCache::instance()->size(); i<8; ++i)
    {
        VIBCache::instance()->preLoadOne();
    }
}

bool cMap::loadObj(const std::string & filename)
{
    debugMessage(_T("INFO: cMap::loadObj..."));

    std::string name = filename;
    if (!name.empty())
    {
        m_objFileName = name;
    }
    if (m_objFileName.empty())
    {
        return false;
    }

    for (NodeIterator it=m_mapNodes.begin();
        it!=m_mapNodes.end(); ++it)
    {
        (*it)->clearObj();
    }

    std::ifstream fin(m_objFileName.c_str());
    if (!fin.good())
    {
        return false;
    }
    
    int count = 0;
    fin>>count;
    for (int i=0; i<count; ++i)
    {
        TerrainObjPtr pObj = createObj();
        fin>>(*pObj);

        if (m_objOnGround)
        {
            pObj->m_vPos.y = getHeight(pObj->m_vPos.x, pObj->m_vPos.z);
        }

        addObj(pObj);

        m_loadingProgress += i * 0.5f / count;
    }

    fin.close();

    return true;
}

bool cMap::saveObj()
{
    doDataConvert();
    return true;
}

TerrainObjPtr cMap::createObj()
{
    return new cTerrainObj();
}

void cMap::addObj(TerrainObjPtr pObj)
{
    if(!pObj) return;

    Physics::AABB aabb;
    pObj->getWorldAABB(aabb);

    cMapNode* pNode = getNode(pObj->m_vPos.x, pObj->m_vPos.z);
    if (!pNode) return;
    
    pObj->setReference(false);
    pNode->addObj(pObj);
    size_t chunkId = pNode->rowID() << 16 | pNode->colID();

    //在其覆盖的所有chunk里，都添加一个副本

    //TODO: 优化此处的查找算法
    NodeIterator it;
    FOR_EACH(m_mapNodes, it)
    {
        if(*it != pNode && (*it)->intersect(aabb))
        {
            TerrainObjPtr copy = pObj->clone();
            copy->setRefChunk(chunkId);
            copy->setReference(true);
            (*it)->addObj(copy);

#if 0
            debugMessage(_T("TRACE: add refrence (%d %d) '%s' refid=%u"), 
                (*it)->rowID(), (*it)->colID(), copy->uuid().c_str(), chunkId);
#endif
        }
    }
}

void cMap::removeObj(TerrainObjPtr pObj)
{
    if (!pObj) return;

    if (pObj == m_pActiveObj)
    {
        m_pActiveObj = NULL;
    }
    
    NodeIterator it;
    FOR_EACH(m_mapNodes, it)
    {
        (*it)->removeObj(pObj);
    }
}

void cMap::refreshObj(TerrainObjPtr pObj)
{
    if (!pObj) return;

    removeObj(pObj);
    addObj(pObj);
}

void cMap::render(IDirect3DDevice9* pDevice)
{
    if (!m_usefull)
    {
        return ;
    }

    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE );
    pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

    pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
    pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);

    pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    CMaterial::setMaterial(pDevice, 
        D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
        D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f), 
        D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f),
        D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f),
        1.0f);

    pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
    
    pDevice->SetTexture(0, getResMgr()->getTextureEx(m_textureName));
    for (size_t i=0; i<m_renderNodes.size(); ++i)
    {
        m_renderNodes[i]->render(pDevice);
    }
    pDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

    pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

    //绘制地图物体
    for (size_t i=0; i<m_renderNodes.size(); ++i)
    {
        m_renderNodes[i]->renderObj(pDevice);
    }
}

void cMap::update(float elapse)
{
    if (!m_usefull)
    {
        return ;
    }
    genRenderNode();
}

MapNodePtr cMap::currentNode()
{
    if (m_pSource)
    {
        D3DXVECTOR3 pos = m_pSource->getPos();
        if (!isInBound(pos.x, pos.z))
        {
            return NULL;
        }
        return getNode(pos.x, pos.z);
    }
    return NULL;
}

/** 根据索引获得地图结点。*/
cMapNode* cMap::getNode(int r, int c)
{
    assert(r>=0 && r<m_nodeR && c>=0 && c<m_nodeC && "获得地图结点时访问越界。");
    int index = r*m_nodeC + c;
    return m_mapNodes[index].get();
}

/** 根据坐标取得地图结点。*/
cMapNode* cMap::getNode(float x, float z)
{
    Square2 sq = getNodeIndex(x, z);
    return getNode(sq.r, sq.c);
}

/** 根据真实位置得到子结点索引。x:为列，y:为行。*/
Square2 cMap::getNodeIndex(float x, float z)
{
    x += m_pTData->width()/2.0f;
    z = m_pTData->height()/2.0f - z;
    return Square2(int(z/m_nodeSize), int(x/m_nodeSize));
}


Square2 cMap::getSquareIndex(float x, float z)
{
    x += m_pTData->width()/2.0f;
    z = m_pTData->height()/2.0f - z;
    return Square2(int(z/m_pTData->squareSize()), int(x/m_pTData->squareSize()));
}


/** 生成邻接的8个结点。*/
void cMap::genRenderNode(void)
{
    D3DXVECTOR3 pos(0, 0, 0);
    if (m_pSource)
    {
        pos = m_pSource->getPos();
        if (!isInBound(pos.x, pos.z))
        {
            return;
        }
    }
    m_renderNodes.clear();

    Square2 sq = getNodeIndex(pos.x, pos.z);

    int showLv = m_showLevel;
    if (showLv < 0)
        showLv = 0;
    else if (showLv > m_nodeR/2)
        showLv = m_nodeR/2;

    //m_renderNodes.push_back(getNode(cur.y, cur.x));
    int rBound = sq.r - showLv;
    int cBound = sq.c - showLv;
    int rRange = rBound + showLv*2 + 1;
    int cRange = cBound + showLv*2 + 1;
    if (rBound < 0)
    {
        rBound = 0;
    }
    if (rRange >= m_nodeR)
    {
        rRange = m_nodeR - 1;
    }
    if (cBound < 0)
    {
        cBound = 0;
    }
    if (cRange >= m_nodeC)
    {
        cRange = m_nodeC - 1;
    }
    for (int r=rBound; r<=rRange; ++r)
    {
        for (int c=cBound; c<=cRange; ++c)
        {
            m_renderNodes.push_back(getNode(r, c));
        }
    }
}


bool cMap::intersect(Physics::Vector3 & position)
{
    for (NodeIterator it=m_renderNodes.begin();
        it != m_renderNodes.end(); 
        ++it)
    {
        if ((*it)->intersect(position))
        {
            return true;
        }
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////
void cMap::handeMouseEvent(UINT msg, WPARAM wParam, LPARAM lParam )
{
    if (!getPick() || !getPick()->isTerrainObjEnable())
    {
        return;
    }
    if (getActiveObj())
    {
        if (msg == WM_LBUTTONDOWN)
        {
            getActiveObj()->focusCursor(CM_LDOWN);
        }
        else if (msg == WM_LBUTTONUP)
        {
            getActiveObj()->focusCursor(CM_LUP);
        }
        else if (msg == WM_RBUTTONDOWN)
        {
            getActiveObj()->focusCursor(CM_RDOWN);
        }
        else if (msg == WM_RBUTTONUP)
        {
            getActiveObj()->focusCursor(CM_RUP);
            m_pSelectObj = getActiveObj();
        }
    }
}

bool cMap::intersectWithCursor()
{
    TerrainObjPtr pActiveObj = NULL;
    float minDist = 1000000.0f;
    for (NodeIterator node = m_renderNodes.begin();
        node != m_renderNodes.end();
        ++node)
    {
        ObjectIter it;
        FOR_EACH((**node), it)
        {
            float dist = (*it)->distToCamera();
            if(dist<minDist && (*it)->intersectWithCursor())
            {
                pActiveObj = *it;
                minDist = dist;
            }
        }
    }
    setActiveObj(pActiveObj);
    return pActiveObj!=NULL;
}

void cMap::setActiveObj(TerrainObjPtr pObj)
{
    if (m_pActiveObj)
    {
        m_pActiveObj->focusCursor(CM_LEAVE);
    }
    m_pActiveObj = pObj;
    if (m_pActiveObj)
    {
        m_pActiveObj->focusCursor(CM_ENTER);
    }
}

TerrainObjPtr cMap::terrainObj(int id)
{
    return NULL;
}

//////////////////////////////////////////////////////////////////////////
bool cMap::isInBound(float x, float z)
{
    return m_pTData->isInBound(x, z);
}

float cMap::getHeight(float x, float z)
{
    return m_pTData->getPhysicalHeight(x, z);
}

void cMap::setShowLevel(int level)
{
    m_showLevel = level;
}


float cMap::xMin()
{
    return -width()/2.0f;
}

float cMap::xMax()
{
    return width()/2.0f;
}

float cMap::zMin()
{
    return -height()/2.0f;
}

float cMap::zMax()
{
    return height()/2.0f;
}

float cMap::width()
{
    return (float)m_pTData->width();
}

float cMap::height()
{
    return (float)m_pTData->height();
}