//Game.cpp
#include "Game.h"
#include "resource.h"

//#include <Python.h>
#include "PyScript.h"

#include "../engine/LZ3DEngine/LZ3DEngine.h"
#include "../engine/LZ3DEngine/Face.h"

#include "../engine/utility/LoadingMgr.h"
#include "../engine/Font/Font.h"
#include "../engine/utility/DataSection/DataSection.h"
#include "../engine/LZGUI/LZGUI.h"

using namespace Lazy;

#define ICON_FILE   L"res/gui/tyj.ico"
#define GAME_CONFIG _T("gameconfig.lzd")
#define CURSOR_FILE "res/cursor/cur.ani"
#define FONT_FILE "res/font/STSONG.TTF"
#define LOADING_IMAGE "res/gui/door.jpg"

//////////////////////////////////////////////////////////////////////////
CGame g_game;
CGame* getGampApp(void)
{
    return &g_game;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	Lazy::defaultCodePage = CP_UTF8;

    LZUtility::ChangeCurDirectory();
    INIT_LOG(_T("MyGame.log"));

    LZDataPtr root = Lzd::createFromFile(GAME_CONFIG);
    if (!root)
    {
        XWRITE_LOGA("ERROR: Load Game config file '%s' failed!", GAME_CONFIG);
        return 0;
    }

    if(g_game.create(hInstance, 
        root->readUtf8(_T("app/name")).c_str(),
        root->readInt(_T("app/width"), 800),
        root->readInt(_T("app/height"), 600),
        root->readBool(_T("app/fullscr"), false)))
    {
        g_game.mainLoop();
    }

    return 0;
}

///////////////////class CGame start/////////////////////////

/*注册窗口类消息。请实现该消息来修改窗口风格。*/
void CGame::onRegisterClass(WNDCLASSEX *pwc)
{
    //pwc->hIcon = LoadIcon(m_hInstance,MAKEINTRESOURCE(IDI_WITG));
	pwc->hIcon = (HICON)::LoadImageW(m_hInstance, ICON_FILE, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    pwc->hIconSm = pwc->hIcon;
    //pwc->hCursor = LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_MYGAME));
    pwc->hCursor = ::LoadCursorFromFile(_T(CURSOR_FILE));
}

/*创建设备消息。实现该方法来修改设备。*/
void CGame::onCreateDevice(D3DPRESENT_PARAMETERS * pParam)
{
    pParam->PresentationInterval = D3DPRESENT_INTERVAL_ONE;
}

//窗口过程，内部使用
LRESULT CGame::wndProc(HWND hWnd,UINT uMsg,
							WPARAM wParam,LPARAM lParam)
{
    if (!m_isLoading && m_pyScript)
    {
       if(m_pyScript->msgProc(uMsg, wParam, lParam))
       {
           return 1;
       }
    }
    return __super::wndProc(hWnd,uMsg,wParam,lParam);
}

CGame::CGame()
{
}

void CGame::clear()
{
    WRITE_LOG(_T("INFO: start clean up..."));

    m_pyScript->release();

    Lazy::LoadingMgr::fini();

    __super::clear();

    WRITE_LOG(_T("INFO: clean up finished."));
}
CGame::~CGame()
{
}

/*游戏初始化*/
bool CGame::init(void)
{
    WRITE_LOG(_T("INFO: start initialize..."));

    srand(timeGetTime());
    __super::init();

    Lazy::FontMgr::instance()->registerFontFile(_T(FONT_FILE), _T("font"));

	//在此添加初始化代码
    m_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0xffafafaf);

    m_pCamera=new CCursorCamera(m_pd3dDevice);
    m_pCamera->setup3DCamera();
    m_pCamera->setPosition(D3DXVECTOR3(0,100,-500));
    m_pCamera->setSpeed(200.0f);
    
	m_pFPS = new cFpsRender();
	m_pFPS->init();

    m_texLogin = (cResTexture*)(cResTexture::createObj());
    m_texLogin->load(LOADING_IMAGE);

    m_scrFace = new C2DFace();
    m_scrFace->init(m_pd3dDevice, 0, 0, float(m_nWidth), float(m_nHeight) );

    m_scrProgress = new ProgressBar();
    m_scrProgress->setProgress(0.0f);
    m_scrProgress->setBackRect(50.0f, m_nHeight-100.0f, m_nWidth-100.0f, 60.0f);
    m_scrProgress->setForeRect(60.0f, m_nHeight-90.0f, m_nWidth-120.0f, 40.0f);


#if 1
    startGameLoading(LoadingType::Initialize);
    Lazy::LoadingMgr::init(2, 20);

    Lazy::TaskPtr initTask = new Lazy::DefTaskObj<CGame>(this, &CGame::initLoading, &CGame::onInitLoadingFinish);
    Lazy::LoadingMgr::instance()->addTask(initTask);

#else
    initLoading();
    onInitLoadingFinish();
    if(!m_isLoadingOK)
        return false;
#endif
	return true;
}

