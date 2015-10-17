//Game.cpp
#include "stdafx.h"
#include "Game.h"
#include "Navigation.h"
#include <tchar.h>

//////////////////////////////////////////////////////////////////////////
CGame g_game;
CGame* getGampApp(void)
{
    return &g_game;
}


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    Lazy::defaultCodePage = Lazy::CP::utf8;
    LZUtility::DefaultResPath = "res";
    LZUtility::ChangeCurDirectory();
    INIT_LOG(_T("navgen.log"));

    if(g_game.create(hInstance, "Navigation Mesh", 800, 600, false))
    {
        g_game.mainLoop();
    }
    return 0;
}

const Lazy::tstring ConfigFile = _T("navgen.lzd");

float g_useTime = 0.0f;
float g_searchTime = 0.0f;

Physics::WpPtrArray g_pathData;//路径数据

//寻路目标点
Physics::Vector3 g_destPos(0, 0, 0);

//需要显示的chunk id
int g_focusWayChunk = 0;

//玩家
EntityPtr g_player;


class PlayerPhysics : public iPhysics
{
public:
    virtual bool isPlayers(){ return true; }

    explicit PlayerPhysics(I3DObject* pSource)
        : iPhysics(pSource)
    {
    }
};

//生成路点
void generateWayPoint()
{
    Physics::ChunkNodeMgr *pMgr = Physics::ChunkNodeMgr::instance();
    
    WRITE_LOG(_T("INFO: start generate..."));

    g_useTime = Lazy::getCurTime();
    pMgr->doGenerate();
    g_useTime = Lazy::getCurTime() - g_useTime;

    XWRITE_LOG(_T("INFO: generate finished. use time:%f"), g_useTime);
}

void generateWayPointMany(Lazy::LZDataPtr maps)
{
    Physics::ChunkNodeMgr *pMgr = Physics::ChunkNodeMgr::instance();

    Lazy::LZDataBase::DataConstIterator it;
    FOR_EACH((*maps), it)
    {
        if((*it)->tag() != _T("name"))
        {
            continue;
        }

        const Lazy::tstring & mapName = (*it)->value();
        if(mapName.empty())
        {
            WRITE_LOG(_T("ERROR: invalid mapName!"));
            continue;
        }

        if(!cMap::instance()->loadMap(Lazy::wcharToChar(mapName)))
        {
            WRITE_LOG(_T("ERROR: load map failed!"));
            break;
        }

        XWRITE_LOG(_T("INFO: start generate '%s'..."), mapName.c_str());

        g_useTime = Lazy::getCurTime();
        if(pMgr->doGenerate())
        {
            pMgr->doSave();
        }
        g_useTime = Lazy::getCurTime() - g_useTime;

        XWRITE_LOG(_T("INFO: generate finished. use time:%f"), g_useTime);

        if(!getApp()->processMessage())
        {
            PostQuitMessage(0);
            break;
        }
    }
}

//鼠标左键点地寻路
void searchPath()
{
    if(!cPick::instance()->queryWithTerrain()) return;
    g_destPos = cPick::instance()->getPosOnTerrain();

    g_pathData.clear();
    g_searchTime = Lazy::getCurTime();

    if(!g_player->getPhysics()->searchPathEx(g_pathData, g_destPos))
    {
        debugMessage(_T("ERORR: find way failed! "));
    }

    g_searchTime = Lazy::getCurTime() - g_searchTime;
}

///////////////////class CGame start/////////////////////////

/*注册窗口类消息。请实现该消息来修改窗口风格。*/
void CGame::onRegisterClass(WNDCLASSEX *pwc)
{
}

/*创建设备消息。实现该方法来修改设备。*/
void CGame::onCreateDevice(D3DPRESENT_PARAMETERS * pParam)
{

}

//窗口过程，内部使用
LRESULT CGame::wndProc(HWND hWnd,UINT uMsg,
							WPARAM wParam,LPARAM lParam)
{
    if (uMsg == WM_KEYUP)
    {
        switch(wParam)
        {
        case VK_F1 :
            Physics::NagenDebug::showMesh = !Physics::NagenDebug::showMesh;
            break;
        case VK_F2:
            Physics::NagenDebug::showLayer = !Physics::NagenDebug::showLayer;
            break;
        case VK_F3:
            Physics::NagenDebug::showHF = !Physics::NagenDebug::showHF;
            break;
        case VK_F4:
            Physics::NagenDebug::showTriangle = !Physics::NagenDebug::showTriangle;
            break;
        case VK_F5:
            Physics::NagenDebug::showWP = !Physics::NagenDebug::showWP;
            break;
        case VK_F6:
            Physics::NagenDebug::showPath = !Physics::NagenDebug::showPath;
            break;
        }
    }

    if (m_pCamera)
    {
        if(m_pCamera->handleMessage(uMsg, wParam, lParam)) return 0;
    }

    cPick::instance()->handleMouseEvent(uMsg, wParam, lParam);

    if(uMsg == WM_LBUTTONUP) searchPath();

    return CApp::wndProc(hWnd,uMsg,wParam,lParam);
}

