
#include "Commen.h"
#include "Collision.h"
#include "App.h"
#include "TerrinMap.h"
#include "Mesh.h"
#include "ImpModel.h"
#include "lzdheader.h"


//////////////////////////////////////////////////////////////////////////


namespace Physics
{

    //////////////////////////////////////////////////////////////////////////

    void check2WithObj( Physics::CollisionPrevent & ci, TerrainObjPtr pobj)
    {
        if(!pobj) return;

        ModelPtr model = pobj->getModel();
        if(!model || !model->isMesh())
        {
            return;
        }

        AABB aabb;
        pobj->getWorldAABB(aabb);
        if(!aabb.intersect(ci.m_aabb)) return;

        D3DXMATRIX world;
        pobj->getAbsModelMatrix(world);

#if 0
        //二叉树碰撞检测
        BspTreePtr tree = getModelBspTree(model->source());
        if (tree)
        {
            tree->collision(ci, world);
        }

#else

        //Physics库实现碰撞检测
        ci.preventCollision(model->getMesh(), world);

#endif

    }

    
    void collisionInSpace(Physics::CollisionPrevent & ci)
    {
        MapNodePtr node = getMap()->currentNode();
        if(!node) return;

        ObjectConstIter it;
        FOR_EACH((*node), it)
        {
            check2WithObj(ci, *it);
        }
    }

    bool preventCollision(Physics::CollisionPrevent & ci)
    {
        collisionInSpace(ci);
        return ci.m_isCollisiond;

        if (ci.m_isCollisiond)
        {
            float dist = ci.m_distance-10.0f;
            if (dist <= 0.0f)
            {
                ci.m_end = ci.m_start;
            }
            else
            {
                ci.m_end = ci.m_start + ci.m_look*dist;
            }
            return true;
        }
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    bool _pickRay(RayCollision & rc, TerrainObjPtr pobj)
    {
        if(!pobj) return false;

        ModelPtr model = pobj->getModel();
        if(!model || !model->isMesh())
        {
            return false;
        }

        float dist = rc.m_start.distTo(pobj->getBBCenter());
        if( dist - pobj->getBBRadius() >= rc.m_hitDistance)
        {
            return false;
        }

        AABB aabb;
        pobj->getWorldAABB(aabb);
        if(!aabb.intersectsRay(rc.m_start, rc.m_dir)) return false;

        D3DXMATRIX world;
        pobj->getAbsModelMatrix(world);
        return rc.pick(model->getMesh(), world);
    }

    bool pickRay(RayCollision & rc)
    {
        MapNodePtr node = getMap()->currentNode();
        if (!node) return false;

        ObjectConstIter it;
        FOR_EACH((*node), it)
        {
            _pickRay(rc, *it);
        }

        return rc.isCollied();
    }

}//namespace Physics