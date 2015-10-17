#pragma once

#include "I3DObject.h"
#include "Model.h"
#include "Pick.h"
#include "Physics.h"
#include "RenderTask.h"
#include "TopBoard.h"

enum CursorMsg
{
    CM_ENTER   = 1,
    CM_LEAVE   = 2,
    CM_LDOWN   = 3,
    CM_LUP     = 4,
    CM_RDOWN   = 5,
    CM_RUP     = 6,
    CM_FORCE_WORD = 0x7fffffff
};


LZDLL_API UINT convertMsg(UINT w_msg);


class LZDLL_API iEntity : public I3DObject
{
public:
    iEntity();
    ~iEntity();

    virtual void update(float elapse);

    virtual void render(IDirect3DDevice9* pDevice);

    ModelPtr getModel(void){ return m_model; }
    void setModel(ModelPtr pModel){ m_model = pModel; }

    int getID(void){ return m_id; }

    bool isActive(void);

    ///是否可以被鼠标选择
    virtual bool canSelect(void){ return true; }

    ///处理鼠标消息
    virtual void focusCursor(UINT msg);

    ///接收到了鼠标消息
    virtual void onFocusCursor(UINT msg);

    ///到相机的距离
    float distToCamera(void);

    ///到主角的距离
    float distToPlayer(void);

    ///朝向某点
    void lookAtPosition(const Physics::Vector3 & pos);

    ///物理系统
    void setPhysics(PhysicsPtr p){ m_physics = p; }
    PhysicsPtr getPhysics(void){ return m_physics; }

    ///头顶板
    void setTopboard(cTopBoard* pboard){ m_topboard = pboard; }
    cTopBoard* getTopboard(void){ return m_topboard.get(); }

    ///可见距离。如果超出可见范围外，将不可见。
    void setShowDistance(float dist){ m_showDistance = dist; }
    float getShowDistance(void){ return m_showDistance; }

    ///显示包围求
    void enableSphereShow(bool s){ m_bEnableSphereShow = s; }
    bool isSphereShowEnable(void){ return m_bEnableSphereShow; }

protected:
    int                 m_id;
    ModelPtr            m_model;
    PhysicsPtr          m_physics;
    RefPtr<cTopBoard>   m_topboard;
    float               m_showDistance;//< 最大显示距离（据离玩家）
    bool                m_bEnableSphereShow;//< 在活动时，是否显示包围球
};

typedef RefPtr<iEntity> EntityPtr;
//////////////////////////////////////////////////////////////////////////
class LZDLL_API cEntityMgr : public IRenderObj, public IBase
{
public:
    typedef VisitPool<iEntity>     EntityPool;
    typedef EntityPool::iterator    PoolIterator;

    cEntityMgr(void);

    ~cEntityMgr(void);

    iEntity* entity(int id);

    iEntity* getActiveEntity(void){ return m_pActiveEntity; }

    UINT handleMouseEvent(UINT msg, WPARAM wParam, LPARAM lParam);

    EntityPool* getPool(void){ return &m_pool; }

    iEntity* player(){ return m_player; }

    void setPlayer(iEntity* p){ m_player = p ;}
public:

    void add(iEntity* pEnt);

    void remove(iEntity* pEnt);

    void clear();

    virtual void update(float fElapse);

    virtual void render(IDirect3DDevice9 * pDevice);

    void setActiveEntity(iEntity* pEnt);

protected:
    EntityPool      m_pool;
    iEntity*        m_pActiveEntity;
    iEntity*        m_player;
};

LZDLL_API cEntityMgr* getEntityMgr();
LZDLL_API bool isEntitySphereEnable();
LZDLL_API void setEntitySphereEnable(bool s);