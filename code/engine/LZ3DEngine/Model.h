#pragma once

#include "Base.h"
#include "RenderObj.h"
#include "../Physics/math3d.h"

class LZDLL_API MeshHolder
{
public:
    MeshHolder() : m_mesh(NULL) {}
    ~MeshHolder() {  SafeRelease(m_mesh); }

    void setMesh(LPD3DXMESH mesh)
    {
        SafeRelease(m_mesh);
        m_mesh = mesh;
    }

    LPD3DXMESH getMesh(){ return m_mesh; }

    bool valid(){ return m_mesh != NULL; }

private:
    LPD3DXMESH m_mesh;
};

typedef RefPtr<class iModel>  ModelPtr;

///模型基类
class LZDLL_API iModel : public IBase, public IRenderObj
{
public:
    ///构造函数
    iModel(void);

    ///析构函数
    ~iModel();

    ///加载模型
    virtual bool load(const std::string & name){ m_source = name; return true; }

    ///是否有骨骼
    virtual bool isSkeleton(void) const { return false; }

    ///是否是简单的mesh
    virtual bool isMesh(void) const { return false; }

    virtual void update(float fElapse) {}
    virtual void render(IDirect3DDevice9 * pDevice);

    ///获取d3d mesh。
    virtual LPD3DXMESH getMesh(){ return NULL; }

public:
    ///获取模型资源名
    const std::string & source() const { return m_source; }

    ///获取最终的矩阵
    void getMatrix(D3DXMATRIX & mat) const;

    ///获得世界矩阵
    const D3DXMATRIX & getWorldMatrix( void) const { return m_matWorld; }

    ///设置世界矩阵
    void setWorldMatrix(const D3DXMATRIX & pMat){ m_matWorld = pMat; }

    ///设置缩放
    void setScale( float s) {  m_scale = s;  }
    float getScale() const { return m_scale; }

    void setYaw(float a){ m_yaw = a; }
    float getYaw(void) const { return m_yaw; }

    void setPicth(float a){ m_pitch = a; }
    float getPitch(void) const { return m_pitch; }

    void setRoll(float a){ m_roll = a; }
    float getRoll(void) const { return m_roll; }

    const Physics::AABB & getAABB() const { return m_aabb; }
    void setAABB(const Physics::AABB & aabb){ m_aabb = aabb; }

    void getTransformdAABB(Physics::AABB & aabb) const;

//包围盒相关
public:

    void renderBoundingBox(IDirect3DDevice9 * pDevice);

    ///显示包围盒
    void showBound( bool bShow ) { m_bShowBound = bShow; }

    ///获得包围盒半径
    virtual float getBBRadius(void) const{ return 0.0f; }

    ///获得包围盒球心
    virtual D3DXVECTOR3 getBBCenter(void) const{ return D3DXVECTOR3(); }

public://动画相关

    /** 播放动画*/
    virtual bool playAction(std::string name, bool loop){ return false; }

    /** 停止动画*/
    virtual void stopAction(void){}

    /** 获得动画个数*/
    virtual int getActionCount(void) const { return 0 ; }

    /** 获得动画名称*/
    virtual std::string getActionName(int index) const { return ""; }

    virtual void setAnimSpeed(float s){}

protected:
    std::string  m_source;      ///< 模型资源名
    D3DXMATRIX  m_matWorld;     ///< 世界矩阵
    bool        m_bShowBound;   ///< 是否显示包围盒
    float       m_scale;        ///< 缩放系数
    float       m_yaw;          ///< 转角
    float       m_pitch;
    float       m_roll;
    Physics::AABB m_aabb;

    static MeshHolder s_pMeshSphere;

    static bool createSphereMesh();
};

class LZDLL_API cModelFactory : public IBase
{
public:
    static ModelPtr createObj(const std::string & fileName, const std::string & type);
    static ModelPtr createObj2(const std::string & filename, int typeID);
};