void CGame::startGameLoading(int type)
{
    CApp::startGameLoading(type);
    m_scrProgress->setProgress(0.0f, true);
}

void CGame::stopGameLoading()
{
    CApp::stopGameLoading();
    m_scrProgress->setProgress(1.0f, true);
    getMap()->setLoadingProgress(0.0f);
}

void CGame::initLoading()
{
    m_isLoadingOK = true;

    //////////////////////////////////////////////////////////////////////////
    WRITE_LOG(_T("INFO: start initialize script..."));
    //加载脚本
    m_pyScript = new cPyScript();
    if( !m_pyScript->init() )
    {
        m_isLoadingOK = false;
        WRITE_LOG(_T("ERROR: init script failed!"));
        return;
    }
    WRITE_LOG(_T("INFO: initialize script finished."));

    m_pFog = m_pyScript->getFog();
    if (m_pFog)
        m_pFog->init(m_pd3dDevice);
}

void CGame::onInitLoadingFinish()
{
    if(!m_isLoadingOK)
    {
        quitGame();
        return;
    }

    stopGameLoading();
    WRITE_LOG(_T("INFO: initialize finished."));
}

void CGame::loadingUpdate()
{
    m_loadingElapse += m_fElapse;
    if (0 && m_loadingType == LoadingType::Initialize)
    {
    }
    else
    {
        float progress = getMap()->getLoadingProgress();
        m_scrProgress->setProgress(progress, false);
    }
    m_scrProgress->update(m_fElapse);
}

void CGame::loadingRender()
{
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pd3dDevice->SetTexture(0, m_texLogin->getTexture());

    m_scrFace->render(m_pd3dDevice);

    m_scrProgress->render(m_pd3dDevice);
}

void CGame::run()
{
    try
    {
        CApp::run();
    }
    catch(...)
    {
        if (PyErr_Occurred())
            PyErr_Print();
    }
}

//更新
void CGame::update()
{
    m_pFPS->update(m_fElapse);

    Lazy::LoadingMgr::instance()->processFinishTask();

	if(m_isLoading)
	{
        loadingUpdate();
		return;
	}

    if(!m_isLoadingOK) return;
    
    __super::update();
    //在此添加更新代码

    cMap::instance()->update(m_fElapse);
    m_pUpdateTaskMgr->update(m_fElapse);

    GUIManagerBase* pgui = getGUIMgr();
    if (pgui) pgui->update(m_fElapse);

	updateCamera(m_fElapse);
}
void CGame::updateCamera(float elapse)
{
    m_pCamera->update(elapse);

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
    if(!m_isLoadingOK) return;

	m_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,100),1.0f,0);

    HRESULT hr = m_pd3dDevice->BeginScene();
	if(FAILED(hr))
	{
        debugMessage(_T("ERROR: begin scene failed! 0x%8.8x"), hr);
        Sleep(100);
        return;
    }

    if(m_isLoading)
    {
        loadingRender();
    }
    else
    {
		//添加渲染代码
		m_pCamera->render(m_pd3dDevice);

        __super::render();

        if (m_pFog)
        {
            m_pFog->setFog();
        }

        //设置材质
        CMaterial::setMaterial(m_pd3dDevice,D3DXCOLOR(0.8f,0.8f,0.8f,1.0f));

        m_pSkyBox->render(m_pd3dDevice);

        //渲染地面
        cMap::instance()->render(m_pd3dDevice);
		m_pRenderTaskMgr->render(m_pd3dDevice);

        m_pBillboardMgr->render(m_pd3dDevice);

        if (m_pFog)
        {
            m_pFog->closeFog();
        }

        GUIManagerBase* pgui = getGUIMgr();
        if (pgui)
        {
            pgui->render(m_pd3dDevice);
        }
    }

    m_pFPS->render(m_pd3dDevice);
	/////////////////////////
	m_pd3dDevice->EndScene();
	m_pd3dDevice->Present(NULL,NULL,NULL,NULL);
}

///////////////////class CGame end/////////////////////////
