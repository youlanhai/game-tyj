
#include "Commen.h"
#include "Utilities.h"
#include "TerrinMap.h"

#include "TopBoard.h"
//#include "App.h"

cTopBoard::cTopBoard(I3DObject* pSource)
    : m_pSource(pSource)
    , m_biasHeight(1.0f)
    , m_pTextTexture(new TextTextureEx())
{
    MEMORY_CHECK_CONS;
}

cTopBoard::~cTopBoard(void)
{
    m_pTextTexture = NULL;
    MEMORY_CHECK_DEST;
}

bool cTopBoard::create(const std::string & text, 
    const std::string & font, DWORD color, float biasHeight)
{
    m_pTextTexture->setText(text);
    m_pTextTexture->setFont(font);
    m_pTextTexture->setColor(color);

    return m_pTextTexture != NULL;
}

void cTopBoard::setText(const std::string & text)
{
    m_pTextTexture->setText(text);
}


std::string cTopBoard::getText(void)
{ 
    return m_pTextTexture->getText();
}

void cTopBoard::setColor(DWORD cr)
{
    m_pTextTexture->setColor(cr);
}

void cTopBoard::render(IDirect3DDevice9 * pDevice)
{
    if (m_pSource)
    {
        m_pos = m_pSource->getPos();
        m_pos.y += m_biasHeight;
    }

    m_pTextTexture->loadTexture();

    setTexture(m_pTextTexture->getTexture());
    m_width = float(m_pTextTexture->getWidth());
    m_height = float(m_pTextTexture->getHeight());

    __super::render(pDevice);
}
