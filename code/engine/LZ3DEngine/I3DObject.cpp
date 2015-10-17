#include "Commen.h"
#include "Utilities.h"
#include "App.h"
#include "CursorCamera.h"
#include "TerrinMap.h"

#include "I3DObject.h"

I3DObject::I3DObject(void)
    : m_vPos(0.0f, 0.0f, 0.0f)
    , m_vLook(0.0f, 0.0f, 1.0f)
    , m_vRight(1.0f, 0.0f, 0.0f)
    , m_vUp(0.0f, 1.0f, 0.0f)
    , m_vSpeed(100.0f, 100.0f, 100.0f)
    , m_vScale(1.0f, 1.0f, 1.0f)
    , m_vRotSpeed(1.0f, 1.0f, 1.0f)
    , m_visible(true)
    , m_lockHInMap(false)
    , m_lockHeight(5.0f)
{
}

I3DObject::~I3DObject()
{
}

void I3DObject::resetdir()
{
    m_vLook.set(0.0f, 0.0f, 1.0f);
    m_vRight.set(1.0f, 0.0f, 0.0f);
    m_vUp.set(0.0f, 1.0f, 0.0f);
}

void I3DObject::normalizeVector(void)
{
    D3DXVec3Normalize(&m_vLook, &m_vLook);
    D3DXVec3Cross(&m_vRight, &m_vUp, &m_vLook);
    D3DXVec3Normalize(&m_vRight, &m_vRight);
    D3DXVec3Cross(&m_vUp, &m_vLook, &m_vRight);
    D3DXVec3Normalize(&m_vUp, &m_vUp);
}

void I3DObject::moveLook(float fElapse)		//前进
{
	m_vPos += m_vLook*(m_vSpeed.z*fElapse);
}

void I3DObject::moveUp(float fElapse)			//上升
{
	m_vPos += m_vUp*(fElapse*m_vSpeed.y);
}

void I3DObject::moveRight(float fElapse)	//平移
{
	m_vPos += m_vRight*(m_vSpeed.x*fElapse);
}

void I3DObject::rotationUp(float angle)		//绕up旋转
{
	Physics::Matrix4x4 t;
	D3DXMatrixRotationAxis(&t, &m_vUp, angle);
	D3DXVec3TransformCoord(&m_vRight, &m_vRight, &t);
	D3DXVec3TransformCoord(&m_vLook, &m_vLook, &t);

    normalizeVector();
}
void I3DObject::rotationLook(float angle)	//绕Look旋转
{
	Physics::Matrix4x4 t;
	D3DXMatrixRotationAxis(&t, &m_vLook, angle);
	D3DXVec3TransformCoord(&m_vRight, &m_vRight, &t);
	D3DXVec3TransformCoord(&m_vUp, &m_vUp, &t);

    normalizeVector();
}
void I3DObject::rotationRight(float angle)	//绕right旋转
{
	Physics::Matrix4x4 t;
	D3DXMatrixRotationAxis(&t, &m_vRight, angle);
	D3DXVec3TransformCoord(&m_vUp, &m_vUp, &t);
	D3DXVec3TransformCoord(&m_vLook, &m_vLook, &t);

    normalizeVector();
}


void I3DObject::rotationAxis(const Physics::Vector3 & a, float angle )
{
	Physics::Matrix4x4 t;
	D3DXMatrixRotationAxis(&t, &a, angle);
	D3DXVec3TransformCoord(&m_vRight, &m_vRight, &t);
	D3DXVec3TransformCoord(&m_vLook, &m_vLook, &t);
	D3DXVec3TransformCoord(&m_vUp, &m_vUp, &t);

    normalizeVector();
}

/*绕X、Y、Z轴旋转*/
void I3DObject::rotationX(float angle)
{
    Physics::Matrix4x4 t;
    D3DXMatrixRotationX(&t,angle);
    D3DXVec3TransformCoord(&m_vLook,&m_vLook,&t);
    D3DXVec3TransformCoord(&m_vRight,&m_vRight,&t);
    D3DXVec3TransformCoord(&m_vUp,&m_vUp,&t);

    normalizeVector();
}

void I3DObject::rotationY(float angle)
{
    Physics::Matrix4x4 t;
    D3DXMatrixRotationY(&t,angle);
    D3DXVec3TransformCoord(&m_vLook,&m_vLook,&t);
    D3DXVec3TransformCoord(&m_vRight,&m_vRight,&t);
    D3DXVec3TransformCoord(&m_vUp,&m_vUp,&t);

    normalizeVector();
}

void I3DObject::rotationZ(float angle)
{
    Physics::Matrix4x4 t;
    D3DXMatrixRotationZ(&t,angle);
    D3DXVec3TransformCoord(&m_vLook,&m_vLook,&t);
    D3DXVec3TransformCoord(&m_vRight,&m_vRight,&t);
    D3DXVec3TransformCoord(&m_vUp,&m_vUp,&t);

    normalizeVector();
}

