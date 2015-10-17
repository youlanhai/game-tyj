
#include "Commen.h"
#include "Utilities.h"
#include "Res.h"
#include "Model.h"
#include "Pick.h"

#include "TerrainObj.h"
#include "Entity.h"
#include "CursorCamera.h"
#include "TerrinMap.h"
#include "..\Physics\Physics.h"
#include "..\Physics\LazyConllision.h"
#include "ImpModel.h"

bool mapDataConvert = false;
bool useChunkData = false;

bool showTerrainObjAABB = false;

namespace Lazy
{

    void writeVector3(LZDataPtr dataPtr, const Lazy::tstring & tag, const Physics::Vector3 & v)
    {
        Lazy::tstring str;
        formatString(str, _T("%f %f %f"), v.x, v.y, v.z);
        dataPtr->writeString(tag, str);
    }

    void readVector3(LZDataPtr dataPtr, const Lazy::tstring & tag, Physics::Vector3 & v)
    {
        Lazy::tstring str = dataPtr->readString(tag);
        if(str.empty())
        {
            v.zero();
            return;
        }

        swscanf(str.c_str(), _T("%f %f %f"), &v.x, &v.y, &v.z);
    }

}

/*static*/ cTerrainRes* cTerrainRes::m_pInstance = NULL;

cTerrainRes::cTerrainRes()
{
    m_cache.id = -1;
    MEMORY_CHECK_CONS;
}
cTerrainRes::~cTerrainRes()
{
    MEMORY_CHECK_DEST;
}

bool cTerrainRes::load(const std::string & filename)
{
    std::ifstream fin(filename.c_str());
    if (!fin.good())
    {
        return false;
    }

    int count;
    fin>>count;
    m_resPool.resize(count);
    for (int i=0; i<count; ++i)
    {
        fin>>m_resPool[i];
    }

    return true;
}

int cTerrainRes::getType(int id)
{
    if (id != m_cache.id)
    {
        if (NULL == find(id))
        {
            return 0;
        }
    }
    return m_cache.type;
}

LPCSTR cTerrainRes::getPath(int id)
{
    if (id != m_cache.id)
    {
        if (NULL == find(id))
        {
            return 0;
        }
    }
    return m_cache.path.c_str();
}

TerrainResNode* cTerrainRes::find(int id)
{
    ResIterator it = std::find_if(
        m_resPool.begin(), 
        m_resPool.end(), 
        TR_PredID<TerrainResNode>(id));
    if (it == m_resPool.end())
    {
        return NULL;
    }
    m_cache = *it;
    return &m_cache;
}

/*static*/ cTerrainRes* cTerrainRes::instance()
{
    if (NULL == m_pInstance)
    {
        m_pInstance = new cTerrainRes();
    }
    return m_pInstance;
}

/*static*/ void cTerrainRes::release()
{
    delete m_pInstance;
}

//////////////////////////////////////////////////////////////////////////
static int g_id_allocator = 100000;

cTerrainObj::cTerrainObj(void)
    : m_collid(true)
    , m_angle(0, 0, 0)
    , m_modeID(0)
    , m_isRef(false)
    , m_refChunk(0)
{
    MEMORY_CHECK_CONS;

    m_id = g_id_allocator++;
    
    Lazy::generateGUID32(m_uuid);
    m_aabb.min.set(-10, -10, -10);
    m_aabb.max.set(10, 10, 10);
}

//克隆一个副本，此副本必定是引用的。
TerrainObjPtr cTerrainObj::clone() const
{
    return new cTerrainObj(*this);
}

cTerrainObj::~cTerrainObj(void)
{
    MEMORY_CHECK_DEST;
}

void cTerrainObj::update(float elapse)
{
    updateRotationAxis();
}

void cTerrainObj::render(IDirect3DDevice9* pDevice)
{
    if (!visible())
    {
        return ;
    }

    if (!m_model) return;

    D3DXMATRIX matWorld;
    getWorldMatrix(matWorld);
    m_model->setWorldMatrix(matWorld);

    if(!m_isRef)
    {
        m_model->render(pDevice);

        Physics::AABB aabb;
        getWorldAABB(aabb);

        if(showTerrainObjAABB)
        {
            D3DXMatrixIdentity(&matWorld);
            pDevice->SetTransform(D3DTS_WORLD, &matWorld);
            Physics::drawAABB(pDevice, aabb, 0xffffffff);
        }
    }

    if (m_model->isMesh() && this == cMap::instance()->getSelectObj())
    {
        Physics::OCTreePtr tree = Physics::findOCTree(m_model->getMesh());
        if (tree)
        {
            m_model->getMatrix(matWorld);
            pDevice->SetTransform(D3DTS_WORLD, &matWorld);
            tree->render(pDevice);
        }
    }
}

void cTerrainObj::setModel(ModelPtr model)
{
    m_model = model;

    if(m_model) m_model->getTransformdAABB(m_aabb);
}

float cTerrainObj::getBBRadius()
{
    if (!m_model)
    {
        return 1;
    }
    return m_model->getBBRadius();
}

