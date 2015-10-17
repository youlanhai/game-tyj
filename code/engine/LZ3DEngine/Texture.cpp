//Texture.cpp
#include "Commen.h"
#include "Utilities.h"
#include "Texture.h"
#include "Res.h"

///构造函数
cResTexture::cResTexture()
{
    m_texture = NULL;
}

cResTexture::~cResTexture()
{
    SafeRelease(m_texture);
}

/** 创建自己。*/
IRenderRes* cResTexture::createObj(void)
{
    return new cResTexture();
}

/** 加载资源。*/
bool cResTexture::load(std::string filename)
{
    if(FAILED(D3DXCreateTextureFromFile(
        getResMgr()->getDevice(),
        Lazy::charToWChar(filename).c_str(),
        &m_texture)))
    {
        XWRITE_LOGA("error 加载纹理%s失败!", filename.c_str());
        m_texture = NULL;
        return false;
    }
    return true;
}


void cResTexture::update(float fElapse)
{
}

void cResTexture::render(IDirect3DDevice9 * pDevice)
{
    pDevice->SetTexture(0, m_texture);
}
