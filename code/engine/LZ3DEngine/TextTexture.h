#pragma once

#include "Base.h"
#include "RenderObj.h"
#include "..\utility\UtilConfig.h"

class LZDLL_API cTextTexture : public IBase
{
public:
    ~cTextTexture(void);

    void setText(LPCSTR text);
    LPCSTR getText(void){ return m_text.c_str(); }

    void setColor(DWORD cr);

    IDirect3DTexture9* getTexture(void){ return m_pTexture; }

protected:

    cTextTexture();

    void init(LPCSTR text, LPCSTR font, DWORD color, int width, int height);

    void setTexture(IDirect3DTexture9* pTex){ m_pTexture = pTex; }

    void drawText();

protected:

    IDirect3DTexture9   *m_pTexture;
    std::string         m_text;
    std::string         m_font;
    DWORD               m_color;
    int                 m_width;
    int                 m_height;

    friend class cTextTextureFactory;
};

class LZDLL_API cTextTextureFactory : public IBase
{
public: 
    cTextTextureFactory(IDirect3DDevice9* pDevice);

    ~cTextTextureFactory();

    cTextTexture* createObj(LPCSTR text, LPCSTR font, DWORD color, int width, int height);

    void updateText(cTextTexture* p);
protected:
    IDirect3DDevice9    *m_pd3dDevice;
};

LZDLL_API cTextTextureFactory* getTextTextureFactory(void);


class LZDLL_API TextTextureEx : public IBase
{
public:
    TextTextureEx();
    ~TextTextureEx();

    void setText(const std::string & text);
    void setColor(DWORD cr);
    void setFont(const std::string & font);

    LPDIRECT3DTEXTURE9 getTexture() const { return m_pTexture; }

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

    const std::string & getText() const { return m_text; }

    void loadTexture();

private:
    std::string     m_text;
    std::string     m_font;
    DWORD           m_color;
    LPDIRECT3DTEXTURE9 m_pTexture;
    bool            m_dirty;
    int             m_width;
    int             m_height;
};