#include <fstream>
#include "Commen.h"
#include "Utilities.h"
#include "Animation.h"
#include "SkinMesh.h"
#include "App.h"

//////////////////////////////////////////////////////////////////////////
cAnimalModel::cAnimalModel()
	: m_pSkinMesh(NULL)
	, m_pAnimControler(NULL)
{
    m_elapsedTime = m_elapse = 0.01f;
    m_loop = true;
    m_speed = 1.0f;
}

cAnimalModel::~cAnimalModel(void)
{
    SafeRelease(m_pAnimControler);
    m_pSkinMesh = NULL;
}

bool cAnimalModel::load(const std::string & name)
{ 
    iModel::load(name);

    m_pSkinMesh = getResMgr()->getSkinMesh(name);
    if (!m_pSkinMesh)
    {
        return false;
    }

    setAABB(m_pSkinMesh->getAABB());
    m_pAnimControler = m_pSkinMesh->cloneAnimaCtrl();
    return true; 
}

/** 更新*/
void cAnimalModel::update(float elapse)
{
    m_elapse = elapse*m_speed;
}

/** 渲染*/
void cAnimalModel::render(IDirect3DDevice9* pDevice)
{
    if (m_pSkinMesh)
    {
        //DebugMsg("render%d", DWORD(this));
        D3DXMATRIX mat;
        getMatrix(mat);

        advanceAnimation();
        m_pSkinMesh->setWorldMatrix(mat);
        m_pSkinMesh->render(pDevice);
    }

    iModel::render(pDevice);
}

///获得包围盒半径
float cAnimalModel::getBBRadius(void) const
{ 
    return m_pSkinMesh->getBBRadius()*m_scale; 
}

///获得包围盒球心
D3DXVECTOR3 cAnimalModel::getBBCenter(void) const
{ 
    return m_pSkinMesh->getBBCenter()*m_scale; 
}

/** 步进动画*/
void cAnimalModel::advanceAnimation(void)
{
    if (m_pAnimControler)
    {
        if (m_loop)
        {
            m_pAnimControler->AdvanceTime(m_elapse, NULL);
        }
        else
        {
            LPD3DXANIMATIONSET pASTrack;
            m_pAnimControler->GetTrackAnimationSet(0, &pASTrack);
            float dt = (float)pASTrack->GetPeriod();
            //DebugMsg("(%f, %f, %lf)", m_elapsedTime, m_elapse, dt);
            if (m_elapsedTime <= dt)
            {
                if (m_elapsedTime + m_elapse > dt)
                {
                    m_elapse = dt - m_elapsedTime;
                    m_elapsedTime = dt;
                }
                else 
                {
                    m_elapsedTime += m_elapse;
                }
                m_pAnimControler->AdvanceTime(m_elapse, NULL);
            }
        }
    }
}

LPD3DXANIMATIONSET cAnimalModel::getAnimSet(int n) const
{
    if (n>=0 && n<getActionCount())
    {
        LPD3DXANIMATIONSET pAnimSet = NULL;
        m_pAnimControler->GetAnimationSet(n, &pAnimSet);
        return pAnimSet;
    }
    return NULL;
}

LPD3DXANIMATIONSET cAnimalModel::getAnimSet(std::string name) const
{
    LPD3DXANIMATIONSET pAnimSet = NULL;
    m_pAnimControler->GetAnimationSetByName(name.c_str(), &pAnimSet);
    return pAnimSet;
}

LPD3DXANIMATIONSET cAnimalModel::findAnimSet(std::string name) const
{
    int n = getActionCount();
    std::string animName;
    LPD3DXANIMATIONSET pAnimSet = NULL;
    for(int i=0; i<n; ++i)
    {
        pAnimSet = getAnimSet(i);
        if (pAnimSet)
        {
            animName = pAnimSet->GetName();
            if (animName.find(name) != name.npos)
            {
                return pAnimSet;
            }
        }
    }
    return NULL;
}

/** 播放动画*/
bool cAnimalModel::playAction(std::string name, bool loop)
{
    //DebugMsg("playAction name=%s, loop=%d", name.c_str(), loop);
    m_loop = loop;
    return playAction(getAnimSet(name));
}

bool cAnimalModel::playAction(LPD3DXANIMATIONSET pAnimSet)
{
    if (NULL == pAnimSet)
    {
        return false;
    }
    if(FAILED(m_pAnimControler->SetTrackAnimationSet(0, pAnimSet)))
    {
        return false;
    }
    m_elapsedTime = 0;
    return true;
}

/** 停止动画*/
void cAnimalModel::stopAction(void)
{
    if(!playAction(findAnimSet("idle")))
    {
        playAction(getAnimSet(0));
    }
}

/** 获得动画个数*/
int cAnimalModel::getActionCount(void) const
{
    return m_pAnimControler->GetNumAnimationSets();
}

/** 获得动画名称*/
std::string cAnimalModel::getActionName(int index) const
{
    LPD3DXANIMATIONSET pAnimSet = getAnimSet(index);
    return pAnimSet->GetName();
}