D3DXVECTOR3 cTerrainObj::getBBCenter()
{
    if (!m_model)
    {
        return m_vPos;
    }
    return m_vPos + m_model->getBBCenter();
}

void cTerrainObj::setModelID(int id)
{
    m_modeID = id;

    TerrainResNode* pNode = cTerrainRes::instance()->find(id);
    if (NULL == pNode)
    {
        setModel(NULL);
        return;
    }

    setModel(cModelFactory::createObj2(pNode->path.c_str(), pNode->type));
}

void cTerrainObj::addLookAngle(float angle)
{
    m_angle.z = angle;
}

void cTerrainObj::addUpAngle(float angle)
{
    m_angle.y = angle;
}

void cTerrainObj::addRightAngle(float angle)
{
    m_angle.x = angle;
}

void cTerrainObj::updateRotationAxis(void)
{
    Physics::Matrix4x4 mat;
    D3DXMatrixRotationYawPitchRoll(&mat, m_angle.y, m_angle.x, m_angle.z);

    m_vRight.set(mat._11, mat._21, mat._31);
    m_vUp.set(mat._12, mat._22, mat._32);
    m_vLook.set(mat._13, mat._23, mat._33);
}

bool cTerrainObj::intersectWithCursor()
{
    cPick *pick = getPick();
    if (pick && m_model)
    {
        return pick->isIntersectWithSphere(getPos()+m_model->getBBCenter(), m_model->getBBRadius());
    }
    return false;
}

bool cTerrainObj::isActive(void)
{
    return getMap()->getActiveObj() == this;
}

void cTerrainObj::focusCursor(UINT msg)
{
    if (m_model)
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

void cTerrainObj::onFocusCursor(UINT msg)
{

}

float cTerrainObj::distToCamera(void)
{
    return m_vPos.distTo(getCamera()->position());
}

void cTerrainObj::save(Lazy::LZDataPtr dataPtr) const
{
    assert(dataPtr && "cTerrainObj::save");

    dataPtr->writeString(_T("uuid"), m_uuid);
    if(m_isRef)
    {
        dataPtr->writeBool(_T("refrence"), m_isRef);
        dataPtr->writeHex(_T("refchunk"), m_refChunk);
    }

    Lazy::tstring temp;
    if(m_model)
    {
        Lazy::charToWChar(temp, m_model->source());
        dataPtr->writeString(_T("path"), temp);

        int type = REST_MESH;
        if(m_model->isSkeleton()) type = REST_SKINMESH;
        dataPtr->writeInt(_T("type"), type);
    }

    Lazy::writeVector3(dataPtr, _T("scale"), m_vScale);
    Lazy::writeVector3(dataPtr, _T("pos"), m_vPos);
    Lazy::writeVector3(dataPtr, _T("angle"), m_angle);
    Lazy::writeVector3(dataPtr, _T("aabb/min"), m_aabb.min);
    Lazy::writeVector3(dataPtr, _T("aabb/max"), m_aabb.max);
}

void cTerrainObj::load(Lazy::LZDataPtr dataPtr)
{
    m_uuid = dataPtr->readString(_T("uuid"));
    m_isRef = dataPtr->readBool(_T("refrence"), false);
    m_refChunk = dataPtr->readHex(_T("refchunk"), 0);

    Lazy::tstring temp;
    temp = dataPtr->readString(_T("path"));
    if(!temp.empty())
    {
        std::string path;
        Lazy::wcharToChar(path, temp);

        int type = dataPtr->readInt(_T("type"));
        setModel(cModelFactory::createObj2(path.c_str(), type));
    }

    Lazy::readVector3(dataPtr, _T("scale"), m_vScale);
    Lazy::readVector3(dataPtr, _T("pos"), m_vPos);
    Lazy::readVector3(dataPtr, _T("angle"), m_angle);
    Lazy::readVector3(dataPtr, _T("aabb/min"), m_aabb.min);
    Lazy::readVector3(dataPtr, _T("aabb/max"), m_aabb.max);

    updateRotationAxis();
}

std::ostream& operator<<(std::ostream & out, cTerrainObj & node )
{
    out<<node.m_modeID<<" "
        <<node.getScale()<<" "
        <<node.m_vPos.x<<" "
        <<node.m_vPos.y<<" "
        <<node.m_vPos.z<<" "
        <<node.m_angle.x<<" "
        <<node.m_angle.y<<" "
        <<node.m_angle.z<<" ";
    return out;
}

std::istream& operator>>(std::istream & in, cTerrainObj & node )
{
    float scale;
    in>>node.m_modeID
        >>scale
        >>node.m_vPos.x
        >>node.m_vPos.y
        >>node.m_vPos.z
        >>node.m_angle.x
        >>node.m_angle.y
        >>node.m_angle.z;
    node.setModelID(node.m_modeID);
    node.setScale(scale);
    return in;
}

//获取真实的模型矩阵
void cTerrainObj::getAbsModelMatrix(Physics::Matrix4x4 & mat) const
{
    getWorldMatrix(mat);
    m_model->setWorldMatrix(mat);
    m_model->getMatrix(mat);
}