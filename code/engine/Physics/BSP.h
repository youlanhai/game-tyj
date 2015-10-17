//////////////////////////////////////////////////////////////////////////
//bsp
//////////////////////////////////////////////////////////////////////////

#pragma

#include "Collision.h"
#include "..\LZData\lzdheader.h"
#include "..\utility\Utility.h"
#include "..\utility\DataStream.h"

namespace Physics
{
    typedef RefPtr<class BspNode> BspNodePtr;
    typedef RefPtr<class BspTree> BspTreePtr;

    //////////////////////////////////////////////////////////////////////////
    //bsp 结点
    //////////////////////////////////////////////////////////////////////////
    class BspNode : public IBase
    {
        MAKE_UNCOPY(BspNode)

    public:

        BspNode(){ }

        explicit BspNode(const Physics::Triangle & tri);

        virtual void addTriangle(const Physics::Triangle & tri, 
            const Physics::Polygon & splitPoly);

        void load(Lazy::DataStream & stream);
        void save(Lazy::DataStream & stream) const;

        bool collision(
            const Physics::Triangle & triangle,
            const Physics::Vector3 & offset,
            Physics::CollisionPrevent & ci, 
            const D3DXMATRIX & world) const;

        bool collisionThis(
            const Physics::Triangle & triangle,
            const Physics::Vector3 & offset,
            Physics::CollisionPrevent & ci, 
            const D3DXMATRIX & world) const;

        bool collision(Physics::CollisionPrevent & ci, const D3DXMATRIX & world) const;

        bool collisionThis(Physics::CollisionPrevent & ci, const D3DXMATRIX & world) const;

        void drawDebug(IDirect3DDevice9 *device) const;

    protected:

        virtual void addToChild(BspNodePtr & child, 
            const Physics::Triangle & tri,  
            const Physics::Polygon & poly);

    protected:
        Physics::Plane           m_panel;    //分割平面
        Physics::TriangleSet     m_triangleSet; //当前节点的三角形集合
        BspNodePtr      m_front;     //左子树
        BspNodePtr      m_back;    //右子树
    };

    //////////////////////////////////////////////////////////////////////////
    //BSP树
    //////////////////////////////////////////////////////////////////////////
    class BspTree : public IBase
    {
        MAKE_UNCOPY(BspTree)

    public:
        BspTree(){}
        ~BspTree(){}

        bool generateBsp(ID3DXMesh* mesh);

        bool load(const Lazy::tstring & fname);

        bool save(const Lazy::tstring & fname) const;

        bool collision(Physics::CollisionPrevent & ci, const D3DXMATRIX & world) const;

        BspNodePtr root() const { return m_root; } 

        void drawDebug(IDirect3DDevice9 *pDevice, const D3DXMATRIX * pWorld=NULL) const;

    protected:

        void addTriangle(const Physics::Triangle & tri);

        virtual BspNodePtr createBspNode(const Physics::Triangle & tri);

    protected:
        BspNodePtr m_root;
    };

    //////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////
    
    //设置bsp文件的存贮路径
    void setBspFilePath(const Lazy::tstring & path);

    const Lazy::tstring & getBspFilePath();

    //根据模型文件名，加载其对应的bsp数据。
    BspTreePtr getModelBspTree(const Lazy::tstring & modelName);

    //从文件中加载bsp数据。
    BspTreePtr loadBspTree(const Lazy::tstring & bspName);

    //////////////////////////////////////////////////////////////////////////
    //平衡二叉树。在构造bsp时使用，以优化二叉树结构。
    //////////////////////////////////////////////////////////////////////////

    class AVLBspNode;
    typedef RefPtr<AVLBspNode> AVLBspNodePtr;

    class AVLBspTree : public BspTree
    {
    public:
        virtual BspNodePtr createBspNode(const Physics::Triangle & tri);

        void setRoot(AVLBspNodePtr ptr);

        void testSave(const std::string & fname);
    };

    class AVLBspNode : public BspNode
    {
    public:
        AVLBspNode(AVLBspNode * parent) 
            : m_parent(parent)
            , m_balence(0)
            , m_tree(NULL)
        {
        }

        AVLBspNode(AVLBspTree *tree, AVLBspNode * parent, const Physics::Triangle & tri) 
            : BspNode(tri)
            , m_tree(tree)
            , m_parent(parent)
            , m_balence(0)
        {
        }

        bool isRoot(){ return m_parent==NULL; }
        bool isLeft(){ return m_parent->left() == this; }
        bool isRight(){ return m_parent->right() == this; }

        AVLBspNodePtr left()
        { 
            //assert(m_front && "no left");
            if (m_front)
            {
                return (AVLBspNodePtr)m_front; 
            }
            return NULL;
        }
        AVLBspNodePtr right()
        { 
            //assert(m_back && "no right"); 
            if(m_back)
            {
                return (AVLBspNodePtr)m_back; 
            }
            return NULL;
        }

        AVLBspNode* parent(){ return m_parent; }
        void setParent(AVLBspNode* ptr){ m_parent = ptr; }

        void setLeft(AVLBspNodePtr ptr)
        { 
            m_front = ptr;
            if (ptr)
            {
                ptr->setParent(this);
            }
        }
        void setRight(AVLBspNodePtr ptr)
        { 
            m_back = ptr;
            if (ptr)
            {
                ptr->setParent(this);
            }
        }

        void updateBalence();

        void testSave(LZDataPtr xmlRoot);

    protected:

        virtual void addToChild(BspNodePtr & child, 
            const Physics::Triangle & tri,  
            const Physics::Polygon & poly);

    public:
        int         m_balence;
        AVLBspNode  *m_parent;
        AVLBspTree  *m_tree;
    };

}//end namespace Physics