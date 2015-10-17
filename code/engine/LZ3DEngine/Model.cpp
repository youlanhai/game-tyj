#include "Commen.h"
#include "Model.h"
#include "Res.h"
#include "Utilities.h"
#include "RenderState.h"

MeshHolder iModel::s_pMeshSphere;

bool iModel::createSphereMesh()
{
    if (s_pMeshSphere.valid())
    {
        return true;
    }
    LPD3DXMESH mesh = NULL;
    if(FAILED(D3DXCreateSphere(getResMgr()->getDevice(), 1.0f, 20, 20, &mesh, NULL)))
    {
        return false;
    }
    s_pMeshSphere.setMesh(mesh);
    return true;
}

///构造函数
iModel::iModel(void)
    : m_yaw(0.0f)
    , m_pitch(0.0f)
    , m_roll(0.0f)
    , m_scale(1.0f)
    , m_bShowBound(false)
{
    D3DXMatrixIdentity(&m_matWorld); //< 世界矩阵

    m_aabb.min.set(-10, -10, -10);
    m_aabb.max.set(10, 10, 10);

    MEMORY_CHECK_CONS;
}

///析构函数
iModel::~iModel()
{
    MEMORY_CHECK_DEST;
}



void iModel::renderBoundingBox(IDirect3DDevice9 * pDevice)
{
    if (!s_pMeshSphere.valid())
    {
        if(!createSphereMesh())
        {
            return;
        }
    }
    
    D3DXMATRIX mat, rote, matTrans;
    D3DXMatrixRotationYawPitchRoll(&rote, m_yaw, m_pitch, m_roll);
    
    float ds = getBBRadius();
    D3DXMatrixScaling(&mat, ds, ds, ds);
    
    D3DXVECTOR3 center = getBBCenter();
    
    D3DXMatrixTranslation(&matTrans, center.x, center.y, center.z);
    mat = rote*mat*matTrans*m_matWorld;
    
    pDevice->SetTransform(D3DTS_WORLD, &mat);
    pDevice->SetTexture(0, NULL);

    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME );

    s_pMeshSphere.getMesh()->DrawSubset(0);

    pDevice->SetRenderState(D3DRS_FILLMODE, DefaultRS::FillMode);
    pDevice->SetRenderState(D3DRS_LIGHTING, DefaultRS::LightingEnable);
}


void iModel::render(IDirect3DDevice9 * pDevice)
{
    if (m_bShowBound)
    {
        renderBoundingBox(pDevice);
    }
}

void iModel::getMatrix(D3DXMATRIX & mat) const
{
    D3DXMATRIX rote;
    D3DXMatrixRotationYawPitchRoll(&rote, m_yaw, m_pitch, m_roll);
    D3DXMatrixScaling(&mat, m_scale, m_scale, m_scale);
    mat = rote * mat * m_matWorld;
}

void iModel::getTransformdAABB(Physics::AABB & aabb) const
{
    D3DXMATRIX mat;
    D3DXMATRIX rote;

    D3DXMatrixRotationYawPitchRoll(&rote, m_yaw, m_pitch, m_roll);
    D3DXMatrixScaling(&mat, m_scale, m_scale, m_scale);
    mat = rote * mat;

    m_aabb.applyMatrix(aabb, mat);
}