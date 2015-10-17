//Texture.h
#pragma once

#include "RenderRes.h"

class LZDLL_API cResTexture : public IRenderRes
{
public:
    ~cResTexture();

    /** 创建自己。*/
    static IRenderRes* createObj(void);

    /** 加载资源。*/
    virtual bool load(std::string filename);

    /** 获得资源类型*/
    virtual LPCSTR getType(void){ return "texture"; }

    void update(float fElapse);

    void render(IDirect3DDevice9 * pDevice);

    IDirect3DTexture9* getTexture(void){ return m_texture; }

protected:
    ///构造函数
    cResTexture();

protected:
    IDirect3DTexture9*  m_texture;
};