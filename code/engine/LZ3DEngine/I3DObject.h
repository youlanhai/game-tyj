#pragma once

#include "Base.h"
#include "RenderObj.h"
#include "../Physics/math3d.h"

//////////////////////////////////////////////////////////////////////////

struct LZDLL_API I3DObject : public IBase, public IRenderObj
{
public:
	I3DObject(void);
    ~I3DObject();

	void moveLook(float fElapse);	//前进
	void moveUp(float fElapse);		//上升
	void moveRight(float fElapse);	//平移

	void rotationUp(float angle);		//绕up旋转
	void rotationLook(float angle);	//绕Look旋转
	void rotationRight(float angle);	//绕right旋转

    /*绕给定向量旋转*/
	void rotationAxis(const Physics::Vector3 & a, float angle );

    /*绕X、Y、Z轴旋转*/
    void rotationX(float angle);
    void rotationY(float angle);
    void rotationZ(float angle);

    //重置方向
    void resetdir();

    virtual void getWorldMatrix(Physics::Matrix4x4 & mat) const;//获得世界矩阵
    virtual void getCameraMatrix(Physics::Matrix4x4 & mat) const;//获得相机矩阵

	virtual Physics::Matrix4x4 getRotationMatrix();//获得旋转矩阵
	virtual Physics::Matrix4x4 getRotationMatrix2();
    virtual void normalizeVector(void);

    virtual void update(float elapse){}
    virtual void render(IDirect3DDevice9* pDevice){}

public://属性
	const Physics::Vector3& getPos(void) const { return m_vPos; }
	void setPos( const Physics::Vector3 &pos ) { m_vPos = pos; }

	const Physics::Vector3& getLook(void ) const { return m_vLook; }
	void setLook( const Physics::Vector3 &look ) { m_vLook = look; }

	const Physics::Vector3& getUp( void ) const { return m_vUp; }
	void setUp( const Physics::Vector3 &up ) { m_vUp = up; }

	const Physics::Vector3& getRight( void ) const { return m_vRight; }
	void setRight( const Physics::Vector3 &right ) { m_vRight = right; }

    const Physics::Vector3 & getScale() const { return m_vScale; }
    void setScale( float s){ m_vScale.set(s, s, s); }
    void setScale(const Physics::Vector3 &v){ m_vScale = v; }
    
    const Physics::Vector3 & getSpeed() const { return m_vSpeed; }
    void setSpeed(float s){ m_vSpeed.set(s, s, s); }
    void setSpeed(const Physics::Vector3 &v){ m_vSpeed = v; }

    const Physics::Vector3 & getRotSpeed() const { return m_vRotSpeed; }
    void setRotSpeed(float s){ m_vRotSpeed.set(s, s, s);}
    void setRotSpeed(const Physics::Vector3 &v){ m_vRotSpeed = v;}

    const Physics::AABB & getAABB() const { return m_aabb; }
    void setAABB(const Physics::AABB & a){ m_aabb = a;}
    void getWorldAABB(Physics::AABB & a) const;

public:
    /** 是否是玩家*/
    virtual bool isPlayer(void){ return false; }

    bool visible(void){ return m_visible; }
    virtual void show(bool s){ m_visible = s; }

    void enableLockHeight(bool l){ m_lockHInMap = l; }
    void setLockHeight(float h){ m_lockHeight = h ;}

public:
	Physics::Vector3    m_vPos;			//位置
	Physics::Vector3    m_vLook;		//朝向，对应z轴
	Physics::Vector3    m_vUp;			//上向量,对应y轴
	Physics::Vector3    m_vRight;		//右向量，对应x轴
    Physics::Vector3    m_vRotSpeed;    //旋转速度
    Physics::Vector3    m_vScale;       //缩放
    Physics::Vector3    m_vSpeed;       //移动速度
    Physics::AABB       m_aabb;         //包围盒

protected:
    bool    m_visible;
    bool    m_lockHInMap;
    float   m_lockHeight;
};

//////////////////////////////////////////////////////////////////////////
class LZDLL_API I3DObjectFPS : public I3DObject
{
public:
    I3DObjectFPS(void);

    void addAngleLook(float angle);
    void addAngleRight(float angle);
    void addAngleUp(float angle);
    void addAngleX(float angle);
    void addAngleY(float angle);
    void addAngleZ(float angle);
    /*更新矩阵。根据旋转角度，重置look，right，up向量*/
    void updateMatrix(void);

public:

 /*绕各个轴的选择角度*/
    float m_fRotAngLook;//绕look方向的旋转角度
    float m_fRotAngRight;
    float m_fRotAngUp;
    float m_fRotAngX;
    float m_fRotAngY;
    float m_fRotAngZ;
};