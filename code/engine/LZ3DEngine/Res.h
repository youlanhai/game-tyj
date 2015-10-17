#pragma once

#include "RenderRes.h"

#define RES_MESH        "mesh"
#define RES_SKINMESH    "skinmesh"
#define RES_FONT        "font"
#define RES_TEXTURE     "texture"
#define RES_ANIMAL      "animation"

enum ResType
{
    REST_MESH = 0,
    REST_SKINMESH,
    REST_FONT,
    REST_TEXTURE,
    REST_ANIMAL,
    REST_SOUND,
};


class LZDLL_API cResFactory : public IResFactory
{
public:
    cResFactory();
    ~cResFactory();

    /** 创建资源对象。*/
    virtual IRenderRes* createObj(std::string type);
};


class cSkinMesh;
class cResMesh;
class cResTexture;
class cResFont;

class LZDLL_API cResMgr : public cRenderResMgr
{
public:
    ///构造函数
    cResMgr(IDirect3DDevice9 *pDevice);

    cSkinMesh* getSkinMesh(std::string filename);
    cResMesh* getMesh(std::string filename);
    cResFont* getFont(std::string filename, bool useDefault=true);
    LPD3DXFONT getFontEx(std::string filename, bool useDefault=true);
    cResTexture* getTexture(std::string filename);
    LPDIRECT3DTEXTURE9 getTextureEx(std::string filename);

};


LZDLL_API cResMgr* getResMgr(void);