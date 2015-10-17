//d3dfont2d.cpp 
#include "Commen.h"
#include "font2d.h"
#include "Utilities.h"
#include "Res.h"

///构造函数
cResFont::cResFont()
{
    m_font = NULL;
}

cResFont::~cResFont()
{
    SafeRelease(m_font);
}

/** 创建自己。*/
IRenderRes* cResFont::createObj(void)
{
    return new cResFont();
}

/** 加载资源。*/
bool cResFont::load(std::string filename)
{
    if (filename.empty())
    {
        return false;
    }
    std::string name = filename;
    int w = 0;
    int h = 0;
    bool b = false;

    std::vector<std::string> names;
    size_t pos = name.find('/');
    while(pos != name.npos) 
    {
        names.push_back(name.substr(0, pos));
        name = name.substr(pos+1);
        pos = name.find('/');
    }
    names.push_back(name);

    if (names.size() < 2)
    {
        return false;
    }
    name = names[0];
    w = atoi(names[1].c_str());
    if (names.size() >= 3)
    {
        h = atoi(names[2].c_str());
    }
    if (names.size() >= 4)
    {
        b = true;
    }

    m_font = LZUtility::createFont(getResMgr()->getDevice(), name, w, h, b);
    return (m_font != NULL);
}


void cResFont::update(float fElapse)
{

}

void cResFont::render(IDirect3DDevice9 * pDevice)
{

}
