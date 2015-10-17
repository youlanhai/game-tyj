#include "Commen.h"
#include "Utilities.h"
#include "Res.h"

#include "Mesh.h"
#include "SkinMesh.h"
#include "Texture.h"
#include "Font2D.h"


//////////////////////////////////////////////////////////////////////////
cResFactory::cResFactory()
{
    MEMORY_CHECK_CONS;
}

cResFactory::~cResFactory()
{
    MEMORY_CHECK_DEST;
}

/** 创建资源对象。*/
IRenderRes* cResFactory::createObj(std::string type)
{
    if (type == RES_TEXTURE)
    {
        return cResTexture::createObj();
    }
    else if (type == RES_FONT)
    {
        return cResFont::createObj();
    }
    else if (type == RES_MESH)
    {
        return cResMesh::createObj();
    }
    else if (type == RES_SKINMESH)
    {
        return cSkinMesh::createObj();
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////

static cResMgr* g_pResMgr_ = NULL;
LZDLL_API cResMgr* getResMgr(void)
{
    return g_pResMgr_;
}

///构造函数
cResMgr::cResMgr(IDirect3DDevice9 *pDevice)
    : cRenderResMgr(pDevice)
{
    g_pResMgr_ = this;
}

cSkinMesh* cResMgr::getSkinMesh(std::string filename)
{
    return (cSkinMesh*)get(filename, RES_SKINMESH);
}

cResMesh* cResMgr::getMesh(std::string filename)
{
    return (cResMesh*)get(filename, RES_MESH);
}

cResFont* cResMgr::getFont(std::string filename, bool useDefault/*=true*/)
{
    IRenderRes* pRes = get(filename, RES_FONT);
    if (NULL == pRes)
    {
        pRes = get("宋体/16", RES_FONT);
        if (NULL == pRes)
        {
            return NULL;
        }
    }
    return (cResFont*)pRes;
}

LPD3DXFONT cResMgr::getFontEx(std::string filename, bool useDefault/*=true*/)
{
    cResFont* pFont = getFont(filename, useDefault);
    if (NULL == pFont)
    {
        return NULL;
    }
    return pFont->getFont();
}

cResTexture* cResMgr::getTexture(std::string filename)
{
    return (cResTexture*)get(filename, RES_TEXTURE);
}

LPDIRECT3DTEXTURE9 cResMgr::getTextureEx(std::string filename)
{
    cResTexture* pRes = getTexture(filename);
    if (NULL == pRes)
    {
        return NULL;
    }
    return pRes->getTexture();
}