CGame::CGame()
{
	m_bGameStart=false;
}

void CGame::clear()
{
    WRITE_LOG(_T("start clear..."));

    CApp::clear();
    Lazy::MemoryPool::instance()->report();

    WRITE_LOG(_T("clear finished."));
}

CGame::~CGame()
{
}

/*游戏初始化*/
bool CGame::init(void)
{
    WRITE_LOG(_T("start initialize..."));
    CApp::init();

	//在此添加初始化代码
    srand(timeGetTime());
    
    m_bGameStart = true;

	m_pFPS = new cFpsRender();
	m_pFPS->init();

    m_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0xff7f7f7f);


    g_player = new iEntity();
    g_player->m_vPos.set(100, 2000.0f, -500.0f);
    g_player->setSpeed(500.0f);
    g_player->setPhysics(new PlayerPhysics(g_player.get()));
    addUpdateRender(g_player.get());

    ModelPtr model = cModelFactory::createObj2("res/model/jingtian/jingtian.x", REST_SKINMESH) ;
    if(model) model->setYaw(3.14f);
    g_player->setModel(model);

    cMap::instance()->setSource(g_player.get());

    m_pCamera = new CCursorCamera(m_pd3dDevice, CCamera::THIRD);
    m_pCamera->setSource(g_player.get());

    m_pCamera->setPosition(D3DXVECTOR3(100, 2000.0f, -500.0f));
    m_pCamera->setSpeed(500.0f);
    m_pCamera->setDistRange(10.0f, 300.0f);
    m_pCamera->setNearFar(1.0f, 10000.0f);

    //设置配置参数
    Physics::FindWay::Config.clampHeight = 30.0f;
    Physics::FindWay::Config.modelHeight = 80.0f;
    Physics::FindWay::Config.cubeSize = 100.0f;
    Physics::FindWay::Config.minSetPoints = 8;

    //加载地图
    Lazy::LZDataPtr root = new Lazy::lzd();
    if(!root->load(ConfigFile))
    {
        XWRITE_LOG(_T("ERROR: load game config file '%s' failed!"), ConfigFile.c_str());
        return false;
    }

    
    Lazy::tstring mapName = root->readString(_T("workmap"));
    if(!mapName.empty())
    {
        if(!cMap::instance()->loadMap(Lazy::wcharToChar(mapName)))
        {
            WRITE_LOG(_T("ERROR: load map failed!"));
            return false;
        }
    }
    else
    {
        Lazy::LZDataPtr maps = root->read(_T("workgroup"));
        if(maps)
        {
            generateWayPointMany(maps);
        }
        else
        {
            WRITE_LOG(_T("ERROR: doen't found any config map!"));
            return false;
        }
    }

    WRITE_LOG(_T("initialize finished."));
	return true;
}

//更新
void CGame::update()
{
    __super::update();

    //在此添加更新代码
    m_pUpdateTaskMgr->update(m_fElapse);

    if (m_pKeyboard->isKeyUp('J'))
    {
        cMap::instance()->setShowLevel(cMap::instance()->getShowLevel()-1);
    }
    else if (m_pKeyboard->isKeyUp('K'))
    {
        cMap::instance()->setShowLevel(cMap::instance()->getShowLevel()+1);
    }
    //计算路点
    else if(m_pKeyboard->isKeyPress(VK_CONTROL) && m_pKeyboard->isKeyUp('G'))
    {
        generateWayPoint();
    }
    //保存路点
    else if(m_pKeyboard->isKeyPress(VK_CONTROL) && m_pKeyboard->isKeyUp('S'))
    {
        Physics::ChunkNodeMgr::instance()->doSave();
    }
    //刷新路点显示区域
    else if(m_pKeyboard->isKeyUp(VK_RETURN))
    {
        Physics::WayChunkPtr ptr = Physics::WayChunkMgr::instance()->getChunkByPos(
            m_pCamera->position().x, m_pCamera->position().z);
        
        if(ptr) g_focusWayChunk = ptr->getId();
    }

    cMap::instance()->update(m_fElapse);
    updateCamera(m_fElapse);
    m_pCamera->update(m_fElapse);
}

