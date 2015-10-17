
#include "Commen.h"
#include "Utilities.h"
#include "TerrinMap.h"
#include "Pick.h"
#include "Physics.h"
#include "CursorCamera.h"

#include "Entity.h"
#include "App.h"

#define USE_ENTITY_POOL 1
#define FLOAT_MAX 999999.0f

bool g_entitySphereEnable = true;

LZDLL_API bool isEntitySphereEnable()
{
    return g_entitySphereEnable;
}
LZDLL_API void setEntitySphereEnable(bool s)
{
    g_entitySphereEnable = s;
}
 
LZDLL_API UINT convertMsg(UINT w_msg)
{
    switch(w_msg)
    {
    case  WM_LBUTTONDOWN :
        return CM_LDOWN;
    case WM_LBUTTONUP :
        return CM_LUP;
    case WM_RBUTTONUP:
        return CM_RUP;
    case WM_RBUTTONDOWN:
        return CM_RDOWN;
    case WM_MOUSELEAVE:
        return CM_LEAVE;
    default:
        break;
    }
    return w_msg;
}

//////////////////////////////////////////////////////////////////////////
static int  g_idAllocator = 1000;
///生成id
int generateID(void)
{
    return g_idAllocator++;
}

//////////////////////////////////////////////////////////////////////////
iEntity::iEntity()
{
    m_id = generateID();
    m_showDistance = 0;
    m_bEnableSphereShow = true;
    MEMORY_CHECK_CONS;
}

iEntity::~iEntity()
{
    m_model = NULL;
    m_physics = NULL;
    m_topboard = NULL;
    
    MEMORY_CHECK_DEST;
}

void iEntity::update(float elapse)
{
    if (m_physics)
    {
        m_physics->update(elapse);
    }
    if (m_model)
    {
        m_model->update(elapse);
    }
}

void iEntity::render(IDirect3DDevice9* pDevice)
{
    if (!visible())
    {
        return ;
    }
    if (m_topboard)
    {
        m_topboard->render(pDevice);
    }
    if (m_model)
    {
        m_model->setWorldMatrix(getRotationMatrix());
        m_model->render(pDevice);
    }
#ifdef _DEBUG
    if(m_physics)
    {
        m_physics->render(pDevice);
    }
#endif
}

bool iEntity::isActive(void)
{
    //return (cEntityMgr::instance()->getActiveEntity() == this);
    return false;
}

void iEntity::focusCursor(UINT msg)
{
    if (m_model && isEntitySphereEnable())
    {
        if (msg==CM_ENTER || msg==CM_LUP || msg ==CM_RUP)
        {
            m_model->showBound(true);
        }
        else if (msg==CM_LEAVE || msg==CM_LDOWN || msg==CM_RDOWN)
        {
            m_model->showBound(false);
        }
    }
    onFocusCursor(msg);
}

void iEntity::onFocusCursor(UINT msg)
{

}


float iEntity::distToCamera(void)
{
    if (!getCamera())
    {
        return FLOAT_MAX;
    }
    D3DXVECTOR3 vec = getPos() - getCamera()->position();
    return D3DXVec3Length(&vec);
}

float iEntity::distToPlayer(void)
{
    if (!getEntityMgr() || !getEntityMgr()->player())
    {
        return FLOAT_MAX;
    }
    D3DXVECTOR3 vec = getPos() - getEntityMgr()->player()->getPos();
    return D3DXVec3Length(&vec);
}

void iEntity::lookAtPosition(const Physics::Vector3 & pos)
{
    m_vLook = pos -  m_vPos;
    m_vLook.normalize();

    m_vUp.set(0, 1, 0);

    D3DXVec3Cross(&m_vRight, &m_vUp, &m_vLook);
    m_vRight.normalize();
}

//////////////////////////////////////////////////////////////////////////

struct PredicateID
{
    int m_id;
    PredicateID(int id)
    {
        m_id = id;
    }
    bool operator()(const VisitPool<iEntity>::VisitNode & ent)
    {
        if (ent.obj())
        {
            return ent.obj()->getID() == m_id;
        }
        else
        {
            return false;
        }
    }
};
//////////////////////////////////////////////////////////////////////////
cEntityMgr* g_pEntityPool_ = NULL;
LZDLL_API cEntityMgr* getEntityMgr()
{
    return g_pEntityPool_;
}
cEntityMgr::cEntityMgr(void)
{
    MEMORY_CHECK_CONS;
    g_pEntityPool_= this;
    m_pActiveEntity = NULL;
    m_player = NULL;
    getApp()->addUpdateRender(this);
}

cEntityMgr::~cEntityMgr(void)
{
    getApp()->removeUpdateRender(this);
    m_pool.clear();
    MEMORY_CHECK_DEST;
}

void cEntityMgr::add(iEntity* pEnt)
{
    m_pool.add(pEnt);
}

void cEntityMgr::remove(iEntity* pEnt)
{
    if (pEnt == m_pActiveEntity)
    {
        m_pActiveEntity = NULL;
    }
    m_pool.remove(pEnt);
}

void cEntityMgr::clear()
{
    m_pActiveEntity = NULL;
    m_player = NULL;
    m_pool.clear();
}

iEntity* cEntityMgr::entity(int id)
{
    PoolIterator it = m_pool.find_if(PredicateID(id));
    if (it == m_pool.end())
    {
        return NULL;
    }
    return it->obj();
}

void cEntityMgr::setActiveEntity(iEntity* pEnt)
{
    if (m_pActiveEntity == pEnt)
    {
        return;
    }
    else
    {
        if (m_pActiveEntity)
        {
            m_pActiveEntity->focusCursor(CM_LEAVE);
        }
        m_pActiveEntity = pEnt;
        if (m_pActiveEntity)
        {
            m_pActiveEntity->focusCursor(CM_ENTER);
        }
    }
}

UINT cEntityMgr::handleMouseEvent(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg==WM_LBUTTONDOWN || \
        msg==WM_LBUTTONUP || \
        msg==WM_RBUTTONDOWN || \
        msg==WM_RBUTTONUP)
    {
        if (getActiveEntity() != NULL)
        {
            getActiveEntity()->focusCursor(convertMsg(msg));
            return 1;
        }
    }
    return 0;
}

void cEntityMgr::update(float fElapse)
{
    m_pool.update(fElapse);
}

void cEntityMgr::render(IDirect3DDevice9 * pDevice)
{
    m_pool.render(pDevice);
}