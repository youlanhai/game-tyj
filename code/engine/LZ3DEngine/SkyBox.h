/**********************************************************
 cSkyBox.h（天空盒子类）
 作用: 创建天空盒子
 **********************************************************/
#pragma once

#include "RenderObj.h"
#include "I3DObject.h"

class LZDLL_API cSkyBox : public IBase, public IRender
{
public:
	cSkyBox();

    cSkyBox(IDirect3DDevice9 *pDevice);

	~cSkyBox(void);

    ///创建顶点
    bool create(IDirect3DDevice9 *pDevice);

    /** 设置天空纹理。
        @param : texNames，纹理名称。格式为name%d.xxx。图片顺序为(z+为北):北东南西顶底。*/
    void setSkyImage(LPCSTR texNames);

    /** 设置天空范围。*/
    void setSkyRange(D3DXVECTOR3 min_, D3DXVECTOR3 max_);
	
	void render(IDirect3DDevice9 *pDevice);

    void setSource(I3DObject* pSource){ m_pSource = pSource; }

    I3DObject* getSource(void){ return m_pSource; }
	
    void show(bool s){ m_visible = s; }

    bool visible(){ return m_visible; }

    void toggle(){ show(!visible()); }
private:
    I3DObject*              m_pSource;
    std::string             m_texNames;
	IDirect3DVertexBuffer9  *m_pVertexBuffer;
    bool                    m_visible;
};
