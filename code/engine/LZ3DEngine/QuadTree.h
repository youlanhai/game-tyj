#pragma once
/*  基于四叉树分隔思想，二叉树数据结构实现的四叉树。
 */

#include "../Physics/math3d.h"

namespace qtree
{

    typedef std::vector<Physics::FRect> RectArray;
    typedef std::vector<size_t>     IndicesArray;

    typedef RefPtr<class RayCollider> RayColliderPtr;

    class RayCollider : public IBase
    {
    public:
        Physics::Vector2 origin;
        Physics::Vector2 dir;
        float distance;


        virtual RayColliderPtr clone() const = 0;
        virtual bool isCollid(const IndicesArray & indices, const RectArray & rects) const = 0;
    };

    ///四叉树基类
    class QuadTreeBase : public IBase
    {
    public:

        ///是否是叶结点
        virtual bool isLeaf() const { return false; }

        ///射线拾取
        virtual bool pick(RayColliderPtr collider) const = 0;

    protected:
        Physics::FRect m_rect;///<此结点所占据的区域
    };

    typedef RefPtr<QuadTreeBase> QuadTreePtr;

    ///四叉树叶结点
    class QuadTreeLeaf : public QuadTreeBase
    {
    public:
        virtual bool isLeaf() const { return true; }

        virtual bool pick(RayColliderPtr collider) const
        {
            //TODO: 先检测本区域是否与射线相交。

            return collider->isCollid(m_indices, m_rects);
        }

    protected:
        IndicesArray m_indices;
        const RectArray & m_rects;
    };

    class QuadTreeNode : public QuadTreeBase
    {
    public:

        static bool build(QuadTreePtr & child,
                const Physics::FRect & rect, 
                const IndicesArray & indices, 
                const RectArray & rects,
                int depth);

        virtual bool pick(RayColliderPtr collider) const
        {
            //TODO: 先检测本区域是否与射线相交。

            if(collider->origin[m_splitAxis] > m_splitPos)
            {
                if(m_front && m_front->pick(collider)) return true;
                if(m_back && m_back->pick(collider)) return true;
            }
            else
            {
                if(m_back && m_back->pick(collider)) return true;
                if(m_front && m_front->pick(collider)) return true;
            }

            return false;
        }

    private:
        float   m_splitPos;
        int     m_splitAxis;

        QuadTreePtr m_front;
        QuadTreePtr m_back;
    };

    class QuadTree: QuadTreeBase
    {
    public:
        QuadTree(void);
        ~QuadTree(void);
    };

}//end namespace qtree
