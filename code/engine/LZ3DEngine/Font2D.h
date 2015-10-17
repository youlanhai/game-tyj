#pragma once

#include "RenderRes.h"

class LZDLL_API cResFont : public IRenderRes
{
public:
    ~cResFont();

    /** 创建自己。*/
    static IRenderRes* createObj(void);

    /** 加载资源。
        @param filename : 字体描述名称。格式：字体/宽/高/是否粗体，后两个可忽略。如果不是粗体，
        则忽略最后一个参数，否则不管什么字符，都认为是粗体。
        */
    virtual bool load(std::string filename);

    /** 获得资源类型*/
    virtual LPCSTR getType(void){ return "texture"; }

    void update(float fElapse);

    void render(IDirect3DDevice9 * pDevice);

    LPD3DXFONT getFont(void){ return m_font; }

protected:
    ///构造函数
    cResFont();

protected:
    LPD3DXFONT m_font;
};