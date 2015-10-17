
#include "Commen.h"
#include "Utilities.h"
#include "Entity.h"
#include "TerrinMap.h"

#include "Pick.h"
#include "App.h"
#include "Material.h"

struct PickLine
{
    D3DXVECTOR3 pos;
    DWORD color;
    static DWORD FVF;
    static int SIZE;
};

DWORD PickLine::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE;
int PickLine::SIZE = sizeof(PickLine);

//////////////////////////////////////////////////////////////////////////
LZDLL_API cPick* getPick(void)
{
    return cPick::instance();
}

//////////////////////////////////////////////////////////////////////////
/*static*/ cPick* cPick::instance()
{
    static cPick s_pick;
    return &s_pick;
}

cPick::cPick(void)
    : m_pos(0, 0, 0)
    , m_dir(0, 1, 0)
    , m_intersectWithTerrain(false)
    , m_terrainObjEnable(true)
{
    MEMORY_CHECK_CONS;
}

cPick::~cPick()
{
    MEMORY_CHECK_DEST;
}

UINT cPick::handleMouseEvent(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_MOUSEMOVE)
    {
        CPoint pt(lParam);
        convertCursor(pt.x, pt.y);

        queryWithEntity();

        if (m_terrainObjEnable)
        {
            queryWithTerrainObj();
        }
    }
    else if(msg == WM_LBUTTONUP)
    {
        CPoint pt(lParam);
        convertCursor(pt.x, pt.y);

        m_intersectWithTerrain = queryWithTerrain();
    }
    return 0;
}

void cPick::convertCursor(int x, int y)
{
    m_cursor.set(x, y);
    
    FLOAT Rx = 0.0f;
    FLOAT Ry = 0.0f;
    FLOAT mx = 0.0f;
    FLOAT my = 0.0f;

    IDirect3DDevice9* pDevice = getApp()->getDevice();

    D3DVIEWPORT9 mv;
    D3DXMATRIX   mProj;
    D3DXMATRIX   mView;
    pDevice->GetViewport(&mv);
    pDevice->GetTransform(D3DTS_PROJECTION, &mProj);

    mx = mProj._11;
    my = mProj._22;
    Rx = (FLOAT)((x * 2.0) - mv.Width) / (mv.Width * mx);
    Ry = (FLOAT)(mv.Height - (y * 2.0)) / (mv.Height * my);

    //相机坐标系下的，原点和射线方向。
    m_pos.set(0.0f, 0.0f, 0.0f);
    m_dir.set(Rx, Ry, 1.0);

    //求相机变换矩阵的逆矩阵。也就是要将坐标从相机空间转换到世界空间。
    pDevice->GetTransform(D3DTS_VIEW, &mView);
    D3DXMatrixInverse(&mView, NULL, &mView);

    //转换到世界坐标系
    m_pos.applyMatrix(mView);
    m_dir.applyNormalMatrix(mView);
    m_dir.normalize();
}

bool cPick::isIntersectWithSphere(const Physics::Vector3 & center, float radius)
{
    Physics::Vector3 v = m_pos - center;
    float b = 2.0f * m_dir.dot(v);
    float c = v.lengthSq() - radius * radius;

    float del = b * b - 4 * c;
    if (del > 0.0)
    {
        del = sqrtf(del);
        float x1 = ( del - b) / 2.0f;
        float x2 = (-del - b) / 2.0f;
        if ((x1 < 0.0f) && (x2 < 0.0f))
        {
            return false;
        }
        return true;
    }
    return false;
}


bool cPick::isIntersectWithEntity(iEntity* pEnt)
{
    if (NULL == pEnt)
    {
        return false;
    }
    ModelPtr pModel = pEnt->getModel();
    if (NULL == pModel)
    {
        return false;
    }
    if (isIntersectWithSphere(pModel->getBBCenter()+pEnt->getPos(), pModel->getBBRadius()))
    {
        return true;
    }
    return false;
}

bool cPick::isIntersectWithTerrainObj(cTerrainObj* pObj)
{
    if (NULL == pObj)
    {
        return false;
    }
    ModelPtr pModel = pObj->getModel();
    if (NULL == pModel)
    {
        return false;
    }
    if (isIntersectWithSphere(pModel->getBBCenter()+pObj->getPos(), pModel->getBBRadius()))
    {
        return true;
    }
    return false;
}

bool cPick::queryWithEntity()
{
    iEntity* pSelEntity = NULL;
    float minDistance = Physics::MAX_FLOAT;

    if (!getEntityMgr()) return false;

    cEntityMgr::EntityPool *pool = getEntityMgr()->getPool();
    if (NULL == pool)return false;

    pool->lock(true);
    for (cEntityMgr::PoolIterator it = pool->begin(); it!=pool->end(); ++it)
    {
        if (!it->canOperate()) continue;

        iEntity* pEnt = it->obj();
        if(!pEnt->canSelect()) continue;
        
        if(!isIntersectWithEntity(pEnt)) continue;

        float distSq = m_pos.distToSq(pEnt->m_vPos);
        
        if (minDistance > distSq)
        {
            pSelEntity = pEnt;
            minDistance = distSq;
        }
    }
    pool->lock(false);

    getEntityMgr()->setActiveEntity(pSelEntity);
    return pSelEntity!= NULL;
}

bool cPick::queryWithTerrain()
{
    cMap* pMap = getMap();
    if (NULL == pMap) return false;

    return pMap->intersect(m_posOnTerrain);
}

bool cPick::queryWithTerrainObj()
{
    cMap* pMap = getMap();
    if (NULL == pMap) return false;

    return pMap->intersectWithCursor();
}
