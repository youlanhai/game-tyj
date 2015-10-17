#pragma once

#include "TerrinData.h"
#include "RenderTask.h"
#include "TerrainObj.h"
#include "TerrainNode.h"

//////////////////////////////////////////////////////////////////////////
class LZDLL_API cMap : public IBase, public IRenderObj
{
public:
    typedef std::vector<MapNodePtr>       NodeContainer;
    typedef NodeContainer::iterator             NodeIterator;

    cMap();
    ~cMap(void);

public:
    static cMap * instance();

    bool init(IDirect3DDevice9* pDevice);
    void fini();

    IDirect3DDevice9* getDevice(){ return m_pd3dDevice; }

    ///更新
    void render(IDirect3DDevice9* pDevice);

    ///渲染
    void update(float elapse);

    /** 根据地图文件名，加载地图。地图文件包含地图的高度图，行列等信息。*/
    bool loadMap(std::string mapName);
    void saveMap(const std::string & nameName);

    /** 地图将以该source进行优化绘制。source往往是玩家。*/
    void setSource(I3DObject* pSource){ m_pSource = pSource; }

    /** 是否在地图范围。*/
    bool isInBound(float x, float z);

    /** 获得物理高度*/
    float getHeight(float x, float z);

    ///获得地形数据
    TerrinData* getTerrinData(void){ return m_pTData.get(); }

    const std::string & getMapName() const { return m_mapName; }

    //数据类型转换
    void doDataConvert();

    ///地图是否可用
    bool isUserfull() const { return m_usefull; }

public:
    void setShowLevel(int level);

    int getShowLevel(void){ return m_showLevel; }

    float xMin();

    float xMax();

    float zMin();

    float zMax();

    float width();

    float height();

    ///求鼠标射线与地表交点
    bool intersect(Physics::Vector3 & position);

    MapNodePtr currentNode();

    void setLoadingProgress(float p){m_loadingProgress = p; }
    float getLoadingProgress() const { return m_loadingProgress;}

    NodeIterator begin(){ return m_mapNodes.begin(); }
    NodeIterator end(){ return m_mapNodes.end(); }

    int nodeRows() const { return m_nodeR; }
    int nodeCols() const { return m_nodeC; }
    float nodeSize() const { return m_nodeSize; }

    /** 根据索引获得地图结点。*/
    cMapNode* getNode(int r, int c);

    /** 根据坐标取得地图结点。*/
    cMapNode* getNode(float x, float z);

    /** 根据真实位置得到子结点索引。x:为列，y:为行。*/
    Square2 getNodeIndex(float x, float z);

    /** 获得地图索引。*/
    Square2 getSquareIndex(float x, float z);

protected:
    /** 释放资源。*/
    void release(void);

    virtual void createNode(void);//创建顶点索引

    /** 生成邻要绘制结点。*/
    void genRenderNode(void);

public://地图物件相关

    ///加载地图物件
    bool loadObj(const std::string & filename);

    ///保存地图物件
    bool saveObj();

    ///创建一个地图物件
    TerrainObjPtr createObj();

    ///将物件添加到地图
    void addObj(TerrainObjPtr pObj);

    ///移除一个物件
    void removeObj(TerrainObjPtr pObj);

    ///刷新物件状态。常用于位置改变后，重新分配到所在地图结点。
    void refreshObj(TerrainObjPtr pObj);

    ///根据id查找物件
    TerrainObjPtr terrainObj(int id);

public://拾取相关

    void handeMouseEvent(UINT msg, WPARAM wParam, LPARAM lParam );

    bool intersectWithCursor();

    TerrainObjPtr getActiveObj(){ return m_pActiveObj; }

    void setActiveObj(TerrainObjPtr pObj);

    TerrainObjPtr getSelectObj(){ return m_pSelectObj; }

protected://顶点无关

    NodeContainer       m_mapNodes;
    NodeContainer       m_renderNodes;//9格
    IDirect3DDevice9*   m_pd3dDevice;
    I3DObject*          m_pSource;
    RefPtr<TerrinData>  m_pTData;
    std::string         m_textureName;
    std::string         m_mapName;

    float   m_nodeSize;     //< 每个结点的尺寸。
    int     m_nodeR;        //< 结点行数
    int     m_nodeC;        //< 结点列数
    
    int     m_showLevel;    //< 显示级别
    bool    m_usefull;      //< 是否可使用

    std::string  m_objFileName;
    TerrainObjPtr m_pActiveObj;//<活动的OBJ，即拥有鼠标焦点
    TerrainObjPtr m_pSelectObj;//<被选择的Obj
    bool         m_objOnGround;//< 物体是否贴地
    
    float       m_loadingProgress;
};

LZDLL_API cMap* getMap(void);
