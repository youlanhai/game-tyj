#pragma once
#include "Model.h"
#include "Res.h"
#include "Mesh.h"

class LZDLL_API cMeshModel : public iModel
{
public:
    cMeshModel(void);

    ~cMeshModel();

    virtual bool isMesh(void) const { return true; }

    virtual bool load(const std::string & name);

    virtual void render(IDirect3DDevice9 * pDevice);

    ///获得包围盒半径
    virtual float getBBRadius(void) const;

    ///获得包围盒球心
    virtual D3DXVECTOR3 getBBCenter(void) const;

    LPD3DXMESH getMesh(){ return m_mesh->getMesh(); }

protected:
    cResMesh*    m_mesh;
};