void CGame::updateCamera(float fEla)
{
    //////////////////////////////////////////////////////////////////////////
    //更新摄像机
    if(m_pKeyboard->isKeyDown('1'))
    {
        m_pCamera->setCamareType(CCamera::FIRST);
    }
    else if(m_pKeyboard->isKeyDown('2'))
    {
        m_pCamera->setCamareType(CCamera::THIRD);
    }
    else if (m_pKeyboard->isKeyDown('3'))
    {
        m_pCamera->setCamareType(CCamera::FREE);
    }

}
//渲染
void CGame::render()
{
    m_pFPS->update(0);

	m_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,100),1.0f,0);
	if(SUCCEEDED(m_pd3dDevice->BeginScene()))
	{
        CLight light;
        light.SetDirectionLight(m_pd3dDevice, 
            D3DXVECTOR3(0, -1.0f, 0), 
            D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.8f));

		//添加渲染代码
		m_pCamera->render(m_pd3dDevice);
        CApp::render();

        m_pRenderTaskMgr->render(m_pd3dDevice);

        m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
        //显示地形
        if ( Physics::NagenDebug::showMesh)
        {
            cMap::instance()->render(m_pd3dDevice);
        }
        m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

        m_pd3dDevice->SetTexture(0, NULL);

        D3DXMATRIX matIdentity;
        D3DXMatrixIdentity(&matIdentity);
        m_pd3dDevice->SetTransform(D3DTS_WORLD, &matIdentity);

        //路点数据
        if(Physics::NagenDebug::showWP)
        {
            //Physics::WayChunkMgr::instance()->render(m_pd3dDevice);
            Physics::WayChunkPtr ptr = Physics::WayChunkMgr::instance()->getChunk(g_focusWayChunk);
            if(ptr) ptr->render(m_pd3dDevice);
        }

        //绘制寻路路径
        if(Physics::NagenDebug::showPath && !g_pathData.empty())
        {
            Physics::WpPtrArray::iterator it;
            FOR_EACH(g_pathData, it)
            {
                (*it)->render(m_pd3dDevice, 0xffff0000);
            }
        }

        //绘制寻路目标点
        {
            Physics::AABB aabb;
            Physics::makeRoleAABB(Physics::FindWay::Config, aabb, g_destPos);
            Physics::drawAABB(m_pd3dDevice, aabb, 0xff0000ff);
        }

		m_pFPS->render(m_pd3dDevice);

        //绘制帮助文本
        LPD3DXFONT pFont = getResMgr()->getFontEx("宋体/17/0/b");
        if (pFont)
        {
            Lazy::tstring strInfo ;
            DWORD color = 0xffffff00;
            DWORD dh = 20;
            CRect rc = getClientRect();

            rc.top += 20;
            formatString(strInfo, _T("gen wp use:%fs"), g_useTime);
            pFont->DrawText(NULL, strInfo.c_str(), -1, &rc, DT_LEFT, color);

            rc.top += 20;
            formatString(strInfo, _T("search use:%fs. num waypoint:%u"), g_searchTime, g_pathData.size());
            pFont->DrawText(NULL, strInfo.c_str(), -1, &rc, DT_LEFT, color);

            Physics::WayChunkPtr pChunk = Physics::WayChunkMgr::instance()->getChunk(0, false);
            if(pChunk)
            {
                formatString(strInfo, _T("[chunk 0]waySet num: %d, wayPoint num: %d"),
                    pChunk->numWaySet(), pChunk->numWayPoint());
                rc.top += 20;
                pFont->DrawText(NULL, strInfo.c_str(), -1, &rc, DT_LEFT|DT_WORDBREAK, color);
            }
            rc.top = rc.bottom - dh*7;
            strInfo = _T(" [F1]-show mesh\r\n [F2]-show layer\r\n [F3]-show height field\r\n")
                _T(" [F4]-show triangle\r\n [F5]-show way point(Enter key refresh to camera.)\r\n")
                _T(" [F6]-show path\r\n [Ctrl+G]-generate waypoint\r\n [Ctrl+S]-save waypoints");

            pFont->DrawText(NULL, strInfo.c_str(), -1, &rc, DT_LEFT|DT_WORDBREAK, color);
        }
		/////////////////////////
		m_pd3dDevice->EndScene();
	}
	else
	{
		WRITE_LOG(_T("render scene failed!(m_pd3dDevice->BeginScene())"));
        Sleep(100);
	}
	m_pd3dDevice->Present(NULL,NULL,NULL,NULL);
}

///////////////////class CGame end/////////////////////////