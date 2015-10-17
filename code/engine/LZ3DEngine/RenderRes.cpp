#include "Commen.h"
#include "RenderRes.h"
#include "Utilities.h"

cRenderResMgr::cRenderResMgr(IDirect3DDevice9 *pDevice)
{
    m_factory = NULL;
    m_pd3dDevice = pDevice;
    m_pd3dDevice->AddRef();

    //DebugMsg("cRenderResMgr construct");
    MEMORY_CHECK_CONS;
}

cRenderResMgr::~cRenderResMgr(void)
{
    clear();
    SafeRelease(m_pd3dDevice);

    //DebugMsg("cRenderResMgr destruct");
    MEMORY_CHECK_DEST;
}

bool cRenderResMgr::init(std::string configFile/*=""*/)
{
    if (configFile.empty())
    {
        return true;
    }

    std::ifstream fin(configFile.c_str());
    if(!fin.good())
    {
        return false;
    }

    int count;
    fin>>count;

    std::string name, type;
    for(int i=0; i<count; ++i)
    {
        fin>>name>>type;
        if (name[0] != 0)
        {
            m_resourses[name] = load(name, type);
        }
    }
    fin.close();

    return true;
}

IRenderRes* cRenderResMgr::get(std::string fileName, std::string type)
{
    if (fileName.empty())
    {
        return NULL;
    }
    ResIterator it = m_resourses.find(fileName);
    if (it != m_resourses.end())
    {
        return it->second;
    }
    IRenderRes* pTex = load(fileName, type);
    m_resourses[fileName] = pTex;
    return pTex;
}

void cRenderResMgr::remove(std::string fileName)
{
    ResIterator it = m_resourses.find(fileName);
    if (it != m_resourses.end())
    {
        delete (it->second);
        m_resourses.erase(it);
    }
}

void cRenderResMgr::clear(void)
{
    for (ResIterator it = m_resourses.begin();
        it != m_resourses.end();
        ++it)
    {
        delete it->second;
    }
    m_resourses.clear();
}

IRenderRes* cRenderResMgr::load(std::string fileName, std::string type)
{
    assert(m_factory!=NULL && "工厂函数未设置！");
    IRenderRes* pRes = m_factory->createObj(type);
    if (NULL == pRes)
    {
        return NULL;
    }
    if(!pRes->load(fileName))
    {
        XWRITE_LOGA("加载资源%s失败!", fileName.c_str());
        delete pRes;
        return NULL;
    }
    return pRes;
}

