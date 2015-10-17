#pragma once

#include "Billboard.h"
#include "TextTexture.h"
#include "I3DObject.h"

class LZDLL_API cTopBoard : public cBillboard
{
public:
    cTopBoard(I3DObject* pSource);
    ~cTopBoard(void);

    bool create(const std::string & text, 
        const std::string & fontName,
        DWORD color, float biasHeight);

    void setText(const std::string & text);

    std::string getText(void);

    void setColor(DWORD cr);

    void setBiasHeight(float h){ m_biasHeight = h; }

    virtual void render(IDirect3DDevice9 * pDevice);

protected:
    RefPtr<TextTextureEx>    m_pTextTexture;
    I3DObject               *m_pSource;
    float                   m_biasHeight;
};