void I3DObject::getWorldMatrix(Physics::Matrix4x4 & mat) const//获得世界矩阵
{
    mat._11 = m_vRight.x;	mat._21 = m_vUp.x;	 mat._31 = m_vLook.x;	mat._14 = 0.0f;
	mat._12 = m_vRight.y;	mat._22 = m_vUp.y;	 mat._32 = m_vLook.y;	mat._24 = 0.0f;
	mat._13 = m_vRight.z;	mat._23 = m_vUp.z;	 mat._33 = m_vLook.z;	mat._34 = 0.0f;
	mat._41 = m_vPos.x;		mat._42 = m_vPos.y;		 mat._43 = m_vPos.z;			mat._44 = 1.0f;
    
#if 0
    if (m_fScale != 1.0f)
    {
        Physics::Matrix4x4 matScale;
        D3DXMatrixScaling(&matScale,m_fScale,m_fScale,m_fScale);
        mat = matScale * mat;
    }
#else
    for(int i=0; i<3; ++i)
    {
        for(int k=0; k<3; ++k)
        {
            mat.m[i][k] *= m_vScale[i];
        }
    }
#endif
}


void I3DObject::getCameraMatrix(Physics::Matrix4x4 & mat) const
{
    float x = -D3DXVec3Dot(&m_vPos, &m_vRight);
    float y = -D3DXVec3Dot(&m_vPos, &m_vUp);
    float z = -D3DXVec3Dot(&m_vPos, &m_vLook);

    mat._11 = m_vRight.x;	mat._12 = m_vUp.x;	 mat._13 = m_vLook.x;	mat._14 = 0.0f;
    mat._21 = m_vRight.y;	mat._22 = m_vUp.y;	 mat._23 = m_vLook.y;	mat._24 = 0.0f;
    mat._31 = m_vRight.z;	mat._32 = m_vUp.z;	 mat._33 = m_vLook.z;	mat._34 = 0.0f;
    mat._41 = x;			mat._42 = y;		 mat._43 = z;			mat._44 = 1.0f;
}

Physics::Matrix4x4 I3DObject::getRotationMatrix()
{
    Physics::Matrix4x4 mat;
    normalizeVector();

	float x=m_vPos.x;
	float y=m_vPos.y;
	float z=m_vPos.z;

    mat._11 = m_vRight.x;	mat._21 = m_vUp.x;	 mat._31 = m_vLook.x;	mat._14 = 0.0f;
	mat._12 = m_vRight.y;	mat._22 = m_vUp.y;	 mat._32 = m_vLook.y;	mat._24 = 0.0f;
	mat._13 = m_vRight.z;	mat._23 = m_vUp.z;	 mat._33 = m_vLook.z;	mat._34 = 0.0f;
	mat._41 = x;			mat._42 = y;		 mat._43 = z;			mat._44 = 1.0f;
    
    Physics::Matrix4x4 matScale;
    D3DXMatrixScaling(&matScale,m_vScale.x,m_vScale.y,m_vScale.z);
    mat = matScale * mat;

    return mat;
}

Physics::Matrix4x4 I3DObject::getRotationMatrix2()
{
    Physics::Matrix4x4 mat;
    normalizeVector();

    float x = -D3DXVec3Dot(&m_vPos, &m_vRight);
    float y = -D3DXVec3Dot(&m_vPos, &m_vUp);
    float z = -D3DXVec3Dot(&m_vPos, &m_vLook);

    mat._11 = m_vRight.x;	mat._12 = m_vUp.x;	 mat._13 = m_vLook.x;	mat._14 = 0.0f;
    mat._21 = m_vRight.y;	mat._22 = m_vUp.y;	 mat._23 = m_vLook.y;	mat._24 = 0.0f;
    mat._31 = m_vRight.z;	mat._32 = m_vUp.z;	 mat._33 = m_vLook.z;	mat._34 = 0.0f;
    mat._41 = x;			mat._42 = y;		 mat._43 = z;			mat._44 = 1.0f;
    return mat;
}


void I3DObject::getWorldAABB(Physics::AABB & aabb) const
{
    Physics::Matrix4x4 mat;
    getWorldMatrix(mat);
    m_aabb.applyMatrix(aabb, mat);
}

//////////////////////////////////////////////////////////////////////////

I3DObjectFPS::I3DObjectFPS(void)
{
    m_fRotAngLook = 0.0f;//绕look方向的旋转角度
    m_fRotAngRight = 0.0f;
    m_fRotAngUp = 0.0f;
    m_fRotAngX = 0.0f;
    m_fRotAngY = 0.0f;
    m_fRotAngZ = 0.0f;
}

void I3DObjectFPS::addAngleLook(float angle)
{
    m_fRotAngLook += angle;
}

void I3DObjectFPS::addAngleRight(float angle)
{
    m_fRotAngRight += angle;

    float bound = D3DX_PI/3; //旋转界限
    if (m_fRotAngRight > bound)
    {
        m_fRotAngRight = bound;
    }
    else if (m_fRotAngRight < -bound)
    {
        m_fRotAngRight = -bound;
    }
}

void I3DObjectFPS::addAngleUp(float angle)
{
    m_fRotAngUp += angle;
}

void I3DObjectFPS::addAngleX(float angle)
{
    m_fRotAngX += angle;
}

void I3DObjectFPS::addAngleY(float angle)
{
    m_fRotAngY += angle;
}

void I3DObjectFPS::addAngleZ(float angle)
{
    m_fRotAngZ += angle;
}


void I3DObjectFPS::updateMatrix(void)
{
    m_vLook = Physics::Vector3(0.0f, 0.0f, 1.0f);
    m_vRight = Physics::Vector3(1.0f, 0.0f, 0.0f);
    m_vUp = Physics::Vector3(0.0f, 1.0f, 0.0f);
    rotationLook(m_fRotAngLook);
    rotationRight(m_fRotAngRight);
    rotationUp(m_fRotAngUp);
    rotationX(m_fRotAngX);
    rotationY(m_fRotAngY);
    rotationZ(m_fRotAngZ);
}
