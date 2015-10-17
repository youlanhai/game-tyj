
#include "Commen.h"
#include "ImpModel.h"
#include "Animation.h"


///构造函数
cMeshModel::cMeshModel(void)
    : m_mesh(NULL)
{

}

cMeshModel::~cMeshModel()
{
    m_mesh = NULL;
}

bool cMeshModel::load(const std::string & name)
{
    iModel::load(name);

    m_mesh = getResMgr()->getMesh(name);
    if(m_mesh) setAABB(m_mesh->getAABB());

    return (m_mesh != NULL);
}

void cMeshModel::render(IDirect3DDevice9 * pDevice)
{
    if (m_mesh)
    {
        D3DXMATRIX mat;
        getMatrix(mat);
        pDevice->SetTransform(D3DTS_WORLD, &mat);
        m_mesh->render(pDevice);
    }

    iModel::render(pDevice);
}


///获得包围盒半径
float cMeshModel::getBBRadius(void) const
{ 
    return m_mesh->getBBRadius()*m_scale; 
}

///获得包围盒球心
D3DXVECTOR3 cMeshModel::getBBCenter(void) const
{ 
    return m_mesh->getBBCenter()*m_scale; 
}

//////////////////////////////////////////////////////////////////////////
/*static*/ ModelPtr cModelFactory::createObj(const std::string & fileName, const std::string & type)
{
    ModelPtr pModel;

    if (type == RES_MESH)
    {
        pModel = new cMeshModel();
    }
    else if (type == RES_SKINMESH)
    {
        pModel = new cAnimalModel();
    }
    else
    {
        return NULL;
    }

    if (!pModel->load(fileName))
    {
        return NULL;
    }

    return pModel;
}

/*static*/ ModelPtr cModelFactory::createObj2(const std::string & filename, int typeID)
{
    if (typeID == REST_MESH)
    {
        return createObj(filename, RES_MESH);
    }
    else if (typeID == REST_SKINMESH)
    {
        return createObj(filename, RES_SKINMESH);
    }
    return NULL;
}